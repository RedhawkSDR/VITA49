/* ===================== COPYRIGHT NOTICE =====================
 * This file is protected by Copyright. Please refer to the COPYRIGHT file
 * distributed with this source distribution.
 *
 * This file is part of REDHAWK.
 *
 * REDHAWK is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * REDHAWK is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see http://www.gnu.org/licenses/.
 * ============================================================
 */

#include "Utilities.h"
#include "TimeStamp.h"
#include "VRTObject.h"
#include <math.h>     // for fmod(..) [do not use tgmath.h in place of this]
#include <cctype>     // for toupper(..) and tolower(..)
#include <algorithm>  // for transform(..)
#include <ctime>      // for gettimeofday(..) / clock_gettime(..)
#include <sys/time.h> // for gettimeofday(..)  [see man page]
#include <time.h>     // for clock_gettime(..) [see man page]
#include <errno.h>    // for errno

using namespace vrt;
using namespace vrt::Utilities;

#ifndef TIMER_ABSTIME
# define TIMER_ABSTIME 1 // Simulate flag where n/a
#endif

static const char hexChar[] = { '0', '1', '2', '3', '4', '5', '6', '7',
                                '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

static int32_t xtoi (string s) {
  int32_t val = 0;
  size_t  len = s.length();
  bool    ok  = (len >= 1) && (len <= 8);
  if (ok) {
    _Intel_Pragma("loop_count min=1, max=8")
    for (size_t i = 0; i < len; i++) {
      switch (s[i]) {
        case '0': val = (val << 4) | 0x0; break;   case '1': val = (val << 4) | 0x1; break;
        case '2': val = (val << 4) | 0x2; break;   case '3': val = (val << 4) | 0x3; break;
        case '4': val = (val << 4) | 0x4; break;   case '5': val = (val << 4) | 0x5; break;
        case '6': val = (val << 4) | 0x6; break;   case '7': val = (val << 4) | 0x7; break;
        case '8': val = (val << 4) | 0x8; break;   case '9': val = (val << 4) | 0x9; break;
        case 'A': case 'a': val = (val << 4) | 0xA; break;
        case 'B': case 'b': val = (val << 4) | 0xB; break;
        case 'C': case 'c': val = (val << 4) | 0xC; break;
        case 'D': case 'd': val = (val << 4) | 0xD; break;
        case 'E': case 'e': val = (val << 4) | 0xE; break;
        case 'F': case 'f': val = (val << 4) | 0xF; break;
        default:
          // Prior versions threw an exception here, however this prevented the
          // parallelizer in the Intel compiler from optimizing this loop.
          ok = false;
          break;
      }
    }
  }
  if (!ok) throw VRTException("Expected hex value but got '"+s+"'");
  return val;
}

void Utilities::getCurrentSystemTime (int64_t &sec, int64_t &ps) {
#if _POSIX_C_SOURCE >= 199309L
  struct timespec tv;
  int status = clock_gettime(CLOCK_REALTIME, &tv);
  if (status != 0) {
    throw VRTException(errno);
  }
  sec = ((int64_t)tv.tv_sec );
  ps  = ((int64_t)tv.tv_nsec) * __INT64_C(1000);
#else
  // As of POSIX.1-2008 use of gettimeofday(..) is obsolete, however it
  // is still the version used under OS X (and other BSD versions?).
  struct timeval tv;
  int status = gettimeofday(&tv, NULL);
  if (status != 0) {
    throw VRTException(errno);
  }
  sec = ((int64_t)tv.tv_sec );
  ps  = ((int64_t)tv.tv_usec) * __INT64_C(1000000);
#endif
}

int64_t Utilities::currentTimeMillis () {
  int64_t sec;
  int64_t ps;
  getCurrentSystemTime(sec, ps);
  return (sec * __INT64_C(1000)) + (ps / __INT64_C(1000000000));
}

/** Calls the applicable sleep command. flags=0 is normal (sleep for a given
 *  duration) and flags=TIMER_ABSTIME is to sleep until a given system time.
 */
static inline void _sleep (int64_t sec, int64_t nsec, int flags=0) {
  if ((sec == 0) && (nsec == 0)) return; // nothing to do

  // This is slightly complicated since usleep(..) isn't completely standard,
  // and is being replaced by nanosleep(..) additionally the clock_* calls
  // aren't supported or OS X (or other BSD versions?).
#if _POSIX_C_SOURCE >= 199309L
  struct timespec req;  // requested sleep time
  struct timespec rem;  // remaining sleep time

  req.tv_sec  = sec;
  req.tv_nsec = nsec;

  // Use CLOCK_MONOTONIC when real-time is not required since this matches the
  // default behavior on Linux for nanosleep(..).
  clockid_t clock = (flags == 0)? CLOCK_MONOTONIC : CLOCK_REALTIME;

  if (flags != 0) {
    struct timespec now;
    int status = clock_gettime(CLOCK_REALTIME, &now);
    if (status != 0) {
      // ignore
    }
    else if (now.tv_sec > req.tv_sec) {
      return; // already passed end time
    }
    else if ((now.tv_sec == req.tv_sec) && (now.tv_nsec >= req.tv_nsec)) {
      return; // already passed end time
    }
    else {
      // need to wait
    }
  }

  if (clock_nanosleep(clock, flags, &req, &rem) != 0) {
    if (errno == EINTR) {
      // ignore
    }
    else {
      throw VRTException(errno);
    }
  }
#elif _BSD_SOURCE || _XOPEN_SOURCE >= 500 || (defined(__APPLE__) && defined(__MACH__))
  if (flags != 0) {
    // Simulate the "sleep until" function by sleeping for the delta between
    // now and the specified time.
    int64_t nowSec;
    int64_t nowPS;

    getCurrentSystemTime(nowSec, nowPS);

    int64_t deltaSec  = sec  - nowSec;
    int64_t deltaNSec = nsec - (nowPS * 1000);

    if (deltaNSec < 0) {
      deltaSec  -= __INT64_C(1);
      deltaNSec += __INT64_C(1000000000);
    }
    if (deltaSec < 0) {
      return; // already passed end time
    }

    sec  = deltaSec;
    nsec = deltaNSec;
  }
# if (defined(__APPLE__) && defined(__MACH__))
  struct timespec req;  // requested sleep time
  struct timespec rem;  // remaining sleep time

  req.tv_sec  = sec;
  req.tv_nsec = nsec;

  if (nanosleep(&req, &rem) != 0) {
    if (errno == EINTR) {
      // ignore
    }
    else {
      throw VRTException(errno);
    }
  }
# else
  while (sec > 0) {
    // Some systems require the sleep time to be smaller than 1 sec so we simulate
    // this by sleeping two 0.5 second intervals for each second requested.
    if (usleep(500000) != 0) {
      throw VRTException(errno);
    }
    if (usleep(500000) != 0) {
      throw VRTException(errno);
    }
    sec--;
  }

  unsigned int usecs = (unsigned int)(nsec / 1000); // <-- cast as per usleep(3) man page
  if (usleep(usecs) != 0) {
    throw VRTException(errno);
  }
# endif
#else
# error "Could not find either nanosleep(2) or usleep(3)"
#endif
}

void Utilities::sleep (int64_t ms) {
  int64_t sec  = (ms / 1000);
  int64_t nsec = (ms % 1000) * 1000000;
  _sleep(sec, nsec, 0);
}

void Utilities::sleepUntil (int64_t ms) {
  int64_t sec  = (ms / 1000);
  int64_t nsec = (ms % 1000) * 1000000;
  _sleep(sec, nsec, TIMER_ABSTIME);
}

void Utilities::sleepUntil (TimeStamp ts) {
  int64_t sec  = ts.getPOSIXSeconds();
  int64_t nsec = ts.getPicoSeconds() / 1000;
  _sleep(sec, nsec, TIMER_ABSTIME);
}

string Utilities::format (const char *fmt, ...) {
  va_list ap;
  for (size_t len = 32; len < 4096; len*=2) {
    char str[len];
    va_start(ap, fmt);
    size_t n = vsnprintf(str, len, fmt, ap);
    va_end(ap);
    if (n < len) return string(str);
  }
  throw exception();
}

string Utilities::fromUTF8 (const wstring utf8, char replacement) {
  string out;
  string tmp = "_";
  string rep = "_";  rep[0] = replacement;

  for (size_t i = 0; i < utf8.size(); i++) {
    wchar_t c = utf8[i];
    if (c <= 0x7F) {
      tmp[0] = (char)c;
      out += tmp;
    }
    else if (!isNull(replacement)) {
      out += rep;
    }
    else {
      throw VRTException("Non-ASCII string given '%s'", fromUTF8(utf8).c_str());
    }
  }
  return out;
}

boolNull Utilities::toBoolean (const string &obj) {
  string s = toLowerCase(obj);
  if (s.size() == 0) return _NULL;
  if (s == "true"  ) return _TRUE;
  if (s == "false" ) return _FALSE;
  if (s == "t"     ) return _TRUE;
  if (s == "f"     ) return _FALSE;
  if (s == "1"     ) return _TRUE;
  if (s == "0"     ) return _FALSE;
  if (s == "yes"   ) return _TRUE;
  if (s == "no"    ) return _FALSE;
  if (s == "y"     ) return _TRUE;
  if (s == "n"     ) return _FALSE;
  if (s == "on"    ) return _TRUE;
  if (s == "off"   ) return _FALSE;
  throw VRTException("Can not convert '"+obj+"' to boolean.");
}

static string _trimNA (const string &str, bool checkNA) {
  size_t length = str.length();
  size_t start  = 0;
  size_t end    = length;
  while ((start < end) && (str[start] <= ' ')) start++;
  while ((start < end) && (str[end-1] <= ' ')) end--;

  if (!checkNA) {
    // don't check for "NA" or "N/A"
  }
  else if (end-start == 2) {
    char c0 = str[start];
    char c1 = str[start+1];
    if ((c0=='n' || c0=='N') && (c1=='a' || c1=='A')) return "";
  }
  else if (end-start == 3) {
    char c0 = str[start];
    char c1 = str[start+1];
    char c2 = str[start+2];
    if ((c0=='n' || c0=='N') && (c1 == '/') && (c2=='a' || c2=='A')) return "";
  }

  return str.substr(start, end-start);
}

string Utilities::trimNA (const string &str) {
  return _trimNA(str, true);
}

string Utilities::trim (const string &str) {
  return _trimNA(str, false);
}

string Utilities::toLowerCase (const string &str) {
  string out;
  out.resize(str.size());
  transform(str.begin(), str.end(), out.begin(), ::tolower);
  return out;
}

string Utilities::toUpperCase (const string &str) {
  string out;
  out.resize(str.size());
  transform(str.begin(), str.end(), out.begin(), ::toupper);
  return out;
}

string Utilities::toHexString (int32_t val, int32_t bytes) {
  int32_t len = bytes*2;
  char chars[8];

  for (int32_t i = len-1; i >= 0; i--) {
    chars[i] = hexChar[val&0xF];
    val = val >> 4;
  }
  return string(chars, len);
}

string Utilities::toHexString (int64_t val, int32_t bytes) {
  int32_t len = bytes*2;
  char chars[16];

  for (int32_t i = len-1; i >= 0; i--) {
    chars[i] = hexChar[val&0xF];
    val = val >> 4;
  }
  return string(chars, len);
}

string Utilities::toStringOUI (int32_t oui) {
  if (isNull(oui)) return string("");

  if ((oui & 0xFF000000) != 0) {
    throw VRTException("Invalid OUI (0x%x).", oui);
  }
  return toHexString((oui >> 16), 1) + "-"
       + toHexString((oui >>  8), 1) + "-"
       + toHexString((oui >>  0), 1);
}

int32_t Utilities::fromStringOUI (string oui) {
  if (oui.length() == 0) return INT32_NULL;

  if ((oui.length() != 8) || (oui[2] != '-') || (oui[5] != '-')) {
    throw VRTException("Invalid OUI "+oui);
  }
  return (xtoi(oui.substr(0,2)) << 16)
       | (xtoi(oui.substr(3,2)) <<  8)
       | (xtoi(oui.substr(6,2)) <<  0);
}

string Utilities::toStringClassID (int64_t id) {
  if (isNull(id)) return string("");

  int32_t oui = (int32_t)(id >> 32);
  int16_t icc = (int16_t)(id >> 16);
  int16_t pcc = (int16_t)(id);

  return toStringClassID(oui, icc, pcc);
}

int64_t Utilities::fromStringClassID (string id) {
  if (id.length() == 0) return INT64_NULL;

  size_t i = id.find(':');
  size_t j = id.find('.');

  if ((id.length() < 12) || (i != 8) || (j < 10)) {
    throw VRTException("Invalid class ID "+id);
  }
  int64_t oui = __INT64_C(0x00FFFFFF00000000) & (((int64_t)fromStringOUI(id.substr(0,8))) << 32);
  int64_t icc = __INT64_C(0x00000000FFFF0000) & (xtoi(id.substr(9,j-9)) << 16);
  int64_t pcc = __INT64_C(0x000000000000FFFF) & (xtoi(id.substr(j+1))        );
  return oui|icc|pcc;
}

string Utilities::toStringDeviceID (int64_t id) {
  if (isNull(id)) return string("");
  int32_t oui = (int32_t)(id >> 32);
  int16_t dev = (int16_t)id;

  return toStringDeviceID(oui, dev);
}

int64_t Utilities::fromStringDeviceID (string id) {
  if (id.length() == 0) return INT64_NULL;

  size_t i = id.find(':');

  if ((id.length() < 10) || (i != 8)) {
    throw VRTException(string("Invalid device ID ")+id);
  }
  int64_t oui = __INT64_C(0x00FFFFFF00000000) & (((int64_t)fromStringOUI(id.substr(0,8))) << 32);
  int64_t dev = __INT64_C(0x000000000000FFFF) & xtoi(id.substr(9));
  return oui|dev;
}

double Utilities::normalizeAngle360 (double deg) {
  // We add 360 the the input value in an effort to minimize the occurrence
  // of the deg<0 case which is computationally costly due to the extra %360
  // at the end which is necessary prevent a result of 360.0 when deg=-360.0*N
  deg += 360;
  return (deg < 0.0)? fmod((360.0-fmod(-deg, 360.0)), 360.0) : fmod(deg, 360.0);
}

double Utilities::normalizeAngle180 (double deg) {
  return Utilities::normalizeAngle360(deg + 180.0) - 180.0;
}

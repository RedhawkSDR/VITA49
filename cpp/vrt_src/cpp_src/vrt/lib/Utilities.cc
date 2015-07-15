/*
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
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#include "Utilities.h"
#include <math.h> // for fmod(..) [do not use tgmath.h in place of this]

using namespace vrt;
using namespace vrt::Utilities;

static const char hexChar[] = { '0', '1', '2', '3', '4', '5', '6', '7',
                                '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

static string toLowerCase (const string &s) {
  string str(s);
  for (size_t i = 0; i < str.size(); i++) {
    char c = str[i];
    if ((c >= 'A') && (c <= 'Z')) str[i] = c - 'A' + 'a';
  }
  return str;
}

static int32_t xtoi (string s) {
  int32_t val = 0;
  for (size_t i = 0; i < s.length(); i++) {
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
      default: throw VRTException("Expected hex value but got '"+s+"'");
    }
  }
  return val;
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

string Utilities::trimNA (const string &str) {
  string s = trim(str);
  if (s.length() == 2) {
    char c0 = s[0];
    char c1 = s[1];
    if ((c0=='n' || c0=='N') && (c1=='a' || c1=='A')) return "";
  }
  else if (s.length() == 3) {
    char c0 = s[0];
    char c1 = s[1];
    char c2 = s[2];
    if ((c0=='n' || c0=='N') && (c1 == '/') && (c2=='a' || c2=='A')) return "";
  }
  return s;
}

string Utilities::trim (const string &str) {
  int length = str.length();
  int start  = 0;
  int end    = length;
  while ((start < end) && (str[start] <= ' ')) start++;
  while ((start < end) && (str[end-1] <= ' ')) end--;
  return str.substr(start, end-start);
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

  int32_t i = id.find(':');
  int32_t j = id.find('.');

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
  
  int32_t i = id.find(':');

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

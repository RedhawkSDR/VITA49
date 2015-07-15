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

#include "TimeStamp.h"
#include <ctime>
#include <stdlib.h>     // required for atoi(..), atoll(..) on GCC4.4/libc6 2.11.1
#include <sys/time.h>

using namespace vrt;

const TimeStamp TimeStamp::NO_TIME_STAMP = TimeStamp(IntegerMode_None,FractionalMode_None,INT64_NULL,INT64_NULL);
const TimeStamp TimeStamp::Y2K_GPS       = TimeStamp::parseTime("2000-01-01T00:00:00.000Z",IntegerMode_GPS);

const int64_t MAX_GPS2UTC     = __INT64_C(0xFFFFFFFF) - TimeStamp::GPS2UTC;
const int64_t MIN_UTC2GPS     = TimeStamp::GPS2UTC;
const int32_t ONE_YEAR_MAX    = 86400*366+12; // One year, in seconds with leap seconds
const int32_t LAST_TWO_WEEKS  = 86400*351;    // Last 31 days of year (approx.)
const int32_t FIRST_TWO_WEEKS = 86400*14;     // First 31 days of year (approx.)

/** Delta between J1970 and J1950 epochs. */
static const int32_t J1970TOJ1950 = 631152000;

TimeStamp::TimeStamp (const TimeStamp &ts) :
  VRTObject(ts),
  tsiMode(ts.tsiMode),
  tsfMode(ts.tsfMode),
  tsi(ts.tsi),
  tsf(ts.tsf),
  ls(ts.ls),
  sr(ts.sr)
{
  // done
}

TimeStamp::TimeStamp () :
  tsiMode(IntegerMode_None),
  tsfMode(FractionalMode_None),
  tsi(0),
  tsf(0),
  ls(LeapSeconds::getDefaultInstance()),
  sr(DOUBLE_NAN)
{
  // done
}

TimeStamp::TimeStamp (IntegerMode epoch, uint32_t sec, uint64_t ps, double sr) :
  tsiMode(epoch),
  tsfMode(FractionalMode_RealTime),
  tsi(sec),
  tsf(ps),
  ls(LeapSeconds::getDefaultInstance()),
  sr(sr)
{
  if ((epoch != IntegerMode_UTC) && (epoch != IntegerMode_GPS)) {
    throw VRTException("Expected GPS or UTC epoch");
  }
  if (ps >= ONE_SECOND) {
    throw VRTException("Invalid number of picoseconds must be in the range [0, 1000000000000).");
  }
}

TimeStamp::TimeStamp (IntegerMode tsiMode, FractionalMode tsfMode, uint32_t tsi, uint64_t tsf, double sr) :
  tsiMode(tsiMode),
  tsfMode(tsfMode),
  tsi((tsiMode == IntegerMode_None   )? 0 : tsi),
  tsf((tsfMode == FractionalMode_None)? 0 : tsf),
  ls(LeapSeconds::getDefaultInstance()),
  sr(sr)
{
  if ((tsfMode == FractionalMode_RealTime) && (tsf >= ONE_SECOND)) {
    throw VRTException("Invalid number of picoseconds must be in the range [0, 1000000000000).");
  }
}

TimeStamp::TimeStamp (IntegerMode tsiMode, FractionalMode tsfMode, uint32_t tsi, uint64_t tsf, LeapSeconds *ls, double sr) :
  tsiMode(tsiMode),
  tsfMode(tsfMode),
  tsi((tsiMode == IntegerMode_None   )? 0 : tsi),
  tsf((tsfMode == FractionalMode_None)? 0 : tsf),
  ls(ls),
  sr(sr)
{
  if ((tsfMode == FractionalMode_RealTime) && (tsf >= ONE_SECOND)) {
    throw VRTException("Invalid number of picoseconds must be in the range [0, 1000000000000).");
  }
}

TimeStamp::TimeStamp (LeapSeconds *ls, IntegerMode tsiMode, FractionalMode tsfMode, uint32_t tsi, uint64_t tsf, double sr) :
  tsiMode(tsiMode),
  tsfMode(tsfMode),
  tsi(tsi),
  tsf(tsf),
  ls(ls),
  sr(sr)
{
  // done
}


TimeStamp TimeStamp::getSystemTime (LeapSeconds *ls) {
  struct timeval tv;

  if (gettimeofday(&tv, NULL) == 0) {
    // Must cast to int64_t below since some 32-bit systems will have a
    // smaller type for tv_usec that will result in an overflow following
    // the multiply.
    int64_t s  = ((int64_t)tv.tv_sec);                       // POSIX seconds
    int64_t ps = ((int64_t)tv.tv_usec) * __INT64_C(1000000); // picoseconds (POSIX/UTC/GPS)

    return forTimePOSIX(s, ps, ls);
  }
  else {
    throw VRTException("Unable to get current time using gettimeofday(..)");
  }
}

TimeStamp TimeStamp::toUTC () const {
  if (tsiMode == IntegerMode_UTC) return *this;
  return TimeStamp(ls, IntegerMode_UTC, tsfMode, getUTCSeconds(), tsf);
}
  
TimeStamp TimeStamp::toGPS () const {
  if (tsiMode == IntegerMode_GPS) return *this;
  return TimeStamp(ls, IntegerMode_GPS, tsfMode, getGPSSeconds(), tsf);
}

TimeStamp TimeStamp::addTime (int64_t sec, int64_t fsec, double sr, bool fract) const {
  if ((tsiMode != IntegerMode_UTC) && (tsiMode != IntegerMode_GPS)) {
    throw VRTException("Can not add seconds to non GPS/UTC time");
  }
  else if (fract && (tsfMode != FractionalMode_RealTime && (tsfMode != FractionalMode_SampleCount || sr <= 0))) {
    throw VRTException("Can not add picoseconds to non-RealTime");
    }
// Not sure why I commented this out - MSM
//  else if (fract && sr == ONE_SEC && (tsfMode != FractionalMode_RealTime)) {
//    throw VRTException("Can not add picoseconds to non-RealTime");
//    }
//  else if (fract && (tsfMode != FractionalMode_SampleCount)) {
//    throw VRTException("Can not add samples to non-SampleCount");
//    }
  else if ((sec == 0L) && (fsec == 0L)) {
    return *this; // no change
    }
  else if (fsec == 0) {
    return TimeStamp(ls, tsiMode, tsfMode, tsi+sec, tsf);
  }
  else {
    if (tsfMode != FractionalMode_SampleCount) sr = ONE_SEC;

    // STEP 1: Add in the seconds plus seconds and fsec plus fsec, taking
    //         care to take any whole seconds in fsec and move them to seconds
    int64_t s    = fsec / sr;
    int64_t _sec = tsi + (sec + s);
    int64_t _fsec  = tsf + (fsec  - (s * sr));

    // STEP 2: There is a good chance _fsec has exceeded +/- sr, so fix it
    s    = _fsec / sr;
    _sec = _sec + s;
    _fsec  = _fsec  - (s * sr);
    
    // STEP 3: If _fsec is negative, make it positive (i.e. -0.4 = -1.0 + 0.6)
    if (_fsec < 0) {
      _sec = _sec - 1;
      _fsec  = _fsec  + sr;
    }

    return TimeStamp(ls, tsiMode, tsfMode, _sec, _fsec);
  }
}

char TimeStamp::compareTo (const TimeStamp &other) const {
  // INTEGER
  if ((tsiMode == IntegerMode_None) || (other.tsiMode == IntegerMode_None)) {
    if (tsiMode != other.tsiMode) {
      throw VRTException("Can not compare time stamps with incompatible epochs");
    }
  }
  else if (tsiMode == other.tsiMode) {
    if (tsi < other.tsi) return -1;
    if (tsi > other.tsi) return +1;
  }
  else {
    int64_t a = getGPSSeconds();
    int64_t b = other.getGPSSeconds();
    if (a < b) return -1;
    if (a > b) return +1;
  }

  // FRACTIONAL
  if (tsfMode != other.tsfMode) {
    throw VRTException("Can not compare time stamps with incompatible epochs");
  }
  else {
    if (tsf < other.tsf) return -1;
    if (tsf > other.tsf) return +1;
    return 0;
  }
}

bool TimeStamp::equals (const VRTObject &o) const {
  if (typeid(TimeStamp) != typeid(o)) return false;
  const TimeStamp *ts = checked_dynamic_cast<const TimeStamp*>(&o);

  return (tsiMode == ts->tsiMode)
      && (tsfMode == ts->tsfMode)
      && (tsi     == ts->tsi    )
      && (tsf     == ts->tsf    )
      && (ls      == ts->ls     );
}

string TimeStamp::toString () const {
  ostringstream str;
  char picosecondString[32];
  snprintf(picosecondString, 32, "0.%.12" PRIu64, tsf);

  if (tsiMode == IntegerMode_None) {
    switch (tsfMode) {
      case FractionalMode_None:             str << "";                                         return str.str();
      case FractionalMode_SampleCount:      str << tsf              << " (SampleCount)";       return str.str();
      case FractionalMode_RealTime:         str << picosecondString << " (RealTime)";          return str.str();
      case FractionalMode_FreeRunningCount: str << tsf              << " (FreeRunningCount)";  return str.str();
    }
  }
  if (tsiMode == IntegerMode_UTC) {
    switch (tsfMode) {
      case FractionalMode_None:             str << ls->utcToYMDHMS(tsi, __INT64_C(-1))               << " (UTC)";                   return str.str();
      case FractionalMode_SampleCount:      str << ls->utcToYMDHMS(tsi, __INT64_C(-1)) << "," << tsf << " (UTC,SampleCount)";       return str.str();
      case FractionalMode_RealTime:         str << ls->utcToYMDHMS(tsi, tsf          )               << " (UTC)";                   return str.str();
      case FractionalMode_FreeRunningCount: str << ls->utcToYMDHMS(tsi, __INT64_C(-1)) << "," << tsf << " (UTC,FreeRunningCount)";  return str.str();
    }
  }
  if (tsiMode == IntegerMode_GPS) {
    switch (tsfMode) {
      case FractionalMode_None:             str << ls->gpsToYMDHMS(tsi, __INT64_C(-1))               << " (GPS)";                   return str.str();
      case FractionalMode_SampleCount:      str << ls->gpsToYMDHMS(tsi, __INT64_C(-1)) << "," << tsf << " (GPS,SampleCount)";       return str.str();
      case FractionalMode_RealTime:         str << ls->gpsToYMDHMS(tsi, tsf          )               << " (GPS)";                   return str.str();
      case FractionalMode_FreeRunningCount: str << ls->gpsToYMDHMS(tsi, __INT64_C(-1)) << "," << tsf << " (GPS,FreeRunningCount)";  return str.str();
    }
  }
  if (tsiMode == IntegerMode_Other) {
    switch (tsfMode) {
      case FractionalMode_None:             str << tsi                            << " (Other)";                   return str.str();
      case FractionalMode_SampleCount:      str << tsi << "," << tsf              << " (Other,SampleCount)";       return str.str();
      case FractionalMode_RealTime:         str << tsi << "," << picosecondString << " (Other,RealTime)";          return str.str();
      case FractionalMode_FreeRunningCount: str << tsi << "," << tsf              << " (Other,FreeRunningCount)";  return str.str();
    }
  }
  throw VRTException("Invalid mode combination -- this should be impossible"); // should be impossible
}

string TimeStamp::toStringUTC(string format) const {
  int64_t utc = getUTCSeconds();
  uint32_t leapSeconds = ls->getLeapSecondsUTC(utc);
  time_t time = utc - leapSeconds;
  char buffer[256];
  strftime(buffer, sizeof(buffer), format.c_str(), gmtime(&time));
  return string(buffer);
}

string TimeStamp::toStringUTC () const {
  // We can't optimize this the same way as the Java due, so use the trivial approach
  string str = toUTC().toString();
  str.resize(str.length() - 6);
  return str;
}

string TimeStamp::toStringGPS () const {
  // We can't optimize this the same way as the Java due, so use the trivial approach
  string str = toGPS().toString();
  str.resize(str.length() - 6);
  return str;
}

IntegerMode TimeStamp::getEpoch () const {
  if (tsiMode == GPS_EPOCH) {
    return GPS_EPOCH;
  }
  else if (tsiMode == UTC_EPOCH) {
    return UTC_EPOCH;
  }
  return NULL_EPOCH;
}

uint32_t TimeStamp::getSecondsUTC () const {
  if (tsiMode == IntegerMode_UTC) return tsi;
  if (tsiMode != IntegerMode_GPS) throw VRTException("Can not convert from non-GPS, non-UTC time to UTC");

  if (tsi > MAX_GPS2UTC) {
    throw VRTException("Can not convert from GPS to UTC (overflow)");
  }
  return tsi+TimeStamp::GPS2UTC;
}


uint32_t TimeStamp::getSecondsGPS () const {
  if (tsiMode == IntegerMode_GPS) return tsi;
  if (tsiMode != IntegerMode_UTC) throw VRTException("Can not convert from non-GPS, non-UTC time to UTC");
  if (tsi < MIN_UTC2GPS) {
    throw VRTException("Can not convert from UTC to GPS (underflow)");
  }
  return tsi-TimeStamp::GPS2UTC;
}

int64_t TimeStamp::_getNORADSeconds (bool leapCounted) const {
  int64_t utc       = getUTCSeconds();
  int64_t yearStart = ls->getYiS(utc);

  if (leapCounted) {
    return utc - yearStart;
  }
  else {
    int32_t utcLS       = ls->getLeapSecondsUTC(utc);
    int32_t yearStartLS = ls->getLeapSecondsUTC(yearStart);
    int32_t midYearLS   = utcLS - yearStartLS;

    return (int64_t)(utc - yearStart - midYearLS);
  }
}

int64_t TimeStamp::getPOSIXSeconds () const {
  int64_t utc  = getUTCSeconds();
  int32_t leap = ls->getLeapSecondsUTC(utc);
  
  return utc - leap;
}

double TimeStamp::getDoubleSeconds (double sr) const {
  if (tsfMode != FractionalMode_RealTime && tsfMode != FractionalMode_SampleCount) {
    throw VRTException("Can not convert non-RealTime/SampleCount time stamp to fractional seconds.");
  }
  if (tsfMode == FractionalMode_SampleCount) {
    if (!isnan(sr)) {
      return (double)getSecondsUTC() + tsf / sr;
    }
    else {
      throw VRTException("Sampling Rate undefined, can not convert SaqmpleCount time stamp to fractional seconds without sampling rate.");
    }
  }
  return (double)getSecondsUTC() + tsf / 1000000000000.0f;
}

double TimeStamp::getDoubleSeconds () const {
  if (isnan(sr) && tsfMode == FractionalMode_SampleCount) {
    throw VRTException("Cannot compute double seconds from SampleCount without sampling rate.");
  }
  return getDoubleSeconds(sr);
}

double TimeStamp::getSampleRate () const {
  return sr;
}

uint64_t TimeStamp::getPicoSeconds (double sr) const {
  if (tsfMode != FractionalMode_RealTime && tsfMode != FractionalMode_SampleCount) {
    throw VRTException("Can not convert non-RealTime/SampleCount time stamp to picoseconds.");
  }
  uint64_t picoSeconds = tsf;
  if (tsfMode == FractionalMode_SampleCount && !isnan(sr)) {
    if (!isnan(sr)) {
      picoSeconds = (uint64_t)(tsf * ONE_SEC/sr);
    }
    else {
      throw VRTException("Sampling Rate undefined, can not convert SaqmpleCount time stamp to picoseconds without sampling rate.");
    }
  }
  return picoSeconds;
}

uint64_t TimeStamp::getPicoSeconds () const {
  return getPicoSeconds(sr);
}

uint64_t TimeStamp::getSampleCount () const {
  if (tsfMode != FractionalMode_SampleCount) {
    throw VRTException("Can not convert non-SampleCount time stamp to sample count.");
  }
  return tsf;
}

TimeStamp TimeStamp::parseTime (string time, IntegerMode tsiMode, LeapSeconds *ls) {
  int32_t t  = time.find('T');
  int32_t d1 = time.find('-', 1);
  int32_t d2 = time.find('-', d1+2);
  int32_t c1 = time.find(':', t+2);
  int32_t c2 = time.find(':', c1+2);
  int32_t p  = time.find('.', c2+2);
  int32_t tz = time.find('Z', c2+2);

  if (tz < 0) tz = time.find('+', c2);
  if (tz < 0) tz = time.find('-', c2);
  if (tz < 0) tz = time.length();

  if ((t < 0) || (d1 < 0) || (d2 < 0) || (c1 < 0) || (c2 < 0)) {
    throw VRTException("Invalid time format");
  }

  int32_t year = atoi(time.substr(0,d1).c_str());
  int32_t mon  = atoi(time.substr(d1+1,d2).c_str());
  int32_t day  = atoi(time.substr(d2+1,t).c_str());
  int32_t hour = atoi(time.substr(t+1,c1).c_str());
  int32_t min  = atoi(time.substr(c1+1,c2).c_str());
  int32_t wsec; // set below
  int64_t psec; // set below
  int32_t zone = 0;

  if (p > 0) {
    string fsec = (time.substr(p+1,tz)+"000000000000").substr(0,12);
    psec = atoll(fsec.c_str());
    wsec = atoi(time.substr(c2+1,p).c_str());
  }
  else {
    psec = __INT64_C(0);
    wsec = atoi(time.substr(c2+1,tz).c_str());
  }

  string z = time.substr(tz);
  if ((z.size() > 0) && (z != "Z")) {
    int c = z.find(':');
    if (c < 0) {
      zone = 3600 * atoi(z.substr(1).c_str());
    }
    else {
      zone = 3600 * atoi(z.substr(1,c).c_str()) + 60 * atoi(z.substr(c+1).c_str());
    }
    if (z[0] == '-') zone = -zone;
  }

  return forTime(year, mon, day, hour, min, wsec, psec, zone, tsiMode, ls);
}

TimeStamp TimeStamp::forTime (int32_t year, int32_t mon, int32_t day, int32_t hour, int32_t min, int32_t wsec,
                              int64_t psec, int32_t zone, IntegerMode tsiMode, LeapSeconds *ls) {

  if (tsiMode == IntegerMode_GPS) {
    int32_t  d   = ls->ymdToGpsDay(year, mon, day); // days since 6-Jan-1980
    uint32_t tsi = (d * 86400) + (hour * 3600) + (min  * 60) + wsec + zone;
    return TimeStamp(IntegerMode_GPS, FractionalMode_RealTime, tsi, psec, ls);
  }

  if (tsiMode == IntegerMode_UTC) {
    if (wsec == 60) {
      // Since our POSIX-based math can not take in a value on the leap-second
      // boundary, we force it to use the time immediately before and then do
      // a +1 after creating the TimeStamp object. (This implementation may be
      // slightly inefficient, but since it hits so rarely it shouldn't matter.)
      return forTime(year, mon, day, hour, min, 59, psec, zone, tsiMode, ls).addSeconds(1);
    }
    int32_t  d    = LeapSeconds::ymdToPosixDay(year, mon, day); // days since 1-Jan-1970
    uint32_t tsi  = (d * 86400) + (hour * 3600) + (min  * 60) + wsec + zone;
    int32_t  leap = ls->getLeapSecondsPOSIX(tsi);
    return TimeStamp(IntegerMode_UTC, FractionalMode_RealTime, tsi+leap, psec, ls);
  }

  throw VRTException("Only GPS and UTC are supported when parsing time");
}

TimeStamp TimeStamp::forTimePOSIX (int64_t seconds, int64_t picoseconds, LeapSeconds *ls) {
  int32_t leap = ls->getLeapSecondsPOSIX(seconds);  // leap seconds to convert POSIX to UTC
  int64_t sec  = seconds + leap;                    // UTC seconds

  return TimeStamp(ls, IntegerMode_UTC, FractionalMode_RealTime, sec, picoseconds);
}

TimeStamp TimeStamp::_forTimeNORAD (int32_t seconds, int64_t picoseconds, bool leapCounted,
                                    int32_t year, LeapSeconds *ls) {
  if ((seconds < 0) || (seconds > ONE_YEAR_MAX)) {
    throw VRTException("Illegal NORAD time, number of seconds exceeds one year");
  }

  if (year < 0) {
    int64_t now   = getSystemTime().getUTCSeconds();   // approximate
    int32_t yr    = ls->getYear(now);                  // approximate
    int64_t start = ls->getStartOfYearPOSIX(yr);
    int64_t soy   = now - start;                       // sec-of-year

         if ((soy < FIRST_TWO_WEEKS) && (seconds > LAST_TWO_WEEKS )) year = yr - 1; // End of previous year
    else if ((soy > LAST_TWO_WEEKS ) && (seconds < FIRST_TWO_WEEKS)) year = yr + 1; // Start of next year
    else                                                             year = yr;     // Current year
  }
  
  if (leapCounted) {
    int64_t sec = ls->getStartOfYearUTC(year) + seconds;
    return TimeStamp(IntegerMode_UTC, FractionalMode_RealTime, sec, picoseconds);
  }
  else {
    int64_t sec = ls->getStartOfYearPOSIX(year) + seconds;
    return TimeStamp::forTimePOSIX(sec, picoseconds);
  }
}

TimeStamp TimeStamp::forTimeIRIG (int32_t seconds, int32_t minutes, int32_t hours,
                                  int32_t days, int32_t hundreths, LeapSeconds *ls) {
  if ((seconds == 60) && (hours == 23) && (minutes == 59)) {
    // Extremely rare leap second seen as 61st second on IRIG time
    return forTimeIRIG(59,59,23,days,hundreths).addSeconds(1);
  }
  if ((seconds > 59) || (minutes > 59) || (hours > 23) || (days > 366) || (hundreths > 99) ||
      (seconds <  0) || (minutes <  0) || (hours <  0) || (days <   1) || (hundreths <  0)) {
    throw VRTException("Illegal IRIG time %d::%d:%d:%d+%d/100", days, hours, minutes, seconds, hundreths);
  }
  int32_t sec = seconds + minutes*60 + hours*3600 + (days-1)*86400;
  int64_t ps  = hundreths * (ONE_SEC / 100);
  return _forTimeNORAD(sec, ps, false, -1, ls);
}

int32_t TimeStamp::getFieldCount () const {
  return 16;
}

string TimeStamp::getFieldName (int32_t id) const {
  switch (id) {
    case  0: return "String";
    case  1: return "StringUTC";
    case  2: return "StringGPS";
    case  3: return "Epoch";
    case  4: return "IntegerMode";
    case  5: return "FractionalMode";
    case  6: return "UTCSeconds";
    case  7: return "GPSSeconds";
    case  8: return "NORADSeconds";
    case  9: return "POSIXSeconds";
    case 10: return "MidasTime";
    case 11: return "MidasSeconds";
    case 12: return "PicoSeconds";
    case 13: return "FractionalSeconds";
    case 14: return "TimeStampInteger";
    case 15: return "TimeStampFractional";
    default: throw VRTException("Invalid field #%d in %s", id, getClassName().c_str());
  }
}

ValueType TimeStamp::getFieldType (int32_t id) const {
  switch (id) {
    case  0: return ValueType_String;
    case  1: return ValueType_String;
    case  2: return ValueType_String;
    case  3: return ValueType_Int8;
    case  4: return ValueType_Int8;
    case  5: return ValueType_Int8;
    case  6: return ValueType_Int64;
    case  7: return ValueType_Int64;
    case  8: return ValueType_Int64;
    case  9: return ValueType_Int64;
    case 10: return ValueType_Double;
    case 11: return ValueType_Double;
    case 12: return ValueType_Int64;
    case 13: return ValueType_Double;
    case 14: return ValueType_Int64;
    case 15: return ValueType_Int64;
    default: throw VRTException("Invalid field #%d in %s", id, getClassName().c_str());
  }
}

Value* TimeStamp::getField (int32_t id) const {
  switch (id) {
    case  0: return new Value(toString());
    case  1: return new Value(toStringUTC());
    case  2: return new Value(toStringGPS());
    case  3: return new Value(getEpoch());
    case  4: return new Value(getIntegerMode());
    case  5: return new Value(getFractionalMode());
    case  6: return new Value(getUTCSeconds());
    case  7: return new Value(getGPSSeconds());
    case  8: return new Value(getNORADSeconds());
    case  9: return new Value(getPOSIXSeconds());
    case 10: return new Value(getMidasTime());
    case 11: return new Value(getMidasSeconds());
    case 12: return new Value((int64_t)getPicoSeconds());
    case 13: return new Value(getFractionalSeconds());
    case 14: return new Value((int64_t)getTimeStampInteger());
    case 15: return new Value((int64_t)getTimeStampFractional());
    default: return new Value(); // null value
  }
}

void TimeStamp::setField (int32_t id, const Value* val) {
  throw VRTException("TimeStamp is read only");
}

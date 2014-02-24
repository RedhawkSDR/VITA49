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

#include "LeapSeconds.h"
#include "Utilities.h"
#include <fstream>
#include <iostream>
#include <cctype>
#include "string.h"         // required for strlen(..) on GCC4.4/libc6 2.11.1
#include "stdlib.h"         // required for atoi(..) on GCC4.4/libc6 2.11.1

using namespace std;
using namespace vrt;

LeapSeconds* LeapSeconds::defaultInstance;

static const size_t  PRE_1972_LENGTH    = 13;
static const size_t  FIRST_LINES_LENGTH = 39;

/** Day In Month map. Maps the day-of-year to the month that day falls in.
    <pre>
      NON-LEAP-YEARS:
        (<i>day</i> &gt;= dim[0]) && (<i>day</i> &lt; dim[1])  --&gt; JAN
        (<i>day</i> &gt;= dim[8]) && (<i>day</i> &lt; dim[9])  --&gt; SEP
      LEAP-YEARS:
        (<i>day</i> &gt;= dim[0+12]) && (<i>day</i> &lt; dim[1+12])  --&gt; JAN
        (<i>day</i> &gt;= dim[8+12]) && (<i>day</i> &lt; dim[9+12])  --&gt; SEP
    </pre>
 */
static const int32_t dim[24] = { 0,31,59,90,120,151,181,212,243,273,304,334,
                                 0,31,60,91,121,152,182,213,244,274,305,335 };

/** Indicates if the given year is a leap year. The code for this method was copied from RFC 3339.
    <pre>
      Source: Klyne, et.al. "RFC 3339 / Date and Time on the Internet: Timestamps."
              ITEF, July 2002. http://tools.ietf.org/html/rfc3339 (Retrieved 2007-06-15)
    </pre>
    @param year The year number.
    @return true if it is a leap year, false otherwise.
 */
static inline int32_t isLeapYear (int32_t year) {
  return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}

static const double PRE_1972[] = {
  // START DATE    CONSTANT  OFFSET      SCALE
  -3287.0 * 86400.0, 1.422818, 3.471552E8, 0.001296,
  -3075.0 * 86400.0, 1.372818, 3.471552E8, 0.001296,
  -2922.0 * 86400.0, 1.845858, 3.786912E8, 0.0011232,
  -2253.0 * 86400.0, 1.945858, 3.786912E8, 0.0011232,
  -2192.0 * 86400.0, 3.24013,  4.733856E8, 0.001296,
  -2101.0 * 86400.0, 3.34013,  4.733856E8, 0.001296,
  -1948.0 * 86400.0, 3.44013,  4.733856E8, 0.001296,
  -1826.0 * 86400.0, 3.54013,  4.733856E8, 0.001296,
  -1767.0 * 86400.0, 3.64013,  4.733856E8, 0.001296,
  -1645.0 * 86400.0, 3.74013,  4.733856E8, 0.001296,
  -1583.0 * 86400.0, 3.84013,  4.733856E8, 0.001296,
  -1461.0 * 86400.0, 4.31317,  5.049216E8, 0.002592,
   -700.0 * 86400.0, 4.21317,  5.049216E8, 0.002592,
};

/** The first lines that should appear in the file. This serves as a sanity check for the
 *  accuracy of the file.
 */
static const char FIRST_LINES[FIRST_LINES_LENGTH][128] = {
  // 0         1         2         3         4         5         6         7         8
  // 012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
    " 1961 JAN  1 =JD 2437300.5  TAI-UTC=   1.4228180 S + (MJD - 37300.) X 0.001296 S", //  0
    " 1961 AUG  1 =JD 2437512.5  TAI-UTC=   1.3728180 S + (MJD - 37300.) X 0.001296 S", //  1
    " 1962 JAN  1 =JD 2437665.5  TAI-UTC=   1.8458580 S + (MJD - 37665.) X 0.0011232S", //  2
    " 1963 NOV  1 =JD 2438334.5  TAI-UTC=   1.9458580 S + (MJD - 37665.) X 0.0011232S", //  3
    " 1964 JAN  1 =JD 2438395.5  TAI-UTC=   3.2401300 S + (MJD - 38761.) X 0.001296 S", //  4
    " 1964 APR  1 =JD 2438486.5  TAI-UTC=   3.3401300 S + (MJD - 38761.) X 0.001296 S", //  5
    " 1964 SEP  1 =JD 2438639.5  TAI-UTC=   3.4401300 S + (MJD - 38761.) X 0.001296 S", //  6
    " 1965 JAN  1 =JD 2438761.5  TAI-UTC=   3.5401300 S + (MJD - 38761.) X 0.001296 S", //  7
    " 1965 MAR  1 =JD 2438820.5  TAI-UTC=   3.6401300 S + (MJD - 38761.) X 0.001296 S", //  8
    " 1965 JUL  1 =JD 2438942.5  TAI-UTC=   3.7401300 S + (MJD - 38761.) X 0.001296 S", //  9
    " 1965 SEP  1 =JD 2439004.5  TAI-UTC=   3.8401300 S + (MJD - 38761.) X 0.001296 S", // 10
    " 1966 JAN  1 =JD 2439126.5  TAI-UTC=   4.3131700 S + (MJD - 39126.) X 0.002592 S", // 11
    " 1968 FEB  1 =JD 2439887.5  TAI-UTC=   4.2131700 S + (MJD - 39126.) X 0.002592 S", // 12
    " 1972 JAN  1 =JD 2441317.5  TAI-UTC=  10.0       S + (MJD - 41317.) X 0.0      S", // 13
    " 1972 JUL  1 =JD 2441499.5  TAI-UTC=  11.0       S + (MJD - 41317.) X 0.0      S", // 14
    " 1973 JAN  1 =JD 2441683.5  TAI-UTC=  12.0       S + (MJD - 41317.) X 0.0      S", // 15
    " 1974 JAN  1 =JD 2442048.5  TAI-UTC=  13.0       S + (MJD - 41317.) X 0.0      S", // 16
    " 1975 JAN  1 =JD 2442413.5  TAI-UTC=  14.0       S + (MJD - 41317.) X 0.0      S", // 17
    " 1976 JAN  1 =JD 2442778.5  TAI-UTC=  15.0       S + (MJD - 41317.) X 0.0      S", // 18
    " 1977 JAN  1 =JD 2443144.5  TAI-UTC=  16.0       S + (MJD - 41317.) X 0.0      S", // 19
    " 1978 JAN  1 =JD 2443509.5  TAI-UTC=  17.0       S + (MJD - 41317.) X 0.0      S", // 20
    " 1979 JAN  1 =JD 2443874.5  TAI-UTC=  18.0       S + (MJD - 41317.) X 0.0      S", // 21
    " 1980 JAN  1 =JD 2444239.5  TAI-UTC=  19.0       S + (MJD - 41317.) X 0.0      S", // 22
    " 1981 JUL  1 =JD 2444786.5  TAI-UTC=  20.0       S + (MJD - 41317.) X 0.0      S", // 23
    " 1982 JUL  1 =JD 2445151.5  TAI-UTC=  21.0       S + (MJD - 41317.) X 0.0      S", // 24
    " 1983 JUL  1 =JD 2445516.5  TAI-UTC=  22.0       S + (MJD - 41317.) X 0.0      S", // 25
    " 1985 JUL  1 =JD 2446247.5  TAI-UTC=  23.0       S + (MJD - 41317.) X 0.0      S", // 26
    " 1988 JAN  1 =JD 2447161.5  TAI-UTC=  24.0       S + (MJD - 41317.) X 0.0      S", // 27
    " 1990 JAN  1 =JD 2447892.5  TAI-UTC=  25.0       S + (MJD - 41317.) X 0.0      S", // 28
    " 1991 JAN  1 =JD 2448257.5  TAI-UTC=  26.0       S + (MJD - 41317.) X 0.0      S", // 29
    " 1992 JUL  1 =JD 2448804.5  TAI-UTC=  27.0       S + (MJD - 41317.) X 0.0      S", // 30
    " 1993 JUL  1 =JD 2449169.5  TAI-UTC=  28.0       S + (MJD - 41317.) X 0.0      S", // 31
    " 1994 JUL  1 =JD 2449534.5  TAI-UTC=  29.0       S + (MJD - 41317.) X 0.0      S", // 32
    " 1996 JAN  1 =JD 2450083.5  TAI-UTC=  30.0       S + (MJD - 41317.) X 0.0      S", // 33
    " 1997 JUL  1 =JD 2450630.5  TAI-UTC=  31.0       S + (MJD - 41317.) X 0.0      S", // 34
    " 1999 JAN  1 =JD 2451179.5  TAI-UTC=  32.0       S + (MJD - 41317.) X 0.0      S", // 35
    " 2006 JAN  1 =JD 2453736.5  TAI-UTC=  33.0       S + (MJD - 41317.) X 0.0      S", // 36
    " 2009 JAN  1 =JD 2454832.5  TAI-UTC=  34.0       S + (MJD - 41317.) X 0.0      S", // 37
    " 2012 JUL  1 =JD 2456109.5  TAI-UTC=  35.0       S + (MJD - 41317.) X 0.0      S", // 38
};

int32_t LeapSeconds::ymdToGpsDay (int32_t year, int32_t month, int32_t day) {
  // Identical to the POSIX version except using 11450 rather than 7793 to account for the
  // additional 3657 days between 1 Jan 1970 and 6 Jan 1980.
  return isLeapYear(year)? ((year-1950)*365 + (year+3)/4 - 11450 + (day + dim[month-1+12] - 1))
                         : ((year-1950)*365 + (year+3)/4 - 11450 + (day + dim[month-1+ 0] - 1));
}

int32_t LeapSeconds::ymdToPosixDay (int32_t year, int32_t month, int32_t day) {
  return isLeapYear(year)? ((year-1950)*365 + (year+3)/4 - 7793 + (day + dim[month-1+12] - 1))
                         : ((year-1950)*365 + (year+3)/4 - 7793 + (day + dim[month-1+ 0] - 1));
}

string YMDHMS::toString () const {
  char str[40];
  if ((int64_t)psec != -1)
    snprintf(str, 40, "%.4d-%.2d-%.2dT%.2d:%.2d:%.2d.%.12" PRIu64 "Z", year, month, day, hour, min, sec, psec);
  else
    snprintf(str, 40, "%.4d-%.2d-%.2dT%.2d:%.2d:%.2d", year, month, day, hour, min, sec);
  return string(str);
}

LeapSeconds::LeapSeconds () :
  startDatePOSIX(0),
  startDateUTC(0),
  leapSeconds(0),
  yearStartPOSIX(0),
  yearStartUTC(0)
{
  // do nothing
}


LeapSeconds::LeapSeconds(const LeapSeconds &ls) :
  startDatePOSIX(ls.startDatePOSIX),
  startDateUTC(ls.startDateUTC),
  leapSeconds(ls.leapSeconds),
  yearStartPOSIX(ls.yearStartPOSIX),
  yearStartUTC(ls.yearStartUTC)
{
  // do nothing
}


LeapSeconds::LeapSeconds (vector<int64_t> wsec, vector<int32_t> ls) :
  startDatePOSIX(ls.size()),
  startDateUTC(ls.size()),
  leapSeconds(ls),
  yearStartPOSIX(2106-1972),  // other stuff breaks in 2106, so don't go beyond there.
  yearStartUTC(2106-1972)     // other stuff breaks in 2106, so don't go beyond there.
{
  for (size_t i = 0; i < ls.size(); i++) {
    startDatePOSIX[i] = (int32_t)(wsec[i]);
    startDateUTC[i]   = (int32_t)(startDatePOSIX[i] + leapSeconds[i]);
  }
  
  int32_t year  = 1972;
  int64_t posix = 365 * 2 * __INT64_C(86400); // start in 1972
  for (size_t i = 0; i < yearStartUTC.size(); i++) {
    yearStartPOSIX[i] = posix;
    yearStartUTC[i]   = posix + getLeapSecondsPOSIX(posix);
    
    posix += (isLeapYear(year))? (366 * __INT64_C(86400))
                               : (365 * __INT64_C(86400));
    year  += 1;
  }
}

LeapSeconds* LeapSeconds::getDefaultInstance () {
  if (defaultInstance == NULL) {
    string fname = VRTConfig::getLeapSecondsFile();
    
    if (fname == "") {
      cout << "WARNING: No leap seconds file found at $VRT_LEAP_SECONDS or "
              "$VRTHOME/cpp_lib/tai-utc.dat, using built-in table." << endl;
    }
    defaultInstance = getInstance(fname);
  }
  return defaultInstance;
}

void LeapSeconds::setDefaultInstance (LeapSeconds *def) {
  defaultInstance = def;
}

LeapSeconds* LeapSeconds::getInstance (string fname) {
  size_t len = 0;
  char   lines[512][128];

  if (fname == "") {
    len = FIRST_LINES_LENGTH;
    for (size_t i = 0; i < FIRST_LINES_LENGTH; i++) {
      strncpy(lines[i], FIRST_LINES[i], 128);
    }
  }
  else {
  ifstream in(fname.c_str(), ifstream::in);
  while (in.good()) {
    in.getline(lines[len], 128);
    if (strlen(lines[len]) > 0) len++;
  }
  in.close();
  }

  if (len < FIRST_LINES_LENGTH) {
    throw VRTException("TAI to UTC mapping file (%s) appears to be out of date or invalid.", fname.c_str());
  }

  vector<int64_t> sd(len - PRE_1972_LENGTH);
  vector<int32_t> ls(len - PRE_1972_LENGTH);

  for (size_t i = 0; i < len; i++) {
    string line = lines[i];

    if ((i < FIRST_LINES_LENGTH) && (line != FIRST_LINES[i])) {
      throw VRTException("TAI to UTC mapping file (%s) appears to be out of date or invalid.", fname.c_str());
    }

    if (i >= PRE_1972_LENGTH) {
      string y = line.substr(1, 4);
      string m = line.substr(6, 3);
      string d = line.substr(10, 2);
      int32_t year = atoi(y.c_str());
      int32_t mon  = 0;
      int32_t day  = atoi(d.c_str());

      if (m == "JAN") mon = 1;  if (m == "FEB") mon = 2;  if (m == "MAR") mon = 3;
      if (m == "APR") mon = 4;  if (m == "MAY") mon = 5;  if (m == "JUN") mon = 6;
      if (m == "JUL") mon = 7;  if (m == "AUG") mon = 8;  if (m == "SEP") mon = 9;
      if (m == "OCT") mon =10;  if (m == "NOV") mon =11;  if (m == "DEC") mon =12;
      if (mon == 0) {
        throw VRTException("TAI to UTC mapping file (%s) appears to be out of date or invalid.", fname.c_str());
      }

      sd[i-PRE_1972_LENGTH] = ymdToPosixDay(year, mon, day) * 86400L; // POSIX date
      ls[i-PRE_1972_LENGTH] = atoi(line.substr(38, 2).c_str()) - UTC2TAI_LS_1970;
    }
  }
  return new LeapSeconds(sd, ls);
}

double LeapSeconds::getLeapSeconds (double wsec, double fsec) const {
  int32_t posix = (int32_t)(wsec - J1970TOJ1950);
    
  if (posix >= startDatePOSIX[0]) {
    return getLeapSecondsPOSIX(posix);
  }
  if (posix == 0) {
    return 0.0; // common special-case where time is zero
  }
    
  for (int32_t i = (PRE_1972_LENGTH*4)-4; i >= 0; i-=4) {
    if (PRE_1972[i] > wsec) continue;
    //     CONSTANT      + (wsec + fsec - OFFSET       ) * (SCALE         / SecondsPerDay)
    return PRE_1972[i+1] + (wsec + fsec - PRE_1972[i+2]) * (PRE_1972[i+3] / 86400.0      ) - 8.000082;
  }
  
  // This should only hit for dates before 1961
  throw VRTException("Can not convert from TAI to UTC, dates before 1960/01/01 are not supported.");
}

int32_t LeapSeconds::getLeapSecondsUTC (int64_t utc) const {
  if (utc > __INT64_C(0xFFFFFFFF)) throw VRTException("Input time exceeds max value.");

  if (utc == 0) { // special case (date not yet initialized, use 1970-01-01T00:00:00)
    return 0;
  }
  if (utc >= startDateUTC[startDateUTC.size()-1]) { // most common case (after last entry)
    return leapSeconds[startDateUTC.size()-1];
  }
  if (utc >= startDateUTC[0]) {
    int32_t i = Utilities::binarySearch(startDateUTC, utc);
    return (i >= 0)? leapSeconds[i] : leapSeconds[-i - 2];
  }
  throw VRTException("Input time is before 1972/01/01.");
}
  
int32_t LeapSeconds::getLeapSecondsPOSIX (int64_t posix) const {
  if (posix > __INT64_C(0xFFFFFFFF)) throw VRTException("Input time exceeds max value.");

  if (posix == 0) { // special case (date not yet initialized, use 1970-01-01T00:00:00)
    return 0;
  }
  if (posix > startDatePOSIX[startDatePOSIX.size()-1]) { // most common case (after last entry)
    return leapSeconds[startDatePOSIX.size()-1];
  }
  if (posix >= startDatePOSIX[0]) {
    int32_t i = Utilities::binarySearch(startDatePOSIX, posix);
    return (i >= 0)? leapSeconds[i] : leapSeconds[-i - 2];
  }
  throw VRTException("Input time is before 1972/01/01.");
}

bool LeapSeconds::isLeapSecond (int64_t utc) const {
  if (utc > __INT64_C(0xFFFFFFFF)) throw VRTException("Input time exceeds max value.");
  
  if ((utc == 0) || (utc >= startDateUTC[startDateUTC.size()-1])) {
    // the two most common cases, and they are both false
    return false;
  }
  if (utc >= startDateUTC[0]) {
    int32_t i = Utilities::binarySearch(startDateUTC, utc+1);
    return (i >= 0);
  }
  throw VRTException("Input time is before 1972/01/01.");
}

int32_t LeapSeconds::getYear (int64_t utc) const {
  if (utc == 0) return 1970;  // special case (date not yet initialized, use 1970-01-01T00:00:00)
  if (utc < yearStartUTC[0]      ) throw VRTException("Year look-up with leap seconds not valid before 1972");
  if (utc > __INT64_C(0xFFFFFFFF)) throw VRTException("Input time exceeds max value.");

  int32_t i = Utilities::binarySearch(yearStartUTC, utc);
  return (i >= 0)? 1972+i : 1972+(-i)-2;
}

int32_t LeapSeconds::getMonth (int64_t utc) const {
  if (utc == 0) return 1; // special case (date not yet initialized, use 1970-01-01T00:00:00)

  int32_t year = getYear(utc);

  for (int32_t mon = 2; mon <= 12; mon++) {
    if (utc < getStartOfMonth(year,mon)) return mon-1;
  }
  return 12;
}
  
int64_t LeapSeconds::getStartOfMonth (int32_t year, int32_t mon) const {
  int64_t monthStartPOSIX = ymdToPosixDay(year, mon, 1)*86400L;
  int64_t monthStartUTC   = monthStartPOSIX + getLeapSecondsPOSIX(monthStartPOSIX);
  return monthStartUTC;
}
  
int64_t LeapSeconds::getStartOfYearUTC (int32_t year) const {
  if (year < 1972) throw VRTException("Year look-up with leap seconds not valid before 1972");
  if (year > 2106) throw VRTException("Year look-up with leap seconds not valid after 2106");
  return yearStartUTC[year - 1972];
}
  
int64_t LeapSeconds::getStartOfYearPOSIX (int32_t year) const {
  if (year < 1972) throw VRTException("Year look-up with leap seconds not valid before 1972");
  if (year > 2106) throw VRTException("Year look-up with leap seconds not valid after 2106");
  return yearStartPOSIX[year - 1972];
}

YMDHMS LeapSeconds::utcToYMDHMS (int64_t seconds, int64_t picoseconds) const {
  YMDHMS ymdhms;
  int32_t sec;

  if (isLeapSecond(seconds)) {
    ymdhms.year  = getYear(seconds);
    ymdhms.month = getMonth(seconds);      sec = (int32_t)(seconds - getStartOfMonth(ymdhms.year, ymdhms.month));
    ymdhms.day   = ((sec-1) / 86400) + 1;
    ymdhms.hour  = 23;
    ymdhms.min   = 59;
    ymdhms.sec   = 60;
    ymdhms.psec  = picoseconds;
  }
  else {
    ymdhms.year  = getYear(seconds);
    ymdhms.month = getMonth(seconds);      sec = (int32_t)(seconds - getStartOfMonth(ymdhms.year, ymdhms.month));
    ymdhms.day   = (sec / 86400) + 1;      sec = sec - (ymdhms.day-1) * 86400;
    ymdhms.hour  = sec / 3600;             sec = sec - ymdhms.hour    * 3600;
    ymdhms.min   = sec / 60;               sec = sec - ymdhms.min     * 60;
    ymdhms.sec   = sec;
    ymdhms.psec  = picoseconds;
  }
  return ymdhms;
}

YMDHMS LeapSeconds::gpsToYMDHMS (int64_t seconds, int64_t picoseconds) {
  // JC4: This method doesn't really belong here, I just didn't know where to put it.
  YMDHMS ymdhms;

  int32_t days = (int32_t)(seconds / 86400);
  int32_t sec  = (int32_t)(seconds % 86400);
  int32_t year = (int32_t)((days+723199.5)/365.25 + .002);
  int32_t doy  = days - ymdToGpsDay(year, 1, 1);
  int32_t mon  = (isLeapYear(year))? 23: 11;
  while (dim[mon] > doy) mon--;

  ymdhms.year  = year;
  ymdhms.month = (mon < 12)? mon+1: mon-11;
  ymdhms.day   = doy - dim[mon] + 1;
  ymdhms.hour  = sec / 3600;                       sec = sec - ymdhms.hour * 3600;
  ymdhms.min   = sec / 60;                         sec = sec - ymdhms.min  * 60;
  ymdhms.sec   = sec;
  ymdhms.psec  = picoseconds;

  return ymdhms;
}

bool LeapSeconds::equals (const VRTObject &o) const {
  if (getClass() != o.getClass()) return false;
  
  LeapSeconds ls = *checked_dynamic_cast<const LeapSeconds*>(&o);
  return (startDatePOSIX == ls.startDatePOSIX)
      && (startDateUTC   == ls.startDateUTC  )
      && (leapSeconds    == ls.leapSeconds   )
      && (yearStartPOSIX == ls.yearStartPOSIX)
      && (yearStartUTC   == ls.yearStartUTC  );
}

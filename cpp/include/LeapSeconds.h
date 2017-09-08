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

#ifndef _LeapSeconds_h
#define _LeapSeconds_h

#include <vector>
#include "VRTObject.h"
#include "VRTConfig.h"
#include "Utilities.h"

using namespace std;
namespace vrt {
  /** Class to deal with leap seconds.
   *  @see vrt::TimeStamp
   */
  class LeapSeconds : public VRTObject {
    /** @deprecated This constant has moved to <tt>VRTIO</tt> in the accompanying Midas option tree. */
    public: __attribute__((deprecated)) __intelattr__((deprecated))
            static const int32_t     J1970TOJ1950       = 631152000; // (7305 days) * (86,400 sec/day)

    /** @deprecated This constant has moved to the {@link vrt::TimeStamp} class. */
    public: __attribute__((deprecated)) __intelattr__((deprecated))
            static const int32_t     GPS2UTC            = 315964811; // (3657 days) * (86,400 sec/day) + (~11 leap sec)

    /** <b>Internal use only:</b> Number of leap seconds between UTC and TAI on 1 JAN 1970, rounded
     *  to the nearest second. See {@link vrt::TimeStamp} for details.
     */
    public: static const int32_t     UTC2TAI_LS_1970    =         8; // ~= 8.000082

    /** <b>Internal use only:</b>  Number of leap seconds between GPS and TAI on 6 JAN 1980.
     *  See {@link vrt::TimeStamp} for details.
     */
    public: static const int32_t     GPS2TAI_LS_1980    =        19;

    private: static LeapSeconds *defaultInstance;

    private: vector<int64_t>  startDatePOSIX;
    private: vector<int64_t>  startDateUTC;
    private: vector<int32_t>  leapSeconds;
    private: vector<int64_t>  yearStartPOSIX;
    private: vector<int64_t>  yearStartUTC;

    /** Internal use only */
    private: LeapSeconds ();

    /** Creates a new instance. */
    private: LeapSeconds (vector<int64_t> wsec, vector<int32_t> ls);

    /** Basic destructor. */
    public: ~LeapSeconds () { }

    /** Creates a new instance. */
    public: LeapSeconds (const LeapSeconds &ls);

    public: virtual bool isNullValue () const {
      return (startDatePOSIX.size() == 0);
    }

    /** Gets the default instance. This will return the instance previously set with
     *  <tt>setDefaultInstance(..)</tt>; if no default instance has yet been set, the
     *  {@link VRTConfig#getLeapSecondsFile()} will be used as the default.
     *  @return An using the give file.
     */
    public: static LeapSeconds* getDefaultInstance ();

    /** Sets the default instance to use.
     *  @param def The default instance to use.
     */
    public: static void setDefaultInstance (LeapSeconds *def);

    /** Sets the default instance to use.
     *  @param fname Name of the file to read from.
     *  @throws VRTException if there is an error while reading and parsing the file.
     */
    public: static inline void setDefaultInstance (string fname) {
      setDefaultInstance(getInstance(fname));
    }

    /** Gets an instance with the given tai-utc.dat file. On the first call to this method the
     *  file will be opened and read. Subsequent calls may either re-open the file or return the an
     *  existing open copy.
     *  @param fname Name of the file to read from.
     *  @return An using the give file.
     */
    public: static LeapSeconds* getInstance (string fname) __attribute__((warn_unused_result));

    /** Calculate and return the difference between TAI (International Atomic Time) and UTC
     *  (Universal Coordinated Time) for the given time. Generally, this will be the number
     *  of leap seconds in effect at that time. Between 1961/01/01 and 1972/01/01 this was a
     *  sliding value with fractional offset; since 1972/01/01 this is an integral number of
     *  seconds. <br>
     *  <br>
     *  The functionality of this method matches the <tt>m_get_leap_seconds(..)</tt> method
     *  in X-Midas's <tt>leap_seconds.cc</tt> class, but uses an optimized algorithm for the
     *  most common cases.
     *  @param wsec The number of whole seconds in Midas time referenced to UTC.
     *  @param fsec The number of fractional seconds (ignored after 1972/01/01).
     *  @return The difference between TAI and UTC (i.e. TAI - UTC) in seconds.
     *  @throws VRTException If the date is before 1961/01/01.
     */
    public: double getLeapSeconds (double wsec, double fsec) const;

    /** Gets the number of leap seconds elapsed prior to the given time.
     *  @param utc 1-second time tics since 1970/01/01.
     *  @return Leap seconds elapsed during prior to the given time.
     *  @throws VRTException if the input time is before 1972/01/01. (Between 1970 and 1972,
     *                                   the number of leap seconds is not-integral.)
     */
    public: int32_t getLeapSecondsUTC (int64_t utc) const;

    /** Gets the number of leap seconds elapsed prior to the given time.
     *  @param posix Number of days elapsed since 1970/01/01 multiplied by 86400, plus the number of
     *               seconds in the current day.
     *  @return Leap seconds elapsed during prior to the given time.
     *  @throws VRTException if the input time is before 1972/01/01. (Between 1970 and 1972,
     *                                   the number of leap seconds is non-integral.)
     */
    public: int32_t getLeapSecondsPOSIX (int64_t posix) const;

    /** <b>Internal use only:</b> Is the identified UTC time equal to an inserted leap second?
     *  That is, is the time equal to the 86401th second of the day (23:59:60)?
     *  @param utc 1-second time tics since 1970/01/01.
     *  @return true if the time is 23:59:60, false otherwise.
     *  @throws VRTException if the input time is before 1972/01/01.
     */
    public: bool isLeapSecond (int64_t utc) const;

    using VRTObject::equals;
    public: virtual bool equals (const VRTObject &o) const;

    /** <b>Internal use only:</b> Converts UTC time to a year number (1970..N). */
    public: int32_t getYear (int64_t utc) const;

    /** <b>Internal use only:</b> Gets month number based on UTC year (1970..N)
     *  and UTC time (from 1970). In this form 'year' is technically redundant
     *  (since it can be computed from 'utc'), but it is here to skip that
     *  computation.
     *  */
    public: int32_t getMonth (int32_t year, int64_t utc) const;

    /** <b>Internal use only:</b> Gets UTC start-of-month based on year (1970..N) and month (1..12). */
    public: int64_t getStartOfMonth (int32_t year, int32_t mon) const;

    /** <b>Internal use only:</b> Gets the start of the UTC year in seconds-since-1970. */
    public: int64_t getStartOfYearUTC (int32_t year) const;

    /** <b>Internal use only:</b> Gets the start of the POSIX year in seconds-since-1970. */
    public: int64_t getStartOfYearPOSIX (int32_t year) const;

    /** <b>Internal use only:</b> Converts UTC time to the UTC time at 1 Jan of the given year. */
    public: inline int64_t getYiS (int64_t utc) const {
      return getStartOfYearUTC(getYear(utc));
    }

    /** <b>Internal Use Only:</b> Calculate the number of days (sonce 6-Jan-1980)
     *  for the supplied date.
     *  @param year  The year (e.g. 2002).
     *  @param month The month (1 = Jan)
     *  @param day   The day.
     *  @return The Modified Julian Day.
     */
    public: static int32_t ymdToGpsDay (int32_t year, int32_t month, int32_t day);

    /** <b>Internal Use Only:</b> Calculate the number of days (since 1-Jan-1970)
     *  for the supplied date.
     *  @param year  The year (e.g. 2002).
     *  @param month The month (1 = Jan)
     *  @param day   The day.
     *  @return The Modified Julian Day.
     */
    public: static int32_t ymdToPosixDay (int32_t year, int32_t month, int32_t day);

    /** <b>Internal Use Only:</b> Converts a UTC time to Year-Month-Day Hour:Min:Sec.
     *  @param seconds     Whole seconds.
     *  @param picoseconds Picoseconds (-1 to ignore).
     *  @return Year-Month-Day Hour:Min:Sec.
     */
    public: string toStringUTC (int64_t seconds, int64_t picoseconds=__INT64_C(-1)) const;


    /** <b>Internal Use Only:</b> Converts a GPS time to Year-Month-Day Hour:Min:Sec.
     *  @param seconds     Whole seconds.
     *  @param picoseconds Picoseconds (-1 to ignore).
     *  @return Year-Month-Day Hour:Min:Sec.
     */
    public: static string toStringGPS (int64_t seconds, int64_t picoseconds=__INT64_C(-1));


    /** Writes a set of picoseconds to a string in the form ".00000000000". */
    private: static char* writePicoseconds (char *str, int32_t off, int64_t psec);

    /** <b>Internal Use Only:</b> Converts a set of picoseconds to a string in
     *  the form "0.00000000000".
     */
    public: static string toPicosecondString (int64_t psec);

    using VRTObject::toString;
    
    /** Converts date/time to string. (psec=-1L to ignore) */
    private: static string toString (int32_t year, int32_t month, int32_t day,
                                     int32_t hour, int32_t min, int32_t sec, int64_t psec);

    /** <b>Internal Use Only:</b> Indicates if the given year is a leap year.
     *  The code for this method was copied from RFC 3339.
     *  <pre>
     *    Source: Klyne, et.al. "RFC 3339 / Date and Time on the Internet: Timestamps."
     *            ITEF, July 2002. http://tools.ietf.org/html/rfc3339 (Retrieved 2007-06-15)
     *  </pre>
     *  @param year The year number.
     *  @return true if it is a leap year, false otherwise.
     */
    public: static inline int32_t isLeapYear (int32_t year) {
      return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
    }
  };
} END_NAMESPACE
#endif /* _LeapSeconds_h */

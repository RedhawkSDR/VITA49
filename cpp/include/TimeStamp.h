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

#ifndef _TimeStamp_h
#define _TimeStamp_h

#include "VRTObject.h"
#include "LeapSeconds.h"
#include "Utilities.h"
#include "VRTMath.h"
#include "HasFields.h"
#include <cctype>
#include <iostream>
#include <sstream>

namespace vrt {
  /** Identifies the time stamp mode used for integeral seconds. */
  enum IntegerMode {
    /** No integral time code. */    IntegerMode_None  = 0,
    /** UTC time code.         */    IntegerMode_UTC   = 1,
    /** GPS time code.         */    IntegerMode_GPS   = 2,
    /** Other time code.       */    IntegerMode_Other = 3
  };

  /** Identifies the time stamp mode used for fractional seconds. */
  enum FractionalMode {
    /** No fractional time code.                    */    FractionalMode_None             = 0,
    /** Sample count.                               */    FractionalMode_SampleCount      = 1,
    /** Real-Time time stamp (picosecond accuracy). */    FractionalMode_RealTime         = 2,
    /** Free-running count.                         */    FractionalMode_FreeRunningCount = 3
  };

  /** An immutable time stamp. <br>
   *  <br>
   *  For time values (i.e. the integer time mode is not <tt>None</tt> or <tt>Other</tt>), there are
   *  four different "epochs" that are supported by this class (see below); however all time values
   *  stored within this class will be in UTC or GPS time.<br>
   *  <br>
   *  <h2>UTC <i>(per VITA-49 specification)</i></h2>
   *  This measures number of 1-second clock tics since 1 JAN 1970. Note that the VITA-49
   *  specification is extremely clear on the point that <b>this includes leap seconds</b>. <br>
   *  <br>
   *  <b>This class makes the assumption that only times after 1 JAN 1972 (or zero) are used. This is
   *  important since prior to 1 JAN 1972 the duration of 1 UTC second differed from the definition
   *  used today</b> (since 1 JAN 1972 the duration of 1 UTC second matches the SI, TAI, and GPS
   *  definitions). <b>It is also assumed that exactly 11 leap seconds took place between 1 JAN 1970
   *  and 6 JAN 1980.</b> This "11 leap second" assumption is consistent with the example provided
   *  in IEEE 1588-2008 (see section B.2 of [2]) that shows an 8 leap-second difference between
   *  UTC and TAI on 1 Jan 1970 and the officially-accepted 19 leap-second difference between UTC
   *  and TAI on 1 Jan 1980 (see [4]).
   *  <pre>
   *    1:  LS  = (Leap Seconds Added Between 1 Jan 1970 to 6 Jan 1980)
   *    2:  LS  = (Leap Seconds 1 Jan 1970 to 1 Jan 1980) + (Leap Seconds 1 Jan 1980 to 6 Jan 1980)
   *    3:  LS  = (Leap Seconds 1 Jan 1970 to 1 Jan 1980) +               0
   *    3:  LS  = (Leap Seconds 1 Jan 1970 to 1 Jan 1980)
   *    4:  LS  = (Total Leap Seconds on 1 Jan 1980) - (Total Leap Seconds on 1 Jan 1970)
   *    5:  LS  =                 19                 - (Total Leap Seconds on 1 Jan 1970)
   *    6:  LS  =                 19                 -                  8
   *    7:  LS  =                                    11
   *
   *   3 - by definition no leap seconds are added mid-month
   *   4 - basic expansion
   *   5 - per USNO, see [4]
   *   6 - per IEEE 1588-2008, see [2]
   *  </pre>
   *  Although UTC time includes leap seconds. The number of leap seconds elapsed must still be
   *  looked up every time a conversion is made to another time format and/or a time string
   *  (year-month-day).<br>
   *  <br>
   *  <h2>GPS <i>(per VITA-49 specification)</i></h2>
   *  This measures number of 1-second clock tics since 6 JAN 1980. In practice, this is usually
   *  the most efficient form to use as there is never a need to worry about leap second adjustments.<br>
   *  <br>
   *  <h2>POSIX <i>(in seconds since start of epoch)</i></h2>
   *  The POSIX time is extremely common on most computers, but it is extremely ill-defined. Many
   *  sources say that POSIX time is equal to UTC time, but (especially when represented in
   *  seconds-since-epoch form) that is simply not true. The true POSIX epoch is equal to:
   *  <pre>
   *    POSIX Seconds    =    (days since 1 Jan 1970) * (86,400 sec/day) + (seconds of current day)
   *  </pre>
   *  Note that the above equation differs from UTC time in that, UTC time is not fixed at 86,400
   *  sec/day, but rather can be 86,399 sec/day (subtract a leap second), 86,400 sec/day (normal), or
   *  86,401 sec/day (add a leap second). This implicitly yields:
   *  <pre>
   *    POSIX Seconds    =    (days since 1 Jan 1970) * (86,400 sec/day) + (seconds of current day)
   *                     =    (UTC seconds since 1 JAN 1970) - (UTC leap-seconds since 1 JAN 1970)
   *  </pre>
   *  When a POSIX time it converted to a string (i.e. year-month-day hours:min:sec) it will match
   *  the string for a UTC time ~99.9996% of the time -- this is why people say it is equal to UTC.
   *  Anyone who cares about precision time should be immediately concerned about the ~0.0004% of
   *  the time where it is just plain wrong (~0.0004% matches the 12 times of the year where a
   *  leap-second can be added/removed).<br>
   *  <br>
   *  The most problematic thing with POSIX time is that it contains no information about the number
   *  of leap-seconds that it didn't count. This means that it is impossible to determine the correct
   *  time when at the point of a leap-second insertion and means that basic math on the times won't
   *  suffice to determine the number UTC seconds elapsed between two POSIX time stamps. <br>
   *  <br>
   *  <b>There is a discontinuity in the POSIX time epoch at the point where a leap-second is
   *  added/removed. This class makes no guarantees with regards to the behavior at the point of
   *  the discontinuity and may report the time immediately before or immediately after the
   *  discontinuity (plus any fractional seconds).</b><br>
   *  <br>
   *  <h2>Midas <i>(in seconds since start of epoch)</i></h2><br>
   *  <br>
   *  Midas time is effectively identical to POSIX time but with an epoch starting at 1 Jan 1950,
   *  and with fractional seconds represented as a fraction rather than the number of picoseconds.
   *  <pre>
   *    Midas Seconds    =    (days since 1 Jan 1950) * (86,400 sec/day) + (seconds of current day)
   *                     =    (POSIX Seconds) + (seconds between 1 Jan 1950 and 1 Jan 1970)
   *                     =    (POSIX Seconds) + {@link #MIDAS2POSIX}
   *  </pre>
   *  All conversions to/from Midas time involve an intermediate conversion to/from POSIX time and
   *  are subject to the same issues with respect to leap-seconds.<br>
   *  <br>
   *  The conversion to/from fractional seconds as used in Midas time and picoseconds is simply done
   *  using the following equation:
   *  <pre>
   *    Fractional Sec   =    (number of picoseconds) / (picoseconds in one second)
   *                     =    (number of picoseconds) / {@link #ONE_SEC}
   *  </pre>
   *  <h2>NORAD/IRIG <i>(in seconds since start of current year)</i></h2>
   *  NORAD time is a fairly common one to use, particularly for aerospace-based applications. NORAD
   *  time measures the number of seconds since the start of the current UTC year. IRIG time measures
   *  day-of-year and seconds-of-day (which effectively equates to seconds-of-year after multiplying
   *  day-of-year by 86,400 sec/day).
   *  <pre>
   *     +------------------------+-----------+-------------+
   *     |      TIME-OF-YEAR      |  CONTROL  | TIME-OF-DAY |
   *     | SEC | MIN | HRS | DAYS | FUNCTIONS |  (seconds)  |
   *     +-----+-----+-----+------+-----------+-------------+
   *     |     |     |     |      |           |             |
   *     +-----+-----+-----+------+-----------+-------------+
   *                    IRIG Format B (see [1])
   *
   *      +------------------------------------+-----------+
   *      |              TIME-OF-YEAR          |  CONTROL  |
   *      | SEC | MIN | HRS | DAYS | 1/100 SEC | FUNCTIONS |
   *      +-----+-----+-----+------+-----------+-----------+
   *      |     |     |     |      |           |           |
   *      +-----+-----+-----+------+-----------+-----------+
   *                    IRIG Format G (see [1])
   *  </pre>
   *  Unfortunately the NORAD definition is some-what ambiguous as to how mid-year leap-seconds are
   *  handled, while IRIG is ambiguous in application. This leads to two possible definitions for
   *  NORAD (and IRIG-based) time:
   *  <pre>
   *    NORAD Seconds    =    (UTC seconds since start of current year)
   *
   *                  -- OR --
   *
   *    NORAD Seconds    =    (POSIX seconds since start of current year)
   *                     =    ((day of year - 1) * (86,400 sec/day)) + (seconds of current day)           [= IRIG B]
   *                     =    ((day of year - 1) * (86,400 sec/day)) + ((hr * 3600) + (min * 60) + sec)   [= IRIG B/G]
   *                     =    (UTC seconds since start of current year) - (mid-year leap-seconds)
   *  </pre>
   *  IRIG time, as reported, follows the second definition (though it is unclear how the 61st
   *  second is to be reported during a leap-second addition, see section 2.6 of [1]). The issue
   *  with IRIG time is that devices reading the IRIG time will be able to detect the leap second
   *  but will not be able to discern that a mid-year leap-second has taken place by exclusively
   *  looking at the IRIG time reported subsequent to the leap-second addition.<br>
   *  <br>
   *  As of current (Jan 2012), it has been 14+ years since the last mid-year leap second, so there
   *  is little reason to believe that past experience will be a good behavior of future behavior
   *  (14 years being longer than the life-cycle of most equipment.) It fact, it is well-known that
   *  a number of recently-produced devices that read IRIG time and produce a NORAD/UTC time-stamp
   *  differ with regards to their implementation. Accordingly, the NORAD-/IRIG-centric functions
   *  permit the specification of which form to use, and also allow a default to be set at run-time
   *  (see {@link VRTConfig}).<br>
   *  <br>
   *  One of the inherent issues with using NORAD/IRIG time is that it is necessary to separately
   *  provide an indication as to the relevant year. The functions in this class that support a NORAD
   *  to UTC conversion allow the the year to be specified. If the year is given as -1, the following
   *  algorithm is used to derive the year from the local system clock:
   *  <ul>
   *    <li>The current year is taken from the local system clock, which is presumed to be
   *        approximate enough to estimate the current year within a couple days</li>
   *    <li>If a late-December NORAD time is seen in early-January, the previous year is used</li>
   *    <li>If an early-January NORAD time is seen in late-December, the subsequent year is used</li>
   *    <li>Otherwise the current year is used</li>
   *  </ul>
   *  The above algorithm gives a few days leeway to account for cases where data that is
   *  time-stamped at the end of one year (e.g. 31 DEC) does not get processed until the
   *  beginning of the next year (e.g. 1 JAN). It also gives allowances for the local system
   *  to be approximate-at-best rather than being locked to a stable time source; and for
   *  the local system time to be using POSIX time where an end-of-year leap second gives
   *  rise to some ambiguity (see above). <br>
   *  <br>
   *  <b>If the second definition is used, there will be a discontinuity in the NORAD time
   *  epoch at the point where a leap-second is added/removed. This class makes no guarantees
   *  with regards to the behavior at the point of the discontinuity and may report the time
   *  immediately before or immediately after the discontinuity (plus any fractional seconds).</b><br>
   *  <br>
   *  <h2>Precision Time Protocol (PTP)</h2><br>
   *  <br>
   *  PTP is an IEEE standard (see [2]) for clock synchronization among computing devices for
   *  situations where "precision" time is required but where the (direct or indirect) cost
   *  of a GPS receiver is prohibitive.<br>
   *  <br>
   *  PTP provides seconds and nanoseconds referenced to 1 Jan 1970 TAI which differs from
   *  1 Jan 1970 UTC by 8 seconds (see section B.2 of [2]). PTP counts TAI seconds (which are
   *  identical to GPS seconds) and has a fixed reference to GPS time (see section 5.3.3 and
   *  Table B.1 of [2]):
   *  <pre>
   *    GPS Seconds = PTP Seconds - 315,964,819 seconds
   *  </pre>
   *  <br>
   *  <i>Only a basic conversion from PTP is provided. Although it would be viable to
   *  include a PTP reader that connects to the applicable sockets, it is currently
   *  outside the scope of this library.</i><br>
   *  <br>
   *  <h2>HasFields</h2>
   *  The following implementation is used for the {@link HasFields} methods, but
   *  note that since the TimeStamp is immutable they are read-only:
   *  <pre>
   *     ID | Name                 | Type
   *    ----+----------------------+---------------
   *      0 | String               | String
   *      1 | StringUTC            | String
   *      2 | StringGPS            | String
   *      3 | Epoch                | Int8
   *      4 | IntegerMode          | Int8
   *      5 | FractionalMode       | Int8
   *      6 | UTCSeconds           | Int64
   *      7 | GPSSeconds           | Int64
   *      8 | NORADSeconds         | Int64
   *      9 | POSIXSeconds         | Int64
   *     10 | MidasTime            | Double
   *     11 | MidasSeconds         | Double
   *     12 | PicoSeconds          | Int64
   *     13 | FractionalSeconds    | Double
   *     14 | TimeStampInteger     | Int64
   *     15 | TimeStampFractional  | Int64
   *    ----+----------------------+---------------
   *  </pre>
   *  <br>
   *  <h2>References</h2>
   *  <pre>
   *     [1] Timing Committee / Range Commanders Council / Telecommunications and Timing
   *         Group. "IRIG Serial Time Code Formats." IRIG Standard 200-98. Secretariat,
   *         Range Commanders Council, U.S. Army White Sands Missile Range Pub. May 1998.
   *
   *     [2] IEEE Instrumentation and Measurement Society. "IEEE Standard for a Precision
   *         Clock Synchronization Protocol for Networked Measurement and Control Systems."
   *         IEEE Std 1588-2008. The Institute of Electrical and Electronics Engineers,
   *         Inc. 24 July 2008.
   *  
   *     [3] USNO. "Leap Seconds." Time Service Dept., U.S. Naval Observatory.
   *         http://tycho.usno.navy.mil/leapsec.html Last Accessed 15 September 2011.
   *         <i>(Note: The companion to this is [4].)</i>
   *
   *     [4] USNO. "tai-utc.dat" Time Service Dept., U.S. Naval Observatory.
   *         http://maia.usno.navy.mil/ser7/tai-utc.dat or
   *         ftp://maia.usno.navy.mil/ser7/tai-utc.dat Last Accessed 19 January 2012.
   *         <i>(Note: This is the same file used by the {@link LeapSeconds} class and
   *         is alternately titled "tai_utc_data.txt" by some systems. It is also
   *         described in Appendix D of [5].)</i>
   *
   *     [5] Klyne, G., C. Newman. "Date and Time on the Internet: Timestamps." Network
   *         Working Group. RFC3339. July 2002.
   *  </pre>
   *  @author         
   */
  class TimeStamp : public VRTObject, public HasFields {
    /** The value of 1 second in picoseconds. */
    public: static const int64_t   ONE_SEC    = __INT64_C(1000000000000);

    /** The value of 1 second in picoseconds. */
    public: static const uint64_t  ONE_SECOND = __INT64_C(1000000000000);

    /** The UTC epoch. */
    public: static const IntegerMode UTC_EPOCH = IntegerMode_UTC;

    /** The GPS epoch. */
    public: static const IntegerMode GPS_EPOCH = IntegerMode_GPS;

    /** The "null" epoch. */
    public: static const IntegerMode NULL_EPOCH = IntegerMode_None;
    
    /** An empty time stamp. This is equal to:
     *  <pre>
     *    NO_TIME_STAMP = TimeStamp(IntegerMode.None, FractionalMode.None, null, null)
     *  </pre>
     */
    public: static const TimeStamp NO_TIME_STAMP;

    /** This is equal to the start of the year 2000 in the GPS epoch and provides a reasonable
     *  minimum bounds for most time values.
     */
    public: static const TimeStamp Y2K_GPS;

    /** Delta between GPS and UTC epochs.
     *  <pre>
     *    GPS2UTC = (days 1 Jan 1970 to 6 Jan 1980) * (86,400 sec/day) + (leap seconds)
     *    GPS2UTC = (3,657 days) * (86,400 sec/day) + (11 seconds)
     *    GPS2UTC = 315,964,811 seconds
     *  </pre>
     *  See top of class for a detailed discussion regarding number of leap seconds added
     *  between 1 Jan 1970 and 6 Jan 1980.
     */
    public: static const int32_t GPS2UTC = 315964811;

    /** Delta between Midas and POSIX epochs.
     *  <pre>
     *    MIDAS2POSIX = (days 1 Jan 1950 to 1 Jan 1970) * (86,400 sec/day)
     *    MIDAS2POSIX = 631,152,000 seconds
     *  </pre>
     */
    public: static const int32_t MIDAS2POSIX = 631152000;

    /** Delta between GPS and PTP epochs.
     *  <pre>
     *    GPS2PTP = 315,964,819 seconds
     *  </pre>
     */
    public: static const int32_t GPS2PTP = 315964819;

    private: IntegerMode    tsiMode;
    private: FractionalMode tsfMode;
    private: uint32_t       tsi;
    private: uint64_t       tsf;
    private: LeapSeconds   *ls;
    private: double         sr;

    /** Fast internal constructor. */
    private: TimeStamp (LeapSeconds *ls, IntegerMode tsiMode, FractionalMode tsfMode, uint32_t sec, uint64_t ps, double sr=DOUBLE_NAN);

    /** Basic no-argument constructor. Just creates a TimeStamp with integer and fractional modes
     *  set to <tt>None</tt>.
     */
    public: TimeStamp ();

    /** Copy constructor. */
    public: TimeStamp (const TimeStamp &ts);

    /** Destructor. */
    public: ~TimeStamp () { }

    /** Creates a new instance, using the default leap-second reference.
     *  @param epoch The epoch used (must be UTC or GPS).
     *  @param sec   The whole seconds (32-bit unsigned number).
     *  @param ps    The picoseconds.
     *  @param sr      The sampling rate.
     *  @throws VRTException if <tt>tsi</tt> is less than 0 or greater than 0xFFFFFFFF;
     *                 or if <tt>tsi</tt> is null and <tt>tsiMode</tt> is not <tt>None</tt>;
     *                 or if <tt>tsf</tt> is null and <tt>tsfMode</tt> is not <tt>None</tt>;
     *                 or if the fractional time stamp mode is <tt>RealTime</tt> and the value
     *                    (in picoseconds) is greater than or equal to one second.
     */
    public: TimeStamp (IntegerMode epoch, uint32_t sec, uint64_t ps, double sr = DOUBLE_NAN);

    /** Creates a new instance, using the default leap-second reference.
     *  @param tsiMode The IntegerMode mode used.
     *  @param tsfMode The FractionalMode mode used.
     *  @param tsi     The IntegerMode value.
     *  @param tsf     The FractionalMode value.
     *  @param sr      The sampling rate.
     *  @throws VRTException if <tt>tsi</tt> is less than 0 or greater than 0xFFFFFFFF;
     *                 or if <tt>tsi</tt> is null and <tt>tsiMode</tt> is not <tt>None</tt>;
     *                 or if <tt>tsf</tt> is null and <tt>tsfMode</tt> is not <tt>None</tt>;
     *                 or if the fractional time stamp mode is <tt>RealTime</tt> and the value
     *                    (in picoseconds) is greater than or equal to one second.
     */
    public: TimeStamp (IntegerMode tsiMode, FractionalMode tsfMode, uint32_t tsi, uint64_t tsf, double sr = DOUBLE_NAN);
    
    /** Creates a new instance.
     *  @param tsiMode The IntegerMode mode used.
     *  @param tsfMode The FractionalMode mode used.
     *  @param tsi     The IntegerMode value.
     *  @param tsf     The FractionalMode value.
     *  @param ls      The leap-seconds reference to use.
     *  @param sr      The sampling rate.
     *  @throws VRTException if <tt>tsi</tt> is less than 0 or greater than 0xFFFFFFFFL;
     *                 or if <tt>tsi</tt> is null and <tt>tsiMode</tt> is not <tt>None</tt>;
     *                 or if <tt>tsf</tt> is null and <tt>tsfMode</tt> is not <tt>None</tt>;
     *                 or if the fractional time stamp mode is <tt>RealTime</tt> and the value
     *                    (in picoseconds) is greater than or equal to one second.
     */
    public: TimeStamp (IntegerMode tsiMode, FractionalMode tsfMode, uint32_t tsi, uint64_t tsf, LeapSeconds *ls, double sr = DOUBLE_NAN);

    /** A record is considered null both the integer and fractional modes are set to None. */
    public: inline virtual bool isNullValue () const {
      return (tsiMode == IntegerMode_None) && (tsfMode == FractionalMode_None);
    }

    /** Gets a time stamp using system time. This time should be considered unreliable as it is
        using <tt>gettimeofday(..)</tt> as its data source rather than a precision time
        reference. The resolution of this time stamp is only as good as that provided by the O/S,
        and will never be better than 1 micorsecond.
     */
    public: static inline TimeStamp getSystemTime () {
      return getSystemTime(LeapSeconds::getDefaultInstance());
    }

    /** Gets a time stamp using system time. This time should be considered unreliable as it is
        using <tt>gettimeofday(..)</tt> as its data source rather than a precision time
        reference. The resolution of this time stamp is only as good as that provided by the O/S,
        and will never be better than 1 micorsecond.
        @param ls The leap-seconds reference to use.
     */
    public: static TimeStamp getSystemTime (LeapSeconds *ls);

    /** Converts from UTC or GPS time to UTC. This operation does nothing to the fractional time
        seconds, and is only valid for dates after 1-Jan-1980.
        @return <tt>this</tt> if already in UTC, otherwise a new TimeStamp in GPS time representing
                the same time.
        @throws VRTException If the current time stamp is not GPS or UTC.
     */
    public: TimeStamp toUTC () const;

    /** Converts from UTC or GPS time to GPS. This operation does nothing to the fractional time
        seconds, and is only valid for dates after 1-Jan-1980.
        @return <tt>this</tt> if already in GPS, otherwise a new TimeStamp in UTC time representing
                the same time.
        @throws VRTException If the current time stamp is not GPS or UTC.
     */
    public: TimeStamp toGPS () const;

    /** Adds the specified number of seconds to the current time and returns the resulting time.
        This is identical to <tt>addTime(sec,0L)</tt> except that it will not throw an exception if
        {@link #getFractionalMode()} is not <tt>FractionalMode_RealTime</tt>.
        @param sec The number of seconds.
        @return A new time object representing the resulting time.
        @throws VRTException If the {@link #getIntegerMode() } is not
                <tt>IntegerMode_UTC</tt> or <tt>IntegerMode_GPS</tt> .
     */
    public: inline TimeStamp addSeconds (int64_t sec) const {
      return addTime(sec, __INT64_C(0), false);
    }

    /** Adds the specified number of picoseconds to the current time and returns the resulting time.
        This is identical to <tt>addTime(0L,ps)</tt>.
        @param ps The number of picoseconds.
        @return A new time object representing the resulting time.
        @throws VRTException If the {@link #getIntegerMode() } is not
                <tt>IntegerMode_UTC</tt> or <tt>IntegerMode_GPS</tt> or if {@link #getFractionalMode()}
                is not <tt>FractionalMode_RealTime</tt>.
     */
    public: inline TimeStamp addPicoSeconds (int64_t ps) const {
      return addTime(__INT64_C(0), ps, ONE_SEC, true);
    }

    /** Adds the specified number of picoseconds to the current time and returns the resulting time.
        This is identical to <tt>addTime(0L,ps)</tt>.
        @param samples The number of samples.
        @param sr      The sample rate.
        @return A new time object representing the resulting time.
        @throws VRTException If the {@link #getIntegerMode() } is not
                <tt>IntegerMode_UTC</tt> or <tt>IntegerMode_GPS</tt> or if {@link #getFractionalMode()}
                is not <tt>FractionalMode_RealTime</tt>.
     */
    public: inline TimeStamp addSamples (int64_t samples, double sr) const {
      return addTime(__INT64_C(0), samples, sr, true);
    }


    /** Adds the specified number of picoseconds to the current time and returns the resulting time.
        <pre>
          r = t.addTime(sec, ps)
          implies: r.getTimeStampInteger()    = t.getTimeStampInteger()    + sec
                   r.getTimeStampFractional() = t.getTimeStampFractional() + ps
        </pre>
         Note that if the value of <tt>ps</tt> exceeds {@link #ONE_SEC}, the input parameters will
        be normalized with:
        <pre>
          sec = (ps / ONE_SEC) + sec;
          ps  = (ps % ONE_SEC);
        </pre>
        Note that subtracting time can be accomplished simply by calling <tt>addTime(-sec,-ps)</tt>.
        Both <tt>sec</tt> and <tt>ps</tt> must be signed negative to accomplish the subtraction of
        seconds+picoseconds. <br>
        <br>
        This method maintains the functionality that the following will all of the following are
        equal (assuming time never exceeds 0xFFFFFFFF, not drops below the start of the epoch):
        <pre>
          r = t.addTime(a,b)
          r = t.addTime(a,0L).addTime(0L,b)
          r = t.addTime(0L,b).addTime(a,0L)
        </pre>
        @param sec The number of seconds.
        @param ps  The number of picoseconds.
        @return A new time object representing the resulting time.
        @throws VRTException If the {@link #getIntegerMode()} is not <tt>IntegerMode_UTC</tt> or
                             <tt>IntegerMode_GPS</tt> or if {@link #getFractionalMode()}
                             is not <tt>FractionalMode_RealTime</tt>.
     */

    public: inline TimeStamp addTime (int64_t sec, int64_t ps) const {
      return addTime(sec, ps, ONE_SEC, true);
    }

    public: inline TimeStamp addTime (int64_t sec, int64_t fsec, double sr) const {
      return addTime(sec, fsec, sr, true);
    }

    /** Internal method, flags if fractional is supported, if fract=false, ps must be 0L. */
    private: TimeStamp addTime (int64_t sec, int64_t fsec, double sr, bool fract) const;

    /** Compares time stamp values. If the time stamps have different epochs, this method will
        attempt to convert both to GPS and will throw a {@link VRTException} if
        unable to do so. If the fractioal time stamp values have a different reference, an
        {@link VRTException} will be thrown. This method assumes that any
        "wrap-arround" within the fractional time stamp is accounted for in the integral time stamp.
        If two GPS/UTC times are compared, this will return 0 iff they represent exactly the same
        instant in time.
        @return <tt>&lt;0</tt> if <tt>this</tt> is comes before than <tt>other</tt>,
                <tt>=0</tt> if <tt>this</tt> is comes at the same point as <tt>other</tt>,
                <tt>&gt;0</tt> if <tt>this</tt> is comes after than <tt>other</tt>.
        @throws VRTException If the current time stamp is not GPS or UTC.
     */
    public: char compareTo (const TimeStamp &other) const;

    /** Equals method that checks for strict equality. Two time stamps representing the same point
        but using different epoch (UTC vs GPS) will not be considered equal.
        @param ts The object to compare to this one.
        @return <tt>true</tt> if exactly equal, <tt>false</tt> otherwise.
     */
    public: bool equals (const VRTObject &ts) const;

    /** A textural representation of the time stamp. If the integer mode is <tt>IntegerMode_GPS</tt>
     *  or <tt>IntegerMode_UTC</tt> and fractional mode is <tt>FractionalMode_None</tt> or
     *  <tt>FractionalMode_RealTime</tt> (i.e. the 90% use case) the output will be an RFC3339-compatible
     *  time stamp with 0 (no fractional time stamp) or 12 (picoseconds) decimal places followed by
     *  either <tt>" (GPS)"</tt> or <tt>" (UTC)"</tt>. Other time stamps will use a custom format. <br>
     *  <br>
     *  References:
     *  <pre>
     *    Klyne, G. and C. Newman. "RFC3339 - Date and Time on the Internet: Timestamps."
     *    Network Working Group. July 2002. <a href="http://www.faqs.org/rfcs/rfc3339.html">http://www.faqs.org/rfcs/rfc3339.html</a>
     *    (accessed 2009-07-31).
     *  </pre>
     *  @return A textural representation of the time stamp, or null if both IntegerMode and
     *          FractionalMode are None.
     */
    public: string toString () const;

    public: string toStringUTC (string format) const;

    /** A textural representation of the time stamp in UTC. This is identical to the time stamp
     *  produced by <tt>ts.toUTC().toString()</tt> except that the " (UTC)" suffix is omitted.
     *  @return A textural representation of the time stamp, or null if both IntegerMode and
     *          FractionalMode are None.
     */
    public: string toStringUTC () const;

    /** A textural representation of the time stamp in UTC. This is identical to the time stamp
     *  produced by <tt>ts.toGPS().toString()</tt> except that the " (GPS)" suffix is omitted.
     *  @return A textural representation of the time stamp, or null if both IntegerMode and
     *          FractionalMode are None.
     */
    public: string toStringGPS () const;

    /** Gets the epoch used.
     *  @return {@link #GPS_EPOCH} or {@link #UTC_EPOCH} or {@link #NULL_EPOCH} if neither apply. This will return
     *          {@link #NULL_EPOCH} any time <tt>FractionalMode_SampleCount</tt> or <tt>FractionalMode_FreeRunningCount</tt>
     *          is in use.
     *  @see #getIntegerMode()
     *  @see #getFractionalMode()
     */
    public: IntegerMode getEpoch () const;

    /** Gets the time stamp mode used for integeral seconds.
     *  @see #getTimeStampInteger()
     */
    public: inline IntegerMode getIntegerMode () const {
      return tsiMode;
    }

    /** Gets the time stamp mode used for fractional seconds.
     *  @see #getTimeStampFractional()
     */
    public: inline FractionalMode getFractionalMode () const {
      return tsfMode;
    }

    /** Gets the integer time stamp in UTC seconds (if GPS it converts to UTC first).
     *  @return The integer time stamp. This will be an unsigned 32-bit value as an <tt>int</tt>.
     *  @throws VRTException If the time stamp is not GPS or UTC.
     *  @see #getUTCSeconds()
     */
    public: uint32_t getSecondsUTC () const;

    /** Gets the floating point time stamp in UTC seconds (if GPS it converts to UTC first).
     *  including fractional seconds.
     *  @return The floating point time stamp. This will be a double-precision floating point number.
     *  @throws VRTException If the time stamp is not GPS or UTC.
     *  @see #getUTCSeconds()
     */
    public: double getDoubleSeconds () const;
    
    /** Gets the floating point time stamp in UTC seconds (if GPS it converts to UTC first) given a sampling rate.
     *  including fractional seconds.
     *  @param sr the sampling rate
     *  @return The floating point time stamp. This will be a double-precision floating point number.
     *  @throws VRTException If the time stamp is not GPS or UTC.
     *  @see #getUTCSeconds()
     */
    public: double getDoubleSeconds (double sr) const;

    /** Gets the integer time stamp in GPS seconds (if UTC it converts to GPS first).
     *  @return The integer time stamp. This will be an unsigned 32-bit value as an <tt>int</tt>.
     *  @throws VRTException If the time stamp is not GPS or UTC.
     *  @see #getGPSSeconds()
     */
    public: uint32_t getSecondsGPS () const;

    /** Gets the integer time stamp in NORAD seconds (if GPS it converts to UTC first then from
     *  UTC to NORAD).
     *  @param leapCounted Were any mid-year leap seconds counted (true) or skipped (false)? If this
     *                     is _NULL, the value of <tt>VRTConfig::getNoradLeapSecCounted()</tt> is used.
     *  @return The integer time stamp. This will be an unsigned 32-bit value as an <tt>int</tt>.
     *  @throws VRTException If the time stamp is not GPS or UTC.
     *  @see #getNORADSeconds()
     */
    public: inline uint32_t getSecondsNORAD (boolNull leapCounted=_NULL) const {
      return (uint32_t)getNORADSeconds(leapCounted);
    }

    /** Gets the integer time stamp in POSIX seconds (if GPS it converts to UTC first then from
     *  UTC to POSIX).
     *  @return The integer time stamp. This will be an unsigned 32-bit value as an <tt>int</tt>.
     *  @throws VRTException If the time stamp is not GPS or UTC.
     *  @see #getPOSIXSeconds()
     */
    public: inline uint32_t getSecondsPOSIX () const {
      return (uint32_t)getPOSIXSeconds();
    }

    /** Gets the integer time stamp in UTC seconds (if GPS it converts to UTC first).
     *  @return The integer time stamp. This will be an unsigned 32-bit value as a <tt>long</tt>.
     *  @throws VRTException If the time stamp is not GPS or UTC.
     *  @see #getSecondsUTC()
     */
    public: inline int64_t getUTCSeconds () const {
      return (int64_t)getSecondsUTC();
    }

    /** Gets the integer time stamp in GPS seconds (if UTC it converts to GPS first).
     *  @return The integer time stamp. This will be an unsigned 32-bit value as a <tt>long</tt>.
     *  @throws VRTException If the time stamp is not GPS or UTC.
     *  @see #getSecondsGPS()
     */
    public: int64_t getGPSSeconds () const {
      return (int64_t)getSecondsGPS();
    }

    /** Gets the integer time stamp in NORAD seconds (if GPS it converts to UTC first then from
     *  UTC to NORAD).
     *  @param leapCounted Were any mid-year leap seconds counted (true) or skipped (false)? If this
     *                     is _NULL, the value of <tt>VRTConfig::getNoradLeapSecCounted()</tt> is used.
     *  @return The integer time stamp. This will be an unsigned 32-bit value as a <tt>long</tt>.
     *  @throws VRTException If the time stamp is not GPS or UTC.
     *  @see #getSecondsNORAD()
     */
    public: inline int64_t getNORADSeconds (boolNull leapCounted=_NULL) const {
      bool _leapCounted;
      if (leapCounted == _NULL) {
        _leapCounted = VRTConfig::getNoradLeapSecCounted();
      }
      else {
        _leapCounted = (leapCounted == _TRUE);
      }
      return _getNORADSeconds(_leapCounted);
    }

    /** Same as above, but with a bool rather than a boolNull. */
    private: int64_t _getNORADSeconds (bool leapCounted) const;

    /** Gets the integer time stamp in POSIX seconds (if GPS it converts to UTC first then from
     *  UTC to POSIX).
     *  @return The integer time stamp. This will be an unsigned 32-bit value as a <tt>long</tt>.
     *  @throws VRTException If the time stamp is not GPS or UTC.
     *  @see #getSecondsPOSIX()
     */
    public: int64_t getPOSIXSeconds () const;

    /** Gets the integer and fractional time stamp in Midas seconds. This is
     *  identical to:
     *  <pre>
     *     getMidasSeconds() + getFractionalSeconds()    // using RealTime
     *    or
     *     getMidasSeconds()                             // not using RealTime
     *  </pre>
     *  <b>WARNING: This will result in a loss of precision! Users are advised
     *  to use {@link #getMidasSeconds} and {@link #getFractionalSeconds} to
     *  avoid this loss of precision.</b>
     *  @return The integer and fractional time stamp with some loss of precision.
     *  @throws VRTException If the time stamp is not GPS or UTC.
     *  @see #getSecondsPOSIX()
     */
    public: inline double getMidasTime () const {
      if (getFractionalMode() == FractionalMode_RealTime) {
        return getMidasSeconds() + getFractionalSeconds();
      }
      else {
        return getMidasSeconds();
      }
    }

    /** Gets the integer time stamp in Midas seconds. This is identical to:
     *  <pre>
     *     getPOSIXSeconds() + MIDAS2POSIX
     *  </pre>
     *  @return The integer time stamp.
     *  @throws VRTException If the time stamp is not GPS or UTC.
     *  @see #getSecondsPOSIX()
     */
    public: inline double getMidasSeconds () const {
      return (double)(getPOSIXSeconds() + MIDAS2POSIX);
    }
    
    /** Gets the sample rate.
     *  @return the sample rate.
     */
    public: double getSampleRate() const;

    /** Gets the fractional time stamp in picoseconds.
     *  @param sr The sample rate (only applicable for tsf of FractionalMode_SampleCount)
     *  @return The fractional time stamp.
     *  @throws VRTException If the fractional time stamp is not RealTime.
     */
    public: uint64_t getPicoSeconds (double sr) const;

    /** Gets the fractional time stamp in picoseconds.
     *  @return The fractional time stamp.
     *  @throws VRTException If the fractional time stamp is not RealTime.
     */
    public: uint64_t getPicoSeconds () const;
    
    /** Gets the fractional time stamp as a fraction of a second. This is
     *  identical to the following (done in double-precision):
     *  <pre>
     *    getPicoSeconds() / ONE_SEC
     *  </pre>
     *  @return The fractional time stamp.
     *  @throws VRTException If the fractional time stamp is not RealTime.
     */
    public: inline double getFractionalSeconds () const {
      return ((double)getPicoSeconds()) / ((double)ONE_SEC);
    }

    /** Gets the fractional time stamp in samples
     *
     *  @return The fractional time stamp.
     *  @throws VRTException If the fractional time stamp is not SampleCount
     */
    public: uint64_t getSampleCount () const;


    /** Gets the integer time stamp. Be sure to consult {@link #getIntegerMode()} to
     *  find out the type of time stamp used.
     *  @return The integer time stamp (returns 0 for <tt>IntegerMode_None</tt>). This will be
     *          an unsigned 32-bit value.
     */
    public: inline uint32_t getTimeStampInteger () const {
      return tsi;
    }

    /** Gets the fractional time stamp. Be sure to consult {@link #getFractionalMode()}
     *  to find out the type of time stamp used.
     *  @return The fractional time stamp (returns 0 for <tt>FractionalMode_None</tt>).
     */
    public: inline uint64_t  getTimeStampFractional () const {
      return tsf;
    }

    /** Gets the leap-seconds reference used.
     *  @return The leap-seconds reference used.
     */
    public: inline LeapSeconds *getLeapSecondRef () const {
      return ls;
    }

    /** Parses a time stamp in one of the ISO 8601 formats documented in <tt>parseTime(time, tsiMode, ls)</tt>.
        @param time    The time string.
        @param tsiMode The time epoch (must be <tt>IntegerMode_GPS</tt> or <tt>IntegerMode_UTC</tt>).
        @return The parsed time stamp.
     */
    public: static inline TimeStamp parseTime (string time, IntegerMode tsiMode) {
      return parseTime(time, tsiMode, LeapSeconds::getDefaultInstance());
    }

    /** Parses a time stamp in one of the following ISO 8601 formats.
        <pre>
          yyyy-mm-ddThh:mm:ss.sss        (TimeZone = +0 hours)
          yyyy-mm-ddThh:mm:ss.sssZ       (TimeZone = +0 hours)
          yyyy-mm-ddThh:mm:ss.sss+zz     (TimeZone = +zz hours)
          yyyy-mm-ddThh:mm:ss.sss-zz     (TimeZone = -zz hours)
          yyyy-mm-ddThh:mm:ss.sss+zz:zz  (TimeZone = +zz:zz hours)
          yyyy-mm-ddThh:mm:ss.sss-zz:zz  (TimeZone = -zz:zz hours)
        </pre>
        @param time    The time string.
        @param tsiMode The time epoch (must be <tt>IntegerMode_GPS</tt> or <tt>IntegerMode_UTC</tt>).
        @param ls      The leap seconds instance to use.
        @return The parsed time stamp.
     */
    public: static TimeStamp parseTime (string time, IntegerMode tsiMode, LeapSeconds *ls);

    /** Converts a time specifier to a time stamp.
        @param year    The year (1981 to 2099).
        @param mon     The month (1 to 12).
        @param day     The day of month (1 to 31).
        @param hour    The hour of day (0 to 23).
        @param min     The minute of hour (0 to 59).
        @param wsec    The whole seconds (0 to 60).
        @param psec    Any picoseconds.
        @param zone    The time-zone delta in seconds.
        @param tsiMode The time epoch (must be <tt>IntegerMode_GPS</tt> or <tt>IntegerMode_UTC</tt>).
        @return The time stamp.
     */
    public: static inline TimeStamp forTime (int32_t year, int32_t mon, int32_t day, int32_t hour, int32_t min, int32_t wsec,
                                             int64_t psec, int32_t zone, IntegerMode tsiMode) {
      return forTime(year, mon, day, hour, min, wsec, psec, zone, tsiMode, LeapSeconds::getDefaultInstance());
    }

    /** Converts a time specifier to a time stamp.
        @param year    The year (1981 to 2099).
        @param mon     The month (1 to 12).
        @param day     The day of month (1 to 31).
        @param hour    The hour of day (0 to 23).
        @param min     The minute of hour (0 to 59).
        @param wsec    The whole seconds (0 to 60).
        @param psec    Any picoseconds.
        @param zone    The time-zone delta in seconds.
        @param tsiMode The time epoch (must be <tt>IntegerMode_GPS</tt> or <tt>IntegerMode_UTC</tt>).
        @param ls      The leap seconds instance to use.
        @return The time stamp.
     */
    public: static TimeStamp forTime (int32_t year, int32_t mon, int32_t day, int32_t hour, int32_t min, int32_t wsec,
                                      int64_t psec, int32_t zone, IntegerMode tsiMode, LeapSeconds *ls);


    /** Creates a time stamp from a POSIX time. See lengthy discussion at top of
        this class for details regarding POSIX to UTC conversions.
        @param seconds     The number of whole seconds since the POSIX epoch.
        @param picoseconds The number of picoseconds since the start of the current second.
        @return The corresponding TimeStamp.
        @throws VRTException if the time corresponds to a value prior to 1972/01/01.
     */
    public: static inline TimeStamp forTimePOSIX (int64_t seconds, int64_t picoseconds) {
      return forTimePOSIX(seconds, picoseconds, LeapSeconds::getDefaultInstance());
    }

    /** Creates a time stamp from a POSIX time. See lengthy discussion at top of
        this class for details regarding POSIX to UTC conversions.
        @param seconds     The number of whole seconds since the POSIX epoch.
        @param picoseconds The number of picoseconds since the start of the current second.
        @param ls          The leap seconds instance to use (null to use default).
        @return The corresponding TimeStamp.
        @throws VRTException if the time corresponds to a value prior to 1972/01/01.
     */
    public: static TimeStamp forTimePOSIX (int64_t seconds, int64_t picoseconds, LeapSeconds *ls);

    /** Creates a time stamp from a Midas time. This is identical to:
        <pre>
          forTimePOSIX(seconds - MIDAS2POSIX, ONE_SEC * fractionalSeconds)
        </pre>
        <b>WARNING: <tt>seconds</tt> must be an integral value and <tt>fractionalSeconds</tt>
        must be a value on the range [0,1). Failure to follow this will result in undefined
        behavior.</b>
        @param seconds           The number of whole seconds since the Midas epoch.
        @param fractionalSeconds The fractional seconds since the start of the current second.
        @return The corresponding TimeStamp.
        @throws VRTException if the time corresponds to a value prior to 1972/01/01.
     */
    public: static inline TimeStamp forTimeMidas (double seconds, double fractionalSeconds) {
      return forTimePOSIX((int64_t)(seconds - MIDAS2POSIX),
                          (int64_t)(ONE_SEC * fractionalSeconds));
    }

    /** Creates a time stamp from a Midas time. This is identical to:
        <pre>
          forTimePOSIX(seconds - MIDAS2POSIX, ONE_SEC * fractionalSeconds, ls)
        </pre>
        <b>WARNING: <tt>seconds</tt> must be an integral value and <tt>fractionalSeconds</tt>
        must be a value on the range [0,1). Failure to follow this will result in undefined
        behavior.</b>
        @param seconds           The number of whole seconds since the Midas epoch.
        @param fractionalSeconds The fractional seconds since the start of the current second.
        @param ls                The leap seconds instance to use (null to use default).
        @return The corresponding TimeStamp.
        @throws VRTException if the time corresponds to a value prior to 1972/01/01.
     */
    public: static inline TimeStamp forTimeMidas (double seconds, double fractionalSeconds, LeapSeconds *ls) {
      return forTimePOSIX((int64_t)(seconds - MIDAS2POSIX),
                          (int64_t)(ONE_SEC * fractionalSeconds),
                          ls);
    }

    /** Creates a GPS time stamp from a PTP time. This only supports times after 6 Jan 1980 since
        there is a simple conversion from PTP to GPS time. It is possible to convert from PTP to UTC
        for dates between 1 Jan 1972 (not 1 Jan 1970) and 6 Jan 1980, but it would add significant
        complication to what is otherwise a trivial function that the compiler can easily in-line.
        @param seconds     The number of whole seconds since the PTP epoch.
        @param nanoseconds The number of nanoseconds since the start of the current second.
        @return The corresponding TimeStamp.
        @throws VRTException if the time corresponds to a value prior to 1980/01/06.
     */
    public: static inline TimeStamp forTimePTP (int64_t seconds, int64_t nanoseconds) {
      return forTimePTP(seconds, nanoseconds, LeapSeconds::getDefaultInstance());
    }

    /** Creates a GPS time stamp from a PTP time. This only supports times after 6 Jan 1980 since
        there is a simple conversion from PTP to GPS time. It is possible to convert from PTP to UTC
        for dates between 1 Jan 1972 (not 1 Jan 1970) and 6 Jan 1980, but it would add significant
        complication to what is otherwise a trivial function that the compiler can easily in-line.
        @param seconds     The number of whole seconds since the PTP epoch.
        @param nanoseconds The number of nanoseconds since the start of the current second.
        @param ls          The leap seconds instance to use.
        @return The corresponding TimeStamp.
        @throws VRTException if the time corresponds to a value prior to 1980/01/06.
     */
    public: static inline TimeStamp forTimePTP (int64_t seconds, int64_t nanoseconds, LeapSeconds *ls) {
      return TimeStamp(ls, IntegerMode_GPS, FractionalMode_RealTime, seconds-GPS2PTP, nanoseconds*1000);
    }

    /** Creates a time stamp from a NORAD time. The NORAD time epoch is measured in number of
        1-second intervals elapsed since the start of the current UTC year; however, there is some
        ambiguity as to whether a mid-year leap second is counted or not counted. This is the same
        as <tt>forTimeNORAD(seconds, picoseconds, -1, null)</tt>
        @param seconds     The number of whole seconds since the start of the year.
        @param picoseconds The number of picoseconds since the start of the current second.
        @param leapCounted Were any mid-year leap seconds counted (true) or skipped (false)? If this
                           is _NULL, the value of <tt>VRTConfig::getNoradLeapSecCounted()</tt> is used.
        @return The corresponding TimeStamp.
     */
    public: static inline TimeStamp forTimeNORAD (int32_t seconds, int64_t picoseconds, boolNull leapCounted=_NULL) {
      return forTimeNORAD(seconds, picoseconds, leapCounted, -1, LeapSeconds::getDefaultInstance());
    }

    /** Creates a time stamp from a NORAD time. The NORAD time epoch is measured in number of
        1-second intervals elapsed since the start of the current UTC year; however, there is some
        ambiguity as to whether a mid-year leap second is counted or not counted. This is the same
        as <tt>forTimeNORAD(seconds, picoseconds, _NULL, year, ls)</tt>
        @param seconds     The number of whole seconds since the start of the year.
        @param picoseconds The number of picoseconds since the start of the current second.
        @param year        The year to use (-1 for automatic, otherwise after 1972).
        @param ls          The leap seconds instance to use.
        @return The corresponding TimeStamp.
     */
    public: static inline TimeStamp forTimeNORAD (int32_t seconds, int64_t picoseconds,
                                                  int32_t year, LeapSeconds *ls) {
      return forTimeNORAD(seconds, picoseconds, _NULL, year, ls);
    }
    
    /** Creates a time stamp from a NORAD time. The NORAD time epoch is measured in number of
        1-second intervals elapsed since the start of the current UTC year; however, there is some
        ambiguity as to whether a mid-year leap second is counted or not counted. At the time this
        function was written (2011), it had been 14+ years since the last mid-year leap second, so
        there is little reason to believe that past experience in this regard is a good indicator
        of future behavior. <br>
        <br>
         <b>Automatic Year Setting:</b> When converting NORAD time to UTC, the number of UTC
        seconds at the start of the current year is added in. If the year is not given (i.e. -1), the
        current year is used. However, if a late-December time is seen in early-January, the previous
        year is used. Similarly if an early-January time is seen in late-December, the subsequent year
        is used. This is done to account for cases where data that is time-stamped at the end of one
        year does not get processed until the beginning of the next year.
        @param seconds     The number of whole seconds since the start of the year.
        @param picoseconds The number of picoseconds since the start of the current second.
        @param leapCounted Were any mid-year leap seconds counted (true) or skipped (false)? If this
                           is _NULL, the value of <tt>VRTConfig::getNoradLeapSecCounted()</tt> is used.
        @param year        The year to use (-1 for automatic, otherwise after 1972).
        @param ls          The leap seconds instance to use.
        @return The corresponding TimeStamp.
     */
    public: static inline TimeStamp forTimeNORAD (int32_t seconds, int64_t picoseconds, boolNull leapCounted,
                                                  int32_t year, LeapSeconds *ls) {
      bool _leapCounted;
      if (leapCounted == _NULL) {
        _leapCounted = VRTConfig::getNoradLeapSecCounted();
      }
      else {
        _leapCounted = (leapCounted == _TRUE);
      }
      return _forTimeNORAD(seconds, picoseconds, _leapCounted, year, ls);
    }
    
    /** Same as above, but bool rather than boolNull. */
    private: static TimeStamp _forTimeNORAD (int32_t seconds, int64_t picoseconds, bool leapCounted,
                                             int32_t year, LeapSeconds *ls);

    /** Creates a time stamp from an IRIG time. This is the same as
        <tt>forTimeIRIG(seconds,minutes,hours,days,0,LeapSeconds::getDefaultInstance())</tt>
        @param seconds     The number of whole seconds since the start of the minute (0 to 59[+1]).
        @param minutes     The number of whole minutes since the start of the hour   (0 to 59).
        @param hours       The number of whole hours   since the start of the day    (0 to 23).
        @param days        The number of whole days    since the start of the year   (0 to 365[+1])
                           where 1=Jan1, 2=Jan2, ..., 32=Feb1, ...).
        @return The corresponding TimeStamp.
        @throws IllegalArgumentException If an invalid parameter is specified.
     */
    public: static inline TimeStamp forTimeIRIG (int32_t seconds, int32_t minutes, int32_t hours,
                                                 int32_t days) {
      return forTimeIRIG(seconds,minutes,hours,days,0,LeapSeconds::getDefaultInstance());
    }

    /** Creates a time stamp from an IRIG time. This is the same as
        <tt>forTimeIRIG(seconds,minutes,hours,days,0,ls)</tt>
        @param seconds     The number of whole seconds since the start of the minute (0 to 59[+1]).
        @param minutes     The number of whole minutes since the start of the hour   (0 to 59).
        @param hours       The number of whole hours   since the start of the day    (0 to 23).
        @param days        The number of whole days    since the start of the year   (0 to 365[+1])
                           where 1=Jan1, 2=Jan2, ..., 32=Feb1, ...).
        @param ls          The leap seconds instance to use (null to use default).
        @return The corresponding TimeStamp.
        @throws IllegalArgumentException If an invalid parameter is specified.
     */
    public: static inline TimeStamp forTimeIRIG (int32_t seconds, int32_t minutes, int32_t hours,
                                                 int32_t days, LeapSeconds *ls) {
      return forTimeIRIG(seconds,minutes,hours,days,0,ls);
    }

    /** Creates a time stamp from an IRIG time. This is the same as
        <tt>forTimeIRIG(seconds,minutes,hours,days,hundreths,LeapSeconds::getDefaultInstance())</tt>
        @param seconds     The number of whole seconds since the start of the minute (0 to 59[+1]).
        @param minutes     The number of whole minutes since the start of the hour   (0 to 59).
        @param hours       The number of whole hours   since the start of the day    (0 to 23).
        @param days        The number of whole days    since the start of the year   (0 to 365[+1])
                           where 1=Jan1, 2=Jan2, ..., 32=Feb1, ...).
        @param hundreths   The number of whole 1/100 seconds since the start of the current second
                           (0 to 99).
        @return The corresponding TimeStamp.
        @throws IllegalArgumentException If an invalid parameter is specified.
     */
    public: static inline TimeStamp forTimeIRIG (int32_t seconds, int32_t minutes, int32_t hours,
                                                 int32_t days, int32_t hundreths) {
      return forTimeIRIG(seconds,minutes,hours,days,hundreths,LeapSeconds::getDefaultInstance());
    }

    /** Creates a time stamp from an IRIG time. This function implicitly knows that IRIG time
        does not indicate if any mid-year leap-seconds that have occurred nor does it count them
        (except in rare situations that the IRIG time stamp is seen as a leap-second is being
        added [i.e. 23:59:60], in which case this function returns
        <tt>forTimeIRIG(59,59,23,days,hundreths,ls).addSeconds(1)</tt>); as such, it always consults
        the applicable leap-seconds table to check for scheduled mid-year leap-seconds.
        @param seconds     The number of whole seconds since the start of the minute (0 to 59[+1]).
        @param minutes     The number of whole minutes since the start of the hour   (0 to 59).
        @param hours       The number of whole hours   since the start of the day    (0 to 23).
        @param days        The number of whole days    since the start of the year   (0 to 365[+1])
                           where 1=Jan1, 2=Jan2, ..., 32=Feb1, ...).
        @param hundreths   The number of whole 1/100 seconds since the start of the current second
                           (0 to 99).
        @param ls          The leap seconds instance to use.
        @return The corresponding TimeStamp.
        @throws IllegalArgumentException If an invalid parameter is specified.
     */
    public: static TimeStamp forTimeIRIG (int32_t seconds, int32_t minutes, int32_t hours,
                                          int32_t days, int32_t hundreths, LeapSeconds *ls);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // DEPRECATED FUNCTIONS
    //////////////////////////////////////////////////////////////////////////////////////////////////
    /** @deprecated use forTimeNORAD(..) */
    public: static inline TimeStamp forNoradTime (int32_t seconds, int64_t picoseconds) {
      return forTimeNORAD(seconds, picoseconds, -1, LeapSeconds::getDefaultInstance());
    }

    /** @deprecated use forTimeNORAD(..) */
    public: static inline TimeStamp forNoradTime (int32_t seconds, int64_t picoseconds, int32_t year, LeapSeconds *ls) {
      return forTimeNORAD(seconds, picoseconds, year, ls);
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Implement HasFields
    //////////////////////////////////////////////////////////////////////////////////////////////////
    public: virtual int32_t   getFieldCount () const;
    public: virtual string    getFieldName  (int32_t id) const;
    public: virtual ValueType getFieldType  (int32_t id) const;
    public: virtual Value*    getField      (int32_t id) const;
    public: virtual void      setField      (int32_t id, const Value* val);
  };

  // See VRTMath.c for function code
  namespace VRTMath {
    /** Pack a VITA 49.0 time stamp into a buffer.
        @param buf   byte array of data [OUTPUT]
        @param off   Offset into array
        @param val   value to pack [INPUT]
        @param epoch The epoch to use (must be either <tt>IntegerMode_UTC</tt> or <tt>IntegerMode_GPS</tt>).
        @param rep   The byte representation to use
     */
    void packTimeStamp (vector<char> &buf, int32_t off, const TimeStamp &val, IntegerMode epoch, ByteOrder rep=BIG_ENDIAN);

    /** Unpack an VITA 49.0 time stamp from a buffer.
        @param buf   byte array of data
        @param off   Offset into array
        @param epoch The epoch to use (must be either <tt>IntegerMode_UTC</tt> or <tt>IntegerMode_GPS</tt>).
        @param rep   The byte representation to use
        @return The unpacked value
     */
    TimeStamp unpackTimeStamp (const vector<char> &buf, int32_t off, IntegerMode epoch, ByteOrder rep=BIG_ENDIAN);
  };

  /** Supports comparing two a {@link TimeStamp} values. This is identical to using
   *  <tt>a.compareTo(b) &lt; 0</tt>.
   */
  inline bool operator<  (const TimeStamp &a, const TimeStamp &b) { return a.compareTo(b) <  0; }

  /** Supports comparing two a {@link TimeStamp} values. This is identical to using
   *  <tt>a.compareTo(b) &lt;= 0</tt>.
   */
  inline bool operator<= (const TimeStamp &a, const TimeStamp &b) { return a.compareTo(b) <= 0; }

  /** Supports comparing two a {@link TimeStamp} values. This is identical to using
   *  <tt>a.compareTo(b) == 0</tt>.
   */
  inline bool operator== (const TimeStamp &a, const TimeStamp &b) { return a.compareTo(b) == 0; }

  /** Supports comparing two a {@link TimeStamp} values. This is identical to using
   *  <tt>a.compareTo(b) &gt;= 0</tt>.
   */
  inline bool operator>= (const TimeStamp &a, const TimeStamp &b) { return a.compareTo(b) >= 0; }

  /** Supports comparing two a {@link TimeStamp} values. This is identical to using
   *  <tt>a.compareTo(b) &gt; 0</tt>.
   */
  inline bool operator>  (const TimeStamp &a, const TimeStamp &b) { return a.compareTo(b) >  0; }
};
#endif /* _TimeStamp_h */

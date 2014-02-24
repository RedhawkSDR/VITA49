/**
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

package nxm.vrt.lib;

import static java.lang.Integer.parseInt;
import static java.lang.Long.parseLong;

import static nxm.vrt.lib.LeapSeconds.toPicosecondString;
import static nxm.vrt.lib.LeapSeconds.gpsToYMDHMS;
import static nxm.vrt.lib.LeapSeconds.ymdToGpsDay;
import static nxm.vrt.lib.LeapSeconds.ymdToPosixDay;

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
 *      3 | Epoch                | IntegerMode
 *      4 | IntegerMode          | IntegerMode
 *      5 | FractionalMode       | FractionalMode
 *      6 | UTCSeconds           | long
 *      7 | GPSSeconds           | long
 *      8 | NORADSeconds         | long
 *      9 | POSIXSeconds         | long
 *     10 | MidasTime            | Double
 *     11 | MidasSeconds         | Double
 *     12 | PicoSeconds          | Long
 *     13 | FractionalSeconds    | Double
 *     14 | TimeStampInteger     | Long
 *     15 | TimeStampFractional  | Long
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
public final class TimeStamp implements Comparable<TimeStamp>, HasFields {
  /** The value of 1 second in picoseconds. */
  public static final long ONE_SEC = 1000000000000L;

  /** The UTC epoch. */
  public static final IntegerMode UTC_EPOCH = IntegerMode.UTC;

  /** The GPS epoch. */
  public static final IntegerMode GPS_EPOCH = IntegerMode.GPS;

  /** The "null" epoch. */
  public static final IntegerMode NULL_EPOCH = IntegerMode.None;
  
  /** An empty time stamp. This is equal to:
   *  <pre>
   *    NO_TIME_STAMP = new TimeStamp(IntegerMode.None, FractionalMode.None, null, null)
   *  </pre>
   */
  public static final TimeStamp NO_TIME_STAMP = new TimeStamp(IntegerMode.None, FractionalMode.None, null, null);

  /** This is equal to the start of the year 2000 in the GPS epoch and provides a reasonable
   *  minimum bounds for most time values.
   */
  public static final TimeStamp Y2K_GPS = TimeStamp.parseTime("2000-01-01T00:00:00.000Z", TimeStamp.IntegerMode.GPS);

  /** Delta between GPS and UTC epochs.
   *  <pre>
   *    GPS2UTC = (days 1 Jan 1970 to 6 Jan 1980) * (86,400 sec/day) + (leap seconds)
   *    GPS2UTC = (3,657 days) * (86,400 sec/day) + (11 seconds)
   *    GPS2UTC = 315,964,811 seconds
   *  </pre>
   *  See top of class for a detailed discussion regarding number of leap seconds added
   *  between 1 Jan 1970 and 6 Jan 1980.
   */
  public static final int GPS2UTC = 315964811;

  /** Delta between Midas and POSIX epochs.
   *  <pre>
   *    MIDAS2POSIX = (days 1 Jan 1950 to 1 Jan 1970) * (86,400 sec/day)
   *    MIDAS2POSIX = 631,152,000 seconds
   *  </pre>
   */
  public static final int MIDAS2POSIX = 631152000;

  /** Delta between GPS and PTP epochs.
   *  <pre>
   *    GPS2PTP = 315,964,819 seconds
   *  </pre>
   */
  public static final int GPS2PTP = 315964819;


  private static final long MAX_GPS2UTC = 0xFFFFFFFFL - GPS2UTC;
  private static final long MIN_UTC2GPS = GPS2UTC;
  
  private static final int ONE_YEAR_MAX    = 86400*366+12; // One year, in seconds with leap seconds
  private static final int LAST_TWO_WEEKS  = 86400*351;    // Last 31 days of year (approx.)
  private static final int FIRST_TWO_WEEKS = 86400*14;     // First 31 days of year (approx.)

  private final IntegerMode    tsiMode;
  private final FractionalMode tsfMode;
  private final long           tsi;
  private final long           tsf;
  private final LeapSeconds    ls;

  /** Creates a new instance, using the default leap-second reference.
   *  @param epoch The epoch used (must be UTC or GPS).
   *  @param sec   The whole seconds (32-bit unsigned number).
   *  @param ps    The picoseconds.
   *  @throws NullPointerException if <tt>tsiMode</tt> or <tt>tsfMode</tt> is null.
   *  @throws IllegalArgumentException if <tt>tsi</tt> is less than 0 or greater than 0xFFFFFFFF;
   *                 or if <tt>tsi</tt> is null and <tt>tsiMode</tt> is not <tt>None</tt>;
   *                 or if <tt>tsf</tt> is null and <tt>tsfMode</tt> is not <tt>None</tt>;
   *                 or if the fractional time stamp mode is <tt>RealTime</tt> and the value
   *                    (in picoseconds) is greater than or equal to one second.
   */
  public TimeStamp (IntegerMode epoch, int sec, long ps) {
    if ((epoch != IntegerMode.UTC) && (epoch != IntegerMode.GPS)) {
      throw new IllegalArgumentException("Expected GPS or UTC but got "+epoch);
    }
    if ((ps < 0L) || (ps >= ONE_SEC)) {
      throw new IllegalArgumentException("Invalid number of picoseconds ("+ps+"), must be in "
                                       + "the range [0, "+ONE_SEC+").");
    }
    this.tsiMode = epoch;
    this.tsfMode = FractionalMode.RealTime;
    this.tsi     = sec & 0xFFFFFFFFL;
    this.tsf     = ps;
    this.ls      = LeapSeconds.getDefaultInstance();
  }
  
  /** Creates a new instance, using the default leap-second reference.
   *  @param epoch The epoch used (must be UTC or GPS).
   *  @param sec   The whole seconds (32-bit unsigned number).
   *  @param ps    The picoseconds.
   *  @throws NullPointerException if <tt>tsiMode</tt> or <tt>tsfMode</tt> is null.
   *  @throws IllegalArgumentException if <tt>tsi</tt> is less than 0 or greater than 0xFFFFFFFF;
   *                 or if <tt>tsi</tt> is null and <tt>tsiMode</tt> is not <tt>None</tt>;
   *                 or if <tt>tsf</tt> is null and <tt>tsfMode</tt> is not <tt>None</tt>;
   *                 or if the fractional time stamp mode is <tt>RealTime</tt> and the value
   *                    (in picoseconds) is greater than or equal to one second.
   */
  public TimeStamp (IntegerMode epoch, long sec, long ps) {
    if ((epoch != IntegerMode.UTC) && (epoch != IntegerMode.GPS)) {
      throw new IllegalArgumentException("Expected GPS or UTC but got "+epoch);
    }
    if ((sec < 0) || (sec > 0xFFFFFFFFL)) {
      throw new IllegalArgumentException("Invalid number of seconds ("+sec+")");
    }
    if ((ps < 0L) || (ps >= ONE_SEC)) {
      throw new IllegalArgumentException("Invalid number of picoseconds ("+ps+"), must be in "
                                       + "the range [0, "+ONE_SEC+").");
    }
    this.tsiMode = epoch;
    this.tsfMode = FractionalMode.RealTime;
    this.tsi     = sec;
    this.tsf     = ps;
    this.ls      = LeapSeconds.getDefaultInstance();
  }
  
  /** Creates a new instance, using the default leap-second reference.
   *  @param tsiMode The IntegerMode mode used.
   *  @param tsfMode The FractionalMode mode used.
   *  @param tsi     The IntegerMode value.
   *  @param tsf     The FractionalMode value.
   *  @throws NullPointerException if <tt>tsiMode</tt> or <tt>tsfMode</tt> is null.
   *  @throws IllegalArgumentException if <tt>tsi</tt> is less than 0 or greater than 0xFFFFFFFF;
   *                 or if <tt>tsi</tt> is null and <tt>tsiMode</tt> is not <tt>None</tt>;
   *                 or if <tt>tsf</tt> is null and <tt>tsfMode</tt> is not <tt>None</tt>;
   *                 or if the fractional time stamp mode is <tt>RealTime</tt> and the value
   *                    (in picoseconds) is greater than or equal to one second.
   */
  public TimeStamp (IntegerMode tsiMode, FractionalMode tsfMode, Long tsi, Long tsf) {
    this(tsiMode, tsfMode, tsi, tsf, LeapSeconds.getDefaultInstance());
  }
  
  /** Creates a new instance.
   *  @param tsiMode The IntegerMode mode used.
   *  @param tsfMode The FractionalMode mode used.
   *  @param tsi     The IntegerMode value.
   *  @param tsf     The FractionalMode value.
   *  @param ls      The leap-seconds reference to use.
   *  @throws NullPointerException if <tt>tsiMode</tt>, <tt>tsfMode</tt>, or <tt>ls</tt> is null.
   *  @throws IllegalArgumentException if <tt>tsi</tt> is less than 0 or greater than 0xFFFFFFFFL;
   *                 or if <tt>tsi</tt> is null and <tt>tsiMode</tt> is not <tt>None</tt>;
   *                 or if <tt>tsf</tt> is null and <tt>tsfMode</tt> is not <tt>None</tt>;
   *                 or if the fractional time stamp mode is <tt>RealTime</tt> and the value
   *                    (in picoseconds) is greater than or equal to one second.
   */
  public TimeStamp (IntegerMode tsiMode, FractionalMode tsfMode, Long tsi, Long tsf, LeapSeconds ls) {
    if (tsiMode == null) throw new NullPointerException("TimeStampInteger is null");
    if (tsfMode == null) throw new NullPointerException("TimeStampFractional is null");
    if (ls      == null) throw new NullPointerException("LeapSeconds is null");
    
    if ((tsi == null) && (tsiMode != IntegerMode.None)) {
      throw new IllegalArgumentException("TimeStampInteger can not be null when mode is "+tsiMode);
    }
    if ((tsf == null) && (tsfMode != FractionalMode.None)) {
      throw new IllegalArgumentException("TimeStampFractional can not be null when mode is "+tsfMode);
    }
    if ((tsiMode != IntegerMode.None) && ((tsi & 0xFFFFFFFF00000000L) != 0)) {
      throw new IllegalArgumentException("Invalid TimeStampInteger value ("+tsi+")");
    }
    if ((tsfMode == FractionalMode.RealTime) && ((tsf < 0L) || (tsf >= ONE_SEC))) {
      throw new IllegalArgumentException("TimeStampInteger value ("+tsf+" picoseconds) is not in "
                                       + "the range [0, "+ONE_SEC+").");
    }
    this.tsiMode = tsiMode;
    this.tsfMode = tsfMode;
    this.tsi     = (tsiMode == IntegerMode.None   )? 0L : tsi;
    this.tsf     = (tsfMode == FractionalMode.None)? 0L : tsf;
    this.ls      = ls;
  }
  
  /** Fast internal constructor. */
  private TimeStamp (LeapSeconds ls, IntegerMode tsiMode, FractionalMode tsfMode, long tsi, long tsf) {
    this.tsiMode = tsiMode;
    this.tsfMode = tsfMode;
    this.tsi     = tsi;
    this.tsf     = tsf;
    this.ls      = ls;
  }


  /** Gets a time stamp using system time. This time should be considered unreliable as it is
      using {@link System#currentTimeMillis()} as its data source rather than a precision time
      reference. The resolution of this time stamp is only as good as that provided by the O/S, and
      will never be better than 1 millisecond.
   */
  public static TimeStamp getSystemTime () {
    return getSystemTime(LeapSeconds.getDefaultInstance());
  }

  /** Gets a time stamp using system time. This time should be considered unreliable as it is
      using {@link System#currentTimeMillis()} as its data source rather than a precision time
      reference. The resolution of this time stamp is only as good as that provided by the O/S,
      and will never be better than 1 millisecond.
      @param ls The leap-seconds reference to use.
   */
  public static TimeStamp getSystemTime (LeapSeconds ls) {
    long ms   = System.currentTimeMillis();  // POSIX time not UTC time
    long s    = ms/1000;                     // POSIX seconds
    long ps   = (ms%1000) * 1000000000;      // picoseconds (POSIX/UTC/GPS)

    return forTimePOSIX(s, ps, ls);
  }
  
  /** Converts from UTC or GPS time to UTC. This operation does nothing to the fractional time
      seconds, and is only valid for dates after 1-Jan-1980.
      @return <tt>this</tt> if already in UTC, otherwise a new TimeStamp in GPS time representing
              the same time.
      @throws UnsupportedOperationException If the current time stamp is not GPS or UTC.
   */
  public TimeStamp toUTC () {
    if (tsiMode == IntegerMode.UTC) return this;
    return new TimeStamp(ls, IntegerMode.UTC, tsfMode, getUTCSeconds(), tsf);
  }
  
  /** Converts from UTC or GPS time to GPS. This operation does nothing to the fractional time
      seconds, and is only valid for dates after 1-Jan-1980.
      @return <tt>this</tt> if already in GPS, otherwise a new TimeStamp in UTC time representing
              the same time.
      @throws UnsupportedOperationException If the current time stamp is not GPS or UTC.
   */
  public TimeStamp toGPS () {
    if (tsiMode == IntegerMode.GPS) return this;
    return new TimeStamp(ls, IntegerMode.GPS, tsfMode, getGPSSeconds(), tsf);
  }

  /** Adds the specified number of seconds to the current time and returns the resulting time.
      This is identical to <tt>addTime(sec,0L)</tt> except that it will not throw an exception if
      {@link #getFractionalMode()} is not {@link FractionalMode#RealTime}.
      @param sec The number of seconds.
      @return A new time object representing the resulting time.
      @throws UnsupportedOperationException If the {@link #getIntegerMode() } is not
              {@link IntegerMode#UTC} or {@link IntegerMode#GPS} .
   */
  public TimeStamp addSeconds (long sec) {
    return addTime(sec, 0L, false);
  }

  /** Adds the specified number of picoseconds to the current time and returns the resulting time.
      This is identical to <tt>addTime(0L,ps)</tt>.
      @param ps The number of picoseconds.
      @return A new time object representing the resulting time.
      @throws UnsupportedOperationException If the {@link #getIntegerMode() } is not
              {@link IntegerMode#UTC} or {@link IntegerMode#GPS} or if {@link #getFractionalMode()}
              is not {@link FractionalMode#RealTime}.
   */
  public TimeStamp addPicoSeconds (long ps) {
    return addTime(0L, ps, true);
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
      @throws UnsupportedOperationException If the {@link #getIntegerMode() } is not
              {@link IntegerMode#UTC} or {@link IntegerMode#GPS} or if {@link #getFractionalMode()}
              is not {@link FractionalMode#RealTime}.
   */
  public TimeStamp addTime (long sec, long ps) {
    return addTime(sec, ps, true);
  }

  /** Internal method, flags if fractional is supported, if fract=false, ps must be 0L. */
  private TimeStamp addTime (long sec, long ps, boolean fract) {
    if ((tsiMode != IntegerMode.UTC) && (tsiMode != IntegerMode.GPS)) {
      throw new UnsupportedOperationException("Can not add seconds to "+this);
    }
    else if (fract && (tsfMode != FractionalMode.RealTime)) {
      throw new UnsupportedOperationException("Can not add fractional seconds to "+this);
      }
    else if ((sec == 0L) && (ps == 0L)) {
      return this; // no change
      }
    else if (ps == 0L) {
      return new TimeStamp(ls, tsiMode, tsfMode, tsi+sec, tsf);
      }
    else {
      // STEP 1: Add in the seconds plus seconds and picoseconds plus picoseconds, taking
      //         care to take any whole seconds in ps and move them to seconds
      long s    = ps / ONE_SEC;
      long _sec = tsi + (sec + s);
      long _ps  = tsf + (ps  - (s * ONE_SEC));

      // STEP 2: There is a good chance _ps has exceeded +/- ONE_SEC, so fix it
      s    = _ps / ONE_SEC;
      _sec = _sec + s;
      _ps  = _ps  - (s * ONE_SEC);
      
      // STEP 3: If _ps is negative, make it positive (i.e. -0.4 = -1.0 + 0.6)
      if (_ps < 0) {
        _sec = _sec - 1;
        _ps  = _ps  + ONE_SEC;
      }

      return new TimeStamp(ls, tsiMode, tsfMode, _sec, _ps);
    }
  }

  /** Compares time stamp values. If the time stamps have different epochs, this method will
      attempt to convert both to GPS and will throw an {@link UnsupportedOperationException} if
      unable to do so. If the fractional time stamp values have a different reference, an
      {@link UnsupportedOperationException} will be thrown. This method assumes that any
      "wrap-around" within the fractional time stamp is accounted for in the integral time stamp.
      If two GPS/UTC times are compared, this will return 0 iff they represent exactly the same
      instant in time.
      @return <tt>&lt;0</tt> if <tt>this</tt> is comes before than <tt>other</tt>,
              <tt>&eq;0</tt> if <tt>this</tt> is comes at the same point as <tt>other</tt>,
              <tt>&gt;0</tt> if <tt>this</tt> is comes after than <tt>other</tt>.
      @throws UnsupportedOperationException If the current time stamp is not GPS or UTC.
      @throws NullPointerException          If either had a null value for integral/fractional time
                                            and the other does not.
   */
  @Override
  public int compareTo (TimeStamp other) {
    // INTEGER
    if ((this.tsiMode == IntegerMode.None) || (other.tsiMode == IntegerMode.None)) {
      if (this.tsiMode != other.tsiMode) {
        throw new UnsupportedOperationException("Can not compare "+this+" to "+other);
      }
    }
    else if (this.tsiMode == other.tsiMode) {
      if (this.tsi < other.tsi) return -1;
      if (this.tsi > other.tsi) return +1;
    }
    else {
      long a = this.getGPSSeconds();
      long b = other.getGPSSeconds();
      if (a < b) return -1;
      if (a > b) return +1;
    }
    
    // FRACTIONAL
    if (this.tsfMode != other.tsfMode) {
      throw new UnsupportedOperationException("Can not compare "+this+" to "+other);
    }
    else {
      if (this.tsf < other.tsf) return -1;
      if (this.tsf > other.tsf) return +1;
      return 0;
    }
  }

  /** Equals method that checks for strict equality. Two time stamps representing the same point
      but using different epoch (UTC vs GPS) will not be considered equal.
      @param o The object to compare to this one.
      @return <tt>true</tt> if exactly equal, <tt>false</tt> otherwise.
   */
  @Override
  public boolean equals (Object o) {
    if (null == o) return false;
    if (this == o) return true;
    if (!(o instanceof TimeStamp)) return false;
    TimeStamp ts = (TimeStamp)o;
    
    return tsiMode.equals(ts.tsiMode)
        && tsfMode.equals(ts.tsfMode)
        && (tsi == ts.tsi)
        && (tsf == ts.tsf)
        && ls.equals(ts.ls);
  }
  
  @Override
  public int hashCode () {
    return tsiMode.hashCode()
         ^ (tsfMode.hashCode() << 2)
         ^ (int)tsi
         ^ (int)(tsf >>> 32)
         ^ (int)tsf
         ^ ls.hashCode();
  }
  
  /** A textural representation of the time stamp. If the integer mode is {@link IntegerMode#GPS}
   *  or {@link IntegerMode#UTC} and fractional mode is {@link FractionalMode#None} or
   *  {@link FractionalMode#RealTime} (i.e. the 90% use case) the output will be an RFC3339-compatible
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
  @Override
  public String toString () {
    if (tsiMode == IntegerMode.None) {
      switch (tsfMode) {
        case None:             return null;
        case SampleCount:      return Long.toString(tsf)              + " (SampleCount)";
        case RealTime:         return "0"+toPicosecondString(tsf, 12) + " (RealTime)";
        case FreeRunningCount: return Long.toString(tsf)              + " (FreeRunningCount)";
      }
    }
    if (tsiMode == IntegerMode.UTC) {
      switch (tsfMode) {
        case None:             return ls.utcToYMDHMS(tsi, -1L)                            + " (UTC)";
        case SampleCount:      return ls.utcToYMDHMS(tsi, -1L) + "," + Long.toString(tsf) + " (UTC,SampleCount)";
        case RealTime:         return ls.utcToYMDHMS(tsi, tsf)                            + " (UTC)";
        case FreeRunningCount: return ls.utcToYMDHMS(tsi, -1L) + "," + Long.toString(tsf) + " (UTC,FreeRunningCount)";
      }
    }
    if (tsiMode == IntegerMode.GPS) {
      switch (tsfMode) {
        case None:             return gpsToYMDHMS(tsi, -1L)                            + " (GPS)";
        case SampleCount:      return gpsToYMDHMS(tsi, -1L) + "," + Long.toString(tsf) + " (GPS,SampleCount)";
        case RealTime:         return gpsToYMDHMS(tsi, tsf)                            + " (GPS)";
        case FreeRunningCount: return gpsToYMDHMS(tsi, -1L) + "," + Long.toString(tsf) + " (GPS,FreeRunningCount)";
      }
    }
    if (tsiMode == IntegerMode.Other) {
      switch (tsfMode) {
        case None:             return Long.toString(tsi)                                         + " (Other)";
        case SampleCount:      return Long.toString(tsi) + "," + Long.toString(tsf)              + " (Other,SampleCount)";
        case RealTime:         return Long.toString(tsi) + "," + "0"+toPicosecondString(tsf, 12) + " (Other,RealTime)";
        case FreeRunningCount: return Long.toString(tsi) + "," + Long.toString(tsf)              + " (Other,FreeRunningCount)";
      }
    }
    throw new AssertionError("Invalid mode combination "+tsiMode+", "+tsfMode); // should be impossible
  }

  /** A textural representation of the time stamp in UTC. This is identical to the time stamp
   *  produced by <tt>ts.toUTC().toString()</tt> except that the " (UTC)" suffix is omitted.
   *  @return A textural representation of the time stamp, or null if both IntegerMode and
   *          FractionalMode are None.
   */
  public String toStringUTC () {
    TimeStamp ts = toUTC();
    return (ts.tsfMode == FractionalMode.None)? ts.ls.utcToYMDHMS(ts.tsi, -1L).toString()
                                              : ts.ls.utcToYMDHMS(ts.tsi, ts.tsf).toString();
  }

  /** A textural representation of the time stamp in UTC. This is identical to the time stamp
   *  produced by <tt>ts.toGPS().toString()</tt> except that the " (GPS)" suffix is omitted.
   *  @return A textural representation of the time stamp, or null if both IntegerMode and
   *          FractionalMode are None.
   */
  public String toStringGPS () {
    TimeStamp ts = toGPS();
    return (ts.tsfMode == FractionalMode.None)? gpsToYMDHMS(ts.tsi, -1L)
                                              : gpsToYMDHMS(ts.tsi, ts.tsf);
  }

  /** Gets the epoch used.
   *  @return {@link #GPS_EPOCH} or {@link #UTC_EPOCH} or null if neither apply. This will return
   *          null any time {@link FractionalMode#SampleCount} or {@link FractionalMode#FreeRunningCount}
   *          is in use.
   *  @see #getIntegerMode()
   *  @see #getFractionalMode()
   */
  public IntegerMode getEpoch () {
    if (tsiMode == GPS_EPOCH) {
      if (tsfMode == FractionalMode.RealTime) return GPS_EPOCH;
      if (tsfMode == FractionalMode.None    ) return GPS_EPOCH;
    }
    else if (tsiMode == UTC_EPOCH) {
      if (tsfMode == FractionalMode.RealTime) return UTC_EPOCH;
      if (tsfMode == FractionalMode.None    ) return UTC_EPOCH;
    }
    return null;
  }

  /** Gets the time stamp mode used for integral seconds.
   *  @see #getTimeStampInteger()
   */
  public IntegerMode getIntegerMode () {
    return tsiMode;
  }
  
  /** Gets the time stamp mode used for fractional seconds.
   *  @see #getTimeStampFractional()
   */
  public FractionalMode getFractionalMode () {
    return tsfMode;
  }

  /** Gets the integer time stamp in UTC seconds (if GPS it converts to UTC first).
   *  @return The integer time stamp. This will be an unsigned 32-bit value as an <tt>int</tt>.
   *  @throws UnsupportedOperationException If the time stamp is not GPS or UTC.
   *  @see #getUTCSeconds()
   */
  public int getSecondsUTC () {
    return (int)getUTCSeconds();
  }

  /** Gets the integer time stamp in GPS seconds (if UTC it converts to GPS first).
   *  @return The integer time stamp. This will be an unsigned 32-bit value as an <tt>int</tt>.
   *  @throws UnsupportedOperationException If the time stamp is not GPS or UTC.
   *  @see #getGPSSeconds()
   */
  public int getSecondsGPS () {
    return (int)getGPSSeconds();
  }

  /** Gets the integer time stamp in NORAD seconds (if GPS it converts to UTC first then from
   *  UTC to NORAD).
   *  @return The integer time stamp. This will be an unsigned 32-bit value as an <tt>int</tt>.
   *  @throws UnsupportedOperationException If the time stamp is not GPS or UTC.
   *  @see #getNORADSeconds()
   */
  public int getSecondsNORAD () {
    return (int)getNORADSeconds();
  }
  
  /** Gets the integer time stamp in NORAD seconds (if GPS it converts to UTC first then from
   *  UTC to NORAD).
   *  @param leapCounted Were any mid-year leap seconds counted (true) or skipped (false)? If this
   *                     is null, the value of {@link VRTConfig#NORAD_LEAP_SEC_COUNTED} will be used.
   *  @return The integer time stamp. This will be an unsigned 32-bit value as an <tt>int</tt>.
   *  @throws UnsupportedOperationException If the time stamp is not GPS or UTC.
   *  @see #getNORADSeconds()
   */
  public int getSecondsNORAD (Boolean leapCounted) {
    return (int)getNORADSeconds(leapCounted);
  }

  /** Gets the integer time stamp in POSIX seconds (if GPS it converts to UTC first then from
   *  UTC to POSIX).
   *  @return The integer time stamp. This will be an unsigned 32-bit value as an <tt>int</tt>.
   *  @throws UnsupportedOperationException If the time stamp is not GPS or UTC.
   *  @see #getNORADSeconds()
   */
  public int getSecondsPOSIX () {
    return (int)getPOSIXSeconds();
  }
  
  /** Gets the integer time stamp in UTC seconds (if GPS it converts to UTC first).
   *  @return The integer time stamp. This will be an unsigned 32-bit value as a <tt>long</tt>.
   *  @throws UnsupportedOperationException If the time stamp is not GPS or UTC.
   *  @see #getSecondsUTC()
   */
  public long getUTCSeconds () {
    if (tsiMode == IntegerMode.UTC) return tsi;
    if (tsiMode != IntegerMode.GPS) throw new UnsupportedOperationException("Can not convert from "+this+" to UTC");

    if (tsi > MAX_GPS2UTC) {
      throw new UnsupportedOperationException("Can not convert from "+this+" to UTC (overflow)");
    }
    return tsi+GPS2UTC;
  }

  /** Gets the integer time stamp in GPS seconds (if UTC it converts to GPS first).
   *  @return The integer time stamp. This will be an unsigned 32-bit value as a <tt>long</tt>.
   *  @throws UnsupportedOperationException If the time stamp is not GPS or UTC.
   *  @see #getSecondsGPS()
   */
  public long getGPSSeconds () {
    if (tsiMode == IntegerMode.GPS) return tsi;
    if (tsiMode != IntegerMode.UTC) throw new UnsupportedOperationException("Can not convert from "+this+" to GPS");

    if (tsi < MIN_UTC2GPS) {
      throw new UnsupportedOperationException("Can not convert from "+this+" to GPS (underflow)");
    }
    return tsi-GPS2UTC;
  }

  /** Gets the integer time stamp in NORAD seconds (if GPS it converts to UTC first then from
   *  UTC to NORAD).
   *  @return The integer time stamp. This will be an unsigned 32-bit value as a <tt>long</tt>.
   *  @throws UnsupportedOperationException If the time stamp is not GPS or UTC.
   *  @see #getSecondsNORAD()
   */
  public long getNORADSeconds () {
    return getNORADSeconds(null);
  }

  /** Gets the integer time stamp in NORAD seconds (if GPS it converts to UTC first then from
   *  UTC to NORAD).
   *  @param leapCounted Were any mid-year leap seconds counted (true) or skipped (false)? If this
   *                     is null, the value of {@link VRTConfig#NORAD_LEAP_SEC_COUNTED} will be used.
   *  @return The integer time stamp. This will be an unsigned 32-bit value as a <tt>long</tt>.
   *  @throws UnsupportedOperationException If the time stamp is not GPS or UTC.
   *  @see #getSecondsNORAD()
   */
  public long getNORADSeconds (Boolean leapCounted) {
    if (leapCounted == null) {
      leapCounted = VRTConfig.NORAD_LEAP_SEC_COUNTED;
    }
    return _getNORADSeconds(leapCounted);
  }

  /** Same as above, but with a boolean rather than a Boolean. */
  private long _getNORADSeconds (boolean leapCounted) {
    long utc       = getUTCSeconds();
    long yearStart = ls.getYiS(utc);

    if (leapCounted) {
      return utc - yearStart;
    }
    else {
      int utcLS       = ls.getLeapSecondsUTC(utc);
      int yearStartLS = ls.getLeapSecondsUTC(yearStart);
      int midYearLS   = utcLS - yearStartLS;

      return utc - yearStart - midYearLS;
    }
  }

  /** Gets the integer time stamp in POSIX seconds (if GPS it converts to UTC first then from
   *  UTC to POSIX).
   *  @return The integer time stamp. This will be an unsigned 32-bit value as a <tt>long</tt>.
   *  @throws UnsupportedOperationException If the time stamp is not GPS or UTC.
   *  @see #getSecondsPOSIX()
   */
  public long getPOSIXSeconds () {
    long utc  = getUTCSeconds();
    int  leap = ls.getLeapSecondsUTC(utc);

    return utc - leap;
  }

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
  public final double getMidasTime () {
    if (getFractionalMode() == FractionalMode.RealTime) {
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
  public final double getMidasSeconds () {
    return getPOSIXSeconds() + MIDAS2POSIX;
  }

  /** Gets the fractional time stamp in picoseconds.
   *  @return The fractional time stamp.
   *  @throws UnsupportedOperationException If the fractional time stamp is not RealTime.
   */
  public long getPicoSeconds () {
    if (tsfMode != FractionalMode.RealTime) {
      throw new UnsupportedOperationException("Can not convert "+tsfMode+" to picoseconds.");
    }
    return tsf;
  }
    
  /** Gets the fractional time stamp as a fraction of a second. This is
   *  identical to the following (done in double-precision):
   *  <pre>
   *    getPicoSeconds() / ONE_SEC
   *  </pre>
   *  @return The fractional time stamp.
   *  @throws VRTException If the fractional time stamp is not RealTime.
   */
  public final double getFractionalSeconds () {
    return ((double)getPicoSeconds()) / ((double)ONE_SEC);
  }
  
  /** Gets the integer time stamp. Be sure to consult {@link #getIntegerMode()} to
   *  find out the type of time stamp used.
   *  @return The integer time stamp (returns null for {@link IntegerMode#None}). This will be
   *          an unsigned 32-bit value.
   */
  public Long getTimeStampInteger () {
    return (tsiMode == IntegerMode.None)? null : tsi;
  }
  
  /** Gets the fractional time stamp. Be sure to consult {@link #getFractionalMode()}
   *  to find out the type of time stamp used.
   *  @return The fractional time stamp (returns null for {@link FractionalMode#None}).
   */
  public Long getTimeStampFractional () {
    return (tsfMode == FractionalMode.None)? null : tsf;
  }
  
  /** Gets the leap-seconds reference used.
   *  @return The leap-seconds reference used.
   */
  public LeapSeconds getLeapSecondRef () {
    return ls;
  }

  /** Parses a time stamp in one of the ISO 8601 formats documented
      {@link #parseTime(String,IntegerMode,LeapSeconds) below}.
      @param time    The time string.
      @param tsiMode The time epoch (must be {@link IntegerMode#GPS} or {@link IntegerMode#UTC}).
      @return The parsed time stamp.
   */
  public static TimeStamp parseTime (String time, IntegerMode tsiMode) {
    return parseTime(time, tsiMode, null);
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
      @param tsiMode The time epoch (must be {@link IntegerMode#GPS} or {@link IntegerMode#UTC}).
      @param ls      The leap seconds instance to use (null to use default).
      @return The parsed time stamp.
   */
  public static TimeStamp parseTime (String time, IntegerMode tsiMode, LeapSeconds ls) {
    int t  = time.indexOf('T');
    int d1 = time.indexOf('-', 1);
    int d2 = time.indexOf('-', d1+2);
    int c1 = time.indexOf(':', t+2);
    int c2 = time.indexOf(':', c1+2);
    int p  = time.indexOf('.', c2+2);
    int tz = time.indexOf('Z', c2+2);

    if (tz < 0) tz = time.indexOf('+', c2);
    if (tz < 0) tz = time.indexOf('-', c2);
    if (tz < 0) tz = time.length();

    if ((t < 0) || (d1 < 0) || (d2 < 0) || (c1 < 0) || (c2 < 0)) {
      throw new IllegalArgumentException("Invalid time format '"+time+"'");
    }

    int  year = parseInt(time.substring(0,d1));
    int  mon  = parseInt(time.substring(d1+1,d2));
    int  day  = parseInt(time.substring(d2+1,t));
    int  hour = parseInt(time.substring(t+1,c1));
    int  min  = parseInt(time.substring(c1+1,c2));
    int  wsec; // set below
    long psec; // set below
    int  zone = 0;
    
    if (p > 0) {
      String fsec = (time.substring(p+1,tz)+"000000000000").substring(0,12);
      psec = parseLong(fsec);
      wsec = parseInt(time.substring(c2+1,p));
    }
    else {
      psec = 0L;
      wsec = parseInt(time.substring(c2+1,tz));
    }

    String z = time.substring(tz);
    if (!z.isEmpty() && !z.equals("Z")) {
      int c = z.indexOf(':');
      if (c < 0) {
        zone = 3600 * parseInt(z.substring(1));
      }
      else {
        zone = 3600 * parseInt(z.substring(1,c)) + 60 * parseInt(z.substring(c+1));
      }
      if (z.startsWith("-")) zone = -zone;
    }
    
    return forTime(year, mon, day, hour, min, wsec, psec, zone, tsiMode, ls);
  }

  /** Converts a time specifier to a time stamp.
      @param year    The year (1981 to 2099).
      @param mon     The month (1 to 12).
      @param day     The day of month (1 to 31).
      @param hour    The hour of day (0 to 23).
      @param min     The minute of hour (0 to 59).
      @param wsec    The whole seconds (0 to 60).get
      @param psec    Any picoseconds.
      @param zone    The time-zone delta in seconds.
      @param tsiMode The time epoch (must be {@link IntegerMode#GPS} or {@link IntegerMode#UTC}).
      @return The time stamp.
   */
  public static TimeStamp forTime (int year, int mon, int day, int hour, int min, int wsec,
                                   long psec, int zone, IntegerMode tsiMode) {
    return forTime(year, mon, day, hour, min, wsec, psec, zone, tsiMode, null);
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
      @param tsiMode The time epoch (must be {@link IntegerMode#GPS} or {@link IntegerMode#UTC}).
      @param ls      The leap seconds instance to use (null to use default).
      @return The time stamp.
   */
  public static TimeStamp forTime (int year, int mon, int day, int hour, int min, int wsec,
                                   long psec, int zone, IntegerMode tsiMode, LeapSeconds ls) {
    if (ls == null) ls = LeapSeconds.getDefaultInstance();
    
    if (tsiMode == IntegerMode.GPS) {
      int  d   = ymdToGpsDay(year, mon, day); // days since 6-Jan-1980
      long tsi = (d * 86400) + (hour * 3600) + (min  * 60) + wsec + zone;
      return new TimeStamp(IntegerMode.GPS, FractionalMode.RealTime, tsi, psec, ls);
    }

    if (tsiMode == IntegerMode.UTC) {
      if (wsec == 60) {
        // Since our POSIX-based math can not take in a value on the leap-second
        // boundary, we force it to use the time immediately before and then do
        // a +1 after creating the TimeStamp object. (This implementation may be
        // slightly inefficient, but since it hits so rarely it shouldn't matter.)
        return forTime(year, mon, day, hour, min, 59, psec, zone, tsiMode, ls).addSeconds(1);
      }
      
      int  d    = ymdToPosixDay(year, mon, day); // days since 1-Jan-1970
      long tsi  = (d * 86400) + (hour * 3600) + (min  * 60) + wsec + zone;
      int  leap = ls.getLeapSecondsPOSIX(tsi);
      return new TimeStamp(IntegerMode.UTC, FractionalMode.RealTime, tsi+leap, psec, ls);
    }
    
    throw new IllegalArgumentException("Time mode "+tsiMode+" not supported");
  }

  /** Creates a time stamp from a POSIX time.  See lengthy discussion at top of
      this class for details regarding POSIX to UTC conversions.
      @param seconds     The number of whole seconds since the POSIX epoch.
      @param picoseconds The number of picoseconds since the start of the current second.
      @return The corresponding TimeStamp.
      @throws IllegalArgumentException if the time corresponds to a value prior to 1972/01/01.
   */
  public static TimeStamp forTimePOSIX (long seconds, long picoseconds) {
    return forTimePOSIX(seconds, picoseconds, null);
  }

  /** Creates a time stamp from a POSIX time. See lengthy discussion at top of
      this class for details regarding POSIX to UTC conversions.
      @param seconds     The number of whole seconds since the POSIX epoch.
      @param picoseconds The number of picoseconds since the start of the current second.
      @param ls          The leap seconds instance to use (null to use default).
      @return The corresponding TimeStamp.
      @throws IllegalArgumentException if the time corresponds to a value prior to 1972/01/01.
   */
  public static TimeStamp forTimePOSIX (long seconds, long picoseconds, LeapSeconds ls) {
    if (ls == null) ls = LeapSeconds.getDefaultInstance();
    int  leap = ls.getLeapSecondsPOSIX(seconds);   // leap seconds to convert POSIX to UTC
    long sec  = seconds + leap;                    // UTC seconds

    return new TimeStamp(ls, IntegerMode.UTC, FractionalMode.RealTime, sec, picoseconds);
  }

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
  public static TimeStamp forTimeMidas (double seconds, double fractionalSeconds) {
    return forTimePOSIX((long)(seconds - MIDAS2POSIX),
                        (long)(ONE_SEC * fractionalSeconds));
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
  public static TimeStamp forTimeMidas (double seconds, double fractionalSeconds, LeapSeconds ls) {
    return forTimePOSIX((long)(seconds - MIDAS2POSIX),
                        (long)(ONE_SEC * fractionalSeconds),
                        ls);
  }

  /** Creates a GPS time stamp from a PTP time. This only supports times after 6 Jan 1980 since
      there is a simple conversion from PTP to GPS time. It is possible to convert from PTP to UTC
      for dates between 1 Jan 1972 (not 1 Jan 1970) and 6 Jan 1980, but it would add significant
      complication to what is otherwise a trivial function that the compiler can easily in-line.
      @param seconds     The number of whole seconds since the PTP epoch.
      @param nanoseconds The number of nanoseconds since the start of the current second.
      @return The corresponding TimeStamp.
      @throws IllegalArgumentException if the time corresponds to a value prior to 1980/01/06.
   */
  public static TimeStamp forTimePTP (long seconds, long nanoseconds) {
    return forTimePTP(seconds, nanoseconds, null);
  }

  /** Creates a GPS time stamp from a PTP time. This only supports times after 6 Jan 1980 since
      there is a simple conversion from PTP to GPS time. It is possible to convert from PTP to UTC
      for dates between 1 Jan 1972 (not 1 Jan 1970) and 6 Jan 1980, but it would add significant
      complication to what is otherwise a trivial function that the compiler can easily in-line.
      @param seconds     The number of whole seconds since the PTP epoch.
      @param nanoseconds The number of nanoseconds since the start of the current second.
      @param ls          The leap seconds instance to use (null to use default).
      @return The corresponding TimeStamp.
      @throws IllegalArgumentException if the time corresponds to a value prior to 1980/01/06.
   */
  public static TimeStamp forTimePTP (long seconds, long nanoseconds, LeapSeconds ls) {
    return new TimeStamp(ls, IntegerMode.GPS, FractionalMode.RealTime, seconds-GPS2PTP, nanoseconds*1000);
  }

  /** Creates a time stamp from a NORAD time. The NORAD time epoch is measured in number of
      1-second intervals elapsed since the start of the current UTC year; however, it may -or- may
      not have counted any mid-year leap seconds. This is the same as
      <tt>forTimeNORAD(seconds, picoseconds, null, -1, null)</tt>
      @param seconds     The number of whole seconds since the start of the year.
      @param picoseconds The number of picoseconds since the start of the current second.
      @return The corresponding TimeStamp.
   */
  public static TimeStamp forTimeNORAD (int seconds, long picoseconds) {
    return forTimeNORAD(seconds, picoseconds, null, -1, null);
  }

  /** Creates a time stamp from a NORAD time. The NORAD time epoch is measured in number of
      1-second intervals elapsed since the start of the current UTC year; however, it may -or- may
      not have counted any mid-year leap seconds. This is the same as
      <tt>forTimeNORAD(seconds, picoseconds, leapCounted, -1, null)</tt>
      @param seconds     The number of whole seconds since the start of the year.
      @param picoseconds The number of picoseconds since the start of the current second.
      @param leapCounted Were any mid-year leap seconds counted (true) or skipped (false)? If this
                         is null, the value of {@link VRTConfig#NORAD_LEAP_SEC_COUNTED} will be used.
      @return The corresponding TimeStamp.
   */
  public static TimeStamp forTimeNORAD (int seconds, long picoseconds, Boolean leapCounted) {
    return forTimeNORAD(seconds, picoseconds, leapCounted, -1, null);
  }

  /** Creates a time stamp from a NORAD time. The NORAD time epoch is measured in number of
      1-second intervals elapsed since the start of the current UTC year; however, there is some
      ambiguity as to whether a mid-year leap second is counted or not counted. This is the same
      as <tt>forTimeNORAD(seconds, picoseconds, null, year, ls)</tt>
      @param seconds     The number of whole seconds since the start of the year.
      @param picoseconds The number of picoseconds since the start of the current second.
      @param year        The year to use (-1 for automatic, otherwise after 1972).
      @param ls          The leap seconds instance to use.
      @return The corresponding TimeStamp.
   */
  public static TimeStamp forTimeNORAD (int seconds, long picoseconds, int year, LeapSeconds ls) {
    return forTimeNORAD(seconds, picoseconds, null, year, ls);
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
                         is null, the value of {@link VRTConfig#NORAD_LEAP_SEC_COUNTED} will be used.
      @param year        The year to use (-1 for automatic, otherwise after 1972).
      @param ls          The leap seconds instance to use (null to use default).
      @return The corresponding TimeStamp.
   */
  public static TimeStamp forTimeNORAD (int seconds, long picoseconds, Boolean leapCounted,
                                        int year, LeapSeconds ls) {
    if (leapCounted == null) {
      leapCounted = VRTConfig.NORAD_LEAP_SEC_COUNTED;
    }
    return _forTimeNORAD(seconds, picoseconds, leapCounted, year, ls);
  }
  
  /** Same as above, but boolean rather than Boolean. */
  private static TimeStamp _forTimeNORAD (int seconds, long picoseconds, boolean leapCounted,
                                          int year, LeapSeconds ls) {
    if (ls == null) ls = LeapSeconds.getDefaultInstance();
    if ((seconds < 0) || (seconds > ONE_YEAR_MAX)) {
      throw new IllegalArgumentException("Illegal NORAD time "+seconds);
    }
    
    if (year < 0) {
      long now   = System.currentTimeMillis() / 1000; // approximate
      int  yr    = ls.getYear(now);                   // approximate
      long start = ls.getStartOfYearPOSIX(yr);
      long soy   = now - start;                       // sec-of-year
      
           if ((soy < FIRST_TWO_WEEKS) && (seconds > LAST_TWO_WEEKS )) year = yr - 1; // End of previous year
      else if ((soy > LAST_TWO_WEEKS ) && (seconds < FIRST_TWO_WEEKS)) year = yr + 1; // Start of next year
      else                                                             year = yr;     // Current year
    }

    if (leapCounted) {
      long sec  = ls.getStartOfYearUTC(year) + seconds;
      return new TimeStamp(IntegerMode.UTC, FractionalMode.RealTime, sec, picoseconds);
    }
    else {
      long sec = ls.getStartOfYearPOSIX(year) + seconds;
      return TimeStamp.forTimePOSIX(sec, picoseconds);
    }
  }

  /** Creates a time stamp from an IRIG time. This is the same as
      <tt>forTimeIRIG(seconds,minutes,hours,days,0,null)</tt>
      @param seconds     The number of whole seconds since the start of the minute (0 to 59[+1]).
      @param minutes     The number of whole minutes since the start of the hour   (0 to 59).
      @param hours       The number of whole hours   since the start of the day    (0 to 23).
      @param days        The number of whole days    since the start of the year   (0 to 365[+1])
                         where 1=Jan1, 2=Jan2, ..., 32=Feb1, ...).
      @return The corresponding TimeStamp.
      @throws IllegalArgumentException If an invalid parameter is specified.
   */
  public static TimeStamp forTimeIRIG (int seconds, int minutes, int hours, int days) {
    return forTimeIRIG(seconds,minutes,hours,days,0,null);
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
  public static TimeStamp forTimeIRIG (int seconds, int minutes, int hours, int days, LeapSeconds ls) {
    return forTimeIRIG(seconds,minutes,hours,days,0,ls);
  }

  /** Creates a time stamp from an IRIG time. This is the same as
      <tt>forTimeIRIG(seconds,minutes,hours,days,hundreths,null)</tt>
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
  public static TimeStamp forTimeIRIG (int seconds, int minutes, int hours, int days, int hundreths) {
    return forTimeIRIG(seconds,minutes,hours,days,hundreths,null);
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
      @param ls          The leap seconds instance to use (null to use default).
      @return The corresponding TimeStamp.
      @throws IllegalArgumentException If an invalid parameter is specified.
   */
  public static TimeStamp forTimeIRIG (int seconds, int minutes, int hours, int days, int hundreths,
                                       LeapSeconds ls) {
    if ((seconds == 60) && (hours == 23) && (minutes == 59)) {
      // Extremely rare leap second seen as 61st second on IRIG time
      return forTimeIRIG(59,59,23,days,hundreths).addSeconds(1);
    }
    if ((seconds > 59) || (minutes > 59) || (hours > 23) || (days > 366) || (hundreths > 99) ||
        (seconds <  0) || (minutes <  0) || (hours <  0) || (days <   1) || (hundreths <  0)) {
      throw new IllegalArgumentException("Illegal IRIG time "+days+"::"+hours+":"+minutes+":"
                                         +seconds+"+"+hundreths+"/100");
    }
    int  sec = seconds + minutes*60 + hours*3600 + (days-1)*86400;
    long ps  = hundreths * (ONE_SEC / 100);
    return _forTimeNORAD(sec, ps, false, -1, ls);
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // ENUMS
  //////////////////////////////////////////////////////////////////////////////////////////////////
  
  /** Identifies the time stamp mode used for integral seconds. */
  public static enum IntegerMode {
    /** No integral time code. */    None,
    /** UTC time code.         */    UTC,
    /** GPS time code.         */    GPS,
    /** Other time code.       */    Other;

    /** Single static instance to eliminate array creation/initialization on every use of {@link #values()}. */
    public static final IntegerMode[] VALUES = values();
  }

  /** Identifies the time stamp mode used for fractional seconds. */
  public static enum FractionalMode {
    /** No fractional time code.                    */    None,
    /** Sample count.                               */    SampleCount,
    /** Real-Time time stamp (picosecond accuracy). */    RealTime,
    /** Free-running count.                         */    FreeRunningCount;

    /** Single static instance to eliminate array creation/initialization on every use of {@link #values()}. */
    public static final FractionalMode[] VALUES = values();
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // IMPLEMENT HASFIELDS
  //////////////////////////////////////////////////////////////////////////////////////////////////
  @Override
  public int getFieldCount () {
    return 16;
  }
  
  @Override
  public String getFieldName (int id) {
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
      default: throw new IllegalArgumentException("Invalid field #"+id+" in "+getClass());
    }
  }

  @Override
  public Class<?> getFieldType (int id) {
    switch (id) {
      case  0: return String.class;
      case  1: return String.class;
      case  2: return String.class;
      case  3: return IntegerMode.class;
      case  4: return IntegerMode.class;
      case  5: return FractionalMode.class;
      case  6: return Long.TYPE;
      case  7: return Long.TYPE;
      case  8: return Long.TYPE;
      case  9: return Long.TYPE;
      case 10: return Double.class;
      case 11: return Double.class;
      case 12: return Long.class;
      case 13: return Double.class;
      case 14: return Long.class;
      case 15: return Long.class;
      default: throw new IllegalArgumentException("Invalid field #"+id+" in "+getClass());
    }
  }

  @Override
  public Object getField (int id) {
    switch (id) {
      case  0: return toString();
      case  1: return toStringUTC();
      case  2: return toStringGPS();
      case  3: return getEpoch();
      case  4: return getIntegerMode();
      case  5: return getFractionalMode();
      case  6: return getUTCSeconds();
      case  7: return getGPSSeconds();
      case  8: return getNORADSeconds();
      case  9: return getPOSIXSeconds();
      case 10: return getMidasTime();
      case 11: return getMidasSeconds();
      case 12: return getPicoSeconds();
      case 13: return getFractionalSeconds();
      case 14: return getTimeStampInteger();
      case 15: return getTimeStampFractional();
      default: throw new IllegalArgumentException("Invalid field #"+id+" in "+getClass());
    }
  }

  @Override
  public void setField (int id, Object val) {
    throw new UnsupportedOperationException("TimeStamp is read only");
  }

  @Override
  public final int getFieldID (String name) {
    return Utilities.getFieldID(this, name);
  }

  @Override
  public final Object getFieldByName (String name) {
    return Utilities.getFieldByName(this, name);
  }

  @Override
  public final void setFieldByName (String name, Object val) {
    Utilities.setFieldByName(this, name, val);
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // DEPRECATED FUNCTIONS
  //////////////////////////////////////////////////////////////////////////////////////////////////
  /** @deprecated Use <tt>forTimeNORAD(..)</tt> */
  @Deprecated
  public static TimeStamp forNoradTime (int seconds, long picoseconds) {
    return forTimeNORAD(seconds, picoseconds);
  }

  /** @deprecated Use <tt>forTimeNORAD(..)</tt> */
  @Deprecated
  public static TimeStamp forNoradTime (int seconds, long picoseconds, int year, LeapSeconds ls) {
    return forTimeNORAD(seconds, picoseconds, null, year, ls);
  }
}

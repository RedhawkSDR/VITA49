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

import java.io.InputStream;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;

import static java.lang.Integer.parseInt;
import static java.util.Arrays.binarySearch;

/** Class to deal with leap seconds.
 *  @see TimeStamp
 *  @author 
 */
public final class LeapSeconds {
  /** Day In Month map. Maps the day-of-year to the month that day falls in.
   *  <pre>
   *    NON-LEAP-YEARS:
   *      (<i>day</i> &gt;= dim[0]) && (<i>day</i> &lt; dim[1])  --&gt; JAN
   *      (<i>day</i> &gt;= dim[8]) && (<i>day</i> &lt; dim[9])  --&gt; SEP
   *
   *    LEAP-YEARS:
   *      (<i>day</i> &gt;= dim[0+12]) && (<i>day</i> &lt; dim[1+12])  --&gt; JAN
   *      (<i>day</i> &gt;= dim[8+12]) && (<i>day</i> &lt; dim[9+12])  --&gt; SEP
   *  </pre>
   */
  static final int dim[] = { 0,31,59,90,120,151,181,212,243,273,304,334,
                             0,31,60,91,121,152,182,213,244,274,305,335 };
  
  private static enum Month { XXX, JAN, FEB, MAR, APR, MAY, JUN, JUL, AUG, SEP, OCT, NOV, DEC };
  
  /** @deprecated This constant has moved to <tt>VRTIO</tt> in the accompanying Midas option tree. */
  @Deprecated
  public static final int J1970TOJ1950       = 631152000; // (7305 days) * (86,400 sec/day)

  /** @deprecated This constant has moved to the {@link TimeStamp} class. */
  @Deprecated
  public static final int GPS2UTC            = 315964811; // (3657 days) * (86,400 sec/day) + (~11 leap sec)

  /** Number of leap seconds between UTC and TAI on 1 JAN 1970, rounded to the nearest second.
   *  See {@link TimeStamp} for details.
   */
  static final int UTC2TAI_LS_1970    =         8; // ~= 8.000082

  /** Number of leap seconds between GPS and TAI on 6 JAN 1980.
   *  See {@link TimeStamp} for details.
   */
  static final int GPS2TAI_LS_1980    =        19;
  
  private static final double[] PRE_1972 = {
    // START DATE    CONSTANT  OFFSET      SCALE
    -3287 * 86400.0, 1.422818, 3.471552E8, 0.001296,
    -3075 * 86400.0, 1.372818, 3.471552E8, 0.001296,
    -2922 * 86400.0, 1.845858, 3.786912E8, 0.0011232,
    -2253 * 86400.0, 1.945858, 3.786912E8, 0.0011232,
    -2192 * 86400.0, 3.24013,  4.733856E8, 0.001296,
    -2101 * 86400.0, 3.34013,  4.733856E8, 0.001296,
    -1948 * 86400.0, 3.44013,  4.733856E8, 0.001296,
    -1826 * 86400.0, 3.54013,  4.733856E8, 0.001296,
    -1767 * 86400.0, 3.64013,  4.733856E8, 0.001296,
    -1645 * 86400.0, 3.74013,  4.733856E8, 0.001296,
    -1583 * 86400.0, 3.84013,  4.733856E8, 0.001296,
    -1461 * 86400.0, 4.31317,  5.049216E8, 0.002592,
     -700 * 86400.0, 4.21317,  5.049216E8, 0.002592,
  };
  
  private static final int PRE_1972_LENGTH = PRE_1972.length / 4;
  
  /** The first lines that should appear in the file. This serves as a sanity check for the
   *  accuracy of the file.
   */
  private static final String[] FIRST_LINES = {
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
  
  private static LeapSeconds defaultInstance = null;
  
  private final long[] startDatePOSIX;
  private final long[] startDateUTC;
  private final int[]  leapSeconds;
  private       long[] yearStartPOSIX;
  private       long[] yearStartUTC;
  
  /** Creates a new instance. */
  private LeapSeconds (long[] wsec, int[] ls) {
    this.startDatePOSIX = new long[wsec.length];
    this.startDateUTC   = new long[wsec.length];
    this.leapSeconds    = ls;
    this.yearStartPOSIX = new long[2106-1972]; // other stuff breaks in 2106, so don't go beyond there.
    this.yearStartUTC   = new long[2106-1972]; // other stuff breaks in 2106, so don't go beyond there.
    
    for (int i = 0; i < wsec.length; i++) {
      startDatePOSIX[i] = (int)(wsec[i]);
      startDateUTC[i]   = (int)(startDatePOSIX[i] + leapSeconds[i]);
    }
    
    int  year  = 1972;
    long posix = 365 * 2 * 86400L; // start in 1972
    for (int i = 0; i < yearStartUTC.length; i++) {
      yearStartPOSIX[i] = posix;
      yearStartUTC[i]   = posix + getLeapSecondsPOSIX(posix);
      
      posix += (isLeapYear(year))? (366 * 86400L)
                                 : (365 * 86400L);
      year  += 1;
    }
  }
  
  /** Gets the default instance. This will return the instance previously set with
   *  <tt>setDefaultInstance(..)</tt>; if no default instance has yet been set, the
   *  {@link VRTConfig#LEAP_SECONDS_FILE} will be used as the default.
   *  @return An using the give file.
   */
  public static synchronized LeapSeconds getDefaultInstance () {
    if (defaultInstance == null) {
      try {
        InputStream in = LeapSeconds.class.getClassLoader().getResourceAsStream(VRTConfig.LEAP_SECONDS_FILE);
        defaultInstance = getInstance(in);
      }
      catch (IOException e) {
        throw new RuntimeException("Unable to initialize default instance.", e);
      }
    }
    return defaultInstance;
  }
  
  /** Sets the default instance to use.
   *  @param def The default instance to use.
   */
  public static synchronized void setDefaultInstance (LeapSeconds def) {
    defaultInstance = def;
  }
  
  /** Sets the default instance to use.
   *  @param in Input stream to read from.
   *  @throws IOException if there is an error while reading and parsing the file.
   */
  public static void setDefaultInstance (InputStream in) throws IOException {
    setDefaultInstance(getInstance(in));
  }
  
  /** Sets the default instance to use.
   *  @param in Input stream to read from.
   *  @throws IOException if there is an error while reading and parsing the file.
   */
  public static void setDefaultInstance (BufferedReader in) throws IOException {
    setDefaultInstance(getInstance(in));
  }
  
  /** Gets an instance with the given tai-utc.dat file. On the first call to this method the
   *  file will be opened and read. Subsequent calls may either re-open the file or return the an
   *  existing open copy.
   *  @param in The input stream for the file to read in.
   *  @return An using the give file.
   */
  public static LeapSeconds getInstance (InputStream in) throws IOException {
    return getInstance(new BufferedReader(new InputStreamReader(in)));
  }
  
  /** Gets an instance with the given tai-utc.dat file. On the first call to this method the
   *  file will be opened and read. Subsequent calls may either re-open the file or return the an
   *  existing open copy.
   *  @param in The buffered reader for the file to read in.
   *  @return An using the give file.
   */
  public static LeapSeconds getInstance (BufferedReader in) throws IOException {
    List<String> lines = new ArrayList<String>();
    String line;
    
    while ((line = in.readLine()) != null) {
      lines.add(line);
    }
    
    if (lines.size() < FIRST_LINES.length) {
      throw new IOException("TAI to UTC mapping file appears to be out of date or invalid.");
    }
    
    long[] sd = new long[lines.size() - PRE_1972_LENGTH];
    int[]  ls = new int [lines.size() - PRE_1972_LENGTH];
    
    for (int i = 0; i < lines.size(); i++) {
      line = lines.get(i);
      
      if ((i < FIRST_LINES.length) && !line.equals(FIRST_LINES[i])) {
        throw new IllegalArgumentException("TAI to UTC mapping file appears invalid: On line "+(i+1)
                                         + " got '"+line+"' but expected '"+FIRST_LINES[i]+"'.");
      }
      
      if (i >= PRE_1972_LENGTH) {
        int year = parseInt(line.substring(1, 5));
        int mon  = Month.valueOf(line.substring(6, 9)).ordinal();
        int day  = parseInt(line.substring(10, 12).trim());
        
        sd[i-PRE_1972_LENGTH] = ymdToPosixDay(year, mon, day) * 86400L; // POSIX date
        ls[i-PRE_1972_LENGTH] = parseInt(line.substring(38, 40)) - UTC2TAI_LS_1970;
      }
    }
    return new LeapSeconds(sd, ls);
  }
  
  /** Calculate and return the difference between TAI (International Atomic Time) and UTC
   *  (Universal Coordinated Time) for the given time. Generally, this will be the number
   *  of leap seconds in effect at that time. Between 1961/01/01 and 1972/01/01 this was a
   *  sliding value with fractional offset; since 1972/01/01 this is an integral number of
   *  seconds. <br>
   *  <br>
   *  @param wsec The number of whole seconds in Midas time referenced to UTC.
   *  @param fsec The number of fractional seconds (ignored after 1972/01/01).
   *  @return The difference between TAI and UTC (i.e. TAI - UTC) in seconds.
   *  @throws IllegalArgumentException If the date is before 1961/01/01.
   */
  public double getLeapSeconds (double wsec, double fsec) {
    int posix = (int)(wsec - J1970TOJ1950);
    
    if (posix >= startDatePOSIX[0]) {
      return getLeapSecondsPOSIX(posix);
    }
    if (posix == 0) {
      return 0.0; // common special-case where time is zero
    }
    
    for (int i = (PRE_1972_LENGTH*4)-4; i >= 0; i-=4) {
      if (PRE_1972[i] > wsec) continue;
      //     CONSTANT      + (wsec + fsec - OFFSET       ) * (SCALE         / SecondsPerDay)
      return PRE_1972[i+1] + (wsec + fsec - PRE_1972[i+2]) * (PRE_1972[i+3] / 86400.0      ) - 8.000082;
    }
    
    // This should only hit for dates before 1961
    throw new IllegalArgumentException("Can not convert "+wsec+"+"+fsec+" TAI to UTC, "
                                     + "dates before 1960/01/01 are not supported.");
  }
  
  /** Gets the number of leap seconds elapsed prior to the given time.
   *  @param utc 1-second time tics since 1970/01/01.
   *  @return Leap seconds elapsed during prior to the given time.
   *  @throws IllegalArgumentException if the input time is before 1972/01/01. (Between 1970 and 1972,
   *                                   the number of leap seconds is not-integral.)
   */
  public int getLeapSecondsUTC (long utc) {
    if (utc > 0xFFFFFFFFL) throw new IllegalArgumentException("Input time ("+utc+" POSIX) exceeds max value.");

    if (utc == 0) { // special case (date not yet initialized, use 1970-01-01T00:00:00)
      return 0;
    }
    if (utc >= startDateUTC[startDateUTC.length-1]) { // most common case (after last entry)
      return leapSeconds[startDateUTC.length-1];
    }
    if (utc >= startDateUTC[0]) {
      int i = binarySearch(startDateUTC, utc);
      return (i >= 0)? leapSeconds[i] : leapSeconds[-i - 2];
    }
    throw new IllegalArgumentException("Input time ("+utc+" UTC) is before 1972/01/01.");
  }
  
  /** Gets the number of leap seconds elapsed prior to the given time.
   *  @param posix Number of days elapsed since 1970/01/01 multiplied by 86400, plus the number of
   *               seconds in the current day.
   *  @return Leap seconds elapsed during prior to the given time.
   *  @throws IllegalArgumentException if the input time is before 1972/01/01. (Between 1970 and 1972,
   *                                   the number of leap seconds is non-integral.)
   */
  public int getLeapSecondsPOSIX (long posix) {
    if (posix > 0xFFFFFFFFL) throw new IllegalArgumentException("Input time ("+posix+" POSIX) exceeds max value.");

    if (posix == 0) { // special case (date not yet initialized, use 1970-01-01T00:00:00)
      return 0;
    }
    if (posix > startDatePOSIX[startDatePOSIX.length-1]) { // most common case (after last entry)
      return leapSeconds[startDatePOSIX.length-1];
    }
    if (posix >= startDatePOSIX[0]) {
      int i = binarySearch(startDatePOSIX, posix);
      return (i >= 0)? leapSeconds[i] : leapSeconds[-i - 2];
    }
    throw new IllegalArgumentException("Input time ("+posix+" POSIX) is before 1972/01/01.");
  }

  /** <b>Internal use only:</b> Is the identified UTC time equal to an inserted leap second?
   *  That is, is the time equal to the 86401th second of the day (23:59:60)?
   *  @param utc 1-second time tics since 1970/01/01.
   *  @return true if the time is 23:59:60, false otherwise.
   *  @throws VRTException if the input time is before 1972/01/01.
   */
  boolean isLeapSecond (long utc) {
    if (utc > 0xFFFFFFFFL) throw new IllegalArgumentException("Input time ("+utc+" POSIX) exceeds max value.");

    if ((utc == 0) || (utc >= startDateUTC[startDateUTC.length-1])) {
      // the two most common cases, and they are both false
      return false;
    }
    if (utc >= startDateUTC[0]) {
      int i = binarySearch(startDateUTC, utc+1);
      return (i >= 0);
    }
    throw new IllegalArgumentException("Input time ("+utc+" UTC) is before 1972/01/01.");
  }

  int getYear (long utc) {
    if (utc == 0) return 1970;  // special case (date not yet initialized, use 1970-01-01T00:00:00)
    if (utc < yearStartUTC[0]) throw new IllegalArgumentException("Year look-up with leap seconds not valid before 1972");
    if (utc > 0xFFFFFFFFL ) throw new IllegalArgumentException("Input time ("+utc+" UTC) exceeds max value.");

    int i = binarySearch(yearStartUTC, utc);
    return (i >= 0)? 1972+i : 1972+(-i)-2;
  }

  int getMonth (long utc) {
    if (utc == 0) return 1; // special case (date not yet initialized, use 1970-01-01T00:00:00)
    int year = getYear(utc);

    for (int mon = 2; mon <= 12; mon++) {
      if (utc < getStartOfMonth(year,mon)) return mon-1;
    }
    return 12;
  }
  
  long getStartOfMonth (int year, int mon) {
    long monthStartPOSIX = ymdToPosixDay(year, mon, 1)*86400L;
    long monthStartUTC   = monthStartPOSIX + getLeapSecondsPOSIX(monthStartPOSIX);
    return monthStartUTC;
  }
  
  long getStartOfYearUTC (int year) {
    if (year < 1972) throw new IllegalArgumentException("Year look-up with leap seconds not valid before 1972");
    if (year > 2106) throw new IllegalArgumentException("Year look-up with leap seconds not valid after 2106");
    return yearStartUTC[year - 1972];
  }

  long getStartOfYearPOSIX (int year) {
    if (year < 1972) throw new IllegalArgumentException("Year look-up with leap seconds not valid before 1972");
    if (year > 2106) throw new IllegalArgumentException("Year look-up with leap seconds not valid after 2106");
    return yearStartPOSIX[year - 1972];
  }
  
  long getYiS (long utc) {
    return yearStartUTC[getYear(utc)-1972];
  }
  

  /** Converts a UTC time to Year-Month-Day Hour:Min:Sec.
   *  @param seconds     Whole seconds.
   *  @param picoseconds Picoseconds (-1 to ignore).
   *  @return Year-Month-Day Hour:Min:Sec.
   */
  YMDHMS utcToYMDHMS (long seconds, long picoseconds) {
    YMDHMS ymdhms = new YMDHMS();
    int sec;

    if (isLeapSecond(seconds)) {
      ymdhms.year  = getYear(seconds);
      ymdhms.month = getMonth(seconds);      sec = (int)(seconds - getStartOfMonth(ymdhms.year, ymdhms.month));
      ymdhms.day   = ((sec-1) / 86400) + 1;
      ymdhms.hour  = 23;
      ymdhms.min   = 59;
      ymdhms.sec   = 60;
      ymdhms.psec  = picoseconds;
    }
    else {
      ymdhms.year  = getYear(seconds);
      ymdhms.month = getMonth(seconds);      sec = (int)(seconds - getStartOfMonth(ymdhms.year, ymdhms.month));
      ymdhms.day   = (sec / 86400) + 1;      sec = sec - (ymdhms.day-1) * 86400;
      ymdhms.hour  = sec / 3600;             sec = sec - ymdhms.hour    * 3600;
      ymdhms.min   = sec / 60;               sec = sec - ymdhms.min     * 60;
      ymdhms.sec   = sec;
      ymdhms.psec  = picoseconds;
    }
    return ymdhms;
  }

  /** Converts a GPS time to Year-Month-Day Hour:Min:Sec.
   *  @param seconds     Whole seconds.
   *  @param picoseconds Picoseconds (-1 to ignore).
   *  @return Year-Month-Day Hour:Min:Sec.
   */
  static String gpsToYMDHMS (long seconds, long picoseconds) {
    // JC4: This method doesn't really belong here, I just didn't know where to put it.
    YMDHMS ymdhms = new YMDHMS();

    int days = (int)(seconds / 86400);
    int sec  = (int)(seconds % 86400);
    int year = (int)((days+723199.5)/365.25 + .002);
    int doy  = days - ymdToGpsDay(year, 1, 1);
    int mon  = (isLeapYear(year))? 23: 11;
    while (LeapSeconds.dim[mon] > doy) mon--;

    ymdhms.year  = year;
    ymdhms.month = (mon < 12)? mon+1: mon-11;
    ymdhms.day   = doy - LeapSeconds.dim[mon] + 1;
    ymdhms.hour  = sec / 3600;                       sec = sec - ymdhms.hour * 3600;
    ymdhms.min   = sec / 60;                         sec = sec - ymdhms.min  * 60;
    ymdhms.sec   = sec;
    ymdhms.psec  = picoseconds;

    return ymdhms.toString();
  }

  /** Converts a set of picoseconds to a string in the form ".00000000000".
   *  @param psec Picoseconds.
   *  @param dec  number of decimal places to display (1..12).
   *  @return Year-Month-Day Hour:Min:Sec.
   */
  static String toPicosecondString (long psec, int dec) {
    // JC4: This method doesn't really belong here, I just didn't know where to put it.
    String ps = Long.toString(psec);
    ps = "000000000000".substring(ps.length()) + ps;   // add leading zeros
    return "."+ps.substring(0, dec);
  }

  /** Calculate the number of days (sonce 6-Jan-1980) for the supplied date.
   *  @param year  The year (e.g. 2002).
   *  @param month The month (1 = Jan)
   *  @param day   The day.
   *  @return The Modified Julian Day.
   */
  static int ymdToGpsDay (int year, int month, int day) {
    // Identical to the POSIX version except using 11450 rather than 7793 to account for the
    // additional 3657 days between 1 Jan 1970 and 6 Jan 1980.
    return isLeapYear(year)? ((year-1950)*365 + (year+3)/4 - 11450 + (day + dim[month-1+12] - 1))
                           : ((year-1950)*365 + (year+3)/4 - 11450 + (day + dim[month-1+ 0] - 1));
  }

  /** Calculate the number of days (since 1-Jan-1970) for the supplied date.
   *  @param year  The year (e.g. 2002).
   *  @param month The month (1 = Jan)
   *  @param day   The day.
   *  @return The Modified Julian Day.
   */
  static int ymdToPosixDay (int year, int month, int day) {
    return isLeapYear(year)? ((year-1950)*365 + (year+3)/4 - 7793 + (day + dim[month-1+12] - 1))
                           : ((year-1950)*365 + (year+3)/4 - 7793 + (day + dim[month-1+ 0] - 1));
  }

  /** Indicates if the given year is a leap year. The code for this method was copied from RFC 3339.
   *  <pre>
   *    Source: Klyne, et.al. "RFC 3339 / Date and Time on the Internet: Timestamps."
   *            ITEF, July 2002. http://tools.ietf.org/html/rfc3339 (Retrieved 2007-06-15)
   *  </pre>
   *  @param year The year number.
   *  @return true if it is a leap year, false otherwise.
   */
  static boolean isLeapYear (int year) {
    return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Nested classes
  //////////////////////////////////////////////////////////////////////////////////////////////////

  /** Stores year-month-dayThour:min:sec. */
  static final class YMDHMS {
    int  year   = 2000;
    int  month  = 1;
    int  day    = 1;
    int  hour   = 0;
    int  min    = 0;
    int  sec    = 0;
    long psec   = -1L;

    @Override
    public String toString () {
      return toStringBuilder(12).toString();
    }

    /** Converts YMDHMS to a string.
     *  @param dec Number of decimal places to include (0..12)
     *  @return The string form of the time.
     */
    public StringBuilder toStringBuilder (int dec) {
      StringBuilder str = new StringBuilder();
                                         str.append(year ).append('-');
      if (month < 10) str.append('0');   str.append(month).append('-');
      if (day   < 10) str.append('0');   str.append(day  ).append('T');
      if (hour  < 10) str.append('0');   str.append(hour ).append(':');
      if (min   < 10) str.append('0');   str.append(min  ).append(':');
      if (sec   < 10) str.append('0');   str.append(sec  );

      if ((psec >= 0) && (dec > 0)) {
        str.append(toPicosecondString(psec, dec));
      }
      str.append("Z");
      return str;
    }
  }
}

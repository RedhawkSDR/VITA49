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

import java.util.Arrays;
import java.util.HashSet;

import nxm.vrt.lib.TimeStamp.FractionalMode;
import nxm.vrt.lib.TimeStamp.IntegerMode;

import static nxm.vrt.lib.Utilities.append;
import static nxm.vrt.lib.VRTMath.fromDouble32;
import static nxm.vrt.lib.VRTMath.toDouble32;

/** A generic description of an IF Context Packet.
 *
 *  @author 
 */
public interface ContextPacket extends VRTPacket, HasStateEventBits {

  /** Prepares a packet for resend resetting any non-persistent values and clearing the context
   *  change flags. This is implemented as follows:
   *  <pre>
   *    public void resetForResend (TimeStamp t) {
   *      super.resetForResend(t);  // see {@link VRTPacket#resetForResend(TimeStamp)}
   *      setChangePacket(false);
   *      setOverRangeCount(null);
   *      setOverRange(null);
   *      setDiscontinuious(null);
   *      return true;
   *    }
   *  </pre>
   *  @param t The new time stamp.
   *  @return true
   */
  @Override
  boolean resetForResend (TimeStamp t);

  /** Gets the adjusted time stamp of the packet. This is implemented as follows:
   *  <pre>
   *  public TimeStamp getAdjustedTimeStamp () {
   *    Long adj = getTimeStampAdjustment();
   *    return (adj == null)? getTimeStamp() : getTimeStamp().addPicoSeconds(adj);
   *  }
   *  </pre>
   *  @return The adjusted time stamp of the packet (never null).
   */
  TimeStamp getAdjustedTimeStamp ();

  /** Creates a copy of this packet that is independent of the original. The copy will not be
   *  read only and will not be "direct" (i.e. will not share data with the original). This
   *  method's behavior is undefined if the packet is invalid and may throw an exception in that
   *  event.
   *  @return A copy of this packet.
   */
  @Override
  ContextPacket copy ();

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // IS/GET METHODS
  //////////////////////////////////////////////////////////////////////////////////////////////////
  /** Gets the Context Field Change Indicator.
   *  @return true if <u>anything</u> in the packet has changed since the last context packet,
   *          false if <u>nothing</u> has changed since the last context packet.
   */
  @Override
  boolean isChangePacket ();
  /** Gets the Reference Point Identifier.
   *  @return The point in the system where this context applies (null if not specified).
   */
  Integer getReferencePointIdentifier ();
  /** Gets the Bandwidth of the signal in Hz.
   *  @return The bandwidth (null if not specified).
   */
  Double getBandwidth ();
  /** Gets the IF Reference Frequency of the signal in Hz.
   *  @return The frequency (null if not specified).
   */
  Double getFrequencyIF ();
  /** Gets the RF Reference Frequency of the signal in Hz.
   *  @return The frequency (null if not specified).
   */
  Double getFrequencyRF ();
  /** Gets the RF Reference Frequency Offset of the signal in Hz.
   *  @return The frequency offset (null if not specified).
   */
  Double getFrequencyOffsetRF ();
  /** Gets the IF Band Offset of the signal in Hz.
   *  @return The band offset (null if not specified).
   */
  Double getBandOffsetIF ();
  /** Gets the Reference Level of the signal in dBm.
   *  @return The reference level (null if not specified).
   */
  Float getReferenceLevel ();
  /** Gets the Total Gain of the device in dB. The total gain is the sum of
   *  {@link #getGain1()} and {@link #getGain2()}.
   *  @return The gain (null if not specified).
   */
  Float getGain ();
  /** Gets the Stage 1 Gain of the device in dB. This is the front-end gain of the
   *  system. In cases where a separate Stage 1 and Stage 2 gain is not necessary, this holds
   *  the total gain of the system and Gain 2 is set to zero.
   *  @return The gain (null if not specified).
   */
  Float getGain1 ();
  /** Gets the Stage 2 Gain of the device in dB. This is the back-end gain of the
   *  system. In cases where a separate Stage 1 and Stage 2 gain is not necessary, the Stage 1
   *  gain holds the total gain of the system and this is set to zero.
   *  @return The gain (null if not specified).
   */
  Float getGain2 ();
  /** Gets the Over-Range Count. This is the count of the number of over-range data
   *  samples in the <u>single</u> paired data packet.
   *  @return The count (null if not specified).
   */
  Long getOverRangeCount ();
  /** Gets the Sample Rate in Hz.
   *  @return The sample rate (null if not specified).
   */
  Double getSampleRate ();
  /** Gets the Sample Period (inverse of Sample Rate) in sec. The following
   *  implementation (or one compatible with this implementation) is required:
   *  <pre>
   *    public final Double getSamplePeriod () {
   *      Double sr = getSampleRate();
   *      return (sr == null)? null : 1.0/sr;
   *    }
   *  </pre>
   *  @return The sample period (null if not specified).
   */
  Double getSamplePeriod ();
  /** Gets the Timestamp Adjustment in picoseconds. This is the required time adjustment
   *  between the time the signal was digitized (i.e. the timestamp) and the time at the reference
   *  point.
   *  @return The timestamp adjustment (null if not specified).
   */
  Long getTimeStampAdjustment ();
  /** Gets the Timestamp Calibration Time in seconds. This is the most recent date and
   *  time when the timestamp in the Data and Context packets was known to be correct.
   *  @return The calibration time (null if not specified).
   */
  Integer getTimeStampCalibration ();
  /** Gets the Temperature in degrees Celsius. This is used to convey the temperature of
   *  any component that may affect the described signal.
   *  @return The temperature (null if not specified).
   */
  Float getTemperature ();
  /** Gets the Device Identifier specifying the manufacturer and model of the device
   *  producing this context packet stream.
   *  @return The device identifier as a string (null if not specified).
   */
  String getDeviceID ();
  /** Gets the Device Identifier specifying the manufacturer and model of the device
   *  producing this context packet stream.
   *  @return The device identifier (null if not specified).
   */
  Long getDeviceIdentifier ();
  /** Gets the User-Defined Bits from the State and Event Indicator Bits. <i>The
   *  definition of these bits should be included in the documentation for the relevent packet
   *  class.</i> Unlike other bits of the State and Event Indicator Field, there is no concept
   *  of an undefined-value for user bits if the State and Event Indicator Field is present to
   *  support some other bits. As such, zero will be returned if the State and Event Indicator
   *  Field is absent.
   *  @return true if the flag is set, false if it is not set, zero if the State and Event
   *  Indicator Field is absent.
   */
  byte getUserDefinedBits ();
  /** Gets the Data Packet Payload Format. This specifies the format of the data in the
   *  paired data packet stream.
   *  @return The payload format (null if not specified). <i>Note that changes to the returned
   *          object do not alter the packet.</i>
   */
  PayloadFormat getDataPayloadFormat ();
  /** Gets the Formatted GPS (Global Positioning System) Geolocation for the collector.
   *  @return The geolocation information (null if not specified). <i>Note that changes to the
   *          returned object do not alter the packet.</i>
   */
  Geolocation getGeolocationGPS ();
  /** Gets the Formatted INS (Inertial Navigation System) Geolocation for the collector.
   *  @return The geolocation information (null if not specified). <i>Note that changes to the
   *          returned object do not alter the packet.</i>
   */
  Geolocation getGeolocationINS ();
  /** Gets the ECEF (Earth-Centered, Earth-Fixed) Ephemeris for the collector. This will also
   *  get any "ECEF (Earth-Centered, Earth-Fixed) Ephemeris Adjunct" information from a
   *  VITA-49.0b packet (where VITA-49.0b is used).
   *  @return The ephemeris information (null if not specified). <i>Note that changes to the
   *          returned object do not alter the packet.</i>
   */
  Ephemeris getEphemerisECEF ();
  /** Gets the Relative Ephemeris for the collector. Unlike {@link #getEphemerisECEF()}
   *  which uses the ECEF coordinate system, this ephemeris is relative to a user-defined system
   *  specified by {@link #getEphemerisReference()}. This will also get any "Relative Ephemeris
   *  Adjunct" information from a VITA-49.0b packet (where VITA-49.0b is used).
   *  @return The ephemeris information (null if not specified). <i>Note that changes to the
   *          returned object do not alter the packet.</i>
   */
  Ephemeris getEphemerisRelative ();
  /** Gets the Ephemeris Reference Identifier. This is used specifies the process
   *  whose origin applies to the ephemeris returned by {@link #getEphemerisRelative()}.
   *  @return The ephemeris information (null if not specified). <i>Note that changes to the
   *          returned object do not alter the packet.</i>
   */
  Integer getEphemerisReference ();
  /** Gets the GPS ASCII "sentences". These are ASCII "sentences" from a GPS (Global
   *  Positioning System) receiver, such as those conforming to NMEA-0183. <i>Due to the great
   *  variety in GPS "sentences", this class makes little effort to do anything useful with them.
   *  Users are encouraged to use {@link #getGeolocationGPS()} which essentially provides the same
   *  information, but in a consistent form.</i>
   *  @return The geolocation information (null if not specified). <i>Note that changes to the
   *          returned object do not alter the packet.</i>
   */
  GeoSentences getGeoSentences ();
  /** Gets the Context Association Lists. These lists indicate the other context/data
   *  streams associated with this one.
   *  @return The context association lists (null if not specified). <i>Note that changes to the
   *          returned object do not alter the packet.</i>
   */
  ContextAssocLists getContextAssocLists ();
  
  //////////////////////////////////////////////////////////////////////////////////////////////////
  // SET METHODS
  //////////////////////////////////////////////////////////////////////////////////////////////////
  /** Sets the Context Field Change Indicator.
   *  @param change true if <u>anything</u> in the packet has changed since the last context packet,
   *                false if <u>nothing</u> has changed since the last context packet.
   */
  void setChangePacket (boolean change);
  /** Sets the Reference Point Identifier.
   *  @param val The point in the system where this context applies (null if not specified).
   */
  void setReferencePointIdentifier (Integer val);
  /** Sets the Bandwidth of the signal in Hz.
   *  @param val The bandwidth (null if not specified).
   */
  void setBandwidth (Double val);
  /** Sets the IF Reference Frequency of the signal in Hz.
   *  @param val The frequency (null if not specified).
   */
  void setFrequencyIF (Double val);
  /** Sets the RF Reference Frequency of the signal in Hz.
   *  @param val The frequency (null if not specified).
   */
  void setFrequencyRF (Double val);
  /** Sets the RF Reference Frequency Offset of the signal in Hz.
   *  @param val The frequency offset (null if not specified).
   */
  void setFrequencyOffsetRF (Double val);
  /** Sets the IF Band Offset of the signal in Hz.
   *  @param val The band offset (null if not specified).
   */
  void setBandOffsetIF (Double val);
  /** Sets the Reference Level of the signal in dBm.
   *  @param val The reference level (null if not specified).
   */
  void setReferenceLevel (Float val);
  /** Sets the Total Gain of the device in dB. This is effectively the same as
   *  <tt>setGain(val,0)</tt> except that <tt>setGain(null)</tt> will mark both gain values
   *  as unspecified. <i>(Note that it is not possible to specify one of the gain values and
   *  leave the other as unspecified.)</i>
   *  @param val The total gain (null if not specified).
   */
  void setGain (Float val);
  /** Sets the Total Gain of the device in dB.
   *  @param gain1 The stage 1 gain.
   *  @param gain2 The stage 2 gain.
   */
  void setGain (float gain1, float gain2);
  /** Sets the Stage 1 Gain of the device in dB. This is the front-end gain of the
   *  system. In cases where a separate Stage 1 and Stage 2 gain is not necessary, this holds
   *  the total gain of the system and Gain 2 is set to zero. <i>If no stage 2 gain has been
   *  specified, it will be set to 0 following the call to this method.</i>
   *  @param val The gain (null if not specified).
   */
  void setGain1 (float val);
  /** Sets the Stage 2 Gain of the device in dB. This is the back-end gain of the
   *  system. In cases where a separate Stage 1 and Stage 2 gain is not necessary, the Stage 1
   *  gain holds the total gain of the system and this is set to zero. <i>If no stage 1 gain
   *  has been specified, it will be set to 0 following the call to this method.</i>
   *  @param val The gain (null if not specified).
   */
  void setGain2 (float val);
  /** Sets the Over-Range Count. This is the count of the number of over-range data
   *  samples in the <u>single</u> paired data packet.
   *  @param val The count (null if not specified).
   */
  void setOverRangeCount (Long val);
  /** Sets the Sample Rate in Hz.
   *  @param val The sample rate (null if not specified).
   */
  void setSampleRate (Double val);
  /** Sets the Sample Period (inverse of Sample Rate) in sec. The following
   *  implementation (or one compatible with this implementation) is required:
   *  <pre>
   *    public final void setSamplePeriod (Double val) {
   *      Double sr = (val == null)? null : 1.0/val;
   *      setSampleRate(sr);
   *    }
   *  </pre>
   *  @param val The sample period (null if not specified).
   */
  void setSamplePeriod (Double val);
  /** Sets the Timestamp Adjustment in picoseconds. This is the required time adjustment
   *  between the time the signal was digitized (i.e. the timestamp) and the time at the reference
   *  point.
   *  @param val The timestamp adjustment (null if not specified).
   */
  void setTimeStampAdjustment (Long val);
  /** Sets the Timestamp Calibration Time in seconds. This is the most recent date and
   *  time when the timestamp in the Data and Context packets was known to be correct.
   *  @param val The calibration time (null if not specified).
   */
  void setTimeStampCalibration (Integer val);
  /** Sets the Temperature in degrees Celsius. This is used to convey the temperature of
   *  any component that may affect the described signal.
   *  @param val The temperature (null if not specified).
   */
  void setTemperature (Float val);
  /** Sets the Device Identifier specifying the manufacturer and model of the device
   *  producing this context packet stream.
   *  @param val The device identifier as a string (null if not specified).
   */
  void setDeviceID (CharSequence val);
  /** Sets the Device Identifier specifying the manufacturer and model of the device
   *  producing this context packet stream.
   *  @param val The device identifier (null if not specified).
   */
  void setDeviceIdentifier (Long val);
  /** Sets the User-Defined Bits from the State and Event Indicator Bits. <i>The
   *  definition of these bits should be included in the documentation for the relevent packet
   *  class.</i> Unlike other bits of the State and Event Indicator Field, there is no concept
   *  of an undefined-value for user bits if the State and Event Indicator Field is present to
   *  support some other bits. As such, no "null value" is supported for the user-defined buts.
   *  @param val true if the flag is set, false if it is not set.
   */
  void setUserDefinedBits (byte val);
  /** Sets the Data Packet Payload Format. This specifies the format of the data in the
   *  paired data packet stream.
   *  @param val The payload format (null if not specified). <i>Note that changes to the returned
   *             object do not alter the packet.</i>
   */
  void setDataPayloadFormat (PayloadFormat val);
  /** Sets the Formatted GPS (Global Positioning System) Geolocation for the collector.
   *  @param val The geolocation information (null if not specified). <i>Note that changes to the
   *             returned object do not alter the packet.</i>
   */
  void setGeolocationGPS (Geolocation val);
  /** Sets the Formatted INS (Inertial Navigation System) Geolocation for the collector.
   *  @param val The geolocation information (null if not specified). <i>Note that changes to the
   *             returned object do not alter the packet.</i>
   */
  void setGeolocationINS (Geolocation val);
  /** Sets the ECEF (Earth-Centered, Earth-Fixed) Ephemeris for the collector. This will also
   *  set any applicable "ECEF (Earth-Centered, Earth-Fixed) Ephemeris Adjunct" information in a
   *  VITA-49.0b packet (where VITA-49.0b is used). If all of the adjunct information is null it is
   *  omitted (i.e. nothing to report); if strict VITA-49.0 is used the adjunct information is also
   *  omitted (even if non-null) since strict VITA-49.0 does not permit it to be reported.
   *  @param val The ephemeris information (null if not specified). <i>Note that changes to the
   *             returned object do not alter the packet.</i>
   */
  void setEphemerisECEF (Ephemeris val);
  /** Sets the Relative Ephemeris for the collector. Unlike {@link #getEphemerisECEF()}
   *  which uses the ECEF coordinate system, this ephemeris is relative to a user-defined system
   *  specified by {@link #getEphemerisReference()}. This will also set any applicable "Relative
   *  Ephemeris Adjunct" information in a VITA-49.0b packet (where VITA-49.0b is used). If all of
   *  the adjunct information is null it is omitted (i.e. nothing to report); if strict VITA-49.0
   *  is used the adjunct information is also omitted (even if non-null) since strict VITA-49.0
   *  does not permit it to be reported.
   *  @param val The ephemeris information (null if not specified). <i>Note that changes to the
   *             returned object do not alter the packet.</i>
   */
  void setEphemerisRelative (Ephemeris val);
  /** Sets the Ephemeris Reference Identifier. This is used specifies the process
   *  whose origin applies to the ephemeris returned by {@link #getEphemerisRelative()}.
   *  @param val The ephemeris information (null if not specified). <i>Note that changes to the
   *             returned object do not alter the packet.</i>
   */
  void setEphemerisReference (Integer val);
  /** Sets the GPS ASCII "sentences". These are ASCII "sentences" from a GPS (Global
   *  Positioning System) receiver, such as those conforming to NMEA-0183. <i>Due to the great
   *  variety in GPS "sentences", this class makes little effort to do anything useful with them.
   *  Users are encouraged to use {@link #getGeolocationGPS()} which essentially provides the same
   *  information, but in a consistent form.</i>
   *  @param val The geolocation information (null if not specified). <i>Note that changes to the
   *             returned object do not alter the packet.</i>
   */
  void setGeoSentences (GeoSentences val);
  /** Sets the Context Association Lists. These lists indicate the other context/data
   *  streams associated with this one.
   *  @param val The context association lists (null if not specified). <i>Note that changes to the
   *          returned object do not alter the packet.</i>
   */
  void setContextAssocLists (ContextAssocLists val);
  
  
  //////////////////////////////////////////////////////////////////////////////////////////////////
  // NESTED CLASSES
  //////////////////////////////////////////////////////////////////////////////////////////////////
  
  /** Represents a GPS or INS geolocation fix. <i>The fact that {@link GeoSentences}
   *  does not extend this class is not accidental.</i>
   *  @see Geolocation
   *  @see Ephemeris
   *  @author  
   */
  public static abstract class AbstractGeolocation extends Record {
    AbstractGeolocation (int len) {
      super(len);
    }

    @Override
    protected StringBuilder toStringBuilder () {
      StringBuilder str = super.toStringBuilder();
      append(str,  "Manufacturer=", getManufacturerID());
      append(str, " TimeStamp=",    getTimeStamp());
      return str;
    }
    
    /** Gets the manufacturer of the GPS/INS device.
     *  @return The GPS/INS manufacturer OUI.
     */
    public int getManufacturerIdentifier () {
      return unpackInt(0) & 0x00FFFFFF;
    }

    /** Sets the manufacturer of the GPS/INS device.
     *  @param oui The GPS/INS manufacturer OUI.
     *  @throws IllegalArgumentException If the OUI is invalid.
     */
    public void setManufacturerIdentifier (int oui) {
      if ((oui & 0xFF000000) != 0) throw new IllegalArgumentException("Invalid OUI ("+oui+").");
      int val = (unpackByte(0) << 24) | oui;
      packInt(0, val);
    }

    /** Gets the manufacturer of the GPS/INS device.
     *  @return The GPS/INS manufacturer OUI.
     */
    public String getManufacturerID () {
      return Utilities.toStringOUI(getManufacturerIdentifier());
    }

    /** Sets the manufacturer of the GPS/INS device.
     *  @param oui The GPS/INS manufacturer OUI.
     *  @throws IllegalArgumentException If the OUI is invalid.
     *  @throws NullPointerException     If the OUI is null.
     */
    public void setManufacturerID (CharSequence oui) {
      if (oui == null) throw new NullPointerException("OUI can not be null");
      setManufacturerIdentifier(Utilities.fromStringOUI(oui));
    }
    
    /** Gets the time stamp for the geolocation fix.
     *  @return The time stamp for the geolocation fix or {@link TimeStamp#NO_TIME_STAMP} if such
     *          is not supported.
     */
    public TimeStamp getTimeStamp () {
      byte           b       = unpackByte(0);
      IntegerMode    tsiMode = IntegerMode.VALUES[(b >>> 2) & 0x3];
      FractionalMode tsfMode = FractionalMode.VALUES[b & 0x3];
      Long           tsi     = unpackInt(4) & 0xFFFFFFFFL;
      Long           tsf     = unpackLong(8);

      return new TimeStamp(tsiMode, tsfMode, tsi, tsf);
    }
    
    /** Sets the time stamp for the geolocation fix.
     *  @param ts The time stamp for the geolocation fix.
     *  @throws NullPointerException If the time stamp is null.
     */
    protected void setTimeStamp (TimeStamp ts) {
      byte b = (byte)((ts.getIntegerMode().ordinal() << 2) | ts.getFractionalMode().ordinal());
      packByte(0, b);
      
      if (ts.getIntegerMode() == IntegerMode.None) {
        packInt(4, 0xFFFFFFFF);
      }
      else {
        packInt(4, ts.getTimeStampInteger().intValue());
      }
      
      if (ts.getFractionalMode() == FractionalMode.None) {
        packInt( 8, 0xFFFFFFFF);
        packInt(12, 0xFFFFFFFF);
      }
      else {
        packLong(8, ts.getTimeStampFractional().longValue());
      }
    }
  }
  
  /** Represents a GPS or INS geolocation fix.
   *  @see Ephemeris
   *  @see GeoSentences
   *  @author 
   */
  public static final class Geolocation extends AbstractGeolocation {
    /** Creates a new instance. */
    public Geolocation () { super(44); }

    @Override
    protected StringBuilder toStringBuilder () {
      StringBuilder str = super.toStringBuilder();
      append(str, " Latitude=",          getLatitude());
      append(str, " Longitude=",         getLongitude());
      append(str, " Altitude=",          getAltitude());
      append(str, " SpeedOverGround=",   getSpeedOverGround());
      append(str, " HeadingAngle=",      getHeadingAngle());
      append(str, " TrackAngle=",        getTrackAngle());
      append(str, " MagneticVariation=", getMagneticVariation());
      return str;
    }
    
    /** Sets the time stamp for the geolocation fix.
     *  @param ts The time stamp for the geolocation fix.
     *  @throws NullPointerException If the time stamp is null.
     */
    @Override
    public void setTimeStamp (TimeStamp ts) {
      super.setTimeStamp(ts);
    }

    /** Gets the latitude of the fix.
     *  @return The latitude in degrees.
     */
    public Double getLatitude () {
      int bits = unpackInt(16);
      return (bits == 0x7FFFFFFF)? null : toDouble32(22, bits);
    }

    /** Gets the longitude of the fix.
     *  @return The longitude in degrees.
     */
    public Double getLongitude () {
      int bits = unpackInt(20);
      return (bits == 0x7FFFFFFF)? null : toDouble32(22, bits);
    }

    /** Gets the altitude of the fix.
     *  @return The altitude in meters (whether this is MSL or WGS-84 is defined elsewhere).
     */
    public Double getAltitude () {
      int bits = unpackInt(24);
      return (bits == 0x7FFFFFFF)? null : toDouble32(5, bits);
    }

    /** Gets the speed over the ground.
     *  @return The speed over the ground in meters/second.
     */
    public Double getSpeedOverGround () {
      int bits = unpackInt(28);
      return (bits == 0x7FFFFFFF)? null : toDouble32(16, bits);
    }

    /** Gets the heading of the vehicle. The vehicle heading specifies the vehicle orientation.
     *  @return The vehicle heading in degrees.
     */
    public Double getHeadingAngle () {
      int bits = unpackInt(32);
      return (bits == 0x7FFFFFFF)? null : toDouble32(22, bits);
    }

    /** Gets the heading of the vehicle. The track angle specifies the direction of travel.
     *  @return The track angle in degrees.
     */
    public Double getTrackAngle () {
      int bits = unpackInt(36);
      return (bits == 0x7FFFFFFF)? null : toDouble32(22, bits);
    }

    /** Gets the magnetic variation from true north.
     *  @return The magnetic variation in degrees.
     */
    public Double getMagneticVariation () {
      int bits = unpackInt(40);
      return (bits == 0x7FFFFFFF)? null : toDouble32(22, bits);
    }

    /** Gets the latitude of the fix.
     *  @param val val The latitude in degrees.
     */
    public void setLatitude (Double val) {
      int bits = (val == null)? 0x7FFFFFFF : fromDouble32(22, val);
      packInt(16, bits);
    }

    /** Gets the longitude of the fix.
     *  @param val The longitude in degrees.
     */
    public void setLongitude (Double val) {
      int bits = (val == null)? 0x7FFFFFFF : fromDouble32(22, val);
      packInt(20, bits);
    }

    /** Gets the altitude of the fix.
     *  @param val The altitude in meters (whether this is MSL or WGS-84 is defined elsewhere).
     */
    public void setAltitude (Double val) {
      int bits = (val == null)? 0x7FFFFFFF : fromDouble32(5, val);
      packInt(24, bits);
    }

    /** Gets the speed over the ground.
     *  @param val The speed over the ground in meters/second.
     */
    public void setSpeedOverGround (Double val) {   
      int bits = (val == null)? 0x7FFFFFFF : fromDouble32(16, val);
      packInt(28, bits);
    }

    /** Gets the heading of the vehicle. The vehicle heading specifies the vehicle orientation.
     *  @param val The vehicle heading in degrees.
     */
    public void setHeadingAngle (Double val) {
      int bits = (val == null)? 0x7FFFFFFF : fromDouble32(22, val);
      packInt(32, bits);
    }

    /** Gets the heading of the vehicle. The track angle specifies the direction of travel.
     *  @param val The track angle in degrees.
     */
    public void setTrackAngle (Double val) {
      int bits = (val == null)? 0x7FFFFFFF : fromDouble32(22, val);
      packInt(36, bits);
    }

    /** Gets the magnetic variation from true north.
     *  @param val The magnetic variation in degrees.
     */
    public void setMagneticVariation (Double val) {
      int bits = (val == null)? 0x7FFFFFFF : fromDouble32(22, val);
      packInt(40, bits);
    }
  }

  /** Represents an ECEF ephemeris fix. This instance will always be associated with an
   *  {@link EphemerisAdjunct}, but the adjunct will only be initialized with non-null values
   *  if VITA-49.0b is used *and* the adjunct is actually present. It will be written out only
   *  if VITA-49.0b is used and one or more values are non-null. This allows code to be written
   *  that presumes the presence of the adjunct (i.e. assumes VITA-49.0b) but still functions
   *  in situations where it can not be inserted into the packet (i.e. strict VITA-49.0).
   *  @see Geolocation
   *  @see EphemerisAdjunct
   *  @author 
   */
  public static final class Ephemeris extends AbstractGeolocation {
    /** The adjunct record, or null if not present. */
    private final EphemerisAdjunct adjunct;

    /** Creates a new instance. */
    public Ephemeris () {
      super(52);
      this.adjunct = new EphemerisAdjunct();
    }

    @Override
    protected StringBuilder toStringBuilder () {
      StringBuilder str = super.toStringBuilder();
      append(str, " PositionX=",     getPositionX());
      append(str, " PositionY=",     getPositionY());
      append(str, " PositionZ=",     getPositionZ());
      append(str, " AttitudeAlpha=", getAttitudeAlpha());
      append(str, " AttitudeBeta=",  getAttitudeBeta());
      append(str, " AttitudePhi=",   getAttitudePhi());
      append(str, " VelocityX=",     getVelocityX());
      append(str, " VelocityY=",     getVelocityY());
      append(str, " VelocityZ=",     getVelocityZ());
      if (adjunct != null) {
        adjunct.addToStringBuilder(str);
      }
      return str;
    }
    
    /** Gets the reference to the ephemeris adjunct. */
    public EphemerisAdjunct getAdjunct () {
      return adjunct;
    }
    
    /** Sets the time stamp for the geolocation fix.
     *  @param ts The time stamp for the geolocation fix.
     *  @throws NullPointerException If the time stamp is null.
     */
    @Override
    public void setTimeStamp (TimeStamp ts) {
      super.setTimeStamp(ts);
    }

    /** Gets the position along the X-axis in meters.
     *  @return The value of null if not specified.
     */
    public Double getPositionX () {
      int bits = unpackInt(16);
      return (bits == 0x7FFFFFFF)? null : toDouble32(5, bits);
    }

    /** Gets the position along the Y-axis in meters.
     *  @return The value of null if not specified.
     */
    public Double getPositionY () {
      int bits = unpackInt(20);
      return (bits == 0x7FFFFFFF)? null : toDouble32(5, bits);
    }

    /** Gets the position along the Z-axis in meters.
     *  @return The value of null if not specified.
     */
    public Double getPositionZ () {
      int bits = unpackInt(24);
      return (bits == 0x7FFFFFFF)? null : toDouble32(5, bits);
    }

    /** Sets the position along the X-axis in meters.
     *  @param val The value or null if not specified.
     */
    public void setPositionX (Double val) {
      int bits = (val == null)? 0x7FFFFFFF : fromDouble32(5, val);
      packInt(16, bits);
    }

    /** Sets the position along the Y-axis in meters.
     *  @param val The value or null if not specified.
     */
    public void setPositionY (Double val) {
      int bits = (val == null)? 0x7FFFFFFF : fromDouble32(5, val);
      packInt(20, bits);
    }

    /** Sets the position along the Z-axis in meters.
     *  @param val The value or null if not specified.
     */
    public void setPositionZ (Double val) {
      int bits = (val == null)? 0x7FFFFFFF : fromDouble32(5, val);
      packInt(24, bits);
    }

    /** Gets the attitude about the Z-axis in degrees.
     *  @return The value of null if not specified.
     */
    public Double getAttitudeAlpha () {
      int bits = unpackInt(28);
      return (bits == 0x7FFFFFFF)? null : toDouble32(22, bits);
    }

    /** Gets the attitude about the Y-axis in degrees.
     *  @return The value of null if not specified.
     */
    public Double getAttitudeBeta () {
      int bits = unpackInt(32);
      return (bits == 0x7FFFFFFF)? null : toDouble32(22, bits);
    }

    /** Gets the attitude about the X-axis in degrees.
     *  @return The value of null if not specified.
     */
    public Double getAttitudePhi () {
      int bits = unpackInt(36);
      return (bits == 0x7FFFFFFF)? null : toDouble32(22, bits);
    }

    /** Sets the attitude about the Z-axis in degrees.
     *  @param val The value or null if not specified.
     */
    public void setAttitudeAlpha (Double val) {
      int bits = (val == null)? 0x7FFFFFFF : fromDouble32(22, val);
      packInt(28, bits);
    }

    /** Sets the attitude about the Y-axis in degrees.
     *  @param val The value or null if not specified.
     */
    public void setAttitudeBeta (Double val) {
      int bits = (val == null)? 0x7FFFFFFF : fromDouble32(22, val);
      packInt(32, bits);
    }

    /** Sets the attitude about the X-axis in degrees.
     *  @param val The value or null if not specified.
     */
    public void setAttitudePhi (Double val) {
      int bits = (val == null)? 0x7FFFFFFF : fromDouble32(22, val);
      packInt(36, bits);
    }

    /** Gets the velocity along the X-axis in meters per second.
     *  @return The value of null if not specified.
     */
    public Double getVelocityX () {
      int bits = unpackInt(40);
      return (bits == 0x7FFFFFFF)? null : toDouble32(16, bits);
    }
    
    /** Gets the velocity along the Y-axis in meters per second.
     *  @return The value of null if not specified.
     */
    public Double getVelocityY () {
      int bits = unpackInt(44);
      return (bits == 0x7FFFFFFF)? null : toDouble32(16, bits);
    }
    
    /** Gets the velocity along the Z-axis in meters per second.
     *  @return The value of null if not specified.
     */
    public Double getVelocityZ () {
      int bits = unpackInt(48);
      return (bits == 0x7FFFFFFF)? null : toDouble32(16, bits);
    }

    /** Sets the velocity along the X-axis in meters per second.
     *  @param val The value or null if not specified.
     */
    public void setVelocityX (Double val) {
      int bits = (val == null)? 0x7FFFFFFF : fromDouble32(16, val);
      packInt(40, bits);
    }

    /** Sets the velocity along the Y-axis in meters per second.
     *  @param val The value or null if not specified.
     */
    public void setVelocityY (Double val) {
      int bits = (val == null)? 0x7FFFFFFF : fromDouble32(16, val);
      packInt(44, bits);
    }

    /** Sets the velocity along the Z-axis in meters per second.
     *  @param val The value or null if not specified.
     */
    public void setVelocityZ (Double val) {
      int bits = (val == null)? 0x7FFFFFFF : fromDouble32(16, val);
      packInt(48, bits);
    }

    /** Gets the rotational velocity about the Z-axis in degrees per second.
     *  @return The value of null if not specified.
     */
    public Double getRotationalVelocityAlpha () {
      return adjunct.getRotationalVelocityAlpha();
    }

    /** Gets the rotational velocity about the Y-axis in degrees per second.
     *  @return The value of null if not specified.
     */
    public Double getRotationalVelocityBeta () {
      return adjunct.getRotationalVelocityBeta();
    }

    /** Gets the rotational velocity about the X-axis in degrees per second.
     *  @return The value of null if not specified.
     */
    public Double getRotationalVelocityPhi () {
      return adjunct.getRotationalVelocityPhi();
    }

    /** Sets the rotational velocity about the Z-axis in degrees per second.
     *  @param val The value or null if not specified.
     */
    public void setRotationalVelocityAlpha (Double val) {
      adjunct.setRotationalVelocityAlpha(val);
    }

    /** Sets the rotational velocity about the Y-axis in degrees per second.
     *  @param val The value or null if not specified.
     */
    public void setRotationalVelocityBeta (Double val) {
      adjunct.setRotationalVelocityBeta(val);
    }

    /** Sets the rotational velocity about the X-axis in degrees per second.
     *  @param val The value or null if not specified.
     */
    public void setRotationalVelocityPhi (Double val) {
      adjunct.setRotationalVelocityPhi(val);
    }



    /** Gets the acceleration along the X-axis in meters per second<sup>2</sup>.
     *  @return The value of null if not specified.
     */
    public Double getAccelerationX () {
      return adjunct.getAccelerationX();
    }

    /** Gets the acceleration along the Y-axis in meters per second<sup>2</sup>.
     *  @return The value of null if not specified.
     */
    public Double getAccelerationY () {
      return adjunct.getAccelerationY();
    }

    /** Gets the acceleration along the Z-axis in meters per second<sup>2</sup>.
     *  @return The value of null if not specified.
     */
    public Double getAccelerationZ () {
      return adjunct.getAccelerationZ();
    }

    /** Sets the acceleration along the X-axis in meters per second<sup>2</sup>.
     *  @param val The value or null if not specified.
     */
    public void setAccelerationX (Double val) {
      adjunct.setAccelerationX(val);
    }

    /** Sets the acceleration along the Y-axis in meters per second<sup>2</sup>.
     *  @param val The value or null if not specified.
     */
    public void setAccelerationY (Double val) {
      adjunct.setAccelerationY(val);
    }

    /** Sets the acceleration along the Z-axis in meters per second<sup>2</sup>.
     *  @param val The value or null if not specified.
     */
    public void setAccelerationZ (Double val) {
      adjunct.setAccelerationZ(val);
    }



    /** Gets the rotational acceleration about the Z-axis in degrees per second<sup>2</sup>.
     *  @return The value of null if not specified.
     */
    public Double getRotationalAccelerationAlpha () {
      return adjunct.getRotationalAccelerationAlpha();
    }

    /** Gets the rotational acceleration about the Y-axis in degrees per second<sup>2</sup>.
     *  @return The value of null if not specified.
     */
    public Double getRotationalAccelerationBeta () {
      return adjunct.getRotationalAccelerationBeta();
    }

    /** Gets the rotational acceleration about the X-axis in degrees per second<sup>2</sup>.
     *  @return The value of null if not specified.
     */
    public Double getRotationalAccelerationPhi () {
      return adjunct.getRotationalAccelerationPhi();
    }

    /** Sets the rotational acceleration about the Z-axis in degrees per second<sup>2</sup>.
     *  @param val The value or null if not specified.
     */
    public void setRotationalAccelerationAlpha (Double val) {
      adjunct.setRotationalAccelerationAlpha(val);
    }

    /** Sets the rotational acceleration about the Y-axis in degrees per second<sup>2</sup>.
     *  @param val The value or null if not specified.
     */
    public void setRotationalAccelerationBeta (Double val) {
      adjunct.setRotationalAccelerationBeta(val);
    }

    /** Sets the rotational acceleration about the X-axis in degrees per second<sup>2</sup>.
     *  @param val The value or null if not specified.
     */
    public void setRotationalAccelerationPhi (Double val) {
      adjunct.setRotationalAccelerationPhi(val);
    }
  }

  /** Represents an Ephemeris Adjunct as used with VITA-49.0b. This class should not be used on
   *  its own, but rather via the {@link Ephemeris} class.
   *  @see Ephemeris
   *  @author 
   */
  public static final class EphemerisAdjunct extends Record {
    /** <b>Internal use only:</b> Creates a new instance. */
    EphemerisAdjunct () {
      super(36);
      // Initialize everything to null to be compatible with cases where no adjunct is present
      packInt( 0, 0x7FFFFFFF);
      packInt( 4, 0x7FFFFFFF);
      packInt( 8, 0x7FFFFFFF);
      packInt(12, 0x7FFFFFFF);
      packInt(16, 0x7FFFFFFF);
      packInt(20, 0x7FFFFFFF);
      packInt(24, 0x7FFFFFFF);
      packInt(28, 0x7FFFFFFF);
      packInt(32, 0x7FFFFFFF);
    }

    /** Is anything set in this record? */
    public boolean isAnythingSet () {
      return (unpackInt( 0) != 0x7FFFFFFF)
          || (unpackInt( 4) != 0x7FFFFFFF)
          || (unpackInt( 8) != 0x7FFFFFFF)
          || (unpackInt(12) != 0x7FFFFFFF)
          || (unpackInt(16) != 0x7FFFFFFF)
          || (unpackInt(20) != 0x7FFFFFFF)
          || (unpackInt(24) != 0x7FFFFFFF)
          || (unpackInt(28) != 0x7FFFFFFF)
          || (unpackInt(32) != 0x7FFFFFFF);
    }

    @Override
    protected StringBuilder toStringBuilder () {
      StringBuilder str = addToStringBuilder(super.toStringBuilder());
      
      if (str.length() > 0) {
        str.deleteCharAt(0); // deletes the leading space
      }
      return str;
    }

    /** Like the normal toStringBuilder(), except that a leading space is prefixed before
        any text and the StringBuilder is re-used.
     */
    StringBuilder addToStringBuilder (StringBuilder str) {
      append(str, " RotationalVelocityAlpha=",     getRotationalVelocityAlpha());
      append(str, " RotationalVelocityBeta=",      getRotationalVelocityBeta());
      append(str, " RotationalVelocityPhi=",       getRotationalVelocityPhi());
      append(str, " AccelerationX=",               getAccelerationX());
      append(str, " AccelerationY=",               getAccelerationY());
      append(str, " AccelerationZ=",               getAccelerationZ());
      append(str, " RotationalAccelerationAlpha=", getRotationalAccelerationAlpha());
      append(str, " RotationalAccelerationBeta=",  getRotationalAccelerationBeta());
      append(str, " RotationalAccelerationPhi=",   getRotationalAccelerationPhi());
      return str;
    }

    /** Gets the rotational velocity about the Z-axis in degrees per second.
     *  @return The value of null if not specified.
     */
    public Double getRotationalVelocityAlpha () {
      int bits = unpackInt(0);
      return (bits == 0x7FFFFFFF)? null : toDouble32(16, bits);
    }

    /** Gets the rotational velocity about the Y-axis in degrees per second.
     *  @return The value of null if not specified.
     */
    public Double getRotationalVelocityBeta () {
      int bits = unpackInt(4);
      return (bits == 0x7FFFFFFF)? null : toDouble32(16, bits);
    }

    /** Gets the rotational velocity about the X-axis in degrees per second.
     *  @return The value of null if not specified.
     */
    public Double getRotationalVelocityPhi () {
      int bits = unpackInt(8);
      return (bits == 0x7FFFFFFF)? null : toDouble32(16, bits);
    }

    /** Sets the rotational velocity about the Z-axis in degrees per second.
     *  @param val The value or null if not specified.
     */
    public void setRotationalVelocityAlpha (Double val) {
      int bits = (val == null)? 0x7FFFFFFF : fromDouble32(16, val);
      packInt(0, bits);
    }

    /** Sets the rotational velocity about the Y-axis in degrees per second.
     *  @param val The value or null if not specified.
     */
    public void setRotationalVelocityBeta (Double val) {
      int bits = (val == null)? 0x7FFFFFFF : fromDouble32(16, val);
      packInt(4, bits);
    }

    /** Sets the rotational velocity about the X-axis in degrees per second.
     *  @param val The value or null if not specified.
     */
    public void setRotationalVelocityPhi (Double val) {
      int bits = (val == null)? 0x7FFFFFFF : fromDouble32(16, val);
      packInt(8, bits);
    }



    /** Gets the acceleration along the X-axis in meters per second<sup>2</sup>.
     *  @return The value of null if not specified.
     */
    public Double getAccelerationX () {
      int bits = unpackInt(12);
      return (bits == 0x7FFFFFFF)? null : toDouble32(24, bits);
    }

    /** Gets the acceleration along the Y-axis in meters per second<sup>2</sup>.
     *  @return The value of null if not specified.
     */
    public Double getAccelerationY () {
      int bits = unpackInt(16);
      return (bits == 0x7FFFFFFF)? null : toDouble32(24, bits);
    }

    /** Gets the acceleration along the Z-axis in meters per second<sup>2</sup>.
     *  @return The value of null if not specified.
     */
    public Double getAccelerationZ () {
      int bits = unpackInt(20);
      return (bits == 0x7FFFFFFF)? null : toDouble32(24, bits);
    }

    /** Sets the acceleration along the X-axis in meters per second<sup>2</sup>.
     *  @param val The value or null if not specified.
     */
    public void setAccelerationX (Double val) {
      int bits = (val == null)? 0x7FFFFFFF : fromDouble32(24, val);
      packInt(12, bits);
    }

    /** Sets the acceleration along the Y-axis in meters per second<sup>2</sup>.
     *  @param val The value or null if not specified.
     */
    public void setAccelerationY (Double val) {
      int bits = (val == null)? 0x7FFFFFFF : fromDouble32(24, val);
      packInt(16, bits);
    }

    /** Sets the acceleration along the Z-axis in meters per second<sup>2</sup>.
     *  @param val The value or null if not specified.
     */
    public void setAccelerationZ (Double val) {
      int bits = (val == null)? 0x7FFFFFFF : fromDouble32(24, val);
      packInt(20, bits);
    }

    

    /** Gets the rotational acceleration about the Z-axis in degrees per second<sup>2</sup>.
     *  @return The value of null if not specified.
     */
    public Double getRotationalAccelerationAlpha () {
      int bits = unpackInt(24);
      return (bits == 0x7FFFFFFF)? null : toDouble32(23, bits);
    }

    /** Gets the rotational acceleration about the Y-axis in degrees per second<sup>2</sup>.
     *  @return The value of null if not specified.
     */
    public Double getRotationalAccelerationBeta () {
      int bits = unpackInt(28);
      return (bits == 0x7FFFFFFF)? null : toDouble32(23, bits);
    }

    /** Gets the rotational acceleration about the X-axis in degrees per second<sup>2</sup>.
     *  @return The value of null if not specified.
     */
    public Double getRotationalAccelerationPhi () {
      int bits = unpackInt(32);
      return (bits == 0x7FFFFFFF)? null : toDouble32(23, bits);
    }

    /** Sets the rotational acceleration about the Z-axis in degrees per second<sup>2</sup>.
     *  @param val The value or null if not specified.
     */
    public void setRotationalAccelerationAlpha (Double val) {
      int bits = (val == null)? 0x7FFFFFFF : fromDouble32(23, val);
      packInt(24, bits);
    }

    /** Sets the rotational acceleration about the Y-axis in degrees per second<sup>2</sup>.
     *  @param val The value or null if not specified.
     */
    public void setRotationalAccelerationBeta (Double val) {
      int bits = (val == null)? 0x7FFFFFFF : fromDouble32(23, val);
      packInt(28, bits);
    }

    /** Sets the rotational acceleration about the X-axis in degrees per second<sup>2</sup>.
     *  @param val The value or null if not specified.
     */
    public void setRotationalAccelerationPhi (Double val) {
      int bits = (val == null)? 0x7FFFFFFF : fromDouble32(23, val);
      packInt(32, bits);
    }
  }
  
  /** Represents GPS "sentences" describing a GPS geolocation fix. <br>
   *  <br>
   *  <i>Users are strongly encouraged to consider using the binary form when specifying a
   *  GPS geolocation fix (see {@link Geolocation}). There is no intention of adding any
   *  significant capabilities to this class.</i>
   *  @see Geolocation
   *  @author 
   */
  public static final class GeoSentences extends AbstractGeolocation {
    /** Creates a new instance. */
    public GeoSentences () { super(8); }
    
    @Override
    protected StringBuilder toStringBuilder () {
      return super.toStringBuilder().append(" Sentences=").append(getSentences());
    }

    @Override
    public void writeBytes (byte[] buffer, int offset) {
      // Write Counts
      super.setByteLength(8);
      super.writeBytes(buffer, offset);

      // Size as appropriate
      int numberOfWords = unpackInt(4);
      setByteLength(numberOfWords*4 + 8);

      // Write Data
      super.writeBytes(buffer, offset);
    }
    
    /** Gets the time stamp for the geolocation fix.
     *  @return Since no standard time stamp exists for the sentences, this always returns
     *          {@link TimeStamp#NO_TIME_STAMP}.
     */
    @Override
    public TimeStamp getTimeStamp () {
      return TimeStamp.NO_TIME_STAMP;
    }

    /** Gets the GPS "sentences".
     *  @return The GPS "sentences".
     */
    public AsciiCharSequence getSentences () {
      return unpackAscii(8, getByteLength()-8);
    }

    /** Sets the GPS "sentences".
     *  @param s The GPS "sentences" (should be ASCII).
     *  @throws NullPointerException  If the sentences are null.
     */
    public void setSentences (CharSequence s) {
      if (s == null) throw new NullPointerException("The GPS sentences can not be null");
      int numberOfWords = (s.length() + 3) / 4; // round up to nearest word
      setByteLength(numberOfWords*4 + 8);
      packInt(4, numberOfWords);
      packAscii(8, s, numberOfWords*4);
    }
  }

  /** Specifies a Context Association List. */
  public static final class ContextAssocLists extends Record {
    public ContextAssocLists () { super(8); }
    
    @Override
    protected StringBuilder toStringBuilder () {
      StringBuilder str = super.toStringBuilder();
      str.append( "SourceContext="      ).append(Arrays.toString(getSourceContext()));
      str.append(" SystemContext="      ).append(Arrays.toString(getSystemContext()));
      str.append(" VectorComponent="    ).append(Arrays.toString(getVectorComponent()));
      str.append(" AsynchronousChannel=").append(Arrays.toString(getAsynchronousChannel()));
      if (getAsynchronousChannelTagsPresent()) {
        str.append(" AsynchronousChannelTag=").append(Arrays.toString(getAsynchronousChannelTag()));
      }
      return str;
    }

    @Override
    public void writeBytes (byte[] buffer, int offset) {
      // Write Counts
      super.setByteLength(8);
      super.writeBytes(buffer, offset);

      // Size as appropriate
      updateByteLength(-1);

      // Write Data
      super.writeBytes(buffer, offset);
    }

    /** Updates the byte length. */
    private void updateByteLength (int off) {
      int len = getAsynchronousChannelTagsOffset();
      if (getAsynchronousChannelTagsPresent()) {
        len += 4*getAsynchronousChannelCount();
      }
      setByteLength(len, off);
    }

    /** Gets the number of <b>source</b> context associations. */
    public int getSourceContextCount () { return unpackShort(0) & 0x01FF; }
    /** Gets the number of <b>system</b> context associations. */
    public int getSystemContextCount () { return unpackShort(2) & 0x01FF; }
    /** Gets the number of <b>vector-component</b> context associations. */
    public int getVectorComponentCount () { return unpackShort(4) & 0xFFFF; }
    /** Gets the number of <b>Asynchronous-Channel</b> context associations. */
    public int getAsynchronousChannelCount () { return unpackShort(6) & 0x7FFF; }
    /** Are the <b>Asynchronous-Channel</b> context associations accompanied by <b>Asynchronous-Channel Tags</b>? */
    public boolean getAsynchronousChannelTagsPresent () { return (unpackShort(6) & 0x8000) != 0; }

    private int getSourceContextOffset ()           { return 8; }
    private int getSystemContextOffset ()           { return getSourceContextOffset()       + 4*getSourceContextCount(); }
    private int getVectorComponentOffset ()         { return getSystemContextOffset()       + 4*getSystemContextCount(); }
    private int getAsynchronousChannelOffset ()     { return getVectorComponentOffset()     + 4*getVectorComponentCount(); }
    private int getAsynchronousChannelTagsOffset () { return getAsynchronousChannelOffset() + 4*getAsynchronousChannelCount(); }

    /** Does the resizing of a list. */
    private void resize (int pos, int max, int off, int old, int val) {
      if (old == val) {
        return; // no change
      }
      if ((val < 0) || (val > max)) {
        throw new IllegalArgumentException("Invalid size "+val+" must be between 0 and "+max);
      }

      int oldLen = getByteLength();
      int newLen = oldLen + ((val - old) * 4);
      int offset = off + (Math.min(old, val) * 4);

      setByteLength(newLen, offset);
      packShort(pos, (short)(val & max));
    }

    /** Sets the number of <b>source</b> context associations. */
    public void setSourceContextCount (int val) {
      resize(0, 0x1FF, getSourceContextOffset(), getSourceContextCount(), val);
    }
    /** Sets the number of <b>system</b> context associations. */
    public void setSystemContextCount (int val) {
      resize(2, 0x1FF, getSystemContextOffset(), getSystemContextCount(), val);
    }
    /** Sets the number of <b>vector-component</b> context associations. */
    public void setVectorComponentCount (int val) {
      resize(4, 0xFFFF, getVectorComponentOffset(), getVectorComponentCount(), val);
    }
    /** Sets the number of <b>Asynchronous-Channel</b> context associations. */
    public void setAsynchronousChannelCount (int val) {
      resize(6, 0x7FFF, getAsynchronousChannelOffset(), getAsynchronousChannelCount(), val);
      if (getAsynchronousChannelTagsPresent()) {
        resize(6, 0x7FFF, getAsynchronousChannelTagsOffset(), getAsynchronousChannelCount(), val);
      }
    }

    /** Are the <b>Asynchronous-Channel</b> context associations accompanied by <b>Asynchronous-Channel Tags</b>? */
    public void setAsynchronousChannelTagsPresent (boolean val) {
      if (val) packShort(6, (short)(unpackShort(6) | 0x8000)); // insert
      else     packShort(6, (short)(unpackShort(6) & 0x7FFF)); // remove
      updateByteLength(-1);
    }
    
    /** Gets a copy of the Source Context Association List.
     *  @return A copy of the association list (returns an array of length 0 if there are no entries).
     */
    public int[] getSourceContext () {
      int[] val = new int[getSourceContextCount()];
      for (int i=0, j=getSourceContextOffset(); i < val.length; i++,j+=4) {
        val[i] = unpackInt(j);
      }
      return val;
    }
    
    /** Gets a copy of the System Context Association List.
     *  @return A copy of the association list (returns an array of length 0 if there are no entries).
     */
    public int[] getSystemContext () {
      int[] val = new int[getSystemContextCount()];
      for (int i=0, j=getSystemContextOffset(); i < val.length; i++,j+=4) {
        val[i] = unpackInt(j);
      }
      return val;
    }
    
    /** Gets a copy of the Vector-Component Association List.
     *  @return A copy of the association list (returns an array of length 0 if there are no entries).
     */
    public int[] getVectorComponent () {
      int[] val = new int[getVectorComponentCount()];
      for (int i=0, j=getVectorComponentOffset(); i < val.length; i++,j+=4) {
        val[i] = unpackInt(j);
      }
      return val;
    }
    
    /** Gets a copy of the Asynchronous-Channel Association List.
     *  @return A copy of the association list (returns an array of length 0 if there are no entries).
     *  @see #getAsynchronousChannelTag()
     */
    public int[] getAsynchronousChannel () {
      int[] val = new int[getAsynchronousChannelCount()];
      for (int i=0, j=getAsynchronousChannelOffset(); i < val.length; i++,j+=4) {
        val[i] = unpackInt(j);
      }
      return val;
    }
    
    /** Gets a copy of the Asynchronous-Channel Tag List.
     *  @return A copy of the tag list (returns an array of length 0 if there are no entries).
     *          The inclusion of the tag list is optional; if it is omitted, this will return null.
     *  @see #getAsynchronousChannel()
     *  @see #setAsynchronousChannel(int[],int[])
     */
    public int[] getAsynchronousChannelTag () {
      if (!getAsynchronousChannelTagsPresent()) return null;
      int[] val = new int[getAsynchronousChannelCount()];
      for (int i=0, j=getAsynchronousChannelTagsOffset(); i < val.length; i++,j+=4) {
        val[i] = unpackInt(j);
      }
      return val;
    }
    
    /** Sets the Source Context Association List.
     *  @param val The association list (an array of length 0 if there are no entries).
     *  @throws NullPointerException if <tt>list</tt> is null.
     */
    public void setSourceContext (int ... val) {
      checkUniqueIDs(val);
      setSourceContextCount(val.length);
      for (int i=0, j=getSourceContextOffset(); i < val.length; i++,j+=4) {
        packInt(j, val[i]);
      }
    }

    /** Sets the Source Context Association List.
     *  @param val The association list (an array of length 0 if there are no entries).
     *  @throws NullPointerException if <tt>list</tt> is null.
     */
    public void setSourceContext (VRTPacket ... val) {
      setSourceContext(getIDList(val));
    }
    
    /** Sets the System Context Association List.
     *  @param val The association list (an array of length 0 if there are no entries).
     *  @throws NullPointerException if <tt>list</tt> is null.
     */
    public void setSystemContext (int ... val) {
      checkUniqueIDs(val);
      setSystemContextCount(val.length);
      for (int i=0, j=getSystemContextOffset(); i < val.length; i++,j+=4) {
        packInt(j, val[i]);
      }
    }

    /** Sets the System Context Association List.
     *  @param val The association list (an array of length 0 if there are no entries).
     *  @throws NullPointerException if <tt>list</tt> is null.
     */
    public void setSystemContext (VRTPacket ... val) {
      setSystemContext(getIDList(val));
    }
    
    /** Sets the Vector-Component Association List.
     *  @param val The association list (an array of length 0 if there are no entries).
     *  @throws NullPointerException if <tt>list</tt> is null.
     */
    public void setVectorComponent (int ... val) {
      checkUniqueIDs(val);
      setVectorComponentCount(val.length);
      for (int i=0, j=getVectorComponentOffset(); i < val.length; i++,j+=4) {
        packInt(j, val[i]);
      }
    }

    /** Sets the Vector-Component Association List.
     *  @param val The association list (an array of length 0 if there are no entries).
     *  @throws NullPointerException if <tt>list</tt> is null.
     */
    public void setVectorComponent (VRTPacket ... val) {
      setVectorComponent(getIDList(val));
    }
    
    /** Sets the Asynchronous-Channel Association List. This is identical to
     *  <tt>setAsynchronousChannel(list,null)</tt>.
     *  @param val The association list (an array of length 0 if there are no entries).
     *  @throws NullPointerException if <tt>list</tt> is null.
     */
    public final void setAsynchronousChannel (int ... val) {
      setAsynchronousChannel(val,null);
    }

    /** Sets the Asynchronous-Channel Association List. This is identical to
     *  <tt>setAsynchronousChannel(list,null)</tt>.
     *  @param val The association list (an array of length 0 if there are no entries).
     *  @throws NullPointerException if <tt>list</tt> is null.
     */
    public final void setAsynchronousChannel (VRTPacket ... val) {
      setAsynchronousChannel(getIDList(val),null);
    }
    
    /** Sets the Asynchronous-Channel Association List.
     *  @param val  The association list (an array of length 0 if there are no entries).
     *  @param tags The tag list (an array of length 0 if there are no entries or null if not
     *              specified). The inclusion of the tag list is optional; if it is included, it
     *              must have the same length as <tt>list</tt>.
     *  @throws NullPointerException if <tt>list</tt> is null.
     *  @throws IllegalArgumentException if <tt>tags</tt> is given but has an invalid length.
     *  @see #getAsynchronousChannelTag()
     */
    public void setAsynchronousChannel (int[] val, int[] tags) {
      if ((tags != null) && (tags.length != val.length)) {
        throw new IllegalArgumentException("Given number of tags ("+tags.length+") must match the "
                                         + "given number of association list entries ("+val.length+")");
      }
      checkUniqueIDs(val);
      setAsynchronousChannelCount(val.length);
      setAsynchronousChannelTagsPresent(tags != null);
      for (int i=0, j=getAsynchronousChannelOffset(); i < val.length; i++,j+=4) {
        packInt(j, val[i]);
      }
      if (tags != null) {
        for (int i=0, j=getAsynchronousChannelTagsOffset(); i < val.length; i++,j+=4) {
          packInt(j, tags[i]);
        }
      }
    }

    /** Gets the IDs from the packets. */
    private int[] getIDList (VRTPacket[] packets) {
      int[] ids = new int[packets.length];
      for (int i = 0; i < packets.length; i++) {
        Integer id = packets[i].getStreamIdentifier();
        if (id == null) {
          throw new NullPointerException("The following packet has no stream identifier and can "
                                        +"not be used to specify a ContextAssociationList entry: "+packets[i]);
        }
        ids[i] = id;
      }
      return ids;
    }

    /** Throws an exception if the list of IDs are not unique. */
    private void checkUniqueIDs (int[] ids) {
      HashSet<Integer> idList = new HashSet<Integer>();
      for (int id : ids) {
        if (idList.contains(id)) {
          throw new IllegalArgumentException("Duplicate entry for ID "+id+" found.");
        }
        idList.add(id);
      }
    }
  }
}

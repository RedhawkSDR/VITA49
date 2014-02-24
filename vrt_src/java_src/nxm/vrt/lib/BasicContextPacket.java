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

import nxm.vrt.lib.VRTConfig.VITAVersion;
import nxm.vrt.lib.VRTPacket.PacketType;

import static java.lang.Integer.bitCount;
import static nxm.vrt.lib.Utilities.append;
import static nxm.vrt.lib.VRTConfig.VRT_VERSION;
import static nxm.vrt.lib.VRTMath.fromDouble64;
import static nxm.vrt.lib.VRTMath.fromFloat16;
import static nxm.vrt.lib.VRTMath.toDouble64;
import static nxm.vrt.lib.VRTMath.toFloat16;
import static nxm.vrt.lib.VRTMath.packShort;
import static nxm.vrt.lib.VRTMath.packInt;
import static nxm.vrt.lib.VRTMath.packLong;
import static nxm.vrt.lib.VRTMath.unpackShort;
import static nxm.vrt.lib.VRTMath.unpackInt;
import static nxm.vrt.lib.VRTMath.unpackLong;

/** A basic but full-featured implementation of {@link VRTPacket} and
 *  {@link ContextPacket}. <br>
 *  <br>
 *  The following implementation is used for the {@link HasFields} methods:
 *  <pre>
 *     ID | Name                     | Type
 *    ----+--------------------------+------------------
 *    N+0 | ChangePacket             | boolean
 *    N+1 | ReferencePointIdentifier | Integer
 *    N+2 | Bandwidth                | Double
 *    N+3 | FrequencyIF              | Double
 *    N+4 | FrequencyRF              | Double
 *    N+5 | FrequencyOffsetRF        | Double
 *    N+6 | BandOffsetIF             | Double
 *    N+7 | ReferenceLevel           | Float
 *    N+8 | Gain                     | Float
 *    N+9 | Gain1                    | Float (can not set to null)
 *    N+10| Gain2                    | Float (can not set to null)
 *    N+11| OverRangeCount           | Long
 *    N+12| SampleRate               | Double
 *    N+13| SamplePeriod             | Double
 *    N+14| TimeStampAdjustment      | Long
 *    N+15| TimeStampCalibration     | Integer
 *    N+16| Temperature              | Float
 *    N+17| DeviceID                 | String
 *    N+18| CalibratedTimeStamp      | Boolean
 *    N+19| DataValid                | Boolean
 *    N+20| ReferenceLocked          | Boolean
 *    N+21| SignalDetected           | Boolean
 *    N+22| InvertedSpectrum         | Boolean
 *    N+23| OverRange                | Boolean
 *    N+24| Discontinuous            | Boolean
 *    N+25| UserDefinedBits          | Byte
 *    N+26| DataPayloadFormat        | PayloadFormat
 *    N+27| GeolocationGPS           | Geolocation
 *    N+28| GeolocationINS           | Geolocation
 *    N+29| EphemerisECEF            | Ephemeris
 *    N+30| EphemerisRelative        | Ephemeris
 *    N+31| EphemerisReference       | Integer
 *    N+32| GeoSentences             | GeoSentences
 *    N+33| ContextAssocLists        | ContextAssocLists
 *    ----+--------------------------+------------------
 *    (N = number of fields in BasicVRTPacket)
 *  </pre>
 *
 *  @author 
 */
public class BasicContextPacket extends BasicVRTPacket implements VRTPacket, ContextPacket {
  private static final int CONTEXT_IND    = 0; // special case
  private static final int CHANGE_IND     = 0x80000000;
  private static final int REF_POINT      = 0x40000000;
  private static final int BANDWIDTH      = 0x20000000;
  private static final int IF_FREQ        = 0x10000000;
  private static final int RF_FREQ        = 0x08000000;
  private static final int RF_OFFSET      = 0x04000000;
  private static final int IF_OFFSET      = 0x02000000;
  private static final int REF_LEVEL      = 0x01000000;
  private static final int GAIN           = 0x00800000;
  private static final int OVER_RANGE     = 0x00400000;
  private static final int SAMPLE_RATE    = 0x00200000;
  private static final int TIME_ADJUST    = 0x00100000;
  private static final int TIME_CALIB     = 0x00080000;
  private static final int TEMPERATURE    = 0x00040000;
  private static final int DEVICE_ID      = 0x00020000;
  private static final int STATE_EVENT    = 0x00010000;
  private static final int DATA_FORMAT    = 0x00008000;
  private static final int GPS_EPHEM      = 0x00004000;
  private static final int INS_EPHEM      = 0x00002000;
  private static final int ECEF_EPHEM     = 0x00001000;
  private static final int REL_EPHEM      = 0x00000800;
  private static final int EPHEM_REF      = 0x00000400;
  private static final int GPS_ASCII      = 0x00000200;
  private static final int CONTEXT_ASOC   = 0x00000100;
  private static final int ECEF_EPHEM_ADJ = 0x00000080;
  private static final int REL_EPHEM_ADJ  = 0x00000040;
    
  // Field lengths in the IFContext packet. Note that the entries in CTX_36_OCTETS
  // all come *after* the variable-length fields, and all others come before.
  private static final int CTX_4_OCTETS  = REF_POINT     | REF_LEVEL     | GAIN          | OVER_RANGE
                                         | TIME_CALIB    | TEMPERATURE   | STATE_EVENT   | EPHEM_REF;
  private static final int CTX_8_OCTETS  = BANDWIDTH     | IF_FREQ       | RF_FREQ       | RF_OFFSET
                                         | IF_OFFSET     | SAMPLE_RATE   | TIME_ADJUST   | DEVICE_ID
                                         | DATA_FORMAT;
  private static final int CTX_44_OCTETS = GPS_EPHEM     | INS_EPHEM;
  private static final int CTX_36_OCTETS = REL_EPHEM_ADJ | ECEF_EPHEM_ADJ;
  private static final int CTX_52_OCTETS = ECEF_EPHEM    | REL_EPHEM;

  /** Internal constructor, see docs below. */
  BasicContextPacket (VRTPacket p, boolean copy) {
    super(p, copy);
    if (p.getPacketType() != PacketType.Context) {
      throw new IllegalArgumentException("Can not create ContextPacket from "+p);
    }
  }

  /** Internal constructor, for use by subclasses.
   *  @param p       The packet (already initialized).
   *  @param copy    Create a copy?
   *  @param classID The expected classID.
   *  @param minPayloadLength The minimum payload length (usually 4 for a context packet)
   *  @param maxPayloadLength The maximum payload length (usually -1 for a context packet).
   *  @throws IllegalArgumentException If packet type or classID of the packet are invalid, or if
   *                                   the payload length is invalid.
   */
  protected BasicContextPacket (VRTPacket p, boolean copy, PacketType type, long classID,
                                int minPayloadLength, int maxPayloadLength) {
    super(p, copy, type, classID, minPayloadLength, maxPayloadLength);
    if (type != PacketType.Context) {
      throw new IllegalArgumentException("BasicContextPacket can only be created with type=Context, "
                                       + "given type="+type);
    }
  }
  
  /** Creates a new instance that can be written to. Initially this will just be a simple context
   *  packet with no fields set (other than the required packet length of 8 and a default stream ID
   *  of 0).
   */
  public BasicContextPacket () {
    super();
  }

  /** Creates a new instance accessing the given data buffer. Note that when the buffer lengths
   *  are given, only the most minimal of error checking is done. Users should call
   *  {@link #isPacketValid()} to verify that the packet is valid. Invalid packets can result
   *  unpredictable behavior, but this is explicitly allowed (to the extent possible) so that
   *  applications creating packets can use this class even if the packet isn't yet "valid".<br>
   *  <br>
   *  <b>Warning: Use of <tt>direct=true</tt> and <tt>readOnly=false</tt> are dangerous, see
   *  {@link BasicVRTPacket#BasicVRTPacket(byte[],int,int,boolean,boolean)}
   *  for details.</b>
   *  @param bbuf     The data buffer to use.
   *  @param boff     The byte offset into the bbuf to start reading/writing at.
   *  @param blen     The length of bbuf in bytes (-1=use size in header of packet).
   *  @param readOnly Should users of this instance be able to modify the underlying data buffer?
   *  @param direct   Should the methods in this class directly interact with the buffer?
   *  @throws NullPointerException If the buffer is null.
   *  @throws IllegalArgumentException If the offset is negative or greater than the
   *          length of the buffer given. Also thrown if the buffer length is -1 and a copy
   *          can not be made because the sizes in the header of the packet are invalid.
   */
  public BasicContextPacket (byte[] bbuf, int boff, int blen, boolean readOnly, boolean direct) {
    super(bbuf, boff, blen, readOnly, direct);
  }

  /** Creates a new instance accessing the given data buffers. Note that when the buffer lengths
   *  are given, only the most minimal of error checking is done. Users should call
   *  {@link #isPacketValid()} to verify that the packet is valid. Invalid packets can result
   *  unpredictable behavior, but this is explicitly allowed (to the extent possible) so that
   *  applications creating packets can use this class even if the packet isn't yet "valid".<br>
   *  <br>
   *  Note that none of the buffers can be null; however, it is legal to have <tt>pbuf</tt>
   *  and/or <tt>tbuf</tt> be of length 0 in the case where there is no payload (rare) and/or
   *  no trailer.<br>
   *  <br>
   *  <b>Warning: Use of <tt>direct=true</tt> and <tt>readOnly=false</tt> are dangerous, see
   *  {@link BasicVRTPacket#BasicVRTPacket(byte[],int,int,byte[],int,int,byte[],int,int,boolean,boolean)}
   *  for details.</b>
   *  @param hbuf     The data buffer to use for the header.
   *  @param hoff     The byte offset into hbuf to start reading/writing at.
   *  @param hlen     The length of hbuf in bytes (-1=use size in header of packet).
   *  @param pbuf     The data buffer to use for the payload.
   *  @param poff     The byte offset into pbuf to start reading/writing at.
   *  @param plen     The length of pbuf in bytes (-1=use size in header of packet).
   *  @param tbuf     The data buffer to use for the trailer.
   *  @param toff     The byte offset into tbuf to start reading/writing at.
   *  @param tlen     The length of tbuf in bytes (-1=use size in header of packet).
   *  @param readOnly Should users of this instance be able to modify the underlying data buffer?
   *  @param direct   Should the methods in this class directly interact with the buffer?
   *  @throws NullPointerException If a buffers is null.
   *  @throws IllegalArgumentException If an offset is negative or greater than the
   *          length of the buffer given. Also thrown if the a buffer length is -1 and a copy
   *          can not be made because the sizes in the header of the packet are invalid.
   */
  public BasicContextPacket (byte[] hbuf, int hoff, int hlen,
                             byte[] pbuf, int poff, int plen,
                             byte[] tbuf, int toff, int tlen, boolean readOnly, boolean direct) {
    super(hbuf, hoff, hlen, pbuf, poff, plen, tbuf, toff, tlen, readOnly, direct);
  }

  @Override
  public String getPacketValid (boolean strict, int length) {
    String err = super.getPacketValid(strict, length);
    if ((err == null) && (getPacketType() != PacketType.Context)) {
      return "Invalid use of "+getClass().getName()+" with "+getPacketType()+" packet.";
    }
    return err;
  }

  @Override
  public BasicContextPacket copy () {
    return new BasicContextPacket(this, true);
  }
  
  @Override
  protected byte[] createDefaultHeader () {
    byte[] buf = new byte[MAX_HEADER_LENGTH];
    buf[0] = 0x48;
    buf[1] = 0x60;
    buf[2] = 0x00;
    buf[3] = 0x08;
    return buf;
  }
  
  @Override
  protected byte[] createDefaultPayload () {
    return new byte[4];
  }
  
  @Override
  protected byte[] createDefaultTrailer () {
    return EMPTY;
  }
  
  @Override
  public void setPacketType (PacketType t) {
    if (t != PacketType.Context) {
      throw new IllegalArgumentException("Can not change packet type to "+t+" when using "+getClass());
    }
    super.setPacketType(t);
  }
  
  @Override
  protected StringBuilder toStringBuilder () {
    StringBuilder str = super.toStringBuilder();
    append(str, " ChangePacket="            ,isChangePacket()             );
    append(str, " ReferencePointIdentifier=",getReferencePointIdentifier());
    append(str, " Bandwidth="               ,getBandwidth()         ,"Hz" );
    append(str, " FrequencyIF="             ,getFrequencyIF()       ,"Hz" );
    append(str, " FrequencyRF="             ,getFrequencyRF()       ,"Hz" );
    append(str, " FrequencyOffsetRF="       ,getFrequencyOffsetRF() ,"Hz" );
    append(str, " BandOffsetIF="            ,getBandOffsetIF()      ,"Hz" );
    append(str, " ReferenceLevel="          ,getReferenceLevel()    ,"dBm");
    append(str, " Gain1="                   ,getGain1()             ,"dB" );
    append(str, " Gain2="                   ,getGain2()             ,"dB" );
    append(str, " OverRangeCount="          ,getOverRangeCount()          );
    append(str, " SampleRate="              ,getSampleRate()        ,"Hz" );
    append(str, " TimeStampAdjustment="     ,getTimeStampAdjustment()     );
    append(str, " TimeStampCalibration="    ,getTimeStampCalibration()    );
    append(str, " Temperature="             ,getTemperature()             );
    append(str, " DeviceID="                ,getDeviceID()                );
    append(str, " CalibratedTimeStamp="     ,isCalibratedTimeStamp()      );
    append(str, " DataValid="               ,isDataValid()                );
    append(str, " ReferenceLocked="         ,isReferenceLocked()          );
    append(str, " AGC="                     ,isAutomaticGainControl()     );
    append(str, " SignalDetected="          ,isSignalDetected()           );
    append(str, " InvertedSpectrum="        ,isInvertedSpectrum()         );
    append(str, " OverRange="               ,isOverRange()                );
    append(str, " Discontinuous="           ,isDiscontinuous()            );
    append(str, " UserDefinedBits="         ,getUserDefinedBits()         );
    append(str, " DataPayloadFormat={"      ,getDataPayloadFormat()   ,"}");
    append(str, " GeolocationGPS={"         ,getGeolocationGPS()      ,"}");
    append(str, " GeolocationINS={"         ,getGeolocationINS()      ,"}");
    append(str, " EphemerisECEF={"          ,getEphemerisECEF()       ,"}");
    append(str, " EphemerisRelative={"      ,getEphemerisRelative()   ,"}");
    append(str, " EphemerisReference={"     ,getEphemerisReference()  ,"}");
    append(str, " GeoSentences={"           ,getGeoSentences()        ,"}");
    append(str, " ContextAssocLists={"      ,getContextAssocLists()   ,"}");
    return str;
  }

  @Override
  public boolean resetForResend (TimeStamp t) {
    super.resetForResend(t);
    setChangePacket(false);
    setOverRangeCount(null);
    setOverRange(null);
    setDiscontinuous(null);
    return true;
  }
  
  @Override
  public TimeStamp getAdjustedTimeStamp () {
    Long adj = getTimeStampAdjustment();
    return (adj == null)? getTimeStamp() : getTimeStamp().addPicoSeconds(adj);
  }

  
  @Override public final boolean           isChangePacket ()              { return ((getContextIndicatorField() & CHANGE_IND) != 0); }
  @Override public final Boolean           isCalibratedTimeStamp ()       { return getStateEventBit(31, 19); }
  @Override public final Boolean           isDataValid ()                 { return getStateEventBit(30, 18); }
  @Override public final Boolean           isReferenceLocked ()           { return getStateEventBit(29, 17); }
  @Override public final Boolean           isAutomaticGainControl ()      { return getStateEventBit(28, 16); }
  @Override public final Boolean           isSignalDetected ()            { return getStateEventBit(27, 15); }
  @Override public final Boolean           isInvertedSpectrum ()          { return getStateEventBit(26, 14); }
  @Override public final Boolean           isOverRange ()                 { return getStateEventBit(25, 13); }
  @Override public final Boolean           isDiscontinuous ()             { return getStateEventBit(24, 12); }
  @Override public final Integer           getReferencePointIdentifier () { return getL(REF_POINT); }
  @Override public final Long              getTimeStampAdjustment ()      { return getX(TIME_ADJUST); }
  @Override public final Integer           getTimeStampCalibration ()     { return getL(TIME_CALIB); }
  @Override public final Integer           getEphemerisReference ()       { return getL(EPHEM_REF); }
  @Override public final Geolocation       getGeolocationGPS ()           { return getRecord(GPS_EPHEM   , new Geolocation()      ); }
  @Override public final Geolocation       getGeolocationINS ()           { return getRecord(INS_EPHEM   , new Geolocation()      ); }
  @Override public final GeoSentences      getGeoSentences ()             { return getRecord(GPS_ASCII   , new GeoSentences()     ); }
  @Override public final ContextAssocLists getContextAssocLists ()        { return getRecord(CONTEXT_ASOC, new ContextAssocLists()); }
  @Override public final String            getDeviceID ()                 { return Utilities.toStringDeviceID(getDeviceIdentifier()); }
  @Override public final Double            getBandwidth ()                { Long  bits = getX(BANDWIDTH    ); return (bits == null)? null : toDouble64(20,bits); }
  @Override public final Double            getFrequencyIF ()              { Long  bits = getX(IF_FREQ      ); return (bits == null)? null : toDouble64(20,bits); }
  @Override public final Double            getFrequencyRF ()              { Long  bits = getX(RF_FREQ      ); return (bits == null)? null : toDouble64(20,bits); }
  @Override public final Double            getFrequencyOffsetRF ()        { Long  bits = getX(RF_OFFSET    ); return (bits == null)? null : toDouble64(20,bits); }
  @Override public final Double            getBandOffsetIF ()             { Long  bits = getX(IF_OFFSET    ); return (bits == null)? null : toDouble64(20,bits); }
  @Override public final Float             getReferenceLevel ()           { Short bits = getI(REF_LEVEL  ,2); return (bits == null)? null : toFloat16 ( 7,bits); }
  @Override public final Float             getGain1 ()                    { Short bits = getI(GAIN       ,0); return (bits == null)? null : toFloat16 ( 7,bits); }
  @Override public final Float             getGain2 ()                    { Short bits = getI(GAIN       ,2); return (bits == null)? null : toFloat16 ( 7,bits); }
  @Override public final Double            getSampleRate ()               { Long  bits = getX(SAMPLE_RATE  ); return (bits == null)? null : toDouble64(20,bits); }
  @Override public final Double            getSamplePeriod ()             { Long  bits = getX(SAMPLE_RATE  ); return (bits == null)? null : 1.0/toDouble64(20,bits); }
  @Override public final Float             getTemperature ()              { Short bits = getI(TEMPERATURE,2); return (bits == null)? null : toFloat16 ( 6,bits); }
  @Override public final byte              getUserDefinedBits ()          { Byte  bits = getB(STATE_EVENT,3); return (bits == null)?    0 : bits; }
  @Override public final PayloadFormat     getDataPayloadFormat ()        { Long  bits = getX(DATA_FORMAT  ); return (bits == null)? null : new PayloadFormat(bits); }
  
  @Override public void setChangePacket (boolean change)             { setContextIndicatorFieldBit(CHANGE_IND, change); }
  @Override public void setCalibratedTimeStamp (Boolean v)           { setStateEventBit(31, 19, v); }
  @Override public void setDataValid (Boolean v)                     { setStateEventBit(30, 18, v); }
  @Override public void setReferenceLocked (Boolean v)               { setStateEventBit(29, 17, v); }
  @Override public void setAutomaticGainControl (Boolean v)          { setStateEventBit(28, 16, v); }
  @Override public void setSignalDetected (Boolean v)                { setStateEventBit(27, 15, v); }
  @Override public void setInvertedSpectrum (Boolean v)              { setStateEventBit(26, 14, v); }
  @Override public void setOverRange (Boolean v)                     { setStateEventBit(25, 13, v); }
  @Override public void setDiscontinuous (Boolean v)                 { setStateEventBit(24, 12, v); }
  @Override public void setReferencePointIdentifier (Integer val)    { setL(REF_POINT, val); }
  @Override public void setUserDefinedBits (byte val)                { setB(STATE_EVENT,3,val); }
  @Override public void setTimeStampAdjustment (Long val)            { setX(TIME_ADJUST,val); }
  @Override public void setTimeStampCalibration (Integer val)        { setL(TIME_CALIB,val); }
  @Override public void setEphemerisReference (Integer val)          { setL(EPHEM_REF,val); }
  @Override public void setGeolocationGPS (Geolocation val)          { setRecord(GPS_EPHEM   , val); }
  @Override public void setGeolocationINS (Geolocation val)          { setRecord(INS_EPHEM   , val); }
  @Override public void setGeoSentences (GeoSentences val)           { setRecord(GPS_ASCII   , val, getGeoSentences()     ); }
  @Override public void setContextAssocLists (ContextAssocLists val) { setRecord(CONTEXT_ASOC, val, getContextAssocLists()); }
  @Override public void setDeviceID (CharSequence val)               { setDeviceIdentifier(Utilities.fromStringDeviceID(val)); }
  @Override public void setBandwidth (Double val)                    { Long  bits = (val == null)? null : fromDouble64(20,val);     setX(BANDWIDTH    ,bits); }
  @Override public void setFrequencyIF (Double val)                  { Long  bits = (val == null)? null : fromDouble64(20,val);     setX(IF_FREQ      ,bits); }
  @Override public void setFrequencyRF (Double val)                  { Long  bits = (val == null)? null : fromDouble64(20,val);     setX(RF_FREQ      ,bits); }
  @Override public void setFrequencyOffsetRF (Double val)            { Long  bits = (val == null)? null : fromDouble64(20,val);     setX(RF_OFFSET    ,bits); }
  @Override public void setBandOffsetIF (Double val)                 { Long  bits = (val == null)? null : fromDouble64(20,val);     setX(IF_OFFSET    ,bits); }
  @Override public void setReferenceLevel (Float val)                { Short bits = (val == null)? null : fromFloat16 ( 7,val);     setI(REF_LEVEL  ,2,bits); }
  @Override public void setGain1 (float val)                         { Short bits =                       fromFloat16 ( 7,val);     setI(GAIN       ,0,bits); }
  @Override public void setGain2 (float val)                         { Short bits =                       fromFloat16 ( 7,val);     setI(GAIN       ,2,bits); }
  @Override public void setSampleRate (Double val)                   { Long  bits = (val == null)? null : fromDouble64(20,val);     setX(SAMPLE_RATE  ,bits); }
  @Override public void setSamplePeriod (Double val)                 { Long  bits = (val == null)? null : fromDouble64(20,1.0/val); setX(SAMPLE_RATE  ,bits); }
  @Override public void setTemperature (Float val)                   { Short bits = (val == null)? null : fromFloat16 ( 6,val);     setI(TEMPERATURE,2,bits); }
  @Override public void setDataPayloadFormat (PayloadFormat val)     { Long  bits = (val == null)? null : val.getBits();            setX(DATA_FORMAT  ,bits); }
  
  @Override @Deprecated public final void    setDiscontinuious (Boolean v) { setDiscontinuous(v); }
  @Override @Deprecated public final Boolean isDiscontinuious ()           { return isDiscontinuous(); }
  
  @Override
  public final Ephemeris getEphemerisECEF () {
    Ephemeris e = new Ephemeris();
    getRecord(ECEF_EPHEM_ADJ, e.getAdjunct());
    return getRecord(ECEF_EPHEM, e);
  }

  @Override
  public final Ephemeris getEphemerisRelative () {
    Ephemeris e = new Ephemeris();
    getRecord(REL_EPHEM_ADJ, e.getAdjunct());
    return getRecord(REL_EPHEM, e);
  }

  @Override
  public void setEphemerisECEF (Ephemeris val) {
    setRecord(ECEF_EPHEM, val);
    if ((VRT_VERSION == VITAVersion.V49b) && val.getAdjunct().isAnythingSet()) {
      setRecord(ECEF_EPHEM_ADJ, val.getAdjunct());
    }
    else {
      setRecord(ECEF_EPHEM_ADJ, null);
    }
  }
  
  @Override
  public void setEphemerisRelative (Ephemeris val) {
    setRecord(REL_EPHEM, val);
    if ((VRT_VERSION == VITAVersion.V49b) && val.getAdjunct().isAnythingSet()) {
      setRecord(REL_EPHEM_ADJ, val.getAdjunct());
    }
    else {
      setRecord(ECEF_EPHEM_ADJ, null);
    }
  }

  @Override
  public final Float getGain () {
    Float gain1 = getGain1();
    Float gain2 = getGain2();
    return (gain1 == null)? null : gain1+gain2;
  }
  
  @Override
  public void setGain (Float val) {
    if (val == null) setI(GAIN,0,null); // clears gain field(s)
    else             setGain(val, 0f);  // sets gain field(s)
  }
  
  @Override
  public void setGain (float gain1, float gain2) {
    setGain1(gain1);
    setGain2(gain2);
  }
  
  @Override
  public final Long getOverRangeCount () {
    Integer bits = getL(OVER_RANGE);
    return (bits == null)? null : (bits.intValue() & 0xFFFFFFFFL);
  }
  
  @Override
  public void setOverRangeCount (Long val) {
    if (val == null) setL(OVER_RANGE, null);
    else             setL(OVER_RANGE, val.intValue());
  }
  
  @Override
  public final Long getDeviceIdentifier () {
    Long bits = getX(DEVICE_ID);
    return (bits == null)? null : (bits & 0x00FFFFFF0000FFFFL);
  }
  
  @Override
  public void setDeviceIdentifier (Long val) {
    if ((val != null) && ((val & ~0x00FFFFFF0000FFFFL) != 0)) {
      throw new IllegalArgumentException("Invalid device identifier");
    }
    setX(DEVICE_ID,val);
  }
  
  /** Gets a block of data. */
  private <T extends Record> T getRecord (int bit, T rec) {
    int off = getOffset(bit);
    return (off < 0)? null : unpackPayloadRecord(off, rec);
  }
  
  /** Sets a block of data. */
  private <T extends Record> void setRecord (int bit, T val, T old) {
    if (old == null) setRecord(bit, val, -1);
    else             setRecord(bit, val, old.getByteLength());
  }

  /** Sets a block of data for a fixed-length field. */
  private <T extends Record> void setRecord (int bit, T val) {
    setRecord(bit, val, getFieldLen(bit));
  }
  
  /** Sets a block of data for a fixed-length field. */
  private <T extends Record> void setRecord (int bit, T val, int oldLen) {
    if (readOnly) throw new UnsupportedOperationException("Can not write to read-only VRTPacket.");
    int off = getOffset(bit);
    
    if ((val == null) && (off < 0)) return; // not present, no change
    
    if (off >= 0) {
      // remove the old one
      setContextIndicatorFieldBit(bit, false);
      shiftPayload(off, oldLen, false);
      off = -off;
    }
    
    if (val != null) {
      setContextIndicatorFieldBit(bit, true);
      off = shiftPayload(off, val.getByteLength(), true);
      packPayloadRecord(off, val);
    }
  }
  
  /** Gets the header type, used with getOffset(..) and OFFSET_TABLE. */
  private int getContextIndicatorField () {
    // Replaced the "trivial" version (immediately below) with a hand-in-lined
    // version of the getL(..) call since we know that within getL(..) the
    // getOffset(CONTEXT_IND) call will always return 0.
    //   return (getL(CONTEXT_IND) & 0xFFFFFF00);
    int val = (pbuf != null)? unpackInt(pbuf, poff)
                            : unpackInt(bbuf, boff+getHeaderLength());
    return (val & 0xFFFFFFC0);
  }
  
  /** Gets the header type, used with getOffset(..) and OFFSET_TABLE. */
  private void setContextIndicatorFieldBit (int bit, boolean set) {
    int cif = getContextIndicatorField();
    int val;
    
    if (set) val = cif | bit;
    else     val = cif & ~bit;
    
    if (val != cif) {
      if (pbuf != null) packInt(pbuf, poff,                   val);
      else              packInt(bbuf, boff+getHeaderLength(), val);
    }
  }
  
  /** Unpacks a 16-bit integer from the payload at the indicated position. */
  private Byte getB (int bit, int xoff) {
    int off = getOffset(bit);
    if (off < 0) return null;
    
    return (pbuf != null)? pbuf[off+xoff+poff]
                         : bbuf[off+xoff+boff+getHeaderLength()];
  }
  /** Packs a 16-bit integer from the payload at the indicated position. */
  private void setB (int bit, int xoff, Byte val) {
    if (readOnly) throw new UnsupportedOperationException("Can not write to read-only VRTPacket.");
    int     off     = getOffset(bit);
    boolean present = (val != null);
    
    setContextIndicatorFieldBit(bit, present);
    off = shiftPayload(off, 4, present);
    
    if (val != null) {
      if (pbuf != null) pbuf[off+xoff+poff]                   = val;
      else              bbuf[off+xoff+boff+getHeaderLength()] = val;
    }
  }
  /** Unpacks a 16-bit integer from the payload at the indicated position. */
  private Short getI (int bit, int xoff) {
    int off = getOffset(bit);
    if (off < 0) return null;
    
    return (pbuf != null)? unpackShort(pbuf, off+xoff+poff)
                         : unpackShort(bbuf, off+xoff+boff+getHeaderLength());
  }
  /** Packs a 16-bit integer from the payload at the indicated position. */
  private void setI (int bit, int xoff, Short val) {
    if (readOnly) throw new UnsupportedOperationException("Can not write to read-only VRTPacket.");
    int     off     = getOffset(bit);
    boolean present = (val != null);
    
    setContextIndicatorFieldBit(bit, present);
    off = shiftPayload(off, 4, present);
    
    if (val != null) {
      if (pbuf != null) packShort(pbuf, off+xoff+poff,                   val);
      else              packShort(bbuf, off+xoff+boff+getHeaderLength(), val);
    }
  }
  /** Unpacks a 32-bit integer from the payload at the indicated position. */
  private Integer getL (int bit) {
    int off = getOffset(bit);
    if (off < 0) return null;
    
    return (pbuf != null)? unpackInt(pbuf, off+poff)
                         : unpackInt(bbuf, off+boff+getHeaderLength());
  }
  /** Packs a 32-bit integer from the payload at the indicated position. */
  private void setL (int bit, Integer val) {
    if (readOnly) throw new UnsupportedOperationException("Can not write to read-only VRTPacket.");
    int     off     = getOffset(bit);
    boolean present = (val != null);
    
    setContextIndicatorFieldBit(bit, present);
    off = shiftPayload(off, 4, present);
    
    if (val != null) {
      if (pbuf != null) packInt(pbuf, off+poff,                   val);
      else              packInt(bbuf, off+boff+getHeaderLength(), val);
    }
  }
  /** Unpacks a 64-bit integer from the payload at the indicated position. */
  private Long getX (int bit) {
    int off = getOffset(bit);
    if (off < 0) return null;
    
    return (pbuf != null)? unpackLong(pbuf, off+poff)
                         : unpackLong(bbuf, off+boff+getHeaderLength());
  }
  /** Packs a 64-bit integer from the payload at the indicated position. */
  private void setX (int bit, Long val) {
    if (readOnly) throw new UnsupportedOperationException("Can not write to read-only VRTPacket.");
    int     off     = getOffset(bit);
    boolean present = (val != null);
    
    setContextIndicatorFieldBit(bit, present);
    off = shiftPayload(off, 8, present);
    
    if (val != null) {
      if (pbuf != null) packLong(pbuf, off+poff,                   val);
      else              packLong(bbuf, off+boff+getHeaderLength(), val);
    }
  }
  
  /** Gets the specified bit from the state and event indicator field.
   *  @param enable    Bit position of the enable flag.
   *  @param indicator Bit position of the indicator flag.
   *  @return null if there is no trailer, null if the enable bit is not set, true if the enable
   *          bit is set and the indicator bit is set, false if the enable bit is set but the
   *          indicator bit is not set.
   */
  protected final Boolean getStateEventBit (int enable, int indicator) {
    int off = getOffset(STATE_EVENT);
    if (off < 0) return null;
    
    if (pbuf != null) return getStateEventBit(pbuf, poff+off,                   enable, indicator);
    else              return getStateEventBit(bbuf, boff+off+getHeaderLength(), enable, indicator);
  }
  
  /** Sets the specified bit from the state and event indicator field.
   *  @param enable    Bit position of the enable flag.
   *  @param indicator Bit position of the indicator flag.
   *  @param value     The value of the bits: null if the enable bit should not be set, true if
   *                   the enable bit and the indicator bit should be set, false if the enable bit
   *                   should be set but the indicator bit should not be set.
   */
  protected final void setStateEventBit (int enable, int indicator, Boolean value) {
    if (readOnly) throw new UnsupportedOperationException("Can not write to read-only VRTPacket.");
    int off = getOffset(STATE_EVENT);
    if (off < 0) {
      if (value == null) return; // no trailer, no need to set to null
      setContextIndicatorFieldBit(STATE_EVENT, true);
      off = shiftPayload(off, 4, true);
    }
    
    if (pbuf != null) setStateEventBit(pbuf, poff+off,                   enable, indicator, value);
    else              setStateEventBit(bbuf, boff+off+getHeaderLength(), enable, indicator, value);
  }
  
  /** Gets the length of the given field when present (-1 if variable, -2 if not found). */
  private int getFieldLen (int field) {
    if ((field & CTX_4_OCTETS ) != 0) return  4;
    if ((field & CTX_8_OCTETS ) != 0) return  8;
    if ((field & CTX_36_OCTETS) != 0) return 36;
    if ((field & CTX_44_OCTETS) != 0) return 44;
    if ((field & CTX_52_OCTETS) != 0) return 52;
                                      return -1;
  }
  
  /** Gets the offset for the given header field. */
  protected final int getOffset (int field) {
    // Since this is the most-used method in the class and often sees millions and
    // millions of calls within a typical application, it has been heavily
    // optimized to remove any loops and minimize the number of branches and other
    // operations required. It is assumed that bitCount(..) will be in-lined during
    // the compile.
    //
    // References:
    //   [1] Warren, Henry S. Jr. "Hacker's Delight." Addison-Wesley. 2002.
    //       ISBN 0-201-91465-4
    if (field == CONTEXT_IND) return 0; // special case, must be first

    // Set the mask for all fields before 'field'. This uses the "x^(x-1)" algorithm
    // from [1] (section 2-1) to set the field bit and all bits to the right, and
    // then inverts it to get all bits left of 'field' to be set.
    int mask = ~(field ^ (field - 1));

    // For efficiency we compute the offset based on an applied bit mask and a few
    // multiplies. For *52 we do an *8 plus *44 to avoid the overhead of an extra
    // bitCount(..). In the below code, we the CTX_8_OCTETS|CTX_52_OCTETS and the
    // CTX_44_OCTETS|CTX_52_OCTETS should be in-lined during the compile; also the
    // *4 and *8 should be optimized (by the compiler) to bit-shifts where
    // appropriate.
    int cif  = getContextIndicatorField();
    int m    = cif & mask;
    int off  = 4
             + (bitCount(m & (CTX_4_OCTETS               )) *  4)
             + (bitCount(m & (CTX_8_OCTETS |CTX_52_OCTETS)) *  8)
             + (bitCount(m & (CTX_44_OCTETS|CTX_52_OCTETS)) * 44);

    // GPS_ASCII length is variable so we handle it separately if applicable. Note
    // that following our initial off= computation the offset is now queued up to
    // point to the start of the GPS_ASCII field.
    if (field < GPS_ASCII) {
      int h = getHeaderLength();
      if ((cif & GPS_ASCII) != 0) {
        off += (pbuf != null)? unpackInt(pbuf, poff+off+4)*4+8
                             : unpackInt(bbuf, boff+h+off+4)*4+8;
      }

      // CONTEXT_ASOC length is also variable, since it comes after GPS_ASCII
      // we nest it here so the check can be skipped in the 80% use case. Note
      // that off= should not point to the start of the CONTEXT_ASOC field.
      if (field < CONTEXT_ASOC) {
        if ((cif & CONTEXT_ASOC) != 0) {
          if (pbuf != null) {
            int source = unpackShort(pbuf, poff+off+0) & 0x01FF;
            int system = unpackShort(pbuf, poff+off+2) & 0x01FF;
            int vector = unpackShort(pbuf, poff+off+4) & 0xFFFF;
            int asynch = unpackShort(pbuf, poff+off+6) & 0x7FFF;
            if ((unpackShort(pbuf, poff+6+off) & 0x8000) != 0) asynch *= 2;
            off += (source + system + vector + asynch)*4+8;
          }
          else {
            int source = unpackShort(bbuf, boff+h+off+0) & 0x01FF;
            int system = unpackShort(bbuf, boff+h+off+2) & 0x01FF;
            int vector = unpackShort(bbuf, boff+h+off+4) & 0xFFFF;
            int asynch = unpackShort(bbuf, boff+h+off+6) & 0x7FFF;
            if ((unpackShort(bbuf, boff+h+6+off) & 0x8000) != 0) asynch *= 2;
            off += (source + system + vector + asynch)*4+8;
          }
        }

        // The two V49.0b entries come after CONTEXT_ASOC, so we nest it here so
        // it can be skipped in the 90% use case.
        off += (bitCount(m & (CTX_36_OCTETS)) * 36);
      }
    }

    return ((cif & field) != 0)? off : -off;  // -off if not present
  }
  
  //////////////////////////////////////////////////////////////////////////////////////////////////
  // HASFIELDS METHODS
  //////////////////////////////////////////////////////////////////////////////////////////////////
  @Override
  public int getFieldCount () {
    return super.getFieldCount() + 35;
  }

  @Override
  public String getFieldName (int id) {
    switch (id - super.getFieldCount()) {
      case  0: return "ChangePacket";
      case  1: return "ReferencePointIdentifier";
      case  2: return "Bandwidth";
      case  3: return "FrequencyIF";
      case  4: return "FrequencyRF";
      case  5: return "FrequencyOffsetRF";
      case  6: return "BandOffsetIF";
      case  7: return "ReferenceLevel";
      case  8: return "Gain";
      case  9: return "Gain1";
      case 10: return "Gain2";
      case 11: return "OverRangeCount";
      case 12: return "SampleRate";
      case 13: return "SamplePeriod";
      case 14: return "TimeStampAdjustment";
      case 15: return "TimeStampCalibration";
      case 16: return "Temperature";
      case 17: return "DeviceID";
      case 18: return "CalibratedTimeStamp";
      case 19: return "DataValid";
      case 20: return "ReferenceLocked";
      case 21: return "AGC";
      case 22: return "SignalDetected";
      case 23: return "InvertedSpectrum";
      case 24: return "OverRange";
      case 25: return "Discontinuous";
      case 26: return "UserDefinedBits";
      case 27: return "DataPayloadFormat";
      case 28: return "GeolocationGPS";
      case 29: return "GeolocationINS";
      case 30: return "EphemerisECEF";
      case 31: return "EphemerisRelative";
      case 32: return "EphemerisReference";
      case 33: return "GeoSentences";
      case 34: return "ContextAssocLists";
      default: return super.getFieldName(id);
    }
  }
  
  @Override
  public Class<?> getFieldType (int id) {
    switch (id - super.getFieldCount()) {
      case  0: return Boolean.TYPE;
      case  1: return Integer.class;
      case  2: return Double.class;
      case  3: return Double.class;
      case  4: return Double.class;
      case  5: return Double.class;
      case  6: return Double.class;
      case  7: return Float.class;
      case  8: return Float.class;
      case  9: return Float.class;
      case 10: return Float.class;
      case 11: return Long.class;
      case 12: return Double.class;
      case 13: return Double.class;
      case 14: return Long.class;
      case 15: return Integer.class;
      case 16: return Float.class;
      case 17: return String.class;
      case 18: return Boolean.class;
      case 19: return Boolean.class;
      case 20: return Boolean.class;
      case 21: return Boolean.class;
      case 22: return Boolean.class;
      case 23: return Boolean.class;
      case 24: return Boolean.class;
      case 25: return Boolean.class;
      case 26: return Byte.class;
      case 27: return PayloadFormat.class;
      case 28: return Geolocation.class;
      case 29: return Geolocation.class;
      case 30: return Ephemeris.class;
      case 31: return Ephemeris.class;
      case 32: return Integer.class;
      case 33: return GeoSentences.class;
      case 34: return ContextAssocLists.class;
      default: return super.getFieldType(id);
    }
  }

  @Override
  public Object getField (int id) {
    switch (id - super.getFieldCount()) {
      case  0: return isChangePacket();
      case  1: return getReferencePointIdentifier();
      case  2: return getBandwidth();
      case  3: return getFrequencyIF();
      case  4: return getFrequencyRF();
      case  5: return getFrequencyOffsetRF();
      case  6: return getBandOffsetIF();
      case  7: return getReferenceLevel();
      case  8: return getGain();
      case  9: return getGain1();
      case 10: return getGain2();
      case 11: return getOverRangeCount();
      case 12: return getSampleRate();
      case 13: return getSamplePeriod();
      case 14: return getTimeStampAdjustment();
      case 15: return getTimeStampCalibration();
      case 16: return getTemperature();
      case 17: return getDeviceID();
      case 18: return isCalibratedTimeStamp();
      case 19: return isDataValid();
      case 20: return isReferenceLocked();
      case 21: return isAutomaticGainControl();
      case 22: return isSignalDetected();
      case 23: return isInvertedSpectrum();
      case 24: return isOverRange();
      case 25: return isDiscontinuous();
      case 26: return getUserDefinedBits();
      case 27: return getDataPayloadFormat();
      case 28: return getGeolocationGPS();
      case 29: return getGeolocationINS();
      case 30: return getEphemerisECEF();
      case 31: return getEphemerisRelative();
      case 32: return getEphemerisReference();
      case 33: return getGeoSentences();
      case 34: return getContextAssocLists();
      default: return super.getField(id);
    }
  }

  @Override
  public void setField (int id, Object val) {
    switch (id - super.getFieldCount()) {
      case  0: setChangePacket(            Utilities.castValue(Boolean.TYPE,            val)); break;
      case  1: setReferencePointIdentifier(Utilities.castValue(Integer.class,           val)); break;
      case  2: setBandwidth(               Utilities.castValue(Double.class,            val)); break;
      case  3: setFrequencyIF(             Utilities.castValue(Double.class,            val)); break;
      case  4: setFrequencyRF(             Utilities.castValue(Double.class,            val)); break;
      case  5: setFrequencyOffsetRF(       Utilities.castValue(Double.class,            val)); break;
      case  6: setBandOffsetIF(            Utilities.castValue(Double.class,            val)); break;
      case  7: setReferenceLevel(          Utilities.castValue(Float.class,             val)); break;
      case  8: setGain(                    Utilities.castValue(Float.class,             val)); break;
      case  9: setGain1(                   Utilities.castValue(Float.TYPE,              val)); break;
      case 10: setGain2(                   Utilities.castValue(Float.TYPE,              val)); break;
      case 11: setOverRangeCount(          Utilities.castValue(Long.class,              val)); break;
      case 12: setSampleRate(              Utilities.castValue(Double.class,            val)); break;
      case 13: setSamplePeriod(            Utilities.castValue(Double.class,            val)); break;
      case 14: setTimeStampAdjustment(     Utilities.castValue(Long.class,              val)); break;
      case 15: setTimeStampCalibration(    Utilities.castValue(Integer.class,           val)); break;
      case 16: setTemperature(             Utilities.castValue(Float.class,             val)); break;
      case 17: setDeviceID(                Utilities.castValue(String.class,            val)); break;
      case 18: setCalibratedTimeStamp(     Utilities.castValue(Boolean.class,           val)); break;
      case 19: setDataValid(               Utilities.castValue(Boolean.class,           val)); break;
      case 20: setReferenceLocked(         Utilities.castValue(Boolean.class,           val)); break;
      case 21: setAutomaticGainControl(    Utilities.castValue(Boolean.class,           val)); break;
      case 22: setSignalDetected(          Utilities.castValue(Boolean.class,           val)); break;
      case 23: setInvertedSpectrum(        Utilities.castValue(Boolean.class,           val)); break;
      case 24: setOverRange(               Utilities.castValue(Boolean.class,           val)); break;
      case 25: setDiscontinuous(           Utilities.castValue(Boolean.class,           val)); break;
      case 26: setUserDefinedBits(         Utilities.castValue(Byte.class,              val)); break;
      case 27: setDataPayloadFormat(       Utilities.castValue(PayloadFormat.class,     val)); break;
      case 28: setGeolocationGPS(          Utilities.castValue(Geolocation.class,       val)); break;
      case 29: setGeolocationINS(          Utilities.castValue(Geolocation.class,       val)); break;
      case 30: setEphemerisECEF(           Utilities.castValue(Ephemeris.class,         val)); break;
      case 31: setEphemerisRelative(       Utilities.castValue(Ephemeris.class,         val)); break;
      case 32: setEphemerisReference(      Utilities.castValue(Integer.class,           val)); break;
      case 33: setGeoSentences(            Utilities.castValue(GeoSentences.class,      val)); break;
      case 34: setContextAssocLists(       Utilities.castValue(ContextAssocLists.class, val)); break;
      default: super.setField(id,val); break;
    }
  }
}

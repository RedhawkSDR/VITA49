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

import nxm.vrt.lib.VRTPacket.PacketType;
import nxm.vrt.lib.VRTPacket.PayloadFormat;
import java.nio.ByteOrder;

import static nxm.vrt.lib.Utilities.append;
import static java.nio.ByteOrder.BIG_ENDIAN;

/** An basic but full-featured implementation of {@link VRTPacket} and
 *  {@link DataPacket}. <br>
 *  <br>
 *  The following implementation is used for the {@link HasFields} methods:
 *  <pre>
 *     ID | Name                 | Type
 *    ----+----------------------+---------------
 *    N+0 | CalibratedTimeStamp  | Boolean
 *    N+1 | DataValid            | Boolean
 *    N+2 | ReferenceLocked      | Boolean
 *    N+3 | AGC                  | Boolean
 *    N+4 | SignalDetected       | Boolean
 *    N+5 | InvertedSpectrum     | Boolean
 *    N+6 | OverRange            | Boolean
 *    N+7 | Discontinuous        | Boolean
 *    N+8 | Bit11                | Boolean
 *    N+9 | Bit10                | Boolean
 *    N+10| Bit9                 | Boolean
 *    N+11| Bit8                 | Boolean
 *    N+12| AssocPacketCount     | Byte
 *    N+13| PayloadFormat        | PayloadFormat
 *    ----+----------------------+---------------
 *    (N = number of fields in BasicVRTPacket)
 *  </pre>
 *
 *  @author 
 */
public class BasicDataPacket extends BasicVRTPacket implements DataPacket {
  private static final ByteOrder BYTE_ORDER = ByteOrder.nativeOrder();
  
  private PayloadFormat payloadFormat;

  /** Internal constructor, see docs below. */
  public BasicDataPacket (VRTPacket p, boolean copy) {
    super(p, copy);
    if (!p.getPacketType().isData()) {
      throw new IllegalArgumentException("Can not create DataPacket from "+p);
    }
  }
  
  /** Creates a new instance that can be written to. Initially this will just be a simple data
   *  packet with no fields set (other than the required packet length of 4).
   */
  public BasicDataPacket () {
    super();
  }
  
  /** Internal constructor, for use by subclasses.
   *  @param p                The packet (already initialized).
   *  @param type             The expected packet type.
   *  @param classID          The expected classID.
   *  @param minPayloadLength The minimum payload length.
   *  @param maxPayloadLength The maximum payload length (&lt;0 if not pre-defined).
   *  @throws IllegalArgumentException If packet type or classID of the packet are invalid, or if
   *                                   the payload length is invalid.
   */
  protected BasicDataPacket (VRTPacket p, boolean copy, PacketType type, long classID,
                             int minPayloadLength, int maxPayloadLength) {
    super(p, copy, type, classID, minPayloadLength, maxPayloadLength);
    if (!type.isData()) {
      throw new IllegalArgumentException("Can not initialize "+getClass()+" when given packet type of "+type);
    }
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
  public BasicDataPacket (byte[] bbuf, int boff, int blen, boolean readOnly, boolean direct) {
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
  public BasicDataPacket (byte[] hbuf, int hoff, int hlen,
                          byte[] pbuf, int poff, int plen,
                          byte[] tbuf, int toff, int tlen, boolean readOnly, boolean direct) {
    super(hbuf, hoff, hlen, pbuf, poff, plen, tbuf, toff, tlen, readOnly, direct);
  }

  @Override
  public String getPacketValid (boolean strict, int length) {
    String err = super.getPacketValid(strict, length);
    if ((err == null) && !getPacketType().isData()) {
      return "Invalid use of "+getClass().getName()+" with "+getPacketType()+" packet.";
    }
    return err;
  }
  
  @Override
  public BasicDataPacket copy () {
    return new BasicDataPacket(this, true);
  }

  @Override
  protected byte[] createDefaultTrailer () {
    return new byte[MAX_TRAILER_LENGTH];
  }
  
  @Override
  public void setPacketType (PacketType t) {
    if (!t.isData()) {
      throw new IllegalArgumentException("Can not change packet type to "+t+" when using "+getClass());
    }
    super.setPacketType(t);
  }
  
  @Override
  protected StringBuilder toStringBuilder () {
    StringBuilder str = super.toStringBuilder();
    append(str, " CalibratedTimeStamp=",isCalibratedTimeStamp());
    append(str, " DataValid=",          isDataValid());
    append(str, " ReferenceLocked=",    isReferenceLocked());
    append(str, " AGC=",                isAutomaticGainControl());
    append(str, " SignalDetected=",     isSignalDetected());
    append(str, " InvertedSpectrum=",   isInvertedSpectrum());
    append(str, " OverRange=",          isOverRange());
    append(str, " Discontinuous=",      isDiscontinuous());
    append(str, " Bit11=",              isBit11());
    append(str, " Bit10=",              isBit10());
    append(str, " Bit9=",               isBit9());
    append(str, " Bit8=",               isBit8());
    append(str, " AssocPacketCount=",   getAssocPacketCount());
    return str;
  }
  
  @Override
  public final Byte getAssocPacketCount () {
    if (!hasTrailer()) return null; // no trailer
    
    byte val;
    if (tbuf == null) val = bbuf[boff+getPacketLength()-1];
    else              val = tbuf[toff+3];
    
    return ((val &0x80) == 0)? null : (byte)(val & 0x7F);
  }
  
  @Override
  public void setAssocPacketCount (Byte v) {
    if (readOnly) throw new UnsupportedOperationException("Can not write to read-only VRTPacket.");
    if ((v != null) && (v < 0)) {
      throw new IllegalArgumentException("Invalid associated packet count "+v);
    }
    if (!hasTrailer()) shiftTrailer(+4);
    
    int val = (v == null)? 0 : (0x80 | v);

    if (tbuf == null) bbuf[boff+getPacketLength()-1] = (byte)val;
    else              tbuf[toff+3]                   = (byte)val;
  }
  
  /** Gets the specified bit from the trailer.
   *  @param enable    Bit position of the enable flag.
   *  @param indicator Bit position of the indicator flag.
   *  @return null if there is no trailer, null if the enable bit is not set, true if the enable
   *          bit is set and the indicator bit is set, false if the enable bit is set but the
   *          indicator bit is not set.
   */
  protected final Boolean getTrailerBit (int enable, int indicator) {
    if (!hasTrailer()) return null; // no trailer
    
    if (tbuf != null) return getStateEventBit(tbuf,                     toff, enable, indicator);
    else              return getStateEventBit(bbuf, boff+getPacketLength()-4, enable, indicator);
  }
  
  /** <i>Optional functionality:</i> Sets the specified bit from the trailer. By default all of
   *  the "set" methods that access the trailer call this method with the appropriate bit values
   *  (see {@link #getTrailerBit(int,int)} for a list).
   *  @param enable    Bit position of the enable flag.
   *  @param indicator Bit position of the indicator flag.
   *  @param value     The value of the bits: null if the enable bit should not be set, true if
   *                   the enable bit and the indicator bit should be set, false if the enable bit
   *                   should be set but the indicator bit should not be set.
   */
  protected void setTrailerBit (int enable, int indicator, Boolean value) {
    if (readOnly) throw new UnsupportedOperationException("Can not write to read-only VRTPacket.");
    if (!hasTrailer()) {
      if (value == null) return; // no trailer, no need to set to null
      shiftTrailer(+4);
      bbuf[boff] |= TRAILER_BIT;
    }
    
    if (tbuf != null) setStateEventBit(tbuf,                     toff, enable, indicator, value);
    else              setStateEventBit(bbuf, boff+getPacketLength()-4, enable, indicator, value);
  }
  
  @Override public final Boolean isCalibratedTimeStamp ()  { return getTrailerBit(31, 19); }
  @Override public final Boolean isDataValid ()            { return getTrailerBit(30, 18); }
  @Override public final Boolean isReferenceLocked ()      { return getTrailerBit(29, 17); }
  @Override public final Boolean isAutomaticGainControl () { return getTrailerBit(28, 16); }
  @Override public final Boolean isSignalDetected ()       { return getTrailerBit(27, 15); }
  @Override public final Boolean isInvertedSpectrum ()     { return getTrailerBit(26, 14); }
  @Override public final Boolean isOverRange ()            { return getTrailerBit(25, 13); }
  @Override public final Boolean isDiscontinuous ()        { return getTrailerBit(24, 12); }
  @Override public final Boolean isBit11 ()                { return getTrailerBit(23, 11); }
  @Override public final Boolean isBit10 ()                { return getTrailerBit(22, 10); }
  @Override public final Boolean isBit9 ()                 { return getTrailerBit(21,  9); }
  @Override public final Boolean isBit8 ()                 { return getTrailerBit(20,  8); }
  
  @Override public final void setCalibratedTimeStamp (Boolean v)  { setTrailerBit(31, 19, v); }
  @Override public final void setDataValid (Boolean v)            { setTrailerBit(30, 18, v); }
  @Override public final void setReferenceLocked (Boolean v)      { setTrailerBit(29, 17, v); }
  @Override public final void setAutomaticGainControl (Boolean v) { setTrailerBit(28, 16, v); }
  @Override public final void setSignalDetected (Boolean v)       { setTrailerBit(27, 15, v); }
  @Override public final void setInvertedSpectrum (Boolean v)     { setTrailerBit(26, 14, v); }
  @Override public final void setOverRange (Boolean v)            { setTrailerBit(25, 13, v); }
  @Override public final void setDiscontinuous (Boolean v)        { setTrailerBit(24, 12, v); }
  @Override public final void setBit11 (Boolean v)                { setTrailerBit(23, 11, v); }
  @Override public final void setBit10 (Boolean v)                { setTrailerBit(22, 10, v); }
  @Override public final void setBit9 (Boolean v)                 { setTrailerBit(21,  9, v); }
  @Override public final void setBit8 (Boolean v)                 { setTrailerBit(20,  8, v); }
  
  @Override @Deprecated public final void    setDiscontinuious (Boolean v) { setDiscontinuous(v); }
  @Override @Deprecated public final Boolean isDiscontinuious ()           { return isDiscontinuous(); }

  @Override public final int           getScalarDataLength ()              { return getScalarDataLength(getPayloadFormat()); }
  @Override public final int           getDataLength ()                    { return getDataLength(getPayloadFormat()); }
  @Override public       PayloadFormat getPayloadFormat ()                 { return payloadFormat; }
  @Override public final void          setDataLength (int length)          { setDataLength(getPayloadFormat(), length); }
  @Override public       void          setPayloadFormat (PayloadFormat pf) { payloadFormat = pf; }

  @Override public double[] getDataDouble (PayloadFormat pf) { int len = getScalarDataLength(pf); return PackUnpack.unpackAsDouble(pf, getPBuf(), getPOff(), new double[len], null, null, len); }
  @Override public float[]  getDataFloat  (PayloadFormat pf) { int len = getScalarDataLength(pf); return PackUnpack.unpackAsFloat( pf, getPBuf(), getPOff(), new float[len],  null, null, len); }
  @Override public long[]   getDataLong   (PayloadFormat pf) { int len = getScalarDataLength(pf); return PackUnpack.unpackAsLong(  pf, getPBuf(), getPOff(), new long[len],   null, null, len); }
  @Override public int[]    getDataInt    (PayloadFormat pf) { int len = getScalarDataLength(pf); return PackUnpack.unpackAsInt(   pf, getPBuf(), getPOff(), new int[len],    null, null, len); }
  @Override public short[]  getDataShort  (PayloadFormat pf) { int len = getScalarDataLength(pf); return PackUnpack.unpackAsShort( pf, getPBuf(), getPOff(), new short[len],  null, null, len); }
  @Override public byte[]   getDataByte   (PayloadFormat pf) { int len = getScalarDataLength(pf); return PackUnpack.unpackAsByte(  pf, getPBuf(), getPOff(), new byte[len],   null, null, len); }

  @Override public double[] getDataDouble () { return getDataDouble(getPayloadFormat()); }
  @Override public float[]  getDataFloat  () { return getDataFloat( getPayloadFormat()); }
  @Override public long[]   getDataLong   () { return getDataLong(  getPayloadFormat()); }
  @Override public int[]    getDataInt    () { return getDataInt(   getPayloadFormat()); }
  @Override public short[]  getDataShort  () { return getDataShort( getPayloadFormat()); }
  @Override public byte[]   getDataByte   () { return getDataByte(  getPayloadFormat()); }

  @Override public void setDataDouble (double[] array) { setDataDouble(getPayloadFormat(), array); }
  @Override public void setDataFloat  (float[]  array) { setDataFloat( getPayloadFormat(), array); }
  @Override public void setDataLong   (long[]   array) { setDataLong(  getPayloadFormat(), array); }
  @Override public void setDataInt    (int[]    array) { setDataInt(   getPayloadFormat(), array); }
  @Override public void setDataShort  (short[]  array) { setDataShort( getPayloadFormat(), array); }
  @Override public void setDataByte   (byte[]   array) { setDataByte(  getPayloadFormat(), array); }

  @Override
  public void setDataDouble (PayloadFormat pf, double[] array) {
    setDataLength(pf, array.length);
    PackUnpack.packAsDouble(pf, getPBuf(), getPOff(), array, null, null, array.length);
  }
  
  @Override
  public void setDataFloat (PayloadFormat pf, float[] array) {
    setDataLength(pf, array.length);
    PackUnpack.packAsFloat(pf, getPBuf(), getPOff(), array, null, null, array.length);
  }
  
  @Override
  public void setDataLong (PayloadFormat pf, long[] array) {
    setDataLength(pf, array.length);
    PackUnpack.packAsLong(pf, getPBuf(), getPOff(), array, null, null, array.length);
  }
  
  @Override
  public void setDataInt (PayloadFormat pf, int[] array) {
    setDataLength(pf, array.length);
    PackUnpack.packAsInt(pf, getPBuf(), getPOff(), array, null, null, array.length);
  }
  
  @Override
  public void setDataShort (PayloadFormat pf, short[] array) {
    setDataLength(pf, array.length);
    PackUnpack.packAsShort(pf, getPBuf(), getPOff(), array, null, null, array.length);
  }
  
  @Override
  public void setDataByte (PayloadFormat pf, byte[] array) {
    if (pf.getDataType() == DataType.Int8) {
      // Fast version where no conversion is required
      setPayloadLength(array.length);
      setData(pf, array, 0, array.length);
    }
    else {
      setDataLength(pf, array.length);
      PackUnpack.packAsByte(pf, getPBuf(), getPOff(), array, null, null, array.length);
    }
  }
  
  @Override
  public int getScalarDataLength (PayloadFormat pf) {
    int complexMult = (pf.isComplex())? 2 : 1;
    return getDataLength(pf) * complexMult;
  }
  
  @Override
  public int getDataLength (PayloadFormat pf) {
    int     bitsPerSample = pf.getItemPackingFieldSize();
    int     complexMult   = (pf.isComplex())? 2 : 1;
    boolean naturalSize   = (bitsPerSample ==  8) || (bitsPerSample == 16) ||
                            (bitsPerSample == 32) || (bitsPerSample == 64);

    if (naturalSize || (!pf.isProcessingEfficient())) {
      // LinkEfficient (or naturally sized, in which case ProcessingEfficient = LinkEfficient)
      int totalBits = (getPayloadLength() * 8) - getPadBitCount();
      return (totalBits / bitsPerSample) / complexMult;
    }
    else if (bitsPerSample <= 32) {
      // ProcessingEfficient with 32-bit word size
      int samplesPerWord = 32 / bitsPerSample;
      int totalWords     = getPayloadLength() / 4;
      int totalSamples   = samplesPerWord * totalWords;
      int padSamples     = getPadBitCount() / bitsPerSample;
      return (totalSamples - padSamples) / complexMult;
    }
    else {
      // ProcessingEfficient with 64-bit word size
      int samplesPerWord = 64 / bitsPerSample;
      int totalWords     = getPayloadLength() / 8;
      int totalSamples   = samplesPerWord * totalWords;
      int padSamples     = getPadBitCount() / bitsPerSample;
      return (totalSamples - padSamples) / complexMult;
    }
  }

  @Override
  public TimeStamp getNextTimeStamp (double sampleRate, PayloadFormat pf) {
    TimeStamp ts = getTimeStamp();
    if (ts == null) return null;
    double dt = getDataLength(pf) / sampleRate; // seconds
    return ts.addPicoSeconds((long)(dt / TimeStamp.ONE_SEC));
  }

  @Override
  public TimeStamp getNextTimeStamp (double sampleRate) {
    return getNextTimeStamp(sampleRate, getPayloadFormat());
  }

  @Override
  public int getLostSamples (TimeStamp expected, double sampleRate) {
    TimeStamp ts = getTimeStamp();
    if ((ts == null) || (expected == null)) {
      throw new NullPointerException("Can not compute number of lost samples, time stamp is null.");
    }
    long timeDeltaSec = ts.getSecondsUTC()  - expected.getSecondsUTC();
    long timeDeltaPS  = ts.getPicoSeconds() - expected.getPicoSeconds() + (timeDeltaSec * TimeStamp.ONE_SEC);
    return (int)(timeDeltaPS / (sampleRate * TimeStamp.ONE_SEC));
  }

  @Override
  public int getLostBytes (TimeStamp expected, double sampleRate, PayloadFormat pf) {
    return getLostSamples(expected, sampleRate) * pf.getDataItemSize()/8;
  }

  @Override
  public int getLostBytes (TimeStamp expected, double sampleRate) {
    return getLostBytes(expected, sampleRate, getPayloadFormat());
  }

  @Override
  public void setDataLength (PayloadFormat pf, int length) {
    int     bitsPerSample = pf.getItemPackingFieldSize();
    int     samples       = (pf.isComplex())? 2*length : length;
    boolean naturalSize   = (bitsPerSample ==  8) || (bitsPerSample == 16) ||
                            (bitsPerSample == 32) || (bitsPerSample == 64);

    if (naturalSize || (!pf.isProcessingEfficient())) {
      // LinkEfficient (if sample size is multiple of 8-bits, ProcessingEfficient = LinkEfficient)
      int totalBits  = samples * bitsPerSample;
      int totalBytes = (totalBits  + 7) / 8;       // +7 to make it round up
      int totalWords = (totalBytes + 3) / 4;       // +3 to make it round up
      setPadBitCount(totalWords*32 - totalBits, bitsPerSample); // words -> bits
      setPayloadLength(totalWords * 4);            // words -> bytes
    }
    else if (bitsPerSample <= 32) {
      // ProcessingEfficient with 32-bit word size
      int samplesPerWord = 32 / bitsPerSample;
      int totalWords      = (samples + samplesPerWord-1) / samplesPerWord;  // +(samplesPerWord-1) to round up
      int samplesLastWord = samples - ((totalWords-1) * samplesPerWord);
      
      setPadBitCount(32 - samplesLastWord*bitsPerSample, bitsPerSample);
        setPayloadLength(totalWords * 4);
      }
      else {
      // ProcessingEfficient with 64-bit word size
        int samplesPerWord = 64 / bitsPerSample;
      int totalWords      = (samples + samplesPerWord-1) / samplesPerWord;  // +(samplesPerWord-1) to round up
      int samplesLastWord = samples - ((totalWords-1) * samplesPerWord);

      setPadBitCount(64 - samplesLastWord*bitsPerSample, bitsPerSample);
      setPayloadLength(totalWords * 8);
  }
  }

  @Override
  public byte[] getData (byte[] array, int offset) {
    return getData(getPayloadFormat(), array, offset);
  }
  
  @Override
  public void setData (byte[] array, int offset, int length) {
    setData(getPayloadFormat(), array, offset, length);
  }

  @Override
  public byte[] getData (PayloadFormat pf, byte[] array, int offset) {
    DataType type = pf.getDataType();
    if (type == null) {
      throw new UnsupportedOperationException("Fast unpacking of given data format ("+pf+") not supported");
    }

    int    len  = getPayloadLength() - getPadBitCount()/8; // only care if PadBitCount > 8
    byte[] buf  = getPBuf();
    int    off  = getPOff();
    int    size = pf.getDataItemSize();

    if ((size <= 8) || (BYTE_ORDER == BIG_ENDIAN)) {
      // Able to use a basic memory copy
      System.arraycopy(buf, off, array, offset, len);
    }
    else if (size == 16) {
      // BIG_ENDIAN -> LITTLE_ENDIAN
      for (int i=0, j=offset; i < len; i+=2,j+=2) {
        array[j+1] = buf[off++];
        array[j+0] = buf[off++];
      }
    }
    else if (size == 32) {
      // BIG_ENDIAN -> LITTLE_ENDIAN
      for (int i=0, j=offset; i < len; i+=4,j+=4) {
        array[j+3] = buf[off++];
        array[j+2] = buf[off++];
        array[j+1] = buf[off++];
        array[j+0] = buf[off++];
      }
    }
    else { // size == 64
      // BIG_ENDIAN -> LITTLE_ENDIAN
      for (int i=0, j=offset; i < len; i+=8,j+=8) {
        array[j+8] = buf[off++];
        array[j+7] = buf[off++];
        array[j+6] = buf[off++];
        array[j+5] = buf[off++];
        array[j+4] = buf[off++];
        array[j+3] = buf[off++];
        array[j+2] = buf[off++];
        array[j+1] = buf[off++];
      }
    }
    return array;
  }

  @SuppressWarnings("fallthrough")
  @Override
  public void setData (PayloadFormat pf, byte[] array, int offset, int len) {
    DataType type = pf.getDataType();
    if (type == null) {
      throw new UnsupportedOperationException("Fast unpacking of given data format ("+pf+") not supported");
    }

    int size = pf.getDataItemSize();
    int bitsPerSample = pf.getItemPackingFieldSize();

    if ((len % (size/8)) != 0) {
      throw new IllegalArgumentException("Given length of "+len+" bytes is not a multiple of "+size+" bits as "
                                       + "required for "+type+" data");
    }

    int overflow = (len % 4);
    int padBytes = (4 - overflow) % 4; // %4 to make 4->0
    setPadBitCount(padBytes * 8, bitsPerSample); // StandardDataType->not implicit
    setPayloadLength(len + padBytes);
    byte[] buf  = getPBuf(); // <-- This can be altered by above call to setPayloadLength(len)
    int    off  = getPOff(); // <-- This can be altered by above call to setPayloadLength(len)

    if ((size <= 8) || (BYTE_ORDER == BIG_ENDIAN)) {
      // Able to use a basic memory copy
      System.arraycopy(array, offset, buf, off, len);
      off += len;
    }
    else if (size == 16) {
      // LITTLE_ENDIAN -> BIG_ENDIAN
      for (int i=0, j=offset; i < len; i+=2,j+=2) {
        buf[off++] = array[j+1];
        buf[off++] = array[j+0];
      }
    }
    else if (size == 32) {
      // LITTLE_ENDIAN -> BIG_ENDIAN
      for (int i=0, j=offset; i < len; i+=4,j+=4) {
        buf[off++] = array[j+3];
        buf[off++] = array[j+2];
        buf[off++] = array[j+1];
        buf[off++] = array[j+0];
      }
    }
    else { // size == 64
      // LITTLE_ENDIAN -> BIG_ENDIAN
      for (int i=0, j=offset; i < len; i+=8,j+=8) {
        buf[off++] = array[j+8];
        buf[off++] = array[j+7];
        buf[off++] = array[j+6];
        buf[off++] = array[j+5];
        buf[off++] = array[j+4];
        buf[off++] = array[j+3];
        buf[off++] = array[j+2];
        buf[off++] = array[j+1];
      }
    }

    off += len;
    switch (padBytes) { // be nice and set pad bytes to 0 so it is easier to detect errors
      case 3: bbuf[off++] = 0; // FALLTHROUGH
      case 2: bbuf[off++] = 0; // FALLTHROUGH
      case 1: bbuf[off++] = 0; // FALLTHROUGH
    }
  }
  
  //////////////////////////////////////////////////////////////////////////////////////////////////
  // HASFIELDS METHODS
  //////////////////////////////////////////////////////////////////////////////////////////////////
  @Override
  public int getFieldCount () {
    return super.getFieldCount() + 14;
  }

  @Override
  public String getFieldName (int id) {
    switch (id - super.getFieldCount()) {
      case  0: return "CalibratedTimeStamp";
      case  1: return "DataValid";
      case  2: return "ReferenceLocked";
      case  3: return "AGC";
      case  4: return "SignalDetected";
      case  5: return "InvertedSpectrum";
      case  6: return "OverRange";
      case  7: return "Discontinuous";
      case  8: return "Bit11";
      case  9: return "Bit10";
      case 10: return "Bit9";
      case 11: return "Bit8";
      case 12: return "AssocPacketCount";
      case 13: return "PayloadFormat";
      default: return super.getFieldName(id);
    }
  }

  @Override
  public Class<?> getFieldType (int id) {
    switch (id - super.getFieldCount()) {
      case  0: return Boolean.class;
      case  1: return Boolean.class;
      case  2: return Boolean.class;
      case  3: return Boolean.class;
      case  4: return Boolean.class;
      case  5: return Boolean.class;
      case  6: return Boolean.class;
      case  7: return Boolean.class;
      case  8: return Boolean.class;
      case  9: return Boolean.class;
      case 10: return Boolean.class;
      case 11: return Boolean.class;
      case 12: return Byte.class;
      case 13: return PayloadFormat.class;
      default: return super.getFieldType(id);
    }
  }

  @Override
  public Object getField (int id) {
    switch (id - super.getFieldCount()) {
      case  0: return isCalibratedTimeStamp();
      case  1: return isDataValid();
      case  2: return isReferenceLocked();
      case  3: return isAutomaticGainControl();
      case  4: return isSignalDetected();
      case  5: return isInvertedSpectrum();
      case  6: return isOverRange();
      case  7: return isDiscontinuous();
      case  8: return isBit11();
      case  9: return isBit10();
      case 10: return isBit9();
      case 11: return isBit8();
      case 12: return getAssocPacketCount();
      case 13: return getPayloadFormat();
      default: return super.getField(id);
    }
  }

  @Override
  public void setField (int id, Object val) {
    switch (id - super.getFieldCount()) {
      case  0: setCalibratedTimeStamp( Utilities.castValue(Boolean.class,       val)); break;
      case  1: setDataValid(           Utilities.castValue(Boolean.class,       val)); break;
      case  2: setReferenceLocked(     Utilities.castValue(Boolean.class,       val)); break;
      case  3: setAutomaticGainControl(Utilities.castValue(Boolean.class,       val)); break;
      case  4: setSignalDetected(      Utilities.castValue(Boolean.class,       val)); break;
      case  5: setInvertedSpectrum(    Utilities.castValue(Boolean.class,       val)); break;
      case  6: setOverRange(           Utilities.castValue(Boolean.class,       val)); break;
      case  7: setDiscontinuous(       Utilities.castValue(Boolean.class,       val)); break;
      case  8: setBit11(               Utilities.castValue(Boolean.class,       val)); break;
      case  9: setBit10(               Utilities.castValue(Boolean.class,       val)); break;
      case 10: setBit9(                Utilities.castValue(Boolean.class,       val)); break;
      case 11: setBit8(                Utilities.castValue(Boolean.class,       val)); break;
      case 12: setAssocPacketCount(    Utilities.castValue(Byte.class,          val)); break;
      case 13: setPayloadFormat(       Utilities.castValue(PayloadFormat.class, val)); break;
      default: super.setField(id,val); break;
    }
  }
}

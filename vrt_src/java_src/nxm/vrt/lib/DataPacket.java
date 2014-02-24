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

/** Defines functionality shared between a VRT IF Data packet and a VRT Extension Data packet.
 *
 *  @author 
 */
public interface DataPacket extends VRTPacket, HasStateEventBits {
  /** Creates a copy of this packet that is independent of the original. The copy will not be
   *  read only and will not be "direct" (i.e. will not share data with the original). This
   *  method's behavior is undefined if the packet is invalid and may throw an exception in that
   *  event.
   *  @return A copy of this packet.
   */
  @Override
  DataPacket copy ();

  //////////////////////////////////////////////////////////////////////////////////////////////////

  /** Gets the #11 user-defined indicator flag. <i>The definition of this flag
   *  should be included in the documentation for the relevant packet class (this flag should be
   *  identified there as "Enable Bit Position 23" and/or "Indicator Bit Position 11").</i>
   *  @return true if the flag is set, false if it is not set, null if the flag isn't specified.
   */
  Boolean isBit11 ();
  
  /** Gets the #10 user-defined indicator flag. <i>The definition of this flag
   *  should be included in the documentation for the relevant packet class (this flag should be
   *  identified there as "Enable Bit Position 22" and/or "Indicator Bit Position 10").</i>
   *  @return true if the flag is set, false if it is not set, null if the flag isn't specified.
   */
  Boolean isBit10 ();
  
  /** Gets the #9 user-defined indicator flag. <i>The definition of this flag
   *  should be included in the documentation for the relevant packet class (this flag should be
   *  identified there as "Enable Bit Position 21" and/or "Indicator Bit Position 9").</i>
   *  @return true if the flag is set, false if it is not set, null if the flag isn't specified.
   */
  Boolean isBit9 ();
  
  /** Gets the #8 user-defined indicator flag. <i>The definition of this flag
   *  should be included in the documentation for the relevant packet class (this flag should be
   *  identified there as "Enable Bit Position 20" and/or "Indicator Bit Position 8").</i>
   *  @return true if the flag is set, false if it is not set, null if the flag isn't specified.
   */
  Boolean isBit8 ();
  
  /** Gets the associated packet count. This indicates the number of context
   *  packets with context change or event information relevant to this data packet.
   *  @return The assocaited packet count (0..127), or null if not specified (null is returned
   *          for all context packets).
   */
  Byte getAssocPacketCount ();

  /** <i>Optional functionality:</i> Sets the #11 user-defined indicator flag.
   *  @param v true if the flag is set, false if not set, null if the flag isn't specified.
   *  @throws UnsupportedOperationException If this method is not supported or if called on a
   *                                        context packet.
   */
  void setBit11 (Boolean v);
  
  /** <i>Optional functionality:</i> Sets the #10 user-defined indicator flag.
   *  @param v true if the flag is set, false if not set, null if the flag isn't specified.
   *  @throws UnsupportedOperationException If this method is not supported or if called on a
   *                                        context packet.
   */
  void setBit10 (Boolean v);
  
  /** <i>Optional functionality:</i> Sets the #9 user-defined indicator flag.
   *  @param v true if the flag is set, false if not set, null if the flag isn't specified.
   *  @throws UnsupportedOperationException If this method is not supported or if called on a
   *                                        context packet.
   */
  void setBit9 (Boolean v);
  
  /** <i>Optional functionality:</i> Sets the #8 user-defined indicator flag.
   *  @param v true if the flag is set, false if not set, null if the flag isn't specified.
   *  @throws UnsupportedOperationException If this method is not supported or if called on a
   *                                        context packet.
   */
  void setBit8 (Boolean v);
  
  /** <i>Optional functionality:</i> Sets the associated packet count.
   *  This indicates the number of context packets with context change or event information
   *  relevant to this data packet.
   *  @param v The assocaited packet count (0..127), or null if not specified.
   *  @throws UnsupportedOperationException If this method is not supported or if called on a
   *                                        context packet.
   *  @throws IllegalArgumentException If the value passed in is invalid.
   */
  void setAssocPacketCount (Byte v);

  /** Gets the assumed payload format used when interacting with the data in this packet. This
   *  setting can either be fixed based on the packet class or set via {@link #setPayloadFormat}.
   *  @return The payload format to assume when interacting with this packet. This will return
   *          null if the payload format appears unspecified for this packet class and no call
   *          to {@link #setPayloadFormat} has been made.
   */
  PayloadFormat getPayloadFormat ();

  /** <i>Optional Functionality:</i> Sets the payload format to be assumed when interacting
   *  with the data in this packet. Note that this setting exists only for the lifetime of this
   *  object and is not directly stored within the data packet. <br>
   *  <br>
   *  Many data packet classes use a fixed payload format (see {@link #getPayloadFormat()}) and
   *  will throw an exception if this method is called.
   *  @param pf The payload format to assume.
   *  @throws UnsupportedOperationException If setting the payload format is unsupported.
   */
  void setPayloadFormat (PayloadFormat pf);

  /** <i>Utility method:</i> gets the length of the data in number of scalar elements. Unlike
   *  {@link #getPayloadLength()} this returns the number of data elements, not bytes. For complex
   *  data the real and imaginary components of a single value count as a two elements (see
   *  {@link #getDataLength()}). <br>
   *  <br>
   *  This method can only be used AFTER a payload format for the packet has been specified
   *  (see {@link #getPayloadFormat()}. This method is identical to calling
   *  <tt>p.getScalarDataLength(p.getPayloadFormat())</tt>
   *  @return The number of elements in the payload.
   */
  int getScalarDataLength ();

  /** <i>Utility method:</i> gets the length of the data in number of scalar elements. Unlike
   *  {@link #getPayloadLength()} this returns the number of data elements, not bytes. For complex
   *  data the real and imaginary components of a single value count as a two elements (see
   *  {@link #getDataLength()}). <br>
   *  @param pf The payload format to assume.
   *  @return The number of elements in the payload.
   *  @throws NullPointerException If the payload format is null.
   */
  int getScalarDataLength (PayloadFormat pf);

  /** Gets the length of the data in number of elements. Unlike {@link #getPayloadLength()}
   *  this returns the number of data elements, not bytes. For complex data the real and imaginary
   *  components of a single value count as a single element (see {@link #getScalarDataLength()}). <br>
   *  <br>
   *  This method can only be used AFTER a payload format for the packet has been specified
   *  (see {@link #getPayloadFormat()}. This method is identical to calling
   *  <tt>p.getDataLength(p.getPayloadFormat())</tt>
   *  @return The number of elements in the payload.
   */
  int getDataLength ();

  /** gets the length of the data in number of elements. Unlike {@link #getPayloadLength()}
   *  this returns the number of data elements, not bytes. For complex data the real and imaginary
   *  components of a single value count as a single element (see {@link #getScalarDataLength()}).
   *  @param pf The payload format to assume.
   *  @return The number of elements in the payload.
   *  @throws NullPointerException If the payload format is null.
   */
  int getDataLength (PayloadFormat pf);

  /** Gets the next expected time stamp for a data packet on this stream. This is computed
   *  as the following:
   *  <pre>
   *    nextTimeStamp = currentTimeStamp + (dataLength * sampleRate)
   *  </pre>
   *  @param sampleRate The applicable sample rate.
   *  @param pf         The payload format to assume.
   *  @return The time stamp to expect for the next data packet.
   *  @throws VRTException If the payload format is null.
   */
  TimeStamp getNextTimeStamp (double sampleRate, PayloadFormat pf);

  /** Gets the next expected time stamp for a data packet on this stream. This is computed
   *  as the following:
   *  <pre>
   *    nextTimeStamp = currentTimeStamp + (dataLength * sampleRate)
   *  </pre>
   *  This method can only be used AFTER a payload format for the packet has been specified
   *  (see {@link #getPayloadFormat()}. This method is identical to calling
   *  <tt>p.getDataLength(p.getPayloadFormat())</tt>
   *  @param sampleRate The applicable sample rate.
   *  @return The time stamp to expect for the next data packet.
   */
  TimeStamp getNextTimeStamp (double sampleRate);

  /** Computes the number of samples lost between two packets.
   *  @param expected    The expected time stamp (predicted from by calling
   *                    <tt>getNextTimeStamp(..)</tt> on the previous packet).
   *  @param sampleRate The applicable sample rate.
   *  @return The number of lost samples. This will be 0 if no samples were lost and -N if there
   *          is an N sample overlap (i.e. extra data). The -N case is usually the result of an
   *          incorrect sample rate and/or incorrect payload format.
   */
  int getLostSamples (TimeStamp expected, double sampleRate);

  /** Computes the number of bytes lost between two packets.
   *  @param expected    The expected time stamp (predicted from by calling
   *                    <tt>getNextTimeStamp(..)</tt> on the previous packet).
   *  @param sampleRate The applicable sample rate.
   *  @param pf         The payload format to assume.
   *  @return The number of lost bytes. This will be 0 if no bytes were lost and -N if there
   *          is an N byte overlap (i.e. extra data). The -N case is usually the result of an
   *          incorrect sample rate and/or incorrect payload format.
   */
  int getLostBytes (TimeStamp expected, double sampleRate, PayloadFormat pf);

  /** Computes the number of bytes lost between two packets. <br>
   *  <br>
   *  This method can only be used AFTER a payload format for the packet has been specified
   *  (see {@link #getPayloadFormat()}. This method is identical to calling
   *  <tt>p.getDataLength(p.getPayloadFormat())</tt>
   *  @param expected    The expected time stamp (predicted from by calling
   *                    <tt>getNextTimeStamp(..)</tt> on the previous packet).
   *  @param sampleRate The applicable sample rate.
   *  @return The number of lost bytes. This will be 0 if no bytes were lost and -N if there
   *          is an N byte overlap (i.e. extra data). The -N case is usually the result of an
   *          incorrect sample rate and/or incorrect payload format.
   */
  int getLostBytes (TimeStamp expected, double sampleRate);

  /** Sets the length of the data in number of elements. Unlike {@link #setPayloadLength(int)}
   *  this sets the number of data elements, not bytes. For complex data the real and imaginary
   *  components of a single value count as a single element (see {@link #getScalarDataLength()}). <br>
   *  <br>
   *  This method can only be used AFTER a payload format for the packet has been specified
   *  (see {@link #getPayloadFormat()}. This method is identical to calling
   *  <tt>p.getDataLength(p.getPayloadFormat())</tt>
   *  @param length The number of elements in the payload.
   */
  void setDataLength (int length);

  /** Sets the length of the data in number of elements. Unlike {@link #setPayloadLength(int)}
   *  this sets the number of data elements, not bytes. For complex data the real and imaginary
   *  components of a single value count as a single element (see {@link #getScalarDataLength()}).
   *  @param pf     The payload format to assume.
   *  @param length The number of elements in the payload.
   *  @throws NullPointerException If the payload format is null.
   */
  void setDataLength (PayloadFormat pf, int length);

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // GET WITH GIVEN PAYLOAD FORMAT
  //////////////////////////////////////////////////////////////////////////////////////////////////
  /** Unpacks the data and returns it in a native byte buffer. This is basically a byte-for-byte
   *  copy of the payload, except that a conversion is done from {@link java.nio.ByteOrder#BIG_ENDIAN} to the native
   *  byte ordering. This method is intended for use within existing programming frameworks that
   *  already provide type conversions. <b>This method does not convert the values to byte data,
   *  only copies the data to what is effectively a <tt>void*</tt> buffer in the native byte order.</b> <br>
   *  <br>
   *  For 8-bit, 4-bit and 1-bit types, this is implemented as a fast array copy with no
   *  translation being done with regards to byte order. Further, with 4-bit and 1-bit types an
   *  assumption is made that the number of entries is an even multiple of one octet (i.e. 4-bit
   *  values must be in multiples of 2 and 1-bit values in multiples of 8).
   *  @param pf     The payload format to assume.
   *  @param array  The array to insert the data into.
   *  @param offset The array offset.
   *  @return <tt>array</tt>
   *  @throws NullPointerException If the payload format is null or if the array is null.
   *  @throws UnsupportedOperationException If the payload format is anything other than
   *          {@link VRTPacket#INT4},    {@link VRTPacket#INT8},
   *          {@link VRTPacket#INT16},   {@link VRTPacket#INT32},    {@link VRTPacket#INT64},
   *          {@link VRTPacket#FLOAT32}, {@link VRTPacket#DOUBLE64},
   *          {@link VRTPacket#UINT1},   {@link VRTPacket#UINT4},    {@link VRTPacket#UINT8},
   *          {@link VRTPacket#UINT16},  {@link VRTPacket#UINT32},   {@link VRTPacket#UINT64}.
   *  @throws ArrayIndexOutOfBoundsException If the array length is shorter than
   *          <tt>offset+{@link #getPayloadLength()}</tt>.
   */
  byte[] getData (PayloadFormat pf, byte[] array, int offset);

  /** Unpacks the data and returns it as a double array. If the underlying data is not double,
   *  it will be converted to double and any values that fall outside of the range of a double will
   *  will have unpredictable output. If the payload format is incorrectly specified, the output of
   *  this method will be useless.
   *  @param pf The payload format to assume.
   *  @return The data elements.
   *  @throws NullPointerException If the payload format is null.
   */
  double[] getDataDouble (PayloadFormat pf);

  /** Unpacks the data and returns it as a float array. If the underlying data is not double,
   *  it will be converted to float and any values that fall outside of the range of a float will
   *  will have unpredictable output. If the payload format is incorrectly specified, the output of
   *  this method will be useless.
   *  @param pf The payload format to assume.
   *  @return The data elements.
   *  @throws NullPointerException If the payload format is null.
   */
  float[] getDataFloat (PayloadFormat pf);

  /** Unpacks the data and returns it as a long array. If the underlying data is not double,
   *  it will be converted to long and any values that fall outside of the range of a long will
   *  will have unpredictable output. If the payload format is incorrectly specified, the output of
   *  this method will be useless.
   *  @param pf The payload format to assume.
   *  @return The data elements.
   *  @throws NullPointerException If the payload format is null.
   */
  long[] getDataLong (PayloadFormat pf);

  /** Unpacks the data and returns it as a int array. If the underlying data is not double,
   *  it will be converted to int and any values that fall outside of the range of a int will
   *  will have unpredictable output. If the payload format is incorrectly specified, the output of
   *  this method will be useless.
   *  @param pf The payload format to assume.
   *  @return The data elements.
   *  @throws NullPointerException If the payload format is null.
   */
  int[] getDataInt (PayloadFormat pf);

  /** Unpacks the data and returns it as a short array. If the underlying data is not double,
   *  it will be converted to short and any values that fall outside of the range of a short will
   *  will have unpredictable output. If the payload format is incorrectly specified, the output of
   *  this method will be useless.
   *  @param pf The payload format to assume.
   *  @return The data elements.
   *  @throws NullPointerException If the payload format is null.
   */
  short[] getDataShort (PayloadFormat pf);

  /** Unpacks the data and returns it as a byte array. If the underlying data is not double,
   *  it will be converted to byte and any values that fall outside of the range of a byte will
   *  will have unpredictable output. If the payload format is incorrectly specified, the output of
   *  this method will be useless.
   *  @param pf The payload format to assume.
   *  @return The data elements.
   *  @throws NullPointerException If the payload format is null.
   */
  byte[] getDataByte (PayloadFormat pf);

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // GET WITH PRE-DEFINED PAYLOAD FORMAT
  //////////////////////////////////////////////////////////////////////////////////////////////////
  /** Unpacks the data and returns it in a native byte buffer. This is basically a byte-for-byte
   *  copy of the payload, except that a conversion is done from {@link java.nio.ByteOrder#BIG_ENDIAN} to the native
   *  byte ordering. This method is intended for use within existing programming frameworks that
   *  already provide type conversions. <b>This method does not convert the values to byte data,
   *  only copies the data to what is effectively a <tt>void*</tt> buffer in the native byte order.</b> <br>
   *  <br>
   *  This method can only be used AFTER a payload format for the packet has been specified
   *  (see {@link #getPayloadFormat()}. This method is identical to calling
   *  <tt>p.getData(p.getPayloadFormat(),array,offset)</tt> <br>
   *  <br>
   *  For 8-bit, 4-bit and 1-bit types, this is implemented as a fast array copy with no
   *  translation being done with regards to byte order.
   *  @param array  The array to insert the data into.
   *  @param offset The array offset.
   *  @return <tt>array</tt>
   *  @throws NullPointerException If the payload format is null or if the array is null.
   *  @throws UnsupportedOperationException If the payload format is anything other than
   *          {@link VRTPacket#INT4},    {@link VRTPacket#INT8},
   *          {@link VRTPacket#INT16},   {@link VRTPacket#INT32},    {@link VRTPacket#INT64},
   *          {@link VRTPacket#FLOAT32}, {@link VRTPacket#DOUBLE64},
   *          {@link VRTPacket#UINT1},   {@link VRTPacket#UINT4},    {@link VRTPacket#UINT8},
   *          {@link VRTPacket#UINT16},  {@link VRTPacket#UINT32},   {@link VRTPacket#UINT64}.
   *  @throws ArrayIndexOutOfBoundsException If the array length is shorter than
   *          <tt>offset+{@link #getPayloadLength()}</tt>.
   */
  byte[] getData (byte[] array, int offset);

  /** Unpacks the data and returns it as a double array. If the underlying data is not double,
   *  it will be converted to double and any values that fall outside of the range of a double will
   *  will have unpredictable output. If the payload format is incorrectly specified, the output of
   *  this method will be useless. <br>
   *  <br>
   *  This method can only be used AFTER a payload format for the packet has been specified
   *  (see {@link #getPayloadFormat()}. This method is identical to calling
   *  <tt>p.getDataDouble(p.getPayloadFormat())</tt>
   *  @return The data elements.
   *  @throws NullPointerException If the payload format is null.
   */
  double[] getDataDouble ();

  /** Unpacks the data and returns it as a float array. If the underlying data is not float,
   *  it will be converted to float and any values that fall outside of the range of a float will
   *  will have unpredictable output. If the payload format is incorrectly specified, the output of
   *  this method will be useless. <br>
   *  <br>
   *   This method can only be used AFTER a payload format for the packet has been specified
   *  (see {@link #getPayloadFormat()}. This method is identical to calling
   *  <tt>p.getDataFloat(p.getPayloadFormat())</tt>
   *  @return The data elements.
   *  @throws NullPointerException If the payload format is null.
   */
  float[] getDataFloat ();

  /** Unpacks the data and returns it as a long array. If the underlying data is not long,
   *  it will be converted to long and any values that fall outside of the range of a long will
   *  will have unpredictable output. If the payload format is incorrectly specified, the output of
   *  this method will be useless. <br>
   *  <br>
   *  This method can only be used AFTER a payload format for the packet has been specified
   *  (see {@link #getPayloadFormat()}. This method is identical to calling
   *  <tt>p.getDataLong(p.getPayloadFormat())</tt>
   *  @return The data elements.
   *  @throws NullPointerException If the payload format is null.
   */
  long[] getDataLong ();

  /** Unpacks the data and returns it as a int array. If the underlying data is not int,
   *  it will be converted to int and any values that fall outside of the range of a int will
   *  will have unpredictable output. If the payload format is incorrectly specified, the output of
   *  this method will be useless. <br>
   *  <br>
   *  This method can only be used AFTER a payload format for the packet has been specified
   *  (see {@link #getPayloadFormat()}. This method is identical to calling
   *  <tt>p.getDataInt(p.getPayloadFormat())</tt>
   *  @return The data elements.
   *  @throws NullPointerException If the payload format is null.
   */
  int[] getDataInt ();

  /** Unpacks the data and returns it as a byte array. If the underlying data is not short,
   *  it will be converted to byte and any values that fall outside of the range of a byte will
   *  will have unpredictable output. If the payload format is incorrectly specified, the output of
   *  this method will be useless. <br>
   *  <br>
   *  This method can only be used AFTER a payload format for the packet has been specified
   *  (see {@link #getPayloadFormat()}. This method is identical to calling
   *  <tt>p.getDataShort(p.getPayloadFormat())</tt>
   *  @return The data elements.
   *  @throws NullPointerException If the payload format is null.
   */
  short[] getDataShort ();

  /** Unpacks the data and returns it as a byte array. If the underlying data is not byte,
   *  it will be converted to byte and any values that fall outside of the range of a byte will
   *  will have unpredictable output. If the payload format is incorrectly specified, the output of
   *  this method will be useless. <br>
   *  <br>
   *  This method can only be used AFTER a payload format for the packet has been specified
   *  (see {@link #getPayloadFormat()}. This method is identical to calling
   *  <tt>p.getDataByte(p.getPayloadFormat())</tt>
   *  @return The data elements.
   *  @throws NullPointerException If the payload format is null.
   */
  byte[] getDataByte ();

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // SET WITH GIVEN PAYLOAD FORMAT
  //////////////////////////////////////////////////////////////////////////////////////////////////
  /** Packs the data from a native byte buffer. This is basically a byte-for-byte
   *  copy of the payload, except that a conversion is done from the native to {@link java.nio.ByteOrder#BIG_ENDIAN}
   *  byte ordering. This method is intended for use within existing programming frameworks that
   *  already provide type conversions. <b>This method does not convert the values from byte data,
   *  only copies the data from what is effectively a <tt>void*</tt> buffer in the native byte order.</b>
   *  @param pf     The payload format to assume.
   *  @param array  The array to insert the data into.
   *  @param offset The array offset.
   *  @param length The number of bytes to pack (must be a multiple of 8 for {@link VRTPacket#INT64}
   *                or {@link VRTPacket#DOUBLE64} or a multiple of 4 for all others).
   *  @throws NullPointerException If the payload format is null or if the array is null.
   *  @throws UnsupportedOperationException If the payload format is anything other than
   *          {@link VRTPacket#INT4},    {@link VRTPacket#INT8},
   *          {@link VRTPacket#INT16},   {@link VRTPacket#INT32},    {@link VRTPacket#INT64},
   *          {@link VRTPacket#FLOAT32}, {@link VRTPacket#DOUBLE64},
   *          {@link VRTPacket#UINT1},   {@link VRTPacket#UINT4},    {@link VRTPacket#UINT8},
   *          {@link VRTPacket#UINT16},  {@link VRTPacket#UINT32},   {@link VRTPacket#UINT64}.
   *  @throws ArrayIndexOutOfBoundsException If the array length is shorter than
   *          <tt>offset+{@link #getPayloadLength()}</tt>.
   *  @throws IllegalArgumentException If the length is invalid.
   */
  void setData (PayloadFormat pf, byte[] array, int offset, int length);

  /** Packs the data using the values from a double array. If the underlying data is not double,
   *  it will be converted from double and any values that fall outside of the range of a of the
   *  payload format will have unpredictable output. This method also has the effect of setting
   *  the size of the data (see {@link #setDataLength}) and indirectly the payload and packet.
   *  @param pf The payload format to assume.
   *  @param array The values to insert.
   *  @throws NullPointerException If the payload format is null.
   */
  void setDataDouble (PayloadFormat pf, double[] array);

  /** Packs the data using the values from a float array. If the underlying data is not float,
   *  it will be converted from float and any values that fall outside of the range of a of the
   *  payload format will have unpredictable output. This method also has the effect of setting
   *  the size of the data (see {@link #setDataLength}) and indirectly the payload and packet.
   *  @param pf The payload format to assume.
   *  @param array The values to insert.
   *  @throws NullPointerException If the payload format is null.
   */
  void setDataFloat (PayloadFormat pf, float[] array);

  /** Packs the data using the values from a long array. If the underlying data is not long,
   *  it will be converted from long and any values that fall outside of the range of a of the
   *  payload format will have unpredictable output. This method also has the effect of setting
   *  the size of the data (see {@link #setDataLength}) and indirectly the payload and packet.
   *  @param pf The payload format to assume.
   *  @param array The values to insert.
   *  @throws NullPointerException If the payload format is null.
   */
  void setDataLong (PayloadFormat pf, long[] array);

  /** Packs the data using the values from a int array. If the underlying data is not int,
   *  it will be converted from int and any values that fall outside of the range of a of the
   *  payload format will have unpredictable output. This method also has the effect of setting
   *  the size of the data (see {@link #setDataLength}) and indirectly the payload and packet.
   *  @param pf The payload format to assume.
   *  @param array The values to insert.
   *  @throws NullPointerException If the payload format is null.
   */
  void setDataInt (PayloadFormat pf, int[] array);

  /** Packs the data using the values from a short array. If the underlying data is not short,
   *  it will be converted from short and any values that fall outside of the range of a of the
   *  payload format will have unpredictable output. This method also has the effect of setting
   *  the size of the data (see {@link #setDataLength}) and indirectly the payload and packet.
   *  @param pf The payload format to assume.
   *  @param array The values to insert.
   *  @throws NullPointerException If the payload format is null.
   */
  void setDataShort (PayloadFormat pf, short[] array);

  /** Packs the data using the values from a byte array. If the underlying data is not byte,
   *  it will be converted from byte and any values that fall outside of the range of a of the
   *  payload format will have unpredictable output. This method also has the effect of setting
   *  the size of the data (see {@link #setDataLength}) and indirectly the payload and packet.
   *  @param pf The payload format to assume.
   *  @param array The values to insert.
   *  @throws NullPointerException If the payload format is null.
   */
  void setDataByte (PayloadFormat pf, byte[] array);

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // SET WITH PRE-DEFINED PAYLOAD FORMAT
  //////////////////////////////////////////////////////////////////////////////////////////////////
  /** Packs the data from a native byte buffer. This is basically a byte-for-byte
   *  copy of the payload, except that a conversion is done from the native to {@link java.nio.ByteOrder#BIG_ENDIAN}
   *  byte ordering. This method is intended for use within existing programming frameworks that
   *  already provide type conversions. <b>This method does not convert the values from byte data,
   *  only copies the data from what is effectively a <tt>void*</tt> buffer in the native byte order.</b> <br>
   *  <br>
   *  This method can only be used AFTER a payload format for the packet has been specified
   *  (see {@link #getPayloadFormat()}. This method is identical to calling
   *  <tt>p.setData(p.getPayloadFormat(),array,offset,length)</tt>
   *  @param array  The array to insert the data into.
   *  @param offset The array offset.
   *  @param length The number of bytes to pack (must be a multiple of 8 for {@link VRTPacket#INT64}
   *                or {@link VRTPacket#DOUBLE64} or a multiple of 4 for all others).
   *  @throws NullPointerException If the payload format is null or if the array is null.
   *  @throws UnsupportedOperationException If the payload format is anything other than
   *          {@link VRTPacket#INT4},    {@link VRTPacket#INT8},
   *          {@link VRTPacket#INT16},   {@link VRTPacket#INT32},    {@link VRTPacket#INT64},
   *          {@link VRTPacket#FLOAT32}, {@link VRTPacket#DOUBLE64},
   *          {@link VRTPacket#UINT1},   {@link VRTPacket#UINT4},    {@link VRTPacket#UINT8},
   *          {@link VRTPacket#UINT16},  {@link VRTPacket#UINT32},   {@link VRTPacket#UINT64}.
   *  @throws ArrayIndexOutOfBoundsException If the array length is shorter than
   *          <tt>offset+{@link #getPayloadLength()}</tt>.
   *  @throws IllegalArgumentException If the length is invalid.
   */
  void setData (byte[] array, int offset, int length);

  /** Packs the data using the values from a double array. If the underlying data is not double,
   *  it will be converted from double and any values that fall outside of the range of a of the
   *  payload format will have unpredictable output. This method also has the effect of setting
   *  the size of the data (see {@link #setDataLength}) and indirectly the payload and packet. <br>
   *  <br>
   *  This method can only be used AFTER a payload format for the packet has been specified
   *  (see {@link #getPayloadFormat()}. This method is identical to calling
   *  <tt>p.setDataDouble(p.getPayloadFormat(),array)</tt>
   *  @param array The values to insert.
   *  @throws NullPointerException If the payload format is null.
   */
  void setDataDouble (double[] array);

  /** Packs the data using the values from a float array. If the underlying data is not float,
   *  it will be converted from float and any values that fall outside of the range of a of the
   *  payload format will have unpredictable output. This method also has the effect of setting
   *  the size of the data (see {@link #setDataLength}) and indirectly the payload and packet. <br>
   *  <br>
   *  This method can only be used AFTER a payload format for the packet has been specified
   *  (see {@link #getPayloadFormat()}. This method is identical to calling
   *  <tt>p.setDataFloat(p.getPayloadFormat(),array)</tt>
   *  @param array The values to insert.
   *  @throws NullPointerException If the payload format is null.
   */
  void setDataFloat (float[] array);

  /** Packs the data using the values from a long array. If the underlying data is not long,
   *  it will be converted from long and any values that fall outside of the range of a of the
   *  payload format will have unpredictable output. This method also has the effect of setting
   *  the size of the data (see {@link #setDataLength}) and indirectly the payload and packet. <br>
   *  <br>
   *  This method can only be used AFTER a payload format for the packet has been specified
   *  (see {@link #getPayloadFormat()}. This method is identical to calling
   *  <tt>p.setDataLong(p.getPayloadFormat(),array)</tt>
   *  @param array The values to insert.
   *  @throws NullPointerException If the payload format is null.
   */
  void setDataLong (long[] array);

  /** Packs the data using the values from a int array. If the underlying data is not int,
   *  it will be converted from int and any values that fall outside of the range of a of the
   *  payload format will have unpredictable output. This method also has the effect of setting
   *  the size of the data (see {@link #setDataLength}) and indirectly the payload and packet. <br>
   *  <br>
   *  This method can only be used AFTER a payload format for the packet has been specified
   *  (see {@link #getPayloadFormat()}. This method is identical to calling
   *  <tt>p.setDataInt(p.getPayloadFormat(),array)</tt>
   *  @param array The values to insert.
   *  @throws NullPointerException If the payload format is null.
   */
  void setDataInt (int[] array);

  /** Packs the data using the values from a short array. If the underlying data is not short,
   *  it will be converted from short and any values that fall outside of the range of a of the
   *  payload format will have unpredictable output. This method also has the effect of setting
   *  the size of the data (see {@link #setDataLength}) and indirectly the payload and packet. <br>
   *  <br>
   *  This method can only be used AFTER a payload format for the packet has been specified
   *  (see {@link #getPayloadFormat()}. This method is identical to calling
   *  <tt>p.setDataShort(p.getPayloadFormat(),array)</tt>
   *  @param array The values to insert.
   *  @throws NullPointerException If the payload format is null.
   */
  void setDataShort (short[] array);

  /** Packs the data using the values from a byte array. If the underlying data is not byte,
   *  it will be converted from byte and any values that fall outside of the range of a of the
   *  payload format will have unpredictable output. This method also has the effect of setting
   *  the size of the data (see {@link #setDataLength}) and indirectly the payload and packet. <br>
   *  <br>
   *  This method can only be used AFTER a payload format for the packet has been specified
   *  (see {@link #getPayloadFormat()}. This method is identical to calling
   *  <tt>p.setDataByte(p.getPayloadFormat(),array)</tt>
   *  @param array The values to insert.
   *  @throws NullPointerException If the payload format is null.
   */
  void setDataByte (byte[] array);
}

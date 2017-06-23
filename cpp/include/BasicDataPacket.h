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

#ifndef _BasicDataPacket_h
#define _BasicDataPacket_h
#include "BasicVRTPacket.h"
#include "PackUnpack.h"

namespace vrt {

  /** An basic but full-featured implementation of a VRT IF Data packet. <br>
   *  <br>
   *  The following implementation is used for the {@link HasFields} methods:
   *  <pre>
   *     ID | Name                 | Type
   *    ----+----------------------+---------------
   *    N+0 | CalibratedTimeStamp  | BoolNull
   *    N+1 | DataValid            | BoolNull
   *    N+2 | ReferenceLocked      | BoolNull
   *    N+3 | AGC                  | BoolNull
   *    N+4 | SignalDetected       | BoolNull
   *    N+5 | InvertedSpectrum     | BoolNull
   *    N+6 | OverRange            | BoolNull
   *    N+7 | Discontinuous        | BoolNull
   *    N+8 | Bit11                | BoolNull
   *    N+9 | Bit10                | BoolNull
   *    N+10| Bit9                 | BoolNull
   *    N+11| Bit8                 | BoolNull
   *    N+12| AssocPacketCount     | Int8
   *    N+13| PayloadFormat        | PayloadFormat
   *    ----+----------------------+---------------
   *    (N = number of fields in BasicVRTPacket)
   *  </pre>
   *
   *  @author 
   */
  class BasicDataPacket : public BasicVRTPacket {
    friend class BasicVRTState;

    /** The payload format to assume. */
    private: PayloadFormat payloadFormat;

    /** Basic destructor for the class. */
    public: ~BasicDataPacket () { }

    /** Basic copy constructor for the class. */
    public: BasicDataPacket (const BasicVRTPacket &p);

    /** Creates a new instance accessing the given data buffer.
     *  @param buf      The pointer to the packet buffer.
     *  @param len      The length  of the packet buffer.
     *  @param readOnly Should users of this instance be able to modify the underlying data buffer?
     */
    public: BasicDataPacket (const void *buf, size_t len, bool readOnly=false);

    /** Creates a new instance that can be written to. Initially this will just be a simple data
     *  packet with no fields set (other than the required packet length of 4).
     */
    public: BasicDataPacket ();

    public: BasicDataPacket (const int32_t pktsize);

    public: BasicDataPacket (const vector<char> &buf, ssize_t start, ssize_t end, bool readOnly = false);

    /** Creates a new instance accessing the given data buffer. Note that when the buffer lengths
     *  are given, only the most minimal of error checking is done. Users should call
     *  {@link #isPacketValid()} to verify that the packet is valid. Invalid packets can result
     *  unpredictable behavior, but this is explicitly allowed (to the extent possible) so that
     *  applications creating packets can use this class even if the packet isn't yet "valid".
     *  @param buf      The data buffer to use.
     *  @param readOnly Should users of this instance be able to modify the underlying data buffer?
     */
    public: BasicDataPacket (const vector<char> &buf, bool readOnly=false);

    /** Creates a new instance accessing the given data buffer pointer. Note that when the buffer lengths
     *  are given, only the most minimal of error checking is done. Users should call
     *  {@link #isPacketValid()} to verify that the packet is valid. Invalid packets can result
     *  unpredictable behavior, but this is explicitly allowed (to the extent possible) so that
     *  applications creating packets can use this class even if the packet isn't yet "valid".
     *  @param buf      The data buffer pointer to use.
     *  @param readOnly Should users of this instance be able to modify the underlying data buffer?
     */
    public: BasicDataPacket (vector<char> *buf, bool readOnly=false);

    /** Internal constructor, for use by subclasses.
        @param p                The packet (already initialized).
        @param type             The expected packet type.
        @param classID          The expected classID.
        @param minPayloadLength The minimum payload length.
        @param maxPayloadLength The maximum payload length (&lt;0 if not pre-defined).
        @throws VRTException If packet type or classID of the packet are invalid, or if
                                         the payload length is invalid.
     */
    protected: BasicDataPacket (const BasicVRTPacket &p, PacketType type, int64_t classID,
                                int32_t minPayloadLength, int32_t maxPayloadLength);

    public: virtual string getPacketValid (bool strict, int32_t length) const;

    public: inline void setPacketType (PacketType t) {
      if (!PacketType_isData(t)) {
        throw VRTException("Can not change packet type to non-Data type when using BasicDataPacket");
      }
      BasicVRTPacket::setPacketType(t);
    }

    protected: virtual void toStringStream (ostringstream &str) const;

    /** Gets the trailer as an integer.
     *  @return null if there is no trailer, an integer representing the trailer if there is.
     */
    protected: int32_t getTrailer () const;

    /** Gets the specified bit from the trailer.
     *  @param enable    Bit position of the enable flag.
     *  @param indicator Bit position of the indicator flag.
     *  @return null if there is no trailer, null if the enable bit is not set, true if the enable
     *          bit is set and the indicator bit is set, false if the enable bit is set but the
     *          indicator bit is not set.
     */
    protected: boolNull getTrailerBit (int32_t enable, int32_t indicator) const;

    /** <i>Optional functionality:</i> Sets the specified bit from the trailer.
     *  @param enable    Bit position of the enable flag.
     *  @param indicator Bit position of the indicator flag.
     *  @param value     The value of the bits: null if the enable bit should not be set, true if
     *                   the enable bit and the indicator bit should be set, false if the enable bit
     *                   should be set but the indicator bit should not be set.
     */
    protected: virtual void setTrailerBit (int32_t enable, int32_t indicator, boolNull value);

    /** Gets the calibrated time indicator flag.
     *  <pre>
     *    true  = The time stamp in this packet is calibrated to an external reference (e.g. atomic clock)
     *    false = The time stamp in this packet is *not* calibrated to an external reference
     *            and may be inaccurate
     *    null  = <i>This information is not available</i>
     *  </pre>
     *  @return true if the flag is set, false if it is not set, null if this optional flag isn't
     *          specified (null is returned for all context packets).
     */
    public: inline boolNull isCalibratedTimeStamp () const  { return getTrailerBit(31, 19); }

    /** Gets the valid data indicator flag.
     *  <pre>
     *    true  = The data in the packet is valid
     *    false = A condition exists that *may* invalidate some or all of the data in the packet
     *    null  = <i>This information is not available</i>
     *  </pre>
     *  <i>Note that the definition of "valid" and "invalid" data is application specific, so it is
     *  important to consult the documentation for the relevant packet class before using this field.</i>
     *  @return true if the flag is set, false if it is not set, null if this optional flag isn't
     *          specified (null is returned for all context packets).
     */
    public: inline boolNull isDataValid () const            { return getTrailerBit(30, 18); }

    /** Gets the reference lock indicator flag.
     *  <pre>
     *    true  = All of the phase-locked loops affecting the data are locked and stable
     *    false = At least one of the phase-locked loops affecting the data is not locked and stable
     *    null  = <i>This information is not available</i>
     *  </pre>
     *  @return true if the flag is set, false if it is not set, null if this optional flag isn't
     *          specified (null is returned for all context packets).
     */
    public: inline boolNull isReferenceLocked () const      { return getTrailerBit(29, 17); }

    /** Gets the AGC/MGC indicator flag.
     *  <pre>
     *    true  = AGC (automatic gain control) is being used
     *    false = MGC (manual gain control) is being used
     *    null  = <i>This information is not available</i>
     *  </pre>
     *  @return true if the flag is set, false if it is not set, null if this optional flag isn't
     *          specified (null is returned for all context packets).
     */
    public: inline boolNull isAutomaticGainControl () const { return getTrailerBit(28, 16); }

    /** Gets the signal detected indicator flag.
     *  <pre>
     *    true  = Some signal detected in this packet
     *    false = No signal detected in this packet
     *    null  = <i>This information is not available</i>
     *  </pre>
     *  <i>Note that the definition of what constitutes a detected signal application specific, so it is
     *  important to conslut the documentation for the relevant packet class before using this field.</i>
     *  @return true if the flag is set, false if it is not set, null if this optional flag isn't
     *          specified (null is returned for all context packets).
     */
    public: inline boolNull isSignalDetected () const       { return getTrailerBit(27, 15); }

    /** Gets the spectral inversion indicator flag.
     *  <pre>
     *    true  = Signal conveyed in the payload has an inverted spectrum w.r.t. an up-stream
     *            reference point
     *    false = Spectrum is not inverted
     *    null  = <i>This information is not available</i>
     *  </pre>
     *  @return true if the flag is set, false if it is not set, null if this optional flag isn't
     *          specified (null is returned for all context packets).
     */
    public: inline boolNull isInvertedSpectrum () const     { return getTrailerBit(26, 14); }

    /** Gets the over-range indicator flag.
     *  <pre>
     *    true  = The data in the payload is invalid since one or more data samples exceeded the
     *            range of the data item
     *    false = No samples over range
     *    null  = <i>This information is not available</i>
     *  </pre>
     *  @return true if the flag is set, false if it is not set, null if this optional flag isn't
     *          specified (null is returned for all context packets).
     */
    public: inline boolNull isOverRange () const            { return getTrailerBit(25, 13); }

    /** Gets the sample loss indicator flag.
     *  <pre>
     *    true  = One or more sample discontinuities present in the data due to processing errors
     *            and/or data overflow
     *    false = No discontinuities present
     *    null  = <i>This information is not available</i>
     *  </pre>
     *  @return true if the flag is set, false if it is not set, null if this optional flag isn't
     *          specified (null is returned for all context packets).
     */
    public: inline boolNull isDiscontinuous () const       { return getTrailerBit(24, 12); }
    
    /** <b>Deprecated:</b> Please change to using <tt>isDiscontinuous</tt>. */
    public: inline __attribute__((deprecated)) __intelattr__((deprecated)) boolNull isDiscontinuious () const { return isDiscontinuous(); }

    /** Gets the #11 user-defined indicator flag. <i>The definition of this flag
     *  should be included in the documentation for the relevant packet class (this flag should be
     *  identified there as "Enable Bit Position 23" and/or "Indicator Bit Position 11").</i>
     *  @return true if the flag is set, false if it is not set, null if the flag isn't specified.
     */
    public: inline boolNull isBit11 () const                { return getTrailerBit(23, 11); }

    /** Gets the #10 user-defined indicator flag. <i>The definition of this flag
     *  should be included in the documentation for the relevant packet class (this flag should be
     *  identified there as "Enable Bit Position 22" and/or "Indicator Bit Position 10").</i>
     *  @return true if the flag is set, false if it is not set, null if the flag isn't specified.
     */
    public: inline boolNull isBit10 () const                { return getTrailerBit(22, 10); }

    /** Gets the #9 user-defined indicator flag. <i>The definition of this flag
     *  should be included in the documentation for the relevant packet class (this flag should be
     *  identified there as "Enable Bit Position 21" and/or "Indicator Bit Position 9").</i>
     *  @return true if the flag is set, false if it is not set, null if the flag isn't specified.
     */
    public: inline boolNull isBit9 () const                 { return getTrailerBit(21,  9); }

    /** Gets the #8 user-defined indicator flag. <i>The definition of this flag
     *  should be included in the documentation for the relevant packet class (this flag should be
     *  identified there as "Enable Bit Position 20" and/or "Indicator Bit Position 8").</i>
     *  @return true if the flag is set, false if it is not set, null if the flag isn't specified.
     */
    public: inline boolNull isBit8 () const                { return getTrailerBit(20,  8); }


    /** Gets the associated packet count. This indicates the number of context
     *  packets with context change or event information relevant to this data packet.
     *  @return The assocaited packet count (0..127), or null if not specified (null is returned
     *          for all context packets).
     */
    public: int8_t getAssocPacketCount () const;


    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Set
    //////////////////////////////////////////////////////////////////////////////////////////////////

    /** <i>Optional functionality:</i> Sets the calibrated time indicator flag.
     *  @param v true if the flag is set, false if not set, null if this optional flag isn't specified.
     *  @throws VRTException If this method is not supported.
     */
    public: inline void setCalibratedTimeStamp (boolNull v)  { setTrailerBit(31, 19, v); }

    /** <i>Optional functionality:</i> Gets the valid data indicator flag.
     *  @param v true if the flag is set, false if not set, null if this optional flag isn't specified.
     *  @throws VRTException If this method is not supported.
     */
    public: inline void setDataValid (boolNull v)            { setTrailerBit(30, 18, v); }

    /** <i>Optional functionality:</i> Gets the reference lock indicator flag.
     *  @param v true if the flag is set, false if not set, null if this optional flag isn't specified.
     *  @throws VRTException If this method is not supported.
     */
    public: inline void setReferenceLocked (boolNull v)      { setTrailerBit(29, 17, v); }

    /** <i>Optional functionality:</i> Gets the AGC/MGC indicator flag.
     *  @param v true if the flag is set, false if not set, null if this optional flag isn't specified.
     *  @throws VRTException If this method is not supported.
     */
    public: inline void setAutomaticGainControl (boolNull v) { setTrailerBit(28, 16, v); }

    /** <i>Optional functionality:</i> Gets the signal detected indicator flag.
     *  @param v true if the flag is set, false if not set, null if this optional flag isn't specified.
     *  @throws VRTException If this method is not supported.
     */
    public: inline void setSignalDetected (boolNull v)       { setTrailerBit(27, 15, v); }

    /** <i>Optional functionality:</i> Gets the spectral inversion indicator flag.
     *  @param v true if the flag is set, false if not set, null if this optional flag isn't specified.
     *  @throws VRTException If this method is not supported.
     */
    public: inline void setInvertedSpectrum (boolNull v)     { setTrailerBit(26, 14, v); }

    /** <i>Optional functionality:</i> Gets the over-range indicator flag.
     *  @param v true if the flag is set, false if not set, null if this optional flag isn't specified.
     *  @throws VRTException If this method is not supported.
     */
    public: inline void setOverRange (boolNull v)            { setTrailerBit(25, 13, v); }

    /** <i>Optional functionality:</i> Gets the sample loss indicator flag.
     *  @param v true if the flag is set, false if not set, null if this optional flag isn't specified.
     *  @throws VRTException If this method is not supported.
     */
    public: inline void setDiscontinuous (boolNull v)       { setTrailerBit(24, 12, v); }
    
    /** <b>Deprecated:</b> Please change to using <tt>setDiscontinuous</tt>. */
    public: inline __attribute__((deprecated)) __intelattr__((deprecated)) void setDiscontinuious (boolNull v) { setDiscontinuous(v); }

    /** <i>Optional functionality:</i> Gets the #11 user-defined indicator flag.
     *  @param v true if the flag is set, false if not set, null if the flag isn't specified.
     *  @throws VRTException If this method is not supported.
     */
    public: inline void setBit11 (boolNull v)                { setTrailerBit(23, 11, v); }

    /** <i>Optional functionality:</i> Gets the #10 user-defined indicator flag.
     *  @param v true if the flag is set, false if not set, null if the flag isn't specified.
     *  @throws VRTException If this method is not supported.
     */
    public: inline void setBit10 (boolNull v)                { setTrailerBit(22, 10, v); }

    /** <i>Optional functionality:</i> Gets the #9 user-defined indicator flag.
     *  @param v true if the flag is set, false if not set, null if the flag isn't specified.
     *  @throws VRTException If this method is not supported.
     */
    public: inline void setBit9 (boolNull v)                 { setTrailerBit(21,  9, v); }

    /** <i>Optional functionality:</i> Gets the #8 user-defined indicator flag.
     *  @param v true if the flag is set, false if not set, null if the flag isn't specified.
     *  @throws VRTException If this method is not supported.
     */
    public: inline void setBit8 (boolNull v)                 { setTrailerBit(20,  8, v); }

    /** <i>Optional functionality:</i> Gets the associated packet count.
     *  This indicates the number of context packets with context change or event information
     *  relevant to this data packet.
     *  @param v The assocaited packet count (0..127), or null if not specified.
     *  @throws VRTException If this method is not supported.
     */
    public: void setAssocPacketCount (int8_t v);

    /** Gets the assumed payload format used when interacting with the data in this packet. This
     *  setting can either be fixed based on the packet class or set via {@link #setPayloadFormat}.
     *  @return The payload format to assume when interacting with this packet. This will return
     *          null if the payload format appears unspecified for this packet class and no call
     *          to {@link #setPayloadFormat} has been made.
     */
    public: virtual inline PayloadFormat getPayloadFormat () const {
      return payloadFormat;
    }

    /** <i>Optional Functionality:</i> Sets the payload format to be assumed when interacting
     *  with the data in this packet. Note that this setting exists only for the lifetime of this
     *  object and is not directly stored within the data packet. <br>
     *  <br>
     *  Many data packet classes use a fixed payload format (see {@link #getPayloadFormat()}) and
     *  will throw an exception if this method is called.
     *  @param pf The payload format to assume.
     *  @throws VRTException If setting the payload format is unsupported.
     */
    public: virtual inline void setPayloadFormat (const PayloadFormat &pf) {
      payloadFormat = pf;
    }

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
    public: inline int32_t getScalarDataLength () const {
      return getScalarDataLength(getPayloadFormat());
    }

    /** <i>Utility method:</i> gets the length of the data in number of scalar elements. Unlike
     *  {@link #getPayloadLength()} this returns the number of data elements, not bytes. For complex
     *  data the real and imaginary components of a single value count as a two elements (see
     *  {@link #getDataLength()}). <br>
     *  @param pf The payload format to assume.
     *  @return The number of elements in the payload.
     *  @throws VRTException If the payload format is null.
     */
    public: inline int32_t getScalarDataLength (const PayloadFormat &pf) const {
      if (isNull(pf)) throw VRTException("Payload format is null");
      int32_t complexMult = (pf.isComplex())? 2 : 1;
      return getDataLength(pf) * complexMult;
    }

    /** Gets the length of the data in number of elements. Unlike {@link #getPayloadLength()}
     *  this returns the number of data elements, not bytes. For complex data the real and imaginary
     *  components of a single value count as a single element (see {@link #getScalarDataLength()}). <br>
     *  <br>
     *  This method can only be used AFTER a payload format for the packet has been specified
     *  (see {@link #getPayloadFormat()}. This method is identical to calling
     *  <tt>p.getDataLength(p.getPayloadFormat())</tt>
     *  @return The number of elements in the payload.
     */
    public: inline int32_t getDataLength () const {
      return getDataLength(getPayloadFormat());
    }

    /** Gets the length of the data in number of elements. Unlike {@link #getPayloadLength()}
     *  this returns the number of data elements, not bytes. For complex data the real and imaginary
     *  components of a single value count as a single element (see {@link #getScalarDataLength()}).
     *  @param pf The payload format to assume.
     *  @return The number of elements in the payload.
     *  @throws VRTException If the payload format is null.
     */
    public: int32_t getDataLength (const PayloadFormat &pf) const;

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
    public: TimeStamp getNextTimeStamp (double sampleRate, const PayloadFormat &pf) const;

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
    public: inline TimeStamp getNextTimeStamp (double sampleRate) const {
      return getNextTimeStamp(sampleRate, getPayloadFormat());
    }

    /** Computes the number of samples lost between two packets.
     *  @param expected   The expected time stamp (predicted from by calling
     *                    <tt>getNextTimeStamp(..)</tt> on the previous packet).
     *  @param sampleRate The applicable sample rate.
     *  @return The number of lost samples. This will be 0 if no samples were lost and -N if there
     *          is an N sample overlap (i.e. extra data). The -N case is usually the result of an
     *          incorrect sample rate and/or incorrect payload format.
     */
    public: int32_t getLostSamples (const TimeStamp &expected, double sampleRate) const;

    /** Computes the number of bytes lost between two packets.
     *  @param expected   The expected time stamp (predicted from by calling
     *                    <tt>getNextTimeStamp(..)</tt> on the previous packet).
     *  @param sampleRate The applicable sample rate.
     *  @param pf         The payload format to assume.
     *  @return The number of lost bytes. This will be 0 if no bytes were lost and -N if there
     *          is an N byte overlap (i.e. extra data). The -N case is usually the result of an
     *          incorrect sample rate and/or incorrect payload format.
     */
    public: inline int32_t getLostBytes (const TimeStamp &expected, double sampleRate, const PayloadFormat &pf) const {
      return getLostSamples(expected, sampleRate) * pf.getDataItemSize()/8;
    }

    /** Computes the number of bytes lost between two packets. <br>
     *  <br>
     *  This method can only be used AFTER a payload format for the packet has been specified
     *  (see {@link #getPayloadFormat()}. This method is identical to calling
     *  <tt>p.getDataLength(p.getPayloadFormat())</tt>
     *  @param expected   The expected time stamp (predicted from by calling
     *                    <tt>getNextTimeStamp(..)</tt> on the previous packet).
     *  @param sampleRate The applicable sample rate.
     *  @return The number of lost bytes. This will be 0 if no bytes were lost and -N if there
     *          is an N byte overlap (i.e. extra data). The -N case is usually the result of an
     *          incorrect sample rate and/or incorrect payload format.
     */
    public: inline int32_t getLostBytes (const TimeStamp &expected, double sampleRate) const {
      return getLostBytes(expected, sampleRate, getPayloadFormat());
    }

    /** Sets the length of the data in number of elements. Unlike {@link #setPayloadLength(int32_t)}
     *  this sets the number of data elements, not bytes. For complex data the real and imaginary
     *  components of a single value count as a single element (see {@link #getScalarDataLength()}). <br>
     *  <br>
     *  This method can only be used AFTER a payload format for the packet has been specified
     *  (see {@link #getPayloadFormat()}. This method is identical to calling
     *  <tt>p.getDataLength(p.getPayloadFormat())</tt>
     *  @param length The number of elements in the payload.
     */
    public: inline void setDataLength (int32_t length) {
      setDataLength(getPayloadFormat(), length);
    }

    /** Sets the length of the data in number of elements. Unlike {@link #setPayloadLength(int32_t)}
     *  this sets the number of data elements, not bytes. For complex data the real and imaginary
     *  components of a single value count as a single element (see {@link #getScalarDataLength()}).
     *  @param pf     The payload format to assume.
     *  @param length The number of elements in the payload.
     *  @throws VRTException If the payload format is null.
     */
    public: void setDataLength (const PayloadFormat &pf, int32_t length);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // GET WITH GIVEN PAYLOAD FORMAT
    //////////////////////////////////////////////////////////////////////////////////////////////////
    /** Unpacks the data and returns it in a native byte buffer. This is basically a byte-for-byte
     *  copy of the payload, except that a conversion is done from <tt>BIG_ENDIAN</tt> to the native
     *  byte ordering. This method is intended for use within existing programming frameworks that
     *  already provide type conversions. <b>This method does not convert the values to byte data,
     *  only copies the data to what is effectively a <tt>void*</tt> buffer in the native byte order.</b> <br>
     *  <br>
     *  For 8-bit, 4-bit and 1-bit types, this is implemented as a fast array copy with no
     *  translation being done with regards to byte order. Further, with 4-bit and 1-bit types an
     *  assumption is made that the number of entries is an even multiple of one octet (i.e. 4-bit
     *  values must be in multiples of 2 and 1-bit values in multiples of 8).
     *  @param pf      The payload format to assume.
     *  @param array   The array to insert the data into.
     *  @param offset  The array offset.
     *  @param convert Convert byte order to match native representation? Set this to false if it
     *                 is necessary to keep the data in big-endian format.
     *  @throws VRTException If the payload format is null or if the array is null.
     *  @throws VRTException If the payload format is anything other than
     *          <tt>PayloadFormat_INT4</tt>,    <tt>PayloadFormat_INT8</tt>,
     *          <tt>PayloadFormat_INT16</tt>,   <tt>PayloadFormat_INT32</tt>,   <tt>PayloadFormat_INT64</tt>,
     *          <tt>PayloadFormat_FLOAT32</tt>, <tt>PayloadFormat_DOUBLE64</tt>
     *          <tt>PayloadFormat_UINT1</tt>,   <tt>PayloadFormat_UINT4</tt>,   <tt>PayloadFormat_UINT8</tt>,
     *          <tt>PayloadFormat_UINT16</tt>,  <tt>PayloadFormat_UINT32</tt>,  <tt>PayloadFormat_UINT64</tt>.
     *  @throws VRTException If the array length is shorter than
     *          <tt>offset+{@link #getPayloadLength()}</tt>.
     */
    public: void swapPayloadBytes(const PayloadFormat &pf, const void* array);

    public: inline void getData (const PayloadFormat &pf, vector<char> &array, size_t offset, bool convert=true) const {
      if (array.size() < (offset+getPayloadLength())) {
        throw VRTException("Illegal offset (%d) for array of length %d", offset, array.size());
      }
      getData(pf, (void*)&array[offset], convert);
    }
    public: void* getData_normal(const PayloadFormat &pf, int position);
    public: void* getData();

    /** Unpacks the data and returns it in a native byte buffer. This is basically a byte-for-byte
     *  copy of the payload, except that a conversion is done from <tt>BIG_ENDIAN</tt> to the native
     *  byte ordering. This method is intended for use within existing programming frameworks that
     *  already provide type conversions. <b>This method does not convert the values to byte data,
     *  only copies the data to what is effectively a <tt>void*</tt> buffer in the native byte order.</b> <br>
     *  <br>
     *  For 8-bit, 4-bit and 1-bit types, this is implemented as a fast array copy with no
     *  translation being done with regards to byte order. Further, with 4-bit and 1-bit types an
     *  assumption is made that the number of entries is an even multiple of one octet (i.e. 4-bit
     *  values must be in multiples of 2 and 1-bit values in multiples of 8).
     *  @param pf      The payload format to assume.
     *  @param array   The array to insert the data into.
     *  @param convert Convert byte order to match native representation? Set this to false if it
     *                 is necessary to keep the data in big-endian format.
     *  @return <tt>array</tt>
     *  @throws VRTException If the payload format is null or if the array is null.
     *  @throws VRTException If the payload format is anything other than
     *          <tt>PayloadFormat_INT4</tt>,    <tt>PayloadFormat_INT8</tt>,
     *          <tt>PayloadFormat_INT16</tt>,   <tt>PayloadFormat_INT32</tt>,   <tt>PayloadFormat_INT64</tt>,
     *          <tt>PayloadFormat_FLOAT32</tt>, <tt>PayloadFormat_DOUBLE64</tt>
     *          <tt>PayloadFormat_UINT1</tt>,   <tt>PayloadFormat_UINT4</tt>,   <tt>PayloadFormat_UINT8</tt>,
     *          <tt>PayloadFormat_UINT16</tt>,  <tt>PayloadFormat_UINT32</tt>,  <tt>PayloadFormat_UINT64</tt>.
     *  @throws VRTException If the array length is shorter than
     *          <tt>offset+{@link #getPayloadLength()}</tt>.
     */
    public: void* getData (const PayloadFormat &pf, void *array, bool convert=true) const;


    /** Unpacks the data and returns it as a double array. If the underlying data is not double,
     *  it will be converted to double and any values that fall outside of the range of a double will
     *  will have unpredictable output. If the payload format is incorrectly specified, the output of
     *  this method will be useless.
     *  @param pf The payload format to assume.
     *  @return The data elements.
     *  @throws VRTException If the payload format is null.
     */
    public: inline vector<double> getDataDouble (const PayloadFormat &pf) const {
      if (isNull(pf)) throw VRTException("Payload format is null");
      int32_t len = getScalarDataLength(pf);
      if (pf.getDataType() == DataType_Double) {
        // Fast version where no conversion (other than byte order) is required
        vector<double> array(len);
        getData(pf, &array[0]);
        return array;
      }
      else {
        vector<double> array(len);
        PackUnpack::unpackAsDouble(pf, &bbuf[0], getHeaderLength(), &array[0], NULL, NULL, len);
        return array;
      }
    }

    /** Unpacks the data and returns it as a float array. If the underlying data is not float,
     *  it will be converted to float and any values that fall outside of the range of a float will
     *  will have unpredictable output. If the payload format is incorrectly specified, the output of
     *  this method will be useless.
     *  @param pf The payload format to assume.
     *  @return The data elements.
     *  @throws VRTException If the payload format is null.
     */
    public: inline vector<float> getDataFloat (const PayloadFormat &pf) const {
      if (isNull(pf)) throw VRTException("Payload format is null");
      int32_t len = getScalarDataLength(pf);
      if (pf.getDataType() == DataType_Float) {
        // Fast version where no conversion (other than byte order) is required
        vector<float> array(len);
        getData(pf, &array[0]);
        return array;
      }
      else {
        vector<float> array(len);
        PackUnpack::unpackAsFloat(pf, &bbuf[0], getHeaderLength(), &array[0], NULL, NULL, len);
        return array;
      }
    }

    /** Unpacks the data and returns it as a long array. If the underlying data is not long,
     *  it will be converted to long and any values that fall outside of the range of a long will
     *  will have unpredictable output. If the payload format is incorrectly specified, the output of
     *  this method will be useless.
     *  @param pf The payload format to assume.
     *  @return The data elements.
     *  @throws VRTException If the payload format is null.
     */
    public: inline vector<int64_t> getDataLong (const PayloadFormat &pf) const {
      if (isNull(pf)) throw VRTException("Payload format is null");
      int32_t len = getScalarDataLength(pf);
      if (pf.getDataType() == DataType_Int64) {
        // Fast version where no conversion (other than byte order) is required
        vector<int64_t> array(len);
        getData(pf, &array[0]);
        return array;
      }
      else {
        vector<int64_t> array(len);
        PackUnpack::unpackAsLong(pf, &bbuf[0], getHeaderLength(), &array[0], NULL, NULL, len);
        return array;
      }
    }

    /** Unpacks the data and returns it as a int array. If the underlying data is not int,
     *  it will be converted to int and any values that fall outside of the range of a int will
     *  will have unpredictable output. If the payload format is incorrectly specified, the output of
     *  this method will be useless.
     *  @param pf The payload format to assume.
     *  @return The data elements.
     *  @throws VRTException If the payload format is null.
     */
    public: inline vector<int32_t> getDataInt (const PayloadFormat &pf) const {
      if (isNull(pf)) throw VRTException("Payload format is null");
      int32_t len = getScalarDataLength(pf);
      if (pf.getDataType() == DataType_Int32) {
        // Fast version where no conversion (other than byte order) is required
        vector<int32_t> array(len);
        getData(pf, &array[0]);
        return array;
      }
      else {
        vector<int32_t> array(len);
        PackUnpack::unpackAsInt(pf, &bbuf[0], getHeaderLength(), &array[0], NULL, NULL, len);
        return array;
      }
    }

    /** Unpacks the data and returns it as a short array. If the underlying data is not short,
     *  it will be converted to short and any values that fall outside of the range of a short will
     *  will have unpredictable output. If the payload format is incorrectly specified, the output of
     *  this method will be useless.
     *  @param pf The payload format to assume.
     *  @return The data elements.
     *  @throws VRTException If the payload format is null.
     */
    public: inline vector<int16_t> getDataShort (const PayloadFormat &pf) const {
      if (isNull(pf)) throw VRTException("Payload format is null");
      int32_t len = getScalarDataLength(pf);
      if (pf.getDataType() == DataType_Int16) {
        // Fast version where no conversion (other than byte order) is required
        vector<int16_t> array(len);
        getData(pf, &array[0]);
        return array;
      }
      else {
        vector<int16_t> array(len);
        PackUnpack::unpackAsShort(pf, &bbuf[0], getHeaderLength(), &array[0], NULL, NULL, len);
        return array;
      }
    }


    void* getDataShort (const PayloadFormat &pf, bool raw);


    /** Unpacks the data and returns it as a byte array. If the underlying data is not byte,
     *  it will be converted to byte and any values that fall outside of the range of a byte will
     *  will have unpredictable output. If the payload format is incorrectly specified, the output of
     *  this method will be useless.
     *  @param pf The payload format to assume.
     *  @return The data elements.
     *  @throws VRTException If the payload format is null.
     */
    public: inline vector<int8_t> getDataByte (const PayloadFormat &pf) const {
      if (isNull(pf)) throw VRTException("Payload format is null");
      int32_t len = getScalarDataLength(pf);
      if (pf.getDataType() == DataType_Int8) {
        // Fast version where no conversion (other than byte order) is required
        vector<int8_t> vector(len);
        getData(pf, &vector[0]);
        return vector;
      }
      else {
        vector<int8_t> array(len);
        PackUnpack::unpackAsByte(pf, &bbuf[0], getHeaderLength(), &array[0], NULL, NULL, len);
        return array;
      }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // GET WITH PRE-DEFINED PAYLOAD FORMAT
    //////////////////////////////////////////////////////////////////////////////////////////////////
    /** Unpacks the data and returns it in a native byte buffer. This is basically a byte-for-byte
     *  copy of the payload, except that a conversion is done from <tt>BIG_ENDIAN</tt> to the native
     *  byte ordering. This method is intended for use within existing programming frameworks that
     *  already provide type conversions. <b>This method does not convert the values to byte data,
     *  only copies the data to what is effectively a <tt>void*</tt> buffer in the native byte order.</b> <br>
     *  <br>
     *  For 8-bit, 4-bit and 1-bit types, this is implemented as a fast array copy with no
     *  translation being done with regards to byte order. Further, with 4-bit and 1-bit types an
     *  assumption is made that the number of entries is an even multiple of one octet (i.e. 4-bit
     *  values must be in multiples of 2 and 1-bit values in multiples of 8). <br>
     *  <br>
     *  This method can only be used AFTER a payload format for the packet has been specified
     *  (see {@link #getPayloadFormat()}. This method is identical to calling
     *  <tt>p.getData(p.getPayloadFormat(),array,offset)</tt>
     *  @param array  The array to insert the data into.
     *  @param offset The array offset.
     *  @return <tt>array</tt>
     *  @throws VRTException If the payload format is null or if the array is null.
     *  @throws VRTException If the payload format is anything other than
     *          <tt>PayloadFormat_INT4</tt>,    <tt>PayloadFormat_INT8</tt>,
     *          <tt>PayloadFormat_INT16</tt>, <tt>PayloadFormat_INT32</tt>,   <tt>PayloadFormat_INT64</tt>,
     *          <tt>PayloadFormat_FLOAT32</tt>, <tt>PayloadFormat_DOUBLE64</tt>
     *          <tt>PayloadFormat_UINT1</tt>, <tt>PayloadFormat_UINT4</tt>,    <tt>PayloadFormat_UINT8</tt>,
     *          <tt>PayloadFormat_UINT16</tt>, <tt>PayloadFormat_UINT32</tt>,   <tt>PayloadFormat_UINT64</tt>.
     *  @throws VRTException If the array length is shorter than
     *          <tt>offset+{@link #getPayloadLength()}</tt>.
     */
    public: inline void getData (vector<char> array, size_t offset) const {
      getData(getPayloadFormat(), array, offset);
    }

    /** Unpacks the data and returns it in a native byte buffer. This is basically a byte-for-byte
     *  copy of the payload, except that a conversion is done from <tt>BIG_ENDIAN</tt> to the native
     *  byte ordering. This method is intended for use within existing programming frameworks that
     *  already provide type conversions. <b>This method does not convert the values to byte data,
     *  only copies the data to what is effectively a <tt>void*</tt> buffer in the native byte order.</b> <br>
     *  <br>
     *  For 8-bit, 4-bit and 1-bit types, this is implemented as a fast array copy with no
     *  translation being done with regards to byte order. Further, with 4-bit and 1-bit types an
     *  assumption is made that the number of entries is an even multiple of one octet (i.e. 4-bit
     *  values must be in multiples of 2 and 1-bit values in multiples of 8). <br>
     *  <br>
     *  This method can only be used AFTER a payload format for the packet has been specified
     *  (see {@link #getPayloadFormat()}. This method is identical to calling
     *  <tt>p.getData(p.getPayloadFormat(),array)</tt>
     *  @param array  The array to insert the data into.
     *  @return <tt>array</tt>
     *  @throws VRTException If the payload format is null or if the array is null.
     *  @throws VRTException If the payload format is anything other than
     *          <tt>PayloadFormat_INT4</tt>,    <tt>PayloadFormat_INT8</tt>,
     *          <tt>PayloadFormat_INT16</tt>, <tt>PayloadFormat_INT32</tt>,   <tt>PayloadFormat_INT64</tt>,
     *          <tt>PayloadFormat_FLOAT32</tt>, <tt>PayloadFormat_DOUBLE64</tt>
     *          <tt>PayloadFormat_UINT1</tt>, <tt>PayloadFormat_UINT4</tt>,    <tt>PayloadFormat_UINT8</tt>,
     *          <tt>PayloadFormat_UINT16</tt>, <tt>PayloadFormat_UINT32</tt>,   <tt>PayloadFormat_UINT64</tt>.
     */
    public: inline void *getData (void *array) const {
      return getData(getPayloadFormat(), array);
    }

    /** Unpacks the data and returns it as a double array. If the underlying data is not double,
     *  it will be converted to double and any values that fall outside of the range of a double will
     *  will have unpredictable output. If the payload format is incorrectly specified, the output of
     *  this method will be useless. <br>
     *  <br>
     *  This method can only be used AFTER a payload format for the packet has been specified
     *  (see {@link #getPayloadFormat()}. This method is identical to calling
     *  <tt>p.getDataDouble(p.getPayloadFormat())</tt>
     *  @return The data elements.
     *  @throws VRTException If the payload format is null.
     */
    public: inline vector<double> getDataDouble () const {
      return getDataDouble(getPayloadFormat());
    }

    /** Unpacks the data and returns it as a float array. If the underlying data is not double,
     *  it will be converted to float and any values that fall outside of the range of a float will
     *  will have unpredictable output. If the payload format is incorrectly specified, the output of
     *  this method will be useless. <br>
     *  <br>
     *  This method can only be used AFTER a payload format for the packet has been specified
     *  (see {@link #getPayloadFormat()}. This method is identical to calling
     *  <tt>p.getDataFloat(p.getPayloadFormat())</tt>
     *  @return The data elements.
     *  @throws VRTException If the payload format is null.
     */
    public: inline vector<float> getDataFloat () const {
      return getDataFloat( getPayloadFormat());
    }

    /** Unpacks the data and returns it as a long array. If the underlying data is not double,
     *  it will be converted to long and any values that fall outside of the range of a long will
     *  will have unpredictable output. If the payload format is incorrectly specified, the output of
     *  this method will be useless. <br>
     *  <br>
     *  This method can only be used AFTER a payload format for the packet has been specified
     *  (see {@link #getPayloadFormat()}. This method is identical to calling
     *  <tt>p.getDataLong(p.getPayloadFormat())</tt>
     *  @return The data elements.
     *  @throws VRTException If the payload format is null.
     */
    public: inline vector<int64_t> getDataLong () const {
      return getDataLong(  getPayloadFormat());
    }

    /** Unpacks the data and returns it as a int array. If the underlying data is not double,
     *  it will be converted to int and any values that fall outside of the range of a int will
     *  will have unpredictable output. If the payload format is incorrectly specified, the output of
     *  this method will be useless. <br>
     *  <br>
     *  This method can only be used AFTER a payload format for the packet has been specified
     *  (see {@link #getPayloadFormat()}. This method is identical to calling
     *  <tt>p.getDataInt(p.getPayloadFormat())</tt>
     *  @return The data elements.
     *  @throws VRTException If the payload format is null.
     */
    public: inline vector<int32_t> getDataInt () const {
      return getDataInt(   getPayloadFormat());
    }

    /** Unpacks the data and returns it as a byte array. If the underlying data is not double,
     *  it will be converted to byte and any values that fall outside of the range of a byte will
     *  will have unpredictable output. If the payload format is incorrectly specified, the output of
     *  this method will be useless. <br>
     *  <br>
     *  This method can only be used AFTER a payload format for the packet has been specified
     *  (see {@link #getPayloadFormat()}. This method is identical to calling
     *  <tt>p.getDataShort(p.getPayloadFormat())</tt>
     *  @return The data elements.
     *  @throws VRTException If the payload format is null.
     */
    public: inline vector<int16_t> getDataShort () const {
      return getDataShort( getPayloadFormat());
    }

    /** Unpacks the data and returns it as a byte array. If the underlying data is not double,
     *  it will be converted to byte and any values that fall outside of the range of a byte will
     *  will have unpredictable output. If the payload format is incorrectly specified, the output of
     *  this method will be useless. <br>
     *  <br>
     *  This method can only be used AFTER a payload format for the packet has been specified
     *  (see {@link #getPayloadFormat()}. This method is identical to calling
     *  <tt>p.getDataByte(p.getPayloadFormat())</tt>
     *  @return The data elements.
     *  @throws VRTException If the payload format is null.
     */
    public: inline vector<int8_t> getDataByte () const {
      return getDataByte(  getPayloadFormat());
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // SET WITH GIVEN PAYLOAD FORMAT
    //////////////////////////////////////////////////////////////////////////////////////////////////
    /** Packs the data from a native byte buffer. This is basically a byte-for-byte
     *  copy of the payload, except that a conversion is done from the native to <tt>BIG_ENDIAN</tt>
     *  byte ordering. This method is intended for use within existing programming frameworks that
     *  already provide type conversions. <b>This method does not convert the values from byte data,
     *  only copies the data from what is effectively a <tt>void*</tt> buffer in the native byte order.</b>
     *  @param pf      The payload format to assume.
     *  @param array   The array to insert the data into.
     *  @param offset  The array offset.
     *  @param length  The number of bytes to pack (must be a multiple of 8 for <tt>PayloadFormat_INT64</tt>
     *                 or <tt>PayloadFormat_DOUBLE64</tt> or a multiple of 4 for all others).
     *  @param convert Convert byte order to match native representation? Set this to false if it
     *                 is necessary to keep the data in big-endian format.
     *  @throws VRTException If the payload format is null or if the array is null.
     *  @throws VRTException If the payload format is anything other than
     *          <tt>PayloadFormat_INT4</tt>,    <tt>PayloadFormat_INT8</tt>,
     *          <tt>PayloadFormat_INT16</tt>, <tt>PayloadFormat_INT32</tt>,   <tt>PayloadFormat_INT64</tt>,
     *          <tt>PayloadFormat_FLOAT32</tt>, <tt>PayloadFormat_DOUBLE64</tt>
     *          <tt>PayloadFormat_UINT1</tt>, <tt>PayloadFormat_UINT4</tt>,    <tt>PayloadFormat_UINT8</tt>,
     *          <tt>PayloadFormat_UINT16</tt>, <tt>PayloadFormat_UINT32</tt>,   <tt>PayloadFormat_UINT64</tt>.
     *  @throws VRTException If the array length is shorter than
     *          <tt>offset+{@link #getPayloadLength()}</tt>.
     *  @throws VRTException If the length is invalid.
     */
    public: inline void setData (const PayloadFormat &pf, const vector<char> &array, size_t offset, int32_t length, bool convert=true) {
      if (array.size() < (offset+getPayloadLength())) {
        throw VRTException("Illegal offset (%d) for array of length %d", offset, array.size());
      }
      setData(pf, (void*)&array[offset], length, convert);
    }

    /** Packs the data from a native byte buffer. This is basically a byte-for-byte
     *  copy of the payload, except that a conversion is done from the native to <tt>BIG_ENDIAN</tt>
     *  byte ordering. This method is intended for use within existing programming frameworks that
     *  already provide type conversions. <b>This method does not convert the values from byte data,
     *  only copies the data from what is effectively a <tt>void*</tt> buffer in the native byte order.</b>
     *  @param pf      The payload format to assume.
     *  @param array   The array to insert the data into.
     *  @param length  The number of bytes to pack (must be a multiple of 8 for <tt>PayloadFormat_INT64</tt>
     *                 or <tt>PayloadFormat_DOUBLE64</tt> or a multiple of 4 for all others).
     *  @param convert Convert byte order to match native representation? Set this to false if it
     *                 is necessary to keep the data in big-endian format.
     *  @throws VRTException If the payload format is null or if the array is null.
     *  @throws VRTException If the payload format is anything other than
     *          <tt>PayloadFormat_INT4</tt>,    <tt>PayloadFormat_INT8</tt>,
     *          <tt>PayloadFormat_INT16</tt>, <tt>PayloadFormat_INT32</tt>,   <tt>PayloadFormat_INT64</tt>,
     *          <tt>PayloadFormat_FLOAT32</tt>, <tt>PayloadFormat_DOUBLE64</tt>
     *          <tt>PayloadFormat_UINT1</tt>, <tt>PayloadFormat_UINT4</tt>,    <tt>PayloadFormat_UINT8</tt>,
     *          <tt>PayloadFormat_UINT16</tt>, <tt>PayloadFormat_UINT32</tt>,   <tt>PayloadFormat_UINT64</tt>.
     *  @throws VRTException If the length is invalid.
     */
    public: void setData (const PayloadFormat &pf, const void *array, int32_t length, bool convert=true);

    /** Packs the data using the values from a double array. If the underlying data is not double,
     *  it will be converted from double and any values that fall outside of the range of a of the
     *  payload format will have unpredictable output. This method also has the effect of setting
     *  the size of the data (see {@link #setDataLength}) and indirectly the payload and packet.
     *  @param pf The payload format to assume.
     *  @param array The values to insert.
     *  @throws VRTException If the payload format is null.
     */
    public: inline void setDataDouble (const PayloadFormat &pf, const vector<double> &array) {
      if (isNull(pf)) throw VRTException("Payload format is null");
      if (pf.getDataType() == DataType_Double) {
        // Fast version where no conversion (other than byte order) is required
        int32_t len = array.size() * 8;
        setPayloadLength(len);
        setData(pf, &array[0], len);
      }
      else {
        PackUnpack::packAsDouble(pf, &bbuf[0], getHeaderLength(), &array[0], NULL, NULL, array.size());
      }
    }

    /** Packs the data using the values from a float array. If the underlying data is not float,
     *  it will be converted from float and any values that fall outside of the range of a of the
     *  payload format will have unpredictable output. This method also has the effect of setting
     *  the size of the data (see {@link #setDataLength}) and indirectly the payload and packet.
     *  @param pf The payload format to assume.
     *  @param array The values to insert.
     *  @throws VRTException If the payload format is null.
     */
    public: inline void setDataFloat (const PayloadFormat &pf, const vector<float> &array) {
      if (isNull(pf)) throw VRTException("Payload format is null");
      if (pf.getDataType() == DataType_Float) {
        // Fast version where no conversion (other than byte order) is required
        int32_t len = array.size() * 4;
        setPayloadLength(len);
        setData(pf, &array[0], len);
      }
      else {
        PackUnpack::packAsFloat(pf, &bbuf[0], getHeaderLength(), &array[0], NULL, NULL, array.size());
      }
    }

    /** Packs the data using the values from a long array. If the underlying data is not long,
     *  it will be converted from long and any values that fall outside of the range of a of the
     *  payload format will have unpredictable output. This method also has the effect of setting
     *  the size of the data (see {@link #setDataLength}) and indirectly the payload and packet.
     *  @param pf The payload format to assume.
     *  @param array The values to insert.
     *  @throws VRTException If the payload format is null.
     */
    public: inline void setDataLong (const PayloadFormat &pf, const vector<int64_t> &array) {
      if (isNull(pf)) throw VRTException("Payload format is null");
      if (pf.getDataType() == DataType_Int64) {
        // Fast version where no conversion (other than byte order) is required
        int32_t len = array.size() * 8;
        setPayloadLength(len);
        setData(pf, &array[0], len);
      }
      else {
        PackUnpack::packAsLong(pf, &bbuf[0], getHeaderLength(), &array[0], NULL, NULL, array.size());
      }
    }

    /** Packs the data using the values from a int array. If the underlying data is not int,
     *  it will be converted from int and any values that fall outside of the range of a of the
     *  payload format will have unpredictable output. This method also has the effect of setting
     *  the size of the data (see {@link #setDataLength}) and indirectly the payload and packet.
     *  @param pf The payload format to assume.
     *  @param array The values to insert.
     *  @throws VRTException If the payload format is null.
     */
    public: inline void setDataInt (const PayloadFormat &pf, const vector<int32_t> &array) {
      if (isNull(pf)) throw VRTException("Payload format is null");
      if (pf.getDataType() == DataType_Int32) {
        // Fast version where no conversion (other than byte order) is required
        int32_t len = array.size() * 4;
        setPayloadLength(len);
        setData(pf, &array[0], len);
      }
      else {
        PackUnpack::packAsInt(pf, &bbuf[0], getHeaderLength(), &array[0], NULL, NULL, array.size());
      }
    }

    /** Packs the data using the values from a short array. If the underlying data is not short,
     *  it will be converted from short and any values that fall outside of the range of a of the
     *  payload format will have unpredictable output. This method also has the effect of setting
     *  the size of the data (see {@link #setDataLength}) and indirectly the payload and packet.
     *  @param pf The payload format to assume.
     *  @param array The values to insert.
     *  @throws VRTException If the payload format is null.
     */
    public: inline void setDataShort (const PayloadFormat &pf, const vector<int16_t> &array) {
      if (isNull(pf)) throw VRTException("Payload format is null");
      if (pf.getDataType() == DataType_Int16) {
        // Fast version where no conversion (other than byte order) is required
        int32_t len = array.size() * 2;
        setPayloadLength(len);
        setData(pf, &array[0], len);
      }
      else {
        PackUnpack::packAsShort(pf, &bbuf[0], getHeaderLength(), &array[0], NULL, NULL, array.size());
      }
    }

    public: inline void setDataShort (const PayloadFormat &pf, int16_t *array, int length) {
          if (isNull(pf)) throw VRTException("Payload format is null");
          if (pf.getDataType() == DataType_Int16) {
            // Fast version where no conversion (other than byte order) is required
            int32_t len = length * 2;
            setPayloadLength(len);
            setData(pf, &array[0], len);
          }
          else {
            PackUnpack::packAsShort(pf, &bbuf[0], getHeaderLength(), &array[0], NULL, NULL, length);
          }
        }
    /** Packs the data using the values from a byte array. If the underlying data is not byte,
     *  it will be converted from byte and any values that fall outside of the range of a of the
     *  payload format will have unpredictable output. This method also has the effect of setting
     *  the size of the data (see {@link #setDataLength}) and indirectly the payload and packet.
     *  @param pf The payload format to assume.
     *  @param array The values to insert.
     *  @throws VRTException If the payload format is null.
     */
    public: inline void setDataByte (const PayloadFormat &pf, const vector<int8_t> &array) {
      if (isNull(pf)) throw VRTException("Payload format is null");
      if (pf.getDataType() == DataType_Int8) {
        // Fast version where no conversion is required
        int32_t len = array.size();
        setPayloadLength(len);
        setData(pf, &array[0], len);
      }
      else {
        setDataLength(pf, array.size());
        PackUnpack::packAsByte(pf, &bbuf[0], getHeaderLength(), &array[0], NULL, NULL, array.size());
      }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // SET WITH PRE-DEFINED PAYLOAD FORMAT
    //////////////////////////////////////////////////////////////////////////////////////////////////
    /** Packs the data from a native byte buffer. This is basically a byte-for-byte
     *  copy of the payload, except that a conversion is done from the native to <tt>BIG_ENDIAN</tt>
     *  byte ordering. This method is intended for use within existing programming frameworks that
     *  already provide type conversions. <b>This method does not convert the values from byte data,
     *  only copies the data from what is effectively a <tt>void*</tt> buffer in the native byte order.</b> <br>
     *  <br>
     *   This method can only be used AFTER a payload format for the packet has been specified
     *  (see {@link #getPayloadFormat()}. This method is identical to calling
     *  <tt>p.setData(p.getPayloadFormat(),array,offset,length)</tt>
     *  @param array  The array to insert the data into.
     *  @param offset The array offset.
     *  @param length The number of bytes to pack (must be a multiple of 8 for <tt>PayloadFormat_INT64</tt>
     *                or <tt>PayloadFormat_DOUBLE64</tt> or a multiple of 4 for all others).
     *  @throws VRTException If the payload format is null or if the array is null.
     *  @throws VRTException If the payload format is anything other than
     *          <tt>PayloadFormat_INT4</tt>,    <tt>PayloadFormat_INT8</tt>,
     *          <tt>PayloadFormat_INT16</tt>, <tt>PayloadFormat_INT32</tt>,   <tt>PayloadFormat_INT64</tt>,
     *          <tt>PayloadFormat_FLOAT32</tt>, <tt>PayloadFormat_DOUBLE64</tt>
     *          <tt>PayloadFormat_UINT1</tt>, <tt>PayloadFormat_UINT4</tt>,    <tt>PayloadFormat_UINT8</tt>,
     *          <tt>PayloadFormat_UINT16</tt>, <tt>PayloadFormat_UINT32</tt>,   <tt>PayloadFormat_UINT64</tt>.
     *  @throws VRTException If the array length is shorter than <tt>offset+{@link #getPayloadLength()}</tt>.
     *  @throws VRTException If the length is invalid.
     */
    public: inline void setData (const vector<char> &array, size_t offset, int32_t length) {
        setData(getPayloadFormat(), array, offset, length);
      }

    /** Packs the data from a native byte buffer. This is basically a byte-for-byte
     *  copy of the payload, except that a conversion is done from the native to <tt>BIG_ENDIAN</tt>
     *  byte ordering. This method is intended for use within existing programming frameworks that
     *  already provide type conversions. <b>This method does not convert the values from byte data,
     *  only copies the data from what is effectively a <tt>void*</tt> buffer in the native byte order.</b> <br>
     *  <br>
     *   This method can only be used AFTER a payload format for the packet has been specified
     *  (see {@link #getPayloadFormat()}. This method is identical to calling
     *  <tt>p.setData(p.getPayloadFormat(),array,offset,length)</tt>
     *  @param array  The array to insert the data into.
     *  @param length The number of bytes to pack (must be a multiple of 8 for <tt>PayloadFormat_INT64</tt>
     *                or <tt>PayloadFormat_DOUBLE64</tt> or a multiple of 4 for all others).
     *  @throws VRTException If the payload format is null or if the array is null.
     *  @throws VRTException If the payload format is anything other than
     *          <tt>PayloadFormat_INT4</tt>,    <tt>PayloadFormat_INT8</tt>,
     *          <tt>PayloadFormat_INT16</tt>, <tt>PayloadFormat_INT32</tt>,   <tt>PayloadFormat_INT64</tt>,
     *          <tt>PayloadFormat_FLOAT32</tt>, <tt>PayloadFormat_DOUBLE64</tt>
     *          <tt>PayloadFormat_UINT1</tt>, <tt>PayloadFormat_UINT4</tt>,    <tt>PayloadFormat_UINT8</tt>,
     *          <tt>PayloadFormat_UINT16</tt>, <tt>PayloadFormat_UINT32</tt>,   <tt>PayloadFormat_UINT64</tt>.
     *  @throws VRTException If the array length is shorter than <tt>offset+{@link #getPayloadLength()}</tt>.
     *  @throws VRTException If the length is invalid.
     */
    public: inline void setData (const void *array, int32_t length) {
      setData(getPayloadFormat(), array, length);
    }

    /** Packs the data using the values from a double array. If the underlying data is not double,
     *  it will be converted from double and any values that fall outside of the range of a of the
     *  payload format will have unpredictable output. This method also has the effect of setting
     *  the size of the data (see {@link #setDataLength}) and indirectly the payload and packet. <br>
     *  <br>
     *   This method can only be used AFTER a payload format for the packet has been specified
     *  (see {@link #getPayloadFormat()}. This method is identical to calling
     *  <tt>p.setDataDouble(p.getPayloadFormat(),array)</tt>
     *  @param array The values to insert.
     *  @throws VRTException If the payload format is null.
     */
    public: inline void setDataDouble (const vector<double> &array) {
      setDataDouble(getPayloadFormat(), array);
    }

    /** Packs the data using the values from a float array. If the underlying data is not float,
     *  it will be converted from float and any values that fall outside of the range of a of the
     *  payload format will have unpredictable output. This method also has the effect of setting
     *  the size of the data (see {@link #setDataLength}) and indirectly the payload and packet. <br>
     *  <br>
     *   This method can only be used AFTER a payload format for the packet has been specified
     *  (see {@link #getPayloadFormat()}. This method is identical to calling
     *  <tt>p.setDataFloat(p.getPayloadFormat(),array)</tt>
     *  @param array The values to insert.
     *  @throws VRTException If the payload format is null.
     */
    public: inline void setDataFloat (const vector<float> &array) {
      setDataFloat(getPayloadFormat(), array);
    }

    /** Packs the data using the values from a long array. If the underlying data is not long,
     *  it will be converted from long and any values that fall outside of the range of a of the
     *  payload format will have unpredictable output. This method also has the effect of setting
     *  the size of the data (see {@link #setDataLength}) and indirectly the payload and packet. <br>
     *  <br>
     *   This method can only be used AFTER a payload format for the packet has been specified
     *  (see {@link #getPayloadFormat()}. This method is identical to calling
     *  <tt>p.setDataLong(p.getPayloadFormat(),array)</tt>
     *  @param array The values to insert.
     *  @throws VRTException If the payload format is null.
     */
    public: inline void setDataLong (const vector<int64_t> &array) {
      setDataLong(getPayloadFormat(), array);
    }

    /** Packs the data using the values from a int array. If the underlying data is not int,
     *  it will be converted from int and any values that fall outside of the range of a of the
     *  payload format will have unpredictable output. This method also has the effect of setting
     *  the size of the data (see {@link #setDataLength}) and indirectly the payload and packet. <br>
     *  <br>
     *   This method can only be used AFTER a payload format for the packet has been specified
     *  (see {@link #getPayloadFormat()}. This method is identical to calling
     *  <tt>p.setDataInt(p.getPayloadFormat(),array)</tt>
     *  @param array The values to insert.
     *  @throws VRTException If the payload format is null.
     */
    public: inline void setDataInt (const vector<int32_t> &array) {
      setDataInt(getPayloadFormat(), array);
    }

    /** Packs the data using the values from a short array. If the underlying data is not short,
     *  it will be converted from short and any values that fall outside of the range of a of the
     *  payload format will have unpredictable output. This method also has the effect of setting
     *  the size of the data (see {@link #setDataLength}) and indirectly the payload and packet. <br>
     *  <br>
     *   This method can only be used AFTER a payload format for the packet has been specified
     *  (see {@link #getPayloadFormat()}. This method is identical to calling
     *  <tt>p.setDataShort(p.getPayloadFormat(),array)</tt>
     *  @param array The values to insert.
     *  @throws VRTException If the payload format is null.
     */
    public: inline void setDataShort (const vector<int16_t> &array) {
      setDataShort(getPayloadFormat(), array);
    }

    /** Packs the data using the values from a byte array. If the underlying data is not byte,
     *  it will be converted from byte and any values that fall outside of the range of a of the
     *  payload format will have unpredictable output. This method also has the effect of setting
     *  the size of the data (see {@link #setDataLength}) and indirectly the payload and packet. <br>
     *  <br>
     *   This method can only be used AFTER a payload format for the packet has been specified
     *  (see {@link #getPayloadFormat()}. This method is identical to calling
     *  <tt>p.setDataByte(p.getPayloadFormat(),array)</tt>
     *  @param array The values to insert.
     *  @throws VRTException If the payload format is null.
     */
    public: inline void setDataByte (const vector<int8_t> &array) {
      setDataByte(getPayloadFormat(), array);
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
};
#endif /* _BasicDataPacket_h */

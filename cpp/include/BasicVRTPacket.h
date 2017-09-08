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

#ifndef BasicVRTPacket_h
#define BasicVRTPacket_h

#include "VRTObject.h"
#include "VRTMath.h"
#if NOT_USING_JNI
# include "MetadataBlock.h"
# include "Utilities.h"
# include "TimeStamp.h"
# include "Record.h"
# include "InetAddress.h"
# include "UUID.h"
# include "HasFields.h"
# include "Record.h"
#endif /* NOT_USING_JNI */

using namespace std;

namespace vrt {
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  // Enumerated Types
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  enum PacketType {
    /** A standard data packet with no stream identifier.     */    PacketType_UnidentifiedData    = 0,
    /** A standard data packet (with stream identifier).      */    PacketType_Data                = 1,
    /** An extension data packet with no stream identifier.   */    PacketType_UnidentifiedExtData = 2,
    /** An extension data packet (with stream identifier).    */    PacketType_ExtData             = 3,
    /** A standard context packet (with stream identifier).   */    PacketType_Context             = 4,
    /** An extension context packet (with stream identifier). */    PacketType_ExtContext          = 5,
    /** <b>Do Not Use:</b> Reserved for future use.           */    PacketType_reserved6           = 6,
    /** <b>Do Not Use:</b> Reserved for future use.           */    PacketType_reserved7           = 7,
    /** <b>Do Not Use:</b> Reserved for future use.           */    PacketType_reserved8           = 8,
    /** <b>Do Not Use:</b> Reserved for future use.           */    PacketType_reserved9           = 9,
    /** <b>Do Not Use:</b> Reserved for future use.           */    PacketType_reserved10          = 10,
    /** <b>Do Not Use:</b> Reserved for future use.           */    PacketType_reserved11          = 11,
    /** <b>Do Not Use:</b> Reserved for future use.           */    PacketType_reserved12          = 12,
    /** <b>Do Not Use:</b> Reserved for future use.           */    PacketType_reserved13          = 13,
    /** <b>Do Not Use:</b> Reserved for future use.           */    PacketType_reserved14          = 14,
    /** <b>Do Not Use:</b> Reserved for future use.           */    PacketType_reserved15          = 15
  };
#if NOT_USING_JNI
  bool PacketType_isData (PacketType pt);
  bool PacketType_hasStreamIdentifier (PacketType pt);
#endif

  /** Available the real/complex types for data items. */
  enum RealComplexType {
    /** Real samples.                                                   */   RealComplexType_Real             = 0,
    /** Complex cartesian samples (in-phase and quadrature components). */   RealComplexType_ComplexCartesian = 1,
    /** Complex polar samples (amplitude and phase components).         */   RealComplexType_ComplexPolar     = 2,
    /** <b>Do Not Use:</b> Reserved for future use.                     */   RealComplexType_reserved3        = 3
  };

  /** Available numeric formats for data items.*/
  enum DataItemFormat {
    /** Signed fixed-point.                         */    DataItemFormat_SignedInt    =  0,
    /** Signed VRT, 1-bit exponent.                 */    DataItemFormat_SignedVRT1   =  1,
    /** Signed VRT, 2-bit exponent.                 */    DataItemFormat_SignedVRT2   =  2,
    /** Signed VRT, 3-bit exponent.                 */    DataItemFormat_SignedVRT3   =  3,
    /** Signed VRT, 4-bit exponent.                 */    DataItemFormat_SignedVRT4   =  4,
    /** Signed VRT, 5-bit exponent.                 */    DataItemFormat_SignedVRT5   =  5,
    /** Signed VRT, 6-bit exponent.                 */    DataItemFormat_SignedVRT6   =  6,
    /** <b>Do Not Use:</b> Reserved for future use. */    DataItemFormat_reserved7    =  7,
    /** <b>Do Not Use:</b> Reserved for future use. */    DataItemFormat_reserved8    =  8,
    /** <b>Do Not Use:</b> Reserved for future use. */    DataItemFormat_reserved9    =  9,
    /** <b>Do Not Use:</b> Reserved for future use. */    DataItemFormat_reserved10   = 10,
    /** <b>Do Not Use:</b> Reserved for future use. */    DataItemFormat_reserved11   = 11,
    /** <b>Do Not Use:</b> Reserved for future use. */    DataItemFormat_reserved12   = 12,
    /** <b>Do Not Use:</b> Reserved for future use. */    DataItemFormat_reserved13   = 13,
    /** 32-bit IEEE-754 floating-point.             */    DataItemFormat_Float        = 14,
    /** 64-bit IEEE-754 floating-point.             */    DataItemFormat_Double       = 15,
    /** Unsigned fixed-point.                       */    DataItemFormat_UnsignedInt  = 16,
    /** Unsigned VRT, 1-bit exponent.               */    DataItemFormat_UnsignedVRT1 = 17,
    /** Unsigned VRT, 2-bit exponent.               */    DataItemFormat_UnsignedVRT2 = 18,
    /** Unsigned VRT, 3-bit exponent.               */    DataItemFormat_UnsignedVRT3 = 19,
    /** Unsigned VRT, 4-bit exponent.               */    DataItemFormat_UnsignedVRT4 = 20,
    /** Unsigned VRT, 5-bit exponent.               */    DataItemFormat_UnsignedVRT5 = 21,
    /** Unsigned VRT, 6-bit exponent.               */    DataItemFormat_UnsignedVRT6 = 22,
    /** <b>Do Not Use:</b> Reserved for future use. */    DataItemFormat_reserved23   = 23,
    /** <b>Do Not Use:</b> Reserved for future use. */    DataItemFormat_reserved24   = 24,
    /** <b>Do Not Use:</b> Reserved for future use. */    DataItemFormat_reserved25   = 25,
    /** <b>Do Not Use:</b> Reserved for future use. */    DataItemFormat_reserved26   = 26,
    /** <b>Do Not Use:</b> Reserved for future use. */    DataItemFormat_reserved27   = 27,
    /** <b>Do Not Use:</b> Reserved for future use. */    DataItemFormat_reserved28   = 28,
    /** <b>Do Not Use:</b> Reserved for future use. */    DataItemFormat_reserved29   = 29,
    /** <b>Do Not Use:</b> Reserved for future use. */    DataItemFormat_reserved30   = 30,
    /** <b>Do Not Use:</b> Reserved for future use. */    DataItemFormat_reserved31   = 31
  };

  /** <b>Internal Use Only:</b> Determines if value format is signed. */
  inline bool DataItemFormat_isSigned (DataItemFormat form) {
    return ((int32_t)form) < 16;
  }

  /** <b>Internal Use Only:</b> Determines number of exponent bits in VRT format. */
  inline int32_t DataItemFormat_getExponentBits (DataItemFormat form) {
    switch (form) {
      case DataItemFormat_SignedVRT1:   return 1;
      case DataItemFormat_SignedVRT2:   return 2;
      case DataItemFormat_SignedVRT3:   return 3;
      case DataItemFormat_SignedVRT4:   return 4;
      case DataItemFormat_SignedVRT5:   return 5;
      case DataItemFormat_SignedVRT6:   return 6;
      case DataItemFormat_UnsignedVRT1: return 1;
      case DataItemFormat_UnsignedVRT2: return 2;
      case DataItemFormat_UnsignedVRT3: return 3;
      case DataItemFormat_UnsignedVRT4: return 4;
      case DataItemFormat_UnsignedVRT5: return 5;
      case DataItemFormat_UnsignedVRT6: return 6;
      default: return -1;
    }
  }

  /** Data types supported by a <tt>StandardDataPacket</tt>. */
  enum DataType {
    /**  4-bit signed integer.                      */  DataType_Int4       = __INT64_C(0x00FFFFFA00010000),
    /**  8-bit signed integer.                      */  DataType_Int8       = __INT64_C(0x00FFFFFA00020000),
    /** 16-bit signed integer.                      */  DataType_Int16      = __INT64_C(0x00FFFFFA00030000),
    /** 32-bit signed integer.                      */  DataType_Int32      = __INT64_C(0x00FFFFFA00040000),
    /** 64-bit signed integer.                      */  DataType_Int64      = __INT64_C(0x00FFFFFA00050000),
    /** 32-bit single-precision floating-point.     */  DataType_Float      = __INT64_C(0x00FFFFFA00060000),
    /** 64-bit double-precision floating-point.     */  DataType_Double     = __INT64_C(0x00FFFFFA00070000),
    /**  1-bit unsigned integer ("packed bits").    */  DataType_UInt1      = __INT64_C(0x00FFFFFA00080000),
    /**  4-bit unsigned integer.                    */  DataType_UInt4      = __INT64_C(0x00FFFFFA00090000),
    /**  8-bit unsigned integer.                    */  DataType_UInt8      = __INT64_C(0x00FFFFFA000A0000),
    /** 16-bit unsigned integer.                    */  DataType_UInt16     = __INT64_C(0x00FFFFFA000B0000),
    /** 32-bit unsigned integer.                    */  DataType_UInt32     = __INT64_C(0x00FFFFFA000C0000),
    /** 64-bit unsigned integer.                    */  DataType_UInt64     = __INT64_C(0x00FFFFFA000D0000)
  };

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  // BasicVRTPacket
  ////////////////////////////////////////////////////////////////////////////////////////////////////
#if NOT_USING_JNI
  /** A basic but full-featured implementation of {@link BasicVRTPacket}. <br>
   *  <br>
   *  The following implementation is used for the {@link HasFields} methods:
   *  <pre>
   *     ID | Name      | Type
   *    ----+-----------+-----------
   *     0  | StreamID  | String
   *     1  | ClassID   | String
   *     2  | TimeStamp | TimeStamp
   *    ----+-----------+-----------
   *  </pre>
   */
  class BasicVRTPacket : public VRTObject, public HasFields {
    friend class VRTReader;//MSM???

    /** The maximum length of the packet in bytes (header + payload + trailer). Note that this
     *  is the absolute limit imposed by the VRT specification. The underlying transport protocols
     *  used (e.g. IP, UDP, TCP, etc.) have limits lower than this.
     *  <pre>
     *    MAX_PACKET_LENGTH = 65536*4
     *  </pre>
     *  @see #getHeaderLength()
     */
    public: static const int32_t MAX_PACKET_LENGTH = 65536*4;

    /** The maximum length of the header in bytes (all optional fields included). Note that
     *  in cases where optional fields are not included, the header will be shorter.
     *  <pre>
     *    MAX_HEADER_LENGTH = 7*4
     *  </pre>
     *  @see #getHeaderLength()
     */
    public: static const int32_t MAX_HEADER_LENGTH = 7*4;

    /** The maximum length of the trailer in bytes.
     *  <pre>
     *    MAX_TRAILER_LENGTH = 1*4
     *  </pre>
     *  @see #getTrailerLength()
     */
    public: static const int32_t MAX_TRAILER_LENGTH = 1*4;

    /** The <b>de-facto</b> maximum length of the payload in bytes. Note that the de-jure limit
     *  can be up to 28 bytes more than this when optional fields in the header and trailer are
     *  dropped. In practice this difference is nearly meaningless since (as noted with
     *  {@link #MAX_PACKET_LENGTH}) this value exceeds the limits the underlying transport protocols.
     *  <pre>
     *    MAX_PAYLOAD_LENGTH = MAX_PACKET_LENGTH - MAX_HEADER_LENGTH - MAX_TRAILER_LENGTH
     *  </pre>
     *  @see #getPayloadLength()
     */
    public: static const int32_t MAX_PAYLOAD_LENGTH = MAX_PACKET_LENGTH - MAX_HEADER_LENGTH - MAX_TRAILER_LENGTH;

    /** Represents a null VRT packet. */
    public: static const BasicVRTPacket NULL_PACKET;

    /** Packet buffer (always includes the header) */
    // TODO FIXME - Revert to public to work with SourceVITA49 and SinkVITA49
    // TODO FIXME - Can those assets be updated to use new VRT and still be efficient?
    //protected: vector<char> bbuf;
    public: vector<char> bbuf;
    // End TODO FIXME

    /** Is this instance read-only? */
    protected: bool readOnly;

    /** Basic destructor for the class. */
    public: ~BasicVRTPacket () { }

    /** Basic no-argument constructor for the class. */
    public: BasicVRTPacket ();

    /** Basic copy constructor for the class.
     *  @param p The packet to copy.
     */
    public: BasicVRTPacket (const BasicVRTPacket &p);

    /** Creates a new instance with a default anticipated length that can be written to.
     *  Initially this will just be a simple data packet with no fields set (other than
     *  the required packet length of 4), but will have the underlying buffers pre-allocated
     *  as required.
     *  @param bufsize The anticipated buffer size.
     */
    public: BasicVRTPacket (int32_t bufsize);

    /** Creates a new instance accessing the given data buffer.
     *  @param buf      The pointer to the packet buffer.
     *  @param len      The length  of the packet buffer.
     *  @param readOnly Should users of this instance be able to modify the underlying data buffer?
     */
    public: BasicVRTPacket (const void *buf, size_t len, bool readOnly=false);

    public: BasicVRTPacket (vector<char> *buf_ptr, bool readOnly);

    /** Creates a new instance accessing the given data buffer.
     *  @param buf      The pointer to the packet buffer.
     *  @param readOnly Should users of this instance be able to modify the underlying data buffer?
     */
    public: BasicVRTPacket (const vector<char> &buf, bool readOnly=false);

    /** Creates a new instance accessing the given data buffer.
     *  @param buf      The pointer to the packet buffer.
     *  @param start    The start point in buffer to use.
     *  @param end      The end point in buffer to use.
     *  @param readOnly Should users of this instance be able to modify the underlying data buffer?
     */
    public: BasicVRTPacket (const vector<char> &buf, ssize_t start, ssize_t end, bool readOnly = false);

    /** Internal constructor, for use by subclasses.
        @param p                The packet (already initialized).
        @param type             The expected packet type.
        @param classID          The expected classID.
        @param minPayloadLength The minimum payload length.
        @param maxPayloadLength The maximum payload length (&lt;0 if not pre-defined).
        @throws VRTException If packet type or classID of the packet are invalid, or if
                                         the payload length is invalid.
     */
    protected: BasicVRTPacket (const BasicVRTPacket &p, PacketType type, int64_t classID,
                               int32_t minPayloadLength, int32_t maxPayloadLength);

    /** Is this object equal to null. */
    public: inline virtual bool isNullValue () const {
      return (bbuf.size() == 0);
    }

//    /** <b>Internal use only:</b> Swaps packet buffer contents. */
//    public: inline void swap (vector<char> &buf) {
//      std::swap(bbuf, buf);
//    }
//
//    /** <b>Internal use only:</b> Swaps packet buffer contents. */
//    public: inline void swap (BasicVRTPacket &p) {
//      std::swap(bbuf, p.bbuf);
//    }

    /** <b>Internal use only:</b> Gets the packet stream code. The stream code is a 64-bit value
     *  equal to:
     *  <tt>
     *    0xt0000000ssssssss   where t = is the numeric value corresponding to the packet type
     *                               s = the stream identifier or 0x00000000 if not specified
     *  </tt>
     *  This is used as the hashtable key when doing packet sequence accounting since the counters
     *  are unique to the stream and type.
     *  @return The packet stream code.
     */
    public: int64_t getStreamCode () const;

    /** <b>Internal use only:</b> Gets the packet stream code.
     *  @param ptr Pointer to the packet content.
     *  @return The packet stream code.
     */
    public: static int64_t getStreamCode (const void *ptr);

    /** Checks for header equality with another BasicVRTPacket. The headers of two VRT packets are
     *  considered equal if the content of the payloads is bit-for-bit identical. The contents of the
     *  payload and trailer need not be equal for this method to return true. However, any two packets
     *  for which {@link #equals} returns true, must have this method return true as well.
     *  @param p The other packet.
     *  @return true if they are identical, false otherwise. (This will always returns false if the
     *          packet is null.)
     */
    public: bool headerEquals (const BasicVRTPacket &p) const;

    /** Checks for payload equality with another BasicVRTPacket. The payloads of two VRT packets are
     *  considered equal if the content of the payloads is bit-for-bit identical. The contents of the
     *  header and trailer need not be equal for this method to return true. However, any two packets
     *  for which {@link #equals} returns true, must have this method return true as well. This
     *  will always return false if the tqo packets have different payload lengths.
     *  @param p The other packet.
     *  @return true if they are identical, false otherwise. (This will always returns false if the
     *          packet is null.)
     */
    public: bool payloadEquals (const BasicVRTPacket &p) const;

    /** Checks for trailer equality with another BasicVRTPacket. This method is identical to
     *  <tt>trailerEquals(p,true)</tt>. <b>In most cases, use of <tt>trailerEquals(p,false)</tt>
     *  is more appropriate for comparing trailers.</b>
     *  @param p The other packet.
     *  @return true if they are identical, false otherwise. (This will always returns false if the
     *          packet is null.)
     */
    public: inline bool trailerEquals (const BasicVRTPacket &p) const {
      return trailerEquals(p, true);
    }

    /** Checks for trailer equality with another BasicVRTPacket. This method allows two different
     *  interpretations of trailer equality:
     *  <ul>
     *    <li><b>Exact:</b> With "exact" equality, the trailers of two VRT packets are considered
     *      equal if the content of the trailers is either bit-for-bit identical, or if <b>both</b>
     *      packets omit the trailers.
     *    </li>
     *    <li><b>Equal:</b> With "equal" equality, the trailers of two VRT packets are considered
     *      equal if the content of the trailers conveys the same meaning. Conveying the same meaning
     *      means that the enable bits in the trailer are checked first. Any values not enabled, are
     *      ignored when comparing. In this form, a packet with no trailer is considered equal to a
     *      packet with a trailer that has nothing enabled.
     *    </li>
     *  </ul>
     *  Regardless of the comparison used, any two packets for which {@link #equals} returns
     *  true, must have this method return true as well. This method will always return true if
     *  comparing two context packets since they do not contain trailers.
     *  @param p     The other packet.
     *  @param exact true to use the <b>Exact</b> comparison, false to use the <b>Equal</b> comparison.
     *  @return true if they are identical, false otherwise. (This will always returns false if the
     *          packet is null.)
     */
    public: bool trailerEquals (const BasicVRTPacket &p, bool exact) const;

    using VRTObject::equals;
    /** Checks for equality with an unknown object. Two VRT packets are considered equal if the
     *  content of the packets is bit-for-bit identical. The actual implementation classes are not
     *  considered relevant for checking equality.
     *  @param o The unknown object.
     *  @return true if they are identical, false otherwise. (This will always returns false if the
     *          object is null.)
     */
    public: virtual bool equals (const BasicVRTPacket &o) const;

    public: virtual inline bool equals (const VRTObject &o) const {
      try {
        return equals(*checked_dynamic_cast<const BasicVRTPacket*>(&o));
      }
      catch (bad_cast &e) {
        UNUSED_VARIABLE(e);
        return false;
      }
    }

    public: virtual string toString () const;

    /** Writes the packet content to a string stream in a user-friendly format.
     *  @param str The string stream to write to.
     */
    protected: virtual void toStringStream (ostringstream &str) const;

    /** Gets the offset for the given header field. */
    private: int32_t getOffset (int32_t field) const;

    /** <b>Internal use only:</b> Does the packet type have a stream identifier. */
    protected: inline bool hasStreamIdentifier() const {
      return PacketType_hasStreamIdentifier(getPacketType());
    }

    /** <b>Internal use only:</b> Shifts the header. */
    protected: void shiftHeader (int32_t field, int32_t bytes, bool present);

    /** <b>Internal use only:</b> Shifts the trailer. */
    protected: void shiftTrailer (int32_t bytes);

    /** <b>Internal use only:</b> Sets the packet length. */
    protected: void setPacketLength (int32_t v);

    /** Does this packet have a trailer? */
    protected: bool    hasTrailer () const;

    /** <b>Internal use only:</b> Shifts the payload. */
    protected: int32_t shiftPayload (int32_t off, int32_t bytes, bool add);

    /** <b>Internal use only:</b> Is the packet type a data packet? */
    public: bool isData () const;

    /** Gets the specified bit from the trailer. By default all of the "is" methods that access the
     *  trailer call this method with the appropriate bit values (no other combinations of inputs
     *  is valid):
     *  <pre>
     *    Name                Enable Indicator
     *    ====================================
     *    CalibratedTimeStamp   31      19
     *    DataValid             30      18
     *    ReferenceLocked       29      17
     *    AutomaticGainControl  28      16
     *    SignalDetected        27      15
     *    InvertedSpectrum      26      14
     *    OverRange             25      13
     *    Discontinuious        24      12
     *    Bit11                 23      11 (Data and ExtData Only)
     *    Bit10                 22      10 (Data and ExtData Only)
     *    Bit9                  21       9 (Data and ExtData Only)
     *    Bit8                  20       8 (Data and ExtData Only)
     *  </pre>
     *  @param buf       The data buffer.
     *  @param off       The offset into the data buffer.
     *  @param enable    Bit position of the enable flag.
     *  @param indicator Bit position of the indicator flag.
     *  @return null if there is no trailer, null if the enable bit is not set, true if the enable
     *          bit is set and the indicator bit is set, false if the enable bit is set but the
     *          indicator bit is not set.
     */
    protected: static boolNull getStateEventBit (const vector<char> &buf, int32_t off, int32_t enable, int32_t indicator);

    /** <i>Optional functionality:</i> Sets the specified bit from the trailer. By default all of
     *  the "set" methods that access the trailer call this method with the appropriate bit values
     *  (see {@link #getStateEventBit} for a list).
     *  @param buf       The data buffer.
     *  @param off       The offset into the data buffer.
     *  @param enable    Bit position of the enable flag.
     *  @param indicator Bit position of the indicator flag.
     *  @param value     The value of the bits: null if the enable bit should not be set, true if
     *                   the enable bit and the indicator bit should be set, false if the enable bit
     *                   should be set but the indicator bit should not be set.
     */
    protected: static void setStateEventBit (vector<char> &buf, int32_t off, int32_t enable, int32_t indicator, boolNull value);

    /** Checks to see if the packet is valid. This checks the overall structure of the packet (i.e.
     *  length is correct, reserved bits set to 0, etc.). It does not check the content of the packet
     *  or the validity of the data in the packet (the state and event indicators are provided for
     *  that purpose).
     *  @return true if it is valid, false otherwise.
     */
    public: inline bool isPacketValid () const {
      return (getPacketValid(true, -1) == "");
    }

    /** Checks to see if the packet is valid. This method adds an extra length check onto the basic
     *  checks performed by {@link #isPacketValid()}. The most common use for this method is to
     *  verify that a packet read from a socket matches the length expected.
     *  @param length Verifies that the length of the packet matches this value.
     *  @return true if it is valid, false otherwise.
     */
    public: inline bool isPacketValid (int32_t length) const {
      return (getPacketValid(true, length) == "");
    }

    /** Checks to see if the packet is valid. This is identical to <tt>isPacketValid()</tt> except
     *  that a description of the error is returned.
     *  @param strict Should strict checking be done, or just enough to verify that things are OK for
     *                reading. No packet failing strict checking should ever be sent out, but it may
     *                be useful to disable strict checking when reading in a packet.
     *  @return Empty string ("") if it is valid, a description of the error otherwise.
     */
    public: inline string getPacketValid (bool strict) const {
      return getPacketValid(strict, -1);
    }

    /** Checks to see if the packet is valid. This is identical to <tt>isPacketValid(int32_t)</tt>
     *  except that a description of the error is returned.
     *  @param strict Should strict checking be done, or just enough to verify that things are OK for
     *                reading. No packet failing strict checking should ever be sent out, but it may
     *                be useful to disable strict checking when reading in a packet.
     *  @param length Verifies that the length of the packet matches this value.
     *  @return Empty string ("") if it is valid, a description of the error otherwise.
     */
    public: virtual string getPacketValid (bool strict, int32_t length) const;

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Context Change
    //////////////////////////////////////////////////////////////////////////////////////////////////
    /** Prepares a packet for resend resetting any non-persistent values and clearing the context
     *  change flags. At a minimum this will update the time stamp. This is implemented as follows:
     *  <pre>
     *    bool BasicVRTPacket::resetForResend (const TimeStamp &t) {
     *      setTimeStamp(t);
     *      return false;
     *    }
     *  </pre>
     *  @param t The new time stamp.
     *  @return true if non-persistent values and the change flags have been reset, false if only the
     *               time stamp has been updated.
     */
    public: virtual bool resetForResend (const TimeStamp &t);

    /** Indicates if any fields have changes since the last packet in this stream. Packet streams
     *  that do not support this concept, should always return true indicating that the recipient must
     *  read the packet to determine if there was a change. An IF Data stream will always return true.
     *  @return true if <b>anything</b> in the packet may have changed since the last context
     *          packet, false if <b>nothing</b> has changed since the last context packet.
     */
    public: virtual bool isChangePacket () const;

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Get
    //////////////////////////////////////////////////////////////////////////////////////////////////
    /** Gets the packet type. */
    public: virtual PacketType getPacketType () const;

    /** Context packets only: Gets the Time Stamp Mode (TSM). This indicates if a context packet
     *  is being used to convey timing of context events related to the signal with fine or coarse
     *  resolution:
     *  <pre>
     *    true  = Time stamp fields indicate *precise* timing of context changes (to the
     *            resolution of the system)
     *    false = Time stamp fields indicate *general* timing of the context changes (i.e.
     *            the change occurred within the time interval covered by this packet)
     *    null  = <i>Error: not a context packet</i>
     *  </pre>
     *  @return true or false as indicated above or null if this is a data packet.
     */
    public: virtual boolNull isTimeStampMode () const;

    /** Gets the time stamp of the packet.
     *  @return The time stamp of the packet (never null).
     */
    public: virtual TimeStamp getTimeStamp () const;

    /** Gets the time stamp of the packet for the given sampling rate.
     *  @param  sr the sampling rate
     *  @return The time stamp of the packet (never null).
     */
    public: virtual TimeStamp getTimeStamp (double sr) const;

    /** Gets the packet count. This is the modulo-16 count of all packets on this stream.
     *  @return The packet count (0..15).
     */
    public: virtual int32_t getPacketCount () const;

    /** Gets the packet length in bytes. The length of the header will be a value <b>up to</b>
     *  {@link #MAX_PACKET_LENGTH} (see {@link #MAX_PACKET_LENGTH} for explanation as to why
     *  actual packet lengths used are usually a lot less than this number). The packet
     *  length will always be a multiple of 4-bytes.
     *  @return The total length of the packet in bytes.
     */
    public: virtual inline int32_t getPacketLength () const {
      return getPacketLength(bbuf, 0);
    }

    /** <b>Internal Use Only:</b> Get VRT packet length using a buffer input. */
    public: inline static int32_t getPacketLength (const vector<char> &buf, int32_t off) {
      return ((0xFF & ((int32_t)buf[off+2])) << 10) | ((0xFF & ((int32_t)buf[off+3])) << 2);
    }

    /** Gets the payload length in bytes. The payload length is a derived value computed as:
     *  <pre>
     *    payloadLength = {@link #getPacketLength()} - {@link #getHeaderLength()} - {@link #getTrailerLength()}
     *  </pre>
     *  The payload length will always be a multiple of 4-bytes.
     *  @return The length of the payload in bytes.
     */
    public: inline int32_t getPayloadLength () const {
      return getPacketLength() - getHeaderLength() - getTrailerLength();
    }

    /** Gets the number of pad bits inserted following the data in the payload such that the
     *  payload section is always a multiple of 4-bytes in length. The number of pad bits is stored
     *  in five of the (formerly-reserved) bits in the ClassID block in the header as part of the
     *  suggested VITA-49.0 modifications (i.e. VITA-49.0b), but could be specified elsewhere for
     *  some user-defined class types. (The default implementation exclusively follows VITA-49.0b.)
     *  <br>
     *  Note that reporting the number of pad bits is optional in situations where the number of pad
     *  bits is less than the length of a single sample (this may occur when using the link-efficient
     *  packing mode with a data format that does not evenly divide into a 32-/64-bit length). In this
     *  situation, the number of pad bits is implicit and can be determined after consideration of
     *  the data payload format and payload length (see {@link BasicDataPacket#getDataLength()}). This
     *  usage matches the VITA-49.0 specification.
     *  @return The number of pad bits *reported* for a data packet. This will return 0 for context
     *          packets and data packets that do not report the number of pad bits.
     *  @see #getPayloadLength()
     *  @see BasicDataPacket#getDataLength()
     */
    public: virtual int32_t getPadBitCount () const;

    /** Gets the header length in bytes (does not include trailer). The length of the header will be
     *  a value <b>up to</b> {@link #MAX_HEADER_LENGTH}. The header length will always be a multiple
     *  of 4-bytes.
     *  @return The length of the header in bytes.
     */
    public: virtual int32_t getHeaderLength () const;

    /** Gets the trailer length in bytes. Note that the presence of the trailer is an all-or-nothing,
     *  so this method return either {@link #MAX_TRAILER_LENGTH} or <tt>0</tt>.
     *  @return The length of the trailer in bytes.
     */
    public: int32_t getTrailerLength () const;

    /** Gets the 32-bit packet stream identifier as a string.
     *  @return The stream identifier or null if not specified.
     */
    public: virtual string getStreamID () const;

    /** Gets the 32-bit packet stream identifier.
     *  @return The stream identifier or INT4_NULL if not specified.
     */
    public: virtual int32_t getStreamIdentifier () const;

    /** Gets the class identifier as a string. This will return the class identifier in the form
     *  <tt>CC-CC-CC:IIII.PPPP</tt> where <tt>CC-CC-CC</tt> is the standard hexadecimal representation
     *  of the OUI (see IEEE standard 802-2001), IIII is the hexadecimal representation of the ICC
     *  and PPPP is the hexadecimal representation of the PCC.
     *  @return The packet class identifier as a string or null if not specified.
     */
    public: virtual string getClassID () const;

    /** Gets the packet class identifier. Note that the the packet class identifier is the union
     *  of the three distinct parts of the class identifier: OUI, ICC, and PCC. This method allows for
     *  easy comparison of the class identifier as a whole. <i>Note that if this is null, the OUI,
     *  ICC, and PCC will all be null; if this is non-null, the OUI, ICC, and PCC will all be
     *  non-null.</i>
     *  @return The packet class identifier or INT8_NULL if not specified.
     */
    public: virtual int64_t getClassIdentifier () const;

    /** Gets the Organizational Unique Identifier (OUI) from the packet class identifier. The OUI
     *  is a 24-bit IEEE-assigned identifier for the organization that defined the packet class.
     *  @return The OUI or null if not specified.
     */
    public: virtual int32_t getClassIdentifierOUI ();

    /** Gets the Information Class Code (ICC) from the packet class identifier. This is a
     *  user-defined value specified by the OUI (see {@link #getClassIdentifierOUI()}) that defined
     *  this packet class.
     *  @return The ICC or null if not specified.
     */
    public: virtual int16_t getClassIdentifierICC ();

    /** Gets the Packet Class Code (PCC) from the packet class identifier. This is a user-defined
     *  value specified by the OUI (see {@link #getClassIdentifierOUI()}) that defined this packet
     *  class.
     *  @return The ICC or null if not specified.
     */
    public: virtual int16_t getClassIdentifierPCC ();

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Set
    //////////////////////////////////////////////////////////////////////////////////////////////////
    /** <i>Optional functionality:</i> Sets the packet type.
     *  @param t The packet type.
     *  @throws VRTException If this method is not supported.
     */
    public: virtual void setPacketType (PacketType t);

    /** <i>Optional functionality:</i> Context packets only: Sets the Time Stamp Mode (TSM).
     *  @param v The new value, see {@link #isTimeStampMode()} for details.
     *  @throws VRTException If this method is not supported or if the packet type
     *                                        doesn't support this setting.
     */
    public: virtual void setTimeStampMode (bool v);

    /** <i>Optional functionality:</i> Sets the time stamp for the packet.
     *  @param v The new time stamp.
     *  @throws VRTException If this method is not supported.
     */
    public: virtual void setTimeStamp (const TimeStamp &v);

    /** <i>Optional functionality:</i> Sets the packet count. This is the modulo-16 count of all
     *  packets on this stream.
     *  @param v The packet count (0..15).
     *  @throws VRTException If this method is not supported.
     *  @throws VRTException If the value passed in is invalid.
     */
    public: virtual void setPacketCount (int32_t v);

    /** <b>Internal Use Only:</b> Sets the packet count. This is the modulo-16 count of all
     *  packets on this stream.
     *  @param ptr Pointer to the packet content.
     *  @param v   The packet count (0..15).
     *  @throws VRTException If the value passed in is invalid.
     */
    public: static void setPacketCount (void *ptr, int32_t v);

    /** <i>Optional functionality:</i> Sets the payload length in bytes. The packet length is
     *  recomputed and trailer is shifted as required. The payload length must be a multiple
     *  of 4-bytes.
     *  @param v The length of the payload in bytes.
     *  @throws VRTException If this method is not supported.
     *  @throws VRTException If the value passed in is invalid.
     */
    public: virtual void setPayloadLength (int32_t v);

    /** <i>Optional functionality:</i> Sets the number of pad bits inserted following the data
     *  in the payload such that the payload section is always a multiple of 4-bytes in length (see
     *  {@link #getPadBitCount()} for a detailed discussion about the reporting of pad bits). <br>
     *  <br>
     *  This function will *not* alter the packet if the number of pad bits is implicit (or zero) when
     *  in strict VITA-49.0 (rather than VITA-49.0b) mode. <br>
     *  <br>
     *  Note: <b>It is not possible to report the number of pad bits if the ClassID is not set.</b>
     *  If this function is called with a non-zero value, the number of pad bits is not implicit and
     *  the ClassID is not set, an exception will be thrown. <br>
     *  <br>
     *  Assuming this method is supported, it is safe to call this method on a non-data packet with
     *  a value of zero. <br>
     *  <br>
     *  <b>In most cases, this method should not be called directly (except to clear it by setting a
     *  value of zero); rather this should be called by {@link BasicDataPacket#setDataLength(int32_t)}.</b>
     *  @param bits          The number of pad bits to report.
     *  @param bitsPerSample Number of bits per sample. This is used to determine if the number of pad
     *                       bits is implicit. Note the value of <tt>bitsPerSample</tt> is irrelevant
     *                       when <tt>bits</tt> is zero, therefore users can just call
     *                       <tt>setPadBitCount(0,0)</tt> to set the field to zero (where applicable).
     *  @see #setPayloadLength(int32_t)
     *  @see BasicDataPacket#setDataLength(int32_t)
     *  @throws VRTException If this method is not supported.
     *  @throws VRTException If this method is called with a non-zero value for a non-data packet
     *                       or if the number of bits is not implicit and the ClassID is not set.
     */
    public: virtual void setPadBitCount (int32_t bits, int32_t bitsPerSample);

    /** <i>Optional functionality:</i> Sets the 32-bit packet stream identifier.
     *  @param v The stream identifier or null if not specified.
     *  @throws VRTException If this method is not supported.
     */
    public: virtual void setStreamID (string v);

    /** <i>Optional functionality:</i> Sets the 32-bit packet stream identifier.
     *  @param v The stream identifier or INT4_NULL if not specified.
     *  @throws VRTException If this method is not supported.
     */
    public: virtual void setStreamIdentifier (int32_t v);

    /** <i>Optional functionality:</i> Sets the packet class identifier.
     *  @param v The packet class identifier or null if not specified.
     *  @throws VRTException If this method is not supported.
     *  @throws VRTException If the value passed in is invalid.
     */
    public: virtual void setClassID (string v);

    /** <i>Optional functionality:</i> Sets the packet class identifier. See
     *  {@link #getClassIdentifier()} for details.
     *  @param v The packet class identifier or INT64_NULL if not specified.
     *  @throws VRTException If this method is not supported.
     *  @throws VRTException If the value passed in is invalid.
     */
    public: virtual void setClassIdentifier (int64_t v);

    /** <i>Optional functionality:</i> Sets the packet class identifier.
     *  @param oui The new OUI value.
     *  @param icc The new ICC value.
     *  @param pcc The new PCC value.
     *  @throws VRTException If this method is not supported.
     *  @throws VRTException If the value passed in is invalid.
     */
    public: virtual void setClassIdentifier (int32_t oui, int16_t icc, int16_t pcc);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Read
    //////////////////////////////////////////////////////////////////////////////////////////////////

    /** <b>Experimental, use with care:</b> Gets a pointer to the packet content. */
    public: char* getPacketPointer ();

    /** <b>Experimental, use with care:</b> Gets a pointer to the packet payload. */
    public: char* getPayloadPointer ();

    /** Reads the packet content as a vector.
     *  @deprecated This function is present for backwards-compatibility with a
     *              user-extension, the proper function to use is <tt>getPacket()</tt>.
     */
    public: inline __attribute__((deprecated)) __intelattr__((deprecated))
            vector<char> getPacketVector () const {
      return getPacket();
    }

    /** <b>Experimental, use with care:</b> Gets a pointer to the packet content.
     *  @deprecated This function is present for backwards-compatibility with a
     *              user-extension, the proper function to use is <tt>getPacketPointer()</tt>.
     */
    public: inline __attribute__((deprecated)) __intelattr__((deprecated))
            char* getPacketPointer_type () {
      return getPacketPointer();
    }

    /** Reads the packet content as a vector. */
    public: vector<char> getPacket () const;

    /** Reads the packet payload as a vector. */
    public: vector<char> getPayload () const;

    /** Reads the packet payload into the provided buffer.
     *  @param buffer The buffer to read into.
     *  @param poff   The payload offset to start reading from.
     *  @param len    The maximum length to read in.
     *  @return The number of bytes read or -1 if the end of the payload has been reached.
     */
    public: int32_t readPacket (void *buffer, int32_t poff, int32_t len) const;

    /** Reads the packet payload into the provided buffer.
     *  @param buffer The buffer to read into.
     *  @param poff   The payload offset to start reading from.
     *  @param len    The maximum length to read in.
     *  @return The number of bytes read or -1 if the end of the payload has been reached.
     */
    public: int32_t readPayload (void *buffer, int32_t poff, int32_t len) const;

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Write
    //////////////////////////////////////////////////////////////////////////////////////////////////


    /** <i>Optional functionality:</i> Sets the content of the packet payload and updates the
     *  length of the payload.
     *  @param buffer The new payload.
     *  @throws VRTException If this method is not supported
     */
    public: inline void setPayload (vector<char> buffer) {
      setPayload(&buffer[0], buffer.size());
    }

    /** <i>Optional functionality:</i> Sets the content of the packet payload and updates the
     *  length of the payload.
     *  @param buffer The new payload.
     *  @param length The number of bytes to copy from the buffer into the payload.
     *  @throws VRTException If this method is not supported
     */
    public: void setPayload (void *buffer, int32_t length) {
      setPayloadLength(length);
      writePayload(buffer, 0, length);
    }

    /** <i>Optional functionality:</i> Writes to the packet payload, but does NOT update the length
     *  of the payload.
     *  @param buffer The buffer to with the new data.
     *  @param poff   The payload offset to start writing to.
     *  @param len    The length to write.
     *  @throws VRTException If this method is not supported or if there is insufficient room to write to.
     */
    public: void writePayload (void *buffer, int32_t poff, int32_t len);


    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Pack/Unpack
    //////////////////////////////////////////////////////////////////////////////////////////////////
    /** <b>Internal Use Only:</b> Does a buffer swap. */
    public: inline void swap (vector<char> *buffer) {
      if (buffer == NULL) throw VRTException("Buffer is null");
      if (readOnly      ) throw VRTException("Packet is read-only");
      bbuf.swap(*buffer);
    }
    /** <b>Internal Use Only:</b> Does a buffer swap. */
    public: inline void swap (BasicVRTPacket *pkt) {
      if (pkt  ==  NULL) throw VRTException("Packet is null");
      if (readOnly     ) throw VRTException("Packet is read-only");
      if (pkt->readOnly) throw VRTException("Packet is read-only");
      bbuf.swap(pkt->bbuf);
    }

    /** Packs an 8-bit integer into the payload of the packet. */
    protected: inline void packPayloadByte      (int32_t off, char     val) { VRTMath::packByte(    bbuf, getHeaderLength()+off, val); }
    /** Packs a 16-bit integer into the payload of the packet. */
    protected: inline void packPayloadShort     (int32_t off, int16_t  val) { VRTMath::packShort(   bbuf, getHeaderLength()+off, val); }
    /** Packs a 32-bit integer into the payload of the packet. */
    protected: inline void packPayloadInt       (int32_t off, int32_t  val) { VRTMath::packInt(     bbuf, getHeaderLength()+off, val); }
    /** Packs a 64-bit integer into the payload of the packet. */
    protected: inline void packPayloadLong      (int32_t off, int64_t  val) { VRTMath::packLong(    bbuf, getHeaderLength()+off, val); }
    /** Packs a 32-bit floating-point value into the payload of the packet. */
    protected: inline void packPayloadFloat     (int32_t off, float    val) { VRTMath::packFloat(   bbuf, getHeaderLength()+off, val); }
    /** Packs a 64-bit floating-point value into the payload of the packet. */
    protected: inline void packPayloadDouble    (int32_t off, double   val) { VRTMath::packDouble(  bbuf, getHeaderLength()+off, val); }
    /** Packs a <tt>boolNull</tt> value into the payload of the packet. */
    protected: inline void packPayloadBoolNull  (int32_t off, boolNull val) { VRTMath::packBoolNull(bbuf, getHeaderLength()+off, val); }
    /** Packs a boolean value into the payload of the packet. */
    protected: inline void packPayloadBoolean   (int32_t off, bool     val) { VRTMath::packBoolean( bbuf, getHeaderLength()+off, val); }
    /** Packs a record value into the payload of the packet. */
    protected: inline void packPayloadRecord    (int32_t off, Record   val) { VRTMath::packRecord(  bbuf, getHeaderLength()+off, val); }
    /** Packs a metadata block into the payload of the packet. */
    protected: inline void packPayloadMetadata  (int32_t off, MetadataBlock       val, int32_t len      ) { VRTMath::packMetadata( bbuf, getHeaderLength()+off, val, len); }
    /** Packs a block of bytes into the payload of the packet. */
    protected: inline void packPayloadBytes     (int32_t off, const vector<char> &val, int32_t len      ) { VRTMath::packBytes(    bbuf, getHeaderLength()+off, val, len); }
    /** Packs a block of bytes into the payload of the packet. */
    protected: inline void packPayloadBytes     (int32_t off, const void         *val, int32_t len      ) { VRTMath::packBytes(    bbuf, getHeaderLength()+off, val, len); }
    /** Packs a {@link TimeStamp} into the payload of the packet. */
    protected: inline void packPayloadTimeStamp (int32_t off, TimeStamp           val, IntegerMode epoch) { VRTMath::packTimeStamp(bbuf, getHeaderLength()+off, val, epoch); }
    /** Packs a {@link InetAddress} into the payload of the packet. */
    protected: inline void packPayloadInetAddr  (int32_t off, InetAddress         val                   ) { VRTMath::packInetAddr( bbuf, getHeaderLength()+off, val); }
    /** Packs a {@link UUID} into the payload of the packet. */
    protected: inline void packPayloadUUID      (int32_t off, UUID                val                   ) { VRTMath::packUUID(     bbuf, getHeaderLength()+off, val); }

    /** Packs an ASCII string into the payload of the packet. */
    protected: inline void packPayloadAscii (int32_t off, string val, int32_t len) {
      if (len < 0) { // compute max length
        int32_t max = -len;
        setPayloadLength(off+max);
        int32_t act = VRTMath::packAscii(bbuf, getHeaderLength()+off, val, max);
        if (act < max) setPayloadLength(off+act);
      }
      else {
        VRTMath::packAscii(bbuf, getHeaderLength()+off, val, len);
      }
    }

    /** Packs a UTF-8 string into the payload of the packet. */
    protected: inline void packPayloadUTF8 (int32_t off, wstring val, int32_t len) {
      if (len < 0) { // compute max length
        int32_t max = -len;
        setPayloadLength(off+max);
        int32_t act = VRTMath::packUTF8(bbuf, getHeaderLength()+off, val, max);
        if (act < max) setPayloadLength(off+act);
      }
      else {
        VRTMath::packUTF8(bbuf, getHeaderLength()+off, val, len);
      }
    }

    /** Packs a UTF-8 string into the payload of the packet. */
    protected: inline void packPayloadUTF8 (int32_t off, string val, int32_t len) {
      if (len < 0) { // compute max length
        int32_t max = -len;
        setPayloadLength(off+max);
        int32_t act = VRTMath::packUTF8(bbuf, getHeaderLength()+off, val, max);
        if (act < max) setPayloadLength(off+act);
      }
      else {
        VRTMath::packUTF8(bbuf, getHeaderLength()+off, val, len);
      }
    }

    /** Unpacks an 8-bit integer from the payload of the packet. */
    protected: inline char          unpackPayloadByte      (int32_t off                        ) const { return VRTMath::unpackByte(     bbuf, getHeaderLength()+off); }
    /** Unpacks a 16-bit integer from the payload of the packet. */
    protected: inline int16_t       unpackPayloadShort     (int32_t off                        ) const { return VRTMath::unpackShort(    bbuf, getHeaderLength()+off); }
    /** Unpacks a 32-bit integer from the payload of the packet. */
    protected: inline int32_t       unpackPayloadInt       (int32_t off                        ) const { return VRTMath::unpackInt(      bbuf, getHeaderLength()+off); }
    /** Unpacks a 64-bit integer from the payload of the packet. */
    protected: inline int64_t       unpackPayloadLong      (int32_t off                        ) const { return VRTMath::unpackLong(     bbuf, getHeaderLength()+off); }
    /** Unpacks a 32-bit floating-point value from the payload of the packet. */
    protected: inline float         unpackPayloadFloat     (int32_t off                        ) const { return VRTMath::unpackFloat(    bbuf, getHeaderLength()+off); }
    /** Unpacks a 64-bit floating-point value from the payload of the packet. */
    protected: inline double        unpackPayloadDouble    (int32_t off                        ) const { return VRTMath::unpackDouble(   bbuf, getHeaderLength()+off); }
    /** Unpacks a <tt>boolNull</tt> value from the payload of the packet. */
    protected: inline boolNull      unpackPayloadBoolNull  (int32_t off                        ) const { return VRTMath::unpackBoolNull( bbuf, getHeaderLength()+off); }
    /** Unpacks a boolean value from the payload of the packet. */
    protected: inline bool          unpackPayloadBoolean   (int32_t off                        ) const { return VRTMath::unpackBoolean(  bbuf, getHeaderLength()+off); }
    /** Unpacks a record from the payload of the packet. */
    protected: inline void          unpackPayloadRecord    (int32_t off, Record &rec           ) const {        VRTMath::unpackRecord(   bbuf, getHeaderLength()+off, rec); }
    /** Unpacks an ASCII string from the payload of the packet. */
    protected: inline string        unpackPayloadAscii     (int32_t off,            int32_t len) const { return VRTMath::unpackAscii(    bbuf, getHeaderLength()+off, len); }
    /** Unpacks a UTF-8 string from the payload of the packet. */
    protected: inline wstring       unpackPayloadUTF8      (int32_t off,            int32_t len) const { return VRTMath::unpackUTF8(     bbuf, getHeaderLength()+off, len); }
    /** Unpacks a metadata block from the payload of the packet. */
    protected: inline MetadataBlock unpackPayloadMetadata  (int32_t off,            int32_t len) const { return VRTMath::unpackMetadata( bbuf, getHeaderLength()+off, len); }
    /** Unpacks a block of bytes from the payload of the packet. */
    protected: inline vector<char>  unpackPayloadBytes     (int32_t off,            int32_t len) const { return VRTMath::unpackBytes(    bbuf, getHeaderLength()+off, len); }
    /** Unpacks a block of bytes from the payload of the packet. */
    protected: inline void*         unpackPayloadBytes     (int32_t off, void *val, int32_t len) const { return VRTMath::unpackBytes(    bbuf, getHeaderLength()+off, val, len); }
    /** Unpacks a {@link TimeStamp} from the payload of the packet. */
    protected: inline TimeStamp     unpackPayloadTimeStamp (int32_t off, IntegerMode epoch     ) const { return VRTMath::unpackTimeStamp(bbuf, getHeaderLength()+off, epoch); }
    /** Unpacks a {@link InetAddress} from the payload of the packet. */
    protected: inline InetAddress   unpackPayloadInetAddr  (int32_t off                        ) const { return VRTMath::unpackInetAddr( bbuf, getHeaderLength()+off); }
    /** Unpacks a {@link UUID} from the payload of the packet. */
    protected: inline UUID          unpackPayloadUUID      (int32_t off                        ) const { return VRTMath::unpackUUID(     bbuf, getHeaderLength()+off); }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Implement HasFields
    //////////////////////////////////////////////////////////////////////////////////////////////////
    public: virtual int32_t   getFieldCount () const;
    public: virtual string    getFieldName  (int32_t id) const;
    public: virtual ValueType getFieldType  (int32_t id) const;
    public: virtual Value*    getField      (int32_t id) const __attribute__((warn_unused_result));
    public: virtual void      setField      (int32_t id, const Value* val);
  };
#endif /* NOT_USING_JNI */

#if NOT_USING_JNI
  /** Specifies the payload format. <br>
   *  <br>
   *  The following implementation is used for the {@link HasFields} methods:
   *  <pre>
   *     ID | Name                 | Type
   *    ----+----------------------+---------------
   *     0  | ProcessingEfficient  | Bool
   *     1  | RealComplexType      | Int8
   *     2  | DataItemFormat       | Int8
   *     3  | Repeating            | Bool
   *     4  | EventTagSize         | Int32
   *     5  | ChannelTagSize       | Int32
   *     6  | ItemPackingFieldSize | Int32
   *     7  | DataItemSize         | Int32
   *     8  | RepeatCount          | Int32
   *     9  | VectorSize           | Int32
   *     10 | DataType             | Int64
   *    ----+----------------------+---------------
   *  </pre>
   */
  class PayloadFormat : public VRTObject, public HasFields {
    // We simulate a null value for a PayloadFormat by setting the reserved bits
    // (which must be 0x0 in ANSI/VITA-49.0) to 0xF. We need to do this in the
    // C++ version since the hi=0,lo=0 is a valid PayloadFormat (though one that
    // users will rarely use). In earlier versions we initialized hi=-1,lo=-1
    // but this caused various issues when it came to users doing something of
    // the form:
    //   PayloadFormat pf;
    //   pf.setDataType(..);
    //   ...
    // Now on every set we simply mask off the bits being set with the mask that
    // will force the other bits to be cleared. The use of MASK_VAL_LO is here
    // for consistency/readability, it has no real use and should be optimized
    // away by the compiler.
    private: static const int32_t NULL_VAL_HI = 0x00000F000;   // null value for hi
    private: static const int32_t NULL_VAL_LO = 0x000000000;   // null value for lo
    private: static const int32_t MASK_VAL_HI = ~NULL_VAL_HI;  // clears null value for hi
    private: static const int32_t MASK_VAL_LO = ~NULL_VAL_LO;  // clears null value for lo

    private: int32_t hi; // high-order bits
    private: int32_t lo; // low-order bits

    /** Creates new instance. */
    public: PayloadFormat ();

    /** Creates new instance. */
    public: PayloadFormat (const PayloadFormat &pf);

    /** Creates new instance. This is identical to
     *  <tt>new PayloadFormat(true,RealComplexType.Real,format,false,0,0,size,size,1,1)</tt>.
     *  @param format        The data format used.
     *  @param size          The data item size.
     */
    public: PayloadFormat (DataItemFormat format, int32_t size);

    /** Creates new instance. This is identical to
     *  <tt>new PayloadFormat(true,type,format,false,0,0,size,size,1,1)</tt>.
     *  @param type          The real/complex type used.
     *  @param format        The data format used.
     *  @param size          The data item size.
     */
    public: PayloadFormat (RealComplexType type, DataItemFormat format, int32_t size);

    /** Creates new instance.
     *  @param dataType     The data format used.
     *  @param realComplex  The real/complex type used.
     */
    public: PayloadFormat (DataType dataType, RealComplexType realComplex=RealComplexType_Real);

    /** Creates new instance.
     *  @param procEfficient true if Processing Efficient or false if Link Efficient.
     *  @param type          The real/complex type used.
     *  @param format        The data format used.
     *  @param repeating     Indicates if the sample-component repeat is used.
     *  @param eventTagSize  the event tag size.
     *  @param chanTagSize   the channel tag size.
     *  @param fieldSize     The item packing field size.
     *  @param itemSize      The data item size.
     *  @param repeatCount   The repeat count.
     *  @param vectorSize    The vector size.
     */
    public: PayloadFormat (bool procEfficient, RealComplexType type, DataItemFormat format,
                           bool repeating, int32_t eventTagSize, int32_t chanTagSize, int32_t fieldSize,
                           int32_t itemSize, int32_t repeatCount, int32_t vectorSize);

    /** Is this object equal to null. Since the values 0x0000000000000000 and 0x8000000000000000
     *  are both valid and 0xFFFFFFFFFFFFFFFF is invalid, this object uses -1 as the indicator for
     *  a null payload format.
     */
    public: inline virtual bool isNullValue () const {
      return (hi == NULL_VAL_HI)
          && (lo == NULL_VAL_LO);
    }

    /** <i>Internal Use Only:</i> Creates new instance. */
    public: PayloadFormat (int64_t bits);

    public: virtual string toString () const;

    using VRTObject::equals;
    public: virtual bool equals (const VRTObject &o) const;

    /** Checks to see if the payload format is valid.
     *  @return true if it is valid, false otherwise.
     */
    public: inline bool isValid () const {
      return (getValid() == "");
    }

    /** Checks to see if the payload format is valid. This is identical to
     *  {@link #isValid()} except that a description of the error is returned.
     *  @return "" if it is valid, a description of the error otherwise.
     */
    public: virtual string getValid () const;

    /** <i>Internal Use Only:</i> Gets the bits. */
    public: inline int64_t getBits () const {
      return (((int64_t)hi) << 32) | (((int64_t)lo) & __INT64_C(0xFFFFFFFF));
    }

    /** <i>Internal Use Only:</i> Sets the bits. */
    public: inline void setBits (int64_t bits) {
      hi = ((int32_t)(bits >> 32)) & 0xFFFF0FFF;
      lo = (int32_t)bits;
    }

    /** Indicates if the processing efficient packing method used.
     *  @return true if Processing Efficient (byte-aligned) or false if Link Efficient (fewest bytes used).
     */
    public: inline bool isProcessingEfficient () const {
      int32_t mask = 0x80000000;
      return ((hi & mask) == 0);
    }

    /** Indicates if the processing efficient packing method used.
     *  @param val true if Processing Efficient (byte-aligned) or false if Link Efficient (fewest bytes used).
     */
    public: inline void setProcessingEfficient (bool val) {
      int32_t mask = 0x80000000;
      int32_t set  = (val)? 0 : mask;
      hi = (set | (hi & ~mask)) & MASK_VAL_HI;
      lo = lo                   & MASK_VAL_LO;
    }

    /** Gets the real/complex type used.
     *  @return The type used.
     */
    public: inline RealComplexType getRealComplexType () const {
      int32_t mask = 0x60000000;
      int32_t val  = (hi & mask) >> 29;
      return (RealComplexType)val;
    }

    /** Sets the real/complex type used.
     *  @param val The type used.
     *  @throws VRTException If the specified value is unsupported.
     */
    public: inline void setRealComplexType (RealComplexType val) {
      int32_t mask = 0x60000000;
      int32_t set  = ((int32_t)val) << 29;
      hi = (set | (hi & ~mask)) & MASK_VAL_HI;
      lo = (lo                ) & MASK_VAL_LO;
    }

    /** <i>Utility method:</i> If this payload format matches one of the pre-defined data types,
        and is stored without any "extra bits", return that type. Although technically "unsigned"
        this method will recognize any 1-bit integer data as <tt>DataType_UInt1</tt>.
        @return The data type or null if not a pre-defined type.
     */
    public: DataType getDataType () const;

    /** <i>Utility method:</i> Sets the payload format to match one of the pre-defined data types.
        @param type The data type.
        @throws NullPointerException if type is null.
     */
    public: void setDataType (DataType type);

    /** <i>Utility method:</i> Indicates if the payload format is complex.
        @return <tt>true</tt> if the data format is complex, <tt>false</tt> if it is real.
     */
    public: inline bool isComplex () const {
      return (getRealComplexType() != RealComplexType_Real);
    }

    /** <i>Utility method:</i> Indicates if the payload format is signed.
        @return <tt>true</tt> if the data format is signed, <tt>false</tt> if it is unsigned.
     */
    public: inline bool isSigned () const {
      return (getDataItemFormat() < 16);
    }

    /** Gets the data format used.
     *  @return The format used.
     */
    public: inline DataItemFormat getDataItemFormat () const {
      int32_t mask = 0x1F000000;
      int32_t val  = (hi & mask) >> 24;
      return (DataItemFormat)val;
    }

    /** Sets the data format used.
     *  @param val The format used.
     *  @throws VRTException If the specified value is unsupported.
     */
    public: inline void setDataItemFormat (DataItemFormat val) {
      int32_t mask = 0x1F000000;
      int32_t set  = ((int32_t)val) << 24;
      hi = (set | (hi & ~mask)) & MASK_VAL_HI;
      lo = (lo                ) & MASK_VAL_LO;
    }

    /** Indicates if the sample-component repeat is used.
     *  @return true if it is being used, false if not.
     */
    public: inline bool isRepeating () const {
      int32_t mask = 0x00800000;
      return ((hi & mask) != 0);
    }

    /** Indicates if the sample-component repeat is used.
     *  @param val true if it is being used, false if not.
     */
    public: inline void setRepeating (bool val) {
      int32_t mask = 0x00800000;
      int32_t set  = (val)? mask : 0;
      hi = (set | (hi & ~mask)) & MASK_VAL_HI;
      lo = (lo                ) & MASK_VAL_LO;
    }

    /** Gets the event tag size.
     *  @return The event tag size (0..7).
     */
    public: inline int32_t getEventTagSize () const {
      int32_t mask = 0x00700000;
      int32_t val  = (hi & mask) >> 20;
      return val;
    }

    /** Sets the event tag size.
     *  @param val The event tag size (0..7).
     *  @throws VRTException If the specified value is not in the allowable range.
     */
    public: inline void setEventTagSize (int32_t val) {
      if ((val < 0) || (val > 7)) {
        throw VRTException("Value (%d) not in range of 0..7", val);
      }
      int32_t mask = 0x00700000;
      int32_t set  = val << 20;
      hi = (set | (hi & ~mask)) & MASK_VAL_HI;
      lo = (lo                ) & MASK_VAL_LO;
    }

    /** Gets the channel tag size.
     *  @return The channel tag size (0..15).
     */
    public: inline int getChannelTagSize () const {
      int32_t mask = 0x000F0000;
      int32_t val  = (hi & mask) >> 16;
      return val;
    }

    /** Sets the channel tag size.
     *  @param val The channel tag size (0..15).
     *  @throws VRTException If the specified value is not in the allowable range.
     */
    public: inline void setChannelTagSize (int32_t val) {
      if ((val < 0) || (val > 15)) {
        throw VRTException("Value (%d) not in range of 0..15", val);
      }
      int32_t mask = 0x000F0000;
      int32_t set  = val << 16;
      hi = (set | (hi & ~mask)) & MASK_VAL_HI;
      lo = (lo                ) & MASK_VAL_LO;
    }

    /** Gets the item packing field size.
     *  @return The field size (0..63).
     */
    public: inline int32_t getItemPackingFieldSize () const {
      int32_t mask = 0x00000FC0;
      int32_t val  = (hi & mask) >> 6;
      return val+1;
    }

    /** Sets the item packing field size.
     *  @param val The field size (0..63).
     *  @throws VRTException If the specified value is not in the allowable range.
     */
    public: inline void setItemPackingFieldSize (int32_t val) {
      if ((val < 1) || (val > 64)) {
        throw VRTException("Value (%d) not in range of 1..64", val);
      }
      int32_t mask = 0x00000FC0;
      int32_t set  = (val-1) << 6;
      hi = (set | (hi & ~mask)) & MASK_VAL_HI;
      lo = (lo                ) & MASK_VAL_LO;
    }

    /** Gets the data item size.
     *  @return The data item size (0..63).
     */
    public: inline int32_t getDataItemSize () const {
      int32_t mask = 0x0000003F;
      int32_t val  = (hi & mask);
      return val+1;
    }

    /** Sets the data item size.
     *  @param val The data item size (0..63).
     *  @throws VRTException If the specified value is not in the allowable range.
     */
    public: inline void setDataItemSize (int32_t val) {
      if ((val < 1) || (val > 64)) {
        throw VRTException("Value (%d) not in range of 1..64", val);
      }
      int32_t mask = 0x0000003F;
      int32_t set  = (val-1);
      hi = (set | (hi & ~mask)) & MASK_VAL_HI;
      lo = (lo                ) & MASK_VAL_LO;
    }

    /** Gets the repeat count.
     *  @return The repeat count (1..65536).
     */
    public: inline int32_t getRepeatCount () const {
      return (0x0000FFFF&(lo >> 16))+1;
    }

    /** Sets the repeat count.
     *  @param val The repeat count (1..65536).
     *  @throws VRTException If the specified value is not in the allowable range.
     */
    public: inline void setRepeatCount (int32_t val) {
      if ((val < 1) || (val > 65536)) {
        throw VRTException("Value (%d) not in range of 1..65536", val);
      }
      hi = (hi                                 ) & MASK_VAL_HI;
      lo = (((val-1) << 16) | (lo & 0x0000FFFF)) & MASK_VAL_LO;
    }

    /** Gets the vector size.
     *  @return The vector size (1..65536).
     */
    public: inline int32_t getVectorSize () const {
      return (lo & 0x0000FFFF)+1;
    }

    /** Sets the vector size.
     *  @param val The vector size (1..65536).
     *  @throws VRTException If the specified value is not in the allowable range.
     */
    public: inline void setVectorSize (int32_t val) {
      if ((val < 1) || (val > 65536)) {
        throw VRTException("Value (%d) not in range of 1..65536", val);
      }
      hi = (hi                         ) & MASK_VAL_HI;
      lo = ((val-1) | (lo & 0xFFFF0000)) & MASK_VAL_LO;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Implement HasFields
    //////////////////////////////////////////////////////////////////////////////////////////////////
    public: virtual int32_t   getFieldCount () const;
    public: virtual string    getFieldName  (int32_t id) const;
    public: virtual ValueType getFieldType  (int32_t id) const;
    public: virtual Value*    getField      (int32_t id) const __attribute__((warn_unused_result));
    public: virtual void      setField      (int32_t id, const Value* val);
  };
#endif /* NOT_USING_JNI */

#if NOT_USING_JNI
  /** Standard payload format: Real, signed 4-bit integer. */
  const PayloadFormat PayloadFormat_INT4        (DataType_Int4);
  /** Standard payload format: Real, signed 8-bit integer. */
  const PayloadFormat PayloadFormat_INT8        (DataType_Int8);
  /** Standard payload format: Real, signed 16-bit integer. */
  const PayloadFormat PayloadFormat_INT16       (DataType_Int16);
  /** Standard payload format: Real, signed 32-bit integer. */
  const PayloadFormat PayloadFormat_INT32       (DataType_Int32);
  /** Standard payload format: Real, signed 64-bit integer. */
  const PayloadFormat PayloadFormat_INT64       (DataType_Int64);
  /** Standard payload format: Real, 32-bit float. */
  const PayloadFormat PayloadFormat_FLOAT32     (DataType_Float);
  /** Standard payload format: Real, 64-bit double. */
  const PayloadFormat PayloadFormat_DOUBLE64    (DataType_Double);
  /** Standard payload format: Real, unsigned 1-bit packed bits. */
  const PayloadFormat PayloadFormat_UINT1       (DataType_UInt1);
  /** Standard payload format: Real, unsigned 4-bit integer. */
  const PayloadFormat PayloadFormat_UINT4       (DataType_UInt4);
  /** Standard payload format: Real, unsigned 8-bit integer. */
  const PayloadFormat PayloadFormat_UINT8       (DataType_UInt8);
  /** Standard payload format: Real, unsigned 16-bit integer. */
  const PayloadFormat PayloadFormat_UINT16      (DataType_UInt16);
  /** Standard payload format: Real, unsigned 32-bit integer. */
  const PayloadFormat PayloadFormat_UINT32      (DataType_UInt32);
  /** Standard payload format: Real, unsigned 64-bit integer. */
  const PayloadFormat PayloadFormat_UINT64      (DataType_UInt64);
#endif /* NOT_USING_JNI */

  namespace VRTMath {
    /** Converts from an IEEE-754 double-precision floating-point value to the
     *  packed bits for a VRT floating-point. <br>
     *  <br>
     *  Although VRT floating-point values can express many values in multiple
     *  ways within the binary form (e.g. 1/4 or 2/8) this function outputs the
     *  values in a "normalized form" where the output value will have a 1 in
     *  the most-significant-(non-sign-)bit of the mantissa, excepting for small
     *  values close to zero where the exponent is zero. This mirrors the behavior
     *  of IEEE-754 floating-point values, excepting that the highest bit is
     *  always present (not just for "denormal" values). <br>
     *  <br>
     *  Since the range of VRT floating-point numbers is [0,1) (signed) or [-1,1)
     *  (unsigned) any values below/above that range will be converted to the
     *  smallest/largest value possible in that range. Any NaN values passed in
     *  will be converted to zero.
     *  <pre>
     *    References:
     *      [1] ANSI/VITA 49.0, Appendix D
     *      [2] IEEE-754
     *  </pre>
     * @param form  The data item format. This must be one of the VRT
     *              floating-point formats.
     * @param dSize The data size in bits (2..63).
     * @param val   The value to convert.
     * @return The packed bits for the VRT floating-point number.
     */
    int64_t toVRTFloat (DataItemFormat form, int32_t dSize, double val); // code in VRTMath.cc

    /** <b>Internal Use Only:</b> Converts from an IEEE-754 double-precision
     *  floating-point value to the packed bits for a VRT floating-point
     *  <b>without normal safety checks</b>.
     *  @param sign  Is output signed?
     *  @param eSize The number of exponent bits (1 to 6).
     *  @param dSize The data size in bits (2..63).
     *  @param val   The value to convert.
     */
    int32_t toVRTFloat32 (bool sign, int32_t eSize, int32_t dSize, double val); // code in VRTMath.cc

    /** <b>Internal Use Only:</b> Converts from an IEEE-754 double-precision
     *  floating-point value to the packed bits for a VRT floating-point
     *  <b>without normal safety checks</b>.
     *  @param sign  Is output signed?
     *  @param eSize The number of exponent bits (1 to 6).
     *  @param dSize The data size in bits (2..63).
     *  @param val   The value to convert.
     */
    int64_t toVRTFloat64 (bool sign, int32_t eSize, int32_t dSize, double val); // code in VRTMath.cc

    /** Converts from the packed bits for a VRT floating-point to an IEEE-754
     *  double-precision floating-point value. <br>
     *  <br>
     *  Despite the fact that {@link #toVRTFloat} normalizes its output values,
     *  this function will support all of the valid VRT forms and convert them to
     *  their respective IEEE-754 value. Since IEEE-754 values are normalized this
     *  process results not reflective.
     *  <pre>
     *    References:
     *      [1] ANSI/VITA 49.0, Appendix D
     *      [2] IEEE-754
     *  </pre>
     *  @param form  The data item format. This must be one of the VRT
     *               floating-point formats.
     *  @param dSize The data size in bits (2..63).
     *  @param bits  The packed bits to convert.
     *  @return The IEEE-754 floating-point number.
     */
    double fromVRTFloat (DataItemFormat form, int32_t dSize, int64_t bits); // code in VRTMath.cc

    /** <b>Internal Use Only:</b> Converts from the packed bits for a VRT
     *  floating-point to an IEEE-754 double-precision floating-point value
     *  <b>without normal safety checks</b>.
     *  @param sign  Is output signed?
     *  @param eSize The number of exponent bits (1 to 6).
     *  @param dSize The data size in bits (2..63).
     *  @param bits  The packed bits to convert.
     */
    double fromVRTFloat32 (bool sign, int32_t eSize, int32_t dSize, int32_t bits); // code in VRTMath.cc

    /** <b>Internal Use Only:</b> Converts from the packed bits for a VRT
     *  floating-point to an IEEE-754 double-precision floating-point value
     *  <b>without normal safety checks</b>.
     *  @param sign  Is output signed?
     *  @param eSize The number of exponent bits (1 to 6).
     *  @param dSize The data size in bits (2..63).
     *  @param bits  The packed bits to convert.
     */
    double fromVRTFloat64 (bool sign, int32_t eSize, int32_t dSize, int64_t bits); // code in VRTMath.cc
  } END_NAMESPACE
} END_NAMESPACE

#if NOT_USING_JNI
string   operator+  (string  &s, vrt::PacketType val);
ostream& operator<< (ostream &s, vrt::PacketType val);
string   operator+  (string  &s, vrt::RealComplexType val);
ostream& operator<< (ostream &s, vrt::RealComplexType val);
string   operator+  (string  &s, vrt::DataItemFormat val);
ostream& operator<< (ostream &s, vrt::DataItemFormat val);
string   operator+  (string  &s, vrt::DataType val);
ostream& operator<< (ostream &s, vrt::DataType val);
#endif /* NOT_USING_JNI */

#endif /* BasicVRTPacket_h */

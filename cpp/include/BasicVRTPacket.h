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
#include "PayloadFormat.h"
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
    /** A standard command packet (with stream identifier).   */    PacketType_Command             = 6,
    /** An extension command packet (with stream identifier). */    PacketType_ExtCommand          = 7,
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
  bool PacketType_isContext (PacketType pt);
  bool PacketType_isCommand (PacketType pt);
  bool PacketType_hasStreamIdentifier (PacketType pt);
#endif

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
     *  @see #getPacketLength()
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

    /** The maximum length of the packet specific prologue (PSP) in bytes (all optional fields
     *  included). Note that in cases where optional fields are not included, the PSP will
     *  be shorter.
     *  <pre>
     *    MAX_PKT_SPECIFIC_PROLOGUE_LENGTH = 10*4
     *  </pre>
     *  @see #getPktSpecificPrologueLength()
     */
    public: static const int32_t MAX_PKT_SPECIFIC_PROLOGUE_LENGTH = 10*4;

    /** The maximum length of the prologue in bytes (all optional fields included). Note that
     *  in cases where optional fields are not included, the prologue will be shorter.
     *  <pre>
     *    MAX_PROLOGUE_LENGTH = MAX_HEADER_LENGTH + MAX_PKT_SPECIFIC_PROLOGUE_LENGTH
     *  </pre>
     *  @see #getPrologueLength()
     */
    public: static const int32_t MAX_PROLOGUE_LENGTH = MAX_HEADER_LENGTH + MAX_PKT_SPECIFIC_PROLOGUE_LENGTH;

    /** The maximum length of the trailer in bytes.
     *  <pre>
     *    MAX_TRAILER_LENGTH = 1*4
     *  </pre>
     *  @see #getTrailerLength()
     */
    public: static const int32_t MAX_TRAILER_LENGTH = 1*4;

    /** The <b>de-facto</b> maximum length of the payload in bytes. Note that the de-jure limit
     *  can be up to 28 bytes more than this when optional fields in the header, packet specific
     *  prologue, and trailer are dropped. In practice this difference is nearly meaningless since
     *  (as noted with {@link #MAX_PACKET_LENGTH}) this value exceeds the limits of the underlying
     *  transport protocols.
     *  <pre>
     *    MAX_PAYLOAD_LENGTH = MAX_PACKET_LENGTH - MAX_PROLOGUE_LENGTH - MAX_TRAILER_LENGTH
     *  </pre>
     *  @see #getPayloadLength()
     */
    public: static const int32_t MAX_PAYLOAD_LENGTH = MAX_PACKET_LENGTH - MAX_PROLOGUE_LENGTH - MAX_TRAILER_LENGTH;

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
     */  // TODO - length of 4?? Implementation sets pkt size to 7 (max prologue size), which is the number of 32-bit words, not bytes.
    public: BasicVRTPacket (int32_t bufsize);

    /** Creates a new instance accessing the given data buffer.
     *  @param buf      The pointer to the packet buffer.
     *  @param len      The length  of the packet buffer.
     *  @param readOnly Should users of this instance be able to modify the underlying data buffer?
     */
    public: BasicVRTPacket (const void *buf, size_t len, bool readOnly=false);

    /** Creates a new instance accessing the given data buffer.
     *  @param buf_ptr  The pointer to the packet buffer.
     *  @param readOnly Should users of this instance be able to modify the underlying data buffer?
     */
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

    /** Checks for prologue equality with another BasicVRTPacket. The prologues of two VRT packets are
     *  considered equal if the content of the prologues is bit-for-bit identical. The contents of the
     *  payload and trailer need not be equal for this method to return true. However, any two packets
     *  for which {@link #equals} returns true, must have this method return true as well.
     *  @param p The other packet.
     *  @return true if they are identical, false otherwise. (This will always returns false if the
     *          packet is null.)
     */
    public: bool prologueEquals (const BasicVRTPacket &p) const;

    /** Checks for header equality with another BasicVRTPacket. The headers of two VRT packets are
     *  considered equal if the content of the headers is bit-for-bit identical. The contents of the
     *  payload and trailer need not be equal for this method to return true. However, any two packets
     *  for which {@link #equals} returns true, must have this method return true as well.
     *  @param p The other packet.
     *  @return true if they are identical, false otherwise. (This will always returns false if the
     *          packet is null.)
     */
    public: bool headerEquals (const BasicVRTPacket &p) const;

    /** Checks for packet specific prologue (PSP) equality with another BasicVRTPacket. The PSPs of
     *  two VRT packets are considered equal if the content of the PSPs is bit-for-bit identical. The
     *  contents of the payload and trailer need not be equal for this method to return true. However,
     *  any two packets for which {@link #equals} returns true, must have this method return true as
     *  well.
     *  @param p The other packet.
     *  @return true if they are identical, false otherwise. (This will always returns false if the
     *          packet is null.)
     */
    public: bool pktSpecificPrologueEquals (const BasicVRTPacket &p) const;

    /** Checks for payload equality with another BasicVRTPacket. The payloads of two VRT packets are
     *  considered equal if the content of the payloads is bit-for-bit identical. The contents of the
     *  prologue and trailer need not be equal for this method to return true. However, any two packets
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

    /** <b>Internal use only:</b> Shifts the header.
     *  Creates or removes space for values in header.
     *  @param field bit field used to identify if field is currently included and the offset
     *         based on other fields that may be present.
     *  @param bytes size of value to be included or removed.
     *  @param present true if need space to store value associated with field, 
     *         false if the value is not present in header.
     */
    protected: void shiftHeader (int32_t field, int32_t bytes, bool present);

    /** <b>Internal use only:</b> Shifts the trailer.
     *  Add or removes trailer of size MAX_TRAILER_LENGTH.
     *  @param add Indicates whether to add or remove trailer. If add is false, removes
     *         trailer from the packet and unsets the Trailer bit in the header. If add is
     *         true, increases packet by MAX_TRAILER_LENGTH and sets Trailer bit in header.
     */
    protected: void shiftTrailer (bool add);

    /** <b>Internal use only:</b> Shifts the packet specific prologue (PSP).
     *  Insert or remove bytes at given offset.
     *  @param off Offset w/in PSP to insert/remove bytes. off>=0 indicates value exists already,
     *             while off<0 indicates it is not present.
     *  @param bytes Non-negative number of bytes to insert or remove.
     *  @param add Indicates whether to add or remove bytes. Nothing is done if True and value exists,
     *             or if False and value is not present.
     *  @return Non-negative value of off param, regardless of action taken (or not taken).
     */
    protected: int32_t shiftPacketSpecificPrologue (int32_t off, int32_t bytes, bool add);

    /** <b>Internal use only:</b> Sets the packet length. */
    protected: void setPacketLength (int32_t v);

    /** Does this packet have a packet specific prologue? */
    public: bool    hasPacketSpecificPrologue () const;

    /** Does this packet have a trailer? */
    public: bool    hasTrailer () const;

    /** <b>Internal use only:</b> Shifts the payload.
     *  Insert or remove bytes at given offset.
     *  @param off Offset w/in payload to insert/remove bytes. off>=0 indicates value exists already,
     *             while off<0 indicates it is not present.
     *  @param bytes Non-negative number of bytes to insert or remove.
     *  @param add Indicates whether to add or remove bytes. Nothing is done if True and value exists,
     *             or if False and value is not present.
     *  @return Non-negative value of off param, regardless of action taken (or not taken).
     */
    protected: int32_t shiftPayload (int32_t off, int32_t bytes, bool add);

    /** Is the packet type a data packet? */
    public: bool isData () const;
    /** Is the packet type a context packet? */
    public: bool isContext () const;
    /** Is the packet type a command packet? */
    public: bool isCommand () const;

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

    /** Data packets only: Gets the Signal Spectrum Data Mode (S). This indicates if a data
     *  packet is used to convey Signal Spectrum Data.
     *  <pre>
     *    true  = Signal Data Packet contains spectral or frequency domain data.
     *    false = Signal Data Packet contains time domain data.
     *    null  = <i>Error: not a data packet</i>
     *  </pre>
     *  @return true or false as indicated above or null if this is a data packet.
     */
    public: virtual boolNull isSpectrumMode () const;

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

    /** Command packets only: Gets the Control/Ack Mode (ACK). This indicates if a command packet
     *  is a Control or Ack subtype:
     *  <pre>
     *    true  = Ack subtype
     *    false = Control subtype
     *    null  = <i>Error: not a command packet</i>
     *  </pre>
     *  @return true or false as indicated above or null if this is not a command packet.
     */
    public: virtual boolNull isControlAckMode () const;

    /** Command packets only: Gets the Cancel Mode (L). This indicates if a command packet
     *  is a Control-Cancel packet:
     *  <pre>
     *    true  = Control-Cancel packet
     *    false = Not a Control-Cancel packet (which leaves Control or one of the Ack subtypes).
     *    null  = <i>Error: not a command packet</i>
     *  </pre>
     *  @return true or false as indicated above or null if this is not a command packet.
     */
    public: virtual boolNull isCancelMode () const;


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
     *    payloadLength = {@link #getPacketLength()} - {@link #getPrologueLength()} - {@link #getTrailerLength()}
     *  </pre>
     *  The payload length will always be a multiple of 4-bytes.
     *  @return The length of the payload in bytes.
     */
    public: inline int32_t getPayloadLength () const {
      return getPacketLength() - getPrologueLength() - getTrailerLength();
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

    /** Gets the header length in bytes (does not include packet specific prologue or trailer). The
     *  length of the header will be a value <b>up to</b> {@link #MAX_HEADER_LENGTH}. The header length
     *  will always be a multiple of 4-bytes.
     *  @return The length of the header in bytes.
     */
    public: virtual int32_t getHeaderLength () const;

    /** Gets the packet specific prologue (PSP) length in bytes. The length of the PSP will be a value
     *  <b>up to</b> {@link #MAX_PKT_SPECIFIC_PROLOGUE_LENGTH}. The PSP length will always be a multiple of 4-bytes.
     *  @return The length of the packet specific prologue in bytes.
     */
    public: virtual int32_t getPktSpecificPrologueLength () const;

    /** Gets the prologue length in bytes (includes header and packet specific prologue, but does not
     *  include trailer). The length of the prologue will be a value <b>up to</b> {@link #MAX_PROLOGUE_LENGTH}.
     *  The prologue length will always be a multiple of 4-bytes.
     *  @return The length of the prologue in bytes.
     */
    public: virtual int32_t getPrologueLength () const;

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

    /** <i>Optional functionality:</i> Data packets only: Sets the Signal Spectrum Data Mode (S).
     *  @param v The new value, see {@link #isSpectrumMode()} for details.
     *  @throws VRTException If this method is not supported or if the packet type
     *                                        doesn't support this setting.
     */
    public: virtual void setSpectrumMode (bool v);

    /** <i>Optional functionality:</i> Context packets only: Sets the Time Stamp Mode (TSM).
     *  @param v The new value, see {@link #isTimeStampMode()} for details.
     *  @throws VRTException If this method is not supported or if the packet type
     *                                        doesn't support this setting.
     */
    public: virtual void setTimeStampMode (bool v);

    /** <i>Optional functionality:</i> Command packets only: Sets the Control/Ack Mode (ACK).
     *  @param v The new value, see {@link #isControlAckMode()} for details.
     *  @throws VRTException If this method is not supported or if the packet type
     *                                        doesn't support this setting.
     */
    public: virtual void setControlAckMode (bool v);

    /** <i>Optional functionality:</i> Command packets only: Gets the Cancel Mode (L).
     *  @param v The new value, see {@link #isCancelMode()} for details.
     *  @throws VRTException If this method is not supported or if the packet type
     *                                        doesn't support this setting.
     */
    public: virtual void setCancelMode (bool v);

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
    protected: inline void packPayloadByte      (int32_t off, char     val) { VRTMath::packByte(    bbuf, getPrologueLength()+off, val); }
    /** Packs a 16-bit integer into the payload of the packet. */
    protected: inline void packPayloadShort     (int32_t off, int16_t  val) { VRTMath::packShort(   bbuf, getPrologueLength()+off, val); }
    /** Packs a 32-bit integer into the payload of the packet. */
    protected: inline void packPayloadInt       (int32_t off, int32_t  val) { VRTMath::packInt(     bbuf, getPrologueLength()+off, val); }
    /** Packs a 64-bit integer into the payload of the packet. */
    protected: inline void packPayloadLong      (int32_t off, int64_t  val) { VRTMath::packLong(    bbuf, getPrologueLength()+off, val); }
    /** Packs a 32-bit floating-point value into the payload of the packet. */
    protected: inline void packPayloadFloat     (int32_t off, float    val) { VRTMath::packFloat(   bbuf, getPrologueLength()+off, val); }
    /** Packs a 64-bit floating-point value into the payload of the packet. */
    protected: inline void packPayloadDouble    (int32_t off, double   val) { VRTMath::packDouble(  bbuf, getPrologueLength()+off, val); }
    /** Packs a <tt>boolNull</tt> value into the payload of the packet. */
    protected: inline void packPayloadBoolNull  (int32_t off, boolNull val) { VRTMath::packBoolNull(bbuf, getPrologueLength()+off, val); }
    /** Packs a boolean value into the payload of the packet. */
    protected: inline void packPayloadBoolean   (int32_t off, bool     val) { VRTMath::packBoolean( bbuf, getPrologueLength()+off, val); }
    /** Packs a record value into the payload of the packet. */
    protected: inline void packPayloadRecord    (int32_t off, Record   val) { VRTMath::packRecord(  bbuf, getPrologueLength()+off, val); }
    /** Packs a metadata block into the payload of the packet. */
    protected: inline void packPayloadMetadata  (int32_t off, MetadataBlock       val, int32_t len      ) { VRTMath::packMetadata( bbuf, getPrologueLength()+off, val, len); }
    /** Packs a block of bytes into the payload of the packet. */
    protected: inline void packPayloadBytes     (int32_t off, const vector<char> &val, int32_t len      ) { VRTMath::packBytes(    bbuf, getPrologueLength()+off, val, len); }
    /** Packs a block of bytes into the payload of the packet. */
    protected: inline void packPayloadBytes     (int32_t off, const void         *val, int32_t len      ) { VRTMath::packBytes(    bbuf, getPrologueLength()+off, val, len); }
    /** Packs a {@link TimeStamp} into the payload of the packet. */
    protected: inline void packPayloadTimeStamp (int32_t off, TimeStamp           val, IntegerMode epoch) { VRTMath::packTimeStamp(bbuf, getPrologueLength()+off, val, epoch); }
    /** Packs a {@link InetAddress} into the payload of the packet. */
    protected: inline void packPayloadInetAddr  (int32_t off, InetAddress         val                   ) { VRTMath::packInetAddr( bbuf, getPrologueLength()+off, val); }
    /** Packs a {@link UUID} into the payload of the packet. */
    protected: inline void packPayloadUUID      (int32_t off, UUID                val                   ) { VRTMath::packUUID(     bbuf, getPrologueLength()+off, val); }

    /** Packs an ASCII string into the payload of the packet. */
    protected: inline void packPayloadAscii (int32_t off, string val, int32_t len) {
      if (len < 0) { // compute max length
        int32_t max = -len;
        setPayloadLength(off+max);
        int32_t act = VRTMath::packAscii(bbuf, getPrologueLength()+off, val, max);
        if (act < max) setPayloadLength(off+act);
      }
      else {
        VRTMath::packAscii(bbuf, getPrologueLength()+off, val, len);
      }
    }

    /** Packs a UTF-8 string into the payload of the packet. */
    protected: inline void packPayloadUTF8 (int32_t off, wstring val, int32_t len) {
      if (len < 0) { // compute max length
        int32_t max = -len;
        setPayloadLength(off+max);
        int32_t act = VRTMath::packUTF8(bbuf, getPrologueLength()+off, val, max);
        if (act < max) setPayloadLength(off+act);
      }
      else {
        VRTMath::packUTF8(bbuf, getPrologueLength()+off, val, len);
      }
    }

    /** Packs a UTF-8 string into the payload of the packet. */
    protected: inline void packPayloadUTF8 (int32_t off, string val, int32_t len) {
      if (len < 0) { // compute max length
        int32_t max = -len;
        setPayloadLength(off+max);
        int32_t act = VRTMath::packUTF8(bbuf, getPrologueLength()+off, val, max);
        if (act < max) setPayloadLength(off+act);
      }
      else {
        VRTMath::packUTF8(bbuf, getPrologueLength()+off, val, len);
      }
    }

    /** Unpacks an 8-bit integer from the payload of the packet. */
    protected: inline char          unpackPayloadByte      (int32_t off                        ) const { return VRTMath::unpackByte(     bbuf, getPrologueLength()+off); }
    /** Unpacks a 16-bit integer from the payload of the packet. */
    protected: inline int16_t       unpackPayloadShort     (int32_t off                        ) const { return VRTMath::unpackShort(    bbuf, getPrologueLength()+off); }
    /** Unpacks a 32-bit integer from the payload of the packet. */
    protected: inline int32_t       unpackPayloadInt       (int32_t off                        ) const { return VRTMath::unpackInt(      bbuf, getPrologueLength()+off); }
    /** Unpacks a 64-bit integer from the payload of the packet. */
    protected: inline int64_t       unpackPayloadLong      (int32_t off                        ) const { return VRTMath::unpackLong(     bbuf, getPrologueLength()+off); }
    /** Unpacks a 32-bit floating-point value from the payload of the packet. */
    protected: inline float         unpackPayloadFloat     (int32_t off                        ) const { return VRTMath::unpackFloat(    bbuf, getPrologueLength()+off); }
    /** Unpacks a 64-bit floating-point value from the payload of the packet. */
    protected: inline double        unpackPayloadDouble    (int32_t off                        ) const { return VRTMath::unpackDouble(   bbuf, getPrologueLength()+off); }
    /** Unpacks a <tt>boolNull</tt> value from the payload of the packet. */
    protected: inline boolNull      unpackPayloadBoolNull  (int32_t off                        ) const { return VRTMath::unpackBoolNull( bbuf, getPrologueLength()+off); }
    /** Unpacks a boolean value from the payload of the packet. */
    protected: inline bool          unpackPayloadBoolean   (int32_t off                        ) const { return VRTMath::unpackBoolean(  bbuf, getPrologueLength()+off); }
    /** Unpacks a record from the payload of the packet. */
    protected: inline void          unpackPayloadRecord    (int32_t off, Record &rec           ) const {        VRTMath::unpackRecord(   bbuf, getPrologueLength()+off, rec); }
    /** Unpacks an ASCII string from the payload of the packet. */
    protected: inline string        unpackPayloadAscii     (int32_t off,            int32_t len) const { return VRTMath::unpackAscii(    bbuf, getPrologueLength()+off, len); }
    /** Unpacks a UTF-8 string from the payload of the packet. */
    protected: inline wstring       unpackPayloadUTF8      (int32_t off,            int32_t len) const { return VRTMath::unpackUTF8(     bbuf, getPrologueLength()+off, len); }
    /** Unpacks a metadata block from the payload of the packet. */
    protected: inline MetadataBlock unpackPayloadMetadata  (int32_t off,            int32_t len) const { return VRTMath::unpackMetadata( bbuf, getPrologueLength()+off, len); }
    /** Unpacks a block of bytes from the payload of the packet. */
    protected: inline vector<char>  unpackPayloadBytes     (int32_t off,            int32_t len) const { return VRTMath::unpackBytes(    bbuf, getPrologueLength()+off, len); }
    /** Unpacks a block of bytes from the payload of the packet. */
    protected: inline void*         unpackPayloadBytes     (int32_t off, void *val, int32_t len) const { return VRTMath::unpackBytes(    bbuf, getPrologueLength()+off, val, len); }
    /** Unpacks a {@link TimeStamp} from the payload of the packet. */
    protected: inline TimeStamp     unpackPayloadTimeStamp (int32_t off, IntegerMode epoch     ) const { return VRTMath::unpackTimeStamp(bbuf, getPrologueLength()+off, epoch); }
    /** Unpacks a {@link InetAddress} from the payload of the packet. */
    protected: inline InetAddress   unpackPayloadInetAddr  (int32_t off                        ) const { return VRTMath::unpackInetAddr( bbuf, getPrologueLength()+off); }
    /** Unpacks a {@link UUID} from the payload of the packet. */
    protected: inline UUID          unpackPayloadUUID      (int32_t off                        ) const { return VRTMath::unpackUUID(     bbuf, getPrologueLength()+off); }

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
} END_NAMESPACE

#if NOT_USING_JNI
string   operator+  (string  &s, vrt::PacketType val);
ostream& operator<< (ostream &s, vrt::PacketType val);
#endif /* NOT_USING_JNI */

#endif /* BasicVRTPacket_h */

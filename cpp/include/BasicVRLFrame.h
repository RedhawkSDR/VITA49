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

#ifndef BasicVRLFrame_h
#define BasicVRLFrame_h

#include "VRTObject.h"
#include "VRTMath.h"
#include "BasicVRTPacket.h"
#include <vector>
#include "BasicDataPacket.h"
#include "BasicContextPacket.h"


using namespace std;

namespace vrt {
  /** Defines a VITA 49.1 VRL frame type. <br>
   *  <br>
   *  Note that the numeric representation (byte order) used by all VRL frames is
   *  <tt>BIG_ENDIAN</tt>.
   */
  class BasicVRLFrame : public VRTObject {

    /** The length of the header in octets.
     *  <pre>
     *    MAX_HEADER_LENGTH = 8
     *  </pre>
     */
    public: static const int32_t HEADER_LENGTH = 8;

    /** The length of the trailer in octets.
     *  <pre>
     *    MAX_TRAILER_LENGTH = 4
     *  </pre>
     */
    public: static const int32_t TRAILER_LENGTH = 4;

    /** The maximum length of the frame in octets (header + payload + trailer).
     *  Note that this is the absolute limit imposed by the VRL specification.
     *  The underlying transport protocols used (e.g. IP, UDP, TCP, etc.) may
     *  have limits lower than this.
     *  <pre>
     *    MAX_FRAME_LENGTH = 0x000FFFFF*4
     *  </pre>
     *  @see #getFrameLength()
     */
    public: static const int32_t MAX_FRAME_LENGTH = 0x000FFFFF*4; // ~4MiB

    /** The minimum length of the frame in octets (header + no payload + trailer).
     *  Note that this is the absolute limit imposed by the VRL specification and
     *  results in a near-useless frame with no payload packets.
     *  <pre>
     *    MIN_FRAME_LENGTH = HEADER_LENGTH + TRAILER_LENGTH
     *  </pre>
     *  @see #getFrameLength()
     */
    public: static const int32_t MIN_FRAME_LENGTH = HEADER_LENGTH + TRAILER_LENGTH;

    /** The maximum length of the payload in octets. Note that this is the absolute
     *  limit imposed by the VRL specification. The underlying transport protocols
     *  used (e.g. IP, UDP, TCP, etc.) may have limits lower than this.
     *  <pre>
     *    MAX_PAYLOAD_LENGTH = MAX_FRAME_LENGTH - MAX_HEADER_LENGTH - MAX_TRAILER_LENGTH
     *  </pre>
     */
    public: static const int32_t MAX_PAYLOAD_LENGTH = MAX_FRAME_LENGTH - HEADER_LENGTH - TRAILER_LENGTH;

    /** This is the 32-bit FWA (frame alignment word) marking the start of the frame.
     *  <pre>
     *    VRL_FAW = 0x56524C50 = { 'V', 'R', 'L', 'P' }
     *  </pre>
     */
    public: static const int32_t VRL_FAW = 0x56524C50;

    /** This is the special code that can be used in place of the CRC32 in cases
     *  where a CRC is not considered unnecessary, and not included.
     *  <pre>
     *    NO_CRC = 0x56454E44 = { 'V', 'E', 'N', 'D' }
     *  </pre>
     */
    public: static const int32_t NO_CRC = 0x56454E44;

    /** This is the 1st transmitted int8_t of the 32-bit FWA ({@link #VRL_FAW}). */
    public: static const int8_t VRL_FAW_0 = 0x56;
    /** This is the 2nd transmitted int8_t of the 32-bit FWA ({@link #VRL_FAW}). */
    public: static const int8_t VRL_FAW_1 = 0x52;
    /** This is the 3rd transmitted int8_t of the 32-bit FWA ({@link #VRL_FAW}). */
    public: static const int8_t VRL_FAW_2 = 0x4C;
    /** This is the 4th transmitted int8_t of the 32-bit FWA ({@link #VRL_FAW}). */
    public: static const int8_t VRL_FAW_3 = 0x50;

    /** This is the 1st transmitted int8_t of the 32-bit {@link #NO_CRC} code. */
    public: static const int8_t NO_CRC_0 = 0x56;
    /** This is the 2nd transmitted int8_t of the 32-bit {@link #NO_CRC} code. */
    public: static const int8_t NO_CRC_1 = 0x45;
    /** This is the 3rd transmitted int8_t of the 32-bit {@link #NO_CRC} code. */
    public: static const int8_t NO_CRC_2 = 0x4E;
    /** This is the 4th transmitted int8_t of the 32-bit {@link #NO_CRC} code. */
    public: static const int8_t NO_CRC_3 = 0x44;

    private: static const int32_t NO_MORE_PACKETS = -1; // Special return value from getPacketLength(..)
    private: static const int32_t INVALID_PACKET  = -2; // Special return value from getPacketLength(..)
    private: static const int32_t INVALID_FRAME   = -3; // Special return value from getPacketLength(..)

    // TODO FIXME - revert to public to work with SourceVITA and SinkVITA
    // TODO FIXME - Can those assets be updated to use lib w/ bbuf as private and still be efficient?
    //private: vector<char> bbuf;
    public: vector<char> bbuf;
    // End TODO FIXME
    private: bool         readOnly;  // Is this instance read-only?

    /** Basic destructor for the class. */
    public: ~BasicVRLFrame ();

    /** Creates a new instance with no packets in the frame. */
    public: BasicVRLFrame ();

    /** Creates a new instance with a default anticipated length that can be written to.
     *  Initially this will have no packets in the frame, but will have the underlying
     *  buffers pre-allocated as required.
     *  @param bufsize The anticipated buffer size.
     */
    public: BasicVRLFrame (int32_t bufsize);

    /** Basic copy constructor for the class.
     *  @param f The frame to copy
     */
    public: BasicVRLFrame (const BasicVRLFrame &f);

    /** Creates a new instance.
     *  @param buf      The data to initialize the frame with.
     *  @param readOnly Should the frame be treated as read-only?
     */
    public: BasicVRLFrame (const vector<char> &buf, bool readOnly=false);

    /** Creates a new instance.
     *  @param buf      The data to initialize the frame with.
     *  @param readOnly Should the frame be treated as read-only?
     */
    public: BasicVRLFrame (vector<char> *buf, bool readOnly=false);

    /** Creates a new instance.
     *  @param buf      The data to initialize the frame with.
     *  @param size     The size of buffer to read in.
     *  @param readOnly Should the frame be treated as read-only?
     */
    public: BasicVRLFrame (const vector<char> &buf, size_t size, bool readOnly=false);

    /** Creates a new instance.
     *  @param ptr      The pointer to the data to initialize the frame with.
     *  @param size     The size of buffer to read in.
     *  @param readOnly Should the frame be treated as read-only?
     */
    public: BasicVRLFrame (const void *ptr, size_t size, bool readOnly=false);

    /** Gets a free-form description of the frame. Note that the content and structure
     *  of this string is implementation dependant and may change at any time.
     *  @return A free-form string describing the frame.
     */
    public: virtual string toString () const;

    /** Checks for equality with an unknown object. Two VRL frames are considered
     *  equal if the content of the frames is bit-for-bit identical. The actual
     *  implementation classes are not considered relevant for checking equality.
     *  @param o The unknown object (can be null).
     *  @return true if they are identical, false otherwise. (This will always
     *          returns false if the object is null.)
     */
    public: bool equals (const BasicVRLFrame &o) const;

    using VRTObject::equals;
    /** Checks for equality with an unknown object. Two VRL frames are considered
     *  equal if the content of the frames is bit-for-bit identical. The actual
     *  implementation classes are not considered relevant for checking equality.
     *  @param o The unknown object (can be null).
     *  @return true if they are identical, false otherwise. (This will always
     *          returns false if the object is null.)
     */
    public: virtual bool equals (const VRTObject &o) const;

    /** Computes the hash code for a VRL frame. For performance reasons, the hash
     *  code is based on up to 32 octets at the start of the frame.
     *  @return The hash code for the object.
     */
    public: int32_t hashCode () const;

    /** Checks to see if the frame is valid. This checks the overall structure of
     *  the frame and verifies the CRC (if specified). It also checks to see that
     *  the length of the frame as reported in the frame header is equal to the
     *  length of the header and trailer plus the length of each packet in the
     *  trailer. For the purposes of the packet length check, only the length of a
     *  packet is considered, no consideration is made to the validity of the packet.
     *  @return true if it is valid, false otherwise.
     */
    public: inline bool isFrameValid () const {
      return isNull(getFrameValid(true,-1));
    }

    /** Checks to see if the frame is valid. This method adds an extra length
     *  check onto the basic checks performed by {@link #isFrameValid()}. The most
     *  common use for this method is to verify that a frame read from a socket
     *  matches the length expected.
     *  @param length Verifies that the length of the frame matches this value.
     *  @return true if it is valid, false otherwise.
     */
    public: inline bool isFrameValid (int32_t length) const {
      return isNull(getFrameValid(true,length));
    }

    /** Checks to see if the frame is valid. This is identical to <tt>isFrameValid()</tt> except
     *  that a description of the error is returned.
     *  @param strict Should strict checking be done, or just enough to verify that things are OK for
     *                reading. No frame failing strict checking should ever be sent out, but it may
     *                be useful to disable strict checking when reading in a frame.
     *  @return Empty string if it is valid, a description of the error otherwise.
     */
    public: inline string getFrameValid (bool strict) const {
      return getFrameValid(strict,-1);
    }

    /** Checks to see if the frame is valid. This is identical to <tt>isFrameValid(..)</tt>
     *  except that a description of the error is returned.
     *  @param strict Should strict checking be done, or just enough to verify that things are OK for
     *                reading. No frame failing strict checking should ever be sent out, but it may
     *                be useful to disable strict checking when reading in a frame.
     *  @param length Verifies that the length of the frame matches this value.
     *  @return Empty string if it is valid, a description of the error otherwise.
     */
    public: string getFrameValid (bool strict, int32_t length) const;

    /** Checks to see if the CRC for the frame is valid. This will always return
     *  true when the CRC is set to the special {@link #NO_CRC} value.
     *  @return true if it is valid, false otherwise.
     */
    public: bool isCRCValid () const;

    /** This will compute the CRC value for the frame and set it in the CRC field.
     *  All successful calls to one of the set methods on this packet will result
     *  in the CRC field being set, but that usually means that the CRC was merely
     *  set to the special {@link #NO_CRC} value. This method must be called on the
     *  packet immediately before transmission (i.e. after the frame counter and
     *  frame length have been updated) if CRC protection is desired. <br>
     *  <br>
     *  Most transmission options (UDP, UDP Multicast, TCP, etc.) provide their
     *  own CRC protection, which renders the CRC field in the VRL frame redundant
     *  and just wastes extra computational time on both the sender and receiver.
     *  As such, this method should only be called when absolutely necessary and
     *  such a call should usually be the responsibility of the frame sending
     *  routines.
     */
    public: void updateCRC ();

    /** Clears the CRC by setting it to the NO_CRC value. */
    private: void clearCRC ();

    /** Computes the CRC for the frame, but does not insert it into the frame. */
    private: int32_t computeCRC () const;

    /** Creates a copy of this frame that is independent of the original. The copy
     *  will not be read only and will not be "direct" (i.e. will not share data
     *  with the original). This method's behavior is undefined if the frame is
     *  invalid and may throw an exception in that event.
     *  @return A copy of this frame.
     */
    public: BasicVRLFrame copy () const;

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Get / Set
    //////////////////////////////////////////////////////////////////////////////////////////////////
    /** Gets the index of the next packet. This is designed to be used iteratively where the return
     *  value from one call becomes the input to the next call. A return value less than zero indicates
     *  the loop should end.
     *  @param index The index of the current packet or FIRST_PACKET for the first one.
     *  @return The index of the next packet or NO_MORE_PACKETS if done. Returns INVALID_PACKET or
     *          INVALID_FRAME on error. (Note that the return value will never be zero.)
     */
    private: inline static int32_t getPacketEnd (const vector<char> &buffer, int32_t off) {
      int32_t max = getFrameLength(buffer, 0) - TRAILER_LENGTH;
      if (off <    0) return off;
      if (off == max) return NO_MORE_PACKETS;
      if (off >  max) return INVALID_FRAME;

      int32_t len = BasicVRTPacket::getPacketLength(buffer, off);
      return ((len > max) || (len < 4))? INVALID_PACKET : off+len;
    }
    private: int32_t getPacketEnd (int32_t index) const;

    /** Gets the number of packets contained in this frame. This will be equal to
     *  <tt>getVRTPackets().size()</tt> though will typically be implemented in a
     *  more efficient manner.
     */
    public: int32_t getPacketCount () const;

    /** <b>Internal Use Only:</b> Gets all of the VRT packets contained in the VRL frame.
     *  @param buffer The frame buffer.
     *  @param len    The number of valid octets in the buffer.
     *  @return The packets (which will be writable copies of what is in the frame) or
     *          NULL on error.
     */
    public: static vector<vector<char>*> *getVRTPackets (const vector<char> &buffer, int32_t len);

    /** <b>Internal Use Only:</b> Gets pointers to the start of all of the VRT
     *  packets contained in the VRL frame.
     *  @return The pointers.
     */
    public: vector<void*> getVRTPacketsRW ();

    /** Gets all of the VRT packets contained in the VRL frame.
     *  @return The packets.
     */
    public: vector<BasicVRTPacket*> getVRTPackets () const;

    /** <i>Optional functionality:</i> Sets all of the VRT packets contained in
     *  the VRL frame. Strictly based on the maximum size of a VRT packet and the
     *  maximum size of a VRL frame, a minimum of 15 packets can be included. In
     *  normal use, only one or two tend to be included at a time.
     *  @param packet The packet.
     *  @throws VRTException If this method is not supported or if the total
     *                       length of all packets exceeds the maximum that can
     *                       be included in a single frame or if any of the
     *                       packets are invalid.
     */
    public: inline void setVRTPackets (const BasicVRTPacket &packet) {
      setVRTPackets(false, MAX_FRAME_LENGTH, &packet);
    }

    /** <i>Optional functionality:</i> Sets all of the VRT packets contained in
     *  the VRL frame. Strictly based on the maximum size of a VRT packet and the
     *  maximum size of a VRL frame, a minimum of 15 packets can be included. In
     *  normal use, only one or two tend to be included at a time.
     *  @param packet The packet.
     *  @throws VRTException If this method is not supported or if the total
     *                       length of all packets exceeds the maximum that can
     *                       be included in a single frame or if any of the
     *                       packets are invalid.
     */
    public: inline void setVRTPackets (const BasicVRTPacket *packet) {
      setVRTPackets(false, MAX_FRAME_LENGTH, packet);
    }

    /** <i>Optional functionality:</i> Sets all of the VRT packets contained in
     *  the VRL frame. Of the packets given, this method will try to put as many
     *  as possible into the frame that will fit given the specified maximum frame
     *  length. If fewer fit, it will put fewer in.
     *  @param maxFrameLength The maximum frame length in octets (inclusive of
     *                        header and trailer).
     *  @param packet         The packet.
     *  @return The number of packets inserted into the frame (could be 0).
     *  @throws VRTException If this method is not supported or if <tt>maxFrameLength</tt> is
     *                       invalid or if any of the packets are invalid.
     */
    public: inline int32_t setVRTPackets (int32_t maxFrameLength, const BasicVRTPacket &packet) {
      if ((maxFrameLength < MIN_FRAME_LENGTH) || (maxFrameLength > MAX_FRAME_LENGTH)) {
        throw VRTException("Illegal max frame length given (%d)", maxFrameLength);
      }
      return setVRTPackets(true, maxFrameLength, &packet);
    }

    /** <i>Optional functionality:</i> Sets all of the VRT packets contained in
     *  the VRL frame. Of the packets given, this method will try to put as many
     *  as possible into the frame that will fit given the specified maximum frame
     *  length. If fewer fit, it will put fewer in.
     *  @param maxFrameLength The maximum frame length in octets (inclusive of
     *                        header and trailer).
     *  @param packet         The packet.
     *  @return The number of packets inserted into the frame (could be 0).
     *  @throws VRTException If this method is not supported or if <tt>maxFrameLength</tt> is
     *                       invalid or if any of the packets are invalid.
     */
    public: inline int32_t setVRTPackets (int32_t maxFrameLength, const BasicVRTPacket *packet) {
      if ((maxFrameLength < MIN_FRAME_LENGTH) || (maxFrameLength > MAX_FRAME_LENGTH)) {
        throw VRTException("Illegal max frame length given (%d)", maxFrameLength);
      }
      return setVRTPackets(true, maxFrameLength, packet);
    }

    /** <i>Optional functionality:</i> Sets all of the VRT packets contained in
     *  the VRL frame. Strictly based on the maximum size of a VRT packet and the
     *  maximum size of a VRL frame, a minimum of 15 packets can be included. In
     *  normal use, only one or two tend to be included at a time.
     *  @param packets The packets.
     *  @throws VRTException If this method is not supported or if the total
     *                       length of all packets exceeds the maximum that can
     *                       be included in a single frame or if any of the
     *                       packets are invalid.
     */
    public: inline void setVRTPackets (const vector<BasicVRTPacket> &packets) {
      setVRTPackets(false, MAX_FRAME_LENGTH, &packets, NULL);
    }

    /** <i>Optional functionality:</i> Sets all of the VRT packets contained in
     *  the VRL frame. Strictly based on the maximum size of a VRT packet and the
     *  maximum size of a VRL frame, a minimum of 15 packets can be included. In
     *  normal use, only one or two tend to be included at a time.
     *  @param packets The packets.
     *  @throws VRTException If this method is not supported or if the total
     *                       length of all packets exceeds the maximum that can
     *                       be included in a single frame or if any of the
     *                       packets are invalid.
     */
    public: inline void setVRTPackets (const vector<BasicVRTPacket*> &packets) {
      setVRTPackets(false, MAX_FRAME_LENGTH, NULL, &packets);
    }

    /** <i>Optional functionality:</i> Sets all of the VRT packets contained in
     *  the VRL frame. Of the packets given, this method will try to put as many
     *  as possible into the frame that will fit given the specified maximum frame
     *  length. If fewer fit, it will put fewer in.
     *  @param maxFrameLength The maximum frame length in octets (inclusive of
     *                        header and trailer).
     *  @param packets        The packets.
     *  @return The number of packets inserted into the frame (could be 0).
     *  @throws VRTException If this method is not supported or if <tt>maxFrameLength</tt> is
     *                       invalid or if any of the packets are invalid.
     */
    public: inline int32_t setVRTPackets (int32_t maxFrameLength, const vector<BasicVRTPacket> &packets) {
      if ((maxFrameLength < MIN_FRAME_LENGTH) || (maxFrameLength > MAX_FRAME_LENGTH)) {
        throw VRTException("Illegal max frame length given (%d)", maxFrameLength);
      }
      return setVRTPackets(true, maxFrameLength, &packets, NULL);
    }

    /** <i>Optional functionality:</i> Sets all of the VRT packets contained in
     *  the VRL frame. Of the packets given, this method will try to put as many
     *  as possible into the frame that will fit given the specified maximum frame
     *  length. If fewer fit, it will put fewer in.
     *  @param maxFrameLength The maximum frame length in octets (inclusive of
     *                        header and trailer).
     *  @param packets        The packets.
     *  @return The number of packets inserted into the frame (could be 0).
     *  @throws VRTException If this method is not supported or if <tt>maxFrameLength</tt> is
     *                       invalid or if any of the packets are invalid.
     */
    public: inline int32_t setVRTPackets (int32_t maxFrameLength, const vector<BasicVRTPacket*> &packets) {
      if ((maxFrameLength < MIN_FRAME_LENGTH) || (maxFrameLength > MAX_FRAME_LENGTH)) {
        throw VRTException("Illegal max frame length given (%d)", maxFrameLength);
      }
      return setVRTPackets(true, maxFrameLength, NULL, &packets);
    }

    private: int32_t setVRTPackets (bool fit, int32_t maxFrameLength,
                                    const BasicVRTPacket *p);

    private: int32_t setVRTPackets (bool fit, int32_t maxFrameLength,
                                    const vector<BasicVRTPacket>  *packets,
                                    const vector<BasicVRTPacket*> *packetPointers);

    /** Gets the value of the 12-bit frame counter.
     *  @return The value of the frame counter.
     */
    public: int32_t getFrameCount () const;

    /** <i>Optional functionality:</i> Sets the value of the 12-bit frame counter.
     *  @param count The frame count.
     *  @throws VRTException If this method is not supported, if the count is
     *                       less than 0 or greater than 0x00000FFF.
     */
    public: void setFrameCount (int32_t count);

    /** Gets the total length of the frame in octets including the header and trailer.
     *  @return The total length of the frame in octets.
     */
    public: inline int32_t getFrameLength () const {
      return getFrameLength(bbuf, 0);
    }

    /** <b>Internal Use Only:</b> Get VRL frame length using a buffer input. */
    public: inline static int32_t getFrameLength (const vector<char> &buf, int32_t off) {
      return getFrameLength(&buf[0], off);
    }

    /** <b>Internal Use Only:</b> Get VRL frame length using a buffer input. */
    public: inline static int32_t getFrameLength (const void *ptr, int32_t off) {
      const char *buf = (const char*)ptr;
      return (0x000FFFFF & VRTMath::unpackInt(buf, off+4)) << 2; // <<2 to convert words to octets
    }

    /** <b>Internal Use Only:</b> Does the given buffer contain a VRL frame?
     *  @param buf The buffer to check. Note that the buffer can not be null and
     *             must have at least 4 octets following the offset given.
     *  @param off The offset into the buffer.
     *  @return true if it is a VRL frame, false otherwise.
     */
    public: inline static bool isVRL (const vector<char> &buf, int32_t off) {
      return isVRL(&buf[0], off);
    }

    /** <b>Internal Use Only:</b> Does the given buffer contain a VRL frame?
     *  @param ptr The buffer to check. Note that the buffer can not be null and
     *             must have at least 4 octets following the offset given.
     *  @param off The offset into the buffer.
     *  @return true if it is a VRL frame, false otherwise.
     */
    public: inline static bool isVRL (const void *ptr, int32_t off) {
      const char *buf = (const char*)ptr;
      return (buf[ off ] == VRL_FAW_0) && (buf[off+1] == VRL_FAW_1)
          && (buf[off+2] == VRL_FAW_2) && (buf[off+3] == VRL_FAW_3);
    }

    /** <i>Optional functionality:</i> Sets the total length of the frame in octets
     *  including the header and trailer. <br>
     *  <br>
     *  In general the frame length should not be set directly; rather the setting
     *  of the packets in the frame should be used as the mechanism for setting
     *  the frame length.
     *  @param length The total length of the frame in octets.
     *  @throws VRTException If this method is not supported, if the length is
     *                       less than 24 octets (the minimum length for a VRL
     *                       frame) and/or not a multiple of 4-octets.
     */
    public: void setFrameLength (int32_t length);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Read / Write
    //////////////////////////////////////////////////////////////////////////////////////////////////

    /** Gets a pointer to the frame's buffer. */
    public: void* getFramePointer ();

    /** <b>Internal Use Only:</b> Does a buffer swap. */
    public: inline void swap (vector<char> *buffer) {
      if (readOnly) throw VRTException("Frame is read-only");
      bbuf.swap(*buffer);
    }
  };

//  /** tbd */
//  class BasicVRLFrame_PacketIterator : public VRTObject, public iterator<input_iterator_tag,BasicVRTPacket> {
//    private: BasicVRLFrame *frame;
//    private: int32_t        start; // start of next packet (i.e. current offset)
//    private: int32_t        end;   // end   of next packet (i.e. next    offset)
//
//    /** Default constructor. */
//    public: BasicVRLFrame_PacketIterator ();
//
//    /** Copy constructor. */
//    public: BasicVRLFrame_PacketIterator (BasicVRLFrame_PacketIterator &pi);
//
//    /** Internal Use Only */
//    public: BasicVRLFrame_PacketIterator (BasicVRLFrame *frame, bool begin);
//
//    public: virtual string toString () const;
//
//    public: virtual bool equals (const VRTObject &pi) const;
//
//    /** Iterator ++ operator (prefix). */
//    public: BasicVRLFrame_PacketIterator& operator++ ();
//
//    /** Iterator ++ operator (postfix). */
//    public: BasicVRLFrame_PacketIterator operator++ (int i);
//
//    /** Iterator * operator. */
//    public: BasicVRTPacket& operator* ();
//  };
} END_NAMESPACE
#endif /* BasicVRLFrame_h */

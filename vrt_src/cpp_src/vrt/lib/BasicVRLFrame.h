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
  /**  A basic but full-featured implementation of {@link BasicVRLFrame}.
   *
   *  @author         
   */
  class BasicVRLFrame : public VRTObject {

    /**  The length of the header in bytes.
     *  <pre>
     *    MAX_HEADER_LENGTH = 8
     *  </pre>
     */
    public: static const int32_t HEADER_LENGTH = 8;

    /**  The length of the trailer in bytes.
     *  <pre>
     *    MAX_TRAILER_LENGTH = 4
     *  </pre>
     */
    public: static const int32_t TRAILER_LENGTH = 4;
    
    /**  The maximum length of the frame in bytes (header + payload + trailer). Note that this
     *  is the absolute limit imposed by the VRL specification. The underlying transport protocols
     *  used (e.g. IP, UDP, TCP, etc.) have limits lower than this.
     *  <pre>
     *    MAX_FRAME_LENGTH = 0x000FFFFF*4
     *  </pre>
     *  @see #getFrameLength()
     */
    public: static const int32_t MAX_FRAME_LENGTH = 0x000FFFFF*4; // ~4MiB

    /**  The minimum length of the frame in bytes (header + no payload + trailer). Note that this
     *  is the absolute limit imposed by the VRL specification and results in a near-useless frame
     *  with no payload packets.
     *  <pre>
     *    MIN_FRAME_LENGTH = HEADER_LENGTH + TRAILER_LENGTH
     *  </pre>
     *  @see #getFrameLength()
     */
    public: static const int32_t MIN_FRAME_LENGTH = HEADER_LENGTH + TRAILER_LENGTH;

    /**  The maximum length of the payload in bytes. Note that the underlying transport protocols
     *  used (e.g. IP, UDP, TCP, etc.) impose limits lower than this.
     *  <pre>
     *    MAX_PAYLOAD_LENGTH = MAX_FRAME_LENGTH - MAX_HEADER_LENGTH - MAX_TRAILER_LENGTH
     *  </pre>
     */
    public: static const int32_t MAX_PAYLOAD_LENGTH = MAX_FRAME_LENGTH - HEADER_LENGTH - TRAILER_LENGTH;

    /**  This is the 32-bit FWA (frame alignment word) marking the start of a VRLP frame.
     *  <pre>
     *    VRL_FAW = 0x56524C50 = { 'V', 'R', 'L', 'P' }
     *  </pre>
     */
    public: static const int32_t VRL_FAW = 0x56524C50;

    /**  This is the special code that can be used in place of the CRC32 in cases where
     *  a CRC is not considered unnecessary, and not included.
     *  <pre>
     *    NO_CRC = 0x56454E44 = { 'V', 'E', 'N', 'D' }
     *  </pre>
     */
    public: static const int32_t NO_CRC = 0x56454E44;

    /**  This is the 1st transmitted int8_t of the 32-bit FWA ({@link #VRL_FAW}). */
    public: static const int8_t VRL_FAW_0 = 0x56;
    /**  This is the 2nd transmitted int8_t of the 32-bit FWA ({@link #VRL_FAW}). */
    public: static const int8_t VRL_FAW_1 = 0x52;
    /**  This is the 3rd transmitted int8_t of the 32-bit FWA ({@link #VRL_FAW}). */
    public: static const int8_t VRL_FAW_2 = 0x4C;
    /**  This is the 4th transmitted int8_t of the 32-bit FWA ({@link #VRL_FAW}). */
    public: static const int8_t VRL_FAW_3 = 0x50;

    /**  This is the 1st transmitted int8_t of the 32-bit {@link #NO_CRC} code. */
    public: static const int8_t NO_CRC_0 = 0x56;
    /**  This is the 2nd transmitted int8_t of the 32-bit {@link #NO_CRC} code. */
    public: static const int8_t NO_CRC_1 = 0x45;
    /**  This is the 3rd transmitted int8_t of the 32-bit {@link #NO_CRC} code. */
    public: static const int8_t NO_CRC_2 = 0x4E;
    /**  This is the 4th transmitted int8_t of the 32-bit {@link #NO_CRC} code. */
    public: static const int8_t NO_CRC_3 = 0x44;

    private: static const int32_t NO_MORE_PACKETS = -1; // Special return value from getPacketLength(..)
    private: static const int32_t INVALID_PACKET  = -2; // Special return value from getPacketLength(..)
    private: static const int32_t INVALID_FRAME   = -3; // Special return value from getPacketLength(..)


    private: vector<char> bbuf;
    private: bool         readOnly;  // Is this instance read-only?

    /**  Basic destructor for the class. */
    public: ~BasicVRLFrame ();

    /**  Creates a new instance with no packets in the frame. */
    public: BasicVRLFrame ();

    /**  Basic copy constructor for the class.
     *  @param f The frame to copy
     */
    public: BasicVRLFrame (const int length);
    public: BasicVRLFrame (const BasicVRLFrame &f);

    /**  Creates a new instance.
     *  @param buf      The data to initialize the frame with.
     *  @param readOnly Should the frame be treated as read-only?
     */
    public: BasicVRLFrame (const vector<char> &buf, bool readOnly=false);

    public: BasicVRLFrame (vector<char> *buf, bool readOnly=false);

    public: BasicVRLFrame (const vector<char> &buf, ssize_t size, bool readOnly=false);

    /**  Gets a free-form description of the frame. Note that the content and structure of this
     *  string is implementation dependant and may change at any time.
     *  @return A free-form string describing the packet.
     */
    public: virtual string toString () const;

    /**  Checks for equality with an unknown object. Two VRL frames are considered equal if the
     *  content of the frames is bit-for-bit identical. The actual implementation classes are not
     *  considered relevent for checking equality.
     *  @param f The unknown object.
     *  @return true if they are identical, false otherwise.
     */
    public: bool equals (const BasicVRLFrame &f) const;

    public: virtual bool equals (const VRTObject &o) const;

    /**  Computes the hash code for a VRT packet. For performance reasons, the hash code is based
     *  on up to 32 bytes at the start of the frame.
     *  @return The hash code for the object.
     */
    public: int32_t hashCode () const;

    /**  The very basic frame validity checks. */
    private: bool isFrameValid0 () const;
    
    /**  Checks to see if the frame is valid. This checks the overall structure of the frame and
     *  verifies the checksum (if specified). It also checks to see that the length of the frame as
     *  reported in the frame header is equal to the length of the header and trailer plus the length
     *  of each packet in the trailer. For the purposes of the packet length check, only the length
     *  of a packet is considered, no consideration is made to the validity of the packet.
     *  @return true if it is valid, false otherwise.
     */
    public: bool isFrameValid () const;

    /**  Checks to see if the frame is valid. This method adds an extra length check onto the basic
     *  checks performed by {@link #isFrameValid()}. The most common use for this method is to
     *  verify that a packet read from a socket matches the length expected.
     *  @param length Verifies that the length of the packet matches this value.
     *  @return true if it is valid, false otherwise.
     */
    public: bool isFrameValid (int32_t length) const;

    /**  Checks to see if the CRC for the frame is valid. This will always return true when the CRC
     *  is set to the special {@link #NO_CRC} value.
     *  @return true if it is valid, false otherwise.
     */
    public: bool isCRCValid () const;

    /**  This will compute the CRC value for the packet and set it in the CRC field. All successful
     *  calls to one of the set methods on this packet will result in the CRC field being updated,
     *  but that usually means that the CRC was merely set to the special {@link #NO_CRC} value. This
     *  method must be called on the packet immediately before transmission (i.e. after the frame
     *  counter and frame length have been updated) if CRC protection is desired. <br>
     *  <br>
     *   Most transmission options (UDP, UDP Multicast, TCP, etc.) provide their own CRC protection,
     *  which renders the CRC field in the VRL frame redundant and just wastes extra computational
     *  time on both the sender and receiver. As such, this method should only be called when
     *  absolutely necessary and such a call should usually be the responsibility of the frame
     *  sending routines.
     */
    public: void updateCRC ();

    /**  Clears the CRC by setting it to the NO_CRC value. */
    private: void clearCRC ();

    /**  Computes the CRC for the frame, but does not insert it into the frame. */
    private: int32_t computeCRC () const;

    /**  Creates a copy of this frame that is independant of the origional. The copy will not be
     *  read only and will not be "direct" (i.e. will not share data with the origional). This
     *  method's behavior is undefined if the packet is invalid and may throw an exception in that
     *  event.
     *  @return A copy of this packet.
     */
    public: BasicVRLFrame copy () const;

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Get / Set
    //////////////////////////////////////////////////////////////////////////////////////////////////
    /**  Gets the index of the next packet. This is designed to be used iteratively where the return
        value from one call becomes the input to the next call. A return value less than zero indicates
        the loop should end.
        @param index The index of the current packet or FIRST_PACKET for the first one.
        @return The index of the next packet or NO_MORE_PACKETS if done. Returns INVALID_PACKET or
                INVALID_FRAME on error. (Note that the return value will never be zero.)
     */
    private: inline static int32_t getPacketEnd (const vector<char> &buffer, int32_t off) {
      int32_t max = getFrameLength(buffer, 0) - TRAILER_LENGTH;
      if (off <    0) return off;
      if (off == max) return NO_MORE_PACKETS;
      if (off >  max) return INVALID_FRAME;

      int32_t len = ((0xFF & buffer[off+2]) << 10) | ((0xFF & buffer[off+3]) << 2);
      return ((len > max) || (len < 4))? INVALID_PACKET : off+len;
    }
    private: int32_t getPacketEnd (int32_t index) const;

    /** Gets the number of packets contained in this frame. This will be equal to
     *  <tt>getVRTPackets().size()</tt> though will typically be implemented in a much
     *  more efficient fashion.
     */
    public: int32_t getPacketCount () const;


    public: inline static vector<BasicVRTPacket*> getVRTPackets (const vector<char> &buffer, ssize_t length, bool readOnly) {
      vector<BasicVRTPacket*> list;
      int32_t start = HEADER_LENGTH;
      int32_t end   = getPacketEnd(buffer, start);
      while (end > 0) {
        if ((buffer[start] & 0xC0) == 0) {
          list.push_back(new BasicDataPacket(buffer, start, end, readOnly));
        }
        else {
          list.push_back(new BasicContextPacket(buffer, start, end, readOnly));
        }
        start = end;
        end   = getPacketEnd(buffer, start);
      }
      if (end != NO_MORE_PACKETS) {
        for (size_t i=0; i<list.size(); i++) {
          delete list[i];
        }
        return vector<BasicVRTPacket*>(0); // error condition indicated
      }
      return list;
    }

    /**  Gets all of the VRT packets contained in the VRL frame.
     *  @return The packets.
     */
    public: vector<BasicVRTPacket*> getVRTPackets () const;

    /**  <i>Optional functionality:</i> Sets all of the VRT packets contained in the VRL frame.
     *  Strictly based on the maximum size of a VRT packet and the maximum size of a VRL frame, a
     *  minimum of 15 packets can be included. In normal use, only one or two tend to be included
     *  at a time.
     *  @param packets The packets.
     *  @throws VRTException If this method is not supported or if the total length of all packets
     *                       exceeds the maximum that can be included in a single frame or if any
     *                       of the packets are invalid.
     */
    public: inline void setVRTPackets (const vector<BasicVRTPacket> &packets) {
      setVRTPackets(false, MAX_FRAME_LENGTH, packets);
    }

    private: int32_t setVRTPacket (int32_t maxFrameLength, const BasicVRTPacket *packet);

    public: inline void setVRTPacket (const BasicVRTPacket *packet){
    	setVRTPacket(MAX_FRAME_LENGTH, packet);
    }

    /**  <i>Optional functionality:</i> Sets all of the VRT packets contained in the VRL frame. Of
     *  the packets given, this method will try to put as many as possible into the frame that will
     *  fit given the specified maximum frame length. If fewer fit, it will put fewer in.
     *  @param maxFrameLength The maximum frame length in bytes (inclusive of header and trailer).
     *  @param packets        The packets.
     *  @return The number of packets inserted into the frame (could be 0).
     *  @throws VRTException If this method is not supported or if <tt>maxFrameLength</tt> is
     *                       invalid or if any of the packets are invalid.
     */

    public: inline int32_t setVRTPackets (int32_t maxFrameLength, const vector<BasicVRTPacket> &packets) {
      if ((maxFrameLength < MIN_FRAME_LENGTH) || (maxFrameLength > MAX_FRAME_LENGTH)) {
        throw VRTException("Illegal max frame length given (%d)", maxFrameLength);
      }
      return setVRTPackets(true, maxFrameLength, packets);
    }



    private: int32_t setVRTPackets (bool fit, int32_t maxFrameLength, const vector<BasicVRTPacket> &packets);

    /**  Gets the value of the 12-bit frame counter.
     *  @return The value of the frame counter.
     */
    public: int32_t getFrameCount () const;

    /**  <i>Optional functionality:</i> Sets the value of the 12-bit frame counter.
     *  @param count The frame count.
     *  @throws UnsupportedOperationException If this method is not supported
     *  @throws IllegalArgumentException If the count is less than 0 or greater than 0x00000FFF.
     */
    public: void setFrameCount (int32_t count);

    /**  Gets the total length of the frame in bytes including the header and trailer.
     *  @return The total length of the frame in bytes.
     */
    public: int32_t getFrameLength () const;


    private: inline static int32_t getFrameLength (const vector<char> &buf, int32_t off) {
      return (0x000FFFFF & VRTMath::unpackInt(buf, off+4, BIG_ENDIAN)) << 2; // <<2 to convert words to bytes
    }

    /**  <i>Optional functionality:</i> Sets the total length of the frame in bytes including the header and trailer.
     *  @param length The total length of the frame in bytes.
     *  @throws UnsupportedOperationException If this method is not supported
     *  @throws IllegalArgumentException If the length is less than 24 bytes (the minimum length for
                                         a VRL frame) and/or not a multiple of 4-bytes.
     */
    public: void setFrameLength (int32_t length);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Read / Write
    //////////////////////////////////////////////////////////////////////////////////////////////////

    /**  Gets a pointer to the frame's buffer. */
    public: void* getFramePointer ();
  };
  
//  /**  A basic but full-featured implementation of {@link BasicVRLFrame}.
//   *
//   *  @author         
//   */
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
};
#endif /* BasicVRLFrame_h */

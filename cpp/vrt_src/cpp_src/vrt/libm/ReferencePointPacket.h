/*
 * This file is protected by Copyright. Please refer to the COPYRIGHT file
 * distributed with this source distribution.
 *
 * This file is part of REDHAWK VITA49Libraries.
 *
 * REDHAWK VITA49Libraries is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * REDHAWK VITA49Libraries is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */
//
//  This class was auto-generated, do not modify.
//
#ifndef _ReferencePointPacket_h
#define _ReferencePointPacket_h

#include "BasicVRTPacket.h"
#include "BasicDataPacket.h"
#include "BasicContextPacket.h"
#include <cstring>
#include <vector>

using namespace std;
using namespace vrt;

namespace vrt {
  
  //======================================================================
  // PACKET TYPE
  //======================================================================
  /** <b>A.6.\ Reference Point Packet</b>. 
   *  
   *          The "reference point packet" provides a free-form description of a
   *          reference point. This class of packet plays a major role in large-scale,
   *          run-time-configurable, installations where the exact processing path is
   *          not pre-determined.
   *        
   */
  class ReferencePointPacket : public BasicVRTPacket {
    /** The packet type (<tt>PacketType_ExtContext</tt>). */
    public: static const PacketType PACKET_TYPE;
    
    /** The packet length (<tt>-40</tt>) excluding the header and trailer (if applicable).
        A value of -40 indicates that the length is variable with a minimum length of 40 octets.
     */
    public: static const int32_t PACKET_LENGTH;
    
    /** The class identifier in numeric form (<tt>0xFFFFFA20110006</tt>). */
    public: static const int64_t CLASS_IDENTIFIER;
    
    /** The class identifier in string form (<tt>FF-FF-FA:2011.0006</tt>). */
    public: static const string CLASS_ID;
    
    //======================================================================
    // CONSTRUCTORS
    //======================================================================
    /** Creates a new instance ready for the fields to be set. The stream ID
     *  and timecode values will be initialized to 0.
     */
    public: ReferencePointPacket ();
    
    /** Creates a new instance from the given packet.
     *  @param p    The packet.
     *  @throws VRTException If the packet given does not match this type.
     */
    public: ReferencePointPacket (const BasicVRTPacket &p);
    
    /** Internal constructor, for use by subclasses.
     *  @param p                The packet (already initialized).
     *  @param type             The expected packet type.
     *  @param classID          The expected classID.
     *  @param minPayloadLength The minimum payload length.
     *  @param maxPayloadLength The maximum payload length (&lt;0 if not pre-defined).
     *  @throws IllegalArgumentException If packet type or classID of the packet are invalid, or if
     *                                   the payload length is invalid.
     */
    protected: ReferencePointPacket (const BasicVRTPacket &p, PacketType type,
                            int64_t classID, int32_t minPayloadLength, int32_t maxPayloadLength);
    
    /** Creates a new instance accessing the given data buffer.
     *  @param bbuf     The data buffer to use.
     *  @param readOnly Should users of this instance be able to modify the underlying data buffer?
     */
    public: ReferencePointPacket (const vector<char> &bbuf, bool readOnly);
    
    //======================================================================
    // UTILITY METHODS
    //======================================================================
    
    public:    virtual string    getPacketValid (bool strict, int32_t length) const;
    protected: virtual void      toStringStream (ostringstream &str) const;
    public:    virtual int32_t   getFieldCount () const;
    public:    virtual string    getFieldName (int32_t id) const;
    public:    virtual ValueType getFieldType (int32_t id) const;
    public:    virtual Value*    getField (int32_t id) const;
    public:    virtual void      setField (int32_t id, const Value* val);
    //======================================================================
    // GET/SET METHODS
    //======================================================================
    
    /** Get the reference point identifier. 
     *  @return The value read from the packet.
     */
    public: inline int32_t getRefPointID () const {
      return unpackPayloadInt(0);
    }
    
    /** Set the reference point identifier. 
     *  @param val The new value to write to the packet.
     */
    public: inline void setRefPointID (int32_t val) {
      packPayloadInt(0, val);
    }
    
    /** Get A free-form name of the reference point. 
     *  <br> Size: <tt>32</tt>
     *  @return The value read from the packet.
     */
    public: inline string getName () const {
      return unpackPayloadAscii(8, 32);
    }
    
    /** Set A free-form name of the reference point. 
     *  <br> Size: <tt>32</tt>
     *  @param val The new value to write to the packet.
     *             Any non-ASCII characters in the string (numeric value &gt;0x7F) or ASCII
     *             control characters (numeric value &lt;0x20, 0x7F) will be replaced with '?'.
     *             If the string is longer than the size allotted, it will be
     *             truncated without warning. 
     */
    public: inline void setName (string val) {
      packPayloadAscii(8, val, 32);
    }
    
    /** Get A free-form description of the reference point. 
     *  <br> Size: <tt>variable</tt>
     *  @return The value read from the packet.
     */
    public: inline wstring getDescription () const {
      return unpackPayloadUTF8(40, getPayloadLength()-40);
    }
    
    /** Set A free-form description of the reference point. 
     *  <br> Size: <tt>variable</tt>
     *  @param val The new value to write to the packet.
     */
    public: inline void setDescription (wstring val) {
    int32_t limit  = MAX_PAYLOAD_LENGTH-40;
    int32_t length = vrt::VRTMath::lengthUTF8(val);
    int32_t len    = (length > limit)? ((limit  + 3) & 0x7FFFFFFC)
                                     : ((length + 3) & 0x7FFFFFFC); // length to next multiple of 4
      setPayloadLength(40 + len);
      packPayloadUTF8(40, val, len);
    }
    
    /** Set A free-form description of the reference point. 
     *  <br> Size: <tt>variable</tt>
     *  @param val The new value to write to the packet.
     */
    public: inline void setDescription (string val) {
    int32_t limit  = MAX_PAYLOAD_LENGTH-40;
    int32_t length = vrt::VRTMath::lengthUTF8(val);
    int32_t len    = (length > limit)? ((limit  + 3) & 0x7FFFFFFC)
                                     : ((length + 3) & 0x7FFFFFFC); // length to next multiple of 4
      setPayloadLength(40 + len);
      packPayloadUTF8(40, val, len);
    }
  };
};
#endif /* _ReferencePointPacket_h */


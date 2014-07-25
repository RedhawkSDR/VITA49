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
#ifndef _StandardContextPacket_h
#define _StandardContextPacket_h

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
  /** <b>A.3.\ Standard Context Packet</b>. 
   *  
   *          The "standard context packet" is simply an IF Context packet that can be
   *          used in any generic IF Context Stream.
   *        
   */
  class StandardContextPacket : public BasicContextPacket {
    /** The packet type (<tt>PacketType_Context</tt>). */
    public: static const PacketType PACKET_TYPE;
    
    /** The packet length (<tt>-4</tt>) excluding the header and trailer (if applicable).
        A value of -4 indicates that the length is variable with a minimum length of 4 octets.
     */
    public: static const int32_t PACKET_LENGTH;
    
    /** The class identifier in numeric form (<tt>0xFFFFFA20110003</tt>). */
    public: static const int64_t CLASS_IDENTIFIER;
    
    /** The class identifier in string form (<tt>FF-FF-FA:2011.0003</tt>). */
    public: static const string CLASS_ID;
    
    //======================================================================
    // CONSTRUCTORS
    //======================================================================
    /** Creates a new instance ready for the fields to be set. The stream ID
     *  and timecode values will be initialized to 0.
     */
    public: StandardContextPacket ();
    
    /** Creates a new instance from the given packet.
     *  @param p    The packet.
     *  @throws VRTException If the packet given does not match this type.
     */
    public: StandardContextPacket (const BasicVRTPacket &p);
    
    /** Internal constructor, for use by subclasses.
     *  @param p                The packet (already initialized).
     *  @param type             The expected packet type.
     *  @param classID          The expected classID.
     *  @param minPayloadLength The minimum payload length.
     *  @param maxPayloadLength The maximum payload length (&lt;0 if not pre-defined).
     *  @throws IllegalArgumentException If packet type or classID of the packet are invalid, or if
     *                                   the payload length is invalid.
     */
    protected: StandardContextPacket (const BasicVRTPacket &p, PacketType type,
                            int64_t classID, int32_t minPayloadLength, int32_t maxPayloadLength);
    
    /** Creates a new instance accessing the given data buffer.
     *  @param bbuf     The data buffer to use.
     *  @param readOnly Should users of this instance be able to modify the underlying data buffer?
     */
    public: StandardContextPacket (const vector<char> &bbuf, bool readOnly);
    
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
  };
};
#endif /* _StandardContextPacket_h */


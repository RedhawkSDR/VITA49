//
//  This class was auto-generated, do not modify.
//
#ifndef _NoDataPacket_h
#define _NoDataPacket_h

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
  /** <b>A.2.\ No Data Packet</b>. 
   *  
   *          The "no data packet" is used to indicate that no data exists in the
   *          Information Stream. Its sole purpose is to permit receivers to identify
   *          the paired IF Context Stream as is required by Section 8 of the "VITA 49
   *          Spectrum Survey Profile". The trailer is usually included for
   *          consistency and to provide the Calibrated Time Indicator. This
   *          packet <b>shall not</b> have any payload associated with it.
   *        
   */
  class NoDataPacket : public BasicDataPacket {
    /** The packet type (<tt>PacketType_UnidentifiedExtData</tt>). */
    public: static const PacketType PACKET_TYPE;
    
    /** The packet length (<tt>0</tt>) excluding the header and trailer (if applicable).
     *  A value of 0 indicates that the length is unknown.
     */
    public: static const int32_t PACKET_LENGTH;
    
    /** The class identifier in numeric form (<tt>0xFFFFFA20110002</tt>). */
    public: static const int64_t CLASS_IDENTIFIER;
    
    /** The class identifier in string form (<tt>FF-FF-FA:2011.0002</tt>). */
    public: static const string CLASS_ID;
    
    //======================================================================
    // CONSTRUCTORS
    //======================================================================
    /** Creates a new instance ready for the fields to be set. The stream ID
     *  and timecode values will be initialized to 0.
     */
    public: NoDataPacket ();
    
    /** Creates a new instance from the given packet.
     *  @param p    The packet.
     *  @throws VRTException If the packet given does not match this type.
     */
    public: NoDataPacket (const BasicVRTPacket &p);
    
    /** Internal constructor, for use by subclasses.
     *  @param p                The packet (already initialized).
     *  @param type             The expected packet type.
     *  @param classID          The expected classID.
     *  @param minPayloadLength The minimum payload length.
     *  @param maxPayloadLength The maximum payload length (&lt;0 if not pre-defined).
     *  @throws IllegalArgumentException If packet type or classID of the packet are invalid, or if
     *                                   the payload length is invalid.
     */
    protected: NoDataPacket (const BasicVRTPacket &p, PacketType type,
                            int64_t classID, int32_t minPayloadLength, int32_t maxPayloadLength);
    
    /** Creates a new instance accessing the given data buffer.
     *  @param bbuf     The data buffer to use.
     *  @param readOnly Should users of this instance be able to modify the underlying data buffer?
     */
    public: NoDataPacket (const vector<char> &bbuf, bool readOnly);
    
    //======================================================================
    // UTILITY METHODS
    //======================================================================
    
    public:    virtual string    getPacketValid (bool strict, int32_t length) const;
    protected: virtual void      toStringStream (ostringstream &str) const;
    public:    virtual int32_t   getFieldCount () const;
    public:    virtual string    getFieldName (int32_t id) const;
    public:    virtual ValueType getFieldType (int32_t id) const;
    public:    virtual Value*    getField (int32_t id) const __attribute__((warn_unused_result));
    public:    virtual void      setField (int32_t id, const Value* val);
    //======================================================================
    // GET/SET METHODS
    //======================================================================
    //======================================================================
    // TRAILER
    //======================================================================
  };
} END_NAMESPACE
#endif /* _NoDataPacket_h */


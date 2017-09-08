//
//  This class was auto-generated, do not modify.
//
#ifndef _TimestampAccuracyPacket_h
#define _TimestampAccuracyPacket_h

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
  /** <b>A.4.\ Timestamp Accuracy Packet</b>. 
   *  
   *          The "timestamp accuracy packet" provides details about the accuracy of
   *          the timing source. Use of this packet is required for all cases where
   *          the Calibrated Time Indicator is enabled and set to one by Rule 6.1.7-1
   *          of "VITA 49 Spectrum Survey Profile".<br><br>
   *  
   *          For the purpose of this packet definition, "timestamp accuracy" is defined
   *          to be the maximal distance (measured in picoseconds) between any two
   *          consecutive "time tics" (a "time tic" is a point in time that is marked
   *          by a stable time reference). In other words the "timestamp accuracy" it is
   *          equal to twice the maximum error.<br><br>
   *  
   *          If the time source is more accurate than 1 picosecond, the accuracy
   *          <b>shall</b> be reported as 1 picosecond if the time values will be
   *          rounded to the nearest 1 picosecond before insertion into the
   *          Fractional-Seconds Timestamp field and <b>shall</b> be 2 picoseconds if
   *          the value is truncated (mathematical floor operation).<br><br>
   *  
   *          Note that this reports the "accuracy" not "precision" of the timestamp.
   *          In all cases the precision is 1 picosecond as required by the Real-Time
   *          mode of the Fractional-Seconds Timestamp field in the packet header.
   *          The Fractional-Seconds Timestamp field <b>may</b> include time value
   *          that is (or appears to be) more precise than the accuracy value implies.<br><br>
   *  
   *          The <tt>TimestampAccuracy</tt> <b>shall not</b> be more than 1e12 picoseconds
   *          (1 second). If the accuracy of the time stamp is less than 1 second, the
   *          Calibrated Time Indicator shall not be set to one. It is worth noting
   *          that although a 1 second accuracy value is permissible, any value greater
   *          than 1 microsecond is generally considered "poor" (a 1 microsecond value
   *          can be obtained by using a 1 pulse-per-second time source and a 10 MHz
   *          reference).<br><br>
   *  
   *          If <b>all</b> packets in the Information Stream do not enable the
   *          Calibrated Time Indicator or otherwise set it to zero, the
   *          <tt>TimestampAccuracy</tt> value <b>may</b> contain any value between 1
   *          and 1e12 (inclusive). <i>(A typical use case for this is to set the
   *          <tt>TimestampAccuracy</tt> value based on the specifications/requirements
   *          of the timing source and to then set the Calibrated Time Indicator to
   *          zero if those specifications/requirements are not being met.)</i><br><br>
   *  
   *          The <tt>TimestampAccuracy</tt> <b>may</b> be any integer between 1 and
   *          1e12 (inclusive) and is not limited to multiples of 10 or 2.<br><br>
   *  
   *          In any Information Stream, there shall not be more than one stream of
   *          Timestamp Accuracy Packets. The stream with the Timestamp Accuracy Packets
   *          shall be considered a System Context stream applicable to all packets
   *          within the Information Stream.
   *        
   */
  class TimestampAccuracyPacket : public BasicVRTPacket {
    /** The packet type (<tt>PacketType_ExtContext</tt>). */
    public: static const PacketType PACKET_TYPE;
    
    /** The packet length (<tt>8</tt>) excluding the header and trailer (if applicable).
     *  A value of 8 indicates that the length is fixed at 8 octets.
     */
    public: static const int32_t PACKET_LENGTH;
    
    /** The class identifier in numeric form (<tt>0xFFFFFA20110004</tt>). */
    public: static const int64_t CLASS_IDENTIFIER;
    
    /** The class identifier in string form (<tt>FF-FF-FA:2011.0004</tt>). */
    public: static const string CLASS_ID;
    
    //======================================================================
    // CONSTRUCTORS
    //======================================================================
    /** Creates a new instance ready for the fields to be set. The stream ID
     *  and timecode values will be initialized to 0.
     */
    public: TimestampAccuracyPacket ();
    
    /** Creates a new instance from the given packet.
     *  @param p    The packet.
     *  @throws VRTException If the packet given does not match this type.
     */
    public: TimestampAccuracyPacket (const BasicVRTPacket &p);
    
    /** Internal constructor, for use by subclasses.
     *  @param p                The packet (already initialized).
     *  @param type             The expected packet type.
     *  @param classID          The expected classID.
     *  @param minPayloadLength The minimum payload length.
     *  @param maxPayloadLength The maximum payload length (&lt;0 if not pre-defined).
     *  @throws IllegalArgumentException If packet type or classID of the packet are invalid, or if
     *                                   the payload length is invalid.
     */
    protected: TimestampAccuracyPacket (const BasicVRTPacket &p, PacketType type,
                            int64_t classID, int32_t minPayloadLength, int32_t maxPayloadLength);
    
    /** Creates a new instance accessing the given data buffer.
     *  @param bbuf     The data buffer to use.
     *  @param readOnly Should users of this instance be able to modify the underlying data buffer?
     */
    public: TimestampAccuracyPacket (const vector<char> &bbuf, bool readOnly);
    
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
    
    /** Get the timestamp accuracy in picoseconds. 
     *  <br> Units: <i>picoseconds</i>
     *  <br> Range: <tt>[1,1e12]</tt>
     *  @return The value read from the packet.
     */
    public: inline int64_t getTimestampAccuracy () const {
      return unpackPayloadLong(0);
    }
    
    /** Set the timestamp accuracy in picoseconds. 
     *  <br> Units: <i>picoseconds</i>
     *  <br> Range: <tt>[1,1e12]</tt>
     *  @param val The new value to write to the packet.
     */
    public: inline void setTimestampAccuracy (int64_t val) {
      packPayloadLong(0, val);
    }
  };
} END_NAMESPACE
#endif /* _TimestampAccuracyPacket_h */


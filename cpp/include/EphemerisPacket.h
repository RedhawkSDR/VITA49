//
//  This class was auto-generated, do not modify.
//
#ifndef _EphemerisPacket_h
#define _EphemerisPacket_h

#include "BasicVRTPacket.h"
#include "BasicDataPacket.h"
#include "BasicContextPacket.h"
#include <cstring>
#include <vector>

using namespace std;
using namespace vrt;

namespace vrt {
  //======================================================================
  // ENUMERATIONS
  //======================================================================
  /** Fix Type Enumerated Values */
 enum FixType { 
    /** Predicted Ephemeris (9+ days before) */
    FixType_PREDICTED_EPHEMERIS_9 = -19,
    /** Predicted Ephemeris (8 days before) */
    FixType_PREDICTED_EPHEMERIS_8 = -18,
    /** Predicted Ephemeris (7 days before) */
    FixType_PREDICTED_EPHEMERIS_7 = -17,
    /** Predicted Ephemeris (6 days before) */
    FixType_PREDICTED_EPHEMERIS_6 = -16,
    /** Predicted Ephemeris (5 days before) */
    FixType_PREDICTED_EPHEMERIS_5 = -15,
    /** Predicted Ephemeris (4 days before) */
    FixType_PREDICTED_EPHEMERIS_4 = -14,
    /** Predicted Ephemeris (3 days before) */
    FixType_PREDICTED_EPHEMERIS_3 = -13,
    /** Predicted Ephemeris (2 days before) */
    FixType_PREDICTED_EPHEMERIS_2 = -12,
    /** Predicted Ephemeris (1 days before) */
    FixType_PREDICTED_EPHEMERIS_1 = -11,
    /** Predicted Ephemeris */
    FixType_PREDICTED_EPHEMERIS = -10,
    /** Predicted INS (Inertial Navigation System) */
    FixType_PREDICTED_INS = -3,
    /** Predicted GPS */
    FixType_PREDICTED_GPS = -2,
    /** Predicted */
    FixType_PREDICTED = -1,
    /** Value not specified */
    FixType_null = 0,
    /** Actual */
    FixType_ACTUAL = 1,
    /** Actual GPS */
    FixType_ACTUAL_GPS = 2,
    /** Actual INS (Inertial Navigation System) */
    FixType_ACTUAL_INS = 3,
    /** Actual Ephemeris */
    FixType_ACTUAL_EPHEMERIS = 10,
    /** Actual Ephemeris (1 days after) */
    FixType_ACTUAL_EPHEMERIS_1 = 11,
    /** Actual Ephemeris (2 days after) */
    FixType_ACTUAL_EPHEMERIS_2 = 12,
    /** Actual Ephemeris (3 days after) */
    FixType_ACTUAL_EPHEMERIS_3 = 13,
    /** Actual Ephemeris (4 days after) */
    FixType_ACTUAL_EPHEMERIS_4 = 14,
    /** Actual Ephemeris (5 days after) */
    FixType_ACTUAL_EPHEMERIS_5 = 15,
    /** Actual Ephemeris (6 days after) */
    FixType_ACTUAL_EPHEMERIS_6 = 16,
    /** Actual Ephemeris (7 days after) */
    FixType_ACTUAL_EPHEMERIS_7 = 17,
    /** Actual Ephemeris (8 days after) */
    FixType_ACTUAL_EPHEMERIS_8 = 18,
    /** Actual Ephemeris (9+ days after) */
    FixType_ACTUAL_EPHEMERIS_9 = 19
  };
  //======================================================================
  // RECORDS
  //======================================================================
    
    
  /** <b>Point Definition</b>.  <br>
   *  <br>
   *  <i>Note: Any changes made to this record will not be reflected in the
   *  packet. This maintains the CONOP that anything read from the packet can be
   *  used independent of the packet and that the packet can only be modified
   *  by calling a set method on the packet its self.</i>
   */
  class Point : public Record {
    
    
    /** The length of the record in octets (<tt>72</tt>). */
    public: static const int32_t RECORD_LENGTH;
    
    /** Basic copy constructor. */
    protected: Point (const Record &r);
    
    /** Creates a new instance ready for the fields to be set. All of the bytes
     *  that make up the record will be initialized to 0.
     */
    public: Point ();
    
    public: virtual string    toString () const;
    public: virtual int32_t   getFieldCount () const;
    public: virtual string    getFieldName (int32_t id) const;
    public: virtual ValueType getFieldType (int32_t id) const;
    public: virtual Value*    getField (int32_t id) const;
    public: virtual void      setField (int32_t id, const Value* val);
    
      
      /** Get ECEF position X. 
       *  <br> Units: <i>m</i>
       *  @return The value read from the packet.
       */
      public: inline double getPositionX () const {
        return unpackDouble(0);
      }
      
      /** Set ECEF position X. 
       *  <br> Units: <i>m</i>
       *  @param val The new value to write to the packet.
       */
      public: inline void setPositionX (double val) {
        packDouble(0, val);
      }
      
      /** Get ECEF position Y. 
       *  <br> Units: <i>m</i>
       *  @return The value read from the packet.
       */
      public: inline double getPositionY () const {
        return unpackDouble(8);
      }
      
      /** Set ECEF position Y. 
       *  <br> Units: <i>m</i>
       *  @param val The new value to write to the packet.
       */
      public: inline void setPositionY (double val) {
        packDouble(8, val);
      }
      
      /** Get ECEF position Z. 
       *  <br> Units: <i>m</i>
       *  @return The value read from the packet.
       */
      public: inline double getPositionZ () const {
        return unpackDouble(16);
      }
      
      /** Set ECEF position Z. 
       *  <br> Units: <i>m</i>
       *  @param val The new value to write to the packet.
       */
      public: inline void setPositionZ (double val) {
        packDouble(16, val);
      }
      
      /** Get ECEF velocity X. 
       *  <br> Units: <i>m/s</i>
       *  @return The value read from the packet.
       */
      public: inline double getVelocityX () const {
        return unpackDouble(24);
      }
      
      /** Set ECEF velocity X. 
       *  <br> Units: <i>m/s</i>
       *  @param val The new value to write to the packet.
       */
      public: inline void setVelocityX (double val) {
        packDouble(24, val);
      }
      
      /** Get ECEF velocity Y. 
       *  <br> Units: <i>m/s</i>
       *  @return The value read from the packet.
       */
      public: inline double getVelocityY () const {
        return unpackDouble(32);
      }
      
      /** Set ECEF velocity Y. 
       *  <br> Units: <i>m/s</i>
       *  @param val The new value to write to the packet.
       */
      public: inline void setVelocityY (double val) {
        packDouble(32, val);
      }
      
      /** Get ECEF velocity Z. 
       *  <br> Units: <i>m/s</i>
       *  @return The value read from the packet.
       */
      public: inline double getVelocityZ () const {
        return unpackDouble(40);
      }
      
      /** Set ECEF velocity Z. 
       *  <br> Units: <i>m/s</i>
       *  @param val The new value to write to the packet.
       */
      public: inline void setVelocityZ (double val) {
        packDouble(40, val);
      }
      
      /** Get ECEF acceleration X. 
       *  <br> Units: <i>m/s^2</i>
       *  @return The value read from the packet.
       */
      public: inline float getAccelerationX () const {
        return unpackFloat(48);
      }
      
      /** Set ECEF acceleration X. 
       *  <br> Units: <i>m/s^2</i>
       *  @param val The new value to write to the packet.
       */
      public: inline void setAccelerationX (float val) {
        packFloat(48, val);
      }
      
      /** Get ECEF acceleration Y. 
       *  <br> Units: <i>m/s^2</i>
       *  @return The value read from the packet.
       */
      public: inline float getAccelerationY () const {
        return unpackFloat(52);
      }
      
      /** Set ECEF acceleration Y. 
       *  <br> Units: <i>m/s^2</i>
       *  @param val The new value to write to the packet.
       */
      public: inline void setAccelerationY (float val) {
        packFloat(52, val);
      }
      
      /** Get ECEF acceleration Z. 
       *  <br> Units: <i>m/s^2</i>
       *  @return The value read from the packet.
       */
      public: inline float getAccelerationZ () const {
        return unpackFloat(56);
      }
      
      /** Set ECEF acceleration Z. 
       *  <br> Units: <i>m/s^2</i>
       *  @param val The new value to write to the packet.
       */
      public: inline void setAccelerationZ (float val) {
        packFloat(56, val);
      }
      
      /** Get attitude Alpha. 
       *  <br> Units: <i>deg</i>
       *  <br> Range: <tt>[0,360)</tt>
       *  @return The value read from the packet.
       */
      public: inline float getAttitudeAlpha () const {
        return unpackFloat(60);
      }
      
      /** Set attitude Alpha. 
       *  <br> Units: <i>deg</i>
       *  <br> Range: <tt>[0,360)</tt>
       *  @param val The new value to write to the packet.
       */
      public: inline void setAttitudeAlpha (float val) {
        packFloat(60, val);
      }
      
      /** Get attitude Beta. 
       *  <br> Units: <i>deg</i>
       *  <br> Range: <tt>[0,360)</tt>
       *  @return The value read from the packet.
       */
      public: inline float getAttitudeBeta () const {
        return unpackFloat(64);
      }
      
      /** Set attitude Beta. 
       *  <br> Units: <i>deg</i>
       *  <br> Range: <tt>[0,360)</tt>
       *  @param val The new value to write to the packet.
       */
      public: inline void setAttitudeBeta (float val) {
        packFloat(64, val);
      }
      
      /** Get attitude Phi. 
       *  <br> Units: <i>deg</i>
       *  <br> Range: <tt>[0,360)</tt>
       *  @return The value read from the packet.
       */
      public: inline float getAttitudePhi () const {
        return unpackFloat(68);
      }
      
      /** Set attitude Phi. 
       *  <br> Units: <i>deg</i>
       *  <br> Range: <tt>[0,360)</tt>
       *  @param val The new value to write to the packet.
       */
      public: inline void setAttitudePhi (float val) {
        packFloat(68, val);
      }
  };
  
  //======================================================================
  // PACKET TYPE
  //======================================================================
  /** <b>A.7.\ ECEF Ephemeris</b>. 
   *  
   *          Describes the position of a vehicle in ECEF (ECR) coordinates. This
   *          augments the standard IF Context packet which includes basic ephemeris
   *          information for a single point in time, but lacks the ability to convey
   *          a description of the actual and anticipated movements of a vehicle. A
   *          good example would be that of a ballistic projectile that lacks a GPS
   *          or INS system, but has an extremely predictable path of travel.<br><br>
   *  
   *          The ECEF and Attitude <b>shall</b> be defined using the same coordinate
   *          system described in VITA 49.0 rules 7.1.5.21-1 and 7.1.5.21-2.<br><br>
   *  
   *          If the associated IF Context packet includes ECEF ephemeris values, they
   *          <b>shall</b> be consistent with the values provided here (excepting rounding
   *          errors). Note that "consistent" differs from "equal" in that the fixes in
   *          the IF Context packet <b>may</b> be sampled at a different time interval
   *          resulting in values that are "consistent" but "unequal". Also note that
   *          the ECEF values <b>may</b> differ significantly from the GPS and/or INS
   *          values that are (optionally) included in the IF Context packet in cases
   *          where there are measurement inconsistencies between the devices used.<br><br>
   *  
   *          The enumerated Fix Type enumeration provides extra detail to processing
   *          systems that assign different "accuracy tolerances" based on the fix
   *          type. All fix types less than zero indicates a predicted/estimated value
   *          is given, fix types greater than zero indicates an actual/measured value
   *          is given. If both Predicted and Actual values are included, the Fix
   *          Types <b>shall</b> report all points as Predicted or <b>shall not</b>
   *          specify the Fix Type.<br><br>
   *  
   *          The Fix Time field specifies the time of the first fix. Where multiple
   *          fix points are provided the Fix Delta gives the delta between each pair
   *          of fixes (e.g. between points(7) and points(8)), all points must be
   *          evenly spaced. A fix at the current time will have the fix time match
   *          that of the packet; though it need not be the first point listed.<br><br>
   *  
   *          This approach allows for old fixes to be re-sent as necessary and allows
   *          use of predicted values for interpolation. In the case of orbiting
   *          spacecraft, future positions of vehicles can be predicted with some
   *          accuracy so the inclusion of points in the past and future allows
   *          consumers to interpolate the positions at any arbitrary time.<br><br>
   *  
   *          Stationary objects are identified by velocity and acceleration values of
   *          zero (but a non-zero fix type for both) and will typically have 1+ past
   *          and 1+ future points specified (with the same values). Any interpolation
   *          algorithms should always resolve to the stationary position.
   *        
   */
  class EphemerisPacket : public BasicVRTPacket {
    /** The packet type (<tt>PacketType_ExtContext</tt>). */
    public: static const PacketType PACKET_TYPE;
    
    /** The packet length (<tt>-56</tt>) excluding the header and trailer (if applicable).
        A value of -56 indicates that the length is variable with a minimum length of 56 octets.
     */
    public: static const int32_t PACKET_LENGTH;
    
    /** The class identifier in numeric form (<tt>0xFFFFFA2011ECEF</tt>). */
    public: static const int64_t CLASS_IDENTIFIER;
    
    /** The class identifier in string form (<tt>FF-FF-FA:2011.ECEF</tt>). */
    public: static const string CLASS_ID;
    
    //======================================================================
    // CONSTRUCTORS
    //======================================================================
    /** Creates a new instance ready for the fields to be set. The stream ID
     *  and timecode values will be initialized to 0.
     */
    public: EphemerisPacket ();
    
    /** Creates a new instance from the given packet.
     *  @param p    The packet.
     *  @throws VRTException If the packet given does not match this type.
     */
    public: EphemerisPacket (const BasicVRTPacket &p);
    
    /** Internal constructor, for use by subclasses.
     *  @param p                The packet (already initialized).
     *  @param type             The expected packet type.
     *  @param classID          The expected classID.
     *  @param minPayloadLength The minimum payload length.
     *  @param maxPayloadLength The maximum payload length (&lt;0 if not pre-defined).
     *  @throws IllegalArgumentException If packet type or classID of the packet are invalid, or if
     *                                   the payload length is invalid.
     */
    protected: EphemerisPacket (const BasicVRTPacket &p, PacketType type,
                            int64_t classID, int32_t minPayloadLength, int32_t maxPayloadLength);
    
    /** Creates a new instance accessing the given data buffer.
     *  @param bbuf     The data buffer to use.
     *  @param readOnly Should users of this instance be able to modify the underlying data buffer?
     */
    public: EphemerisPacket (const vector<char> &bbuf, bool readOnly);
    
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
    
    /** Get ephemeris produced by. 
     *  <br> Size: <tt>32</tt>
     *  @return The value read from the packet.
     */
    public: inline string getProducedBy () const {
      return unpackPayloadAscii(0, 32);
    }
    
    /** Set ephemeris produced by. 
     *  <br> Size: <tt>32</tt>
     *  @param val The new value to write to the packet.
     *             Any non-ASCII characters in the string (numeric value &gt;0x7F) or ASCII
     *             control characters (numeric value &lt;0x20, 0x7F) will be replaced with '?'.
     *             If the string is longer than the size allotted, it will be
     *             truncated without warning. 
     */
    public: inline void setProducedBy (string val) {
      packPayloadAscii(0, val, 32);
    }
    
    /** Get the position fix type. 
     *  @return The value read from the packet.
     */
    public: inline FixType getPosFixType () const {
      return (FixType)unpackPayloadByte(32);
    }
    
    /** Set the position fix type. 
     *  @param val The new value to write to the packet.
     */
    public: inline void setPosFixType (FixType val) {
      packPayloadByte(32, (int8_t)val);
    }
    
    /** Get the velocity fix type. 
     *  @return The value read from the packet.
     */
    public: inline FixType getVelFixType () const {
      return (FixType)unpackPayloadByte(33);
    }
    
    /** Set the velocity fix type. 
     *  @param val The new value to write to the packet.
     */
    public: inline void setVelFixType (FixType val) {
      packPayloadByte(33, (int8_t)val);
    }
    
    /** Get the acceleration fix type. 
     *  @return The value read from the packet.
     */
    public: inline FixType getAccFixType () const {
      return (FixType)unpackPayloadByte(34);
    }
    
    /** Set the acceleration fix type. 
     *  @param val The new value to write to the packet.
     */
    public: inline void setAccFixType (FixType val) {
      packPayloadByte(34, (int8_t)val);
    }
    
    /** Get the attitude fix type. 
     *  @return The value read from the packet.
     */
    public: inline FixType getAttFixType () const {
      return (FixType)unpackPayloadByte(35);
    }
    
    /** Set the attitude fix type. 
     *  @param val The new value to write to the packet.
     */
    public: inline void setAttFixType (FixType val) {
      packPayloadByte(35, (int8_t)val);
    }
    
    /** Get time stamp of the first point. 
     *  <br> Units: <i>sec</i>
     *  @return The value read from the packet.
     */
    public: inline TimeStamp getFixTime () const {
      return unpackPayloadTimeStamp(36, IntegerMode_GPS);
    }
    
    /** Set time stamp of the first point. 
     *  <br> Units: <i>sec</i>
     *  @param val The new value to write to the packet.
     */
    public: inline void setFixTime (TimeStamp val) {
      packPayloadTimeStamp(36, val, IntegerMode_GPS);
    }
    
    /** Get time delta between each set of points (seconds + picoseconds) max is one day (0 if only one point). 
     *  <br> Units: <i>ps</i>
     *  <br> Range: <tt>[0,86401e12]</tt>
     *  @return The value read from the packet.
     */
    public: inline int64_t getFixDelta () const {
      return unpackPayloadLong(48);
    }
    
    /** Set time delta between each set of points (seconds + picoseconds) max is one day (0 if only one point). 
     *  <br> Units: <i>ps</i>
     *  <br> Range: <tt>[0,86401e12]</tt>
     *  @param val The new value to write to the packet.
     */
    public: inline void setFixDelta (int64_t val) {
      packPayloadLong(48, val);
    }
    
    /** Get ECEF fix points (1+). 
     *  <br> Size: <tt>variable</tt>
     *  @return The value read from the packet.
     */
    public: vector<Point> getAllPoints () const;
    
    /** Get method used by the HasFields functions. */
    private: vector<VRTObject*>* _getAllPoints () const;
    
    /** Get item in ECEF fix points (1+). 
     *  @param i The index.
     *  @return The value read from the packet.
     *  @throws VRTException if <tt>i</tt> is greater than or equal to getPointCount()
     */
    public: Point getPoint (int32_t i) const;
    
    /** Get length of ECEF fix points (1+). 
     *  @return The number of entries.
     */
    public: inline int32_t getPointCount () const {
      return (getPayloadLength()-56)/72;
    }
    
    /** Set ECEF fix points (1+). 
     *  <br> Size: <tt>variable</tt>
     *  @param val The new value to write to the packet.
     *  @throws VRTException If the length of <tt>val</tt> is invalid
     */
    public: void setAllPoints (const vector<Point> &val);
    
    /** Set method used by the HasFields functions. */
    private: void _setAllPoints (const Value *val);
    
    /** Set item in ECEF fix points (1+). 
     *  @param i   The index.
     *  @param val The new value.
     *  @throws VRTException if <tt>i</tt> is greater than or equal to getPointCount()
     */
    public: void setPoint (int32_t i, const Point & val);
    
    /** Set length of ECEF fix points (1+). 
     *  @param length The number of entries.
     *  @throws VRTException if <tt>length</tt> is negative
     */
    public: void setPointCount (int32_t length);
  };
} END_NAMESPACE
string   operator+ (string  &s, FixType val);
ostream& operator<<(ostream &s, FixType val);
#endif /* _EphemerisPacket_h */


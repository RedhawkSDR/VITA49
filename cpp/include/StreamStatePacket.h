//
//  This class was auto-generated, do not modify.
//
#ifndef _StreamStatePacket_h
#define _StreamStatePacket_h

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
  /** System State Enumerated Values */
 enum SystemState { 
    /** Value not specified */
    SystemState_null = 0,
    /** System is booting up, but not yet ready for initialization */
    SystemState_BOOTING = 1,
    /** System is initializing */
    SystemState_INITIALIZING = 2,
    /** System has been initialized, and is preparing to run */
    SystemState_STARTING = 3,
    /** System is running */
    SystemState_RUNNING = 4,
    /** System is done processing */
    SystemState_STOPPING = 5,
    /** System has completed operation and is exiting normally */
    SystemState_EXITING = 6,
    /** System is pausing */
    SystemState_PAUSING = 7,
    /** System is breaking out of the current running state on its way to a restart */
    SystemState_BREAKING = 8,
    /** System is restarting */
    SystemState_RESTARTING = 9,
    /** An error event has taking place and the system is now aborting */
    SystemState_ABORTING = 10,
    /** System has aborted operation and is terminating abnormally */
    SystemState_TERMINATING = 11,
    /** System is running and is currently identifying the input format */
    SystemState_IDENTIFYING = 12,
    /** System is running and is configuring itsself for processing */
    SystemState_CONFIGURING = 13,
    /** System is running and is processing the input */
    SystemState_PROCESSING = 14
  };
  /** System Status Enumerated Values */
 enum SystemStatus { 
    /** Value not specified */
    SystemStatus_null = 0,
    /** One of more errors, functionality greatly restricted (or non-functional) */
    SystemStatus_RED = 1,
    /** One or more warnings, functionality may be limited */
    SystemStatus_YELLOW = 2,
    /** Everything is good, functioning normally */
    SystemStatus_GREEN = 3,
    /** Everything is good, but nothing to do */
    SystemStatus_BLUE = 4,
    /** Not applicable or turned off */
    SystemStatus_GRAY = 5
  };
  
  //======================================================================
  // PACKET TYPE
  //======================================================================
  /** <b>A.5.\ Stream State Packet</b>. 
   *  
   *          The "stream state packet" provides describes the overall state of the
   *          system at the given reference point and provides receivers with greater
   *          insight into the current activity state of the packet's emitter.<br><br>
   *  
   *          The "state" of a system refers to its position in the state diagram of
   *          the emitter. The states are based on the following generalized state
   *          diagram, though individual systems <b>may</b> omit stages and/or
   *          <b>may</b> differ slightly:<br><br>
   *  
   *          <pre>
   *                                                        +---------+
   *                                                        | PAUSING |
   *                                                        +---------+
   *                                                           ^   |
   *                                                           |   V
   *  +---------+     +--------------+     +----------+     +---------+     +----------+     +---------+
   *  | BOOTING | --&gt; | INITIALIZING | --&gt; | STARTING | --&gt; | RUNNING | --&gt; | STOPPING | --&gt; | EXITING |
   *  +---------+     +--------------+     +----------+     +---------+     +----------+     +---------+
   *                                                           ^   |
   *                                                           |   |
   *                                     +------------+        |   |        +----------+
   *                                     | RESTARTING | -------+   +------&gt; | BREAKING |
   *                                     +------------+                     +----------+
   *                                           ^                                  |
   *                                           +----------------------------------+<br><br>
   *  
   *  
   *                                                                    +----------+     +-------------+
   *                                                           (*) ---&gt; | ABORTING | --&gt; | TERMINATING |
   *                                                                    +----------+     +-------------+
   *          </pre><br><br>
   *  
   *          The states are as follows:
   *          <ul>
   *            <li><b>BOOTING</b>      - System is booting up, but not yet ready for
   *                                      initialization, <i>(this state is generally
   *                                      "implied" and rarely visible outside of log
   *                                      file messages)</i>;</li>
   *            <li><b>INITIALIZING</b> - System is initializing;</li>
   *            <li><b>STARTING</b>     - System has been initialized, and is preparing
   *                                      to run;</li>
   *            <li><b>RUNNING</b>      - System is running, this included situations
   *                                      where the system is waiting for input;</li>
   *            <li><b>STOPPING</b>     - System is done processing (e.g. in response
   *                                      to user input or the result of not having
   *                                      any more data to process) and is stopping;</li>
   *            <li><b>EXITING</b>      - System has completed operation and is exiting
   *                                      normally <i>(this state is generally "implied"
   *                                      and rarely visible outside of log file
   *                                      messages)</i>;</li>
   *            <li><b>PAUSING</b>      - System is pausing, this is generally in
   *                                      response to user input, such as pausing
   *                                      playback of a recorded signal;</li>
   *            <li><b>BREAKING</b>     - System is breaking out of the current running
   *                                      state on its way to a restart;</li>
   *            <li><b>RESTARTING</b>   - System is restarting;</li>
   *            <li><b>ABORTING</b>     - An error event has taking place or an "abort"
   *                                      signal has been sent to the system, and the
   *                                      system is now aborting normal operation <i>(this
   *                                      state can follow any other state and might not
   *                                      be visible outside of log file messages)</i>;
   *                                      and</li>
   *            <li><b>TERMINATING</b>  - System has aborted operation and is terminating
   *                                      abnormally <i>(this state is generally "implied"
   *                                      and rarely visible outside of log file
   *                                      messages)</i>.</li>
   *          </ul><br><br>
   *  
   *          Some systems <b>may</b> split the <b>RUNNING</b> state into three separate
   *          states:
   *          <ul>
   *            <li><b>IDENTIFYING</b>  - System is running and is currently identifying
   *                                      the input format;</li>
   *            <li><b>CONFIGURING</b>
   *                 or <b>LOCKING</b>  - System is running, has identified the input
   *                                      format and is configuring itself for processing;
   *                                      and</li>
   *            <li><b>PROCESSING</b>   - System is running, has identified the input
   *                                      format, has been configured and is processing
   *                                      the input.</li>
   *          </ul><br><br>
   *  
   *          The typical example of where the <b>IDENTIFYING</b>, <b>LOCKING</b>, and
   *          <b>PROCESSING</b> states are used is within a demodulator where the
   *          <b>IDENTIFYING</b> state implies that the demodulator is detecting the
   *          modulation type; <b>LOCKING</b> implies that the demodulator is trying
   *          to lock up to the signal; and <b>PROCESSING</b> means that the demodulator
   *          is locked to the identified signal and is demodulating it. If the demodulator
   *          looses lock, it can return to either the <b>IDENTIFYING</b> (it wants to
   *          re-check the modulation type) or the <b>LOCKING</b> state.<br><br>
   *  
   *          The "status" of a system refers to the "health" of the system's processing
   *          ability at three key points (input, internal, output) and an aggregate
   *          status indicator. The "health" is essentially a "traffic light" color
   *          coding that mimics the status lights on traditional radio equipment. The
   *          <b>BLUE</b> state is an addition that may be used to differentiate normal
   *          (<b>GREEN</b>) operation, where output is being produced, from that where
   *          there is no output being produced (e.g. a radio that is receiving and
   *          processing a signal, but there is no "user data" to output since the
   *          decoded signal consists only of "idle" indicators).
   *        
   */
  class StreamStatePacket : public BasicVRTPacket {
    /** The packet type (<tt>PacketType_ExtContext</tt>). */
    public: static const PacketType PACKET_TYPE;
    
    /** The packet length (<tt>-8</tt>) excluding the header and trailer (if applicable).
        A value of -8 indicates that the length is variable with a minimum length of 8 octets.
     */
    public: static const int32_t PACKET_LENGTH;
    
    /** The class identifier in numeric form (<tt>0xFFFFFA20110005</tt>). */
    public: static const int64_t CLASS_IDENTIFIER;
    
    /** The class identifier in string form (<tt>FF-FF-FA:2011.0005</tt>). */
    public: static const string CLASS_ID;
    
    //======================================================================
    // CONSTRUCTORS
    //======================================================================
    /** Creates a new instance ready for the fields to be set. The stream ID
     *  and timecode values will be initialized to 0.
     */
    public: StreamStatePacket ();
    
    /** Creates a new instance from the given packet.
     *  @param p    The packet.
     *  @throws VRTException If the packet given does not match this type.
     */
    public: StreamStatePacket (const BasicVRTPacket &p);
    
    /** Internal constructor, for use by subclasses.
     *  @param p                The packet (already initialized).
     *  @param type             The expected packet type.
     *  @param classID          The expected classID.
     *  @param minPayloadLength The minimum payload length.
     *  @param maxPayloadLength The maximum payload length (&lt;0 if not pre-defined).
     *  @throws IllegalArgumentException If packet type or classID of the packet are invalid, or if
     *                                   the payload length is invalid.
     */
    protected: StreamStatePacket (const BasicVRTPacket &p, PacketType type,
                            int64_t classID, int32_t minPayloadLength, int32_t maxPayloadLength);
    
    /** Creates a new instance accessing the given data buffer.
     *  @param bbuf     The data buffer to use.
     *  @param readOnly Should users of this instance be able to modify the underlying data buffer?
     */
    public: StreamStatePacket (const vector<char> &bbuf, bool readOnly);
    
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
    
    /** Get the system state. 
     *  @return The value read from the packet.
     */
    public: inline SystemState getSystemState () const {
      return (SystemState)unpackPayloadByte(0);
    }
    
    /** Set the system state. 
     *  @param val The new value to write to the packet.
     */
    public: inline void setSystemState (SystemState val) {
      packPayloadByte(0, (int8_t)val);
    }
    
    /** Get the aggregate system status. 
     *  @return The value read from the packet.
     */
    public: inline SystemStatus getAggregateSystemStatus () const {
      return (SystemStatus)unpackPayloadByte(4);
    }
    
    /** Set the aggregate system status. 
     *  @param val The new value to write to the packet.
     */
    public: inline void setAggregateSystemStatus (SystemStatus val) {
      packPayloadByte(4, (int8_t)val);
    }
    
    /** Get the status of the input. 
     *  @return The value read from the packet.
     */
    public: inline SystemStatus getInputStatus () const {
      return (SystemStatus)unpackPayloadByte(5);
    }
    
    /** Set the status of the input. 
     *  @param val The new value to write to the packet.
     */
    public: inline void setInputStatus (SystemStatus val) {
      packPayloadByte(5, (int8_t)val);
    }
    
    /** Get the status of the system's internals. 
     *  @return The value read from the packet.
     */
    public: inline SystemStatus getInternalStatus () const {
      return (SystemStatus)unpackPayloadByte(6);
    }
    
    /** Set the status of the system's internals. 
     *  @param val The new value to write to the packet.
     */
    public: inline void setInternalStatus (SystemStatus val) {
      packPayloadByte(6, (int8_t)val);
    }
    
    /** Get the status of the output. 
     *  @return The value read from the packet.
     */
    public: inline SystemStatus getOutputStatus () const {
      return (SystemStatus)unpackPayloadByte(7);
    }
    
    /** Set the status of the output. 
     *  @param val The new value to write to the packet.
     */
    public: inline void setOutputStatus (SystemStatus val) {
      packPayloadByte(7, (int8_t)val);
    }
    
    /** Get any messages to better explain the processing state and/or status. 
     *  <br> Size: <tt>variable</tt>
     *  @return The value read from the packet.
     */
    public: inline wstring getMessage () const {
      return unpackPayloadUTF8(8, getPayloadLength()-8);
    }
    
    /** Set any messages to better explain the processing state and/or status. 
     *  <br> Size: <tt>variable</tt>
     *  @param val The new value to write to the packet.
     */
    public: inline void setMessage (wstring val) {
    int32_t limit  = MAX_PAYLOAD_LENGTH-8;
    int32_t length = vrt::VRTMath::lengthUTF8(val);
    int32_t len    = (length > limit)? ((limit  + 3) & 0x7FFFFFFC)
                                     : ((length + 3) & 0x7FFFFFFC); // length to next multiple of 4
      setPayloadLength(8 + len);
      packPayloadUTF8(8, val, len);
    }
    
    /** Set any messages to better explain the processing state and/or status. 
     *  <br> Size: <tt>variable</tt>
     *  @param val The new value to write to the packet.
     */
    public: inline void setMessage (string val) {
    int32_t limit  = MAX_PAYLOAD_LENGTH-8;
    int32_t length = vrt::VRTMath::lengthUTF8(val);
    int32_t len    = (length > limit)? ((limit  + 3) & 0x7FFFFFFC)
                                     : ((length + 3) & 0x7FFFFFFC); // length to next multiple of 4
      setPayloadLength(8 + len);
      packPayloadUTF8(8, val, len);
    }
  };
} END_NAMESPACE
string   operator+ (string  &s, SystemState val);
ostream& operator<<(ostream &s, SystemState val);
string   operator+ (string  &s, SystemStatus val);
ostream& operator<<(ostream &s, SystemStatus val);
#endif /* _StreamStatePacket_h */


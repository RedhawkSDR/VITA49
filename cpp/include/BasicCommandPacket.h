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

#ifndef _BasicCommandPacket_h
#define _BasicCommandPacket_h

#include "BasicVRTPacket.h"
//#include "VRTConfig.h"
#include "UUID.h"
#include <cstring>
#include <vector>
#include <sstream>
#include <iomanip>

namespace vrt {
  using namespace std;

  // Bit masks for all pkt types
  namespace protected_PSP {
    static const int32_t CONTROL_CE_BIT  = 0x80000000; // Bit 31
    static const int32_t CONTROL_IE_BIT  = 0x40000000; // Bit 30
    static const int32_t CONTROL_CR_BIT  = 0x20000000; // Bit 29
    static const int32_t CONTROL_IR_BIT  = 0x10000000; // Bit 28
    static const int32_t CONTROL_P_BIT   = 0x08000000; // Bit 27
    static const int32_t CONTROL_W_BIT   = 0x04000000; // Bit 26
    static const int32_t CONTROL_E_BIT   = 0x02000000; // Bit 25
    static const int32_t CONTROL_A1_BIT  = 0x01000000; // Bit 24
    static const int32_t CONTROL_A0_BIT  = 0x00800000; // Bit 23
    static const int32_t CONTROL_NK_BIT  = 0x00400000; // Bit 22
    static const int32_t PSP_RESERVED_21 = 0x00200000; // Bit 21
    static const int32_t CTRLACK_V_BIT   = 0x00100000; // Bit 20
    static const int32_t CTRLACK_X_BIT   = 0x00080000; // Bit 19
    static const int32_t CTRLACK_S_BIT   = 0x00040000; // Bit 18
    static const int32_t CTRLACK_W_BIT   = 0x00020000; // Bit 17
    static const int32_t CTRLACK_E_BIT   = 0x00010000; // Bit 16
    static const int32_t PSP_RESERVED_15 = 0x00008000; // Bit 15
    static const int32_t CTRLACK_T2_BIT  = 0x00004000; // Bit 14
    static const int32_t CTRLACK_T1_BIT  = 0x00002000; // Bit 13
    static const int32_t CTRLACK_T0_BIT  = 0x00001000; // Bit 12
    static const int32_t ACK_P_BIT       = 0x00000800; // Bit 11
    static const int32_t ACK_SX_BIT      = 0x00000400; // Bit 10
    static const int32_t PSP_RESERVED_9  = 0x00000200; // Bit 9
    static const int32_t PSP_RESERVED_8  = 0x00000100; // Bit 8
    static const int32_t PSP_RESERVED_7  = 0x00000080; // Bit 7
    static const int32_t PSP_RESERVED_6  = 0x00000040; // Bit 6
    static const int32_t PSP_RESERVED_5  = 0x00000020; // Bit 5
    static const int32_t PSP_RESERVED_4  = 0x00000010; // Bit 4
    static const int32_t PSP_RESERVED_3  = 0x00000008; // Bit 3
    static const int32_t PSP_RESERVED_2  = 0x00000004; // Bit 2
    static const int32_t PSP_RESERVED_1  = 0x00000002; // Bit 1
    static const int32_t PSP_RESERVED_0  = 0x00000001; // Bit 0
  } END_NAMESPACE // protected_PSP namespace
  using namespace protected_PSP;



  /*****************************************************************************/
  /*****************************************************************************/
  /**                                                                         **/
  /**                        BasicCommandPacket                               **/
  /**                                                                         **/
  /*****************************************************************************/
  /*****************************************************************************/

  // XXX - Note: HasFields implementation may be complicated for ControlPacket w/ conflicts b/t Context and Command
  class BasicCommandPacket : public virtual BasicVRTPacket {

    //======================================================================
    // Constructors/Destructors
    //======================================================================

    /** Basic destructor. */
    public: ~BasicCommandPacket () { }

    /** Basic copy constructor. */
    public: BasicCommandPacket (const BasicVRTPacket &p);

    /** Internal constructor, for use by subclasses.
     *  @param p       The packet (already initialized).
     *  @param classID The expected classID.
     *  @throws VRTException If packet type or classID of the packet are invalid.
     */
    public: BasicCommandPacket (const BasicVRTPacket &p, int64_t classID);

    /** Creates a new instance accessing the given data buffer.
     *  @param buf      The pointer to the packet buffer.
     *  @param len      The length  of the packet buffer.
     *  @param readOnly Should users of this instance be able to modify the underlying data buffer?
     */
    public: BasicCommandPacket (const void *buf, size_t len, bool readOnly=false);

    /** Creates a new instance that can be written to. Initially this will just be a simple command
     *  packet with StreamID, ClassID, TSI=UTC, TSF=Real-time, CAM field, MessageID, 128-bit
     *  ControlleeID, 128-bit ControllerID, CIF0, and a packet size of 18.
     */
    public: BasicCommandPacket ();

    /** Creates a new instance with a default anticipated length that can be written to.
     *  Initially this will just be a simple context packet with no fields set (other than
     *  the required packet length of 8 and a default stream ID  of 0), but will have the
     *  underlying buffers pre-allocated as required.
     *  @param bufsize The anticipated buffer size.
     */
    public: BasicCommandPacket (int32_t bufsize);

    public: BasicCommandPacket (const vector<char> &buf, ssize_t start, ssize_t end, bool readOnly = false);

    /** Creates a new instance accessing the given data buffer. Note that when the buffer lengths
     *  are given, only the most minimal of error checking is done. Users should call
     *  <tt>isPacketValid()</tt> to verify that the packet is valid. Invalid packets can result
     *  unpredictable behavior, but this is explicitly allowed (to the extent possible) so that
     *  applications creating packets can use this class even if the packet isn't yet "valid".
     *  @param buf      The data buffer to use.
     *  @param readOnly Should users of this instance be able to modify the underlying data buffer?
     *  @throws VRTException If the offset is negative or greater than the
     *          length of the buffer given. Also thrown if the buffer length is -1 and a copy
     *          can not be made because the sizes in the header of the packet are invalid.
     */
    public: BasicCommandPacket (const vector<char> &buf, bool readOnly = false);

    public: BasicCommandPacket (vector<char> *buf_ptr, bool readOnly = false);

    /** Internal constructor, for use by subclasses.
     *  @param p                The packet (already initialized).
     *  @param type             The expected packet type.
     *  @param classID          The expected classID.
     *  @param minPayloadLength The minimum payload length.
     *  @param maxPayloadLength The maximum payload length (&lt;0 if not pre-defined).
     *  @throws VRTException If packet type or classID of the packet are invalid, or if
     *                       the payload length is invalid.
     */
    protected: BasicCommandPacket (const BasicVRTPacket &p, PacketType type, int64_t classID,
                                   int32_t minPayloadLength, int32_t maxPayloadLength);



    //======================================================================
    // UTILITY METHODS
    //======================================================================

    public:    virtual string    getPacketValid (bool strict, int32_t length) const;

    protected: virtual void      toStringStream (ostringstream &str) const;

    public: virtual inline void setPacketType (PacketType t) {
      if (t != PacketType_Command) {
        throw VRTException("Can not change packet type to non-Command type when using BasicCommandPacket");
      }
      BasicVRTPacket::setPacketType(t);
    }

    // TODO - what new things need to be reset? if nothing, don't re-implement
    /** Prepares a packet for resend resetting any non-persistent values. This is implemented as follows:
     *  <!--
     *  <pre>
     *    bool BasicCommandPacket::resetForResend (const TimeStamp &t) {
     *      BasicVRTPacket::resetForResend(t);
     *      return true;
     *    }
     *  </pre>
     *  @param t The new time stamp.
     *  @return true
     *  -->
     */
    //public: virtual bool resetForResend (const TimeStamp &t);

    // override prologue-related functions now that we have a PSP
    // XXX - anything else besides these two??

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




    //======================================================================
    // COMMAND PACKET METHODS (PacketSpecificPrologue, etc.)
    //======================================================================

    // Note: to be consistent with rest of library, getters/setters for Header indicator fields
    //       are defined in the BasicVRTPacket class (i.e. isControlAckMode() for Ack bit)

    // Control Acknowledge Settings Field
    //Control-CE
    public:    bool isControlleeEnable () const;
    protected: void setControlleeEnable (bool set);

    //Control-IE
    public:    bool getControlleeFormat () const;
    protected: void setControlleeFormat (bool set);

    //Control-CR
    public:    bool isControllerEnable () const;
    protected: void setControllerEnable (bool set);

    //Control-IR
    public:    bool getControllerFormat () const;
    protected: void setControllerFormat (bool set);

    //Control-P
    public: bool isPartialChangePermitted () const;
    public: void setPartialChangePermitted (bool set);

    //Control-W
    public: bool isWarningsPermitted () const;
    public: void setWarningsPermitted (bool set);

    //Control-E
    public: bool isErrorsPermitted () const;
    public: void setErrorsPermitted (bool set);

    //Control-A (A1..A0)
    public: bool getAction1Flag () const;
    public: void setAction1Flag (bool set);
    public: bool getAction0Flag () const;
    public: void setAction0Flag (bool set);
    public: int8_t getAction () const;
    public: void setAction (int8_t action);
    public: bool isActionNoAction () const;
    public: void setActionNoAction ();
    public: bool isActionDryRun () const;
    public: void setActionDryRun ();
    public: bool isActionTakeAction () const;
    public: void setActionTakeAction ();

    //Control-NK
    public: bool getNotAckOnly () const;
    public: void setNotAckOnly (bool set);

    //Control-V
    public: bool getRequestValidationAcknowledge () const;
    public: void setRequestValidationAcknowledge (bool set);
    //Acknowledge-V
    public: bool isValidationAcknowledge () const {
      return getRequestValidationAcknowledge();
    }

    /** Sets specific type of Acknowledge Packet
     *  With no arguments, default is to set packet to Validation Acknowledge
     *  @param set If true (default), set packet type to Validation Acknowledge
     *             If false, set packet type to Execution Acknowledge
     */
    public: void setValidationAcknowledge (bool set=true) {
      setRequestExecutionAcknowledge(!set);
      setRequestValidationAcknowledge(set);
    }

    //Control-X
    public: bool getRequestExecutionAcknowledge () const;
    public: void setRequestExecutionAcknowledge (bool set);
    //Acknowledge-X
    public: bool isExecutionAcknowledge () const {
      return getRequestExecutionAcknowledge();
    }

    /** Sets specific type of Acknowledge Packet
     *  With no arguments, default is to set packet to Execution Acknowledge
     *  @param set If true (default), set packet type to Execution Acknowledge
     *             If false, set packet type to Validation Acknowledge
     */
    public: void setExecutionAcknowledge (bool set=true) {
      setRequestValidationAcknowledge(!set);
      setRequestExecutionAcknowledge(set);
    }

    //Control-S
    public: bool getRequestQueryAcknowledge () const;
    public: void setRequestQueryAcknowledge (bool set);
    //Acknowledge-S
    public: bool isQueryAcknowledge () const {
      return getRequestQueryAcknowledge();
    }
    public: void setQueryAcknowledge (bool set) {
      setRequestQueryAcknowledge(set);
    }

    //Acknowledge-W
    public: bool getWarningsGenerated () const;
    public: virtual void setWarningsGenerated (bool set);
    //Control-W
    public: bool getRequestWarningsGenerated () const {
      return getWarningsGenerated();
    }
    public: virtual void setRequestWarningsGenerated (bool set) {
      setWarningsGenerated(set);
    }

    //Acknowledge-E
    public: bool getErrorsGenerated () const;
    public: virtual void setErrorsGenerated (bool set);
    //Control-E
    public: bool getRequestErrorsGenerated () const {
      return getErrorsGenerated();
    }
    public: virtual void setRequestErrorsGenerated (bool set) {
      setErrorsGenerated(set);
    }

    //Control/Ack-T2,T1,T0
    public: bool getTimestampControl2Bit () const;
    public: void setTimestampControl2Bit (bool set);
    public: bool getTimestampControl1Bit () const;
    public: void setTimestampControl1Bit (bool set);
    public: bool getTimestampControl0Bit () const;
    public: void setTimestampControl0Bit (bool set);
    public: int8_t getTimestampControlMode () const;
    public: void setTimestampControlMode (int8_t mode);

    //Acknowledge-P
    public: bool getPartialAction () const;
    public: void setPartialAction (bool set);

    //Acknowledge-SX
    public: bool getActionExecuted () const;
    public: void setActionExecuted (bool set);

    // Rest of Packet Specific Prologue for Command Packets
    // Message ID - 32Bit Number
    public: int32_t getMessageID () const;
    public: void setMessageID (int32_t val);

    // Controllee ID/UUID
    public: int32_t getControlleeIDNumber () const;
    public: void setControlleeIDNumber (int32_t val);
    public: UUID getControlleeUUID () const;
    public: void setControlleeUUID (const UUID &val);

    // Controller ID/UUID
    public: int32_t getControllerIDNumber () const;
    public: void setControllerIDNumber (int32_t val);
    public: UUID getControllerUUID () const;
    public: void setControllerUUID (const UUID &val);

    // HELPERS
    protected: virtual int32_t getCtrlAckSettingsField () const {
      return VRTMath::unpackInt(bbuf, getHeaderLength());
    }
    protected: virtual bool getCtrlAckSettingsBit (int32_t bit) const {
      return ((getCtrlAckSettingsField() & bit) != 0);
    }
    protected: virtual void setCtrlAckSettingsBit (int32_t bit, bool set);

    private: int32_t getOffset (bool controller) const; // TODO why not name it getPspOffset(...)?
    private: int32_t getFieldLen (bool controller) const; // TODO why not name it getPspFieldLen(...)?

    // Optional PSP fields - getters
    private: int32_t getControlIDNumber (bool controller) const;
    private: UUID getControlUUID (bool controller) const;

    // Optional PSP fields - setters
    private: void setControlIDNumber (bool controller, int32_t val);
    private: void setControlUUID (bool controller, const UUID &val);


    /* deprecated - using UUID lib instead
    public: string getControlleeUUID () const;
    public: void setControlleeUUID (string val);
    public: void getControlleeUUID (int32_t& val0, int32_t& val1, int32_t& val2, int32_t& val3) const;
    public: void setControlleeUUID (int32_t val0, int32_t val1, int32_t val2, int32_t val3);
    */
    /* deprecated - using UUID lib instead
    public: string getControllerUUID () const;
    public: void setControllerUUID (string val);
    public: void getControllerUUID (int32_t& val0, int32_t& val1, int32_t& val2, int32_t& val3) const;
    public: void setControllerUUID (int32_t val0, int32_t val1, int32_t val2, int32_t val3);
    */
    /* deprecated - using UUID lib instead
    private: void getControlUUID (bool controller,
                                  int32_t& val0, int32_t& val1, int32_t& val2, int32_t& val3) const;
    // delim=0 will produce string with no delimiters
    private: string getControlUUID (bool controller, char delim='-') const;
    */
    /* deprecated - using UUID lib instead
    private: void setControlUUID (bool controller,
                                  int32_t val0, int32_t val1, int32_t val2, int32_t val3);
    private: void setControlUUID (bool controller, string val);
    */


    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Implement HasFields
    //////////////////////////////////////////////////////////////////////////////////////////////////
    /* TODO
    public: virtual int32_t   getFieldCount () const;
    public: virtual string    getFieldName  (int32_t id) const;
    public: virtual ValueType getFieldType  (int32_t id) const;
    public: virtual Value*    getField      (int32_t id) const __attribute__((warn_unused_result));
    public: virtual void      setField      (int32_t id, const Value* val);
    */

  }; /* class BasicCommandPacket */

} END_NAMESPACE /* vrt namespace */

#endif /* _BasicCommandPacket_h */

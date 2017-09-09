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

#include "BasicCommandPacket.h"
#include <bitset> // includes the popcount functions // TODO needed?

using namespace vrt;
using namespace protected_PSP;

//TODO if the below is needed, consider moving to a .h file to be included
//     rather than duplicating in multiple .cc files (also in BAsicContextPacket.cc)
/** Basic population count. This algorithm comes from [1] and is believed to be
 *  nearly-identical to what is being done in Java. The use of GCC's
 *  <tt>__builtin_popcount(..)</tt> was considered, but excluded since it is
 *  slower than <tt>__builtin_popcountll(..)</tt> due to a GCC bug (see [2]).
 *  Use of <tt>__builtin_popcountll(..)</tt> was excluded since, in practice,
 *  it is comparable to what is done here (see measurement results in [2]).
 *  <pre>
 *    References:
 *     [1] Wikipedia. "Hamming Weight." http://en.wikipedia.org/wiki/Hamming_weight
 *         (Last Accessed 23 April 2013)
 *
 *     [2] Dalke, Andrew. "Update: Faster population counts." 2 November 2011.
 *         http://www.dalkescientific.com/writings/diary/archive/2011/11/02/faster_popcount_update.html
 *         (Last Accessed 23 April 2013)
 *  </pre>
 */
# if defined(__INTEL_COMPILER)
#  define bitCount(val) _popcnt32(val)
# else
static inline int32_t bitCount (int32_t val) {
  uint32_t n = (uint32_t)val;
  n = n - ((n >> 1) & 0x55555555);
  n = (n & 0x33333333) + ((n >> 2) & 0x33333333);
  n = (n + (n >> 4)) & 0x0F0F0F0F;
  n = (n + (n >> 8));
  n = (n + (n >> 16));
  return n & 0x3F;
}
#endif


  /*****************************************************************************/
  /*****************************************************************************/
  /**                                                                         **/
  /**                        BasicCommandPacket                               **/
  /**                                                                         **/
  /*****************************************************************************/
  /*****************************************************************************/

/**  Creates the default prologue for the packet. This is defined separate from the class to account
 *  for the systems/compilers that disallow calling of subclass methods from the constructor.
 */
static inline vector<char> BasicCommandPacket_createDefaultPacket () {
  vector<char> buf(BasicVRTPacket::MAX_PROLOGUE_LENGTH + 4);
  buf[0]  = 0x68; // Control w/ CID, not StaleTs, not Cancel
  buf[1]  = 0x60; // TSI: UTC, TSF: Real-Time (ps) fractional timestamp, packet count =0
  buf[2]  = 0x00; // 
  buf[3]  = 0x12; // Packet size = 18 (full header, full psp, +1 for CIF0)
  buf[28] = 0xF0; // Has 128-bit ControlleeID and 128-bit ControllerID
  return buf;
}


//======================================================================
// Constructors/Destructors
//======================================================================

BasicCommandPacket::BasicCommandPacket (const BasicVRTPacket &p) :
  BasicVRTPacket(p)
{
  if (!isNullValue() && (getPacketType() != PacketType_Command)) {
    throw VRTException("Can not create CommandPacket from given packet");
  }
}

BasicCommandPacket::BasicCommandPacket (const BasicVRTPacket &p, int64_t classID) :
  BasicVRTPacket(p)
{
  if (getPacketType() != PacketType_Command) {
    throw VRTException("Can not create CommandPacket from given packet");
  }
  setClassIdentifier(classID);
}

BasicCommandPacket::BasicCommandPacket (const void *buf, size_t len, bool readOnly) :
  BasicVRTPacket(buf,len,readOnly)
{
  // done
}

BasicCommandPacket::BasicCommandPacket () :
  BasicVRTPacket(BasicCommandPacket_createDefaultPacket(),false)
{
  // done
}

BasicCommandPacket::BasicCommandPacket (int32_t bufsize) :
  BasicVRTPacket(bufsize)
{
  // done
}

BasicCommandPacket::BasicCommandPacket (const vector<char> &buf, ssize_t start, ssize_t end, bool readOnly) :
  BasicVRTPacket(buf, start, end, readOnly)
{
  // done
}

BasicCommandPacket::BasicCommandPacket (const vector<char> &buf, bool readOnly) :
  BasicVRTPacket::BasicVRTPacket(buf,readOnly)
{
  // done
}

BasicCommandPacket::BasicCommandPacket (vector<char> *buf_ptr, bool readOnly) :
  BasicVRTPacket(buf_ptr, readOnly)
{
  // done
}

BasicCommandPacket::BasicCommandPacket (const BasicVRTPacket &p, PacketType type, int64_t classID,
                                        int32_t minPayloadLength, int32_t maxPayloadLength) :
  BasicVRTPacket(p,type,classID,minPayloadLength,maxPayloadLength)
{
  if (type != PacketType_Command) {
    throw VRTException("BasicCommandPacket can only be created with type=Command");
  }
}

//======================================================================
// UTILITY METHODS
//======================================================================

std::string BasicCommandPacket::getPacketValid (bool strict, int32_t length) const {
  std::string err = BasicVRTPacket::getPacketValid(strict, length);
  if (err != "") return err;

  if (getPacketType() != PacketType_Command) {
    std::ostringstream str;
    str << "Invalid use of " << getClassName() << " with " << getPacketType() << " packet.";
    return str.str();
  }
  return "";
}

void BasicCommandPacket::toStringStream (std::ostringstream &str) const {
  BasicVRTPacket::toStringStream(str);
  // TODO add ControlAck settings (16 bits)
  // TODO add Controllee ID
  // TODO add Controller ID
  //Utilities::append(str, " ChangePacket="            ,isChangePacket()             );
  //Utilities::append(str, " ReferencePointIdentifier=",getReferencePointIdentifier());
  //Utilities::append(str, " Bandwidth="               ,getBandwidth()         ,"Hz" );
}

// bool BasicCommandPacket::resetForResend (const TimeStamp &t) { ... }

// override prologue-related functions now that we have a PSP
int32_t BasicCommandPacket::getPrologueLength () const {
  return getHeaderLength()+getPktSpecificPrologueLength();
}

int32_t BasicCommandPacket::getPktSpecificPrologueLength () const {
  int32_t cas = getCtrlAckSettingsField() & 0xF0000000; // zero all but 4 msbits
  // CONTROL_CE_BIT  = 0x80000000; // Bit 31
  // CONTROL_IE_BIT  = 0x40000000; // Bit 30
  // CONTROL_CR_BIT  = 0x20000000; // Bit 29
  // CONTROL_IR_BIT  = 0x10000000; // Bit 28
  int32_t ids = (cas & (CONTROL_CE_BIT|CONTROL_CR_BIT));  // ids present
  int32_t len = 4  * (2                       )  // mandatory ctrl/ack and msgID
              + 4  * (bitCount(ids           ))  // at least 4 bytes if ID is present
              + 12 * (bitCount(ids & (cas<<1))); // 12 additional bytes if 32-byte ID
  return len; 
}


//======================================================================
// COMMAND PACKET METHODS (PacketSpecificPrologue, etc.)
//======================================================================

// Control Acknowledge Settings Field
//Control-CE
bool BasicCommandPacket::isControlleeEnable () const {
  return getCtrlAckSettingsBit(CONTROL_CE_BIT);
}
void BasicCommandPacket::setControlleeEnable (bool set) {
  setCtrlAckSettingsBit(CONTROL_CE_BIT, set);
}

//Control-IE
bool BasicCommandPacket::getControlleeFormat () const {
  return getCtrlAckSettingsBit(CONTROL_IE_BIT);
}
void BasicCommandPacket::setControlleeFormat (bool set) {
  setCtrlAckSettingsBit(CONTROL_IE_BIT, set);
}

//Control-CR
bool BasicCommandPacket::isControllerEnable () const {
  return getCtrlAckSettingsBit(CONTROL_CR_BIT);
}
void BasicCommandPacket::setControllerEnable (bool set) {
  setCtrlAckSettingsBit(CONTROL_CR_BIT, set);
}

//Control-IR
bool BasicCommandPacket::getControllerFormat () const {
  return getCtrlAckSettingsBit(CONTROL_IR_BIT);
}
void BasicCommandPacket::setControllerFormat (bool set) {
  setCtrlAckSettingsBit(CONTROL_IR_BIT, set);
}

//Control-A
bool BasicCommandPacket::getActionFlag () const {
  return getCtrlAckSettingsBit(CONTROL_A_BIT);
}
void BasicCommandPacket::setActionFlag (bool set) {
  setCtrlAckSettingsBit(CONTROL_A_BIT, set);
}

//Control-P
bool BasicCommandPacket::isPartialChangePermitted() const {
  return getCtrlAckSettingsBit(CONTROL_P_BIT);
}
void BasicCommandPacket::setPartialChangePermitted (bool set) {
  setCtrlAckSettingsBit(CONTROL_P_BIT, set);
}

//Control-W
bool BasicCommandPacket::isWarningsPermitted() const {
  return getCtrlAckSettingsBit(CONTROL_W_BIT);
}
void BasicCommandPacket::setWarningsPermitted (bool set) {
  setCtrlAckSettingsBit(CONTROL_W_BIT, set);
}

//Control-E
bool BasicCommandPacket::isErrorsPermitted() const {
  return getCtrlAckSettingsBit(CONTROL_E_BIT);
}
void BasicCommandPacket::setErrorsPermitted (bool set) {
  setCtrlAckSettingsBit(CONTROL_E_BIT, set);
}

//Control-NK
bool BasicCommandPacket::getNotAckOnly() const {
  return getCtrlAckSettingsBit(CONTROL_NK_BIT);
}
void BasicCommandPacket::setNotAckOnly (bool set) {
  setCtrlAckSettingsBit(CONTROL_NK_BIT, set);
}

//Control-QV
bool BasicCommandPacket::getRequestValidationAcknowledge() const {
  return getCtrlAckSettingsBit(CONTROL_QV_BIT);
}
void BasicCommandPacket::setRequestValidationAcknowledge (bool set) {
  setCtrlAckSettingsBit(CONTROL_QV_BIT, set);
}

//Control-QX
bool BasicCommandPacket::getRequestExecutionAcknowledge() const {
  return getCtrlAckSettingsBit(CONTROL_QX_BIT);
}
void BasicCommandPacket::setRequestExecutionAcknowledge (bool set) {
  setCtrlAckSettingsBit(CONTROL_QX_BIT, set);
}

//Control-QS
bool BasicCommandPacket::getRequestQueryAcknowledge() const {
  return getCtrlAckSettingsBit(CONTROL_QS_BIT);
}
void BasicCommandPacket::setRequestQueryAcknowledge (bool set) {
  setCtrlAckSettingsBit(CONTROL_QS_BIT, set);
}

//Acknowledge-SX
bool BasicCommandPacket::getActionExecuted() const {
  return getCtrlAckSettingsBit(ACK_SX_BIT);
}
void BasicCommandPacket::setActionExecuted (bool set) {
  setCtrlAckSettingsBit(ACK_SX_BIT, set);
}

//Acknowledge-P
bool BasicCommandPacket::getPartialAction() const {
  return getCtrlAckSettingsBit(ACK_P_BIT);
}
void BasicCommandPacket::setPartialAction (bool set) {
  setCtrlAckSettingsBit(ACK_P_BIT, set);
}

//Acknowledge-W
bool BasicCommandPacket::getWarningsGenerated() const {
  return getCtrlAckSettingsBit(ACK_W_BIT);
}
void BasicCommandPacket::setWarningsGenerated (bool set) {
  setCtrlAckSettingsBit(ACK_W_BIT, set);
}

//Acknowledge-E
bool BasicCommandPacket::getErrorsGenerated() const {
  return getCtrlAckSettingsBit(ACK_E_BIT);
}
void BasicCommandPacket::setErrorsGenerated (bool set) {
  setCtrlAckSettingsBit(ACK_E_BIT, set);
}

// Message ID - 32Bit Number
int32_t BasicCommandPacket::getMessageID() const {
  return VRTMath::unpackInt(bbuf, getHeaderLength()+4);
}
void BasicCommandPacket::setMessageID(int32_t val) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");
  VRTMath::packInt(bbuf, getHeaderLength()+4, val); // +4 for CtrlAckSettingsField
}

// Controllee ID/UUID
int32_t BasicCommandPacket::getControlleeIDNumber() const {
  return getControlIDNumber(false); // (controller=false)
}
void BasicCommandPacket::setControlleeIDNumber(int32_t val) {
  setControlIDNumber(false, val); // (controller=false)
}

void BasicCommandPacket::getControlleeUUID(int32_t& val0, int32_t& val1, int32_t& val2, int32_t& val3) const {
  getControlUUID(false, val0, val1, val2, val3); // (controller=false)
}
void BasicCommandPacket::setControlleeUUID(int32_t val0, int32_t val1, int32_t val2, int32_t val3) {
  setControlUUID(false, val0, val1, val2, val3); // (controller=false)
}
std::string BasicCommandPacket::getControlleeUUID() const {
  return getControlUUID(false); // (controller=false)
}
void BasicCommandPacket::setControlleeUUID(std::string val) {
  setControlUUID(false, val); // (controller=false)
}

// Controller ID/UUID
int32_t BasicCommandPacket::getControllerIDNumber() const {
  return getControlIDNumber(true); // (controller=true)
}
void BasicCommandPacket::setControllerIDNumber(int32_t val) {
  setControlIDNumber(true, val); // (controller=true)
}

void BasicCommandPacket::getControllerUUID(int32_t& val0, int32_t& val1, int32_t& val2, int32_t& val3) const {
  getControlUUID(true, val0, val1, val2, val3); // (controller=true)
}
void BasicCommandPacket::setControllerUUID(int32_t val0, int32_t val1, int32_t val2, int32_t val3) {
  setControlUUID(true, val0, val1, val2, val3); // (controller=true)
}
std::string BasicCommandPacket::getControllerUUID() const {
  return getControlUUID(true); // (controller=true)
}
void BasicCommandPacket::setControllerUUID(std::string val) {
  setControlUUID(true, val); // (controller=true)
}


// Helpers
void BasicCommandPacket::setCtrlAckSettingsBit (int32_t bit, bool set) {
  int32_t field = getCtrlAckSettingsField();
  int32_t val;

  if (set) val = field | bit;
  else     val = field & ~bit;

  if (val != field) {
    VRTMath::packInt(bbuf, getHeaderLength(), val);
  }
}

int32_t BasicCommandPacket::getOffset (bool controller) const {
  int32_t off = 8; // both fall immediately after CtrlAckSettings(4) and MessageID(4)
  if (!controller) { // controllee
    return (isControlleeEnable()) ? off : -off;
  }
  else { // controller
    // if controllee, controller is immediately after controllee
    if (isControlleeEnable()) { // account for size of controllee
      off += getControlleeFormat() ? 16 : 4;
    }
    return (isControllerEnable()) ? off : -off;
  }
}

int32_t BasicCommandPacket::getFieldLen(bool controller) const {
  if (!controller) {
    return (getControlleeFormat() ? 16 : 4);
  }
  else {
    return (getControllerFormat() ? 16 : 4);
  }
}

int32_t BasicCommandPacket::getControlIDNumber (bool controller) const {
  int32_t off = getOffset(controller);
  if (off<0) return INT32_NULL;
  if (getFieldLen(controller)==4) { // ID
    return VRTMath::unpackInt(bbuf, getHeaderLength()+off);
  }
  else { // UUID
    throw VRTException("Cannot get ID Number when Format=UUID.");
  }
}

void BasicCommandPacket::getControlUUID (bool controller,
                         int32_t& val0, int32_t& val1, int32_t& val2, int32_t& val3) const {
  int32_t off = getOffset(controller);
  if (off<0) {
    val0 = val1 = val2 = val3 = INT32_NULL;
    return;
  }
  if (getFieldLen(controller)==16) { // UUID
    val0 = VRTMath::unpackInt(bbuf, getHeaderLength()+off   );
    val1 = VRTMath::unpackInt(bbuf, getHeaderLength()+off+4 );
    val2 = VRTMath::unpackInt(bbuf, getHeaderLength()+off+8 );
    val3 = VRTMath::unpackInt(bbuf, getHeaderLength()+off+12);
  }
  else { // ID Number
    throw VRTException("Cannot get UUID when Format=ID Number.");
  }
}

std::string BasicCommandPacket::getControlUUID (bool controller, char delim) const {
  if (getOffset(controller)<0) return "";
  int32_t val0, val1, val2, val3;
  getControlUUID(controller, val0, val1, val2, val3);
  std::stringstream ss;
  // X bytes at a time, where X = 4, 2, 2, 2, 6 (required width is 2X)
  ss<<std::uppercase<<std::setfill('0')<<std::hex<<std::setw(8)<< val0;                 if (delim>0) ss<< delim;
  ss<<std::uppercase<<std::setfill('0')<<std::hex<<std::setw(4)<< ((val0 >> 2) & 0xFF); if (delim>0) ss<< delim;
  ss<<std::uppercase<<std::setfill('0')<<std::hex<<std::setw(4)<< ((val0     ) & 0xFF); if (delim>0) ss<< delim;
  ss<<std::uppercase<<std::setfill('0')<<std::hex<<std::setw(4)<< ((val0 >> 2) & 0xFF); if (delim>0) ss<< delim;
  ss<<std::uppercase<<std::setfill('0')<<std::hex<<std::setw(4)<< ((val2     ) & 0xFF); // no delim this time
  ss<<std::uppercase<<std::setfill('0')<<std::hex<<std::setw(8)<< val3;
  return ss.str();
}

void BasicCommandPacket::setControlIDNumber (bool controller, int32_t val) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");
  
  int32_t off = getOffset(controller);
  bool add = !isNull(val);

  if (off<0 && !add) return; // already set to null

  int32_t oldLen = getFieldLen(controller);
  if (off>0 && add && oldLen==4) {
    // present, no change in size
    VRTMath::packInt(bbuf, getHeaderLength()+off, val);
  }
  else {
    if (off>0) {
      // remove existing ID or UUID
      if (!controller) setCtrlAckSettingsBit(CONTROL_CE_BIT, false);
      else             setCtrlAckSettingsBit(CONTROL_CR_BIT, false);
      shiftPacketSpecificPrologue(off, oldLen, false);
      off = -off;
    }

    if (add) {
      // add field and set value
      if (!controller) {
        setCtrlAckSettingsBit(CONTROL_CE_BIT, true);
        setCtrlAckSettingsBit(CONTROL_IE_BIT, false);
      }
      else {
        setCtrlAckSettingsBit(CONTROL_CR_BIT, true);
        setCtrlAckSettingsBit(CONTROL_IR_BIT, false);
      }
      off = shiftPacketSpecificPrologue(off, 4, true);
      VRTMath::packInt(bbuf, getHeaderLength()+off, val);
    }
  }
}

void BasicCommandPacket::setControlUUID (bool controller,
                         int32_t val0, int32_t val1, int32_t val2, int32_t val3) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");

  int32_t off = getOffset(controller);
  // XXX - for UUID to be NULL, require all 4 values to be NULL
  bool add = !(isNull(val0) && isNull(val1) && isNull(val2) && isNull(val3));

  if (off<0 && !add) return; // already set to null

  int32_t oldLen = getFieldLen(controller);
  if (off>0 && add && oldLen==16) {
    // present, no change in size
    VRTMath::packInt(bbuf, getHeaderLength()+off,    val0);
    VRTMath::packInt(bbuf, getHeaderLength()+off+4,  val1);
    VRTMath::packInt(bbuf, getHeaderLength()+off+8,  val2);
    VRTMath::packInt(bbuf, getHeaderLength()+off+12, val3);
  }
  else {
    if (off>0) {
      // remove existing ID or UUID
      if (!controller) setCtrlAckSettingsBit(CONTROL_CE_BIT, false);
      else             setCtrlAckSettingsBit(CONTROL_CR_BIT, false);
      shiftPacketSpecificPrologue(off, oldLen, false);
      off = -off;
    }

    if (add) {
      // add field and set value
      if (!controller) {
        setCtrlAckSettingsBit(CONTROL_CE_BIT, true);
        setCtrlAckSettingsBit(CONTROL_IE_BIT, true);
      }
      else {
        setCtrlAckSettingsBit(CONTROL_CR_BIT, true);
        setCtrlAckSettingsBit(CONTROL_IR_BIT, true);
      }
      off = shiftPacketSpecificPrologue(off, 16, true);
      VRTMath::packInt(bbuf, getHeaderLength()+off,    val0);
      VRTMath::packInt(bbuf, getHeaderLength()+off+4,  val1);
      VRTMath::packInt(bbuf, getHeaderLength()+off+8,  val2);
      VRTMath::packInt(bbuf, getHeaderLength()+off+12, val3);
    }
  }
}

void BasicCommandPacket::setControlUUID (bool controller, std::string val) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");
  int32_t val0, val1, val2, val3;
  // XXX - for string UUID to be NULL, require empty string
  if (val.empty()) {
    val0 = val1 = val2 = val3 = INT32_NULL;
  } else {

    // Valid string with delimiters will have length 36
    if (val.size() == 36) {
      // remove delimiters
      // form: 12345678-1234-1234-1234-123456789ABC
      val.erase(val.begin()+8 );  
      val.erase(val.begin()+12);
      val.erase(val.begin()+16);
      val.erase(val.begin()+20);
    }

    // Valid string with no delimiters will have length 32
    if (val.size() != 32) {
      throw VRTException("Invalid length of UUID string.");
    }

    // form: 12345678123412341234123456789ABC
    // TODO - does this work?
    std::stringstream ss;
    ss << std::hex << std::string(val,  0, 8); ss >> val0;
    ss << std::hex << std::string(val,  8, 8); ss >> val1;
    ss << std::hex << std::string(val, 16, 8); ss >> val2;
    ss << std::hex << std::string(val, 24, 8); ss >> val3;
  }
  setControlUUID(controller, val0, val1, val2, val3);
}


/* Initial implementation - preserved for debug if there are errors
void BasicCommandPacket::setControlleeIDNumber(int32_t val) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");
  
  // XXX this is bytes offset from start of PSP (i.e. end of header)
  // // 8 for CtrlAckSettingsField(4) and MessageID(4)
  int32_t off = 8 * (isControlleeEnable())? 1 : -1;
  bool add = !isNull(val);

  if (off<0 && !add) return; // already set to null

  if (off>0 && add && !getControlleeFormat()) {
    // present, no change in size
    VRTMath::packInt(bbuf, getHeaderLength()+off, val)
  }
  else {
    if (off>0) {
      // remove existing ID or UUID
      setControlleeEnable(false);
      shiftPacketSpecificPrologue(off, (getControlleeFormat()? 16 : 4), false);
      off = -off;
    }

    if (add) {
      // add field and set value
      setControlleeEnable(true);
      setControlleeFormat(false); // ID, not UUID
      off = shiftPacketSpecificPrologue(off, 4, true);
      VRTMath::packInt(bbuf, getHeaderLength()+off, val);
    }
  }
}
*/


/** TODO - implement HasFields methods for BasicCommandPacket
    public: virtual int32_t   getFieldCount () const;
    public: virtual string    getFieldName  (int32_t id) const;
    public: virtual ValueType getFieldType  (int32_t id) const;
    public: virtual Value*    getField      (int32_t id) const __attribute__((warn_unused_result));
    public: virtual void      setField      (int32_t id, const Value* val);
*/



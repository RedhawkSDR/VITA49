//
//  This class was auto-generated, do not modify.
//
#include "StreamStatePacket.h"

using namespace std;
using namespace vrt;

//======================================================================
// ENUMERATIONS
//======================================================================
string operator+(string &s, SystemState val) {
  ostringstream str;
  str << s << val;
  return str.str();
}

ostream& operator<<(ostream &s, SystemState val) {
  switch (val) {
    case 0: return s << "null";
    case 1: return s << "BOOTING";
    case 2: return s << "INITIALIZING";
    case 3: return s << "STARTING";
    case 4: return s << "RUNNING";
    case 5: return s << "STOPPING";
    case 6: return s << "EXITING";
    case 7: return s << "PAUSING";
    case 8: return s << "BREAKING";
    case 9: return s << "RESTARTING";
    case 10: return s << "ABORTING";
    case 11: return s << "TERMINATING";
    case 12: return s << "IDENTIFYING";
    case 13: return s << "CONFIGURING";
    case 14: return s << "PROCESSING";
    default: return s << "Unknown SystemState " << (int32_t)val;
  }
}
string operator+(string &s, SystemStatus val) {
  ostringstream str;
  str << s << val;
  return str.str();
}

ostream& operator<<(ostream &s, SystemStatus val) {
  switch (val) {
    case 0: return s << "null";
    case 1: return s << "RED";
    case 2: return s << "YELLOW";
    case 3: return s << "GREEN";
    case 4: return s << "BLUE";
    case 5: return s << "GRAY";
    default: return s << "Unknown SystemStatus " << (int32_t)val;
  }
}

//======================================================================
// NON-CLASS METHODS
//======================================================================
/** Creates the default header for the packet. This is defined separate
 *  from the class to account for the systems/compilers that disallow
 *  calling of subclass methods from the constructor.
 */
static inline vector<char> StreamStatePacket_createDefaultPacket () {
  vector<char> buf(36);
  buf[0] = (char)88;
  buf[1] = (char)-96;
  buf[2] = (char)0;
  buf[3] = (char)9;
  buf[4] = (char)0;
  buf[5] = (char)0;
  buf[6] = (char)0;
  buf[7] = (char)0;
  buf[8] = (char)0;
  buf[9] = (char)-1;
  buf[10] = (char)-1;
  buf[11] = (char)-6;
  buf[12] = (char)32;
  buf[13] = (char)17;
  buf[14] = (char)0;
  buf[15] = (char)5;
  buf[16] = (char)0;
  buf[17] = (char)0;
  buf[18] = (char)0;
  buf[19] = (char)0;
  buf[20] = (char)0;
  buf[21] = (char)0;
  buf[22] = (char)0;
  buf[23] = (char)0;
  buf[24] = (char)0;
  buf[25] = (char)0;
  buf[26] = (char)0;
  buf[27] = (char)0;
  return buf;
}

//======================================================================
// PACKET TYPE STATIC CONSTANTS
//======================================================================
const PacketType StreamStatePacket::PACKET_TYPE = PacketType_ExtContext;
const int32_t    StreamStatePacket::PACKET_LENGTH = -8;
const int64_t    StreamStatePacket::CLASS_IDENTIFIER = __INT64_C(0xFFFFFA20110005);
const string     StreamStatePacket::CLASS_ID("FF-FF-FA:2011.0005");

//======================================================================
// PACKET TYPE CONSTRUCTORS
//======================================================================
StreamStatePacket::StreamStatePacket () :
  BasicVRTPacket(StreamStatePacket_createDefaultPacket(), false)
{
 // done
}

StreamStatePacket::StreamStatePacket (const vector<char> &bbuf, bool readOnly) :
  BasicVRTPacket(bbuf, readOnly)
{
 // done
}

StreamStatePacket::StreamStatePacket (const BasicVRTPacket &p) :
  BasicVRTPacket(p, PACKET_TYPE, CLASS_IDENTIFIER, 8, PACKET_LENGTH)
{
 // done
}

StreamStatePacket::StreamStatePacket (const BasicVRTPacket &p, PacketType type,
                       int64_t classID, int32_t minPayloadLength, int32_t maxPayloadLength) :
  BasicVRTPacket(p, type, classID, minPayloadLength, maxPayloadLength)
{
 // done
}

//======================================================================
// UTILITY METHODS
//======================================================================
string StreamStatePacket::getPacketValid (bool strict, int32_t length) const {
  string err = BasicVRTPacket::getPacketValid(strict, length);
  if (err != "") return err;
  
  bool okType = (getPacketType() == PACKET_TYPE);
  bool okID   = (getClassIdentifier() == CLASS_IDENTIFIER);
  
  if (!okType) {
    ostringstream str;
    str << "Invalid use of " << getClassName() << " with " << getPacketType() << " packet.";
    return str.str();
  }
  if (!okID) {
    ostringstream str;
    str << "Invalid use of " << getClassName() << " with ClassID=" << getClassID() << ".";
    return str.str();
  }
  return "";
}

void StreamStatePacket::toStringStream (ostringstream &str) const {
  BasicVRTPacket::toStringStream(str);
  str << " SystemState=" << getSystemState();
  str << " AggregateSystemStatus=" << getAggregateSystemStatus();
  str << " InputStatus=" << getInputStatus();
  str << " InternalStatus=" << getInternalStatus();
  str << " OutputStatus=" << getOutputStatus();
  str << " Message='" << getMessage() << "'";
}

int32_t StreamStatePacket::getFieldCount () const {
  return BasicVRTPacket::getFieldCount() + 6;
}

string StreamStatePacket::getFieldName (int32_t id) const {
  switch (id - BasicVRTPacket::getFieldCount()) {
    case 0: return "SystemState";
    case 1: return "AggregateSystemStatus";
    case 2: return "InputStatus";
    case 3: return "InternalStatus";
    case 4: return "OutputStatus";
    case 5: return "Message";
    default: return BasicVRTPacket::getFieldName(id);
  }
}

ValueType StreamStatePacket::getFieldType (int32_t id) const {
  switch (id - BasicVRTPacket::getFieldCount()) {
    case 0: return ValueType_Int8;
    case 1: return ValueType_Int8;
    case 2: return ValueType_Int8;
    case 3: return ValueType_Int8;
    case 4: return ValueType_Int8;
    case 5: return ValueType_WString;
    default: return BasicVRTPacket::getFieldType(id);
  }
}

Value* StreamStatePacket::getField (int32_t id) const {
  switch (id - BasicVRTPacket::getFieldCount()) {
    case 0: return new Value((int8_t)getSystemState());
    case 1: return new Value((int8_t)getAggregateSystemStatus());
    case 2: return new Value((int8_t)getInputStatus());
    case 3: return new Value((int8_t)getInternalStatus());
    case 4: return new Value((int8_t)getOutputStatus());
    case 5: return new Value(getMessage());
    default: return BasicVRTPacket::getField(id);
  }
}

void StreamStatePacket::setField (int32_t id, const Value* val) {
  switch (id - BasicVRTPacket::getFieldCount()) {
    case 0: setSystemState((SystemState)val->as<int8_t>()); break;
    case 1: setAggregateSystemStatus((SystemStatus)val->as<int8_t>()); break;
    case 2: setInputStatus((SystemStatus)val->as<int8_t>()); break;
    case 3: setInternalStatus((SystemStatus)val->as<int8_t>()); break;
    case 4: setOutputStatus((SystemStatus)val->as<int8_t>()); break;
    case 5: setMessage(val->as<wstring>()); break;
    default: BasicVRTPacket::setField(id,val); break;
  }
}

//======================================================================
// GET/SET METHODS
//======================================================================


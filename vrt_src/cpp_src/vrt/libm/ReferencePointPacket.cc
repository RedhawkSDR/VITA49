//
//  This class was auto-generated, do not modify.
//
#include "ReferencePointPacket.h"

using namespace std;
using namespace vrt;


//======================================================================
// NON-CLASS METHODS
//======================================================================
/** Creates the default header for the packet. This is defined separate
 *  from the class to account for the systems/compilers that disallow
 *  calling of subclass methods from the constructor.
 */
static inline vector<char> ReferencePointPacket_createDefaultPacket () {
  vector<char> buf(68);
  buf[0] = (char)88;
  buf[1] = (char)-96;
  buf[2] = (char)0;
  buf[3] = (char)17;
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
  buf[15] = (char)6;
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
const PacketType ReferencePointPacket::PACKET_TYPE = PacketType_ExtContext;
const int32_t    ReferencePointPacket::PACKET_LENGTH = -40;
const int64_t    ReferencePointPacket::CLASS_IDENTIFIER = __INT64_C(0xFFFFFA20110006);
const string     ReferencePointPacket::CLASS_ID("FF-FF-FA:2011.0006");

//======================================================================
// PACKET TYPE CONSTRUCTORS
//======================================================================
ReferencePointPacket::ReferencePointPacket () :
  BasicVRTPacket(ReferencePointPacket_createDefaultPacket(), false)
{
 // done
}

ReferencePointPacket::ReferencePointPacket (const vector<char> &bbuf, bool readOnly) :
  BasicVRTPacket(bbuf, readOnly)
{
 // done
}

ReferencePointPacket::ReferencePointPacket (const BasicVRTPacket &p) :
  BasicVRTPacket(p, PACKET_TYPE, CLASS_IDENTIFIER, 40, PACKET_LENGTH)
{
 // done
}

ReferencePointPacket::ReferencePointPacket (const BasicVRTPacket &p, PacketType type,
                       int64_t classID, int32_t minPayloadLength, int32_t maxPayloadLength) :
  BasicVRTPacket(p, type, classID, minPayloadLength, maxPayloadLength)
{
 // done
}

//======================================================================
// UTILITY METHODS
//======================================================================
string ReferencePointPacket::getPacketValid (bool strict, int32_t length) const {
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

void ReferencePointPacket::toStringStream (ostringstream &str) const {
  BasicVRTPacket::toStringStream(str);
  str << " RefPointID=" << getRefPointID();
  str << " Name='" << getName() << "'";
  str << " Description='" << getDescription() << "'";
}

int32_t ReferencePointPacket::getFieldCount () const {
  return BasicVRTPacket::getFieldCount() + 3;
}

string ReferencePointPacket::getFieldName (int32_t id) const {
  switch (id - BasicVRTPacket::getFieldCount()) {
    case 0: return "RefPointID";
    case 1: return "Name";
    case 2: return "Description";
    default: return BasicVRTPacket::getFieldName(id);
  }
}

ValueType ReferencePointPacket::getFieldType (int32_t id) const {
  switch (id - BasicVRTPacket::getFieldCount()) {
    case 0: return ValueType_Int32;
    case 1: return ValueType_String;
    case 2: return ValueType_WString;
    default: return BasicVRTPacket::getFieldType(id);
  }
}

Value* ReferencePointPacket::getField (int32_t id) const {
  switch (id - BasicVRTPacket::getFieldCount()) {
    case 0: return new Value(getRefPointID());
    case 1: return new Value(getName());
    case 2: return new Value(getDescription());
    default: return BasicVRTPacket::getField(id);
  }
}

void ReferencePointPacket::setField (int32_t id, const Value* val) {
  switch (id - BasicVRTPacket::getFieldCount()) {
    case 0: setRefPointID(val->as<int32_t>()); break;
    case 1: setName(val->as<string>()); break;
    case 2: setDescription(val->as<wstring>()); break;
    default: BasicVRTPacket::setField(id,val); break;
  }
}

//======================================================================
// GET/SET METHODS
//======================================================================


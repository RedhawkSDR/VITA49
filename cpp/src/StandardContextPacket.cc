//
//  This class was auto-generated, do not modify.
//
#include "StandardContextPacket.h"

using namespace std;
using namespace vrt;


//======================================================================
// NON-CLASS METHODS
//======================================================================
/** Creates the default header for the packet. This is defined separate
 *  from the class to account for the systems/compilers that disallow
 *  calling of subclass methods from the constructor.
 */
static inline vector<char> StandardContextPacket_createDefaultPacket () {
  vector<char> buf(32);
  buf[0] = (char)72;
  buf[1] = (char)-96;
  buf[2] = (char)0;
  buf[3] = (char)8;
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
  buf[15] = (char)3;
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
const PacketType StandardContextPacket::PACKET_TYPE = PacketType_Context;
const int32_t    StandardContextPacket::PACKET_LENGTH = -4;
const int64_t    StandardContextPacket::CLASS_IDENTIFIER = __INT64_C(0xFFFFFA20110003);
const string     StandardContextPacket::CLASS_ID("FF-FF-FA:2011.0003");

//======================================================================
// PACKET TYPE CONSTRUCTORS
//======================================================================
StandardContextPacket::StandardContextPacket () :
  BasicContextPacket(StandardContextPacket_createDefaultPacket(), false), BasicVRTPacket(StandardContextPacket_createDefaultPacket(), false)
{
 // done
}

StandardContextPacket::StandardContextPacket (const vector<char> &_bbuf, bool _readOnly) :
  BasicContextPacket(_bbuf, _readOnly)
{
 // done
}

StandardContextPacket::StandardContextPacket (const BasicVRTPacket &p) :
  BasicContextPacket(p, PACKET_TYPE, CLASS_IDENTIFIER, 4, PACKET_LENGTH)
{
 // done
}

StandardContextPacket::StandardContextPacket (const BasicVRTPacket &p, PacketType type,
                       int64_t classID, int32_t minPayloadLength, int32_t maxPayloadLength) :
  BasicContextPacket(p, type, classID, minPayloadLength, maxPayloadLength)
{
 // done
}

//======================================================================
// UTILITY METHODS
//======================================================================
string StandardContextPacket::getPacketValid (bool strict, int32_t length) const {
  string err = BasicContextPacket::getPacketValid(strict, length);
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

void StandardContextPacket::toStringStream (ostringstream &str) const {
  BasicContextPacket::toStringStream(str);
}

int32_t StandardContextPacket::getFieldCount () const {
  return BasicContextPacket::getFieldCount() + 0;
}

string StandardContextPacket::getFieldName (int32_t id) const {
  switch (id - BasicContextPacket::getFieldCount()) {
    default: return BasicContextPacket::getFieldName(id);
  }
}

ValueType StandardContextPacket::getFieldType (int32_t id) const {
  switch (id - BasicContextPacket::getFieldCount()) {
    default: return BasicContextPacket::getFieldType(id);
  }
}

Value* StandardContextPacket::getField (int32_t id) const {
  switch (id - BasicContextPacket::getFieldCount()) {
    default: return BasicContextPacket::getField(id);
  }
}

void StandardContextPacket::setField (int32_t id, const Value* val) {
  switch (id - BasicContextPacket::getFieldCount()) {
    default: BasicContextPacket::setField(id,val); break;
  }
}

//======================================================================
// GET/SET METHODS
//======================================================================


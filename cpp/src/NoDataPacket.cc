//
//  This class was auto-generated, do not modify.
//
#include "NoDataPacket.h"

using namespace std;
using namespace vrt;


//======================================================================
// NON-CLASS METHODS
//======================================================================
/** Creates the default header for the packet. This is defined separate
 *  from the class to account for the systems/compilers that disallow
 *  calling of subclass methods from the constructor.
 */
static inline vector<char> NoDataPacket_createDefaultPacket () {
  vector<char> buf(32);
  buf[0] = (char)44;
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
  buf[15] = (char)2;
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
const PacketType NoDataPacket::PACKET_TYPE = PacketType_UnidentifiedExtData;
const int32_t    NoDataPacket::PACKET_LENGTH = 0;
const int64_t    NoDataPacket::CLASS_IDENTIFIER = __INT64_C(0xFFFFFA20110002);
const string     NoDataPacket::CLASS_ID("FF-FF-FA:2011.0002");

//======================================================================
// PACKET TYPE CONSTRUCTORS
//======================================================================
NoDataPacket::NoDataPacket () :
  BasicDataPacket(NoDataPacket_createDefaultPacket(), false)
{
 // done
}

NoDataPacket::NoDataPacket (const vector<char> &_bbuf, bool _readOnly) :
  BasicDataPacket(_bbuf, _readOnly)
{
 // done
}

NoDataPacket::NoDataPacket (const BasicVRTPacket &p) :
  BasicDataPacket(p, PACKET_TYPE, CLASS_IDENTIFIER, 0, PACKET_LENGTH)
{
 // done
}

NoDataPacket::NoDataPacket (const BasicVRTPacket &p, PacketType type,
                       int64_t classID, int32_t minPayloadLength, int32_t maxPayloadLength) :
  BasicDataPacket(p, type, classID, minPayloadLength, maxPayloadLength)
{
 // done
}

//======================================================================
// UTILITY METHODS
//======================================================================
string NoDataPacket::getPacketValid (bool strict, int32_t length) const {
  string err = BasicDataPacket::getPacketValid(strict, length);
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

void NoDataPacket::toStringStream (ostringstream &str) const {
  BasicDataPacket::toStringStream(str);
}

int32_t NoDataPacket::getFieldCount () const {
  return BasicDataPacket::getFieldCount() + 0;
}

string NoDataPacket::getFieldName (int32_t id) const {
  switch (id - BasicDataPacket::getFieldCount()) {
    default: return BasicDataPacket::getFieldName(id);
  }
}

ValueType NoDataPacket::getFieldType (int32_t id) const {
  switch (id - BasicDataPacket::getFieldCount()) {
    default: return BasicDataPacket::getFieldType(id);
  }
}

Value* NoDataPacket::getField (int32_t id) const {
  switch (id - BasicDataPacket::getFieldCount()) {
    default: return BasicDataPacket::getField(id);
  }
}

void NoDataPacket::setField (int32_t id, const Value* val) {
  switch (id - BasicDataPacket::getFieldCount()) {
    default: BasicDataPacket::setField(id,val); break;
  }
}

//======================================================================
// GET/SET METHODS
//======================================================================
//======================================================================
// TRAILER
//======================================================================


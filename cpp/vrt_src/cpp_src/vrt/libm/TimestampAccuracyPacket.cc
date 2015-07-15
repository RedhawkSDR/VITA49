//
//  This class was auto-generated, do not modify.
//
#include "TimestampAccuracyPacket.h"

using namespace std;
using namespace vrt;


//======================================================================
// NON-CLASS METHODS
//======================================================================
/** Creates the default header for the packet. This is defined separate
 *  from the class to account for the systems/compilers that disallow
 *  calling of subclass methods from the constructor.
 */
static inline vector<char> TimestampAccuracyPacket_createDefaultPacket () {
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
  buf[15] = (char)4;
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
const PacketType TimestampAccuracyPacket::PACKET_TYPE = PacketType_ExtContext;
const int32_t    TimestampAccuracyPacket::PACKET_LENGTH = 8;
const int64_t    TimestampAccuracyPacket::CLASS_IDENTIFIER = __INT64_C(0xFFFFFA20110004);
const string     TimestampAccuracyPacket::CLASS_ID("FF-FF-FA:2011.0004");

//======================================================================
// PACKET TYPE CONSTRUCTORS
//======================================================================
TimestampAccuracyPacket::TimestampAccuracyPacket () :
  BasicVRTPacket(TimestampAccuracyPacket_createDefaultPacket(), false)
{
 // done
}

TimestampAccuracyPacket::TimestampAccuracyPacket (const vector<char> &bbuf, bool readOnly) :
  BasicVRTPacket(bbuf, readOnly)
{
 // done
}

TimestampAccuracyPacket::TimestampAccuracyPacket (const BasicVRTPacket &p) :
  BasicVRTPacket(p, PACKET_TYPE, CLASS_IDENTIFIER, 8, PACKET_LENGTH)
{
 // done
}

TimestampAccuracyPacket::TimestampAccuracyPacket (const BasicVRTPacket &p, PacketType type,
                       int64_t classID, int32_t minPayloadLength, int32_t maxPayloadLength) :
  BasicVRTPacket(p, type, classID, minPayloadLength, maxPayloadLength)
{
 // done
}

//======================================================================
// UTILITY METHODS
//======================================================================
string TimestampAccuracyPacket::getPacketValid (bool strict, int32_t length) const {
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

void TimestampAccuracyPacket::toStringStream (ostringstream &str) const {
  BasicVRTPacket::toStringStream(str);
  str << " TimestampAccuracy=" << getTimestampAccuracy();
}

int32_t TimestampAccuracyPacket::getFieldCount () const {
  return BasicVRTPacket::getFieldCount() + 1;
}

string TimestampAccuracyPacket::getFieldName (int32_t id) const {
  switch (id - BasicVRTPacket::getFieldCount()) {
    case 0: return "TimestampAccuracy";
    default: return BasicVRTPacket::getFieldName(id);
  }
}

ValueType TimestampAccuracyPacket::getFieldType (int32_t id) const {
  switch (id - BasicVRTPacket::getFieldCount()) {
    case 0: return ValueType_Int64;
    default: return BasicVRTPacket::getFieldType(id);
  }
}

Value* TimestampAccuracyPacket::getField (int32_t id) const {
  switch (id - BasicVRTPacket::getFieldCount()) {
    case 0: return new Value(getTimestampAccuracy());
    default: return BasicVRTPacket::getField(id);
  }
}

void TimestampAccuracyPacket::setField (int32_t id, const Value* val) {
  switch (id - BasicVRTPacket::getFieldCount()) {
    case 0: setTimestampAccuracy(val->as<int64_t>()); break;
    default: BasicVRTPacket::setField(id,val); break;
  }
}

//======================================================================
// GET/SET METHODS
//======================================================================


//
//  This class was auto-generated, do not modify.
//
#include "EphemerisPacket.h"

using namespace std;
using namespace vrt;

//======================================================================
// ENUMERATIONS
//======================================================================
string operator+(string &s, FixType val) {
  ostringstream str;
  str << s << val;
  return str.str();
}

ostream& operator<<(ostream &s, FixType val) {
  switch (val) {
    case -19: return s << "PREDICTED_EPHEMERIS_9";
    case -18: return s << "PREDICTED_EPHEMERIS_8";
    case -17: return s << "PREDICTED_EPHEMERIS_7";
    case -16: return s << "PREDICTED_EPHEMERIS_6";
    case -15: return s << "PREDICTED_EPHEMERIS_5";
    case -14: return s << "PREDICTED_EPHEMERIS_4";
    case -13: return s << "PREDICTED_EPHEMERIS_3";
    case -12: return s << "PREDICTED_EPHEMERIS_2";
    case -11: return s << "PREDICTED_EPHEMERIS_1";
    case -10: return s << "PREDICTED_EPHEMERIS";
    case -3: return s << "PREDICTED_INS";
    case -2: return s << "PREDICTED_GPS";
    case -1: return s << "PREDICTED";
    case 0: return s << "null";
    case 1: return s << "ACTUAL";
    case 2: return s << "ACTUAL_GPS";
    case 3: return s << "ACTUAL_INS";
    case 10: return s << "ACTUAL_EPHEMERIS";
    case 11: return s << "ACTUAL_EPHEMERIS_1";
    case 12: return s << "ACTUAL_EPHEMERIS_2";
    case 13: return s << "ACTUAL_EPHEMERIS_3";
    case 14: return s << "ACTUAL_EPHEMERIS_4";
    case 15: return s << "ACTUAL_EPHEMERIS_5";
    case 16: return s << "ACTUAL_EPHEMERIS_6";
    case 17: return s << "ACTUAL_EPHEMERIS_7";
    case 18: return s << "ACTUAL_EPHEMERIS_8";
    case 19: return s << "ACTUAL_EPHEMERIS_9";
    default: return s << "Unknown FixType " << (int32_t)val;
  }
}
//======================================================================
// RECORDS
//======================================================================

const int32_t Point::RECORD_LENGTH = 72;

Point::Point () : Record(72) { }
Point::Point (const Record &r) : Record(r) { }

string Point::toString () const {
  ostringstream str;
  str << Record::toString();
  str << "PositionX=" << getPositionX();
  str << " PositionY=" << getPositionY();
  str << " PositionZ=" << getPositionZ();
  str << " VelocityX=" << getVelocityX();
  str << " VelocityY=" << getVelocityY();
  str << " VelocityZ=" << getVelocityZ();
  str << " AccelerationX=" << getAccelerationX();
  str << " AccelerationY=" << getAccelerationY();
  str << " AccelerationZ=" << getAccelerationZ();
  str << " AttitudeAlpha=" << getAttitudeAlpha();
  str << " AttitudeBeta=" << getAttitudeBeta();
  str << " AttitudePhi=" << getAttitudePhi();
  return str.str();
}

int32_t Point::getFieldCount () const {
  return Record::getFieldCount() + 12;
}

string Point::getFieldName (int32_t id) const {
  switch (id - Record::getFieldCount()) {
    case 0: return "PositionX";
    case 1: return "PositionY";
    case 2: return "PositionZ";
    case 3: return "VelocityX";
    case 4: return "VelocityY";
    case 5: return "VelocityZ";
    case 6: return "AccelerationX";
    case 7: return "AccelerationY";
    case 8: return "AccelerationZ";
    case 9: return "AttitudeAlpha";
    case 10: return "AttitudeBeta";
    case 11: return "AttitudePhi";
    default: return Record::getFieldName(id);
  }
}

ValueType Point::getFieldType (int32_t id) const {
  switch (id - Record::getFieldCount()) {
    case 0: return ValueType_Double;
    case 1: return ValueType_Double;
    case 2: return ValueType_Double;
    case 3: return ValueType_Double;
    case 4: return ValueType_Double;
    case 5: return ValueType_Double;
    case 6: return ValueType_Float;
    case 7: return ValueType_Float;
    case 8: return ValueType_Float;
    case 9: return ValueType_Float;
    case 10: return ValueType_Float;
    case 11: return ValueType_Float;
    default: return Record::getFieldType(id);
  }
}

Value* Point::getField (int32_t id) const {
  switch (id - Record::getFieldCount()) {
    case 0: return new Value(getPositionX());
    case 1: return new Value(getPositionY());
    case 2: return new Value(getPositionZ());
    case 3: return new Value(getVelocityX());
    case 4: return new Value(getVelocityY());
    case 5: return new Value(getVelocityZ());
    case 6: return new Value(getAccelerationX());
    case 7: return new Value(getAccelerationY());
    case 8: return new Value(getAccelerationZ());
    case 9: return new Value(getAttitudeAlpha());
    case 10: return new Value(getAttitudeBeta());
    case 11: return new Value(getAttitudePhi());
    default: return Record::getField(id);
  }
}

void Point::setField (int32_t id, const Value* val) {
  switch (id - Record::getFieldCount()) {
    case 0: setPositionX(val->as<double>()); break;
    case 1: setPositionY(val->as<double>()); break;
    case 2: setPositionZ(val->as<double>()); break;
    case 3: setVelocityX(val->as<double>()); break;
    case 4: setVelocityY(val->as<double>()); break;
    case 5: setVelocityZ(val->as<double>()); break;
    case 6: setAccelerationX(val->as<float>()); break;
    case 7: setAccelerationY(val->as<float>()); break;
    case 8: setAccelerationZ(val->as<float>()); break;
    case 9: setAttitudeAlpha(val->as<float>()); break;
    case 10: setAttitudeBeta(val->as<float>()); break;
    case 11: setAttitudePhi(val->as<float>()); break;
    default: Record::setField(id,val); break;
  }
}


//======================================================================
// NON-CLASS METHODS
//======================================================================
/** Creates the default header for the packet. This is defined separate
 *  from the class to account for the systems/compilers that disallow
 *  calling of subclass methods from the constructor.
 */
static inline vector<char> EphemerisPacket_createDefaultPacket () {
  vector<char> buf(84);
  buf[0] = (char)88;
  buf[1] = (char)-96;
  buf[2] = (char)0;
  buf[3] = (char)21;
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
  buf[14] = (char)-20;
  buf[15] = (char)-17;
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
const PacketType EphemerisPacket::PACKET_TYPE = PacketType_ExtContext;
const int32_t    EphemerisPacket::PACKET_LENGTH = -56;
const int64_t    EphemerisPacket::CLASS_IDENTIFIER = __INT64_C(0xFFFFFA2011ECEF);
const string     EphemerisPacket::CLASS_ID("FF-FF-FA:2011.ECEF");

//======================================================================
// PACKET TYPE CONSTRUCTORS
//======================================================================
EphemerisPacket::EphemerisPacket () :
  BasicVRTPacket(EphemerisPacket_createDefaultPacket(), false)
{
 // done
}

EphemerisPacket::EphemerisPacket (const vector<char> &_bbuf, bool _readOnly) :
  BasicVRTPacket(_bbuf, _readOnly)
{
 // done
}

EphemerisPacket::EphemerisPacket (const BasicVRTPacket &p) :
  BasicVRTPacket(p, PACKET_TYPE, CLASS_IDENTIFIER, 56, PACKET_LENGTH)
{
 // done
}

EphemerisPacket::EphemerisPacket (const BasicVRTPacket &p, PacketType type,
                       int64_t classID, int32_t minPayloadLength, int32_t maxPayloadLength) :
  BasicVRTPacket(p, type, classID, minPayloadLength, maxPayloadLength)
{
 // done
}

//======================================================================
// UTILITY METHODS
//======================================================================
string EphemerisPacket::getPacketValid (bool strict, int32_t length) const {
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

void EphemerisPacket::toStringStream (ostringstream &str) const {
  BasicVRTPacket::toStringStream(str);
  str << " ProducedBy='" << getProducedBy() << "'";
  str << " PosFixType=" << getPosFixType();
  str << " VelFixType=" << getVelFixType();
  str << " AccFixType=" << getAccFixType();
  str << " AttFixType=" << getAttFixType();
  str << " FixTime=[" << getFixTime().toString() << "]";
  str << " FixDelta=" << getFixDelta();
  { // Print up to 8 of the array entries
    str << " Points={";
    int32_t count = getPointCount();
    for (int32_t i = 0; (i < count) && (i < 8); i++) {
      if (i > 0) str << ", ";
      str << "[" << getPoint(i).toString() << "]";
    }
    if (count > 8) str << ", ...}";
    else           str << "}";
  }
}

int32_t EphemerisPacket::getFieldCount () const {
  return BasicVRTPacket::getFieldCount() + 8;
}

string EphemerisPacket::getFieldName (int32_t id) const {
  switch (id - BasicVRTPacket::getFieldCount()) {
    case 0: return "ProducedBy";
    case 1: return "PosFixType";
    case 2: return "VelFixType";
    case 3: return "AccFixType";
    case 4: return "AttFixType";
    case 5: return "FixTime";
    case 6: return "FixDelta";
    case 7: return "Points";
    default: return BasicVRTPacket::getFieldName(id);
  }
}

ValueType EphemerisPacket::getFieldType (int32_t id) const {
  switch (id - BasicVRTPacket::getFieldCount()) {
    case 0: return ValueType_String;
    case 1: return ValueType_Int8;
    case 2: return ValueType_Int8;
    case 3: return ValueType_Int8;
    case 4: return ValueType_Int8;
    case 5: return ValueType_VRTObject;
    case 6: return ValueType_Int64;
    case 7: return (ValueType)-ValueType_VRTObject;
    default: return BasicVRTPacket::getFieldType(id);
  }
}

Value* EphemerisPacket::getField (int32_t id) const {
  switch (id - BasicVRTPacket::getFieldCount()) {
    case 0: return new Value(getProducedBy());
    case 1: return new Value((int8_t)getPosFixType());
    case 2: return new Value((int8_t)getVelFixType());
    case 3: return new Value((int8_t)getAccFixType());
    case 4: return new Value((int8_t)getAttFixType());
    case 5: return new Value(new TimeStamp(getFixTime()), true);
    case 6: return new Value(getFixDelta());
    case 7: return new Value(_getAllPoints());
    default: return BasicVRTPacket::getField(id);
  }
}

void EphemerisPacket::setField (int32_t id, const Value* val) {
  switch (id - BasicVRTPacket::getFieldCount()) {
    case 0: setProducedBy(val->as<string>()); break;
    case 1: setPosFixType((FixType)val->as<int8_t>()); break;
    case 2: setVelFixType((FixType)val->as<int8_t>()); break;
    case 3: setAccFixType((FixType)val->as<int8_t>()); break;
    case 4: setAttFixType((FixType)val->as<int8_t>()); break;
    case 5: setFixTime(*val->cast<TimeStamp*>()); break;
    case 6: setFixDelta(val->as<int64_t>()); break;
    case 7: _setAllPoints(val); break;
    default: BasicVRTPacket::setField(id,val); break;
  }
}

//======================================================================
// GET/SET METHODS
//======================================================================
vector<Point> EphemerisPacket::getAllPoints () const {
  int32_t len = getPointCount();
  vector<Point> all(len);
  for (int32_t i = 0; i < len; i++) {
    all[i] = getPoint(i);
  }
  return all;
}

Point EphemerisPacket::getPoint (int32_t i) const {
  if ((i < 0) || (i > getPointCount())) {
    throw VRTException("Invalid index %d into Points", i);
  }
  int32_t off = 56 + (72 * i);
  Point r;
  unpackPayloadRecord(off, r);
  return r;
}

vector<VRTObject*>* EphemerisPacket::_getAllPoints () const {
  int32_t len = getPointCount();
  vector<VRTObject*>* all = new vector<VRTObject*>;
  for (int32_t i = 0; i < len; i++) {
    all->push_back(new Point(getPoint(i)));
  }
  return all;
}

void EphemerisPacket::setAllPoints (const vector<Point> &val) {
  setPointCount(val.size());
  for (size_t i = 0; i < val.size(); i++) {
    setPoint(i, val[i]);
  }
}

void EphemerisPacket::setPoint (int32_t i, const Point & val) {
  if ((i < 0) || (i > getPointCount())) {
    throw VRTException("Invalid index %d into Points", i);
  }
  int32_t off = 56 + (72 * i);
  packPayloadRecord(off, val);
}

void EphemerisPacket::setPointCount (int32_t length) {
  if (length < 0) {
    throw VRTException("Illegal length %d for Points", length);
  }
  int32_t len = ((length * 72) + 3) & 0x7FFFFFFC; // length to next multiple of 4
  setPayloadLength(56 + len);
}

void EphemerisPacket::_setAllPoints (const Value* val) {
  setPointCount(val->size());
  for (size_t i = 0; i < val->size(); i++) {
    Value *v = val->at(i);
    setPoint(i, *v->cast<Point*>());
    delete v;
  }
}



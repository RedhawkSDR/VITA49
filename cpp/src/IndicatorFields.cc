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

#include "IndicatorFields.h"
#include <bitset> // includes the popcount functions

using namespace vrt;
using namespace IndicatorFields;


ContextAssocLists::ContextAssocLists     ()                             : Record(8) { }
ContextAssocLists::ContextAssocLists     (const ContextAssocLists   &r) : Record(r) { }
AbstractGeolocation::AbstractGeolocation (size_t n)                     : Record(n) { }
AbstractGeolocation::AbstractGeolocation (const AbstractGeolocation &r) : Record(r) { }
Geolocation::Geolocation                 ()                             : AbstractGeolocation(44) { }
Geolocation::Geolocation                 (const Geolocation         &r) : AbstractGeolocation(r ) { }
GeoSentences::GeoSentences               ()                             : AbstractGeolocation(8 ) { }
GeoSentences::GeoSentences               (const GeoSentences        &r) : AbstractGeolocation(r ) { }

string AbstractGeolocation::toString () const {
  ostringstream str;
  str << Record::toString();
  Utilities::append(str,  "Manufacturer=", getManufacturerID());
  Utilities::append(str, " TimeStamp=",    getTimeStamp());
  return str.str();
}

TimeStamp AbstractGeolocation::getTimeStamp () const {
  int8_t         b       = unpackByte(0);
  IntegerMode    tsiMode = (IntegerMode)((b >> 2) & 0x3);
  FractionalMode tsfMode = (FractionalMode)(b & 0x3);
  uint32_t       tsi     = (uint32_t)unpackInt(4);
  uint64_t       tsf     = (uint64_t)unpackLong(8);

  return TimeStamp(tsiMode, tsfMode, tsi, tsf);
}

void AbstractGeolocation::setTimeStamp (const TimeStamp &ts) {
  int8_t b = (int8_t)((ts.getIntegerMode() << 2) | ts.getFractionalMode());
  packByte(0, b);

  if (ts.getIntegerMode() == IntegerMode_None) {
    packInt(4, 0xFFFFFFFF);
  }
  else {
    packInt(4, (int32_t)ts.getTimeStampInteger());
  }

  if (ts.getFractionalMode() == FractionalMode_None) {
    packInt( 8, 0xFFFFFFFF);
    packInt(12, 0xFFFFFFFF);
  }
  else {
    packLong(8, (int64_t)ts.getTimeStampFractional());
  }
}

int32_t AbstractGeolocation::getFieldCount () const {
  return Record::getFieldCount() + 2;
}

string AbstractGeolocation::getFieldName (int32_t id) const {
  switch (id - Record::getFieldCount()) {
    case  0: return "Manufacturer";
    case  1: return "TimeStamp";
    default: return Record::getFieldName(id);
  }
}

ValueType AbstractGeolocation::getFieldType (int32_t id) const {
  switch (id - Record::getFieldCount()) {
    case  0: return ValueType_Bool;
    case  1: return ValueType_VRTObject;
    default: return Record::getFieldType(id);
  }
}

Value* AbstractGeolocation::getField (int32_t id) const {
  switch (id - Record::getFieldCount()) {
    case  0: return new Value(getManufacturerID());
    case  1: return new Value(new TimeStamp(getTimeStamp()));
    default: return Record::getField(id);
  }
}

void AbstractGeolocation::setField (int32_t id, const Value* val) {
  switch (id - Record::getFieldCount()) {
    case  0: setManufacturerID(val->as<string>()); return;
    case  1: setTimeStamp(*val->cast<TimeStamp*>()); return;
    default: Record::setField(id,val); return;
  }
}

string Geolocation::toString () const {
  ostringstream str;
  str << AbstractGeolocation::toString();
  Utilities::append(str, " Latitude=",          getLatitude());
  Utilities::append(str, " Longitude=",         getLongitude());
  Utilities::append(str, " Altitude=",          getAltitude());
  Utilities::append(str, " SpeedOverGround=",   getSpeedOverGround());
  Utilities::append(str, " HeadingAngle=",      getHeadingAngle());
  Utilities::append(str, " TrackAngle=",        getTrackAngle());
  Utilities::append(str, " MagneticVariation=", getMagneticVariation());
  return str.str();
}

int32_t Geolocation::getFieldCount () const {
  return AbstractGeolocation::getFieldCount() + 7;
}

string Geolocation::getFieldName (int32_t id) const {
  switch (id - AbstractGeolocation::getFieldCount()) {
    case  0: return "Latitude";
    case  1: return "Longitude";
    case  2: return "Altitude";
    case  3: return "SpeedOverGround";
    case  4: return "HeadingAngle";
    case  5: return "TrackAngle";
    case  6: return "MagneticVariation";
    default: return AbstractGeolocation::getFieldName(id);
  }
}

ValueType Geolocation::getFieldType (int32_t id) const {
  switch (id - AbstractGeolocation::getFieldCount()) {
    case  0: return ValueType_Double;
    case  1: return ValueType_Double;
    case  2: return ValueType_Double;
    case  3: return ValueType_Double;
    case  4: return ValueType_Double;
    case  5: return ValueType_Double;
    case  6: return ValueType_Double;
    default: return AbstractGeolocation::getFieldType(id);
  }
}

Value* Geolocation::getField (int32_t id) const {
  switch (id - AbstractGeolocation::getFieldCount()) {
    case  0: return new Value(getLatitude());
    case  1: return new Value(getLongitude());
    case  2: return new Value(getAltitude());
    case  3: return new Value(getSpeedOverGround());
    case  4: return new Value(getHeadingAngle());
    case  5: return new Value(getTrackAngle());
    case  6: return new Value(getMagneticVariation());
    default: return AbstractGeolocation::getField(id);
  }
}

void Geolocation::setField (int32_t id, const Value* val) {
  switch (id - AbstractGeolocation::getFieldCount()) {
    case  0: setLatitude(         val->as<double>()); return;
    case  1: setLongitude(        val->as<double>()); return;
    case  2: setAltitude(         val->as<double>()); return;
    case  3: setSpeedOverGround(  val->as<double>()); return;
    case  4: setHeadingAngle(     val->as<double>()); return;
    case  5: setTrackAngle(       val->as<double>()); return;
    case  6: setMagneticVariation(val->as<double>()); return;
    default: AbstractGeolocation::setField(id,val); return;
  }
}

Ephemeris::Ephemeris () :
  AbstractGeolocation(52)
{
  // done
}

Ephemeris::Ephemeris (const Ephemeris &r) :
  AbstractGeolocation(r)
{
  // done
}

string Ephemeris::toString () const {
  ostringstream str;
  str << AbstractGeolocation::toString();
  Utilities::append(str, " PositionX=",     getPositionX());
  Utilities::append(str, " PositionY=",     getPositionY());
  Utilities::append(str, " PositionZ=",     getPositionZ());
  Utilities::append(str, " AttitudeAlpha=", getAttitudeAlpha());
  Utilities::append(str, " AttitudeBeta=",  getAttitudeBeta());
  Utilities::append(str, " AttitudePhi=",   getAttitudePhi());
  Utilities::append(str, " VelocityX=",     getVelocityX());
  Utilities::append(str, " VelocityY=",     getVelocityY());
  Utilities::append(str, " VelocityZ=",     getVelocityZ());
  return str.str();
}

int32_t Ephemeris::getFieldCount () const {
  return AbstractGeolocation::getFieldCount() + 18;
}

string Ephemeris::getFieldName (int32_t id) const {
  switch (id - AbstractGeolocation::getFieldCount()) {
    case  0: return "PositionX";
    case  1: return "PositionY";
    case  2: return "PositionZ";
    case  3: return "AttitudeAlpha";
    case  4: return "AttitudeBeta";
    case  5: return "AttitudePhi";
    case  6: return "VelocityX";
    case  7: return "VelocityY";
    case  8: return "VelocityZ";
    default: return AbstractGeolocation::getFieldName(id);
  }
}

ValueType Ephemeris::getFieldType (int32_t id) const {
  switch (id - AbstractGeolocation::getFieldCount()) {
    case  0: return ValueType_Double;
    case  1: return ValueType_Double;
    case  2: return ValueType_Double;
    case  3: return ValueType_Double;
    case  4: return ValueType_Double;
    case  5: return ValueType_Double;
    case  6: return ValueType_Double;
    case  7: return ValueType_Double;
    case  8: return ValueType_Double;
    default: return AbstractGeolocation::getFieldType(id);
  }
}

Value* Ephemeris::getField (int32_t id) const {
  switch (id - AbstractGeolocation::getFieldCount()) {
    case  0: return new Value(getPositionX());
    case  1: return new Value(getPositionY());
    case  2: return new Value(getPositionZ());
    case  3: return new Value(getAttitudeAlpha());
    case  4: return new Value(getAttitudeBeta());
    case  5: return new Value(getAttitudePhi());
    case  6: return new Value(getVelocityX());
    case  7: return new Value(getVelocityY());
    case  8: return new Value(getVelocityZ());
    default: return AbstractGeolocation::getField(id);
  }
}

void Ephemeris::setField (int32_t id, const Value* val) {
  switch (id - AbstractGeolocation::getFieldCount()) {
    case  0: setPositionX(                  val->as<double>()); return;
    case  1: setPositionY(                  val->as<double>()); return;
    case  2: setPositionZ(                  val->as<double>()); return;
    case  3: setAttitudeAlpha(              val->as<double>()); return;
    case  4: setAttitudeBeta(               val->as<double>()); return;
    case  5: setAttitudePhi(                val->as<double>()); return;
    case  6: setVelocityX(                  val->as<double>()); return;
    case  7: setVelocityY(                  val->as<double>()); return;
    case  8: setVelocityZ(                  val->as<double>()); return;
    default: AbstractGeolocation::setField(id,val); return;
  }
}

string GeoSentences::toString () const {
  ostringstream str;
  str << AbstractGeolocation::toString();
  str << " Sentences=" << getSentences();
  return str.str();
}

int32_t GeoSentences::getFieldCount () const {
  return AbstractGeolocation::getFieldCount() + 1;
}

string GeoSentences::getFieldName (int32_t id) const {
  switch (id - AbstractGeolocation::getFieldCount()) {
    case  0: return "Sentences";
    default: return AbstractGeolocation::getFieldName(id);
  }
}

ValueType GeoSentences::getFieldType (int32_t id) const {
  switch (id - AbstractGeolocation::getFieldCount()) {
    case  0: return ValueType_String;
    default: return AbstractGeolocation::getFieldType(id);
  }
}

Value* GeoSentences::getField (int32_t id) const {
  switch (id - AbstractGeolocation::getFieldCount()) {
    case  0: return new Value(getSentences());
    default: return AbstractGeolocation::getField(id);
  }
}

void GeoSentences::setField (int32_t id, const Value* val) {
  switch (id - AbstractGeolocation::getFieldCount()) {
    case  0: setSentences(val->as<string>()); return;
    default: AbstractGeolocation::setField(id,val); return;
  }
}

void ContextAssocLists::setAsynchronousChannelTagsPresent (bool val) {
  if (val) packShort(6, (short)(unpackShort(6) | 0x8000)); // insert
  else     packShort(6, (short)(unpackShort(6) & 0x7FFF)); // remove
  updateByteLength(-1);
}

vector<int32_t> ContextAssocLists::getSourceContext () const {
  vector<int32_t> val(getSourceContextCount());
  for (size_t i=0, j=getSourceContextOffset(); i < val.size(); i++,j+=4) {
    val[i] = unpackInt(j);
  }
  return val;
}

vector<int32_t> ContextAssocLists::getSystemContext () const {
  vector<int32_t> val(getSystemContextCount());
  for (size_t i=0, j=getSystemContextOffset(); i < val.size(); i++,j+=4) {
    val[i] = unpackInt(j);
  }
  return val;
}

vector<int32_t> ContextAssocLists::getVectorComponent () const {
  vector<int32_t> val(getVectorComponentCount());
  for (size_t i=0, j=getVectorComponentOffset(); i < val.size(); i++,j+=4) {
    val[i] = unpackInt(j);
  }
  return val;
}

vector<int32_t> ContextAssocLists::getAsynchronousChannel () const {
  vector<int32_t> val(getAsynchronousChannelCount());
  for (size_t i=0, j=getAsynchronousChannelOffset(); i < val.size(); i++,j+=4) {
    val[i] = unpackInt(j);
  }
  return val;
}

vector<int32_t> ContextAssocLists::getAsynchronousChannelTag () const {
  if (!getAsynchronousChannelTagsPresent()) return vector<int32_t>(0);
  vector<int32_t> val(getAsynchronousChannelCount());
  for (size_t i=0, j=getAsynchronousChannelTagsOffset(); i < val.size(); i++,j+=4) {
    val[i] = unpackInt(j);
  }
  return val;
}

void ContextAssocLists::setSourceContext (const vector<int32_t> &val) {
  setSourceContextCount(val.size());
  for (size_t i=0, j=getSourceContextOffset(); i < val.size(); i++,j+=4) {
    packInt(j, val[i]);
  }
}

void ContextAssocLists::setSystemContext (const vector<int32_t> &val) {
  setSystemContextCount(val.size());
  for (size_t i=0, j=getSystemContextOffset(); i < val.size(); i++,j+=4) {
    packInt(j, val[i]);
  }
}

void ContextAssocLists::setVectorComponent (const vector<int32_t> &val) {
  setVectorComponentCount(val.size());
  for (size_t i=0, j=getVectorComponentOffset(); i < val.size(); i++,j+=4) {
    packInt(j, val[i]);
  }
}

void ContextAssocLists::setAsynchronousChannel (const vector<int32_t> &val) {
  setAsynchronousChannelCount(val.size());
  setAsynchronousChannelTagsPresent(false);
  for (size_t i=0, j=getAsynchronousChannelOffset(); i < val.size(); i++,j+=4) {
    packInt(j, val[i]);
  }
}

void ContextAssocLists::setAsynchronousChannel (const vector<int32_t> &val, const vector<int32_t> &tags) {
  setAsynchronousChannel(val);
  if (val.size() == tags.size()) {
    setAsynchronousChannelTagsPresent(true);
    for (size_t i=0, j=getAsynchronousChannelTagsOffset(); i < val.size(); i++,j+=4) {
      packInt(j, tags[i]);
    }
  }
}

string ContextAssocLists::toString () const {
  vector<int32_t> _source = getSourceContext();
  vector<int32_t> _system = getSystemContext();
  vector<int32_t> _vector = getVectorComponent();
  vector<int32_t> _asynch = getAsynchronousChannel();
  vector<int32_t> _asytag = getAsynchronousChannelTag();

  ostringstream str;
  str << Record::toString();
  str << "SourceContext=[";
  for (size_t i = 0; i < _source.size(); i++) {
    if (i > 0) str << ", ";
    str << _source[i];
  }
  str << "] SystemContext=[";
  for (size_t i = 0; i < _system.size(); i++) {
    if (i > 0) str << ", ";
    str << _system[i];
  }
  str << "] VectorComponent=[";
  for (size_t i = 0; i < _vector.size(); i++) {
    if (i > 0) str << ", ";
    str << _vector[i];
  }
  str << "] AsynchronousChannel=[";
  for (size_t i = 0; i < _asynch.size(); i++) {
    if (i > 0) str << ", ";
    str << _asynch[i];
  }
  if (getAsynchronousChannelTagsPresent()) {
    str << "] AsynchronousChannelTag=[";
    for (size_t i = 0; i < _asytag.size(); i++) {
      if (i > 0) str << ", ";
      str << _asytag[i];
    }
  }
  str << "]";
  return str.str();
}

void ContextAssocLists::updateByteLength (int32_t off) {
  int32_t len = getAsynchronousChannelTagsOffset();
  if (getAsynchronousChannelTagsPresent()) {
    len += 4*getAsynchronousChannelCount();
  }
  setByteLength(len, off);
}

void ContextAssocLists::resize (int32_t pos, int32_t max, int32_t off, int32_t old, int32_t val) {
  if (old == val) {
    return; // no change
  }
  if ((val < 0) || (val > max)) {
    throw VRTException("Invalid size %d must be between 0 and %d", val, max);
  }

  int32_t oldLen = getByteLength();
  int32_t newLen = oldLen + ((val - old) * 4);
  int32_t offset = off + (min(old, val) * 4);

  setByteLength(newLen, offset);
  packShort(pos, (int16_t)(val & max));
}

int32_t ContextAssocLists::getFieldCount () const {
  return Record::getFieldCount() + 5;
}

string ContextAssocLists::getFieldName (int32_t id) const {
  switch (id - Record::getFieldCount()) {
    case  0: return "SourceContext";
    case  1: return "SystemContext";
    case  2: return "VectorComponent";
    case  3: return "AsynchronousChannel";
    case  4: return "AsynchronousChannelTag";
    default: return Record::getFieldName(id);
  }
}

ValueType ContextAssocLists::getFieldType (int32_t id) const {
  switch (id - Record::getFieldCount()) {
    case  0: return (ValueType)-ValueType_Int32;
    case  1: return (ValueType)-ValueType_Int32;
    case  2: return (ValueType)-ValueType_Int32;
    case  3: return (ValueType)-ValueType_Int32;
    case  4: return (ValueType)-ValueType_Int32;
    default: return Record::getFieldType(id);
  }
}

Value* ContextAssocLists::getField (int32_t id) const {
  switch (id - Record::getFieldCount()) {
    case  0: return new Value(new vector<int32_t>(getSourceContext()),          true);
    case  1: return new Value(new vector<int32_t>(getSystemContext()),          true);
    case  2: return new Value(new vector<int32_t>(getVectorComponent()),        true);
    case  3: return new Value(new vector<int32_t>(getAsynchronousChannel()),    true);
    case  4: return new Value(new vector<int32_t>(getAsynchronousChannelTag()), true);
    default: return Record::getField(id);
  }
}

void ContextAssocLists::setField (int32_t id, const Value* val) {
  int n = (id - Record::getFieldCount());

  if ((n >= 0) && (n <= 4)) {
    vector<int32_t> vec(val->size());
    for (size_t i = 0; i < val->size(); i++) {
      Value *v = val->at(i);
      vec[i] = *v;
      delete v;
    }

    switch (n) {
      case  0: setSourceContext(vec); return;
      case  1: setSystemContext(vec); return;
      case  2: setVectorComponent(vec); return;
      case  3: setAsynchronousChannel(vec); return;
      case  4: setAsynchronousChannel(getAsynchronousChannel(), vec); return;
    }
  }
  else {
    Record::setField(id,val); return;
  }
}

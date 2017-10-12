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


SpectrumField::SpectrumField             ()                             : Record(56) { }
SpectrumField::SpectrumField             (const SpectrumField       &r) : Record(r) { }
ArrayOfRecords::ArrayOfRecords           ()                             : Record(12) { }
ArrayOfRecords::ArrayOfRecords           (const ArrayOfRecords      &r) : Record(r) { }
IndexFieldList::IndexFieldList           ()                             : Record(8) { }
IndexFieldList::IndexFieldList           (const IndexFieldList      &r) : Record(r) { }
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
  return AbstractGeolocation::getFieldCount() + 9;
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

// IndexFieldList
string IndexFieldList::toString () const {
  ostringstream str;
  str << Record::toString();
  int8_t bpe = getIndexEntrySize();
  int32_t cnt = getIndexEntriesCount();
  if (bpe == 0 || cnt == 0)
    return str.str();
  int32_t off = getIndexEntriesByteOffset();

  str << "IndexFieldList=[" << std::hex << std::setw(bpe*2) << std::setfill('0');
  for (int32_t i = 0; i < cnt; i++) {
    if (i > 0) str << ", ";
    switch (bpe) {
      case 1: str << unpackByte(off+i); break;
      case 2: str << unpackShort(off+i*2); break;
      case 4: str << unpackInt(off+i*4); break;
      default: break;
    }
  }
  str << "]";
  return str.str();
}

void IndexFieldList::updateByteLength () {
  int32_t len = getTotalFieldSize()*4;
  setByteLength(len);            // Do this first to zero buffer if len==0
  if (len < 8) setByteLength(8); // But make sure we maintain at least 8 bytes
}

void IndexFieldList::setIndexEntriesCount (int32_t val) {
  // set num entries
  int8_t bpe = getIndexEntrySize(); // preserve entry size
  packInt(4, val & 0xFFFFF);
  packByte(4, bpe);
  // set total size
  int32_t list_words = (int32_t)((bpe*(val&0xFFFFF)+3)/4); 
  packInt(0, 2+list_words);
  updateByteLength();
}

void IndexFieldList::setIndexEntrySize (int8_t val) {
  // set entry size
  packByte(4, val&0x7);
  // set total size
  int32_t list_words = (int32_t)((getIndexEntriesCount()*(val&0x7)+3)/4); 
  packInt(0, 2+list_words);
  updateByteLength();
}

vector<int32_t> IndexFieldList::getIndexEntries () const {
  vector<int32_t> val(getIndexEntriesCount());
  int8_t bpe = getIndexEntrySize();
  for (size_t i=0, j=getIndexEntriesByteOffset(); i < val.size(); i++,j+=bpe) {
    switch (bpe) {
      case 1: val[i] = (int32_t) unpackByte(j);
      case 2: val[i] = (int32_t) unpackShort(j);
      case 4: val[i] = (int32_t) unpackInt(j);
      default: break;
    }
  }
  return val;
}

void IndexFieldList::setIndexEntries (const vector<int32_t> &val, int8_t bpe) {
  if (bpe!=0) setIndexEntrySize(bpe);
  else bpe = getIndexEntrySize();
  if (bpe==4) return setIndexEntries(val);
  setIndexEntriesCount(val.size());
  for (size_t i=0, j=getIndexEntriesByteOffset(); i < val.size(); i++,j+=bpe) {
    switch (bpe) {
      case 1: packByte (j, (int8_t)  val[i]);
      case 2: packShort(j, (int16_t) val[i]);
      //case 4: packInt  (j, (int32_t) val[i]);
      default: break;
    }
  }
}

void IndexFieldList::setIndexEntries (const vector<int32_t> &val) {
  setIndexEntrySize(4);
  setIndexEntriesCount(val.size());
  memcpy(&buf[8], &val[0], val.size()*4);
  //for (size_t i=0, j=getIndexEntriesByteOffset(); i < val.size(); i++,j+=4) {
  //  packInt(j, val[i]);
  //}
}

void IndexFieldList::setIndexEntries (const vector<int16_t> &val) {
  setIndexEntrySize(2);
  setIndexEntriesCount(val.size());
  memcpy(&buf[8], &val[0], val.size()*2);
  //for (size_t i=0, j=getIndexEntriesByteOffset(); i < val.size(); i++,j+=2) {
  //  packShort(j, val[i]);
  //}
}

void IndexFieldList::setIndexEntries (const vector<int8_t> &val) {
  setIndexEntrySize(1);
  setIndexEntriesCount(val.size());
  memcpy(&buf[8], &val[0], val.size());
  //for (size_t i=0, j=getIndexEntriesByteOffset(); i < val.size(); i++,j+=1) {
  //  packByte(j, val[i]);
  //}
}

int32_t IndexFieldList::getFieldCount () const {
  return Record::getFieldCount() + 1;
}

string IndexFieldList::getFieldName (int32_t id) const {
  if ((id - Record::getFieldCount()) == 0)
    return "IndexEntries";
  else
    return Record::getFieldName(id);
}

ValueType IndexFieldList::getFieldType (int32_t id) const {
  if ((id - Record::getFieldCount()) == 0)
    return (ValueType)-ValueType_Int32;
  else
    return Record::getFieldType(id);
}

Value* IndexFieldList::getField (int32_t id) const {
  if ((id - Record::getFieldCount()) == 0)
    return new Value(new vector<int32_t>(getIndexEntries()), true);
  else
    return Record::getField(id);
}

void IndexFieldList::setField (int32_t id, const Value* val) {
  if ((id - Record::getFieldCount()) == 0) {
    vector<int32_t> vec(val->size());
    for (size_t i = 0; i < val->size(); i++) {
      Value *v = val->at(i);
      vec[i] = *v;
      delete v;
    }
    setIndexEntries(vec); return;
  }
  else {
    Record::setField(id,val); return;
  }
}

// ArrayOfRecords
string ArrayOfRecords::toString () const {
  ostringstream str;
  str << Record::toString();
  str << "BitMappedIndicator=" << std::hex << std::setw(8) << std::setfill('0')
      << getBitMappedIndicator();

  str << " ArrayOfRecords=[";
  std::vector<Record> recs(getRecords());
  for (size_t i = 0; i < recs.size(); i++) {
    if (i > 0) str << ", ";
    str << recs[i].toString();
  }
  str << "]";
  return str.str();
}

void ArrayOfRecords::updateByteLength (int32_t off) {
  int32_t totalSize = 3 + getHeaderSize() + getRecordSize()*getRecordCount();
  setByteLength(totalSize*4, off);
}

void ArrayOfRecords::setRecordSize (int32_t val) {
  // set record size, preserving header size and record count
  int32_t hdr1 = unpackInt(4) & 0xFF000FFF; // clear previous rec size
  hdr1 |= (val<<12) & 0x00FFF000;   // update with new rec size
  packInt(4, val);
  // set total size
  packInt(0, 3+getHeaderSize()+(val&0xFFF)*getRecordCount());
  updateByteLength();
}

void ArrayOfRecords::setRecordCount (int32_t val, int32_t off) {
  // set record count, preserving header size and record size
  int32_t hdr1 = unpackInt(4) & 0xFFFFF000; // clear previous rec count
  hdr1 |= (val) & 0x00000FFF;               // update with new rec count
  packInt(4, val);
  // set total size
  packInt(0, 3+getHeaderSize()+getRecordSize()*(val&0xFFF));
  updateByteLength(off);
}

void ArrayOfRecords::setApplicationSpecificHeader (const vector<char> &val) {
  setApplicationSpecificHeader(&val[0], val.size());
}

void ArrayOfRecords::setApplicationSpecificHeader (const char* val, size_t bytes) {
  if (bytes > 255*4)
    throw VRTException("Application Specific Header size exceeded limit of 255 32-bit words.");
  packByte(4, (int8_t) ((bytes+3)/4) & 0xFF); // header size in full 32-bit words
  updateByteLength(12); // insert before records, or erase from existing app-spec header
  if (val!=NULL) memcpy(&buf[12], val, bytes);
}

vector<Record> ArrayOfRecords::getRecords (int32_t idx, int32_t num) const {
  const int32_t numRecs = getRecordCount();
  if (idx < 0 || idx >= numRecs || num == 0)
    return vector<Record>();
  if (num < 0 || idx+num > numRecs)
    num = numRecs-idx;
  int32_t bpe = getRecordSize()*4; // bytes per record entry
  vector<Record> val(num, Record(bpe));
  for (size_t i=0, j=getRecordEntriesByteOffset()+idx*bpe; i < val.size(); i++,j+=bpe) {
    VRTMath::unpackRecord(buf, j, val[i]);
    //val[i].writeBytes(&buf[j]); // the previous call does exactly this...
  }
  return val;
}

void ArrayOfRecords::getRecord (int32_t idx, Record &rec) const {
  if (idx < 0 || idx >= getRecordCount()) {
    //rec.setByteLength(0); // can't do this b/c protected method
    rec = Record(0);
  } else {
    int32_t bpe = getRecordSize()*4; // bytes per record entry
    //rec.setByteLength(bpe); // can't do this b/c protected method
    rec = Record(bpe);
    VRTMath::unpackRecord(buf, getRecordEntriesByteOffset()+idx*bpe, rec);
  }
}

void ArrayOfRecords::setRecords (const vector<Record> &val) {
  if (val.size() == 0) {
    setRecordCount(0);
    setRecordSize(0);
  } else {
    int32_t wpe = (val[0].getByteLength()+3)/4; // number of full 32-bit words
    setRecordSize(wpe);
    setRecordCount(val.size());
    for (size_t i=0, j=getRecordEntriesByteOffset(); i < val.size(); i++,j+=wpe*4) {
      VRTMath::packRecord(buf, j, val[i]);
    }
  }
}

int32_t ArrayOfRecords::addRecords (const vector<Record> &val, int32_t idx) {
  if (val.size() == 0) return 0;
  if (idx < 0 || idx >= getRecordCount()) idx = getRecordCount();
  int32_t wpe = (val[0].getByteLength()+3)/4; // number of full 32-bit words
  if (getRecordCount() == 0) {
    setRecordSize(wpe);
    setRecordCount(val.size());
  } else if (wpe != getRecordSize()) { // can't add records of different size
    return 0;
  } else { // add space for records at proper byte offset based on idx
    setRecordCount(getRecordCount()+val.size(), getRecordEntriesByteOffset()+idx*wpe*4);
  }
  for (size_t i=0, j=getRecordEntriesByteOffset()+idx*wpe*4; i < val.size(); i++,j+=wpe*4) {
    // recalc wpe for each Record to ensure they do not change
    if (wpe != (int32_t) (val[i].getByteLength()+3)/4) {
      setRecordCount(getRecordCount()+i, j); // remove unnecessary space for invalid records
      return i;
    }
    VRTMath::packRecord(buf, j, val[i]);
  }
  return val.size();
}

bool ArrayOfRecords::addRecord (const Record &val, int32_t idx) {
  if (isNull(val)) return true; // or false? Adding null is considered successful
  if (idx < 0 || idx >= getRecordCount()) idx = getRecordCount();
  int32_t wpe = (val.getByteLength()+3)/4; // number of full 32-bit words
  if (getRecordCount() == 0) {
    setRecordSize(wpe);
    setRecordCount(1);
  } else if (wpe != getRecordSize()) { // can't add a record of different size
    return false;
  } else { // add space for record at proper byte offset based on idx
    setRecordCount(getRecordCount()+1, getRecordEntriesByteOffset()+idx*wpe*4);
  }
  VRTMath::packRecord(buf, getRecordEntriesByteOffset()+idx*wpe*4, val);
  return true;
}

int32_t ArrayOfRecords::getFieldCount () const {
  return Record::getFieldCount() + 1;
}

string ArrayOfRecords::getFieldName (int32_t id) const {
  switch (id - Record::getFieldCount()) {
    case  0: return "BitMappedIndicator";
    default: return Record::getFieldName(id);
  }
}

ValueType ArrayOfRecords::getFieldType (int32_t id) const {
  switch (id - Record::getFieldCount()) {
    case  0: return (ValueType)ValueType_Int32;
    default: return Record::getFieldType(id);
  }
}

Value* ArrayOfRecords::getField (int32_t id) const {
  switch (id - Record::getFieldCount()) {
    case  0:
      return new Value(getBitMappedIndicator());
    default: return Record::getField(id);
  }
}

void ArrayOfRecords::setField (int32_t id, const Value* val) {
  switch (id - Record::getFieldCount()) {
    case  0:
      setBitMappedIndicator(val->as<int32_t>()); return;
    default: Record::setField(id,val); return;
  }
}

// SpectrumField
string SpectrumField::toString () const {
  ostringstream str;
  str << Record::toString();
  Utilities::append(str,  "SpectrumType=",       getSpectrumType());
  Utilities::append(str, " AveragingType=",      getAveragingType());
  Utilities::append(str, " WindowTime=",         getWindowTime());
  Utilities::append(str, " WindowType=",         getWindowType());
  Utilities::append(str, " NumTransformPoints=", getNumTransformPoints());
  Utilities::append(str, " NumWindowPoints=",    getNumWindowPoints());
  Utilities::append(str, " Resolution=",         getResolution());
  Utilities::append(str, " Span=",               getSpan());
  Utilities::append(str, " NumAverages=",        getNumAverages());
  Utilities::append(str, " WeightingFactor=",    getWeightingFactor());
  Utilities::append(str, " SpectrumF1Index=",    getSpectrumF1Index());
  Utilities::append(str, " SpectrumF2Index=",    getSpectrumF2Index());
  Utilities::append(str, " WindowTimeDelta=",    getWindowTimeDelta());
  return str.str();
}

int32_t SpectrumField::getFieldCount () const {
  return Record::getFieldCount() + 13;
}

string SpectrumField::getFieldName (int32_t id) const {
  switch (id - Record::getFieldCount()) {
    case  0: return "SpectrumType";
    case  1: return "AveragingType";
    case  2: return "WindowTime";
    case  3: return "WindowType";
    case  4: return "NumTransformPoints";
    case  5: return "NumWindowPoints";
    case  6: return "Resolution";
    case  7: return "Span";
    case  8: return "NumAverages";
    case  9: return "WeightingFactor";
    case 10: return "SpectrumF1Index";
    case 11: return "SpectrumF2Index";
    case 12: return "WindowTimeDelta";
    default: return Record::getFieldName(id);
  }
}

ValueType SpectrumField::getFieldType (int32_t id) const {
  switch (id - Record::getFieldCount()) {
    case  0: return ValueType_Int8;
    case  1: return ValueType_Int8;
    case  2: return ValueType_Int8;
    case  3: return ValueType_Int8;
    case  4: return ValueType_Int32;
    case  5: return ValueType_Int32;
    case  6: return ValueType_Double;
    case  7: return ValueType_Double;
    case  8: return ValueType_Int32;
    case  9: return ValueType_Int32;
    case 10: return ValueType_Int32;
    case 11: return ValueType_Int32;
    case 12: return ValueType_Double;
    default: return Record::getFieldType(id);
  }
}

Value* SpectrumField::getField (int32_t id) const {
  switch (id - Record::getFieldCount()) {
    case  0: return new Value(getSpectrumType());
    case  1: return new Value(getAveragingType());
    case  2: return new Value(getWindowTime());
    case  3: return new Value(getWindowType());
    case  4: return new Value(getNumTransformPoints());
    case  5: return new Value(getNumWindowPoints());
    case  6: return new Value(getResolution());
    case  7: return new Value(getSpan());
    case  8: return new Value(getNumAverages());
    case  9: return new Value(getWeightingFactor());
    case 10: return new Value(getSpectrumF1Index());
    case 11: return new Value(getSpectrumF2Index());
    case 12: return new Value(getWindowTimeDelta());
    default: return Record::getField(id);
  }
}

void SpectrumField::setField (int32_t id, const Value* val) {
  switch (id - Record::getFieldCount()) {
    case  0: setSpectrumType(       val->as<int8_t>());  return;
    case  1: setAveragingType(      val->as<int8_t>());  return;
    case  2: setWindowTime(         val->as<int8_t>());  return;
    case  3: setWindowType(         val->as<int8_t>());  return;
    case  4: setNumTransformPoints( val->as<int32_t>()); return;
    case  5: setNumWindowPoints(    val->as<int32_t>()); return;
    case  6: setResolution(         val->as<double>());  return;
    case  7: setSpan(               val->as<double>());  return;
    case  8: setNumAverages(        val->as<int32_t>()); return;
    case  9: setWeightingFactor(    val->as<int32_t>()); return;
    case 10: setSpectrumF1Index(    val->as<int32_t>()); return;
    case 11: setSpectrumF2Index(    val->as<int32_t>()); return;
    case 12: setWindowTimeDelta(    val->as<double>());  return;
    default: Record::setField(id,val); return;
  }
}


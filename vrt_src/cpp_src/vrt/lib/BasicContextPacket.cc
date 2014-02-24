/*
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
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#include "BasicContextPacket.h"
#include <bitset> // includes the popcount functions

using namespace vrt;
using namespace private_BasicContextPacket;

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

/**  Creates the default header for the packet. This is defined separate from the class to account
 *  for the systems/compilers that disallow calling of subclass methods from the constructor.
 */
static inline vector<char> BasicContextPacket_createDefaultPacket () {
  vector<char> buf(BasicVRTPacket::MAX_HEADER_LENGTH + 4);
  buf[0] = 0x48;
  buf[1] = 0x60;
  buf[2] = 0x00;
  buf[3] = 0x08;
  return buf;
}
BasicContextPacket::BasicContextPacket (const int32_t pktsize) :
  BasicVRTPacket(pktsize) // done
{
}

BasicContextPacket::BasicContextPacket (const BasicVRTPacket &p) :
  BasicVRTPacket(p)
{
  if (getPacketType() != PacketType_Context) {
    throw VRTException("Can not create ContextPacket from given packet");
  }
}

BasicContextPacket::BasicContextPacket (const void *buf, size_t len, bool readOnly) :
  BasicVRTPacket(buf,len,readOnly)
{
  // done
}

BasicContextPacket::BasicContextPacket (const BasicVRTPacket &p, int64_t classID) :
  BasicVRTPacket(p)
{
  if (getPacketType() != PacketType_Context) {
    throw VRTException("Can not create ContextPacket from given packet");
  }
  setClassIdentifier(classID);
}


BasicContextPacket::BasicContextPacket (const vector<char> &buf, ssize_t start, ssize_t end, bool readOnly) :
  BasicVRTPacket(buf, start, end, readOnly)
{
  // done
}
BasicContextPacket::BasicContextPacket (vector<char> *buf_ptr, bool readOnly) :
  BasicVRTPacket(buf_ptr, readOnly)
{
  // done
}

BasicContextPacket::BasicContextPacket (const vector<char> &buf, bool readOnly) :
  BasicVRTPacket::BasicVRTPacket(buf,readOnly)
{
  // done
}

BasicContextPacket::BasicContextPacket (const BasicVRTPacket &p, PacketType type, int64_t classID,
                                        int32_t minPayloadLength, int32_t maxPayloadLength) :
  BasicVRTPacket(p,type,classID,minPayloadLength,maxPayloadLength)
{
  if (type != PacketType_Context) {
    throw VRTException("BasicContextPacket can only be created with type=Context");
  }
}

BasicContextPacket::BasicContextPacket () :
  BasicVRTPacket(BasicContextPacket_createDefaultPacket(),false)
{
  // done
}

void BasicContextPacket::toStringStream (ostringstream &str) const {
  BasicVRTPacket::toStringStream(str);
  Utilities::append(str, " ChangePacket="            ,isChangePacket()             );
  Utilities::append(str, " ReferencePointIdentifier=",getReferencePointIdentifier());
  Utilities::append(str, " Bandwidth="               ,getBandwidth()         ,"Hz" );
  Utilities::append(str, " FrequencyIF="             ,getFrequencyIF()       ,"Hz" );
  Utilities::append(str, " FrequencyRF="             ,getFrequencyRF()       ,"Hz" );
  Utilities::append(str, " FrequencyOffsetRF="       ,getFrequencyOffsetRF() ,"Hz" );
  Utilities::append(str, " BandOffsetIF="            ,getBandOffsetIF()      ,"Hz" );
  Utilities::append(str, " ReferenceLevel="          ,getReferenceLevel()    ,"dBm");
  Utilities::append(str, " Gain1="                   ,getGain1()             ,"dB" );
  Utilities::append(str, " Gain2="                   ,getGain2()             ,"dB" );
  Utilities::append(str, " OverRangeCount="          ,getOverRangeCount()          );
  Utilities::append(str, " SampleRate="              ,getSampleRate()        ,"Hz" );
  Utilities::append(str, " TimeStampAdjustment="     ,getTimeStampAdjustment()     );
  Utilities::append(str, " TimeStampCalibration="    ,getTimeStampCalibration()    );
  Utilities::append(str, " Temperature="             ,getTemperature()             );
  Utilities::append(str, " DeviceID="                ,getDeviceID()                );
  Utilities::append(str, " CalibratedTimeStamp="     ,isCalibratedTimeStamp()      );
  Utilities::append(str, " DataValid="               ,isDataValid()                );
  Utilities::append(str, " ReferenceLocked="         ,isReferenceLocked()          );
  Utilities::append(str, " AGC="                     ,isAutomaticGainControl()     );
  Utilities::append(str, " SignalDetected="          ,isSignalDetected()           );
  Utilities::append(str, " InvertedSpectrum="        ,isInvertedSpectrum()         );
  Utilities::append(str, " OverRange="               ,isOverRange()                );
  Utilities::append(str, " Discontinuous="           ,isDiscontinuous()            );
  Utilities::append(str, " UserDefinedBits="         ,getUserDefinedBits()         );
  Utilities::append(str, " DataPayloadFormat={"      ,getDataPayloadFormat()   ,"}");
  Utilities::append(str, " GeolocationGPS={"         ,getGeolocationGPS()      ,"}");
  Utilities::append(str, " GeolocationINS={"         ,getGeolocationINS()      ,"}");
  Utilities::append(str, " EphemerisECEF={"          ,getEphemerisECEF()       ,"}");
  Utilities::append(str, " EphemerisRelative={"      ,getEphemerisRelative()   ,"}");
  Utilities::append(str, " EphemerisReference={"     ,getEphemerisReference()  ,"}");
  Utilities::append(str, " GeoSentences={"           ,getGeoSentences()        ,"}");
  Utilities::append(str, " ContextAssocLists={"      ,getContextAssocLists()   ,"}");
}

bool BasicContextPacket::resetForResend (const TimeStamp &t) {
  BasicVRTPacket::resetForResend(t);
  setChangePacket(false);
  setOverRangeCount(INT64_NULL);
  setOverRange(_NULL);
  setDiscontinuous(_NULL);
  return true;
}

void BasicContextPacket::setRecord (int32_t bit, Record val, int32_t oldLen) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");
  int32_t off = getOffset(bit);

  if ((isNull(val)) && (off < 0)) return; // not present, no change

  if (off >= 0) {
    // remove the old one
    setContextIndicatorFieldBit(bit, false);
    shiftPayload(off, oldLen, false);
    off = -off;
  }

  if (!isNull(val)) {
    setContextIndicatorFieldBit(bit, true);
    off = shiftPayload(off, val.getByteLength(), true);
    packPayloadRecord(off, val);
  }
}

Ephemeris BasicContextPacket::getEphemerisECEF () const {
  int32_t   off = getOffset(ECEF_EPHEM);
  int32_t   adj = getOffset(ECEF_EPHEM_ADJ);
  Ephemeris val;
  
  if (off >= 0) unpackPayloadRecord(off, val);
  if (adj >= 0) unpackPayloadRecord(adj, val.getAdjunctRef());

  return val;
}

Ephemeris BasicContextPacket::getEphemerisRelative () const {
  int32_t   off = getOffset(REL_EPHEM);
  int32_t   adj = getOffset(REL_EPHEM_ADJ);
  Ephemeris val;

  if (off >= 0) unpackPayloadRecord(off, val);
  if (adj >= 0) unpackPayloadRecord(adj, val.getAdjunctRef());

  return val;
}

void BasicContextPacket::setEphemerisECEF (const Ephemeris &val) {
  setRecord(ECEF_EPHEM, val);

  if ((VRTConfig::getVRTVersion() == VRTConfig::VITAVersion_V49b) && val.getAdjunct().isAnythingSet()) {
    setRecord(ECEF_EPHEM_ADJ, val.getAdjunct());
  }
  else {
    setRecord(ECEF_EPHEM_ADJ, EphemerisAdjunct()); // null value
  }
}

void BasicContextPacket::setEphemerisRelative (const Ephemeris &val) {
  setRecord(REL_EPHEM, val);

  if ((VRTConfig::getVRTVersion() == VRTConfig::VITAVersion_V49b) && val.getAdjunct().isAnythingSet()) {
    setRecord(REL_EPHEM_ADJ, val.getAdjunct());
  }
  else {
    setRecord(ECEF_EPHEM_ADJ, EphemerisAdjunct()); // null value
  }
}

void BasicContextPacket::setContextIndicatorFieldBit (int32_t bit, bool set) {
  int32_t cif = getContextIndicatorField();
  int32_t val;

  if (set) val = cif | bit;
  else     val = cif & ~bit;

  if (val != cif) {
    VRTMath::packInt(bbuf, getHeaderLength(), val);
  }
}

int8_t BasicContextPacket::getB (int32_t bit, int32_t xoff) const {
  int32_t off = getOffset(bit);
  if (off < 0) return INT8_NULL;
  return bbuf[off+xoff+getHeaderLength()];
}
void BasicContextPacket::setB (int32_t bit, int32_t xoff, int8_t val) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");
  int32_t off = getOffset(bit);
  bool present = !isNull(val);

  setContextIndicatorFieldBit(bit, present);
  off = shiftPayload(off, 4, present);

  if (!isNull(val)) {
    bbuf[off+xoff+getHeaderLength()] = val;
  }
}
int16_t BasicContextPacket::getI (int32_t bit, int32_t xoff) const {
  int32_t off = getOffset(bit);
  if (off < 0) return INT16_NULL;
  return VRTMath::unpackShort(bbuf, off+xoff+getHeaderLength());
}
void BasicContextPacket::setI (int32_t bit, int32_t xoff, int16_t val) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");
  int32_t off = getOffset(bit);
  bool present = !isNull(val);

  setContextIndicatorFieldBit(bit, present);
  off = shiftPayload(off, 4, present);

  if (!isNull(val)) {
    VRTMath::packShort(bbuf, off+xoff+getHeaderLength(), val);
  }
}
int32_t BasicContextPacket::getL24 (int32_t bit, int32_t offset) const {
  int32_t off = getOffset(bit);
  if (off < 0) return INT32_NULL;
  int32_t bits = VRTMath::unpackInt(bbuf, off+getHeaderLength());
  bits = (bits & (0xFFFFFF00 >> 8*offset)) >> 8*offset;
  return bits;
}
int32_t BasicContextPacket::getL (int32_t bit) const {
  int32_t off = getOffset(bit);
  if (off < 0) return INT32_NULL;
  return VRTMath::unpackInt(bbuf, off+getHeaderLength());
}
void BasicContextPacket::setL (int32_t bit, int32_t val) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");
  int32_t off = getOffset(bit);
  bool present = !isNull(val);

  setContextIndicatorFieldBit(bit, present);
  off = shiftPayload(off, 4, present);

  if (!isNull(val)) {
    VRTMath::packInt(bbuf, off+getHeaderLength(), val);
  }
}
int64_t BasicContextPacket::getX (int32_t bit) const {
  int32_t off = getOffset(bit);
  if (off < 0) return INT64_NULL;
  return VRTMath::unpackLong(bbuf, off+getHeaderLength());
}
void BasicContextPacket::setX (int32_t bit, int64_t val) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");
  int32_t off = getOffset(bit);
  bool present = !isNull(val);

  setContextIndicatorFieldBit(bit, present);
  off = shiftPayload(off, 8, present);

  if (!isNull(val)) {
    VRTMath::packLong(bbuf, off+getHeaderLength(), val);
  }
}

boolNull BasicContextPacket::getStateEventBit (int32_t enable, int32_t indicator) const {
  int32_t off = getOffset(STATE_EVENT);
  if (off < 0) return _NULL;
  return BasicVRTPacket::getStateEventBit(bbuf, off+getHeaderLength(), enable, indicator);
}

void BasicContextPacket::setStateEventBit (int32_t enable, int32_t indicator, boolNull value) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");
  int32_t off = getOffset(STATE_EVENT);
  if (off < 0) {
    if (value == _NULL) return; // no trailer, no need to set to null
    setContextIndicatorFieldBit(STATE_EVENT, true);
    off = shiftPayload(off, 4, true);
  }
  BasicVRTPacket::setStateEventBit(bbuf, off+getHeaderLength(), enable, indicator, value);
}

int32_t BasicContextPacket::getFieldLen (int32_t field) const {
  if ((field & CTX_4_OCTETS ) != 0) return  4;
  if ((field & CTX_8_OCTETS ) != 0) return  8;
  if ((field & CTX_36_OCTETS) != 0) return 36;
  if ((field & CTX_44_OCTETS) != 0) return 44;
  if ((field & CTX_52_OCTETS) != 0) return 52;
  if (field == GPS_ASCII) {
    int32_t h = getHeaderLength();
    int off = getOffset(GPS_ASCII);
    return VRTMath::unpackInt(bbuf, h+4+off)*4+8;
  }
  if (field == CONTEXT_ASOC) {
    int32_t h = getHeaderLength();
    int off = getOffset(CONTEXT_ASOC);
    int32_t source = VRTMath::unpackShort(bbuf, h+0+off) & 0x01FF;
    int32_t system = VRTMath::unpackShort(bbuf, h+2+off) & 0x01FF;
    int32_t vector = VRTMath::unpackShort(bbuf, h+4+off) & 0xFFFF;
    int32_t asynch = VRTMath::unpackShort(bbuf, h+6+off) & 0x7FFF;
    if ((VRTMath::unpackShort(bbuf, h+6+off) & 0x8000) != 0) asynch *= 2;
    return (source + system + vector + asynch)*4+8;
  }
  return -1;
}

string BasicContextPacket::getPacketValid (bool strict, int32_t length) const {
  string err = BasicVRTPacket::getPacketValid(strict, length);
  if (err != "") return err;

  if (getPacketType() != PacketType_Context) {
    ostringstream str;
    str << "Invalid use of " << getClassName() << " with " << getPacketType() << " packet.";
    return str.str();
  }
  return "";
}

int32_t __attribute__((hot)) BasicContextPacket::getOffset (int32_t field) const {
  // Since this is the most-used method in the class and often sees millions and
  // millions of calls within a typical application, it has been heavily
  // optimized to remove any loops and minimize the number of branches and other
  // operations required. It is assumed that bitCount(..) will be in-lined during
  // the compile.
  //
  // References:
  //   [1] Warren, Henry S. Jr. "Hacker's Delight." Addison-Wesley. 2002.
  //       ISBN 0-201-91465-4
  if (field == CONTEXT_IND) return 0; // special case, must be first

  // Set the mask for all fields before 'field'. This uses the "x^(x-1)" algorithm
  // from [1] (section 2-1) to set the field bit and all bits to the right, and
  // then inverts it to get all bits left of 'field' to be set.
  int32_t mask = ~(field ^ (field - 1));
  
  // For efficiency we compute the offset based on an applied bit mask and a few
  // multiplies. For *52 we do an *8 plus *44 to avoid the overhead of an extra
  // bitCount(..). In the below code, we the CTX_8_OCTETS|CTX_52_OCTETS and the
  // CTX_44_OCTETS|CTX_52_OCTETS should be in-lined during the compile; also the
  // *4 and *8 should be optimized (by the compiler) to bit-shifts where
  // appropriate.
  int32_t cif  = getContextIndicatorField();
  int32_t m    = cif & mask;
  int32_t off  = 4
               + (bitCount(m & (CTX_4_OCTETS               )) *  4)
               + (bitCount(m & (CTX_8_OCTETS |CTX_52_OCTETS)) *  8)
               + (bitCount(m & (CTX_44_OCTETS|CTX_52_OCTETS)) * 44);

  // GPS_ASCII length is variable so we handle it separately if applicable. Note
  // that following our initial off= computation the offset is now queued up to
  // point to the start of the GPS_ASCII field.
  if (field < GPS_ASCII) {
    int32_t h = getHeaderLength();
    if ((cif & GPS_ASCII) != 0) {
      off += VRTMath::unpackInt(bbuf, h+off+4)*4+8;
    }
    
    // CONTEXT_ASOC length is also variable, since it comes after GPS_ASCII
    // we nest it here so the check can be skipped in the 80% use case. Note
    // that off= should not point to the start of the CONTEXT_ASOC field.
    if (field < CONTEXT_ASOC) {
      if ((cif & CONTEXT_ASOC) != 0) {
        int32_t source = VRTMath::unpackShort(bbuf, h+off+0) & 0x01FF;
        int32_t system = VRTMath::unpackShort(bbuf, h+off+2) & 0x01FF;
        int32_t vector = VRTMath::unpackShort(bbuf, h+off+4) & 0xFFFF;
        int32_t asynch = VRTMath::unpackShort(bbuf, h+off+6) & 0x7FFF;
        if ((VRTMath::unpackShort(bbuf, h+6+off) & 0x8000) != 0) asynch *= 2;
        off += (source + system + vector + asynch)*4+8;
      }
      
      // The two V49.0b entries come after CONTEXT_ASOC, so we nest it here so
      // it can be skipped in the 90% use case.
      off += (bitCount(m & (CTX_36_OCTETS)) * 36);
    }
  }
  return ((cif & field) != 0)? off : -off;  // -off if not present
}

int32_t BasicContextPacket::getFieldCount () const {
  return BasicVRTPacket::getFieldCount() + 35;
}

string BasicContextPacket::getFieldName (int32_t id) const {
  switch (id - BasicVRTPacket::getFieldCount()) {
    case  0: return "ChangePacket";
    case  1: return "ReferencePointIdentifier";
    case  2: return "Bandwidth";
    case  3: return "FrequencyIF";
    case  4: return "FrequencyRF";
    case  5: return "FrequencyOffsetRF";
    case  6: return "BandOffsetIF";
    case  7: return "ReferenceLevel";
    case  8: return "Gain";
    case  9: return "Gain1";
    case 10: return "Gain2";
    case 11: return "OverRangeCount";
    case 12: return "SampleRate";
    case 13: return "SamplePeriod";
    case 14: return "TimeStampAdjustment";
    case 15: return "TimeStampCalibration";
    case 16: return "Temperature";
    case 17: return "DeviceID";
    case 18: return "CalibratedTimeStamp";
    case 19: return "DataValid";
    case 20: return "ReferenceLocked";
    case 21: return "AGC";
    case 22: return "SignalDetected";
    case 23: return "InvertedSpectrum";
    case 24: return "OverRange";
    case 25: return "Discontinuous";
    case 26: return "UserDefinedBits";
    case 27: return "DataPayloadFormat";
    case 28: return "GeolocationGPS";
    case 29: return "GeolocationINS";
    case 30: return "EphemerisECEF";
    case 31: return "EphemerisRelative";
    case 32: return "EphemerisReference";
    case 33: return "GeoSentences";
    case 34: return "ContextAssocLists";
    default: return BasicVRTPacket::getFieldName(id);
  }
}

ValueType BasicContextPacket::getFieldType (int32_t id) const {
  switch (id - BasicVRTPacket::getFieldCount()) {
    case  0: return ValueType_Bool;
    case  1: return ValueType_Int32;
    case  2: return ValueType_Double;
    case  3: return ValueType_Double;
    case  4: return ValueType_Double;
    case  5: return ValueType_Double;
    case  6: return ValueType_Double;
    case  7: return ValueType_Float;
    case  8: return ValueType_Float;
    case  9: return ValueType_Float;
    case 10: return ValueType_Float;
    case 11: return ValueType_Int64;
    case 12: return ValueType_Double;
    case 13: return ValueType_Double;
    case 14: return ValueType_Int64;
    case 15: return ValueType_Int32;
    case 16: return ValueType_Float;
    case 17: return ValueType_String;
    case 18: return ValueType_BoolNull;
    case 19: return ValueType_BoolNull;
    case 20: return ValueType_BoolNull;
    case 21: return ValueType_BoolNull;
    case 22: return ValueType_BoolNull;
    case 23: return ValueType_BoolNull;
    case 24: return ValueType_BoolNull;
    case 25: return ValueType_BoolNull;
    case 26: return ValueType_Int8;
    case 27: return ValueType_VRTObject;
    case 28: return ValueType_VRTObject;
    case 29: return ValueType_VRTObject;
    case 30: return ValueType_VRTObject;
    case 31: return ValueType_VRTObject;
    case 32: return ValueType_Int32;
    case 33: return ValueType_VRTObject;
    case 34: return ValueType_VRTObject;
    default: return BasicVRTPacket::getFieldType(id);
  }
}

Value* BasicContextPacket::getField (int32_t id) const {
  switch (id - BasicVRTPacket::getFieldCount()) {
    case  0: return new Value(isChangePacket());
    case  1: return new Value(getReferencePointIdentifier());
    case  2: return new Value(getBandwidth());
    case  3: return new Value(getFrequencyIF());
    case  4: return new Value(getFrequencyRF());
    case  5: return new Value(getFrequencyOffsetRF());
    case  6: return new Value(getBandOffsetIF());
    case  7: return new Value(getReferenceLevel());
    case  8: return new Value(getGain());
    case  9: return new Value(getGain1());
    case 10: return new Value(getGain2());
    case 11: return new Value(getOverRangeCount());
    case 12: return new Value(getSampleRate());
    case 13: return new Value(getSamplePeriod());
    case 14: return new Value(getTimeStampAdjustment());
    case 15: return new Value(getTimeStampCalibration());
    case 16: return new Value(getTemperature());
    case 17: return new Value(getDeviceID());
    case 18: return new Value(isCalibratedTimeStamp());
    case 19: return new Value(isDataValid());
    case 20: return new Value(isReferenceLocked());
    case 21: return new Value(isAutomaticGainControl());
    case 22: return new Value(isSignalDetected());
    case 23: return new Value(isInvertedSpectrum());
    case 24: return new Value(isOverRange());
    case 25: return new Value(isDiscontinuous());
    case 26: return new Value(getUserDefinedBits());
    case 27: return new Value(new PayloadFormat(    getDataPayloadFormat()));
    case 28: return new Value(new Geolocation(      getGeolocationGPS()));
    case 29: return new Value(new Geolocation(      getGeolocationINS()));
    case 30: return new Value(new Ephemeris(        getEphemerisECEF()));
    case 31: return new Value(new Ephemeris(        getEphemerisRelative()));
    case 32: return new Value(getEphemerisReference());
    case 33: return new Value(new GeoSentences(     getGeoSentences()));
    case 34: return new Value(new ContextAssocLists(getContextAssocLists()));
    default: return BasicVRTPacket::getField(id);
  }
}

void BasicContextPacket::setField (int32_t id, const Value* val) {
  switch (id - BasicVRTPacket::getFieldCount()) {
    case  0: setChangePacket(            val->as<bool    >()); return;
    case  1: setReferencePointIdentifier(val->as<int32_t >()); return;
    case  2: setBandwidth(               val->as<double  >()); return;
    case  3: setFrequencyIF(             val->as<double  >()); return;
    case  4: setFrequencyRF(             val->as<double  >()); return;
    case  5: setFrequencyOffsetRF(       val->as<double  >()); return;
    case  6: setBandOffsetIF(            val->as<double  >()); return;
    case  7: setReferenceLevel(          val->as<float   >()); return;
    case  8: setGain(                    val->as<float   >()); return;
    case  9: setGain1(                   val->as<float   >()); return;
    case 10: setGain2(                   val->as<float   >()); return;
    case 11: setOverRangeCount(          val->as<int64_t >()); return;
    case 12: setSampleRate(              val->as<double  >()); return;
    case 13: setSamplePeriod(            val->as<double  >()); return;
    case 14: setTimeStampAdjustment(     val->as<int64_t >()); return;
    case 15: setTimeStampCalibration(    val->as<int32_t >()); return;
    case 16: setTemperature(             val->as<float   >()); return;
    case 17: setDeviceID(                val->as<string  >()); return;
    case 18: setCalibratedTimeStamp(     val->as<boolNull>()); return;
    case 19: setDataValid(               val->as<boolNull>()); return;
    case 20: setReferenceLocked(         val->as<boolNull>()); return;
    case 21: setAutomaticGainControl(    val->as<boolNull>()); return;
    case 22: setSignalDetected(          val->as<boolNull>()); return;
    case 23: setInvertedSpectrum(        val->as<boolNull>()); return;
    case 24: setOverRange(               val->as<boolNull>()); return;
    case 25: setDiscontinuous(           val->as<boolNull>()); return;
    case 26: setUserDefinedBits(         val->as<int32_t >()); return;
    case 27: setDataPayloadFormat(      *val->cast<PayloadFormat*    >()); return;
    case 28: setGeolocationGPS(         *val->cast<Geolocation*      >()); return;
    case 29: setGeolocationINS(         *val->cast<Geolocation*      >()); return;
    case 30: setEphemerisECEF(          *val->cast<Ephemeris*        >()); return;
    case 31: setEphemerisRelative(      *val->cast<Ephemeris*        >()); return;
    case 32: setEphemerisReference(      val->as<int32_t>()); return;
    case 33: setGeoSentences(           *val->cast<GeoSentences*     >()); return;
    case 34: setContextAssocLists(      *val->cast<ContextAssocLists*>()); return;
    default: BasicVRTPacket::setField(id,val); return;
  }
}

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
  AbstractGeolocation(52),
  adjunct()
{
  // done
}

Ephemeris::Ephemeris (const Ephemeris &r) :
  AbstractGeolocation(r),
  adjunct(r.adjunct)
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
  str << adjunct.toString();
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
    case  9: return "RotationalVelocityAlpha";
    case 10: return "RotationalVelocityBeta";
    case 11: return "RotationalVelocityPhi";
    case 12: return "AccelerationX";
    case 13: return "AccelerationY";
    case 14: return "AccelerationZ";
    case 15: return "RotationalAccelerationAlpha";
    case 16: return "RotationalAccelerationBeta";
    case 17: return "RotationalAccelerationPhi";
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
    case  9: return ValueType_Double;
    case 10: return ValueType_Double;
    case 11: return ValueType_Double;
    case 12: return ValueType_Double;
    case 13: return ValueType_Double;
    case 14: return ValueType_Double;
    case 15: return ValueType_Double;
    case 16: return ValueType_Double;
    case 17: return ValueType_Double;
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
    case  9: return new Value(getRotationalVelocityAlpha());
    case 10: return new Value(getRotationalVelocityBeta());
    case 11: return new Value(getRotationalVelocityPhi());
    case 12: return new Value(getAccelerationX());
    case 13: return new Value(getAccelerationY());
    case 14: return new Value(getAccelerationZ());
    case 15: return new Value(getRotationalAccelerationAlpha());
    case 16: return new Value(getRotationalAccelerationBeta());
    case 17: return new Value(getRotationalAccelerationPhi());
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
    case  9: setRotationalVelocityAlpha(    val->as<double>()); return;
    case 10: setRotationalVelocityBeta(     val->as<double>()); return;
    case 11: setRotationalVelocityPhi(      val->as<double>()); return;
    case 12: setAccelerationX(              val->as<double>()); return;
    case 13: setAccelerationY(              val->as<double>()); return;
    case 14: setAccelerationZ(              val->as<double>()); return;
    case 15: setRotationalAccelerationAlpha(val->as<double>()); return;
    case 16: setRotationalAccelerationBeta( val->as<double>()); return;
    case 17: setRotationalAccelerationPhi(  val->as<double>()); return;
    default: AbstractGeolocation::setField(id,val); return;
  }
}

EphemerisAdjunct::EphemerisAdjunct () :
  Record(36)
{
  // Initialize everything to null to be compatible with cases where no adjunct is present
  packInt( 0, 0x7FFFFFFF);
  packInt( 4, 0x7FFFFFFF);
  packInt( 8, 0x7FFFFFFF);
  packInt(12, 0x7FFFFFFF);
  packInt(16, 0x7FFFFFFF);
  packInt(20, 0x7FFFFFFF);
  packInt(24, 0x7FFFFFFF);
  packInt(28, 0x7FFFFFFF);
  packInt(32, 0x7FFFFFFF);
}

EphemerisAdjunct::EphemerisAdjunct (const EphemerisAdjunct& r) :
  Record(r)
{
  // done
}

bool EphemerisAdjunct::isAnythingSet () const {
      return (unpackInt( 0) != 0x7FFFFFFF)
          || (unpackInt( 4) != 0x7FFFFFFF)
          || (unpackInt( 8) != 0x7FFFFFFF)
          || (unpackInt(12) != 0x7FFFFFFF)
          || (unpackInt(16) != 0x7FFFFFFF)
          || (unpackInt(20) != 0x7FFFFFFF)
          || (unpackInt(24) != 0x7FFFFFFF)
          || (unpackInt(28) != 0x7FFFFFFF)
          || (unpackInt(32) != 0x7FFFFFFF);
}

string EphemerisAdjunct::toString () const {
  ostringstream str;
  str << Record::toString();
  Utilities::append(str, " RotationalVelocityAlpha=",     getRotationalVelocityAlpha());
  Utilities::append(str, " RotationalVelocityBeta=",      getRotationalVelocityBeta());
  Utilities::append(str, " RotationalVelocityPhi=",       getRotationalVelocityPhi());
  Utilities::append(str, " AccelerationX=",               getAccelerationX());
  Utilities::append(str, " AccelerationY=",               getAccelerationY());
  Utilities::append(str, " AccelerationZ=",               getAccelerationZ());
  Utilities::append(str, " RotationalAccelerationAlpha=", getRotationalAccelerationAlpha());
  Utilities::append(str, " RotationalAccelerationBeta=",  getRotationalAccelerationBeta());
  Utilities::append(str, " RotationalAccelerationPhi=",   getRotationalAccelerationPhi());
  return str.str();
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

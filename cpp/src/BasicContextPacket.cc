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

#include "BasicContextPacket.h"
#include <bitset> // includes the popcount functions

using namespace vrt;
using namespace IndicatorFields;

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
  buf[0] = 0x48; // Context w/ CID, TSM=0
  buf[1] = 0x60; // TSI: UTC, TSF: Real-Time (ps) fractional timestamp, packet count =0
  buf[2] = 0x00; // 
  buf[3] = 0x08; // Packet size = 8
  return buf;
}

BasicContextPacket::BasicContextPacket (const BasicVRTPacket &p) :
  BasicVRTPacket(p)
{
  if (!isNullValue() && (getPacketType() != PacketType_Context)) {
    throw VRTException("Can not create ContextPacket from given packet");
  }
}

BasicContextPacket::BasicContextPacket (const void *buf, size_t len, bool readOnly) :
  BasicVRTPacket(buf,len,readOnly)
{
  // done
}

BasicContextPacket::BasicContextPacket (int32_t bufsize) :
  BasicVRTPacket(bufsize)
{
  bbuf[0] = 0x48; // Context w/ CID, TSM=0
  bbuf[1] = 0x60; // TSI: UTC, TSF: Real-Time (ps) fractional timestamp, packet count =0
  bbuf[2] = 0x00; // 
  bbuf[3] = 0x08; // Packet size = 8
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

// TODO - update with more CIFs
void BasicContextPacket::toStringStream (ostringstream &str) const {
  BasicVRTPacket::toStringStream(str);
  //Utilities::append(str, " ChangePacket="            ,IndicatorFieldProvider::isChangePacket()             );
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
  // ...
  Utilities::append(str, " ModeID={"                 ,getModeID()              ,"}");
}

// TODO - update for additional CIFs
bool BasicContextPacket::resetForResend (const TimeStamp &t) {
  BasicVRTPacket::resetForResend(t);
  setChangePacket(false);
  setOverRangeCount(INT64_NULL);
  setOverRange(_NULL);
  setDiscontinuous(_NULL);
  return true;
}

// TODO update setters to all use a common function like below:
/*int32_t setGenericField(int8_t cifNum, int32_t bit, int32_t xoff, bool present, int32_t cif7bit) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");
  int32_t cif7off = 0;
  int32_t fieldLen = getFieldLen(cifNum,bit);
  if (cif7bit != 0) {
    if (!present) throw VRTException("CIF7 attribute must be removed globally for all fields");
    cif7off = getCIF7Offset(cif7bit, fieldLen); // This will throw exception if cif7 not enabled, which is appropriate
    if (cif7off < 0) throw VRTException("CIF7 attribute must be enabled prior to assigning value");
    int32_t cif7length = getFieldLen(7,cif7bit, fieldLen);
    cif7off -= cif7length; // adjust to be offset to start of attribute, not end.
  // Note: If setting value w/o specifying cif7bit, the value will be set as the first attribute
  //       The alternative is to error check:
  //} else if (!isNull(val) && isCIF7Enable()) throw VRTException("CIF7 attribute must be specified when CIF7 is enabled");
  }

  int32_t poff = getOffset(cifNum, bit); // offset to beginning of [parent] field
  //bool present = !isNull(val);

  // are we adding or removing? then we have to do some extra work
  if (present != (poff>0)) {
    int32_t totalSize = getTotalFieldSize(fieldLen);
    setContextIndicatorFieldBit(cifNum, bit, present);
    poff = shiftPayload(poff, totalSize, present);
  }

  //if (present) {
  //  bbuf[poff+cif7off+xoff+getPrologueLength()] = val;
  //}
  return present? poff+cif7off+xoff+getPrologueLength() : -1;
}*/

// This is the State and Event Indicator, which has the same format as the Trailer for Data/ExtData packets
// The trailer function is leveraged to eliminate duplicate code.
boolNull BasicContextPacket::getStateEventBit (int32_t enable, int32_t indicator, int32_t cif7bit) const {
  int32_t off = getOffset(0, protected_CIF0::STATE_EVENT_mask, cif7bit);
  if (off < 0) return _NULL;
  // Note: offset passed to BasicVRTPacket is from start of bbuf
  return BasicVRTPacket::getStateEventBit(bbuf, off+getPrologueLength(), enable, indicator);
}

// This is the State and Event Indicator, which has the same format as the Trailer for Data/ExtData packets
// The trailer function is leveraged to eliminate duplicate code.
void BasicContextPacket::setStateEventBit (int32_t enable, int32_t indicator, boolNull value, int32_t cif7bit) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");
  int32_t cif7off = 0;
  int32_t fieldLen = 4; // STATE_EVENT is 4 bytes
  if (cif7bit != 0) {
    cif7off = getCIF7Offset(cif7bit, fieldLen); // This will throw exception if cif7 not enabled, which is appropriate
    if (cif7off < 0) throw VRTException("CIF7 attribute must be enabled prior to assigning value");
    int32_t cif7length = 4; // all CIF7 attributes for STATE_EVENT are 4 bytes
    cif7off -= cif7length; // adjust to be offset to start of attribute, not end.
  // Note: If setting value w/o specifying cif7bit, the value will be set as the first attribute
  //       The alternative is to error check:
  //} else if (!isNull(val) && isCIF7Enable()) throw VRTException("CIF7 attribute must be specified when CIF7 is enabled");
  }

  int32_t poff = getOffset(0, protected_CIF0::STATE_EVENT_mask); // offset to beginning of [parent] field
  if (poff < 0) {
    if (value == _NULL) return; // no State and Event Indicator, no need to set to null
    int32_t totalSize = getTotalFieldSize(fieldLen);
    setContextIndicatorFieldBit(0, protected_CIF0::STATE_EVENT_mask, true);
    poff = shiftPayload(poff, totalSize, true);
  }
  // Note: offset passed to BasicVRTPacket is from start of bbuf
  BasicVRTPacket::setStateEventBit(bbuf, poff+cif7off+getPrologueLength(), enable, indicator, value);
}

void BasicContextPacket::setRecord (int8_t cifNum, int32_t bit, const Record *val, int32_t oldLen, int32_t cif7bit) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");
  int32_t cif7off = 0;
  int32_t newLen = (val == NULL)? 0 : val->getByteLength();
  if (cif7bit != 0) {
    if (val==NULL) throw VRTException("CIF7 attribute must be removed globally for all fields");
    cif7off = getCIF7Offset(cif7bit, newLen); // This will throw exception if cif7 not enabled, which is appropriate
    if (cif7off < 0) throw VRTException("CIF7 attribute must be enabled prior to assigning value");
    int32_t cif7length = getFieldLen(7,cif7bit, newLen);
    cif7off -= cif7length; // adjust to be offset to start of attribute, not end.
  // Note: If setting value w/o specifying cif7bit, the value will be set as the first attribute
  //       The alternative is to error check:
  //} else if (!isNull(val) && isCIF7Enable()) throw VRTException("CIF7 attribute must be specified when CIF7 is enabled");
  }

  int32_t poff = getOffset(cifNum, bit); // offset to beginning of [parent] field

  if ((val == NULL) && (poff < 0)) return; // not present, no change

  if ((poff >= 0) && (val != NULL) && (oldLen == newLen)) {
    // present, no change in size
    packPayloadRecord(poff+cif7off, *val);
  }
  else {
    if (poff >= 0) {
      // remove the old one
      int32_t oldTotalSize = getTotalFieldSize(oldLen);
      setContextIndicatorFieldBit(cifNum, bit, false);
      shiftPayload(poff, oldTotalSize, false);
      poff = -poff;
    }

    if (val != NULL) {
      int32_t newTotalSize = getTotalFieldSize(newLen);
      setContextIndicatorFieldBit(cifNum, bit, true);
      poff = shiftPayload(poff, newTotalSize, true);
      packPayloadRecord(poff+cif7off, *val);
    }
  }
}

void BasicContextPacket::setContextIndicatorField0Bit (int32_t bit, bool set, bool occurrence) {
  UNUSED_VARIABLE(occurrence);
  int32_t cif0 = getContextIndicatorField0();
  int32_t val;

  if (set) val = cif0 | bit;
  else     val = cif0 & ~bit;

  if (val != cif0) {
    VRTMath::packInt(bbuf, getPrologueLength(), val);
  }
}
void BasicContextPacket::setContextIndicatorField1Bit (int32_t bit, bool set, bool occurrence) {
  UNUSED_VARIABLE(occurrence);
  if (!isCIF1Enable()) throw VRTException("Context Indicator Field 1 not enabled.");
  int32_t cif1 = getContextIndicatorField1();
  int32_t val;

  if (set) val = cif1 | bit;
  else     val = cif1 & ~bit;

  if (val != cif1) {
    VRTMath::packInt(bbuf, getPrologueLength()+4, val);
  }
}
void BasicContextPacket::setContextIndicatorField2Bit (int32_t bit, bool set, bool occurrence) {
  UNUSED_VARIABLE(occurrence);
  if (!isCIF2Enable()) throw VRTException("Context Indicator Field 2 not enabled.");
  int32_t cif2 = getContextIndicatorField2();
  int32_t val;

  if (set) val = cif2 | bit;
  else     val = cif2 & ~bit;

  if (val != cif2) {
    int32_t off = 0;
    if (isCIF1Enable()) off+=4;
    VRTMath::packInt(bbuf, getPrologueLength()+4+off, val);
  }
}
void BasicContextPacket::setContextIndicatorField3Bit (int32_t bit, bool set, bool occurrence) {
  UNUSED_VARIABLE(occurrence);
  if (!isCIF3Enable()) throw VRTException("Context Indicator Field 3 not enabled.");
  int32_t cif3 = getContextIndicatorField3();
  int32_t val;

  if (set) val = cif3 | bit;
  else     val = cif3 & ~bit;

  if (val != cif3) {
    int32_t off = 0;
    if (isCIF1Enable()) off+=4;
    if (isCIF2Enable()) off+=4;
    VRTMath::packInt(bbuf, getPrologueLength()+4+off, val);
  }
}
void BasicContextPacket::setContextIndicatorField7Bit (int32_t bit, bool set, bool occurrence) {
  UNUSED_VARIABLE(occurrence);
  if (!isCIF7Enable()) throw VRTException("Context Indicator Field 7 not enabled.");
  int32_t cif7 = getContextIndicatorField7();
  int32_t val;

  if (set) val = cif7 | bit;
  else     val = cif7 & ~bit;

  if (val != cif7) {
    int32_t off = 0;
    if (isCIF1Enable()) off+=4;
    if (isCIF2Enable()) off+=4;
    if (isCIF3Enable()) off+=4;
    VRTMath::packInt(bbuf, getPrologueLength()+4+off, val);
  }
}

void BasicContextPacket::addCIF1 (bool add, bool occurrence) {
  // TODO - either need to ensure CIF is 0 already, or also remove each set CIF bit and associated fields
  UNUSED_VARIABLE(occurrence);
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");
  if (isCIF1Enable() == add) return; // Nothing to do

  setContextIndicatorField0Bit(protected_CIF0::CIF1_ENABLE_mask, add);

  int32_t off = 4; // CIF0
  if (add) off = -off; // negative if doesn't exist
  off = shiftPayload(off, 4, add);
}

void BasicContextPacket::addCIF2 (bool add, bool occurrence) {
  // TODO - either need to ensure CIF is 0 already, or also remove each set CIF bit and associated fields
  UNUSED_VARIABLE(occurrence);
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");
  if (isCIF2Enable() == add) return; // Nothing to do

  setContextIndicatorField0Bit(protected_CIF0::CIF2_ENABLE_mask, add);

  int32_t off = 4; // CIF0
  if (isCIF1Enable()) off+=4;
  if (add) off = -off; // negative if doesn't exist
  off = shiftPayload(off, 4, add);
}

void BasicContextPacket::addCIF3 (bool add, bool occurrence) {
  // TODO - either need to ensure CIF is 0 already, or also remove each set CIF bit and associated fields
  UNUSED_VARIABLE(occurrence);
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");
  if (isCIF3Enable() == add) return; // Nothing to do

  setContextIndicatorField0Bit(protected_CIF0::CIF3_ENABLE_mask, add);

  int32_t off = 4; // CIF0
  if (isCIF1Enable()) off+=4;
  if (isCIF2Enable()) off+=4;
  if (add) off = -off; // negative if doesn't exist
  off = shiftPayload(off, 4, add);
}

void BasicContextPacket::addCIF7 (bool add, bool occurrence) {
  // TODO - either need to ensure CIF is 0 already, or also remove each set CIF bit and associated fields
  UNUSED_VARIABLE(occurrence);
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");
  if (isCIF7Enable() == add) return; // Nothing to do

  setContextIndicatorField0Bit(protected_CIF0::CIF7_ENABLE_mask, add);

  // calculate offset for CIF7 and determine if CURRENT_VALUE bit of CIF7 needs to be set
  int32_t prologlen = getPrologueLength();
  // the following are N/A for CIF7
  int32_t cif0_mask = ~(protected_CIF0::CIF1_ENABLE_mask | protected_CIF0::CIF2_ENABLE_mask
                      | protected_CIF0::CIF3_ENABLE_mask | protected_CIF0::CIF7_ENABLE_mask
                      | protected_CIF0::CHANGE_IND_mask);
  int32_t cif_bits = VRTMath::unpackInt(bbuf, prologlen) & cif0_mask;
  int32_t off = 4; // CIF0
  if (isCIF1Enable()) {
    cif_bits |= VRTMath::unpackInt(bbuf, prologlen+off);
    off+=4;
  }
  if (isCIF2Enable()) {
    cif_bits |= VRTMath::unpackInt(bbuf, prologlen+off);
    off+=4;
  }
  if (isCIF3Enable()) {
    cif_bits |= VRTMath::unpackInt(bbuf, prologlen+off);
    off+=4;
  }
  if (add) off = -off; // negative if doesn't exist
  off = shiftPayload(off, 4, add);

  if (add && (cif_bits!=0)) { // at least one CIF bit is set
    //setContextIndicatorField7Bit(protected_CIF7::CURRENT_VALUE_mask, true);
    VRTMath::packInt(bbuf, prologlen+off, protected_CIF7::CURRENT_VALUE_mask);
    // no shifting required because the values must already be there in this case
  }
}

void BasicContextPacket::setCIF7Bit (int32_t cif7bit, bool set, bool occurrence) {
  UNUSED_VARIABLE(occurrence);
  boolNull present = getCIF7Bit(cif7bit);
  if ((present!=_TRUE) && !set) return; // nothing to do
  if ((present==_TRUE) && set) return; // nothing to do
  if (present==_NULL) { // need to add CIF7 enable
    addCIF7(true);
  }
  
  // for set=true, set bit first, then adjust from top to bottom, left to right
  int8_t  cif_start = 0;  // start at CIF0
  int8_t  cif_stop  = 4;  // stop after CIF3
  int8_t  cif_inc   = 1;  // increment cifNum by 1 each iteration
  int32_t bit_start = 31; // start at the msb (31)
  int32_t bit_stop  = -1; // stop after the lsb (0)
  int32_t bit_inc   = -1; // decrement bit number by 1 each  iteration
  if (!set) { // for set=false, adjust from bottom to top, right to left first, then unset bit
    cif_start = 3;  // start at CIF3
    cif_stop  = -1; // stop after CIF0
    cif_inc   = -1; // decrement cifNum by 1 each iteration
    bit_start = 0;  // start at the lsb (0)
    bit_stop  = 32; // stop after the msb (31)
    bit_inc   = 1;  // increment bit number by 1 each iteration
    // and do not un-set the CIF7 enable bit until after adjusting the payload
  } else {
    // set the CIF7 enable bit prior to adjusting the payload
    setContextIndicatorField7Bit(cif7bit, set); // set bit
  }

  // ignore CIF0 bits that are not affected by CIF7
  // for each set bit, shift in (or out) space for cif7 attribute
  int32_t cif0_mask = ~(protected_CIF0::CIF1_ENABLE_mask | protected_CIF0::CIF2_ENABLE_mask
                      | protected_CIF0::CIF3_ENABLE_mask | protected_CIF0::CIF7_ENABLE_mask
                      | protected_CIF0::CHANGE_IND_mask); // = 0x7FFFFF00

  for (int8_t cifNum = cif_start; cifNum != cif_stop; cifNum+=cif_inc) {
    if (!isCIFEnable(cifNum)) continue;
    int32_t cif = getContextIndicatorField(cifNum);
    if (cifNum == 0) cif &= cif0_mask;
    for (int32_t bit = bit_start; bit!=bit_stop; bit+=bit_inc) {
      if (((0x1<<bit)&cif) != 0) {
        // get cif7 field size and offset
        int32_t fieldLen = getFieldLen(cifNum, (0x1<<bit));
        int32_t cif7size = getFieldLen(7, cif7bit, fieldLen);
        int32_t off = getOffset(cifNum, (0x1<<bit)); // this must be positive
        int32_t cif7off = getCIF7Offset(cif7bit, fieldLen); // if set=true, sign reflects end result, not current reality
        if (set) cif7off = -cif7off; // flip sign to reflect current reality, not end result
        if (cif7off < 0) {
          off = -off; // flip offset because cif7 field is not currently present
          cif7off += cif7size; // adjust to beginning of cif7 field rather than end
        } else {
          cif7off -= cif7size; // adjust to beginning of cif7 field rather than end
        }
        // shift in (or out) cif7field size at cif7offset
        off = shiftPayload(off+cif7off, cif7size, set);
      }
    }
  }
  if (!set) {
    setContextIndicatorField7Bit(cif7bit, set); // unset bit
  }
}

int8_t BasicContextPacket::getB (int8_t cifNum, int32_t bit, int32_t xoff, int32_t cif7bit) const {
  int32_t off = getOffset(cifNum, bit, cif7bit);
  if (isNull(off) || off < 0) return INT8_NULL;
  return bbuf[off+xoff+getPrologueLength()];
}
void BasicContextPacket::setB (int8_t cifNum, int32_t bit, int32_t xoff, int8_t val, int32_t cif7bit) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");
  int32_t cif7off = 0;
  int32_t fieldLen = getFieldLen(cifNum,bit);
  if (cif7bit != 0) {
    if (isNull(val)) throw VRTException("CIF7 attribute must be removed globally for all fields");
    cif7off = getCIF7Offset(cif7bit, fieldLen); // This will throw exception if cif7 not enabled, which is appropriate
    if (cif7off < 0) throw VRTException("CIF7 attribute must be enabled prior to assigning value");
    int32_t cif7length = getFieldLen(7,cif7bit, fieldLen);
    cif7off -= cif7length; // adjust to be offset to start of attribute, not end.
  // Note: If setting value w/o specifying cif7bit, the value will be set as the first attribute
  //       The alternative is to error check:
  //} else if (!isNull(val) && isCIF7Enable()) throw VRTException("CIF7 attribute must be specified when CIF7 is enabled");
  }

  int32_t poff = getOffset(cifNum, bit); // offset to beginning of [parent] field
  bool present = !isNull(val);

  // are we adding or removing? then we have to do some extra work
  if (present != (poff>0)) {
    int32_t totalSize = getTotalFieldSize(fieldLen);
    setContextIndicatorFieldBit(cifNum, bit, present);
    poff = shiftPayload(poff, totalSize, present);
  }

  if (present) {
    bbuf[poff+cif7off+xoff+getPrologueLength()] = val;
  }
}
int16_t BasicContextPacket::getI (int8_t cifNum, int32_t bit, int32_t xoff, int32_t cif7bit) const {
  int32_t off = getOffset(cifNum, bit, cif7bit);
  if (isNull(off) || off < 0) return INT16_NULL;
  return VRTMath::unpackShort(bbuf, off+xoff+getPrologueLength());
}
void BasicContextPacket::setI (int8_t cifNum, int32_t bit, int32_t xoff, int16_t val, int32_t cif7bit) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");
  int32_t cif7off = 0;
  int32_t fieldLen = getFieldLen(cifNum,bit);
  if (cif7bit != 0) {
    if (isNull(val)) throw VRTException("CIF7 attribute must be removed globally for all fields");
    cif7off = getCIF7Offset(cif7bit, fieldLen); // This will throw exception if cif7 not enabled, which is appropriate
    if (cif7off < 0) throw VRTException("CIF7 attribute must be enabled prior to assigning value");
    int32_t cif7length = getFieldLen(7,cif7bit, fieldLen);
    cif7off -= cif7length; // adjust to be offset to start of attribute, not end.
  // Note: If setting value w/o specifying cif7bit, the value will be set as the first attribute
  //       The alternative is to error check:
  //} else if (!isNull(val) && isCIF7Enable()) throw VRTException("CIF7 attribute must be specified when CIF7 is enabled");
  }

  int32_t poff = getOffset(cifNum, bit); // offset to beginning of [parent] field
  bool present = !isNull(val);

  // are we adding or removing? then we have to do some extra work
  if (present != (poff>0)) {
    int32_t totalSize = getTotalFieldSize(fieldLen);
    setContextIndicatorFieldBit(cifNum, bit, present);
    poff = shiftPayload(poff, totalSize, present);
  }

  if (present) {
    VRTMath::packShort(bbuf, poff+cif7off+xoff+getPrologueLength(), val);
  }
}
int32_t BasicContextPacket::getL24 (int8_t cifNum, int32_t bit, int32_t offset, int32_t cif7bit) const {
  int32_t off = getOffset(cifNum, bit, cif7bit);
  if (isNull(off) || off < 0) return INT32_NULL;
  int32_t bits = VRTMath::unpackInt(bbuf, off+getPrologueLength());
  bits = (bits & (0xFFFFFF00 >> 8*offset)) >> 8*offset;
  return bits;
}
int32_t BasicContextPacket::getL (int8_t cifNum, int32_t bit, int32_t cif7bit) const {
  int32_t off = getOffset(cifNum, bit, cif7bit);
  if (isNull(off) || off < 0) return INT32_NULL;
  return VRTMath::unpackInt(bbuf, off+getPrologueLength());
}
void BasicContextPacket::setL (int8_t cifNum, int32_t bit, int32_t val, int32_t cif7bit) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");
  int32_t cif7off = 0;
  int32_t fieldLen = getFieldLen(cifNum,bit);
  if (cif7bit != 0) {
    if (isNull(val)) throw VRTException("CIF7 attribute must be removed globally for all fields");
    cif7off = getCIF7Offset(cif7bit, fieldLen); // This will throw exception if cif7 not enabled, which is appropriate
    if (cif7off < 0) throw VRTException("CIF7 attribute must be enabled prior to assigning value");
    int32_t cif7length = getFieldLen(7,cif7bit, fieldLen);
    cif7off -= cif7length; // adjust to be offset to start of attribute, not end.
  // Note: If setting value w/o specifying cif7bit, the value will be set as the first attribute
  //       The alternative is to error check:
  //} else if (!isNull(val) && isCIF7Enable()) throw VRTException("CIF7 attribute must be specified when CIF7 is enabled");
  }

  int32_t poff = getOffset(cifNum, bit); // offset to beginning of [parent] field
  bool present = !isNull(val);

  // are we adding or removing? then we have to do some extra work
  if (present != (poff>0)) {
    int32_t totalSize = getTotalFieldSize(fieldLen);
    setContextIndicatorFieldBit(cifNum, bit, present);
    poff = shiftPayload(poff, totalSize, present);
  }

  if (present) {
    VRTMath::packInt(bbuf, poff+cif7off+getPrologueLength(), val);
  }
}
int64_t BasicContextPacket::getX (int8_t cifNum, int32_t bit, int32_t cif7bit) const {
  int32_t off = getOffset(cifNum, bit, cif7bit);
  if (isNull(off) || off < 0) return INT64_NULL;
  return VRTMath::unpackLong(bbuf, off+getPrologueLength());
}
void BasicContextPacket::setX (int8_t cifNum, int32_t bit, int64_t val, int32_t cif7bit) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");
  int32_t cif7off = 0;
  int32_t fieldLen = getFieldLen(cifNum,bit);
  if (cif7bit != 0) {
    if (isNull(val)) throw VRTException("CIF7 attribute must be removed globally for all fields");
    cif7off = getCIF7Offset(cif7bit, fieldLen); // This will throw exception if cif7 not enabled, which is appropriate
    if (cif7off < 0) throw VRTException("CIF7 attribute must be enabled prior to assigning value");
    int32_t cif7length = getFieldLen(7,cif7bit, fieldLen);
    cif7off -= cif7length; // adjust to be offset to start of attribute, not end.
  // Note: If setting value w/o specifying cif7bit, the value will be set as the first attribute
  //       The alternative is to error check:
  //} else if (!isNull(val) && isCIF7Enable()) throw VRTException("CIF7 attribute must be specified when CIF7 is enabled");
  }

  int32_t poff = getOffset(cifNum, bit); // offset to beginning of [parent] field
  bool present = !isNull(val);

  // are we adding or removing? then we have to do some extra work
  if (present != (poff>0)) {
    int32_t totalSize = getTotalFieldSize(fieldLen);
    setContextIndicatorFieldBit(cifNum, bit, present);
    poff = shiftPayload(poff, totalSize, present);
  }

  if (present) {
    VRTMath::packLong(bbuf, poff+cif7off+getPrologueLength(), val);
  }
}

UUID BasicContextPacket::getUUID (int8_t cifNum, int32_t bit, int32_t cif7bit) const {
  int32_t off = getOffset(cifNum, bit, cif7bit);
  if (isNull(off) || off < 0) return UUID(); // return NULL UUID
  return VRTMath::unpackUUID(bbuf, off+getPrologueLength());
}
void BasicContextPacket::setUUID (int8_t cifNum, int32_t bit, const UUID &val, int32_t cif7bit) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");
  int32_t cif7off = 0;
  int32_t fieldLen = getFieldLen(cifNum,bit);
  if (cif7bit != 0) {
    if (isNull(val)) throw VRTException("CIF7 attribute must be removed globally for all fields");
    cif7off = getCIF7Offset(cif7bit, fieldLen); // This will throw exception if cif7 not enabled, which is appropriate
    if (cif7off < 0) throw VRTException("CIF7 attribute must be enabled prior to assigning value");
    int32_t cif7length = getFieldLen(7,cif7bit, fieldLen);
    cif7off -= cif7length; // adjust to be offset to start of attribute, not end.
  // Note: If setting value w/o specifying cif7bit, the value will be set as the first attribute
  //       The alternative is to error check:
  //} else if (!isNull(val) && isCIF7Enable()) throw VRTException("CIF7 attribute must be specified when CIF7 is enabled");
  }

  int32_t poff = getOffset(cifNum, bit);
  bool present = !isNull(val);

  // are we adding or removing? then we have to do some extra work
  if (present != (poff>0)) {
    int32_t totalSize = getTotalFieldSize(fieldLen);
    setContextIndicatorFieldBit(cifNum, bit, present);
    poff = shiftPayload(poff, totalSize, present);
  }

  if (present) {
    VRTMath::packUUID(bbuf, poff+cif7off+getPrologueLength(), val);
  }
}

TimeStamp BasicContextPacket::getTimeStampField (int8_t cifNum, int32_t bit, int32_t cif7bit) const {
  int32_t off = getOffset(cifNum, bit, cif7bit);
  if (isNull(off) || off < 0) return TimeStamp(); // return NULL TimeStamp (int/frac modes both set to None)

  IntegerMode    tsiMode = (IntegerMode   )((bbuf[1] >> 6) & 0x3);
  FractionalMode tsfMode = (FractionalMode)((bbuf[1] >> 4) & 0x3);
  if (tsiMode == IntegerMode_None && tsfMode == FractionalMode_None)
    return TimeStamp(); // return NULL TimeStamp (int/frac modes both set to None)

  uint32_t tsi     = 0;
  uint64_t tsf     = 0;
  if (tsiMode != IntegerMode_None) {
    tsi = VRTMath::unpackUInt(bbuf, off+getPrologueLength());
    off += 4;
  }
  if (tsfMode != FractionalMode_None) {
    tsf = VRTMath::unpackULong(bbuf, off+getPrologueLength());
  }
  return TimeStamp(tsiMode, tsfMode, tsi, tsf, DOUBLE_NAN);
}
void BasicContextPacket::setTimeStampField (int8_t cifNum, int32_t bit, const TimeStamp &val, int32_t cif7bit) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");

  // Get packet timestamp info from TSI/TSF
  IntegerMode    tsiModePkt = (IntegerMode   )((bbuf[1] >> 6) & 0x3);
  FractionalMode tsfModePkt = (FractionalMode)((bbuf[1] >> 4) & 0x3);
  if (tsiModePkt == IntegerMode_None && tsfModePkt == FractionalMode_None) {
    if (!isNull(val))
      throw VRTException("Can not set TimeStamp field on VRTPacket without a TimeStamp.");
    return; // nothing to do
  }

  // Get timestamp info from new value
  if (!isNull(val) && (tsiModePkt != val.getIntegerMode() || tsfModePkt != val.getFractionalMode()))
    throw VRTException("Can not set TimeStamp field to format not consistent with VRTPacket TimeStamp format.");

  int32_t len = 0;
  if (tsiModePkt != IntegerMode_None) len+=4;
  if (tsfModePkt != FractionalMode_None) len+=8;

  int32_t cif7off = 0;
  if (cif7bit != 0) {
    if (isNull(val)) throw VRTException("CIF7 attribute must be removed globally for all fields");
    cif7off = getCIF7Offset(cif7bit, len); // This will throw exception if cif7 not enabled, which is appropriate
    if (cif7off < 0) throw VRTException("CIF7 attribute must be enabled prior to assigning value");
    int32_t cif7length = getFieldLen(7,cif7bit, len);
    cif7off -= cif7length; // adjust to be offset to start of attribute, not end.
  // Note: If setting value w/o specifying cif7bit, the value will be set as the first attribute
  //       The alternative is to error check:
  //} else if (!isNull(val) && isCIF7Enable()) throw VRTException("CIF7 attribute must be specified when CIF7 is enabled");
  }

  int32_t poff = getOffset(cifNum, bit);
  bool present = !isNull(val);

  // are we adding or removing? then we have to do some extra work
  if (present != (poff>0)) {
    int32_t totalSize = getTotalFieldSize(len);
    setContextIndicatorFieldBit(cifNum, bit, present);
    poff = shiftPayload(poff, totalSize, present);
  }

  if (present) {
    if (tsiModePkt != IntegerMode_None) {
      VRTMath::packUInt(bbuf, poff+cif7off+getPrologueLength(), val.getTimeStampInteger());
      poff += 4;
    }
    if (tsfModePkt != FractionalMode_None) {
      VRTMath::packULong(bbuf, poff+cif7off+getPrologueLength(), val.getTimeStampFractional());
    }
  }
}

void BasicContextPacket::setDataPayloadFormat (const PayloadFormat &val, IndicatorFieldEnum_t cif7field) {
  int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
  if (isNull(val)) {
    setX(0, protected_CIF0::DATA_FORMAT_mask, INT64_NULL, cif7bit);
  } else if (isNull(val.getBits())) {
    // XXX - special case: setX will interpret the request as a removal of the field (NULL)
    // First, let setX do the heavy lifting with a non-NULL value
    setX(0, protected_CIF0::DATA_FORMAT_mask, val.getBits()+1, cif7bit);
    // now assign the correct value
    int32_t off = getOffset(0, protected_CIF0::DATA_FORMAT_mask, cif7bit);
    VRTMath::packLong(bbuf, off+getPrologueLength(), val.getBits());
  } else {
    setX(0, protected_CIF0::DATA_FORMAT_mask, val.getBits(), cif7bit);
  }
}

int32_t BasicContextPacket::getTotalFieldSize (int32_t fieldLen, bool occurrence) const {
  UNUSED_VARIABLE(occurrence);
  if (!isCIF7Enable()) return fieldLen;
  int32_t cif7 = getContextIndicatorField7();
  int32_t sz = (bitCount(cif7 & protected_CIF7::CTX_4_OCTETS    ) * 4       )
             + (bitCount(cif7 & protected_CIF7::CTX_SAME_OCTETS ) * fieldLen);
  return sz;
}

int32_t BasicContextPacket::getFieldLen (int8_t cifNum, int32_t field, int32_t parent) const {
  switch(cifNum) {
  case 0:
    if ((field & protected_CIF0::CTX_4_OCTETS ) != 0) return  4;
    if ((field & protected_CIF0::CTX_8_OCTETS ) != 0) return  8;
    if ((field & protected_CIF0::CTX_44_OCTETS) != 0) return 44;
    if ((field & protected_CIF0::CTX_52_OCTETS) != 0) return 52;
    if (field == protected_CIF0::GPS_ASCII_mask) {
      int32_t prologlen = getPrologueLength();
      int off = getOffset(cifNum, field);
      if (off<0) return -1;
      return VRTMath::unpackInt(bbuf, prologlen+4+off)*4+8;
    }
    if (field == protected_CIF0::CONTEXT_ASOC_mask) {
      int32_t prologlen = getPrologueLength();
      int off = getOffset(cifNum, field);
      if (off<0) return -1;
      int32_t source = VRTMath::unpackShort(bbuf, prologlen+0+off) & 0x01FF;
      int32_t system = VRTMath::unpackShort(bbuf, prologlen+2+off) & 0x01FF;
      int32_t vector = VRTMath::unpackShort(bbuf, prologlen+4+off) & 0xFFFF;
      int32_t asynch = VRTMath::unpackShort(bbuf, prologlen+6+off) & 0x7FFF;
      if ((VRTMath::unpackShort(bbuf, prologlen+6+off) & 0x8000) != 0) asynch *= 2;
      return (source + system + vector + asynch)*4+8;
    }
    break;
  case 1:
    if ((field & protected_CIF1::CTX_4_OCTETS ) != 0) return 4;
    if ((field & protected_CIF1::CTX_8_OCTETS ) != 0) return 8;
    if ((field & protected_CIF1::CTX_52_OCTETS ) != 0) return 52;
    if ((field & protected_CIF1::CTX_ARR_OF_RECS ) != 0) {
      int32_t prologlen = getPrologueLength();
      int off = getOffset(cifNum, field);
      if (off<0) return -1;
      return VRTMath::unpackInt(bbuf, prologlen+off)*4;
    }
    break;
  case 2:
    if ((field & protected_CIF2::CTX_4_OCTETS ) != 0) return 4;
    if ((field & protected_CIF2::CTX_16_OCTETS ) != 0) return 16;
    break;
  case 3:
    if ((field & protected_CIF3::CTX_4_OCTETS ) != 0) return 4;
    if ((field & protected_CIF3::CTX_8_OCTETS ) != 0) return 8;
    if ((field & protected_CIF3::CTX_TSTAMP_OCTETS ) != 0) {
      // BasicVRTPacket doesn't provide us any help, so do it manually
      //      - for TSI and TSF, 0x00 = no timestamp (+0 octets)
      //      - for TSI!=0, integer tstamp present (+4 octets)
      //      - for TSF!=0, fractional tstamp present (+8 octets)
      // TSI: ((bbuf[1] >> 6) & 0x3);
      // TSF: ((bbuf[1] >> 4) & 0x3);
      int32_t len = 0;
      if (((bbuf[1] >> 6) & 0x3) != 0) len+=4;
      if (((bbuf[1] >> 4) & 0x3) != 0) len+=8;
      return len;
    }
    break;
  case 7:
    if ((field & protected_CIF7::CTX_4_OCTETS    ) != 0) return 4;
    if ((field & protected_CIF7::CTX_SAME_OCTETS ) != 0) return parent;
  default:
    throw VRTException("Invalid Context Indicator Field number.");
  } // switch

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

int32_t BasicContextPacket::getCIF7Offset (int32_t attr, int32_t len, bool occurrence) const {
  UNUSED_VARIABLE(occurrence);
  if (!isCIF7Enable()) throw VRTException("CIF7 is not enabled.");
  int32_t cif7 = getContextIndicatorField7();
  //if (isNull(cif7)) {
    // 0x80000000 is INT32_NULL, which is also CURRENT_VALUE_mask
    //throw VRTException("CIF7 is not enabled.");
  //}
  //int32_t mask = ~(attr ^ (attr - 1));
  // This offset needs to be calculated differently from other CIFs because an
  // offset of 0 is permitted, and we can't return -0 to indicate not present.
  // Instead, we return offet+length of the field. This is essentially the
  // offset to the end of the field instead of the beginning.
  // A value of 0 is now only possible if the attr is 0 (i.e. no CIF7 attribute)
  int32_t mask = ~(attr - 1); // this will include the field of interest in the offset
  int32_t m    = (cif7 & mask) | attr;
  int32_t off  = (bitCount(m & protected_CIF7::CTX_4_OCTETS    ) * 4  )
               + (bitCount(m & protected_CIF7::CTX_SAME_OCTETS ) * len);
  return ((cif7 & attr) != 0)? off: -off;  // -off if not present
}

int32_t __attribute__((hot)) BasicContextPacket::getOffset (int8_t cifNum, int32_t field) const {
  // Since this is the most-used method in the class and often sees millions and
  // millions of calls within a typical application, it has been heavily
  // optimized to remove any loops and minimize the number of branches and other
  // operations required. It is assumed that bitCount(..) will be in-lined during
  // the compile.
  //
  // References:
  //   [1] Warren, Henry S. Jr. "Hacker's Delight." Addison-Wesley. 2002.
  //       ISBN 0-201-91465-4


  // Set the mask for all fields before 'field'. This uses the "x^(x-1)" algorithm
  // from [1] (section 2-1) to set the field bit and all bits to the right, and
  // then inverts it to get all bits left of 'field' to be set.
  int32_t mask0, mask1, mask2, mask3;
  int32_t field0, field1, field2, field3;
  mask0 = mask1 = mask2 = mask3 = 0xFFFFFFFF; // mask to use for CIFs before cifNum of interest
  field0 = field1 = field2 = field3 = 0x0;   // field to use for CIFs before cifNum of interest
  switch(cifNum) {
    case 0:
      mask0 = ~(field ^ (field - 1));
      field0 = field;
      break;
    case 1:
      mask1 = ~(field ^ (field - 1));
      field1 = field;
      break;
    case 2:
      mask2 = ~(field ^ (field - 1));
      field2 = field;
      break;
    case 3:
      mask3 = ~(field ^ (field - 1));
      field3 = field;
      break;
    case 7:
      throw VRTException("Invalid Context Indicator Field number (CIF7).");
      break;
    default:
      throw VRTException("Invalid Context Indicator Field number (undefined).");
      break;
  } // switch(cifNum)

  // If CIF7 is in use, calculate offset multiple
  // could just do isCIF7Enable(), but we need cif0 and we likely need header length later
  // so we break out that function here to store off the parts we'll re-use
  int32_t prologlen = getPrologueLength();
  int32_t cif0 = VRTMath::unpackInt(bbuf, prologlen);
  int32_t cifOffset = 4; // offset of next CIF; currently 'next' is after CIF0
  int32_t cif7Mult = 1;  // Multiplier to apply to all fields for CIF7 attributes
  int32_t cif7Add = 0;   // octets to add to all fields for CIF7 attributes
  if((cif0 & protected_CIF0::CIF7_ENABLE_mask) != 0) {
    // Count number of CIFs enabled, add 4 bytes to offset for each
    // Note: CIF7 is enabled but shouldn't count towards offset (+4 bytes of offset)
    //       CIF0 has no enable but should count towards offset (-4 bytes of offset)
    //       +4 and -4 ==> 0 adjustement, so it works out as planned.
    int32_t cif7 = VRTMath::unpackInt(bbuf, prologlen+(bitCount(cif0 & 0xFF) * 4));
    // Calcluate multiplier (i.e. if max/min/actual, mult=3)
    cif7Add  = bitCount((cif7 & protected_CIF7::CTX_4_OCTETS   )) * 4;
    cif7Mult = bitCount((cif7 & protected_CIF7::CTX_SAME_OCTETS));
  }

  // Start offset calculation with the CIFs present
  int32_t off = 4 + (bitCount(cif0 & 0xFF) *  4); // 4 for CIF0 + 4 for each other CIF present

  /** CIF0 calculation
   */

  // For efficiency we compute the offset based on an applied bit mask and a few
  // multiplies. For *52 we do an *8 plus *44 to avoid the overhead of an extra
  // bitCount(..). In the below code, we the CTX_8_OCTETS|CTX_52_OCTETS and the
  // CTX_44_OCTETS|CTX_52_OCTETS should be in-lined during the compile; also the
  // *4 and *8 should be optimized (by the compiler) to bit-shifts where
  // appropriate.
  int32_t m    = cif0 & (mask0 & 0xFFFFFF00); // clear CIF enable bits of CIF0 mask - they're already counted
  int32_t off0 = (bitCount(m & protected_CIF0::CTX_4_OCTETS ) * (cif7Add + ( 4*cif7Mult) ))
               + (bitCount(m & protected_CIF0::CTX_8_OCTETS ) * (cif7Add + ( 8*cif7Mult) ))
               + (bitCount(m & protected_CIF0::CTX_44_OCTETS) * (cif7Add + (44*cif7Mult) ))
               + (bitCount(m & protected_CIF0::CTX_52_OCTETS) * (cif7Add + (52*cif7Mult) ));

  // GPS_ASCII length is variable so we handle it separately if applicable. Note
  // that following our initial off+off0 computation the offset is now queued up to
  // point to the start of the GPS_ASCII field.
  if (field0 < protected_CIF0::GPS_ASCII_mask) {
    if ((cif0 & protected_CIF0::GPS_ASCII_mask) != 0) {
      // TODO - when and where does cif7Mult apply? perhaps not all field sizes are affected.    
      off0 += (VRTMath::unpackInt(bbuf, prologlen+off+off0+4)*4+8)*cif7Mult + cif7Add;
    }
    

    // CONTEXT_ASOC length is also variable, since it comes after GPS_ASCII
    // we nest it here so the check can be skipped in the 80% use case. Note
    // that off+off0 should now point to the start of the CONTEXT_ASOC field.
    if (field0 < protected_CIF0::CONTEXT_ASOC_mask) {
      if ((cif0 & protected_CIF0::CONTEXT_ASOC_mask) != 0) {
        int32_t source = VRTMath::unpackShort(bbuf, prologlen+off+off0+0) & 0x01FF;
        int32_t system = VRTMath::unpackShort(bbuf, prologlen+off+off0+2) & 0x01FF;
        int32_t vector = VRTMath::unpackShort(bbuf, prologlen+off+off0+4) & 0xFFFF;
        int32_t asynch = VRTMath::unpackShort(bbuf, prologlen+off+off0+6) & 0x7FFF;
        if ((VRTMath::unpackShort(bbuf, prologlen+6+off+off0) & 0x8000) != 0) asynch *= 2;
        off0 += ((source + system + vector + asynch)*4+8)*cif7Mult + cif7Add;
      }
    }
  }
  off += off0;
  if (cifNum == 0) return ((cif0 & field) != 0)? off: -off;  // -off if not present

  // CIF1
  if((cif0 & protected_CIF0::CIF1_ENABLE_mask) != 0) {
    int32_t cif1 = VRTMath::unpackInt(bbuf, prologlen+cifOffset);
    cifOffset += 4; // increment for next CIF
    // Only count fields before the first variable length field (PNT_VECT_3D_ST)
    // Note: all of which are 4-octets, so only check that one
    m = cif1 & mask1 & 0xE0000000;
    int32_t off1 = (bitCount(m & protected_CIF1::CTX_4_OCTETS) * (cif7Add + (4*cif7Mult) ));

    // PNT_VECT_3D_ST length is Array-of-Records format, which has variable size
    // so we handle it separately if applicable. Note that off+off1 is the offset
    // up to point to the start of the PNT_VECT_3D_ST field.
    if (field1 < protected_CIF1::PNT_VECT_3D_ST_mask) {
      if ((cif1 & protected_CIF1::PNT_VECT_3D_ST_mask) != 0) {
        // TODO - when and where does cif7Mult apply? perhaps not all field sizes are affected.    
        off1 += (VRTMath::unpackInt(bbuf, prologlen+off+off1)*4)*cif7Mult + cif7Add;
      }

      // Only count fields not yet counted (i.e. after PNT_VECT_3D_ST) and before
      // the next variable length field (CIFS_ARRAY).
      // Note: all of which are either 4- or 8-octets, so only check those
      m = cif1 & mask1 & 0x0FFFF000;
      off1 += (bitCount(m & (protected_CIF1::CTX_4_OCTETS)) * (cif7Add + (4*cif7Mult) ))
            + (bitCount(m & (protected_CIF1::CTX_8_OCTETS)) * (cif7Add + (8*cif7Mult) ));

      // CIFS_ARRAY is also Array-of-Records format, and since it comes after
      // PNT_VECT_3D_ST, we nest it here so the check can be skipped.
      if (field1 < protected_CIF1::CIFS_ARRAY_mask) {
        if ((cif1 & protected_CIF1::CIFS_ARRAY_mask) != 0) {
          // TODO - when and where does cif7Mult apply? perhaps not all field sizes are affected.    
          off1 += (VRTMath::unpackInt(bbuf, prologlen+off+off1)*4)*cif7Mult + cif7Add;
        }

        // Only count SPECTRUM since it is the only field between the previous
        // and next variable length fields. SPECTRUM is 52-octets.
        if ((cif1 & mask1 & protected_CIF1::SPECTRUM_mask) != 0) off1 += (cif7Add + (52*cif7Mult));

        // SECTOR_SCN_STP is also Array-of-Records format, and since it comes after
        // CIFS_ARRAY, we nest it here so the check can be skipped.
        if (field1 < protected_CIF1::SECTOR_SCN_STP_mask) {
          if ((cif1 & protected_CIF1::SECTOR_SCN_STP_mask) != 0) {
            // TODO - when and where does cif7Mult apply? perhaps not all field sizes are affected.    
            off1 += (VRTMath::unpackInt(bbuf, prologlen+off+off1)*4)*cif7Mult + cif7Add;
          }

          // Only field between the previous and next variable length fields is reserved (i.e. 0).
          // Nothing to add here for CIF1_RESERVED_8.

          // INDEX_LIST is also Array-of-Records format, and since it comes after
          // SECTOR_SCN_STP, we nest it here so the check can be skipped.
          if (field1 < protected_CIF1::INDEX_LIST_mask) {
            if ((cif1 & protected_CIF1::INDEX_LIST_mask) != 0) {
              // TODO - when and where does cif7Mult apply? perhaps not all field sizes are affected.    
              off1 += (VRTMath::unpackInt(bbuf, prologlen+off+off1)*4)*cif7Mult + cif7Add;
            }
          }
          
          // Finally, count the remaining fields, which are all either 4- or 8-octets.
          m = cif1 & mask1 & 0x0000007F;
          off1 += (bitCount(m & (protected_CIF1::CTX_4_OCTETS)) * (cif7Add + (4*cif7Mult) ))
                + (bitCount(m & (protected_CIF1::CTX_8_OCTETS)) * (cif7Add + (8*cif7Mult) ));
        }
      }
    }

    off += off1;
    if (cifNum == 1) return ((cif1 & field) != 0)? off : -off;  // -off if not present
  } else {
    // return NULL to indicate invalid CIF
    if (cifNum == 1) return INT32_NULL;
  }

  // CIF2
  if((cif0 & protected_CIF0::CIF2_ENABLE_mask) != 0) {
    int32_t cif2 = VRTMath::unpackInt(bbuf, prologlen+cifOffset);
    cifOffset += 4; // increment for next CIF
    m    = cif2 & mask2;
    int32_t off2 = (bitCount(m & protected_CIF2::CTX_4_OCTETS)  * (cif7Add + ( 4*cif7Mult) ))
                 + (bitCount(m & protected_CIF2::CTX_16_OCTETS) * (cif7Add + (16*cif7Mult) ));
    off += off2;
    if (cifNum == 2) return ((cif2 & field) != 0)? off : -off;  // -off if not present
  } else {
    // return NULL to indicate invalid CIF
    if (cifNum == 2) return INT32_NULL;
  }

  // CIF3
  if((cif0 & protected_CIF0::CIF3_ENABLE_mask) != 0) {
    int32_t cif3 = VRTMath::unpackInt(bbuf, prologlen+cifOffset);
    cifOffset += 4; // increment for next CIF
    m    = cif3 & mask3;
    int32_t off3 = (bitCount(m & protected_CIF3::CTX_4_OCTETS) * (cif7Add + (4*cif7Mult) ))
                 + (bitCount(m & protected_CIF3::CTX_8_OCTETS) * (cif7Add + (8*cif7Mult) ));

    // AGE length is variable so we handle it separately if applicable.
    if (field3 < protected_CIF3::AGE_mask) {
      // AGE and SHELF_LIFE variable length fields are both dependant on TSI/TSF
      // calculate once for both.
      // BasicVRTPacket doesn't provide us any help, so do it manually
      //      - for TSI and TSF, 0x00 = no timestamp (+0 octets)
      //      - for TSI!=0, integer tstamp present (+4 octets)
      //      - for TSF!=0, fractional tstamp present (+8 octets)
      // TSI: ((bbuf[1] >> 6) & 0x3);
      // TSF: ((bbuf[1] >> 4) & 0x3);
      int32_t tstampLen = 0;
      if (((bbuf[1] >> 6) & 0x3) != 0) tstampLen+=4;
      if (((bbuf[1] >> 4) & 0x3) != 0) tstampLen+=8;

      if ((cif3 & protected_CIF3::AGE_mask) != 0) {
        off3 += (cif7Add + (tstampLen*cif7Mult));
      }

      // SHELF_LIFE is also variable...
      if (field3 < protected_CIF3::SHELF_LIFE_mask) {
        if ((cif3 & protected_CIF3::SHELF_LIFE_mask) != 0) {
          off3 += (cif7Add + (tstampLen*cif7Mult));
        }
      }
    }

    off += off3;
    if (cifNum == 3) return ((cif3 & field) != 0)? off : -off;  // -off if not present
  } else {
    // return NULL to indicate invalid CIF
    if (cifNum == 3) return INT32_NULL;
  }
  // should never get here since CIF3 defines the last potential part of payload
  throw VRTException("Invalid Context Indicator Field number (undefined).");
}

// TODO - update with new CIFs
int32_t BasicContextPacket::getFieldCount () const {
  return BasicVRTPacket::getFieldCount() + 35;
}

// TODO - update with new CIFs
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

// TODO - update with new CIFs
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

// TODO - update with new CIFs
Value* BasicContextPacket::getField (int32_t id) const {
  switch (id - BasicVRTPacket::getFieldCount()) {
    //case  0: return new Value(IndicatorFieldProvider::isChangePacket());
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

// TODO - update with new CIFs
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

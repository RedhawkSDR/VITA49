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

#include "BasicAcknowledgePacket.h"
#include <bitset> // includes the popcount functions

using namespace vrt;
using namespace protected_PSP;

//TODO consider moving to a .h file to be included
//     rather than duplicating in multiple .cc files (also in BAsicContextPacket.cc)
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


  /*****************************************************************************/
  /*****************************************************************************/
  /**                                                                         **/
  /**                        BasicAcknowledgePacket (AckX and AckX)           **/
  /**                                                                         **/
  /*****************************************************************************/
  /*****************************************************************************/

/**  Creates the default prologue for the packet. This is defined separate from the class to account
 *  for the systems/compilers that disallow calling of subclass methods from the constructor.
 */
static inline vector<char> BasicAcknowledgePacket_createDefaultPacket () {
  vector<char> buf(BasicVRTPacket::MAX_PROLOGUE_LENGTH);
  buf[0]  = 0x6C; // Ack w/ CID, not StaleTs, not Cancel
  buf[1]  = 0x60; // TSI: UTC, TSF: Real-Time (ps) fractional timestamp, packet count =0
  buf[2]  = 0x00; // 
  buf[3]  = 0x11; // Packet size = 17 (full header, full psp, no payload!)
  buf[28] = 0xF0; // Has 128-bit ControlleeID and 128-bit ControllerID
  buf[29] = 0x20; // AckX
  return buf;
}

//======================================================================
// CONSTRUCTORS
//======================================================================

// Constructors
BasicAcknowledgePacket::BasicAcknowledgePacket (const BasicVRTPacket &p) :
  BasicVRTPacket(p)
{
  if (!isNullValue() && (getPacketType() != PacketType_Command)) {
    throw VRTException("Can not create AcknowledgePacket from given packet");
  }
}

BasicAcknowledgePacket::BasicAcknowledgePacket (const BasicVRTPacket &p, int64_t classID) :
  BasicVRTPacket(p)
{
  if (getPacketType() != PacketType_Command) {
    throw VRTException("Can not create AcknowledgePacket from given packet");
  }
  setClassIdentifier(classID);
}

BasicAcknowledgePacket::BasicAcknowledgePacket (const void *buf, size_t len, bool readOnly) :
  BasicVRTPacket(buf,len,readOnly)
{
  // done
}

BasicAcknowledgePacket::BasicAcknowledgePacket () :
  BasicVRTPacket(BasicAcknowledgePacket_createDefaultPacket(),false)
{
  // done
}

BasicAcknowledgePacket::BasicAcknowledgePacket (int32_t bufsize) :
  BasicVRTPacket(bufsize)
{
  // done
}

BasicAcknowledgePacket::BasicAcknowledgePacket (const vector<char> &buf, ssize_t start, ssize_t end, bool readOnly) :
  BasicVRTPacket(buf, start, end, readOnly)
{
  // done
}

BasicAcknowledgePacket::BasicAcknowledgePacket (const vector<char> &buf, bool readOnly) :
  BasicVRTPacket::BasicVRTPacket(buf,readOnly)
{
  // done
}

BasicAcknowledgePacket::BasicAcknowledgePacket (vector<char> *buf_ptr, bool readOnly) :
  BasicVRTPacket(buf_ptr, readOnly)
{
  // done
}

BasicAcknowledgePacket::BasicAcknowledgePacket (const BasicVRTPacket &p, PacketType type, int64_t classID,
                                        int32_t minPayloadLength, int32_t maxPayloadLength) :
  BasicVRTPacket(p,type,classID,minPayloadLength,maxPayloadLength)
{
  if (type != PacketType_Command) {
    throw VRTException("BasicAcknowledgePacket can only be created with type=Command");
  }
}

// UTILITIES

void BasicAcknowledgePacket::toStringStream (std::ostringstream &str) const {
  BasicCommandPacket::toStringStream(str);
  // TODO - add new stuff, like list of warnings and errors?
  //Utilities::append(str, " ChangePacket="            ,isChangePacket()             );
  //Utilities::append(str, " ReferencePointIdentifier=",getReferencePointIdentifier());
  //Utilities::append(str, " Bandwidth="               ,getBandwidth()         ,"Hz" );
}

int32_t BasicAcknowledgePacket::getCIF7Offset (int32_t attr, int32_t len, bool occurrence) const {
  UNUSED_VARIABLE(len);
  if (!isCIF7Enable(occurrence)) throw VRTException("CIF7 is not enabled.");
  int32_t cif7 = getContextIndicatorField7(occurrence);
  //int32_t mask = ~(attr ^ (attr - 1));
  // This offset needs to be calculated differently from other CIFs because an
  // offset of 0 is permitted, and we can't return -0 to indicate not present.
  // Instead, we return offet+length of the field. This is essentially the
  // offset to the end of the field instead of the beginning.
  // A value of 0 is now only possible if the attr is 0 (i.e. no CIF7 attribute)
  int32_t mask = ~(attr - 1); // this will include the field of interest in the offset
  int32_t m    = (cif7 & mask) | attr;
  int32_t off  = (bitCount(m) * 4); // All fields are 32-bits
  return ((cif7 & attr) != 0)? off: -off;  // -off if not present
}

int32_t BasicAcknowledgePacket::getOffset (int8_t cifNum, int32_t field) const {

  //      - if no warnings/errors, no payload, including no CIF0 that's normally always there
  //      - Ack-W and Ack-E bits of Ctrl/AckSettings field are set in control packet to indicate
  //        whether or not to include Warn/Error IndicatorFields, and Ack-W and Ack-E are set in
  //        AckV and AckX packets if Warn/Error occurred regardless of whether Warn/Error fields
  //        were requested.
  //      - *XXX* SO, for AckV and AckX packets:
  //          - If Ack-W bit is set, WIF0 must be present, even if all 0's.
  //          - If Ack-E bit is set, EIF0 must be present, even if all 0's.
  //          - If Ack-W bit is unset, WIF0 must not be present.
  //          - If Ack-E bit is unset, EIF0 must not be present.
  //          - Free-form texual error message, if present, will be determined based on packet
  //            size. If payload length is larger than is necessary for all CIF and associated
  //            32-bit warn/error fields, the remaining bytes are the texual message.
  //          - To be safe, check for empty payload before continuing. This protects against the
  //            case where Ack-W or Ack-E is set but there is no WIF0 or EIF0 or texual message.
  if (getPayloadLength() == 0) {
    //throw VRTException("Cannot get offset with zero-length payload.");
    return INT32_NULL;
  }

  // Set the mask for all fields before 'field'. This uses the "x^(x-1)" algorithm
  // from [1] (section 2-1) to set the field bit and all bits to the right, and
  // then inverts it to get all bits left of 'field' to be set.
  int32_t mask0, mask1, mask2, mask3, warnMask;
  mask0 = mask1 = mask2 = mask3 = warnMask = 0xFFFFFFFF; // mask to use for CIFs before cifNum of interest
  // warnMask  OR'd  w/ all other masks for wifs
  // if error we're interested in, it'll include all warnings in offset
  // if warning we're interested in, set warnMask to have no affect
  //if ( (cifNum >> 3) >= 1) { // divide by 8 to see if looking for error
  if ( (cifNum >> 3) == 0) { // divide by 8 to see if looking for warning
    warnMask = 0x0;
  }
  // if valid WIF or EIF (i.e. <=15), this will mod 8 to ignore whether warning or error
  // if not, this will preserve the most-significant-bits and cause bit3 to be 0
  switch(cifNum & 0xF7) {
    case 0:
      mask0 = ~(field ^ (field - 1));
      break;
    case 1:
      mask1 = ~(field ^ (field - 1));
      break;
    case 2:
      mask2 = ~(field ^ (field - 1));
      break;
    case 3:
      mask3 = ~(field ^ (field - 1));
      break;
    case 7:
      throw VRTException("Invalid Indicator Field number (CIF7).");
      //break;
    default:
      // If cifNum == 0xFF, we're looking for the free-form message at the end
      if (cifNum != FREE_FORM_MESSAGE) {
        throw VRTException("Invalid Indicator Field number (undefined).");
      } // else looking for free-form message; continue without error
      break;
  } // switch(cifNum)

  int32_t prologlen = getPrologueLength();
  int32_t cifOffset = 0; // offset of next CIF; currently next is whatever is first
  int32_t cifLen    = 0; // length of all CIFs; currently none known
  int32_t off       = 0; // offset of field desired from end of CIF block

  // If CIF7 is in use, calculate offset multiple
  // since all warnings/errors are 4 bytes, we only need the multiple
  // TODO - does CIF7 even apply to this class? look at V49.2 standard
  //  - need to update CIF7 bit setter for WIF/EIF
  //  - WIF7 and EIF7 may be different, so need wif7Mult and eif7Mult
  int32_t wif7Mult = 1;
  int32_t eif7Mult = 1;

  // determine wif0/eif0 and total wif/cif block size
  int32_t wif0 = 0x0; // default for when it does not exist
  int32_t eif0 = 0x0; // default for when it does not exist
  if (getWarningsGenerated()) {
    // update wif0, cifLen, offset, and wif7Mult
    wif0 = VRTMath::unpackInt(bbuf, prologlen);
    if ((wif0 & protected_CIF0::CIF7_ENABLE_mask) != 0) {
      int32_t wif7 = VRTMath::unpackInt(bbuf, prologlen+(bitCount(wif0&0xFF)*4));
      wif7Mult = bitCount(wif7);
    }
    cifOffset += 4;
    cifLen += (4 + (bitCount((wif0 & 0x000000FF)                   ) * 4)); // 4 bytes for each WIF present
    off    +=      (bitCount((wif0 & 0x7FFFFF00) & (mask0|warnMask)) * 4*wif7Mult);  // 4 bytes for each warning, excluding CHANGE_IND bit
  } else if ((cifNum&0xF8) == 0x00) { // if WIF0-7
    return INT32_NULL;
  }
  // XXX - always execute this even if looking for warning offset
  //       needed to update cifLen
  if (getErrorsGenerated()) {
    // update eif0 and cifLen values, but not offset values yet
    eif0 = VRTMath::unpackInt(bbuf, prologlen+cifLen);
    if ((eif0 & protected_CIF0::CIF7_ENABLE_mask) != 0) {
      int32_t eif7 = VRTMath::unpackInt(bbuf, prologlen+cifLen+(bitCount(eif0&0xFF)*4));
      eif7Mult = bitCount(eif7);
    }
    cifLen += (4 + (bitCount((eif0 & 0x000000FF)                   ) * 4)); // 4 bytes for each EIF present
  } else if ((cifNum&0xF8) == 0x08) { // if EIF0-7
    return INT32_NULL;
  }
  if (cifNum==0) return ((wif0 & field) != 0)? (off+cifLen): -(off+cifLen);  // -off if not present

  // now do wif1,2,3,7 
  if (getWarningsGenerated()) {

    // WIF1
    if((wif0 & protected_CIF0::CIF1_ENABLE_mask) != 0) {
      int32_t wif1 = VRTMath::unpackInt(bbuf, prologlen+cifOffset);
      cifOffset += 4; // increment for next CIF
      off += (bitCount(wif1 & (mask1|warnMask)) * 4*wif7Mult);  // 4 bytes for each warning
      if (cifNum == 1) return ((wif1 & field) != 0)? (off+cifLen) : -(off+cifLen);  // -off if not present
    } else if (cifNum == 1) {
      return INT32_NULL;
    }

    // WIF2
    if((wif0 & protected_CIF0::CIF2_ENABLE_mask) != 0) {
      int32_t wif2 = VRTMath::unpackInt(bbuf, prologlen+cifOffset);
      cifOffset += 4; // increment for next CIF
      off += (bitCount(wif2 & (mask2|warnMask)) * 4*wif7Mult);  // 4 bytes for each warning
      if (cifNum == 2) return ((wif2 & field) != 0)? (off+cifLen) : -(off+cifLen);  // -off if not present
    } else if (cifNum == 2) {
      return INT32_NULL;
    }

    // WIF3
    if((wif0 & protected_CIF0::CIF3_ENABLE_mask) != 0) {
      int32_t wif3 = VRTMath::unpackInt(bbuf, prologlen+cifOffset);
      cifOffset += 4; // increment for next CIF
      off += (bitCount(wif3 & (mask3|warnMask)) * 4*wif7Mult);  // 4 bytes for each warning
      if (cifNum == 3) return ((wif3 & field) != 0)? (off+cifLen) : -(off+cifLen);  // -off if not present
    } else if (cifNum == 3) {
      return INT32_NULL;
    }

    // WIF7
    if((wif0 & protected_CIF0::CIF7_ENABLE_mask) != 0) {
      cifOffset += 4; // increment for next CIF
    }
  }

  // now do eif0,1,2,3,7
  if (getErrorsGenerated()) {

    // EIF0
    // already got eif0 above, use it.
    cifOffset += 4;
    off += (bitCount((eif0 & 0x7FFFFF00) & mask0) * 4*eif7Mult);  // 4 bytes for each error, excluding CHANGE_IND bit
    if (cifNum==8) return ((eif0 & field) != 0)? (off+cifLen): -(off+cifLen);  // -off if not present

    // EIF1
    if((eif0 & protected_CIF0::CIF1_ENABLE_mask) != 0) {
      int32_t eif1 = VRTMath::unpackInt(bbuf, prologlen+cifOffset);
      cifOffset += 4; // increment for next CIF
      off += (bitCount(eif1 & mask1) * 4*eif7Mult);  // 4 bytes for each error
      if (cifNum == 9) return ((eif1 & field) != 0)? (off+cifLen) : -(off+cifLen);  // -off if not present
    } else if (cifNum == 9) {
      return INT32_NULL;
    }

    // EIF2
    if((eif0 & protected_CIF0::CIF2_ENABLE_mask) != 0) {
      int32_t eif2 = VRTMath::unpackInt(bbuf, prologlen+cifOffset);
      cifOffset += 4; // increment for next CIF
      off += (bitCount(eif2 & mask2) * 4*eif7Mult);  // 4 bytes for each error
      if (cifNum == 10) return ((eif2 & field) != 0)? (off+cifLen) : -(off+cifLen);  // -off if not present
    } else if (cifNum == 10) {
      return INT32_NULL;
    }

    // EIF3
    if((eif0 & protected_CIF0::CIF3_ENABLE_mask) != 0) {
      int32_t eif3 = VRTMath::unpackInt(bbuf, prologlen+cifOffset);
      cifOffset += 4; // increment for next CIF
      off += (bitCount(eif3 & mask3) * 4*eif7Mult);  // 4 bytes for each error
      if (cifNum == 11) return ((eif3 & field) != 0)? (off+cifLen) : -(off+cifLen);  // -off if not present
    } else if (cifNum == 11) {
      return INT32_NULL;
    }
  }
  
  // XXX - If valid CIF#, should never get here since EIF3 defines the last potential Warn/Error field.
  //       The only valid way to reach here is when looking for free-form message (i.e. cifNum==0xFF)
  if (cifNum == FREE_FORM_MESSAGE) {
    return (getPayloadLength() > (off+cifLen))? (off+cifLen) : -(off+cifLen);
  } else {
    throw VRTException("Invalid CIF number.");
  }
}

int32_t BasicAcknowledgePacket::getTotalFieldSize (int32_t fieldLen, bool occurrence) const {
  if (!isCIF7Enable(occurrence)) return fieldLen;
  int32_t cif7 = getContextIndicatorField7(occurrence);
  int32_t sz = (bitCount(cif7 & protected_CIF7::CTX_4_OCTETS    ) * 4       )
             + (bitCount(cif7 & protected_CIF7::CTX_SAME_OCTETS ) * fieldLen);
  return sz;
}

int32_t BasicAcknowledgePacket::getFieldLen (int8_t cifNum, int32_t field, int32_t parent) const {
  UNUSED_VARIABLE(parent);
  // Warnings and Errors are always a single 32-bit field.
  // if valid WIF or EIF (i.e. <=15), this will mod 8 to ignore whether warning or error
  // if not, this will preserve the most-significant-bits and cause bit3 to be 0
  switch(cifNum & 0xF7) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 7:
      return 4; // 4-bytes = 32-bits
      //break;
    //case 7:
    //  throw VRTException("Invalid Indicator Field number (CIF7).");
      //break;
    default:
      // If cifNum == 0xFF, we're looking for the free-form message at the end
      if (cifNum == FREE_FORM_MESSAGE) {
        // looking for length of free-form message
        int32_t off = getOffset(FREE_FORM_MESSAGE, field);
        return (isNull(off) || off<0)? 0 : getPayloadLength()-off;
      }
      break;
  } // switch(cifNum)
  throw VRTException("Invalid Indicator Field number (undefined).");
}

int32_t BasicAcknowledgePacket::getL (int8_t cifNum, int32_t bit, int32_t cif7bit) const {
  int32_t off = getOffset(cifNum, bit, cif7bit);
  // Note: INT32_NULL is a valid field value, but zero (WEF_NULL) is not.
  if (isNull(off) || off<0) return WEF_NULL;
  return VRTMath::unpackInt(bbuf, off+getPrologueLength());
}

void BasicAcknowledgePacket::setL (int8_t cifNum, int32_t bit, int32_t val, int32_t cif7bit) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");

  int32_t cif7off = 0;
  int32_t fieldLen = 4; // this is always 4
  if (cif7bit != 0) {
    // Note: INT32_NULL is a valid field value, but zero (WEF_NULL) is not.
    if (val==WEF_NULL) throw VRTException("CIF7 attribute must be removed globally for all fields");
    cif7off = getCIF7Offset(cif7bit, fieldLen, cifNum&0x8); // This will throw exception if cif7 not enabled, which is appropriate
    if (cif7off < 0) throw VRTException("CIF7 attribute must be enabled prior to assigning value");
    cif7off -= 4; // adjust to be offset to start of attribute, not end (cif7len is also always 4 here).
  // Note: If setting value w/o specifying cif7bit, the value will be set as the first attribute
  //       The alternative is to error check:
  //} else if (val!=WEF_NULL && isCIF7Enable()) { throw VRTException("CIF7 attribute must be specified when CIF7 is enabled");
  }

  int32_t poff = getOffset(cifNum, bit); // offset to beginning of [parent] field
  if (isNull(poff)) {
    throw VRTException("Indicator Field not enabled; enable Indicator Field first.");
  }
  // Note: INT32_NULL is a valid field value, but zero (WEF_NULL) is not.
  bool present = (val != WEF_NULL); // !isNull(val);

  // are we adding or removing? then we have to do some extra work
  if (present != (poff>0)) {
    int32_t totalSize = getTotalFieldSize(fieldLen, cifNum&0x8);
    setContextIndicatorFieldBit(cifNum, bit, present);
    poff = shiftPayload(poff, totalSize, present);
  }

  if (present) {
    VRTMath::packInt(bbuf, poff+cif7off+getPrologueLength(), val);
  }
}


int32_t BasicAcknowledgePacket::getContextIndicatorField0 (bool occurrence) const {
  if ((!occurrence && !getWarningsGenerated()) || (occurrence && !getErrorsGenerated()))
    return 0;

  int32_t cifOffset = 0;
  int32_t prologlen = getPrologueLength();
  if (occurrence && getWarningsGenerated()) {
    int32_t wif0 = VRTMath::unpackInt(bbuf, prologlen);
    cifOffset += (4 + (bitCount(wif0 & 0x000000FF) * 4)); // 4 bytes for each WIF present
  }
  return VRTMath::unpackInt(bbuf, prologlen+cifOffset);
}
// The commented out implementations work, but do too much work more than once
// so they've been replaced with the implementations below them
/*protected: virtual int32_t getContextIndicatorField1 (bool occurrence=0) const {
  if (!isCIF1Enable(occurrence)) return INT32_NULL;
  int32_t cifOffset = 0;
  if (occurrence && getWarningsGenerated()) {
    int32_t wif0 = VRTMath::unpackInt(bbuf, getPrologueLength());
    cifOffset += (4 + (bitCount(wif0 & 0x000000FF) * 4)); // 4 bytes for each WIF present
  }
  return VRTMath::unpackInt(bbuf, getPrologueLength()+4+cifOffset);
}*/
/*protected: virtual int32_t getContextIndicatorField2 (bool occurrence=0) const {
  if (!isCIF2Enable(occurrence)) return INT32_NULL;
  int32_t cifOffset = 0;
  if (occurrence && getWarningsGenerated()) {
    int32_t wif0 = VRTMath::unpackInt(bbuf, getPrologueLength());
    cifOffset += (4 + (bitCount(wif0 & 0x000000FF) * 4)); // 4 bytes for each WIF present
  }
  if (isCIF1Enable(occurrence)) cifOffset+=4;
  return VRTMath::unpackInt(bbuf, getPrologueLength()+4+cifOffset);
}*/
/*protected: virtual int32_t getContextIndicatorField3 (bool occurrence=0) const {
  if (!isCIF3Enable(occurrence)) return INT32_NULL;
  int32_t cifOffset = 0;
  if (occurrence && getWarningsGenerated()) {
    int32_t wif0 = VRTMath::unpackInt(bbuf, getPrologueLength());
    cifOffset += (4 + (bitCount(wif0 & 0x000000FF) * 4)); // 4 bytes for each WIF present
  }
      if (isCIF1Enable(occurrence)) cifOffset+=4;
  if (isCIF2Enable(occurrence)) cifOffset+=4;
  return VRTMath::unpackInt(bbuf, getPrologueLength()+4+cifOffset);
}*/
/*protected: virtual int32_t getContextIndicatorField7 (bool occurrence=0) const {
  if (!isCIF7Enable(occurrence)) return INT32_NULL;
  int32_t cifOffset = 0;
  if (occurrence && getWarningsGenerated()) {
    int32_t wif0 = VRTMath::unpackInt(bbuf, getPrologueLength());
    cifOffset += (4 + (bitCount(wif0 & 0x000000FF) * 4)); // 4 bytes for each WIF present
  }
  if (isCIF1Enable(occurrence)) cifOffset+=4;
  if (isCIF2Enable(occurrence)) cifOffset+=4;
  if (isCIF3Enable(occurrence)) cifOffset+=4;
  return VRTMath::unpackInt(bbuf, getPrologueLength()+4+cifOffset);
}*/
int32_t BasicAcknowledgePacket::getContextIndicatorField1 (bool occurrence) const {
  if ((!occurrence && !getWarningsGenerated()) || (occurrence && !getErrorsGenerated()))
    return 0;
  
  int32_t cifOffset = 0;
  int32_t prologlen = getPrologueLength();
  if (occurrence && getWarningsGenerated()) {
    int32_t wif0 = VRTMath::unpackInt(bbuf, prologlen);
    cifOffset += (4 + (bitCount(wif0 & 0x000000FF) * 4)); // 4 bytes for each WIF present
  }

  // Check if cif1 is not enabled
      int32_t cif0 = VRTMath::unpackInt(bbuf, prologlen+cifOffset);
  if ((cif0 & protected_CIF0::CIF1_ENABLE_mask) == 0) return 0;

  return VRTMath::unpackInt(bbuf, prologlen+4+cifOffset);
}
int32_t BasicAcknowledgePacket::getContextIndicatorField2 (bool occurrence) const {
  if ((!occurrence && !getWarningsGenerated()) || (occurrence && !getErrorsGenerated()))
    return 0;
  
  int32_t cifOffset = 0;
  int32_t prologlen = getPrologueLength();
  if (occurrence && getWarningsGenerated()) {
    int32_t wif0 = VRTMath::unpackInt(bbuf, prologlen);
    cifOffset += (4 + (bitCount(wif0 & 0x000000FF) * 4)); // 4 bytes for each WIF present
  }

  // Check if cif2 is not enabled
  int32_t cif0 = VRTMath::unpackInt(bbuf, prologlen+cifOffset);
  if ((cif0 & protected_CIF0::CIF2_ENABLE_mask) == 0) return 0;

  // Check if cif1 is enabled
  if ((cif0 & protected_CIF0::CIF1_ENABLE_mask) != 0) cifOffset+=4;

  return VRTMath::unpackInt(bbuf, prologlen+4+cifOffset);
}
int32_t BasicAcknowledgePacket::getContextIndicatorField3 (bool occurrence) const {
  if ((!occurrence && !getWarningsGenerated()) || (occurrence && !getErrorsGenerated()))
    return 0;
  
  int32_t cifOffset = 0;
  int32_t prologlen = getPrologueLength();
  if (occurrence && getWarningsGenerated()) {
    int32_t wif0 = VRTMath::unpackInt(bbuf, prologlen);
    cifOffset += (4 + (bitCount(wif0 & 0x000000FF) * 4)); // 4 bytes for each WIF present
  }

  // Check if cif3 is not enabled
  int32_t cif0 = VRTMath::unpackInt(bbuf, prologlen+cifOffset);
  if ((cif0 & protected_CIF0::CIF3_ENABLE_mask) == 0) return 0;

  // Check if cif1 or cif2 are enabled
  if ((cif0 & protected_CIF0::CIF1_ENABLE_mask) != 0) cifOffset+=4;
  if ((cif0 & protected_CIF0::CIF2_ENABLE_mask) != 0) cifOffset+=4;

  return VRTMath::unpackInt(bbuf, prologlen+4+cifOffset);
}
int32_t BasicAcknowledgePacket::getContextIndicatorField7 (bool occurrence) const {
  if ((!occurrence && !getWarningsGenerated()) || (occurrence && !getErrorsGenerated()))
    return 0;
  
  int32_t cifOffset = 0;
  int32_t prologlen = getPrologueLength();
  if (occurrence && getWarningsGenerated()) {
    int32_t wif0 = VRTMath::unpackInt(bbuf, prologlen);
    cifOffset += (4 + (bitCount(wif0 & 0x000000FF) * 4)); // 4 bytes for each WIF present
  }

  // Check if cif7 is not enabled
  int32_t cif0 = VRTMath::unpackInt(bbuf, prologlen+cifOffset);
  if ((cif0 & protected_CIF0::CIF7_ENABLE_mask) == 0) return 0;

  // Check if cif1, cif2, or cif3 are enabled
  if ((cif0 & protected_CIF0::CIF1_ENABLE_mask) != 0) cifOffset+=4;
  if ((cif0 & protected_CIF0::CIF2_ENABLE_mask) != 0) cifOffset+=4;
  if ((cif0 & protected_CIF0::CIF3_ENABLE_mask) != 0) cifOffset+=4;

  return VRTMath::unpackInt(bbuf, prologlen+4+cifOffset);
}



void BasicAcknowledgePacket::setContextIndicatorField0Bit (int32_t bit, bool set, bool occurrence) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");
  if ((!occurrence && !getWarningsGenerated()) || (occurrence && !getErrorsGenerated()))
    throw VRTException("Attempt to set warning or error indicator field bit without Ack-W or Ack-E set.");

  int32_t cifOffset = 0;
  int32_t prologlen = getPrologueLength();
  if (occurrence && getWarningsGenerated()) {
    int32_t wif0 = VRTMath::unpackInt(bbuf, prologlen);
    cifOffset += (4 + (bitCount(wif0 & 0x000000FF) * 4)); // 4 bytes for each WIF present
  }
  int32_t cif0 = VRTMath::unpackInt(bbuf, prologlen+cifOffset);
  int32_t val;

  if (set) val = cif0 | bit;
  else     val = cif0 & ~bit;

  if (val != cif0) {
    VRTMath::packInt(bbuf, prologlen+cifOffset, val);
  }
}
void BasicAcknowledgePacket::setContextIndicatorField1Bit (int32_t bit, bool set, bool occurrence) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");
  if ((!occurrence && !getWarningsGenerated()) || (occurrence && !getErrorsGenerated()))
    throw VRTException("Attempt to set warning or error indicator field bit without Ack-W or Ack-E set.");

  int32_t cifOffset = 0;
  int32_t prologlen = getPrologueLength();
  if (occurrence && getWarningsGenerated()) {
    int32_t wif0 = VRTMath::unpackInt(bbuf, prologlen);
    cifOffset += (4 + (bitCount(wif0 & 0x000000FF) * 4)); // 4 bytes for each WIF present
  }

  // Check if cif1 is not enabled
  int32_t cif0 = VRTMath::unpackInt(bbuf, prologlen+cifOffset);
  if ((cif0 & protected_CIF0::CIF1_ENABLE_mask) == 0)
    throw VRTException("Indicator Field 1 not enabled.");

  int32_t cif1 = VRTMath::unpackInt(bbuf, prologlen+4+cifOffset);
  int32_t val;

  if (set) val = cif1 | bit;
  else     val = cif1 & ~bit;

  if (val != cif1) {
    VRTMath::packInt(bbuf, prologlen+4+cifOffset, val);
  }
}
void BasicAcknowledgePacket::setContextIndicatorField2Bit (int32_t bit, bool set, bool occurrence) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");
  if ((!occurrence && !getWarningsGenerated()) || (occurrence && !getErrorsGenerated()))
    throw VRTException("Attempt to set warning or error indicator field bit without Ack-W or Ack-E set.");

  int32_t cifOffset = 0;
  int32_t prologlen = getPrologueLength();
  if (occurrence && getWarningsGenerated()) {
    int32_t wif0 = VRTMath::unpackInt(bbuf, prologlen);
    cifOffset += (4 + (bitCount(wif0 & 0x000000FF) * 4)); // 4 bytes for each WIF present
  }

  // Check if cif2 is not enabled
  int32_t cif0 = VRTMath::unpackInt(bbuf, prologlen+cifOffset);
  if ((cif0 & protected_CIF0::CIF2_ENABLE_mask) == 0)
    throw VRTException("Indicator Field 2 not enabled.");

  // Check if cif1 is enabled
  if ((cif0 & protected_CIF0::CIF1_ENABLE_mask) != 0) cifOffset+=4;

  int32_t cif2 = VRTMath::unpackInt(bbuf, prologlen+4+cifOffset);
  int32_t val;

  if (set) val = cif2 | bit;
  else     val = cif2 & ~bit;

  if (val != cif2) {
    VRTMath::packInt(bbuf, prologlen+4+cifOffset, val);
  }
}
void BasicAcknowledgePacket::setContextIndicatorField3Bit (int32_t bit, bool set, bool occurrence) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");
  if ((!occurrence && !getWarningsGenerated()) || (occurrence && !getErrorsGenerated()))
    throw VRTException("Attempt to set warning or error indicator field bit without Ack-W or Ack-E set.");

  int32_t cifOffset = 0;
  int32_t prologlen = getPrologueLength();
  if (occurrence && getWarningsGenerated()) {
    int32_t wif0 = VRTMath::unpackInt(bbuf, prologlen);
    cifOffset += (4 + (bitCount(wif0 & 0x000000FF) * 4)); // 4 bytes for each WIF present
  }

  // Check if cif3 is not enabled
  int32_t cif0 = VRTMath::unpackInt(bbuf, prologlen+cifOffset);
  if ((cif0 & protected_CIF0::CIF3_ENABLE_mask) == 0)
    throw VRTException("Indicator Field 3 not enabled.");

  // Check if cif1 or cif2 are enabled
  if ((cif0 & protected_CIF0::CIF1_ENABLE_mask) != 0) cifOffset+=4;
  if ((cif0 & protected_CIF0::CIF2_ENABLE_mask) != 0) cifOffset+=4;

  int32_t cif3 = VRTMath::unpackInt(bbuf, prologlen+4+cifOffset);
  int32_t val;

  if (set) val = cif3 | bit;
  else     val = cif3 & ~bit;

  if (val != cif3) {
    VRTMath::packInt(bbuf, prologlen+4+cifOffset, val);
  }
}
void BasicAcknowledgePacket::setContextIndicatorField7Bit (int32_t bit, bool set, bool occurrence) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");
  if ((!occurrence && !getWarningsGenerated()) || (occurrence && !getErrorsGenerated()))
    throw VRTException("Attempt to set warning or error indicator field bit without Ack-W or Ack-E set.");

  int32_t cifOffset = 0;
  int32_t prologlen = getPrologueLength();
  if (occurrence && getWarningsGenerated()) {
    int32_t wif0 = VRTMath::unpackInt(bbuf, prologlen);
    cifOffset += (4 + (bitCount(wif0 & 0x000000FF) * 4)); // 4 bytes for each WIF present
  }

  // Check if cif7 is not enabled
  int32_t cif0 = VRTMath::unpackInt(bbuf, prologlen+cifOffset);
  if ((cif0 & protected_CIF0::CIF7_ENABLE_mask) == 0)
    throw VRTException("Indicator Field 7 not enabled.");

  // Check if cif1, cif2, or cif3 are enabled
  if ((cif0 & protected_CIF0::CIF1_ENABLE_mask) != 0) cifOffset+=4;
  if ((cif0 & protected_CIF0::CIF2_ENABLE_mask) != 0) cifOffset+=4;
  if ((cif0 & protected_CIF0::CIF3_ENABLE_mask) != 0) cifOffset+=4;

  int32_t cif7 = VRTMath::unpackInt(bbuf, prologlen+4+cifOffset);
  int32_t val;

  if (set) val = cif7 | bit;
  else     val = cif7 & ~bit;

  if (val != cif7) {
    VRTMath::packInt(bbuf, prologlen+4+cifOffset, val);
  }
}

void BasicAcknowledgePacket::addCIF1 (bool add, bool occurrence) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");
  if ((!occurrence && !getWarningsGenerated()) || (occurrence && !getErrorsGenerated()))
    throw VRTException("Attempt to add warning or error indicator field without Ack-W or Ack-E set.");
  if (isCIF1Enable(occurrence) == add) return; // Nothing to do

  // Get CIF0 (really WIF0 or EIF0)
  int32_t cifOffset = 0;
  int32_t prologlen = getPrologueLength();
  if (occurrence && getWarningsGenerated()) {
    int32_t wif0 = VRTMath::unpackInt(bbuf, prologlen);
    cifOffset += (4 + (bitCount(wif0 & 0x000000FF) * 4)); // 4 bytes for each WIF present
  }
  int32_t cif0 = VRTMath::unpackInt(bbuf, prologlen+cifOffset);

  // Update CIF0 enable bit
  int32_t val;
  if (add) val = cif0 | protected_CIF0::CIF1_ENABLE_mask;
  else     val = cif0 & ~protected_CIF0::CIF1_ENABLE_mask;
  if (val != cif0) {
    VRTMath::packInt(bbuf, prologlen+cifOffset, val);
  }

  // Add or remove 4-bytes for CIF1
  int32_t off = cifOffset+4; // account for CIF0
  if (add) off = -off; // negative if doesn't exist
  off = shiftPayload(off, 4, add);
}

void BasicAcknowledgePacket::addCIF2 (bool add, bool occurrence) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");
  if ((!occurrence && !getWarningsGenerated()) || (occurrence && !getErrorsGenerated()))
    throw VRTException("Attempt to add warning or error indicator field without Ack-W or Ack-E set.");
  if (isCIF2Enable(occurrence) == add) return; // Nothing to do

  // Get CIF0 (really WIF0 or EIF0)
  int32_t cifOffset = 0;
  int32_t prologlen = getPrologueLength();
  if (occurrence && getWarningsGenerated()) {
    int32_t wif0 = VRTMath::unpackInt(bbuf, prologlen);
    cifOffset += (4 + (bitCount(wif0 & 0x000000FF) * 4)); // 4 bytes for each WIF present
  }
  int32_t cif0 = VRTMath::unpackInt(bbuf, prologlen+cifOffset);

  // Update CIF0 enable bit
  int32_t val;
  if (add) val = cif0 | protected_CIF0::CIF2_ENABLE_mask;
  else     val = cif0 & ~protected_CIF0::CIF2_ENABLE_mask;
  if (val != cif0) {
    VRTMath::packInt(bbuf, prologlen+cifOffset, val);
  }

  // Add or remove 4-bytes for CIF2
  int32_t off = cifOffset + 4; // 4 bytes for CIF0
  off += 4 * bitCount(cif0 & protected_CIF0::CIF1_ENABLE_mask);
  if (add) off = -off; // negative if doesn't exist
  off = shiftPayload(off, 4, add);
}

void BasicAcknowledgePacket::addCIF3 (bool add, bool occurrence) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");
  if ((!occurrence && !getWarningsGenerated()) || (occurrence && !getErrorsGenerated()))
    throw VRTException("Attempt to add warning or error indicator field without Ack-W or Ack-E set.");
  if (isCIF3Enable(occurrence) == add) return; // Nothing to do

  // Get CIF0 (really WIF0 or EIF0)
  int32_t cifOffset = 0;
  int32_t prologlen = getPrologueLength();
  if (occurrence && getWarningsGenerated()) {
    int32_t wif0 = VRTMath::unpackInt(bbuf, prologlen);
    cifOffset += (4 + (bitCount(wif0 & 0x000000FF) * 4)); // 4 bytes for each WIF present
  }
  int32_t cif0 = VRTMath::unpackInt(bbuf, prologlen+cifOffset);

  // Update CIF0 enable bit
  int32_t val;
  if (add) val = cif0 | protected_CIF0::CIF3_ENABLE_mask;
  else     val = cif0 & ~protected_CIF0::CIF3_ENABLE_mask;
  if (val != cif0) {
    VRTMath::packInt(bbuf, prologlen+cifOffset, val);
  }

  // Add or remove 4-bytes for CIF3
  int32_t off = cifOffset + 4; // 4 bytes for CIF0
  off += 4 * bitCount(cif0 & (protected_CIF0::CIF1_ENABLE_mask | protected_CIF0::CIF2_ENABLE_mask));
  if (add) off = -off; // negative if doesn't exist
  off = shiftPayload(off, 4, add);
}

void BasicAcknowledgePacket::addCIF7 (bool add, bool occurrence) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");
  if ((!occurrence && !getWarningsGenerated()) || (occurrence && !getErrorsGenerated()))
    throw VRTException("Attempt to add warning or error indicator field without Ack-W or Ack-E set.");
  if (isCIF7Enable(occurrence) == add) return; // Nothing to do

  // Get CIF0 (really WIF0 or EIF0)
  int32_t cifOffset = 0;
  int32_t prologlen = getPrologueLength();
  if (occurrence && getWarningsGenerated()) {
    int32_t wif0 = VRTMath::unpackInt(bbuf, prologlen);
    cifOffset += (4 + (bitCount(wif0 & 0x000000FF) * 4)); // 4 bytes for each WIF present
  }
  int32_t cif0 = VRTMath::unpackInt(bbuf, prologlen+cifOffset);

  // Update CIF0 enable bit
  int32_t val;
  if (add) val = cif0 | protected_CIF0::CIF7_ENABLE_mask;
  else     val = cif0 & ~protected_CIF0::CIF7_ENABLE_mask;
  if (val != cif0) {
    VRTMath::packInt(bbuf, prologlen+cifOffset, val);
  }

  // Add or remove 4-bytes for CIF7
  int32_t off = cifOffset + 4; // 4 bytes for CIF0
  // the following are N/A for CIF7
  int32_t cif0_mask = ~(protected_CIF0::CIF1_ENABLE_mask | protected_CIF0::CIF2_ENABLE_mask
                      | protected_CIF0::CIF3_ENABLE_mask | protected_CIF0::CIF7_ENABLE_mask
                      | protected_CIF0::CHANGE_IND_mask);
  int32_t cif_bits = cif0 & cif0_mask;
  if (cif0 & protected_CIF0::CIF1_ENABLE_mask) {
    cif_bits |= VRTMath::unpackInt(bbuf, prologlen+off);
    off += 4;
  }
  if (cif0 & protected_CIF0::CIF2_ENABLE_mask) {
    cif_bits |= VRTMath::unpackInt(bbuf, prologlen+off);
    off += 4;
  }
  if (cif0 & protected_CIF0::CIF3_ENABLE_mask) {
    cif_bits |= VRTMath::unpackInt(bbuf, prologlen+off);
    off += 4;
  }
  if (add) off = -off; // negative if doesn't exist
  off = shiftPayload(off, 4, add);

  if (add && (cif_bits!=0)) { // at least one CIF bit is set
    //setContextIndicatorField7Bit(protected_CIF7::CURRENT_VALUE_mask, true, occurrence);
    VRTMath::packInt(bbuf, prologlen+off, protected_CIF7::CURRENT_VALUE_mask);
    // no shifting required because the values must already be there in this case
  }
}

void BasicAcknowledgePacket::setCIF7Bit (int32_t cif7bit, bool set, bool occurrence) {
  boolNull present = getCIF7Bit(cif7bit, occurrence);
  if ((present!=_TRUE) && !set) return; // nothing to do
  if ((present==_TRUE) && set) return; // nothing to do
  if (present==_NULL) { // need to add CIF7 enable
    addCIF7(true, occurrence);
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
    setContextIndicatorField7Bit(cif7bit, set, occurrence); // set or unset bit
  }

  // ignore CIF0 bits that are not affected by CIF7
  // for each set bit, shift in (or out) space for cif7 attribute
  int32_t cif0_mask = ~(protected_CIF0::CIF1_ENABLE_mask | protected_CIF0::CIF2_ENABLE_mask
                      | protected_CIF0::CIF3_ENABLE_mask | protected_CIF0::CIF7_ENABLE_mask
                      | protected_CIF0::CHANGE_IND_mask); // = 0x7FFFFF00

  for (int8_t cifNum = cif_start; cifNum != cif_stop; cifNum+=cif_inc) {
    int8_t tmpCifNum =cifNum | (((int8_t)occurrence)<<3); // adjust for occurrence
    if (!isCIFEnable(tmpCifNum)) continue;
    int32_t cif = getContextIndicatorField(tmpCifNum);
    if ((tmpCifNum & 0xF7) == 0) cif &= cif0_mask; // if WIF0 or EIF0, ignore CIF enable and CHANGE_IND bits
    for (int32_t bit = bit_start; bit!=bit_stop; bit+=bit_inc) {
      if (((0x1<<bit)&cif) != 0) {
        // get cif7 field size and offset
        int32_t fieldLen = getFieldLen(tmpCifNum, (0x1<<bit));   // 4
        int32_t cif7size = getFieldLen(7, cif7bit, fieldLen); // 4
        int32_t off = getOffset(tmpCifNum, (0x1<<bit));          // this must be positive b/c of conditional check above
        int32_t cif7off = getCIF7Offset(cif7bit, fieldLen, occurrence);   // if set=true, sign reflects end result, not current reality
        if (set) cif7off = -cif7off;                          // flip sign to reflect current reality, not end result
        if (cif7off < 0) {
          off = -off;                                         // flip offset because cif7 field is not currently present
          cif7off += cif7size;                                // adjust to beginning of cif7 field rather than end
        } else {
          cif7off -= cif7size;                                // adjust to beginning of cif7 field rather than end
        }
        // shift in (or out) cif7field size at cif7offset
        off = shiftPayload(off+cif7off, cif7size, set);
      }
    }
  }
  if (!set) {
    setContextIndicatorField7Bit(cif7bit, set, occurrence); // unset bit
  }
}

void BasicAcknowledgePacket::setWarningsGenerated (bool set) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");
  // Replaced trivial implementation with one that adds/removes WIF0
  //setCtrlAckSettingsBit(CTRLACK_W_BIT, set);
  int32_t field = getCtrlAckSettingsField();
  int32_t val;

  if (set) val = field | CTRLACK_W_BIT;
  else     val = field & ~CTRLACK_W_BIT;

  if (val != field) {
    // Update bit and adjust payload
    VRTMath::packInt(bbuf, getHeaderLength(), val);
    // XXX shiftPayload has a bug where offset of 0 cannot be used...
    // as a workaround, we can use shiftPacketSpecificPrologue when off=0
    //shiftPayload(0, 4, set); // offset is always 0 for WIF0
    //int32_t off = getPrologueLength()-getHeaderLength(); // size of packet specific prologue
    int32_t off = getPktSpecificPrologueLength(); // size of packet specific prologue
    if (set) off = -off; // negative if not already there
    off = shiftPacketSpecificPrologue(off, 4, set);
  }
}

void BasicAcknowledgePacket::setErrorsGenerated (bool set) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");
  // Replaced trivial implementation with one that adds/removes EIF0
  //setCtrlAckSettingsBit(CTRLACK_E_BIT, set);
  int32_t field = getCtrlAckSettingsField();
  int32_t val;

  if (set) val = field | CTRLACK_E_BIT;
  else     val = field & ~CTRLACK_E_BIT;

  if (val != field) {
    VRTMath::packInt(bbuf, getHeaderLength(), val);
    // find offset for EIF0
    int32_t off = 0;
    int32_t prologlen = getPrologueLength();
    if ((field & CTRLACK_W_BIT) != 0) {
      int32_t wif0 = VRTMath::unpackInt(bbuf, prologlen);
      off += (4 + (bitCount(wif0 & 0x000000FF) * 4)); // 4 bytes for each WIF present
      if (set) off = -off; // negative if not already there
      off = shiftPayload(off, 4, set);
    }
    else {
      // XXX shiftPayload has a bug where offset of 0 cannot be used...
      // as a workaround, we can use shiftPacketSpecificPrologue when off=0
      //off = prologlen-getHeaderLength(); // size of packet specific prologue
      off = getPktSpecificPrologueLength(); // size of packet specific prologue
      if (set) off = -off; // negative if not already there
      off = shiftPacketSpecificPrologue(off, 4, set);
    }
  }
}

std::vector<WarningErrorField_t> BasicAcknowledgePacket::getWarnings() const {
  std::vector<WarningErrorField_t> warnings;
  if (getPayloadLength() == 0 || !getWarningsGenerated()) {
    return warnings;
  }
  int32_t prologlen = getPrologueLength();
  int32_t wif0      = VRTMath::unpackInt(bbuf, prologlen);
  int32_t cifOffset = 4;                                   // offset of next CIF after WIF0
  int32_t cifLen    = (4 + 4*bitCount(wif0 & 0x000000FF)); // 4 bytes for each WIF present
  if (getErrorsGenerated()) {
    // update eif0 and cifLen values
    int32_t eif     = VRTMath::unpackInt(bbuf, prologlen+cifLen);
    cifLen         += (4 + 4*bitCount(eif & 0x000000FF)); // 4 bytes for each EIF present
  }
  // WIF0
  int32_t off = 0;
  WarningErrorField_t wef;
  for (int32_t idx=31; idx > 7; idx--) { // do not include WIF enable bits
    if (((wif0>>idx) & 0x1) != 0) {
      wef.field = getCIFEnum(0, idx);
      wef.responseField = VRTMath::unpackInt(bbuf, prologlen+cifLen+off);
      warnings.push_back(wef);
      off += 4;
    }
  }
  // WIF1-3
  int32_t wif;
  for (int8_t cif=1; cif <= 3; cif++) {
    // Continue if CIF is not enabled via CIF0 enable bits...
    if ((wif0 & (0x1 << cif)) == 0) continue;
    wif = VRTMath::unpackInt(bbuf, prologlen+cifOffset);
    cifOffset += 4;
    for (int32_t idx=31; idx > 0; idx--) { // do not include reserved bit 0
      if (((wif>>idx) & 0x1) != 0) {
        wef.field = getCIFEnum(cif, idx);
        wef.responseField = VRTMath::unpackInt(bbuf, prologlen+cifLen+off);
        warnings.push_back(wef);
        off += 4;
      }
    }
  }
  return warnings;
}

std::vector<WarningErrorField_t> BasicAcknowledgePacket::getErrors() const {
  std::vector<WarningErrorField_t> errors;
  if (getPayloadLength() == 0 || !getErrorsGenerated()) {
    return errors;
  }
  int32_t prologlen = getPrologueLength();
  int32_t cifOffset = 0;                    // offset to nex CIF
  int32_t cifLen    = 0;                    // total length of CIF block
  int32_t off       = 0;                    // offset into fields
  if (getWarningsGenerated()) {
    // update cifOffset, cifLen, and off values to advance past warnings
    int32_t wif0  = VRTMath::unpackInt(bbuf, prologlen+cifOffset);
    cifOffset    += 4;
    cifLen       += (4 + 4*bitCount(wif0 & 0x000000FF)); // 4 bytes for each WIF present
    off          += (    4*bitCount(wif0 & 0xFFFFFF00)); // 4 bytes for each bit enabled
    // CIF1-3
    int32_t wif;
    for (int8_t cif=1; cif <= 3; cif++) {
      // Continue if CIF is not enabled via CIF0 enable bits...
      if ((wif0 & (0x1 << cif)) == 0) continue;
      wif = VRTMath::unpackInt(bbuf, prologlen+cifOffset);
      cifOffset += 4;
      off       += (4*bitCount(wif & 0xFFFFFF00)); // 4 bytes for each bit enabled
    }
    // cifOffset should equal cifLen unless there's a WIF7, which we want to skip past anyway
    cifOffset = cifLen;
  }
  int32_t eif0 = VRTMath::unpackInt(bbuf, prologlen+cifOffset);
  cifOffset   += 4;
  cifLen      += (4 + 4*bitCount(eif0 & 0x000000FF)); // 4 bytes for each EIF present
  // EIF0
  WarningErrorField_t wef;
  for (int32_t idx=31; idx > 7; idx--) { // do not include EIF enable bits
    if (((eif0>>idx) & 0x1) != 0) {
      wef.field = getCIFEnum(0, idx);
      wef.responseField = VRTMath::unpackInt(bbuf, prologlen+cifLen+off);
      errors.push_back(wef);
      off += 4;
    }
  }
  // WIF1-3
  int32_t eif;
  for (int8_t cif=1; cif <= 3; cif++) {
    // Continue if CIF is not enabled via CIF0 enable bits...
    if ((eif0 & (0x1 << cif)) == 0) continue;
    eif = VRTMath::unpackInt(bbuf, prologlen+cifOffset);
    cifOffset += 4;
    for (int32_t idx=31; idx > 0; idx--) { // do not include reserved bit 0
      if (((eif>>idx) & 0x1) != 0) {
        wef.field = getCIFEnum(cif, idx);
        wef.responseField = VRTMath::unpackInt(bbuf, prologlen+cifLen+off);
        errors.push_back(wef);
        off += 4;
      }
    }
  }
  return errors;
}

bool BasicAcknowledgePacket::hasFreeFormMessage () const {
  int32_t off = getOffset(FREE_FORM_MESSAGE, 0);
  return (isNull(off) || off < 0)? false : true;
}

FreeFormMessage_t BasicAcknowledgePacket::getFreeFormMessage () const {
  int32_t off = getOffset(FREE_FORM_MESSAGE, 0);
  if (isNull(off) || off < 0) {
    return FreeFormMessage_t();
  } else {
    int32_t prologlen = getPrologueLength();
    int32_t packetlen = getPacketLength();
    return FreeFormMessage_t(&bbuf[prologlen+off], packetlen-prologlen-off);
  }
}

void BasicAcknowledgePacket::setFreeFormMessage (const FreeFormMessage_t msg) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");
  int32_t off = getOffset(FREE_FORM_MESSAGE, 0);
  if (isNull(off)) {
    throw VRTException("Can not set free-form message if Ack-W and Ack-E are both unset.");
  }
  int32_t prologlen = getPrologueLength();
  
  // If one exists, remove it
  if (off > 0 ) {
    int32_t oldmsglen = getPacketLength()-prologlen-off;
    off = shiftPayload(off, oldmsglen, false);
    off = -off;
  }

  if (!msg.empty()) {
    size_t words = size_t((msg.size()+3)/4); // whole 4-byte words
    off = shiftPayload(off, 4*words, true);
    //memcpy(&bbuf[off+prologlen], msg.buffer(), msg.size());
    VRTMath::packBytes(bbuf, off+prologlen, msg.buffer(), msg.size());
  }
}


/** TODO - implement HasFields methods for BasicAcknowledgePacket 
 *  XXX - Has BasicCommandPacket impl, need to add ErrorWarn-related fields
    public: virtual int32_t   getFieldCount () const;
    public: virtual string    getFieldName  (int32_t id) const;
    public: virtual ValueType getFieldType  (int32_t id) const;
    public: virtual Value*    getField      (int32_t id) const __attribute__((warn_unused_result));
    public: virtual void      setField      (int32_t id, const Value* val);
*/




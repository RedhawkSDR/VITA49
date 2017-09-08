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

#include "BasicVRTPacket.h"
#include <cstring>
#include <stdlib.h>         // required for atoi(..) on GCC4.4/libc6 2.11.1

using namespace vrt;

const BasicVRTPacket BasicVRTPacket::NULL_PACKET = BasicVRTPacket(vector<char>(0), true);

static const vector<char> NULL_TRAILER(BasicVRTPacket::MAX_TRAILER_LENGTH); // only used by trailerEquals(..)

static const char CLASS_ID_BIT = 0x8;  // Class ID present bit in buf[0]
static const char TRAILER_BIT  = 0x4;  // Trailer present bit in buf[0]
static const char TSM_BIT      = 0x1;  // TimeStamp Mode bit in buf[0]

// Flags for getOffset(..). These values are in the form:
//   AABBaabb         AA - Bit mask to apply to bbuf[0] indicating flag is present
//                    BB - Bit mask to apply to bbuf[1] indicating flag is present
//                    aa - Bit mask to apply to bbuf[0] highlighting all preceding
//                    bb - Bit mask to apply to bbuf[1] highlighting all preceding
// In doing this, we use AA=0x20 to indicate the presence of the payload (and always
// apply it to our read of bbuf[0] since the payload is mandatory). 0x20 was chosen
// since that bit is always 0 in the header.
static const int32_t STREAM_ID = 0x50000000; // getOffset(..) flag for field: STREAM_ID
static const int32_t CLASS_ID  = 0x08005000; // getOffset(..) flag for field: CLASS_ID
static const int32_t TSI       = 0x00C05800; // getOffset(..) flag for field: TSI
static const int32_t TSF       = 0x003058C0; // getOffset(..) flag for field: TSF
static const int32_t PAYLOAD   = 0x020058F0; // getOffset(..) flag for field: PAYLOAD


ostream& operator<< (ostream &s, PacketType val) {
  switch (val) {
    case PacketType_UnidentifiedData:    return s << "UnidentifiedData";     case PacketType_Data:        return s << "Data";
    case PacketType_UnidentifiedExtData: return s << "UnidentifiedExtData";  case PacketType_ExtData:     return s << "ExtData";
    case PacketType_Context:             return s << "Context";              case PacketType_ExtContext:  return s << "ExtContext";
    case PacketType_reserved6:           return s << "reserved6";            case PacketType_reserved7:   return s << "reserved7";
    case PacketType_reserved8:           return s << "reserved8";            case PacketType_reserved9:   return s << "reserved9";
    case PacketType_reserved10:          return s << "reserved10";           case PacketType_reserved11:  return s << "reserved11";
    case PacketType_reserved12:          return s << "reserved12";           case PacketType_reserved13:  return s << "reserved13";
    case PacketType_reserved14:          return s << "reserved14";           case PacketType_reserved15:  return s << "reserved15";
    default:                             return s << "Unknown PacketType " << (int32_t)val;
  }
}

string operator+ (string &s, PacketType val) {
  ostringstream str;
  str << s << val;
  return str.str();
}

bool vrt::PacketType_isData (PacketType pt) {
  switch (pt) {
    case PacketType_UnidentifiedData:    return true;     case PacketType_Data:                return true;
    case PacketType_UnidentifiedExtData: return true;     case PacketType_ExtData:             return true;
    case PacketType_Context:             return false;    case PacketType_ExtContext:          return false;
    case PacketType_reserved6:           return false;    case PacketType_reserved7:           return false;
    case PacketType_reserved8:           return false;    case PacketType_reserved9:           return false;
    case PacketType_reserved10:          return false;    case PacketType_reserved11:          return false;
    case PacketType_reserved12:          return false;    case PacketType_reserved13:          return false;
    case PacketType_reserved14:          return false;    case PacketType_reserved15:          return false;
  }
  throw VRTException("Invalid packet type");
}

bool vrt::PacketType_hasStreamIdentifier (PacketType pt) {
  switch (pt) {
    case PacketType_UnidentifiedData:    return false;    case PacketType_Data:                return true;
    case PacketType_UnidentifiedExtData: return false;    case PacketType_ExtData:             return true;
    case PacketType_Context:             return true;     case PacketType_ExtContext:          return true;
    case PacketType_reserved6:           return false;    case PacketType_reserved7:           return false;
    case PacketType_reserved8:           return false;    case PacketType_reserved9:           return false;
    case PacketType_reserved10:          return false;    case PacketType_reserved11:          return false;
    case PacketType_reserved12:          return false;    case PacketType_reserved13:          return false;
    case PacketType_reserved14:          return false;    case PacketType_reserved15:          return false;
  }
  throw VRTException("Invalid packet type");
}

ostream& operator<<(ostream &s, RealComplexType val) {
  switch (val) {
    case RealComplexType_Real:             return s << "Real";
    case RealComplexType_ComplexCartesian: return s << "ComplexCartesian";
    case RealComplexType_ComplexPolar:     return s << "ComplexPolar";
    case RealComplexType_reserved3:        return s << "reserved3";
    default:                               return s << "Unknown RealComplexType " << (int32_t)val;
  }
}

string operator+(string &s, RealComplexType val) {
  ostringstream str;
  str << s << val;
  return str.str();
}

ostream& operator<<(ostream &s, DataItemFormat val) {
  switch (val) {
    case DataItemFormat_SignedInt    : return s << "SignedInt";      case DataItemFormat_SignedVRT1   : return s << "SignedVRT1";
    case DataItemFormat_SignedVRT2   : return s << "SignedVRT2";     case DataItemFormat_SignedVRT3   : return s << "SignedVRT3";
    case DataItemFormat_SignedVRT4   : return s << "SignedVRT4";     case DataItemFormat_SignedVRT5   : return s << "SignedVRT5";
    case DataItemFormat_SignedVRT6   : return s << "SignedVRT6";     case DataItemFormat_reserved7    : return s << "reserved7";
    case DataItemFormat_reserved8    : return s << "reserved8";      case DataItemFormat_reserved9    : return s << "reserved9";
    case DataItemFormat_reserved10   : return s << "reserved10";     case DataItemFormat_reserved11   : return s << "reserved11";
    case DataItemFormat_reserved12   : return s << "reserved12";     case DataItemFormat_reserved13   : return s << "reserved13";
    case DataItemFormat_Float        : return s << "Float";          case DataItemFormat_Double       : return s << "Double";
    case DataItemFormat_UnsignedInt  : return s << "UnsignedInt";    case DataItemFormat_UnsignedVRT1 : return s << "UnsignedVRT1";
    case DataItemFormat_UnsignedVRT2 : return s << "UnsignedVRT2";   case DataItemFormat_UnsignedVRT3 : return s << "UnsignedVRT3";
    case DataItemFormat_UnsignedVRT4 : return s << "UnsignedVRT4";   case DataItemFormat_UnsignedVRT5 : return s << "UnsignedVRT5";
    case DataItemFormat_UnsignedVRT6 : return s << "UnsignedVRT6";   case DataItemFormat_reserved23   : return s << "reserved23";
    case DataItemFormat_reserved24   : return s << "reserved24";     case DataItemFormat_reserved25   : return s << "reserved25";
    case DataItemFormat_reserved26   : return s << "reserved26";     case DataItemFormat_reserved27   : return s << "reserved27";
    case DataItemFormat_reserved28   : return s << "reserved28";     case DataItemFormat_reserved29   : return s << "reserved29";
    case DataItemFormat_reserved30   : return s << "reserved30";     case DataItemFormat_reserved31   : return s << "reserved31";
    default                          : return s << "Unknown DataItemFormat " << (int32_t)val;
  }
}

string operator+(string &s, DataItemFormat val) {
  ostringstream str;
  str << s << val;
  return str.str();
}

ostream& operator<<(ostream &s, DataType val) {
  if (((int32_t)val) == -1) return s << "null";
  switch (val) {
    case DataType_Int4       : return s << "Int4";
    case DataType_Int8       : return s << "Int8";
    case DataType_Int16      : return s << "Int16";
    case DataType_Int32      : return s << "Int32";
    case DataType_Int64      : return s << "Int64";
    case DataType_Float      : return s << "Float";
    case DataType_Double     : return s << "Double";
    case DataType_UInt1      : return s << "UInt1";
    case DataType_UInt4      : return s << "UInt4";
    case DataType_UInt8      : return s << "UInt8";
    case DataType_UInt16     : return s << "UInt16";
    case DataType_UInt32     : return s << "UInt32";
    case DataType_UInt64     : return s << "UInt64";
    default                  : return s << "Unknown DataType " << (int64_t)val;
  }
}

string operator+(string &s, DataType val) {
  ostringstream str;
  str << s << val;
  return str.str();
}

BasicVRTPacket::BasicVRTPacket () :
  bbuf(7*4/*=MAX_HEADER_LENGTH*/),
  readOnly(false)
{
  // Initializes a default header for the packet.
  bbuf[0] = 0x18;
  bbuf[1] = 0x60;
  bbuf[2] = 0x00;
  bbuf[3] = 0x07;
  // done
}

BasicVRTPacket::BasicVRTPacket (int32_t bufsize) :
  bbuf(max(7*4/*=MAX_HEADER_LENGTH*/, bufsize)),
  readOnly(false)
{
  // Initializes a default header for the packet.
  bbuf[0] = 0x18;
  bbuf[1] = 0x60;
  bbuf[2] = 0x00;
  bbuf[3] = 0x07;
}

BasicVRTPacket::BasicVRTPacket (const BasicVRTPacket &p) :
  VRTObject(p), // <-- Used to avoid warnings under GCC with -Wextra turned on
  bbuf(p.bbuf),
  readOnly(p.readOnly)
{
  // done
}

BasicVRTPacket::BasicVRTPacket (const void *buf, size_t len, bool readOnly) :
  readOnly(readOnly)
{
  if (len < 4) {
    // If user accidentally calls with 'BasicVRTPacket(buf, false)' there will
    // be no compile-time warning (C++ changes false/true to 0/1), but there
    // will be a segfault later when they try to do stuff. As such we should
    // check for this and issue an error.
    throw VRTException("Attempt to create BasicVRTPacket with invalid length of %d "
                       "(possible call to constructor with false/true in wrong "
                       "argument position)", (int)len);
  }
  bbuf.assign((char*)buf, &((char*)buf)[len]);
}

BasicVRTPacket::BasicVRTPacket (vector<char> *buf_ptr, bool readOnly) :
  bbuf(*buf_ptr),
  readOnly(readOnly)
{
  // done
}

BasicVRTPacket::BasicVRTPacket (const vector<char> &buf, bool readOnly) :
  bbuf(buf),
  readOnly(readOnly)
{
  // done
}

BasicVRTPacket::BasicVRTPacket (const vector<char> &buf, ssize_t start, ssize_t end, bool readOnly) :
  bbuf(buf.begin() + start,buf.begin() + end),
  readOnly(readOnly)
{
  // done
}

BasicVRTPacket::BasicVRTPacket (const BasicVRTPacket &p, PacketType type, int64_t classID,
                                int32_t minPayloadLength, int32_t maxPayloadLength) :
  bbuf(p.bbuf),
  readOnly(p.readOnly)
{
  if (getPacketType() != type) {
    throw VRTException("Can not initialize packet class when given packet type of %d", getPacketType());
  }
  if (getClassIdentifier() != classID) {
    throw VRTException("Can not initialize packet class when given packet class ID of "+getClassID());
  }

  int32_t len = getPayloadLength();
  if (len < minPayloadLength) {
    throw VRTException("Can not initialize packet class when payload length is invalid");
  }
  if ((maxPayloadLength >= 0) && (len > maxPayloadLength)) {
    throw VRTException("Can not initialize packet class when payload length is invalid");
  }
}

int64_t BasicVRTPacket::getStreamCode () const {
  switch (bbuf[0] & 0xF0) {
    case 0x00: return __INT64_C(0x0000000000000000); // no stream identifier
    case 0x20: return __INT64_C(0x2000000000000000); // no stream identifier
    default:   return VRTMath::unpackLong(bbuf, 0) & __INT64_C(0xF0000000FFFFFFFF);
  }
}

int64_t BasicVRTPacket::getStreamCode (const void *ptr) {
  const char *bbuf = (const char*)ptr;
  switch (bbuf[0] & 0xF0) {
    case 0x00: return __INT64_C(0x0000000000000000); // no stream identifier
    case 0x20: return __INT64_C(0x2000000000000000); // no stream identifier
    default:   return VRTMath::unpackLong(bbuf, 0) & __INT64_C(0xF0000000FFFFFFFF);
  }
}

void BasicVRTPacket::toStringStream (ostringstream &str) const {
  str << getClassName() << ":";
  str << " PacketType="   << getPacketType();
  str << " PacketCount="  << getPacketCount();
  str << " PacketLength=" << getPacketLength();
  str <<   " (Header="    << getHeaderLength();
  str <<   ",Payload="    << getPayloadLength();
  str <<   ",Trailer="    << getTrailerLength() << ")";
  str << " PadBitCount="  << getPadBitCount();
  Utilities::append(str, " TimeStamp=",     getTimeStamp());
  Utilities::append(str, " StreamID=",      getStreamID());
  Utilities::append(str, " ClassID=",       getClassID());
  Utilities::append(str, " TimeStampMode=", isTimeStampMode());
}

bool BasicVRTPacket::headerEquals (const BasicVRTPacket &p2) const {
  int32_t hLen = getHeaderLength();
  if (hLen != p2.getHeaderLength()) return false;
  return (memcmp(&bbuf[0], &p2.bbuf[0], hLen) == 0);
}

bool BasicVRTPacket::payloadEquals (const BasicVRTPacket &p2) const {
  int32_t pLen = getPayloadLength();
  if (pLen != p2.getPayloadLength()) return false;
  return (memcmp(&bbuf[getHeaderLength()], &p2.bbuf[p2.getHeaderLength()], pLen) == 0);
}

bool BasicVRTPacket::trailerEquals (const BasicVRTPacket &p2, bool exact) const {
  bool t1 = hasTrailer();
  bool t2 = (p2.getTrailerLength() > 0);

  if (!t1 && !t2) return true; // no trailer in either one
  if (exact && (t1 != t2)) return false;

  const vector<char> buf1 = (t1)? bbuf                                    : NULL_TRAILER;
  int32_t            off1 = (t1)? getPacketLength()-MAX_TRAILER_LENGTH    : 0;
  const vector<char> buf2 = (t2)? p2.bbuf                                 : NULL_TRAILER;
  int32_t            off2 = (t2)? p2.getPacketLength()-MAX_TRAILER_LENGTH : 0;

  if (exact) {
    return (memcmp(&buf1[off1], &buf2[off2], MAX_TRAILER_LENGTH) == 0);
  }
  else {
    if ((buf1[off1+3] < 0) || (buf2[off2+3] < 0)) {
      if (buf1[off1+3] != buf2[off2+3]) return false; // assoc. context packet counts don't match
    }
    for (int e=20,i=8; e <= 31; e++,i++) {
      boolNull bit1 = getStateEventBit(buf1, off1, e, i);
      boolNull bit2 = getStateEventBit(buf2, off2, e, i);
      if (bit1 != bit2) return false; // state/event bits don't match
    }
    return true;
  }
}

bool BasicVRTPacket::equals (const BasicVRTPacket &p2) const {
  if (isNullValue()   ) return p2.isNullValue();
  if (p2.isNullValue()) return false;

  int32_t pLen = getPacketLength();
  if (pLen != p2.getPacketLength()) return false;
  return (memcmp(&bbuf[0], &p2.bbuf[0], pLen) == 0);
}

string BasicVRTPacket::toString () const {
  if (isNullValue()) return getClassName()+": <null>";

  try {
    string err = getPacketValid(false);
    if (err != "") return getClassName()+": <"+err+">";

    ostringstream str;
    toStringStream(str);
    return str.str();
  }
  catch (VRTException e) {
    return getClassName()+": <Invalid VRTPacket: "+e.toString()+">";
  }
}

int32_t __attribute__((hot)) BasicVRTPacket::getOffset (int32_t field) const {
  int32_t a0  = (bbuf[0] | 0x02) & 0xFF;
  int32_t b0  = (bbuf[1]       ) & 0xFF;
  int32_t a   = a0 & (field >> 8);
  int32_t b   = b0 & (field     );
  int32_t sgn = ~(-((a0 & (field >> 24))    // If any bits in mask -> "N>0", otherwise "N=0",
                   |(b0 & (field >> 16)))); // then doing ~(-N) changes this to "N>0" and "N<0"
  int32_t mul = (sgn >> 31) | 0x00000001;   // This changes it to "N=+1" or "N=-1"
  int32_t off = 4
              + (((a >> 2) | (a >> 4)) & 0x04)   // xxx1 xxxx OR x1xx xxxx -> has stream identifier (4)
              + (( a                 ) & 0x08)   // xxxx 1xxx              -> has class identifier  (8)
              + (((b >> 5) | (b >> 4)) & 0x04)   // 1xxx xxxx OR x1xx xxxx -> has TSI               (4)
              + (((b >> 2) | (b >> 1)) & 0x08);  // xx1x xxxx OR xxx1 xxxx -> has TSF               (8)
  return mul * off;
}

void BasicVRTPacket::shiftHeader (int32_t field, int32_t bytes, bool present) {
  int32_t pos = getOffset(field);
  if ((pos >= 0) == present) return; // no change

  if (pos < 0) { // ADD
    int32_t off = -pos;
    bbuf.reserve(getPacketLength()+bytes);
    bbuf.insert(bbuf.begin()+off, bytes, 0);
    setPacketLength(getPacketLength()+bytes);
  }
  else { // REMOVE
    int32_t off = +pos;
    bbuf.erase(bbuf.begin()+off, bbuf.begin()+off+bytes);
    setPacketLength(getPacketLength()-bytes);
  }
}

int32_t BasicVRTPacket::shiftPayload (int32_t off, int32_t bytes, bool add) {
  int32_t offset = (off < 0)? -off : off;
  if ((off >= 0) == add) return offset; // no change

  int32_t hlen = getHeaderLength();
  if (add) {
    bbuf.reserve(getPacketLength()+bytes);
    bbuf.insert(bbuf.begin()+hlen+offset, bytes, 0);
    setPacketLength(getPacketLength()+bytes);
  }
  else {
    bbuf.erase(bbuf.begin()+hlen+offset, bbuf.begin()+hlen+offset+bytes);
    setPacketLength(getPacketLength()-bytes);
  }
  return offset;
}

void BasicVRTPacket::shiftTrailer (int32_t bytes) {
  int32_t oldLen = getPacketLength();
  int32_t newLen = oldLen + bytes;

  bbuf.resize(newLen, 0);
  if (bytes < 0) {
    bbuf[0] &= ~TRAILER_BIT;
  }
  else {
    bbuf[0] |= TRAILER_BIT;
  }
  setPacketLength(newLen);
}

boolNull BasicVRTPacket::getStateEventBit (const vector<char> &buf, int32_t off, int32_t enable, int32_t indicator) {
  int32_t eByte = 3 - (enable    / 8);
  int32_t iByte = 3 - (indicator / 8);
  int32_t eBit  = 0x1 << (enable    % 8);
  int32_t iBit  = 0x1 << (indicator % 8);

  if ((buf[off+eByte] & eBit) == 0) return _NULL;
  if ((buf[off+iByte] & iBit) != 0) return _TRUE;
                                    return _FALSE;
}

void BasicVRTPacket::setStateEventBit (vector<char> &buf, int32_t off, int32_t enable, int32_t indicator, boolNull value) {
  int32_t eByte = 3 - (enable    / 8);
  int32_t iByte = 3 - (indicator / 8);
  int32_t eBit  = 0x1 << (enable    % 8);
  int32_t iBit  = 0x1 << (indicator % 8);

  if (value == _NULL) {
    buf[off+eByte] &= (char)~eBit;
    buf[off+iByte] &= (char)~iBit;
  }
  else if (value == _TRUE) {
    buf[off+eByte] |= (char)eBit;
    buf[off+iByte] |= (char)iBit;
  }
  else { // _FALSE
    buf[off+eByte] |= (char) eBit;
    buf[off+iByte] &= (char)~iBit;
  }
}

string BasicVRTPacket::getPacketValid (bool strict, int32_t length) const {
  static const int32_t CONTEXT_CLASSID_RESERVED = 0xFF;
  static const int32_t DATA_CLASSID_RESERVED    = (VRTConfig::getVRTVersion() == VRTConfig::VITAVersion_V49b)? 0x07 : 0xFF;

  // This method is called a LOT so it has been re-written to eliminate any branches outside of
  // the error case.

  bool    data = isData();
  int32_t cid  = getOffset(CLASS_ID);
  bool    resOK = !strict || (( data && ((bbuf[0] & 0x3) == 0))               // Fixed Reserved Bits (data)
                          ||  (!data && ((bbuf[0] & 0x6) == 0)));             // Fixed Reserved Bits (context)
  bool    cidOK = !strict || (cid < 0)
                          || ( data && ((bbuf[cid] & DATA_CLASSID_RESERVED   ) == 0))  // ClassID Reserved Bits (data)
                          || (!data && ((bbuf[cid] & CONTEXT_CLASSID_RESERVED) == 0)); // ClassID Reserved Bits (context)
  // Check lengths
  int32_t pLen     = getPacketLength();
  int32_t hLen     = getHeaderLength();
  int32_t tLen     = getTrailerLength();
  bool    sizeMin  = (pLen >= hLen+tLen);
  bool    sizeOk1  = (bbuf.size() >= (size_t)pLen);
  bool    lenOK    = (length == -1) || (length == pLen);
  bool    allOK    = resOK && cidOK && sizeMin && lenOK && sizeOk1;

  if (allOK) {
    return "";
  }
  else {
    ostringstream str;
    str << "Invalid VRTPacket: ";

         if (!resOK  ) str << "Reserved bit(s) set to 1 but expected 0.";
    else if (!cidOK  ) str << "Reserved ClassID bit(s) set to 1 but expected 0 (using "
                              "VRT_VERSION=" << VRTConfig::getVRTVersion() << ").";
    else if (!sizeMin) str << "Invalid packet length, packet reports " << pLen << " octets total, "
                              "but has " << hLen << "+" << tLen << " octets in header+trailer.";
    else if (!lenOK  ) str << "Invalid packet length, packet reports " << pLen << " octets, "
                              "but working with " << length << " octets.";
    else if (!sizeOk1) str << "Allocated buffer shorter than packet length.";
    else               str << "Unknown issue with packet.";

    return str.str();
  }
}

int32_t BasicVRTPacket::getPadBitCount () const {
  // This function optimizes out the "isData()" and "VRT_VERSION == V49b" checks since all other
  // uses require the bits to be set to zero (which matches the required return value for this
  // function in those situations). The isPacketValid() check will allert the user if the bits
  // were incorrectly set.
  int32_t cid = getOffset(CLASS_ID);

  return (cid > 0)? ((bbuf[cid] >> 3) & 0x1F) : 0;
}

void BasicVRTPacket::setPadBitCount (int32_t bits, int32_t bitsPerSample) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");
  int32_t cid = getOffset(CLASS_ID);

  // bits==0 is most common case so check that first
  if (bits == 0) {
    // If a ClassID is present, make sure the bits are set to 0 (this works with both
    // VITA-49.0 and VITA-49.0b since the bits should be all zero in VITA-49.0b), this
    // also works with context packets (the bits should be all zero here too). If there
    // is no ClassID, don't worry about anything (no ClassID -> no pad bits)
    if (cid > 0) {
      bbuf[cid] = 0;
    }
  }
  else if (!isData()) {
    // Context packets expect PadBitCount=0
    throw VRTException("Can not set PadBitCount to %d for a context packet.", bits);
  }
  else if ((bits < 0) || (bits > 31)) {
    throw VRTException("Given PadBitCount of %d is outside of allowable limits.", bits);
  }
  else if (bits < bitsPerSample) {
    // Value is implicit, setting is optional (so clear it out to match V49)
    if (cid > 0) {
      bbuf[cid] = 0;
    }
  }
  else if (cid < 0) {
    // Value is not implicit, but can not set
    throw VRTException("Can not set PadBitCount to %d without a ClassID set.", bits);
  }
  else if (VRTConfig::getVRTVersion() != VRTConfig::VITAVersion_V49b) {
    // Value is not implicit, but can not set
    throw VRTException("Can only set PadBitCount to %d when using VRT_VERSION=V49b.", bits);
  }
  else {
    // Have a ClassID and using VITA-49.0b so set the bits
    bbuf[cid] = (int8_t)(bits << 3);
  }
}

bool BasicVRTPacket::isData () const {
  // This used to be implemented as
  //    return PacketType_isData(getPacketType());
  // but changed to make it faster
  return ((bbuf[0] & 0xC0) == 0); // i.e. accept types 0x0o, 0x1o, 0x2o, 0x3o
}

bool BasicVRTPacket::hasTrailer () const {
  // This used to be implemented as
  //    return isData() && ((bbuf[0]&TRAILER_BIT) != 0);
  // but changed to make it faster since getTrailerLength() is called frequently
  return ((bbuf[0] & 0xC4) == 0x4); // i.e. accept types 0x0o, 0x1o, 0x2o, 0x3o with 0x04 set
}

int32_t  BasicVRTPacket::getTrailerLength () const {
  // This used to be implemented as
  //    return (hasTrailer())? MAX_TRAILER_LENGTH : 0;
  // but has been changed to eliminate the branch
  int32_t n = bbuf[0];
  int32_t e = ((n>>4) | (n>>5)) & 0x4; // set e=0x4 if not a data packet
  return n & 0x4 & ~e; // if data, and if trailer present this will be 0x4 which matches the trailer length of 4
}

bool BasicVRTPacket::resetForResend (const TimeStamp &t) {
  setTimeStamp(t);
  return false;
}

bool BasicVRTPacket::isChangePacket () const {
  return true;
}

boolNull BasicVRTPacket::isTimeStampMode () const {
  if (isData()) return _NULL;
  if ((bbuf[0]&TSM_BIT)!=0) return _TRUE;
  return _FALSE;
}

PacketType BasicVRTPacket::getPacketType () const {
  unsigned char c = bbuf[0];
  return (PacketType)((c >> 4) & 0xF);
}

int32_t BasicVRTPacket::getPacketCount () const {
  return bbuf[1] & 0xF;
}

int32_t BasicVRTPacket::getHeaderLength () const {
  return getOffset(PAYLOAD);
}

void BasicVRTPacket::setPacketLength (int32_t v) {
  VRTMath::packShort(bbuf, 2, v/4); // divide by 4 to convert bytes to words
}

void BasicVRTPacket::setPacketType (PacketType t) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");

  shiftHeader(STREAM_ID, 4, PacketType_hasStreamIdentifier(t));
  bbuf[0] = (char)((0x0F & bbuf[0]) | (t << 4));

  if (isData()) bbuf[0] &= ~TSM_BIT;     // clear TSM bit
  else          bbuf[0] &= ~TRAILER_BIT; // clear trailer bit
}

void BasicVRTPacket::setTimeStampMode (bool v) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");
  if (isData()) throw VRTException("Can not set time-stamp mode on a data packet.");

  if (v) bbuf[0] |=  TSM_BIT;
  else   bbuf[0] &= ~TSM_BIT;
}

TimeStamp BasicVRTPacket::getTimeStamp (double sr) const {
  IntegerMode    tsiMode = (IntegerMode   )((bbuf[1] >> 6) & 0x3);
  FractionalMode tsfMode = (FractionalMode)((bbuf[1] >> 4) & 0x3);
  int32_t        tsiOff  = getOffset(TSI);
  int32_t        tsfOff  = getOffset(TSF);
  uint32_t       tsi     = (tsiOff < 0)? 0 : VRTMath::unpackUInt(bbuf, tsiOff);
  uint64_t       tsf     = (tsfOff < 0)? 0 : VRTMath::unpackULong(bbuf, tsfOff);

  return TimeStamp(tsiMode, tsfMode, tsi, tsf, sr);
}
TimeStamp BasicVRTPacket::getTimeStamp () const {
  return getTimeStamp(DOUBLE_NAN);
}

void BasicVRTPacket::setTimeStamp (const TimeStamp &ts) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");

  IntegerMode    im = ts.getIntegerMode();
  FractionalMode fm = ts.getFractionalMode();

  // The next 4 lines must follow in the order shown!
  shiftHeader(TSI, 4, (im != IntegerMode_None));
  bbuf[1] = (char)((0x3F & bbuf[1]) | (im << 6));
  shiftHeader(TSF, 8, (fm != FractionalMode_None));
  bbuf[1] = (char)((0xCF & bbuf[1]) | (fm << 4));

  if (im != IntegerMode_None) {
    VRTMath::packUInt(bbuf, getOffset(TSI), ts.getTimeStampInteger());
  }
  if (fm != FractionalMode_None) {
    VRTMath::packULong(bbuf, getOffset(TSF), ts.getTimeStampFractional());
  }
}

void BasicVRTPacket::setPacketCount (int32_t v) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");
  if ((v < 0) || (v > 15)) throw VRTException("Invalid packet count %d",v);
  bbuf[1] = (char)((bbuf[1] & 0xF0) | v);
}

void BasicVRTPacket::setPacketCount (void *ptr, int32_t v) {
  char *buf = (char*)ptr;
  if ((v < 0) || (v > 15)) throw VRTException("Invalid packet count %d",v);
  buf[1] = (char)((buf[1] & 0xF0) | v);
}

void BasicVRTPacket::setPayloadLength (int32_t v) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");
  int32_t plen = getPayloadLength();
  int32_t hlen = getHeaderLength();
  if (v == plen) return; // no change

  if ((v < 0) || (v > MAX_PAYLOAD_LENGTH)) {
    throw VRTException("Given length of %d is outside of allowable limits.", v);
  }
  if ((v & 0x3) != 0) {
    throw VRTException("Given length of %d is not a multiple of 4-bytes.", v);
  }

  if (v > plen) {
    bbuf.reserve(hlen+v+getTrailerLength());
    bbuf.insert(bbuf.begin()+hlen+plen, v-plen, 0);
  }
  else {
    bbuf.erase(bbuf.begin()+hlen+v, bbuf.begin()+hlen+plen);
  }
  setPacketLength(hlen+v+getTrailerLength());
}

int32_t BasicVRTPacket::getStreamIdentifier () const {
  int32_t off = getOffset(STREAM_ID);
  return (off < 0)? INT32_NULL : VRTMath::unpackInt(bbuf, off);
}

void BasicVRTPacket::setStreamIdentifier (int32_t v) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");

  if (isNull(v) == hasStreamIdentifier()) {
    throw VRTException("Can not set stream identifier to %d for a TYPE=%d packet.", v, getPacketType());
  }
  if (!isNull(v)) {
    VRTMath::packInt(bbuf, getOffset(STREAM_ID), v);
  }
}

int64_t BasicVRTPacket::getClassIdentifier () const {
  int32_t off = getOffset(CLASS_ID);
  return (off < 0)? INT64_NULL : (VRTMath::unpackLong(bbuf, off) & __INT64_C(0x00FFFFFFFFFFFFFF));
}

int32_t BasicVRTPacket::getClassIdentifierOUI () {
  int32_t off = getOffset(CLASS_ID);
  return (off < 0)? INT32_NULL : VRTMath::unpackInt(bbuf, off) & 0x00FFFFFF;
}

int16_t BasicVRTPacket::getClassIdentifierICC () {
  int32_t off = getOffset(CLASS_ID);
  return (off < 0)? INT16_NULL : VRTMath::unpackShort(bbuf, off+4);
}

int16_t BasicVRTPacket::getClassIdentifierPCC () {
  int32_t off = getOffset(CLASS_ID);
  return (off < 0)? INT16_NULL : VRTMath::unpackShort(bbuf, off+6);
}

string BasicVRTPacket::getStreamID () const {
  int32_t streamID = getStreamIdentifier();
  return (isNull(streamID))? string("") : Utilities::format("%d", streamID);
}

void BasicVRTPacket::setStreamID (string v) {
  if (v.size() == 0) { setStreamIdentifier(INT32_NULL); return; }
  setStreamIdentifier(atoi(v.c_str()));
}

string BasicVRTPacket::getClassID () const {
  int64_t id = getClassIdentifier();
  if (isNull(id)) return string("");
  return Utilities::toStringClassID(id);
}

void BasicVRTPacket::setClassID (string v) {
  setClassIdentifier(Utilities::fromStringClassID(v));
}

void BasicVRTPacket::setClassIdentifier (int64_t v) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");

  if ((v != INT64_NULL) && ((v & __INT64_C(0xFF00000000000000)) != 0)) {
    throw VRTException("Invalid OUI in ClassIdentifier 0x" PRIx64, v);
  }

  int32_t oldCID   = getOffset(CLASS_ID);
  char    saveBits = (oldCID < 0)? (char)0 : bbuf[oldCID];

  if ((v == INT64_NULL) && (saveBits != 0)) {
    throw VRTException("Can not set ClassIdentifier to null when PadBitCount=%d", getPadBitCount());
  }

  shiftHeader(CLASS_ID, 8, (v != INT64_NULL));
  if (v != INT64_NULL) {
    bbuf[0] |= CLASS_ID_BIT;

    int32_t newCID = getOffset(CLASS_ID);

    VRTMath::packLong(bbuf, newCID, v);
    bbuf[newCID] = saveBits;
  }
  else {
    bbuf[0] &= ~CLASS_ID_BIT;
  }
}

void BasicVRTPacket::setClassIdentifier (int32_t oui, int16_t icc, int16_t pcc) {
  int64_t _oui = (((int64_t)(oui)) & __INT64_C(0xFFFFFFFF)) << 32;
  int64_t _icc = (((int64_t)(icc)) & __INT64_C(0x0000FFFF)) << 16;
  int64_t _pcc = (((int64_t)(pcc)) & __INT64_C(0x0000FFFF));
  setClassIdentifier(_oui|_icc|_pcc);
}

char* BasicVRTPacket::getPacketPointer () {
  return &bbuf[0];
}

char* BasicVRTPacket::getPayloadPointer () {
  return &bbuf[getHeaderLength()];
}

vector<char> BasicVRTPacket::getPacket () const {
  const char *p = &bbuf[0];
  return vector<char>(p, p+getPacketLength());
}

vector<char> BasicVRTPacket::getPayload () const {
  const char *p = &bbuf[getHeaderLength()];
  return vector<char>(p, p+getPayloadLength());
}

int32_t BasicVRTPacket::readPacket (void *buffer, int32_t poff, int32_t len) const {
  int32_t plen = getPacketLength();
  if (len > plen-poff) len = plen-poff;
  memmove(buffer, &bbuf[poff], len);
  return len;
}

int32_t BasicVRTPacket::readPayload (void *buffer, int32_t poff, int32_t len) const {
  int32_t plen = getPayloadLength();
  if (len > plen-poff) len = plen-poff;
  memmove(buffer, &bbuf[getHeaderLength()+poff], len);
  return len;
}

void BasicVRTPacket::writePayload (void *buffer, int32_t payloadOffset, int32_t len) {
  if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");

  if (len+payloadOffset > getPayloadLength()) {
    throw VRTException("Bytes to be written to payload (%d) with given start offset (%d) exceeds payload length (%d).",
                        len, payloadOffset, getPayloadLength());
  }
  memmove(&bbuf[getHeaderLength()+payloadOffset], buffer, len);
}

int32_t BasicVRTPacket::getFieldCount () const {
  return 3;
}

string BasicVRTPacket::getFieldName (int32_t id) const {
  switch (id) {
    case 0: return "StreamID";
    case 1: return "ClassID";
    case 2: return "TimeStamp";
    default: throw VRTException("Invalid field #%d in %s", id, getClassName().c_str());
  }
}

ValueType BasicVRTPacket::getFieldType (int32_t id) const {
  switch (id) {
    case 0: return ValueType_String;
    case 1: return ValueType_String;
    case 2: return ValueType_VRTObject;
    default: throw VRTException("Invalid field #%d in %s", id, getClassName().c_str());
  }
}

Value* BasicVRTPacket::getField (int32_t id) const {
  switch (id) {
    case 0:  return new Value(getStreamID());
    case 1:  return new Value(getClassID());
    case 2:  return new Value(new TimeStamp(getTimeStamp()), true);
    default: return new Value(); // null value
  }
}

void BasicVRTPacket::setField (int32_t id, const Value* val) {
  switch (id) {
    case 0: setStreamID(val->as<const string>()); break;
    case 1: setClassID( val->as<const string>()); break;
    case 2: setTimeStamp(*val->cast<const TimeStamp*>()); break;
    default: throw VRTException("Invalid field #%d in %s", id, getClassName().c_str());
  }
}

PayloadFormat::PayloadFormat () :
  hi(PayloadFormat::NULL_VAL_HI),
  lo(PayloadFormat::NULL_VAL_LO)
{
  // done
}

PayloadFormat::PayloadFormat (const PayloadFormat &pf) :
  VRTObject(pf), // <-- Used to avoid warnings under GCC with -Wextra turned on
  hi(pf.hi),
  lo(pf.lo)
{
  // done
}

PayloadFormat::PayloadFormat (int64_t bits) : hi(0), lo(0) {
  setBits(bits);
}

PayloadFormat::PayloadFormat (DataItemFormat format, int32_t size) : hi(0), lo(0) {
  setProcessingEfficient(true);
  setRealComplexType(RealComplexType_Real);
  setDataItemFormat(format);
  setRepeating(false);
  setEventTagSize(0);
  setChannelTagSize(0);
  setItemPackingFieldSize(size);
  setDataItemSize(size);
  setRepeatCount(1);
  setVectorSize(1);
}

PayloadFormat::PayloadFormat (RealComplexType type, DataItemFormat format, int32_t size) : hi(0), lo(0) {
  setProcessingEfficient(true);
  setRealComplexType(type);
  setDataItemFormat(format);
  setRepeating(false);
  setEventTagSize(0);
  setChannelTagSize(0);
  setItemPackingFieldSize(size);
  setDataItemSize(size);
  setRepeatCount(1);
  setVectorSize(1);
}

PayloadFormat::PayloadFormat (DataType dataType, RealComplexType realComplex) : hi(0), lo(0) {
  setRealComplexType(realComplex);
  setDataType(dataType);
}

PayloadFormat::PayloadFormat (bool procEfficient, RealComplexType type, DataItemFormat format,
                              bool repeating, int32_t eventTagSize, int32_t chanTagSize, int32_t fieldSize,
                              int32_t itemSize, int32_t repeatCount, int32_t vectorSize) : hi(0), lo(0) {
  setProcessingEfficient(procEfficient);
  setRealComplexType(type);
  setDataItemFormat(format);
  setRepeating(repeating);
  setEventTagSize(eventTagSize);
  setChannelTagSize(chanTagSize);
  setItemPackingFieldSize(fieldSize);
  setDataItemSize(itemSize);
  setRepeatCount(repeatCount);
  setVectorSize(vectorSize);
}

string PayloadFormat::getValid () const {
  int            fSize  = getItemPackingFieldSize();
  int            dSize  = getDataItemSize();
  int            eSize  = getEventTagSize();
  int            cSize  = getChannelTagSize();
  DataItemFormat format = getDataItemFormat();

  if (fSize < (dSize+eSize+cSize)) {
    return "Given ItemPackingFieldSize is incompatible with DataItemSize, "
           "EventTagSize, and ChannelTagSize.";
  }

  switch (format) {
    case DataItemFormat_SignedVRT1:   if (dSize <   2) return "Invalid use of DataItemFormat=SignedVRT1 with DataItemSize<2"; break;
    case DataItemFormat_SignedVRT2:   if (dSize <   3) return "Invalid use of DataItemFormat=SignedVRT2 with DataItemSize<3"; break;
    case DataItemFormat_SignedVRT3:   if (dSize <   4) return "Invalid use of DataItemFormat=SignedVRT3 with DataItemSize<4"; break;
    case DataItemFormat_SignedVRT4:   if (dSize <   5) return "Invalid use of DataItemFormat=SignedVRT4 with DataItemSize<5"; break;
    case DataItemFormat_SignedVRT5:   if (dSize <   6) return "Invalid use of DataItemFormat=SignedVRT5 with DataItemSize<6"; break;
    case DataItemFormat_SignedVRT6:   if (dSize <   7) return "Invalid use of DataItemFormat=SignedVRT6 with DataItemSize<7"; break;
    case DataItemFormat_UnsignedVRT1: if (dSize <   2) return "Invalid use of DataItemFormat=UnsignedVRT1 with DataItemSize<2"; break;
    case DataItemFormat_UnsignedVRT2: if (dSize <   3) return "Invalid use of DataItemFormat=UnsignedVRT2 with DataItemSize<3"; break;
    case DataItemFormat_UnsignedVRT3: if (dSize <   4) return "Invalid use of DataItemFormat=UnsignedVRT3 with DataItemSize<4"; break;
    case DataItemFormat_UnsignedVRT4: if (dSize <   5) return "Invalid use of DataItemFormat=UnsignedVRT4 with DataItemSize<5"; break;
    case DataItemFormat_UnsignedVRT5: if (dSize <   6) return "Invalid use of DataItemFormat=UnsignedVRT5 with DataItemSize<6"; break;
    case DataItemFormat_UnsignedVRT6: if (dSize <   7) return "Invalid use of DataItemFormat=UnsignedVRT6 with DataItemSize<7"; break;
    case DataItemFormat_Float:        if (dSize != 32) return "Invalid use of DataItemFormat=Float with DataItemSize!=32"; break;
    case DataItemFormat_Double:       if (dSize != 64) return "Invalid use of DataItemFormat=Double with DataItemSize!=64"; break;
    default: break;
  }

  return "";
}

string PayloadFormat::toString () const {
  if (isNullValue()) return "null";
  ostringstream str;
  str <<  "ProcessingEfficient="  << isProcessingEfficient();
  str << " RealComplexType="      << getRealComplexType();
  str << " DataItemFormat="       << getDataItemFormat();
  str << " Repeating="            << isRepeating();
  str << " EventTagSize="         << getEventTagSize();
  str << " ChannelTagSize="       << getChannelTagSize();
  str << " ItemPackingFieldSize=" << getItemPackingFieldSize();
  str << " DataItemSize="         << getDataItemSize();
  str << " RepeatCount="          << getRepeatCount();
  str << " VectorSize="           << getVectorSize();
  str << " DataType="             << getDataType();
  return str.str();
}

bool PayloadFormat::equals (const VRTObject &o) const {
  if (getClass() != o.getClass()) return false;
  const PayloadFormat pf = *checked_dynamic_cast<const PayloadFormat*>(&o);
  return (pf.hi == hi) && (pf.lo == lo);
}

DataType PayloadFormat::getDataType () const {
  int size = getDataItemSize();
  if (getItemPackingFieldSize() != size) return (DataType)-1; // ERROR

  switch (getDataItemFormat()) {
    case DataItemFormat_SignedInt:
      switch (size) {
        case  1:                 return DataType_UInt1; // allow packed bits as signed or unsigned
        case  4:                 return DataType_Int4;
        case  8:                 return DataType_Int8;
        case 16:                 return DataType_Int16;
        case 32:                 return DataType_Int32;
        case 64:                 return DataType_Int64;
        default:                 return (DataType)-1; // ERROR
      }
    case DataItemFormat_UnsignedInt:
      switch (size) {
        case  1:                 return DataType_UInt1; // allow packed bits as signed or unsigned
        case  4:                 return DataType_UInt4;
        case  8:                 return DataType_UInt8;
        case 16:                 return DataType_UInt16;
        case 32:                 return DataType_UInt32;
        case 64:                 return DataType_UInt64;
        default:                 return (DataType)-1; // ERROR
      }
    case DataItemFormat_Float:   return DataType_Float;
    case DataItemFormat_Double:  return DataType_Double;
    default:                     return (DataType)-1; // ERROR
  }
}

void PayloadFormat::setDataType (DataType type) {
  int size;
  switch (type) {
    case DataType_Int4:       size =  4; setDataItemFormat(DataItemFormat_SignedInt  ); break; // nibble
    case DataType_Int8:       size =  8; setDataItemFormat(DataItemFormat_SignedInt  ); break; // byte
    case DataType_Int16:      size = 16; setDataItemFormat(DataItemFormat_SignedInt  ); break; // short
    case DataType_Int32:      size = 32; setDataItemFormat(DataItemFormat_SignedInt  ); break; // int
    case DataType_Int64:      size = 64; setDataItemFormat(DataItemFormat_SignedInt  ); break; // long
    case DataType_UInt1:      size =  1; setDataItemFormat(DataItemFormat_UnsignedInt); break; // bit
    case DataType_UInt4:      size =  4; setDataItemFormat(DataItemFormat_UnsignedInt); break; // nibble
    case DataType_UInt8:      size =  8; setDataItemFormat(DataItemFormat_UnsignedInt); break; // byte
    case DataType_UInt16:     size = 16; setDataItemFormat(DataItemFormat_UnsignedInt); break; // short
    case DataType_UInt32:     size = 32; setDataItemFormat(DataItemFormat_UnsignedInt); break; // int
    case DataType_UInt64:     size = 64; setDataItemFormat(DataItemFormat_UnsignedInt); break; // long
    case DataType_Float:      size = 32; setDataItemFormat(DataItemFormat_Float      ); break; // int
    case DataType_Double:     size = 64; setDataItemFormat(DataItemFormat_Double     ); break; // long
    default:                  throw VRTException("Unsupported data type %d", type);
  }
  setItemPackingFieldSize(size);
  setDataItemSize(size);
}

int32_t PayloadFormat::getFieldCount () const {
  return 11;
}

string PayloadFormat::getFieldName (int32_t id) const {
  switch (id) {
    case  0: return "ProcessingEfficient";
    case  1: return "RealComplexType";
    case  2: return "DataItemFormat";
    case  3: return "Repeating";
    case  4: return "EventTagSize";
    case  5: return "ChannelTagSize";
    case  6: return "ItemPackingFieldSize";
    case  7: return "DataItemSize";
    case  8: return "RepeatCount";
    case  9: return "VectorSize";
    case 10: return "DataType";
    default: throw VRTException("Invalid field #%d in %s", id, getClassName().c_str());
  }
}

ValueType PayloadFormat::getFieldType (int32_t id) const {
  switch (id) {
    case  0: return ValueType_Bool;
    case  1: return ValueType_Int8;
    case  2: return ValueType_Int8;
    case  3: return ValueType_Bool;
    case  4: return ValueType_Int32;
    case  5: return ValueType_Int32;
    case  6: return ValueType_Int32;
    case  7: return ValueType_Int32;
    case  8: return ValueType_Int32;
    case  9: return ValueType_Int32;
    case 10: return ValueType_Int64;
    default: throw VRTException("Invalid field #%d in %s", id, getClassName().c_str());
  }
}

Value* PayloadFormat::getField (int32_t id) const {
  switch (id) {
    case  0: return new Value(isProcessingEfficient());
    case  1: return new Value((int8_t)getRealComplexType());
    case  2: return new Value((int8_t)getDataItemFormat());
    case  3: return new Value(isRepeating());
    case  4: return new Value(getEventTagSize());
    case  5: return new Value(getChannelTagSize());
    case  6: return new Value(getItemPackingFieldSize());
    case  7: return new Value(getDataItemSize());
    case  8: return new Value(getRepeatCount());
    case  9: return new Value(getVectorSize());
    case 10: return new Value((int64_t)getDataType());
    default: return new Value(); // null value
  }
}

void PayloadFormat::setField (int32_t id, const Value* val) {
  switch (id) {
    case  0: setProcessingEfficient(                  val->as<bool   >()); break;
    case  1: setRealComplexType(     (RealComplexType)val->as<int32_t>()); break;
    case  2: setDataItemFormat(      (DataItemFormat )val->as<int32_t>()); break;
    case  3: setRepeating(                            val->as<bool   >()); break;
    case  4: setEventTagSize(                         val->as<int32_t>()); break;
    case  5: setChannelTagSize(                       val->as<int32_t>()); break;
    case  6: setItemPackingFieldSize(                 val->as<int32_t>()); break;
    case  7: setDataItemSize(                         val->as<int32_t>()); break;
    case  8: setRepeatCount(                          val->as<int32_t>()); break;
    case  9: setVectorSize(                           val->as<int32_t>()); break;
    case 10: setDataType(            (DataType       )val->as<int64_t>()); break;
    default: throw VRTException("Invalid field #%d in %s", id, getClassName().c_str());
  }
}

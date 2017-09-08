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

#include <cstring>
#include "BasicVRLFrame.h"
using namespace vrt;

BasicVRLFrame::~BasicVRLFrame () {
  // done
}

BasicVRLFrame::BasicVRLFrame () :
  bbuf(12),
  readOnly(false)
{
  bbuf[ 0] = VRL_FAW_0;
  bbuf[ 1] = VRL_FAW_1;
  bbuf[ 2] = VRL_FAW_2;
  bbuf[ 3] = VRL_FAW_3;
  bbuf[ 4] = 0;
  bbuf[ 5] = 0;
  bbuf[ 6] = 0;
  bbuf[ 7] = 3;
  bbuf[ 8] = NO_CRC_0;
  bbuf[ 9] = NO_CRC_1;
  bbuf[10] = NO_CRC_2;
  bbuf[11] = NO_CRC_3;
}

BasicVRLFrame::BasicVRLFrame (int32_t bufsize) :
  bbuf(bufsize),
  readOnly(false)
{
  bbuf[ 0] = VRL_FAW_0;
  bbuf[ 1] = VRL_FAW_1;
  bbuf[ 2] = VRL_FAW_2;
  bbuf[ 3] = VRL_FAW_3;
  bbuf[ 4] = 0;
  bbuf[ 5] = 0;
  bbuf[ 6] = 0;
  bbuf[ 7] = 3;
  bbuf[ 8] = NO_CRC_0;
  bbuf[ 9] = NO_CRC_1;
  bbuf[10] = NO_CRC_2;
  bbuf[11] = NO_CRC_3;
}

BasicVRLFrame::BasicVRLFrame (const BasicVRLFrame &f) :
  VRTObject(f), // <-- Used to avoid warnings under GCC with -Wextra turned on
  bbuf(f.bbuf),
  readOnly(f.readOnly)
{
  // done
}

BasicVRLFrame::BasicVRLFrame (vector<char> *buf, bool readOnly) :
  bbuf(*buf),
  readOnly(readOnly)
{
  // done
}

BasicVRLFrame::BasicVRLFrame (const vector<char> &buf, bool readOnly) :
  bbuf(buf),
  readOnly(readOnly)
{
  // done
}

BasicVRLFrame::BasicVRLFrame (const vector<char> &buf, size_t size, bool readOnly) :
  bbuf(buf.begin(), buf.begin() + size),
  readOnly(readOnly)
{
  // done
}

BasicVRLFrame::BasicVRLFrame (const void *ptr, size_t size, bool readOnly) :
  bbuf(&((const char*)ptr)[0], &((const char*)ptr)[size]),
  readOnly(readOnly)
{
  // done
}

string BasicVRLFrame::toString () const {
  if (isNullValue()) return getClassName()+": <null>";

  try {
    string err = getFrameValid(false);
    if (err != "") return getClassName()+": <"+err+">";

    ostringstream str;
    str << getClassName() << ":";
    str << " FrameCount="  << getFrameCount();
    str << " FrameLength=" << getFrameLength();
    return str.str();
  }
  catch (VRTException e) {
    return getClassName()+": <Invalid VRLFrame: "+e.toString()+">";
  }
}

bool BasicVRLFrame::equals (const BasicVRLFrame &f) const {
  int32_t len = getFrameLength();
  if (f.getFrameLength() != len) return false; // quick check

  for (int32_t i= 0; i < len; i++) {
    if (bbuf[i] != f.bbuf[i]) return false;
  }
  return true;
}

 bool BasicVRLFrame::equals (const VRTObject &o) const {
  try {
    return equals(*checked_dynamic_cast<const BasicVRLFrame*>(&o));
  }
  catch (bad_cast &e) {
    UNUSED_VARIABLE(e);
    return false;
  }
}

int32_t BasicVRLFrame::hashCode () const {
  int32_t hash = 0;
  int32_t len  = min(getFrameLength(), 32);

  int32_t off = 0;
  for (int32_t i = 0; i < len; i++) {
    hash ^= (bbuf[off++] << i);
  }
  return hash;
}

string BasicVRLFrame::getFrameValid (bool strict, int32_t length) const {
  if (!isVRL(bbuf,0)) {
    return "Invalid VRLFrame: Missing frame alignment word.";
  }
  if (bbuf.size() < (size_t)getFrameLength()) {
    return "Invalid VRLFrame: Allocated buffer shorter than packet length.";
  }
  if (getFrameLength() < HEADER_LENGTH+TRAILER_LENGTH) {
    return Utilities::format("Invalid VRLFrame: Frame reports length of %d "
                             "but minimum is %d.", getFrameLength(),
                             (HEADER_LENGTH+TRAILER_LENGTH));
  }
  if (!isCRCValid()) {
    return "Invalid VRLFrame: Failed CRC check.";
  }
  if ((length != -1) && (getFrameLength() != length)) {
    return Utilities::format("Invalid VRLFrame: Invalid frame length, frame "
                             "reports %d octets, but working with %d octets.",
                             getFrameLength(), length);
  }

  if (strict) {
    int32_t start = HEADER_LENGTH;
    int32_t end   = getPacketEnd(start);
    while (end > 0) {
      start = end;
      end   = getPacketEnd(start);
    }
    if (end != NO_MORE_PACKETS) {
      return "Invalid VRLFrame: Length of packets in frame is not consistent "
             "with length of frame.";
    }
  }
  return "";
}

bool BasicVRLFrame::isCRCValid () const {
  int32_t off = getFrameLength() - TRAILER_LENGTH;
  bool noCRC = (bbuf[ off ] == NO_CRC_0)
            && (bbuf[off+1] == NO_CRC_1)
            && (bbuf[off+2] == NO_CRC_2)
            && (bbuf[off+3] == NO_CRC_3);

  return noCRC || (VRTMath::unpackInt(bbuf, off) == computeCRC());
}

void BasicVRLFrame::updateCRC () {
  int32_t off = getFrameLength() - TRAILER_LENGTH;
  VRTMath::packInt(bbuf, off, computeCRC());
}

void BasicVRLFrame::clearCRC () {
  int32_t off = getFrameLength() - TRAILER_LENGTH;
  bbuf[ off ] = NO_CRC_0;
  bbuf[off+1] = NO_CRC_1;
  bbuf[off+2] = NO_CRC_2;
  bbuf[off+3] = NO_CRC_3;
}

int32_t BasicVRLFrame::computeCRC () const {
  // References:
  //   [1] VITA 49.1
  //   [2] Warren, Henry S. Jr. "Hacker's Delight." Addison-Wesley, 2002. (ISBN 0-201-91465-4)
  const uint32_t COEFICIENTS = 0xEDB88320; // Corresponds to CRC32 polynomial
  uint32_t crc = 0;
  int32_t  end = getFrameLength() - 4; // -4 since the CRC field is skipped

  // This is based on the code given in Appendix A of [1], but it has
  // been extensively optimized. Note that to make the (COEFICIENTS * (0 or 1)) trick work
  // we need to compute the CRC in bit-reversed order and then flip the bits at the end.
  for (int32_t off = 0; off < end; off++) {
    int32_t val = bbuf[off];
    crc = (crc >> 1) ^ (COEFICIENTS * ((crc ^ (val >> 7)) & 1));
    crc = (crc >> 1) ^ (COEFICIENTS * ((crc ^ (val >> 6)) & 1));
    crc = (crc >> 1) ^ (COEFICIENTS * ((crc ^ (val >> 5)) & 1));
    crc = (crc >> 1) ^ (COEFICIENTS * ((crc ^ (val >> 4)) & 1));
    crc = (crc >> 1) ^ (COEFICIENTS * ((crc ^ (val >> 3)) & 1));
    crc = (crc >> 1) ^ (COEFICIENTS * ((crc ^ (val >> 2)) & 1));
    crc = (crc >> 1) ^ (COEFICIENTS * ((crc ^ (val >> 1)) & 1));
    crc = (crc >> 1) ^ (COEFICIENTS * ((crc ^ (val     )) & 1));
  }

  // Flip CRC bits as noted above, we are using the bit-shifting algorithm from Section 7-1 in
  // [2] rather than a loop-based solution in Appendix A of [1] which is considerably slower.
  crc = ((crc & 0x55555555) <<  1) | ((crc & 0xAAAAAAAA) >>  1);  // abcdefgh -> badcfehg (per byte)
  crc = ((crc & 0x33333333) <<  2) | ((crc & 0xCCCCCCCC) >>  2);  // badcfehg -> dcbahgfe (per byte)
  crc = ((crc & 0x0F0F0F0F) <<  4) | ((crc & 0xF0F0F0F0) >>  4);  // dcbahgfe -> hgfedcba (per byte)
  crc = ((crc & 0x00FF00FF) <<  8) | ((crc & 0xFF00FF00) >>  8);  // 0123 -> 1032
  crc = ((crc & 0x0000FFFF) << 16) | ((crc & 0xFFFF0000) >> 16);  // 1032 -> 3210

  return crc;
}

BasicVRLFrame BasicVRLFrame::copy () const {
  return BasicVRLFrame(bbuf, false);
}

int32_t BasicVRLFrame::getPacketEnd (int32_t off) const {
  return getPacketEnd(bbuf, off);
}

int32_t BasicVRLFrame::getPacketCount () const {
  int32_t count = 0;
  int32_t end   = getPacketEnd(HEADER_LENGTH);

  while (end > 0) {
    count++;
    end = getPacketEnd(end);
  }
  return count;
}

vector<void*> BasicVRLFrame::getVRTPacketsRW () {
  // This functions needs to be cleaned up... it looks this way since it
  // pre-dates some of the newer functionality, and we didn't want to spend
  // too much time revising it until we have the iterators ready for use.
  vector<void*> list;
  int32_t start = HEADER_LENGTH;
  int32_t end   = getPacketEnd(bbuf, start);
  while (end > 0) {
    list.push_back(&bbuf[start]);
    start = end;
    end   = getPacketEnd(bbuf, start);
  }
  if (end != NO_MORE_PACKETS) {
    return vector<void*>(0); // error condition indicated
  }
  return list;
}

vector<BasicVRTPacket*> BasicVRLFrame::getVRTPackets () const {
  // This functions needs to be cleaned up... it looks this way since it
  // pre-dates some of the newer functionality, and we didn't want to spend
  // too much time revising it until we have the iterators ready for use.
  vector<BasicVRTPacket*> list;
  int32_t start = HEADER_LENGTH;
  int32_t end   = getPacketEnd(bbuf, start);
  while (end > 0) {
    if ((bbuf[start] & 0xC0) == 0) {
      list.push_back(new BasicDataPacket(bbuf, start, end, readOnly));
    }
    else {
      list.push_back(new BasicContextPacket(bbuf, start, end, readOnly));
    }
    start = end;
    end   = getPacketEnd(bbuf, start);
  }
  if (end != NO_MORE_PACKETS) {
    for (size_t i=0; i<list.size(); i++) {
      delete list[i];
    }
    return vector<BasicVRTPacket*>(0); // error condition indicated
  }
  return list;
}


vector<vector<char>*> *BasicVRLFrame::getVRTPackets (const vector<char> &buffer, int32_t len) {
  vector<vector<char>*> *list = new vector<vector<char>*>();
  try {
    int32_t start = HEADER_LENGTH;
    int32_t end   = getPacketEnd(buffer, start);
    while (end > 0) {
      list->push_back(new vector<char>(buffer.begin()+start, buffer.begin()+end));
      start = end;
      end   = getPacketEnd(buffer, start);
      if (end > len) break; // ERROR
    }
    if (end == NO_MORE_PACKETS) {
      return list;
    }
  }
  catch (VRTException e) {
    // ignore and just return null below
    UNUSED_VARIABLE(e);
  }

  // Error condition, delete anything and return null
  if (list != NULL) {
    for (size_t i = 0; i < list->size(); i++) {
      delete list->at(i);
    }
    delete list;
  }
  return NULL;
}

int32_t BasicVRLFrame::setVRTPackets (bool fit, int32_t maxFrameLength,
                                      const BasicVRTPacket *p) {
  if (readOnly ) throw VRTException("Frame is read only");
  if (p == NULL) throw VRTException("Null packet given");

  int32_t len = HEADER_LENGTH + TRAILER_LENGTH;
  string  err = p->getPacketValid(true);
  if (!isNull(err)) throw VRTException(err);

  int32_t plen   = p->getPacketLength();
  int32_t length = len + plen;
  if (length < maxFrameLength) {
    len = length;
  }
  else if (fit) {
    return 0;
  }
  else if (maxFrameLength == MAX_FRAME_LENGTH) {
    throw VRTException("Total packet length exceeds MAX_FRAME_LENGTH");
  }
  else {
    throw VRTException("Total packet length exceeds buffer length");
  }
  bbuf.resize(len);

  // Note that the code below may cause the old CRC to be overwritten, but since we clear it
  // via the call to setFrameLength, this should not be an issue.
  p->readPacket(&bbuf[HEADER_LENGTH], 0, plen);
  clearCRC();
  setFrameLength(len);
  return 1;
}

int32_t BasicVRLFrame::setVRTPackets (bool fit, int32_t maxFrameLength,
                                      const vector<BasicVRTPacket>  *packets,
                                      const vector<BasicVRTPacket*> *packetPointers) {
  size_t packetCount = (packets != NULL)? packets->size() : packetPointers->size();

  if (readOnly        ) throw VRTException("Frame is read only");
  if (packetCount == 0) return 0;
  if (packetCount == 1) {
    const BasicVRTPacket *p = (packets != NULL)? (&(packets->at(0))) : packetPointers->at(0);
    return setVRTPackets(fit, maxFrameLength, p);
  }

  int32_t count = 0;
  int32_t len   = HEADER_LENGTH + TRAILER_LENGTH;

  for (size_t i = 0; i < packetCount; i++) {
    const BasicVRTPacket *p = (packets != NULL)? (&(packets->at(i))) : packetPointers->at(i);
    if (p == NULL) throw VRTException("Null packet given");
    string err = p->getPacketValid(false);
    if (err != "") throw VRTException(err);
    int32_t length = len + p->getPacketLength();

    if (length < maxFrameLength) {
      len = length;
      count++;
    }
    else if (fit) {
      break;
    }
    else if (maxFrameLength == MAX_FRAME_LENGTH) {
      throw VRTException("Total packet length exceeds MAX_FRAME_LENGTH");
    }
    else {
      throw VRTException("Total packet length exceeds buffer length");
    }
  }
  bbuf.resize(len);

  // Note that the code below may cause the old CRC to be overwritten, but since we clear it
  // via the call to setFrameLength, this should not be an issue.
  int32_t off = HEADER_LENGTH;
  for (int32_t i = 0; i < count; i++) {
    const BasicVRTPacket *p = (packets != NULL)? (&(packets->at(i))) : packetPointers->at(i);
    int32_t plen = p->getPacketLength();
    p->readPacket(&bbuf[off], 0, plen);
    off += plen;
  }
  setFrameLength(len);
  return count;
}

int32_t BasicVRLFrame::getFrameCount () const {
  return (VRTMath::unpackInt(bbuf, 4) >> 20) & 0x00000FFF;
}

void BasicVRLFrame::setFrameCount (int32_t count) {
  if (readOnly) throw VRTException("Frame is read only");
  if ((count < 0) || (count > 0x00000FFF)) {
    throw VRTException("Invalid frame count %d", count);
  }

  int32_t val = VRTMath::unpackInt(bbuf, 4);
  val = (count << 20) | (val & 0x000FFFFF);
  VRTMath::packInt(bbuf, 4, val);
  clearCRC();
}

void BasicVRLFrame::setFrameLength (int32_t length) {
  if (readOnly) throw VRTException("Frame is read only");
  if (((length & 0x3) != 0) || (length < MIN_FRAME_LENGTH)) {
    throw VRTException("Invalid frame length %d", length);
  }

  int32_t val = VRTMath::unpackInt(bbuf, 4);
  val = (val & 0xFFF00000) | ((length>>2) & 0x000FFFFF); // >>2 to convert bytes to words
  VRTMath::packInt(bbuf, 4, val);
  clearCRC();
}

void* BasicVRLFrame::getFramePointer () {
  return &bbuf[0];
}


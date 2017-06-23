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

#include "AbstractPacketFactory.h"
#include "AbstractVRAFile.h"
#include "BasicVRLFrame.h"
#include "Utilities.h"
#include "VRTMath.h"
#include <string.h>     // for memcmp(..)


using namespace vrt;

const string AbstractVRAFile::FILE_NAME_EXT = ".vra";
const string AbstractVRAFile::MIME_TYPE     = "application/x-vita-radio-archive";



static const char DEFAULT_HEADER[20] = { 'V','R','A','F',
                                         AbstractVRAFile::DEFAULT_VERSION, 0, 0, 0,
                                         0, 0, 0, 0,
                                         0, 0, 0, 0,
                                         'V','E','N','D' };

AbstractVRAFile::AbstractVRAFile (const AbstractVRAFile &f) :
  hdrVersion(f.hdrVersion),
  hdrFileLength(f.hdrFileLength),
  hdrCRC(f.hdrCRC),
  uri(f.uri),
  isRead(f.isRead),
  isWrite(f.isWrite),
  isSetSize(f.isSetSize),
  isSetCRC(f.isSetCRC),
  isStrict(f.isStrict)
{
  memcpy(header, f.header, HEADER_LENGTH);
}

AbstractVRAFile::AbstractVRAFile (string uri, bool isRead,    bool isWrite,
                                  bool isSetSize, bool isSetCRC,
                                  bool isStrict) :
  hdrVersion(DEFAULT_VERSION),
  hdrFileLength(0),
  hdrCRC(BasicVRLFrame::NO_CRC),
  uri(uri),
  isRead(isRead),
  isWrite(isWrite),
  isSetSize(isSetSize),
  isSetCRC(isSetCRC),
  isStrict(isStrict)
{
  memcpy(header, DEFAULT_HEADER, HEADER_LENGTH);
}

string AbstractVRAFile::toString () const {
    // Do not include the file length or version since it may not be readable if
    // the file isn't open and this may result in errors with exception handling,
    // etc.
    return getClassName() + ": URI=" + getURI();
}

bool AbstractVRAFile::equals (const AbstractVRAFile &that) const {
  bool ok = (this->getVersion()    == that.getVersion())
         && (this->getFileLength() == that.getFileLength());

  if (!ok) return false;

  // If both have a matching FileLength in the header, and both have their
  // CRC set, then the CRC values must be equal. If not, we can assume the
  // files are not equal.
  if ((hdrFileLength == that.hdrFileLength) &&
      (hdrCRC != BasicVRLFrame::NO_CRC) && (that.hdrCRC != BasicVRLFrame::NO_CRC) &&
      (hdrCRC != that.hdrCRC)) {
    return false;
  }

  if (memcmp(header, that.header, HEADER_LENGTH) != 0) return false;

  ConstPacketIterator this_begin = this->begin();
  ConstPacketIterator that_begin = that.begin();
  ConstPacketIterator this_end   = this->end();
  ConstPacketIterator that_end   = that.end();
  
  while (this_begin != this_end) {
    if (that_begin == that_end) return false;
    
    BasicVRTPacket *this_pkt = *this_begin;
    BasicVRTPacket *that_pkt = *that_begin;
    
    if (!this_pkt->equals(*that_pkt)) {
      return false;
    }
    
    delete this_pkt;
    delete that_pkt;
    
    ++this_begin;
    ++that_begin;
  }
  
  return (that_begin == that_end);
}

bool AbstractVRAFile::isFileValid () const {
  if (!isFileValid0()) return false;
    
  int64_t lenFromHeader = getFileLengthHeader();
  int64_t lenFromOS     = getFileLengthOS();

  if (lenFromOS > 0) {
    if ((lenFromHeader != 0) && (lenFromOS < lenFromHeader)) {
      return false; // file on disk shorter than header reports
    }
    if ((lenFromHeader == 0) && ((lenFromOS & 0x03) != 0)) {
      return false; // file not a multiple of 32-bits in length
    }
  }

  try {
    ConstPacketIterator this_begin = this->begin();
    ConstPacketIterator this_end   = this->end();
    
    while (this_begin != this_end) {
      ++this_begin; // <-- throws exception if invalid
    }
  }
  catch (VRTException e) {
    return false;
  }
  return true;
}

bool AbstractVRAFile::isFileValid (int64_t length) const {
  return isFileValid() && (getFileLength() == length);
}

void AbstractVRAFile::updateCRC () {
  int32_t crc = (isSetCRC)? computeCRC() : BasicVRLFrame::NO_CRC;
  setCRC(crc);
}

void AbstractVRAFile::updateFileLength () {
  int64_t len = (isSetSize)? getFileLengthRW() : __INT64_C(0);
  setFileLengthHeader(len);
}

int64_t AbstractVRAFile::getFileLength () const {
  int64_t length = getFileLengthHeader();
  return (length != 0)? length : getFileLengthOS();
}

void AbstractVRAFile::append (BasicVRTPacket &p) {
  string err = p.getPacketValid(isStrict);
  if (!isNull(err)) throw VRTException(err);
  write(EOF, p.getPacketPointer(), p.getPacketLength());
}

void AbstractVRAFile::close () {
  if (isWrite) {
    flush(true);
  }
}

void AbstractVRAFile::open () {
  if (isRead) {
    readHeader();
  }
  if (isWrite) {
    // If we don't need to set FileLength or CRC, clear them now so we don't
    // need to fuss with them in the future.
    if (!isSetSize) _setFileLengthHeader(__INT64_C(0));
    if (!isSetCRC ) _setCRC(BasicVRLFrame::NO_CRC);
    writeHeader();
  }
}

/** Reads the header in. */
void AbstractVRAFile::readHeader () {
  read(__INT64_C(0), header, HEADER_LENGTH);
  hdrVersion    = 0xFF & header[4];
  hdrFileLength = VRTMath::unpackLong(header, 8);
  hdrCRC        = VRTMath::unpackInt(header, 16);
}

/** The very basic frame validity checks. */
bool AbstractVRAFile::isFileValid0 () const {
  return (getFileLength() >= HEADER_LENGTH)
      && (header[0] == VRA_FAW_0)
      && (header[1] == VRA_FAW_1)
      && (header[2] == VRA_FAW_2)
      && (header[3] == VRA_FAW_3)
      && isCRCValid();
}

int32_t AbstractVRAFile::computeCRC () const {
  // References:
  //   [1] VITA 49.1
  //   [2] Warren, Henry S. Jr. "Hacker's Delight." Addison-Wesley, 2002. (ISBN 0-201-91465-4)
  const uint32_t COEFICIENTS = 0xEDB88320; // Corresponds to CRC32 polynomial
  uint32_t crc = 0;

  // This is based on the code given in Appendix A of [1], but it has
  // been extensively optimized. Note that to make the (COEFICIENTS * (0 or 1)) trick work
  // we need to compute the CRC in bit-reversed order and then flip the bits at the end.

  // compute over the header
  for (int32_t off = 0; off < HEADER_LENGTH; off++) {
    if ((off >= 16) && (off < 20)) continue; // skip CRC field
    uint32_t val = (uint32_t)header[off];
    crc = (crc >> 1) ^ (COEFICIENTS * ((crc ^ (val >> 7)) & 1));
    crc = (crc >> 1) ^ (COEFICIENTS * ((crc ^ (val >> 6)) & 1));
    crc = (crc >> 1) ^ (COEFICIENTS * ((crc ^ (val >> 5)) & 1));
    crc = (crc >> 1) ^ (COEFICIENTS * ((crc ^ (val >> 4)) & 1));
    crc = (crc >> 1) ^ (COEFICIENTS * ((crc ^ (val >> 3)) & 1));
    crc = (crc >> 1) ^ (COEFICIENTS * ((crc ^ (val >> 2)) & 1));
    crc = (crc >> 1) ^ (COEFICIENTS * ((crc ^ (val >> 1)) & 1));
    crc = (crc >> 1) ^ (COEFICIENTS * ((crc ^ (val     )) & 1));
  }

  // compute over the payload
  int64_t offset = HEADER_LENGTH;
  int64_t end    = getFileLength();
  char    buffer[4096];
  while (offset < end) {
    int32_t numRead = read(offset, buffer, 4096);

    for (int32_t off = 0; off < numRead; off++) {
      uint32_t val = (uint32_t)buffer[off];
      crc = (crc >> 1) ^ (COEFICIENTS * ((crc ^ (val >> 7)) & 1));
      crc = (crc >> 1) ^ (COEFICIENTS * ((crc ^ (val >> 6)) & 1));
      crc = (crc >> 1) ^ (COEFICIENTS * ((crc ^ (val >> 5)) & 1));
      crc = (crc >> 1) ^ (COEFICIENTS * ((crc ^ (val >> 4)) & 1));
      crc = (crc >> 1) ^ (COEFICIENTS * ((crc ^ (val >> 3)) & 1));
      crc = (crc >> 1) ^ (COEFICIENTS * ((crc ^ (val >> 2)) & 1));
      crc = (crc >> 1) ^ (COEFICIENTS * ((crc ^ (val >> 1)) & 1));
      crc = (crc >> 1) ^ (COEFICIENTS * ((crc ^ (val     )) & 1));
    }
    offset += numRead;
  }

  // Flip CRC bits as noted above, we are using the bit-shifting algorithm from Section 7-1 in
  // [2] rather than a loop-based solution in Appendix A of [1] which is considerably slower.
  crc = ((crc & 0x55555555) <<  1) | ((crc & 0xAAAAAAAA) >>  1);  // abcdefgh -> badcfehg (per byte)
  crc = ((crc & 0x33333333) <<  2) | ((crc & 0xCCCCCCCC) >>  2);  // badcfehg -> dcbahgfe (per byte)
  crc = ((crc & 0x0F0F0F0F) <<  4) | ((crc & 0xF0F0F0F0) >>  4);  // dcbahgfe -> hgfedcba (per byte)
  crc = ((crc & 0x00FF00FF) <<  8) | ((crc & 0xFF00FF00) >>  8);  // 0123 -> 1032
  crc = ((crc & 0x0000FFFF) << 16) | ((crc & 0xFFFF0000) >> 16);  // 1032 -> 3210

  return (int32_t)crc;
}

bool AbstractVRAFile::_setVersion (int32_t ver) {
  if (hdrVersion == ver) return false;
  if (!isWrite) throw VRTException("File is read-only");

  if ((ver < MIN_VERSION_SUPPORTED) || (ver > MAX_VERSION_SUPPORTED)) {
    throw VRTException("Invalid version specified, expected version in the range %d <= ver <= %d "
                       "but given ver=%d.", MIN_VERSION_SUPPORTED, MAX_VERSION_SUPPORTED, ver);
  }
  header[4] = (char)ver;
  return true;
}

bool AbstractVRAFile::_setFileLengthHeader (int64_t len) {
  if (hdrFileLength == len) return false;
  if (!isWrite) throw VRTException("File is read-only");
  hdrFileLength = len;
  VRTMath::packLong(header, 8, len);
  return true;
}

bool AbstractVRAFile::_setCRC (int32_t crc) {
  if (hdrCRC == crc) return false;
  if (!isWrite) throw VRTException("File is read-only");
  hdrCRC = crc;
  VRTMath::packInt(header, 16, crc);
  return true;
}

void AbstractVRAFile::writeHeader () {
  write(0, header, HEADER_LENGTH, false);
}

void AbstractVRAFile::flush (bool force) {
  if (force) {
    // Since this is called frequently, we do some stuff manually so that
    // we can use a single writeHeader() call. Note that setting of the
    // length must come *before* computation of the updated CRC.
    int64_t len      = (isSetSize)? getFileLengthRW() : 0L;
    bool    writeLen = _setFileLengthHeader(len);
    int32_t crc      = (isSetCRC)? computeCRC() : BasicVRLFrame::NO_CRC;
    bool    writeCRC = _setCRC(crc);

    if (writeLen || writeCRC) {
      writeHeader();
    }
  }
}

ConstPacketIterator AbstractVRAFile::begin () const {
  return ConstPacketIterator(this, HEADER_LENGTH);
}

ConstPacketIterator AbstractVRAFile::end () const {
  return ConstPacketIterator(this, getFileLength());
}

void AbstractVRAFile::gotoNextPacket (ConstPacketIterator &pi) const {
  if (pi.length < 0) getThisPacket(pi, true); // <-- updates pi.length
  pi.offset += pi.length;
  pi.length = -1;
}

BasicVRTPacket* AbstractVRAFile::getThisPacket (ConstPacketIterator &pi, bool skip) const {
  if (pi.offset >= getFileLength()) {
    throw VRTException("No such element in %s", pi.toString().c_str());
  }
  
  // ==== DUPLICATE READ? ====================================================
  if (pi.length > 0) {
    if (skip) {
      return NULL;
    }
    else if (pi.resolve) {
      BasicVRTPacket p(&pi.buf[0], pi.buf.size(), false);
      return VRTConfig::getPacket(&p);
    }
    else {
      return new BasicVRTPacket(&pi.buf[0], pi.buf.size(), false);
    }
  }
  
  // ==== READ PACKET HEADER =================================================
  int32_t numRead = read(pi.offset, &pi.buf[0], 4);

  if (numRead != 4) {
    // Invalid File
    throw VRTException("Error reading from %s at %" PRId64, toString().c_str(), pi.offset);
  }
  int32_t len = ((0xFF & pi.buf[2]) << 10) | ((0xFF & pi.buf[3]) << 2);

  if ((pi.offset+len > getFileLength()) || (len < 4)) {
    // Invalid Packet
    throw VRTException("Error reading from %s at %" PRId64, toString().c_str(), pi.offset);
  }

  // ==== UPDATE LENGTH ======================================================
  pi.length = len;
  
  
  // ==== READ PACKET ========================================================
  if (!skip) {
    pi.buf.resize(len);

    int32_t toRead = len - 4;

    do {
      int32_t n = read(pi.offset+numRead, &pi.buf[numRead], toRead);
      if (n < 0) {
        // Invalid Packet
        throw VRTException("Error reading from %s at %" PRId64, toString().c_str(), pi.offset);
      }
      numRead += n;
      toRead  -= n;
    } while (numRead < toRead);


    if (pi.resolve) {
      BasicVRTPacket p(&pi.buf[0], pi.buf.size(), false);
      return VRTConfig::getPacket(&p);
    }
    else {
      return new BasicVRTPacket(&pi.buf[0], pi.buf.size(), false);
    }
  }
  else {
    return NULL;
  }
}

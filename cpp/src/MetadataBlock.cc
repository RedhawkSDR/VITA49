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

#include "MetadataBlock.h"
#include <cstring>
#include <iostream>
#include <sstream>
#include <string.h>

using namespace vrt;

static const int32_t      NULL_FORM_LEN     = 1;
static const int32_t      SHORT_FORM_LEN    = 2;
static const int32_t      STANDARD_FORM_LEN = 4;
static const int32_t      LONG_FORM_LEN     = 8;
static       vector<char> EMPTY_BUF(0);

static inline int32_t max (int32_t a, int32_t b) {
  return (a < b)? b : a;
}
static inline int32_t max (int32_t a, size_t b) {
  return (a < 0)? a : max(a, (int32_t)b);
}

MetadataEntry::MetadataEntry (const MetadataEntry &m)  :
  VRTObject(m), // <-- Used to avoid warnings under GCC with -Wextra turned on
  level(m.level),
  key(m.key),
  val(m.val)
{
  // done
}

MetadataEntry::MetadataEntry () :
  level(-1),
  key(),
  val()
{
  // done
}

MetadataEntry::MetadataEntry (int32_t level, const string &key, const wstring &val) :
  level(-1),
  key(),
  val()
{
  setLevel(level);
  setKey(key);
  setValue(val);
}

string MetadataEntry::toString () const {
  ostringstream str;
  str << level << ": " << key << "=" << val;
  return str.str();
}

void MetadataEntry::setKey (const string *key) {
  if (key == NULL) {
    this->key     = string();
    this->keyNull = true;
  }
  else {
    if (key->length() > 32767) {
      throw VRTException("Given key (%s), exceeds 32767 length limit.", key->c_str());
    }
    this->key     = *key;
    this->keyNull = false;
  }
}

void MetadataEntry::setValue (const wstring *val) {
  if (val == NULL) {
    this->val     = wstring();
    this->valNull = true;
  }
  else {
    int32_t lval = VRTMath::packUTF8(EMPTY_BUF, 0, *val, 0);
    if (lval > 32767) {
      throw VRTException("Given value (%ls), exceeds 32767 length limit.", val->c_str());
    }
    this->val     = *val;
    this->valNull = false;
  }
}

int32_t MetadataEntry::writeBytes (vector<char> &buf, int32_t off, int32_t len) const {
  int32_t keyLength = (keyNull)? 0 : max(1, key.length());
  int32_t valLength = (valNull)? 0 : max(1, VRTMath::packUTF8(EMPTY_BUF, 0, val, 0));

  int32_t headLength;
  if (level == -1) {
    headLength = NULL_FORM_LEN;
    keyLength  = 0; // <-- make sure this is written as null
    valLength  = 0; // <-- make sure this is written as null
  }
  else if ((level > 0x1F) || (keyLength > 0xFF) || (valLength + keyLength + 8 > 0xFFFF)) {
    headLength = LONG_FORM_LEN;
  }
  else if ((level > 0x03) || (keyLength > 0x0F) || (valLength+keyLength+4 > 0xFF)) {
    headLength = STANDARD_FORM_LEN;
  }
  else {
    headLength = SHORT_FORM_LEN;
  }

  int32_t totalLength = headLength+keyLength+valLength;
  if (totalLength > len) return -totalLength;

  switch (headLength) {
    case NULL_FORM_LEN:
      buf[ off ] = 0;
      break;
    case SHORT_FORM_LEN:
      buf[ off ] = (int8_t)(((level & 0x3) << 4) | (keyLength & 0x0F));
      buf[off+1] = (int8_t)(totalLength & 0xFF);
      break;
    case STANDARD_FORM_LEN:
      buf[ off ] = (int8_t)(0xC0 | (level & 0x1F));
      buf[off+1] = (int8_t)(keyLength & 0xFF);
      buf[off+2] = (int8_t)((totalLength >> 8) & 0xFF);
      buf[off+3] = (int8_t)(  totalLength      & 0xFF);
      break;
    case LONG_FORM_LEN:
      buf[ off ] = (int8_t)(0xC0 | ((level >> 8) & 0x0F));
      buf[off+1] = (int8_t)(level & 0xFF);
      buf[off+2] = (int8_t)((keyLength   >>  8) & 0xFF);
      buf[off+3] = (int8_t)( keyLength          & 0xFF);
      buf[off+4] = (int8_t)((totalLength >> 24) & 0xFF);
      buf[off+5] = (int8_t)((totalLength >> 16) & 0xFF);
      buf[off+6] = (int8_t)((totalLength >>  8) & 0xFF);
      buf[off+7] = (int8_t)(  totalLength       & 0xFF);
      break;
  }

  if (keyLength > 0) VRTMath::packAscii(buf, off+headLength,           key, keyLength);
  if (valLength > 0) VRTMath::packUTF8( buf, off+headLength+keyLength, val, valLength);
  return totalLength;
}


int32_t MetadataEntry::readBytes (const vector<char> &buf, int32_t off, int32_t len) {
  if (len < 1) throw VRTException("Unreadable metadata block (invalid length).");
  int32_t headLength;

       if ((buf[off] & 0x80) == 0x00) headLength = NULL_FORM_LEN;
  else if ((buf[off] & 0xC0) == 0x80) headLength = SHORT_FORM_LEN;
  else if ((buf[off] & 0xE0) == 0xC0) headLength = STANDARD_FORM_LEN;
  else if ((buf[off] & 0xF0) == 0xE0) headLength = LONG_FORM_LEN;
  else throw VRTException("Unreadable metadata block (invalid entry header).");

  if (len < headLength) throw VRTException("Unreadable metadata block (invalid length).");

  int32_t keyLength;
  int32_t totalLength;
  switch (headLength) {
    case NULL_FORM_LEN:
      level       = 0;
      keyLength   = 0;
      totalLength = 1;
      break;
    case SHORT_FORM_LEN:
      level       = (buf[ off ] >> 4) & 0x07;
      keyLength   =  buf[ off ]       & 0x0F;
      totalLength =  buf[off+1]       & 0xFF;
      break;
    case STANDARD_FORM_LEN:
      level       = buf[ off ] & 0x1F;
      keyLength   = buf[off+1] & 0xFF;
      totalLength = (buf[off+2] << 8) | (buf[off+3] & 0xFF);
      break;
    case LONG_FORM_LEN:
      level       = ((buf[ off ] & 0x0F) << 8) | (buf[off+1] & 0xFF);
      keyLength   = ((buf[off+2] & 0x7F) << 8) | (buf[off+3] & 0xFF);
      totalLength = ((buf[off+4] & 0x7F) << 24) | ((buf[off+5] & 0xFF) << 16)
                  | ((buf[off+6] & 0xFF) <<  8) | (buf[off+7] & 0xFF);
      break;
    default:
      throw VRTException("This should be impossible");
  }

  int32_t valLength = totalLength - headLength - keyLength;
  if (len < totalLength) throw VRTException("Unreadable metadata block (invalid length).");
  key     = (keyLength == 0)? string()  : VRTMath::unpackAscii(buf, off+headLength,           len-headLength);
  val     = (valLength == 0)? wstring() : VRTMath::unpackUTF8( buf, off+headLength+keyLength, len-headLength-keyLength);
  keyNull = (keyLength == 0);
  valNull = (valLength == 0);

  return totalLength;
}


MetadataBlock::MetadataBlock (const MetadataBlock &m) :
  VRTObject(m), // <-- Used to avoid warnings under GCC with -Wextra turned on
  entries(m.entries)
{
  // done
}

MetadataBlock::MetadataBlock () :
  entries()
{
  // done
}

MetadataBlock::MetadataBlock (const vector<MetadataEntry> &entries)  :
  entries(entries)
{
  // done
}

string MetadataBlock::toString () const {
  ostringstream str;
  str << "MetadataBlock:";

  for (size_t n = 0; n < entries.size(); n++) {
    MetadataEntry e = entries[n];
    str << "\n  ";
    for (int32_t i = 1; i < e.getLevel(); i++) {
      str << " | ";
    }
    str << " +- " << e.getKey() << "=" << e.getValue();
  }
  return str.str();
}

int32_t MetadataBlock::getLengthInBytes () const {
  return writeBytes(EMPTY_BUF, 0, 0);
}

void MetadataBlock::readBytes (const vector<char> &buf, int32_t off, int32_t len, bool keepNull) {
  vector<MetadataEntry> list;

  int32_t end = off + len;
  while (off < end) {
    MetadataEntry e;
    int32_t num = e.readBytes(buf, off, len);
    off += num;
    len -= num;
    if (keepNull || !e.isNull()) {
      list.push_back(e);
    }
  }
  this->entries = list;
}

int32_t MetadataBlock::writeBytes (vector<char> &buf, int32_t off, int32_t len) const {
  int32_t length = 0;

  for (size_t i = 0; i < entries.size(); i++) {
    MetadataEntry e = entries[i];

    int32_t num = e.writeBytes(buf, off, len);
    if (num < 0) {
      off = 0;
      len = 0;
      length += -num;
    }
    else {
      off += num;
      len -= num;
      length += num;
    }
  }
  memset(&buf[off], 0, len);
  return length;
}




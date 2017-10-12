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

#include "Record.h"

using namespace vrt;

Record::Record (const Record &r) :
  VRTObject(r), // <-- Used to avoid warnings under GCC with -Wextra turned on
  strictEquality(r.strictEquality),
  buf(r.buf)
{
  // done
}

Record::Record (int32_t len, bool strictEquality) :
  strictEquality(strictEquality),
  buf(vector<char>(len))
{
  // done
}

void Record::setByteLength (int32_t len, int32_t off) {
  if ((size_t)len == buf.size()) return;

  if (off == -1) {
    buf.reserve(len);
    buf.resize(len, 0);
  }
  else if ((size_t)len < buf.size()) {
    int32_t delta = (int32_t)(buf.size() - len);
    buf.erase(buf.begin()+off, buf.begin()+off+delta);
  }
  else {
    int32_t delta = (int32_t)(len - buf.size());
    buf.reserve(len);
    buf.insert(buf.begin()+off, delta, 0);
  }
}

bool Record::equals (const VRTObject &o) const {
  if (strictEquality) {
    if (o.getClass() != getClass()) return false;
  }
  try {
    const Record r = *checked_dynamic_cast<const Record*>(&o);
    if (buf.size() != r.buf.size()) return false;
    return (memcmp(&buf[0], &r.buf[0], buf.size()) == 0);
  }
  catch (bad_cast &e) {
    UNUSED_VARIABLE(e);
    return false;
  }
}

bool Record::unpackBit (int32_t off, int32_t bit) const {
  int32_t iByte = 3 - (bit / 8);
  int32_t iBit  = 0x1 << (bit % 8);
  if ((buf[off+iByte] & iBit) != 0) return true;
                                    return false;
}

boolNull Record::getStateEventBit (int32_t off, int32_t enable, int32_t indicator) const {
  int32_t eByte = 3 - (enable    / 8);
  int32_t iByte = 3 - (indicator / 8);
  int32_t eBit  = 0x1 << (enable    % 8);
  int32_t iBit  = 0x1 << (indicator % 8);

  if ((buf[off+eByte] & eBit) == 0) return _NULL;
  if ((buf[off+iByte] & iBit) != 0) return _TRUE;
                                    return _FALSE;
}

int32_t Record::getFieldCount () const {
  return 0;
}

string Record::getFieldName (int32_t id) const {
  throw VRTException("Invalid field #%d in %s", id, getClassName().c_str());
}

ValueType Record::getFieldType (int32_t id) const {
  throw VRTException("Invalid field #%d in %s", id, getClassName().c_str());
}

Value* Record::getField (int32_t id) const {
  UNUSED_VARIABLE(id);
  return new Value(); // null value
}

void Record::setField (int32_t id, const Value* val) {
  UNUSED_VARIABLE(val);
  throw VRTException("Invalid field #%d in %s", id, getClassName().c_str());
}

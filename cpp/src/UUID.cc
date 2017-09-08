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

#include "UUID.h"
#include <stdlib.h>

using namespace vrt;

#if INTERNAL_UUID_LIB != 0
# if INTERNAL_UUID_LIB == 2
#  include <gcrypt.h> // used for gcry_randomize(..)
#  ifndef _GCRYPT_H
#   error ****************************************************
#   error * Could not find system GCRYPT library, try building
#   error * with INTERNAL_UUID_LIB=1
#   error ****************************************************
#  endif
    void uuid_generate (uuid_t out) {
      unsigned char *u = (unsigned char*)out;

      gcry_randomize(u, 16, GCRY_STRONG_RANDOM);

      u[6] = (u[6] & 0x0F) | 0x40; // version = 1000 (4 = random)
      u[8] = (u[8] & 0x3F) | 0x80; // variant = 10x  (RFC 4122)
    }
# endif
  void uuid_clear (uuid_t out);
  int  uuid_compare (const uuid_t uu1, const uuid_t uu2);
  void uuid_generate (uuid_t out);
  int  uuid_is_null (const uuid_t uu);
  void uuid_unparse (const uuid_t uu, char *out);
  int  uuid_parse (char *in, uuid_t uu);

#endif

UUID::UUID () {
  uuid_clear(uu);
}

UUID::UUID (const UUID &uuid) :
  VRTObject(uuid)  // <-- Used to avoid warnings under GCC with -Wextra turned on
{
  memcpy(uu, uuid.uu, 16);
}

UUID::UUID (const uuid_t uuid) {
  memcpy(uu, uuid, 16);
}

#if INTERNAL_UUID_LIB != 1
  UUID UUID::randomUUID () {
    UUID val;
    uuid_generate(val.uu);
    return val;
  }
#endif

bool UUID::isNullValue () const {
  return (uuid_is_null(uu) != 0);
}

bool UUID::equals (const VRTObject &o) const {
  try {
    return equals(*checked_dynamic_cast<const UUID*>(&o));
  }
  catch (ClassCastException e) {
    UNUSED_VARIABLE(e);
    return false;
  }
}

bool UUID::equals (const UUID &uuid) const {
  return (uuid_compare(uu, uuid.uu) == 0);
}

#if INTERNAL_UUID_LIB >= 1
  /** Converts two hex chars to an Int8 returning -1 on error. */
  static inline int32_t hexToDec (char a, char b) {
    int32_t _a;
    int32_t _b;

         if ((a >= '0') && (a <= '9')) _a = a - '0';
    else if ((a >= 'A') && (a <= 'F')) _a = a - 'A' + 10;
    else if ((a >= 'a') && (a <= 'f')) _a = a - 'a' + 10;
    else                               return -1;

         if ((b >= '0') && (b <= '9')) _b = b - '0';
    else if ((b >= 'A') && (b <= 'F')) _b = b - 'A' + 10;
    else if ((b >= 'a') && (b <= 'f')) _b = b - 'a' + 10;
    else                               return -1;

    return (_a << 4) | _b;
  }

  void uuid_clear (uuid_t out) {
    char *u = (char*)out;

    for (size_t i = 0; i < 16; i++) {
      u[i] = 0;
    }
  }

  int  uuid_compare (const uuid_t uu1, const uuid_t uu2) {
    char *u1 = (char*)uu1;
    char *u2 = (char*)uu2;

    for (size_t i = 0; i < 16; i++) {
      if (u1[i] != u2[i]) return u1[i] - u2[i];
    }

    return 0;
  }

  int uuid_is_null (const uuid_t uu) {
    char *u = (char*)uu;

    for (size_t i = 0; i < 16; i++) {
      if (u[i] != 0) return 0;
    }
    return 1;
  }

  void uuid_unparse (const uuid_t uu, char *out) {
    char *u = (char*)uu;
    sprintf(out, "%.2x%.2x%.2x%.2x-"
                 "%.2x%.2x-%.2x%.2x-"
                 "%.2x%.2x-%.2x%.2x"
                 "%.2x%.2x%.2x%.2x",
                 0xFF&(int32_t)u[ 0], 0xFF&(int32_t)u[ 1], 0xFF&(int32_t)u[ 2], 0xFF&(int32_t)u[ 3],
                 0xFF&(int32_t)u[ 4], 0xFF&(int32_t)u[ 5], 0xFF&(int32_t)u[ 6], 0xFF&(int32_t)u[ 7],
                 0xFF&(int32_t)u[ 8], 0xFF&(int32_t)u[ 9], 0xFF&(int32_t)u[10], 0xFF&(int32_t)u[11],
                 0xFF&(int32_t)u[12], 0xFF&(int32_t)u[13], 0xFF&(int32_t)u[14], 0xFF&(int32_t)u[15]);
  }

  int uuid_parse (char *in, uuid_t uu) {
    if ((strlen(in) != 36) || (in[ 8] != '-') || (in[13] != '-')
                           || (in[18] != '-') || (in[23] != '-')) {
      return -1;
    }
    char u[16];
    int  t;

    t = hexToDec(in[ 0],in[ 1]);    if (t < 0) return -1;    u[ 0] = (char)t;
    t = hexToDec(in[ 2],in[ 3]);    if (t < 0) return -1;    u[ 1] = (char)t;
    t = hexToDec(in[ 4],in[ 5]);    if (t < 0) return -1;    u[ 2] = (char)t;
    t = hexToDec(in[ 6],in[ 7]);    if (t < 0) return -1;    u[ 3] = (char)t;
    t = hexToDec(in[ 9],in[10]);    if (t < 0) return -1;    u[ 4] = (char)t;
    t = hexToDec(in[11],in[12]);    if (t < 0) return -1;    u[ 5] = (char)t;
    t = hexToDec(in[14],in[15]);    if (t < 0) return -1;    u[ 6] = (char)t;
    t = hexToDec(in[16],in[17]);    if (t < 0) return -1;    u[ 7] = (char)t;
    t = hexToDec(in[19],in[20]);    if (t < 0) return -1;    u[ 8] = (char)t;
    t = hexToDec(in[21],in[22]);    if (t < 0) return -1;    u[ 9] = (char)t;
    t = hexToDec(in[24],in[25]);    if (t < 0) return -1;    u[10] = (char)t;
    t = hexToDec(in[26],in[27]);    if (t < 0) return -1;    u[11] = (char)t;
    t = hexToDec(in[28],in[29]);    if (t < 0) return -1;    u[12] = (char)t;
    t = hexToDec(in[30],in[31]);    if (t < 0) return -1;    u[13] = (char)t;
    t = hexToDec(in[32],in[33]);    if (t < 0) return -1;    u[14] = (char)t;
    t = hexToDec(in[34],in[35]);    if (t < 0) return -1;    u[15] = (char)t;

    memcpy(uu, u, 16);
    return 0;
  }
#endif

string UUID::getUUID () const {
  char str[40]; // technically only 37 are required (36 + '\0')
  uuid_unparse(uu,str);
  return str;
}

void UUID::setUUID (const string &uuid) {
  // for some reason uuid_parse(..) wants a non-const 'char*'
  char *str = (char*)malloc(uuid.length()+1);
  memcpy(str, uuid.c_str(), uuid.length()+1);
  int ok = uuid_parse(str,uu);
  free(str);
  if (ok != 0) {
    throw VRTException("Error while parsing UUID '%s'", uuid.c_str());
  }
}

void UUID::setUUID (const char *uuid) {
  if (uuid == NULL) throw VRTException("Can not set UUID to null");
  setUUID(string(uuid));
}

int32_t UUID::getFieldCount () const {
  return 1;
}

string UUID::getFieldName (int32_t id) const {
  switch (id) {
    case 0: return "UUID";
    default: throw VRTException("Invalid field #%d in %s", id, getClassName().c_str());
  }
}

ValueType UUID::getFieldType (int32_t id) const {
  switch (id) {
    case 0: return ValueType_String;
    default: throw VRTException("Invalid field #%d in %s", id, getClassName().c_str());
  }
}

Value* UUID::getField (int32_t id) const {
  switch (id) {
    case 0:  return new Value(getUUID());
    default: return new Value(); // null value
  }
}

void UUID::setField (int32_t id, const Value* val) {
  switch (id) {
    case 0:  setUUID(val->toString()); return;
    default: throw VRTException("Invalid field #%d in %s", id, getClassName().c_str());
  }
}


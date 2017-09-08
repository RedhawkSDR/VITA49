//
//  THIS FILE IS AUTO-GENERATED, DO NOT MODIFY
//

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

#include "PackUnpack.h"

using namespace std;
using namespace vrt;
using namespace vrt::VRTMath;

/** <b>Internal Use Only:</b> Fast unpack method for double data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits (must be 64)
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed? (must be true)
 */
static __attribute__((flatten)) void unpackDoubles (const void *ptr, int32_t off, double *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  // All floating-point values are signed, so 'sign' is unused, but is included
  // in function definition for consistency.
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(align);
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  // When using Double data, it is required that dSize=64 and fSize=64 so we can
  // skip some of the normal checks we would otherwise do here.
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=8) {
    array[offset++] = (double)unpackDouble(buf, off);
  }
}
/** <b>Internal Use Only:</b>
 *  Fast pack method for double data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits (must be 64)
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed? (must be true)
 */
static __attribute__((flatten)) void packDoubles (void *ptr, int32_t off, const double *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  // All floating-point values are signed, so 'sign' is unused, but is included
  // in function definition for consistency.
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(align);
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  // When using Double data, it is required that dSize=64 and fSize=64 so we can
  // skip some of the normal checks we would otherwise do here.
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=8) {
    packDouble(buf, off, (double)array[offset++]);
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for float data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits (must be 32)
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed? (must be true)
 */
static __attribute__((flatten)) void unpackFloats (const void *ptr, int32_t off, double *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  // All floating-point values are signed, so 'sign' is unused, but is included
  // in function definition for consistency.
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(dSize);
  int32_t delta; // int8_t offset delta
  if (align) delta = (fSize <= 32)? 4 : 8;
  else       delta = fSize / 8;
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=delta) {
    array[offset++] = (double)unpackFloat(buf, off);
  }
}
/** <b>Internal Use Only:</b>
 *  Fast pack method for float data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits (must be 32)
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed? (must be true)
 */
static __attribute__((flatten)) void packFloats (void *ptr, int32_t off, const double *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  // All floating-point values are signed, so 'sign' is unused, but is included
  // in function definition for consistency.
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(dSize);
  int32_t delta; // int8_t offset delta
  if (align) delta = (fSize <= 32)? 4 : 8;
  else       delta = fSize / 8;
  if (delta == 4) {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=4) {
      packFloat(buf, off, (float)array[offset++]);
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++) {
      packFloat(buf, off, (float)array[offset++]);
      off += 4;
      _Intel_Pragma("loop_count min=1, max=4")
      for (int32_t j = 4; j < delta; j++) {
        buf[off++] = 0; // set unused bits to zero
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed int64_t data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void unpackLongs (const void *ptr, int32_t off, double *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  int32_t delta = (align)? ((fSize <= 32)? 4 : 8) : (fSize/8); // int8_t offset delta
  if (sign) {
    if (dSize == 64) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (double)unpackLong(buf, off);
      }
    }
    else {
      int32_t shift  = 64 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (double)(unpackLong(buf, off) >> shift);
      }
    }
  }
  else {
    if (dSize == 64) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (double)(unpackLong(buf, off) & __INT64_C(0xFFFFFFFFFFFFFFFF));
      }
    }
    else {
      int32_t shift  = 64 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (double)((unpackLong(buf, off) & __INT64_C(0xFFFFFFFFFFFFFFFF)) >> shift);
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for unsigned int64_t data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void packLongs (void *ptr, int32_t off, const double *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  int32_t delta = (align)? ((fSize <= 32)? 4 : 8) : (fSize/8); // int8_t offset delta
  int64_t valMask = (sign)? ((int64_t)-1) : ((int64_t)__INT64_C(0xFFFFFFFFFFFFFFFF)); // Bit mask for input value (controls sign-extend)
  if (dSize == 64) {
    if (delta == 8) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=8) {
        int64_t val = (int64_t)(((int64_t)(int64_t)array[offset++]) & valMask);
        packLong(buf, off, val);
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int64_t val = (int64_t)(((int64_t)(int64_t)array[offset++]) & valMask);
        packLong(buf, off, val);
        off += 8;
        _Intel_Pragma("loop_count min=1, max=0")
        for (int32_t j = 8; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
  else {
    int32_t shift = 64 - dSize;
    if (delta == 8) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=8) {
        int64_t val = (int64_t)(((int64_t)(int64_t)array[offset++]) & valMask);
        packLong(buf, off, (int64_t)(val << shift));
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int64_t val = (int64_t)(((int64_t)(int64_t)array[offset++]) & valMask);
        packLong(buf, off, (int64_t)(val << shift));
        off += 8;
        _Intel_Pragma("loop_count min=1, max=0")
        for (int32_t j = 8; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed int32_t data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void unpackInts (const void *ptr, int32_t off, double *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  int32_t delta = (align)? ((fSize <= 32)? 4 : 8) : (fSize/8); // int8_t offset delta
  if (sign) {
    if (dSize == 32) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (double)unpackInt(buf, off);
      }
    }
    else {
      int32_t shift  = 32 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (double)(unpackInt(buf, off) >> shift);
      }
    }
  }
  else {
    if (dSize == 32) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (double)(unpackInt(buf, off) & __INT64_C(0xFFFFFFFF));
      }
    }
    else {
      int32_t shift  = 32 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (double)((unpackInt(buf, off) & __INT64_C(0xFFFFFFFF)) >> shift);
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for unsigned int32_t data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void packInts (void *ptr, int32_t off, const double *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  int32_t delta = (align)? ((fSize <= 32)? 4 : 8) : (fSize/8); // int8_t offset delta
  int32_t valMask = (sign)? ((int32_t)-1) : ((int32_t)__INT64_C(0xFFFFFFFFFFFFFFFF)); // Bit mask for input value (controls sign-extend)
  if (dSize == 32) {
    if (delta == 4) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=4) {
        int32_t val = (int32_t)(((int32_t)(int64_t)array[offset++]) & valMask);
        packInt(buf, off, val);
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int32_t val = (int32_t)(((int32_t)(int64_t)array[offset++]) & valMask);
        packInt(buf, off, val);
        off += 4;
        _Intel_Pragma("loop_count min=1, max=4")
        for (int32_t j = 4; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
  else {
    int32_t shift = 32 - dSize;
    if (delta == 4) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=4) {
        int32_t val = (int32_t)(((int32_t)(int64_t)array[offset++]) & valMask);
        packInt(buf, off, (int32_t)(val << shift));
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int32_t val = (int32_t)(((int32_t)(int64_t)array[offset++]) & valMask);
        packInt(buf, off, (int32_t)(val << shift));
        off += 4;
        _Intel_Pragma("loop_count min=1, max=4")
        for (int32_t j = 4; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed int32_t data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void unpackInt24s (const void *ptr, int32_t off, double *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  int32_t delta = (align)? ((fSize <= 32)? 4 : 8) : (fSize/8); // int8_t offset delta
  if (sign) {
    if (dSize == 24) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (double)unpackInt24(buf, off);
      }
    }
    else {
      int32_t shift  = 24 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (double)(unpackInt24(buf, off) >> shift);
      }
    }
  }
  else {
    if (dSize == 24) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (double)(unpackInt24(buf, off) & __INT64_C(0xFFFFFF));
      }
    }
    else {
      int32_t shift  = 24 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (double)((unpackInt24(buf, off) & __INT64_C(0xFFFFFF)) >> shift);
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for unsigned int32_t data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void packInt24s (void *ptr, int32_t off, const double *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  int32_t delta = (align)? ((fSize <= 32)? 4 : 8) : (fSize/8); // int8_t offset delta
  int32_t valMask = (sign)? ((int32_t)-1) : ((int32_t)__INT64_C(0xFFFFFFFFFFFFFFFF)); // Bit mask for input value (controls sign-extend)
  if (dSize == 24) {
    if (delta == 3) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=3) {
        int32_t val = (int32_t)(((int32_t)(int64_t)array[offset++]) & valMask);
        packInt24(buf, off, val);
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int32_t val = (int32_t)(((int32_t)(int64_t)array[offset++]) & valMask);
        packInt24(buf, off, val);
        off += 3;
        _Intel_Pragma("loop_count min=1, max=5")
        for (int32_t j = 3; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
  else {
    int32_t shift = 24 - dSize;
    if (delta == 3) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=3) {
        int32_t val = (int32_t)(((int32_t)(int64_t)array[offset++]) & valMask);
        packInt24(buf, off, (int32_t)(val << shift));
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int32_t val = (int32_t)(((int32_t)(int64_t)array[offset++]) & valMask);
        packInt24(buf, off, (int32_t)(val << shift));
        off += 3;
        _Intel_Pragma("loop_count min=1, max=5")
        for (int32_t j = 3; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed int16_t data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void unpackShorts (const void *ptr, int32_t off, double *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  int32_t delta = fSize / 8; // int8_t offset delta
  if (sign) {
    if (dSize == 16) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (double)unpackShort(buf, off);
      }
    }
    else {
      int32_t shift  = 16 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (double)(unpackShort(buf, off) >> shift);
      }
    }
  }
  else {
    if (dSize == 16) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (double)(unpackShort(buf, off) & __INT64_C(0xFFFF));
      }
    }
    else {
      int32_t shift  = 16 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (double)((unpackShort(buf, off) & __INT64_C(0xFFFF)) >> shift);
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for unsigned int16_t data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void packShorts (void *ptr, int32_t off, const double *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(align);
  int32_t delta = fSize / 8; // int8_t offset delta
  int16_t valMask = (sign)? ((int16_t)-1) : ((int16_t)__INT64_C(0xFFFFFFFFFFFFFFFF)); // Bit mask for input value (controls sign-extend)
  if (dSize == 16) {
    if (delta == 2) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=2) {
        int16_t val = (int16_t)(((int16_t)(int64_t)array[offset++]) & valMask);
        packShort(buf, off, val);
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int16_t val = (int16_t)(((int16_t)(int64_t)array[offset++]) & valMask);
        packShort(buf, off, val);
        off += 2;
        _Intel_Pragma("loop_count min=1, max=6")
        for (int32_t j = 2; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
  else {
    int32_t shift = 16 - dSize;
    if (delta == 2) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=2) {
        int16_t val = (int16_t)(((int16_t)(int64_t)array[offset++]) & valMask);
        packShort(buf, off, (int16_t)(val << shift));
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int16_t val = (int16_t)(((int16_t)(int64_t)array[offset++]) & valMask);
        packShort(buf, off, (int16_t)(val << shift));
        off += 2;
        _Intel_Pragma("loop_count min=1, max=6")
        for (int32_t j = 2; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed Int12 data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert (must be a multiple of 2)
 *  @param fSize  (IN)  Item packing field size in bits (must be 12)
 *  @param dSize  (IN)  Data item size in bits (must be 12)
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackInt12s (const void *ptr, int32_t off, double *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  int32_t skip = (align)? 1 : 0;
  if (sign) {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i+=2) {
      int32_t a = (int32_t)buf[off++];
      int32_t b = (int32_t)buf[off++];
      int32_t c = (int32_t)buf[off++];
      int32_t x = (a << 4) | ((b >> 4) & 0x000F);
      int32_t y = (b << 8) | ((c     ) & 0x00FF);
      array[offset++] = (double)((x << 20) >> 20); // sign extend
      array[offset++] = (double)((y << 20) >> 20); // sign extend
      off += skip; // skip extra int8_t in processing-efficient mode
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i+=2) {
      int32_t a = (int32_t)buf[off++];
      int32_t b = (int32_t)buf[off++];
      int32_t c = (int32_t)buf[off++];
      int32_t x = (a << 4) | ((b >> 4) & 0x000F);
      int32_t y = (b << 8) | ((c     ) & 0x00FF);
      array[offset++] = (double)(x & 0x0FFF); // clear high-order bits
      array[offset++] = (double)(y & 0x0FFF); // clear high-order bits
      off += skip; // skip extra int8_t in processing-efficient mode
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for signed Int12 data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert (must be a multiple of 2)
 *  @param fSize  (IN)  Item packing field size in bits (must be 12)
 *  @param dSize  (IN)  Data item size in bits (must be 12)
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packInt12s (void *ptr, int32_t off, const double *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  int32_t skip = (align)?  1 : 0;
  int32_t mask = (sign )? -1 : ((int32_t)__INT64_C(0xFFFFFFFFFFFFFFFF));
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i+=2) {
    int32_t first  = ((int32_t)(int64_t)array[offset++]) & mask;
    int32_t second = ((int32_t)(int64_t)array[offset++]) & mask;
    buf[off++] = (int8_t)(first >> 4);
    buf[off++] = (int8_t)((first << 4) | ((second >> 8) & 0x0F));
    buf[off++] = (int8_t)(second);
    off += skip; // skip extra int8_t in processing-efficient mode
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed int8_t data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void unpackBytes (const void *ptr, int32_t off, double *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  int32_t delta = fSize / 8; // int8_t offset delta
  if (sign) {
    if (dSize == 8) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (double)unpackByte(buf, off);
      }
    }
    else {
      int32_t shift  = 8 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (double)(unpackByte(buf, off) >> shift);
      }
    }
  }
  else {
    if (dSize == 8) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (double)(unpackByte(buf, off) & __INT64_C(0xFF));
      }
    }
    else {
      int32_t shift  = 8 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (double)((unpackByte(buf, off) & __INT64_C(0xFF)) >> shift);
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for unsigned int8_t data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void packBytes (void *ptr, int32_t off, const double *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(align);
  int32_t delta = fSize / 8; // int8_t offset delta
  int8_t valMask = (sign)? ((int8_t)-1) : ((int8_t)__INT64_C(0xFFFFFFFFFFFFFFFF)); // Bit mask for input value (controls sign-extend)
  if (dSize == 8) {
    if (delta == 1) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=1) {
        int8_t val = (int8_t)(((int8_t)(int64_t)array[offset++]) & valMask);
        packByte(buf, off, val);
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int8_t val = (int8_t)(((int8_t)(int64_t)array[offset++]) & valMask);
        packByte(buf, off, val);
        off += 1;
        _Intel_Pragma("loop_count min=1, max=7")
        for (int32_t j = 1; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
  else {
    int32_t shift = 8 - dSize;
    if (delta == 1) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=1) {
        int8_t val = (int8_t)(((int8_t)(int64_t)array[offset++]) & valMask);
        packByte(buf, off, (int8_t)(val << shift));
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int8_t val = (int8_t)(((int8_t)(int64_t)array[offset++]) & valMask);
        packByte(buf, off, (int8_t)(val << shift));
        off += 1;
        _Intel_Pragma("loop_count min=1, max=7")
        for (int32_t j = 1; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed Int4 (nibble) data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert (must be a multiple of 2)
 *  @param fSize  (IN)  Item packing field size in bits (must be 4)
 *  @param dSize  (IN)  Data item size in bits (must be 4)
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackNibbles (const void *ptr, int32_t off, double *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  if (sign) {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i+=2) {
      int32_t b = (int32_t)buf[off++];
      array[offset++] = (double)((b << 24) >> 28); // shift & sign extend
      array[offset++] = (double)((b << 28) >> 28); // shift & sign extend
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i+=2) {
      int32_t b = (int32_t)buf[off++];
      array[offset++] = (double)((b >> 4) & 0xF);
      array[offset++] = (double)((b     ) & 0xF);
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for signed Int4 (nibble) data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert (must be a multiple of 2)
 *  @param fSize  (IN)  Item packing field size in bits (must be 4)
 *  @param dSize  (IN)  Data item size in bits (must be 4)
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packNibbles (void *ptr, int32_t off, const double *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(align);
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  // Pack function for signed and unsigned are identical for nibble data, hence
  // 'sign' is unused here, but included in function definition for consistency.
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i+=2) {
    int32_t first  = (int32_t)(int64_t)array[offset++];
    int32_t second = (int32_t)(int64_t)array[offset++];
    buf[off++] = (int8_t)((first << 4) | (second & 0xF));
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for bit data. Each bit in the
 *  input buffer will be checked, a 0 bit will correspond to the value 0 and a
 *  1 bit will correspond to the value 1.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert (must be a multiple of 8)
 *  @param fSize  (IN)  Item packing field size in bits (must be 1)
 *  @param dSize  (IN)  Data item size in bits (must be 1)
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackBits (const void *ptr, int32_t off, double *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(align);
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  if (sign) {
    // This uses sign-extending principles to get each bit as 0 or -1 which reflects
    // the fact that 0/-1 and not 0/+1 are the available values in a one-bit use of
    // twos-complement.
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i+=8) {
      int32_t b = (int32_t)buf[off++];
      array[offset++] = (double)((b << 24) >> 31);
      array[offset++] = (double)((b << 25) >> 31);
      array[offset++] = (double)((b << 26) >> 31);
      array[offset++] = (double)((b << 27) >> 31);
      array[offset++] = (double)((b << 28) >> 31);
      array[offset++] = (double)((b << 29) >> 31);
      array[offset++] = (double)((b << 30) >> 31);
      array[offset++] = (double)((b << 31) >> 31);
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i+=8) {
      int32_t b = (int32_t)buf[off++];
      array[offset++] = (double)((b >> 7) & 0x1);
      array[offset++] = (double)((b >> 6) & 0x1);
      array[offset++] = (double)((b >> 5) & 0x1);
      array[offset++] = (double)((b >> 4) & 0x1);
      array[offset++] = (double)((b >> 3) & 0x1);
      array[offset++] = (double)((b >> 2) & 0x1);
      array[offset++] = (double)((b >> 1) & 0x1);
      array[offset++] = (double)((b     ) & 0x1);
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for bit data. Each value in the
 *  input array will be checked, a zero value will correspond to the bit 0 and
 *  any non-zero value will correspond to the bit 1.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert (must be a multiple of 8)
 *  @param fSize  (IN)  Item packing field size in bits (must be 1)
 *  @param dSize  (IN)  Data item size in bits (must be 1)
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packBits (void *ptr, int32_t off, const double *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(align);
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  // Pack function for signed and unsigned are identical for bit data, hence
  // 'sign' is unused here, but included in function definition for consistency.
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i+=8) {
    int32_t b = 0;
    b |= (((int64_t)array[offset++]) << 7) & 0x80;
    b |= (((int64_t)array[offset++]) << 6) & 0x40;
    b |= (((int64_t)array[offset++]) << 5) & 0x20;
    b |= (((int64_t)array[offset++]) << 4) & 0x10;
    b |= (((int64_t)array[offset++]) << 3) & 0x08;
    b |= (((int64_t)array[offset++]) << 2) & 0x04;
    b |= (((int64_t)array[offset++]) << 1) & 0x02;
    b |= (((int64_t)array[offset++])     ) & 0x01;
    buf[off++] = (int8_t)b;
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array, where the data is
 *  aligned to 32-bit word boundaries.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Bit offset into buf (must be multiple of 32 bits)
 *  @param array  (OUT) Output value array (not null)
 *  @param chan   (OUT) Output channel tag array (null if n/a)
 *  @param evt    (OUT) Output event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackIntWord32 (const void *ptr, int32_t off, double *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 32 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  int32_t valMask  = (sign)? ((int32_t)-1) : (~(((int32_t)-1) << dSize));
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=4) {
    int32_t value = unpackInt(buf, off);
    array[i] = (double)((value >> (32-dSize)) & valMask);
    if (chan != NULL) chan[i] = (value >> (pSize      )) & chanMask;
    if (evt  != NULL) evt[i]  = (value >> (pSize+cSize)) & evtMask;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array, where the data is
 *  aligned to 32-bit word boundaries.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input value array
 *  @param chan   (IN)  Input channel tag array (null if n/a)
 *  @param evt    (IN)  Input event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packIntWord32 (void *ptr, int32_t off, const double *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 32 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  int32_t valMask  = (sign)? ((int32_t)-1) : ((int32_t)__INT64_C(0xFFFFFFFFFFFFFFFF));
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=4) {
    int32_t val   = (int32_t)(((int32_t)(int64_t)array[i]) & valMask);
    int32_t value = val << (32-dSize);
    if (chan != NULL) value |= (chan[i] & chanMask) << (pSize);
    if (evt  != NULL) value |= (evt[i]  & evtMask ) << (pSize+cSize);
    packInt(buf, off, value);
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array, where the data is
 *  aligned to 64-bit word boundaries.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Bit offset into buf (must be multiple of 64 bits)
 *  @param array  (OUT) Output value array (not null)
 *  @param chan   (OUT) Output channel tag array (null if n/a)
 *  @param evt    (OUT) Output event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackIntWord64 (const void *ptr, int32_t off, double *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 64 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  int64_t valMask  = (sign)? ((int64_t)-1) : (~(((int64_t)-1) << dSize));
  if (dSize == 32) {
    // When using Int32/UInt32 the first 32-bits will always the the Int32/UInt32
    // value, so use 32-bit operations to improve performance
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int32_t val   = unpackInt(buf, off);
      int32_t value = unpackInt(buf, off+4);
      array[i] = (double)(val & valMask);
      if (chan != NULL) chan[i] = ((int32_t)(value >> (pSize      ))) & chanMask;
      if (evt  != NULL) evt[i]  = ((int32_t)(value >> (pSize+cSize))) & evtMask;
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int64_t value = unpackLong(buf, off);
      array[i] = (double)((value >> (64-dSize)) & valMask);
      if (chan != NULL) chan[i] = ((int32_t)(value >> (pSize      ))) & chanMask;
      if (evt  != NULL) evt[i]  = ((int32_t)(value >> (pSize+cSize))) & evtMask;
    }
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array, where the data is
 *  aligned to 64-bit word boundaries.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input value array
 *  @param chan   (IN)  Input channel tag array (null if n/a)
 *  @param evt    (IN)  Input event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packIntWord64 (void *ptr, int32_t off, const double *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 64 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  int64_t valMask  = (sign)? ((int64_t)-1) : ((int64_t)__INT64_C(0xFFFFFFFFFFFFFFFF));
  if (dSize == 32) {
    // When using Int32/UInt32 the first 32-bits will always the the Int32/UInt32
    // value, so use 32-bit operations to improve performance
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int32_t val   = (int32_t)(((int64_t)(int64_t)array[i]) & valMask);
      int32_t value = 0;
      if (chan != NULL) value  = (chan[i] & chanMask) << (pSize);
      if (evt  != NULL) value |= (evt[i]  & evtMask ) << (pSize+cSize);
      packInt(buf, off,   val);
      packInt(buf, off+4, value);
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int64_t val   = (int64_t)(((int64_t)(int64_t)array[i]) & valMask);
      int64_t value = val << (64-dSize);
      if (chan != NULL) value |= ((int64_t)(chan[i] & chanMask)) << (pSize);
      if (evt  != NULL) value |= ((int64_t)(evt[i]  & evtMask )) << (pSize+cSize);
      packLong(buf, off, value);
    }
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array, where the data is
 *  aligned to 64-bit word boundaries.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Bit offset into buf (must be multiple of 64 bits)
 *  @param array  (OUT) Output value array (not null)
 *  @param chan   (OUT) Output channel tag array (null if n/a)
 *  @param evt    (OUT) Output event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackFloatWord64 (const void *ptr, int32_t off, double *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 64 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(dSize);
  // When using Float32 the first 32-bits will always the the Float32 value, so
  // use 32-bit operations to improve performance
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=8) {
    array[i] = (double)unpackFloat(buf, off);
    int32_t value = unpackInt(buf, off+4);
    if (chan != NULL) chan[i] = (value >> (pSize      )) & chanMask;
    if (evt  != NULL) evt[i]  = (value >> (pSize+cSize)) & evtMask;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array, where the data is
 *  aligned to 64-bit word boundaries.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input value array
 *  @param chan   (IN)  Input channel tag array (null if n/a)
 *  @param evt    (IN)  Input event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packFloatWord64 (void *ptr, int32_t off, const double *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 64 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(dSize);
  // When using Float32 the first 32-bits will always the the Float32 value, so
  // use 32-bit operations to improve performance
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=8) {
    float val   = (float)array[i];
    int32_t   value = 0;
    if (chan != NULL) value  = (chan[i] & chanMask) << (pSize);
    if (evt  != NULL) value |= (evt[i]  & evtMask ) << (pSize+cSize);
    packFloat(buf, off,   val);
    packInt(  buf, off+4, value);
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array, where the data is
 *  aligned to 32-bit word boundaries.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Bit offset into buf (must be multiple of 32 bits)
 *  @param array  (OUT) Output value array (not null)
 *  @param chan   (OUT) Output channel tag array (null if n/a)
 *  @param evt    (OUT) Output event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackVRTFloatWord32 (const void *ptr, int32_t off, double *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 32 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=4) {
    int32_t value = unpackInt(buf, off);
    int32_t bits  = (value >> (32-dSize)); // fromVRTFloat32(..) will mask off any extra bits
    array[i] = (double)fromVRTFloat32(sign, expBits, dSize, bits);
    if (chan != NULL) chan[i] = (value >> (pSize      )) & chanMask;
    if (evt  != NULL) evt[i]  = (value >> (pSize+cSize)) & evtMask;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array, where the data is
 *  aligned to 32-bit word boundaries.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input value array
 *  @param chan   (IN)  Input channel tag array (null if n/a)
 *  @param evt    (IN)  Input event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packVRTFloatWord32 (void *ptr, int32_t off, const double *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 32 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=4) {
    int32_t val   = toVRTFloat32(sign, expBits, dSize, (double)array[i]);
    int32_t value = val << (32-dSize);
    if (chan != NULL) value |= (chan[i] & chanMask) << (pSize);
    if (evt  != NULL) value |= (evt[i]  & evtMask ) << (pSize+cSize);
    packInt(buf, off, value);
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array, where the data is
 *  aligned to 64-bit word boundaries.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Bit offset into buf (must be multiple of 64 bits)
 *  @param array  (OUT) Output value array (not null)
 *  @param chan   (OUT) Output channel tag array (null if n/a)
 *  @param evt    (OUT) Output event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackVRTFloatWord64 (const void *ptr, int32_t off, double *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 64 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  if (dSize == 32) {
    // When using Int32/UInt32 the first 32-bits will always the the Int32/UInt32
    // value, so use 32-bit operations to improve performance
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int32_t val   = unpackInt(buf, off);
      int32_t value = unpackInt(buf, off+4);
      array[i] = (double)fromVRTFloat32(sign, expBits, dSize, val);
      if (chan != NULL) chan[i] = ((int32_t)(value >> (pSize      ))) & chanMask;
      if (evt  != NULL) evt[i]  = ((int32_t)(value >> (pSize+cSize))) & evtMask;
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int64_t value = unpackLong(buf, off);
      int64_t bits  = (value >> (64-dSize)); // fromVRTFloat64(..) will mask off any extra bits
      array[i] = (double)fromVRTFloat64(sign, expBits, dSize, bits);
      if (chan != NULL) chan[i] = ((int32_t)(value >> (pSize      ))) & chanMask;
      if (evt  != NULL) evt[i]  = ((int32_t)(value >> (pSize+cSize))) & evtMask;
    }
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array, where the data is
 *  aligned to 64-bit word boundaries.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input value array
 *  @param chan   (IN)  Input channel tag array (null if n/a)
 *  @param evt    (IN)  Input event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packVRTFloatWord64 (void *ptr, int32_t off, const double *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 64 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  if (dSize == 32) {
    // When using Int32/UInt32 the first 32-bits will always the the Int32/UInt32
    // value, so use 32-bit operations to improve performance
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int32_t val   = toVRTFloat32(sign, expBits, dSize, (double)array[i]);
      int32_t value = 0;
      if (chan != NULL) value  = (chan[i] & chanMask) << (pSize);
      if (evt  != NULL) value |= (evt[i]  & evtMask ) << (pSize+cSize);
      packInt(buf, off,   val);
      packInt(buf, off+4, value);
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int64_t val   = toVRTFloat64(sign, expBits, dSize, (double)array[i]);
      int64_t value = val << (64-dSize);
      if (chan != NULL) value |= ((int64_t)(chan[i] & chanMask)) << (pSize);
      if (evt  != NULL) value |= ((int64_t)(evt[i]  & evtMask )) << (pSize+cSize);
      packLong(buf, off, value);
    }
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array.
 *  @param ptr       (IN)  Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf (must be multiple of 32 bits)
 *  @param array     (OUT) Output value array (not null)
 *  @param chan      (OUT) Output channel tag array (null if n/a)
 *  @param evt       (OUT) Output event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackIntBuffer32 (const void *ptr, int32_t bitOffset, double *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  const int32_t bitmask      = 32 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t _bits_dSize  = 32 - dSize;    // pre-compute "32 - dSize" for efficiency
  int32_t _bits_fSize  = 32 - fSize;    // pre-compute "32 - fSize" for efficiency
  int32_t _fSize_dSize = fSize - dSize;         // pre-compute "fSize - dSize" for efficiency
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      // Not enough bits remaining in this word, move to next word (skipping pad bits)
      bitOffset = (bitOffset + 32) & bitmaskInv;
    }
    int32_t bits  = unpackBits32(buf, bitOffset, fSize);
    int32_t value = bits >> _fSize_dSize;
    // sign extend as needed (otherwise leave top bits 0)
    array[i] = (sign)? (double)((value << _bits_dSize) >>  _bits_dSize)
                     : (double)value;
    if (evt  != NULL) evt[i]  = ((int32_t)(bits >> cSize)) & eMask;
    if (chan != NULL) chan[i] = ((int32_t)(bits         )) & cMask;
    bitOffset += fSize;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array.
 *  @param ptr       (I/O) Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf
 *  @param array     (IN)  Input value array
 *  @param chan      (IN)  Input channel tag array (null if n/a)
 *  @param evt       (IN)  Input event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void packIntBuffer32 (void *ptr, int32_t bitOffset, const double *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  const int32_t bitmask      = 32 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t uSize        = fSize - dSize - eSize - cSize;
  int32_t _bits_fSize  = 32 - fSize;    // pre-compute "32 - fSize" for efficiency
  int32_t valMask      = (sign)? ((int32_t)-1) : ((int32_t)__INT64_C(0xFFFFFFFFFFFFFFFF)); // Bit mask for input value (controls sign-extend)
  if ((evt == NULL) || (eSize == 0)) {
    // Treat event tag as unused since we have nothing to insert there (this
    // will be slightly faster in the loop below).
    uSize = uSize + eSize;
    eSize = 0;
    evt   = NULL;
    if (chan == NULL) {
      // Treat channel tag as unused since we have nothing to insert there (this
      // will be slightly faster in the loop below).
      uSize = uSize + cSize;
      cSize = 0;
      chan  = NULL;
    }
  }
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  int32_t _uSize_eSize = uSize + eSize;         // pre-compute "uSize + eSize" for efficiency
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      if (true) { //TODO: make this optional
        // Not enough bits remaining in this word, move to next word (clearing pad bits)
        int32_t zSize = 32 - (bitOffset & bitmask); // bits remaining to set
        packBits32(buf, bitOffset, zSize, 0);
        bitOffset += zSize;
      }
      else {
        // Not enough bits remaining in this word, move to next word (skipping pad bits)
        bitOffset = (bitOffset + 32) & bitmaskInv;
      }
    }
    int32_t value = (int32_t)(((int32_t)(int64_t)array[i]) & valMask);
    value = value << _uSize_eSize;
    if (evt != NULL) value = value | (evt[i] & eMask);
    value = value << cSize;
    if (chan != NULL) value = value | (chan[i] & cMask);
    packBits32(buf, bitOffset, fSize, value);
    bitOffset += fSize;
  }
  // ==== End of Packet Pad Bits ===============================================
  // Clear out any bits remaining in the current word that would be at the end
  // of the packet, this applies to both link and processing efficient.
  if (true) { //TODO: make this optional
    int32_t xSize = 32 - (bitOffset & bitmask); // bits remaining to set
    if (xSize > 0) {
      packBits32(buf, bitOffset, xSize, 0);
      bitOffset += xSize;
    }
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array.
 *  @param ptr       (IN)  Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf (must be multiple of 64 bits)
 *  @param array     (OUT) Output value array (not null)
 *  @param chan      (OUT) Output channel tag array (null if n/a)
 *  @param evt       (OUT) Output event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackIntBuffer64 (const void *ptr, int32_t bitOffset, double *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  const int32_t bitmask      = 64 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t _bits_dSize  = 64 - dSize;    // pre-compute "64 - dSize" for efficiency
  int32_t _bits_fSize  = 64 - fSize;    // pre-compute "64 - fSize" for efficiency
  int32_t _fSize_dSize = fSize - dSize;         // pre-compute "fSize - dSize" for efficiency
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      // Not enough bits remaining in this word, move to next word (skipping pad bits)
      bitOffset = (bitOffset + 32) & bitmaskInv;
    }
    int64_t bits  = unpackBits64(buf, bitOffset, fSize);
    int64_t value = bits >> _fSize_dSize;
    // sign extend as needed (otherwise leave top bits 0)
    array[i] = (sign)? (double)((value << _bits_dSize) >>  _bits_dSize)
                     : (double)value;
    if (evt  != NULL) evt[i]  = ((int32_t)(bits >> cSize)) & eMask;
    if (chan != NULL) chan[i] = ((int32_t)(bits         )) & cMask;
    bitOffset += fSize;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array.
 *  @param ptr       (I/O) Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf
 *  @param array     (IN)  Input value array
 *  @param chan      (IN)  Input channel tag array (null if n/a)
 *  @param evt       (IN)  Input event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void packIntBuffer64 (void *ptr, int32_t bitOffset, const double *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  const int32_t bitmask      = 64 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t uSize        = fSize - dSize - eSize - cSize;
  int32_t _bits_fSize  = 64 - fSize;    // pre-compute "64 - fSize" for efficiency
  int64_t valMask      = (sign)? ((int64_t)-1) : ((int64_t)__INT64_C(0xFFFFFFFFFFFFFFFF)); // Bit mask for input value (controls sign-extend)
  if ((evt == NULL) || (eSize == 0)) {
    // Treat event tag as unused since we have nothing to insert there (this
    // will be slightly faster in the loop below).
    uSize = uSize + eSize;
    eSize = 0;
    evt   = NULL;
    if (chan == NULL) {
      // Treat channel tag as unused since we have nothing to insert there (this
      // will be slightly faster in the loop below).
      uSize = uSize + cSize;
      cSize = 0;
      chan  = NULL;
    }
  }
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  int32_t _uSize_eSize = uSize + eSize;         // pre-compute "uSize + eSize" for efficiency
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      if (true) { //TODO: make this optional
        // Not enough bits remaining in this word, move to next word (clearing pad bits)
        int32_t zSize = 64 - (bitOffset & bitmask); // bits remaining to set
        packBits64(buf, bitOffset, zSize, 0);
        bitOffset += zSize;
      }
      else {
        // Not enough bits remaining in this word, move to next word (skipping pad bits)
        bitOffset = (bitOffset + 32) & bitmaskInv;
      }
    }
    int64_t value = (int64_t)(((int64_t)(int64_t)array[i]) & valMask);
    value = value << _uSize_eSize;
    if (evt != NULL) value = value | (evt[i] & eMask);
    value = value << cSize;
    if (chan != NULL) value = value | (chan[i] & cMask);
    packBits64(buf, bitOffset, fSize, value);
    bitOffset += fSize;
  }
  // ==== End of Packet Pad Bits ===============================================
  // Clear out any bits remaining in the current word that would be at the end
  // of the packet, this applies to both link and processing efficient.
  if (true) { //TODO: make this optional
    int32_t xSize = 64 - (bitOffset & bitmask); // bits remaining to set
    if (xSize > 0) {
      packBits64(buf, bitOffset, xSize, 0);
      bitOffset += xSize;
    }
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array.
 *  @param ptr       (IN)  Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf (must be multiple of 64 bits)
 *  @param array     (OUT) Output value array (not null)
 *  @param chan      (OUT) Output channel tag array (null if n/a)
 *  @param evt       (OUT) Output event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackFloatBuffer64 (const void *ptr, int32_t bitOffset, double *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  const int32_t bitmask      = 64 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t _bits_fSize  = 64 - fSize;    // pre-compute "64 - fSize" for efficiency
  int32_t _fSize_dSize = fSize - dSize;         // pre-compute "fSize - dSize" for efficiency
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      // Not enough bits remaining in this word, move to next word (skipping pad bits)
      bitOffset = (bitOffset + 32) & bitmaskInv;
    }
    int64_t bits  = unpackBits64(buf, bitOffset, fSize);
    int64_t value = bits >> _fSize_dSize;
    UNUSED_VARIABLE(sign);
    array[i] = (double)intBitsToFloat((int32_t)value);
    if (evt  != NULL) evt[i]  = ((int32_t)(bits >> cSize)) & eMask;
    if (chan != NULL) chan[i] = ((int32_t)(bits         )) & cMask;
    bitOffset += fSize;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array.
 *  @param ptr       (I/O) Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf
 *  @param array     (IN)  Input value array
 *  @param chan      (IN)  Input channel tag array (null if n/a)
 *  @param evt       (IN)  Input event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void packFloatBuffer64 (void *ptr, int32_t bitOffset, const double *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  const int32_t bitmask      = 64 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t uSize        = fSize - dSize - eSize - cSize;
  int32_t _bits_fSize  = 64 - fSize;    // pre-compute "64 - fSize" for efficiency
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  if ((evt == NULL) || (eSize == 0)) {
    // Treat event tag as unused since we have nothing to insert there (this
    // will be slightly faster in the loop below).
    uSize = uSize + eSize;
    eSize = 0;
    evt   = NULL;
    if (chan == NULL) {
      // Treat channel tag as unused since we have nothing to insert there (this
      // will be slightly faster in the loop below).
      uSize = uSize + cSize;
      cSize = 0;
      chan  = NULL;
    }
  }
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  int32_t _uSize_eSize = uSize + eSize;         // pre-compute "uSize + eSize" for efficiency
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      if (true) { //TODO: make this optional
        // Not enough bits remaining in this word, move to next word (clearing pad bits)
        int32_t zSize = 64 - (bitOffset & bitmask); // bits remaining to set
        packBits64(buf, bitOffset, zSize, 0);
        bitOffset += zSize;
      }
      else {
        // Not enough bits remaining in this word, move to next word (skipping pad bits)
        bitOffset = (bitOffset + 32) & bitmaskInv;
      }
    }
    int64_t value = (int64_t)floatToRawIntBits((float)array[i]);
    value = value << _uSize_eSize;
    if (evt != NULL) value = value | (evt[i] & eMask);
    value = value << cSize;
    if (chan != NULL) value = value | (chan[i] & cMask);
    packBits64(buf, bitOffset, fSize, value);
    bitOffset += fSize;
  }
  // ==== End of Packet Pad Bits ===============================================
  // Clear out any bits remaining in the current word that would be at the end
  // of the packet, this applies to both link and processing efficient.
  if (true) { //TODO: make this optional
    int32_t xSize = 64 - (bitOffset & bitmask); // bits remaining to set
    if (xSize > 0) {
      packBits64(buf, bitOffset, xSize, 0);
      bitOffset += xSize;
    }
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array.
 *  @param ptr       (IN)  Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf (must be multiple of 32 bits)
 *  @param array     (OUT) Output value array (not null)
 *  @param chan      (OUT) Output channel tag array (null if n/a)
 *  @param evt       (OUT) Output event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackVRTFloatBuffer32 (const void *ptr, int32_t bitOffset, double *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  const char *buf = (const char*)ptr;
  const int32_t bitmask      = 32 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t _bits_fSize  = 32 - fSize;    // pre-compute "32 - fSize" for efficiency
  int32_t _fSize_dSize = fSize - dSize;         // pre-compute "fSize - dSize" for efficiency
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      // Not enough bits remaining in this word, move to next word (skipping pad bits)
      bitOffset = (bitOffset + 32) & bitmaskInv;
    }
    int32_t bits  = unpackBits32(buf, bitOffset, fSize);
    int32_t value = bits >> _fSize_dSize;
    array[i] = (double)fromVRTFloat32(sign, expBits, dSize, value);
    if (evt  != NULL) evt[i]  = ((int32_t)(bits >> cSize)) & eMask;
    if (chan != NULL) chan[i] = ((int32_t)(bits         )) & cMask;
    bitOffset += fSize;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array.
 *  @param ptr       (I/O) Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf
 *  @param array     (IN)  Input value array
 *  @param chan      (IN)  Input channel tag array (null if n/a)
 *  @param evt       (IN)  Input event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void packVRTFloatBuffer32 (void *ptr, int32_t bitOffset, const double *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  char *buf = (char*)ptr;
  const int32_t bitmask      = 32 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t uSize        = fSize - dSize - eSize - cSize;
  int32_t _bits_fSize  = 32 - fSize;    // pre-compute "32 - fSize" for efficiency
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  if ((evt == NULL) || (eSize == 0)) {
    // Treat event tag as unused since we have nothing to insert there (this
    // will be slightly faster in the loop below).
    uSize = uSize + eSize;
    eSize = 0;
    evt   = NULL;
    if (chan == NULL) {
      // Treat channel tag as unused since we have nothing to insert there (this
      // will be slightly faster in the loop below).
      uSize = uSize + cSize;
      cSize = 0;
      chan  = NULL;
    }
  }
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  int32_t _uSize_eSize = uSize + eSize;         // pre-compute "uSize + eSize" for efficiency
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      if (true) { //TODO: make this optional
        // Not enough bits remaining in this word, move to next word (clearing pad bits)
        int32_t zSize = 32 - (bitOffset & bitmask); // bits remaining to set
        packBits32(buf, bitOffset, zSize, 0);
        bitOffset += zSize;
      }
      else {
        // Not enough bits remaining in this word, move to next word (skipping pad bits)
        bitOffset = (bitOffset + 32) & bitmaskInv;
      }
    }
    int32_t value = toVRTFloat32(sign, expBits, dSize, (double)array[i]);
    value = value << _uSize_eSize;
    if (evt != NULL) value = value | (evt[i] & eMask);
    value = value << cSize;
    if (chan != NULL) value = value | (chan[i] & cMask);
    packBits32(buf, bitOffset, fSize, value);
    bitOffset += fSize;
  }
  // ==== End of Packet Pad Bits ===============================================
  // Clear out any bits remaining in the current word that would be at the end
  // of the packet, this applies to both link and processing efficient.
  if (true) { //TODO: make this optional
    int32_t xSize = 32 - (bitOffset & bitmask); // bits remaining to set
    if (xSize > 0) {
      packBits32(buf, bitOffset, xSize, 0);
      bitOffset += xSize;
    }
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array.
 *  @param ptr       (IN)  Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf (must be multiple of 64 bits)
 *  @param array     (OUT) Output value array (not null)
 *  @param chan      (OUT) Output channel tag array (null if n/a)
 *  @param evt       (OUT) Output event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackVRTFloatBuffer64 (const void *ptr, int32_t bitOffset, double *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  const char *buf = (const char*)ptr;
  const int32_t bitmask      = 64 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t _bits_fSize  = 64 - fSize;    // pre-compute "64 - fSize" for efficiency
  int32_t _fSize_dSize = fSize - dSize;         // pre-compute "fSize - dSize" for efficiency
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      // Not enough bits remaining in this word, move to next word (skipping pad bits)
      bitOffset = (bitOffset + 32) & bitmaskInv;
    }
    int64_t bits  = unpackBits64(buf, bitOffset, fSize);
    int64_t value = bits >> _fSize_dSize;
    array[i] = (double)fromVRTFloat64(sign, expBits, dSize, value);
    if (evt  != NULL) evt[i]  = ((int32_t)(bits >> cSize)) & eMask;
    if (chan != NULL) chan[i] = ((int32_t)(bits         )) & cMask;
    bitOffset += fSize;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array.
 *  @param ptr       (I/O) Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf
 *  @param array     (IN)  Input value array
 *  @param chan      (IN)  Input channel tag array (null if n/a)
 *  @param evt       (IN)  Input event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void packVRTFloatBuffer64 (void *ptr, int32_t bitOffset, const double *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  char *buf = (char*)ptr;
  const int32_t bitmask      = 64 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t uSize        = fSize - dSize - eSize - cSize;
  int32_t _bits_fSize  = 64 - fSize;    // pre-compute "64 - fSize" for efficiency
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  if ((evt == NULL) || (eSize == 0)) {
    // Treat event tag as unused since we have nothing to insert there (this
    // will be slightly faster in the loop below).
    uSize = uSize + eSize;
    eSize = 0;
    evt   = NULL;
    if (chan == NULL) {
      // Treat channel tag as unused since we have nothing to insert there (this
      // will be slightly faster in the loop below).
      uSize = uSize + cSize;
      cSize = 0;
      chan  = NULL;
    }
  }
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  int32_t _uSize_eSize = uSize + eSize;         // pre-compute "uSize + eSize" for efficiency
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      if (true) { //TODO: make this optional
        // Not enough bits remaining in this word, move to next word (clearing pad bits)
        int32_t zSize = 64 - (bitOffset & bitmask); // bits remaining to set
        packBits64(buf, bitOffset, zSize, 0);
        bitOffset += zSize;
      }
      else {
        // Not enough bits remaining in this word, move to next word (skipping pad bits)
        bitOffset = (bitOffset + 32) & bitmaskInv;
      }
    }
    int64_t value = toVRTFloat64(sign, expBits, dSize, (double)array[i]);
    value = value << _uSize_eSize;
    if (evt != NULL) value = value | (evt[i] & eMask);
    value = value << cSize;
    if (chan != NULL) value = value | (chan[i] & cMask);
    packBits64(buf, bitOffset, fSize, value);
    bitOffset += fSize;
  }
  // ==== End of Packet Pad Bits ===============================================
  // Clear out any bits remaining in the current word that would be at the end
  // of the packet, this applies to both link and processing efficient.
  if (true) { //TODO: make this optional
    int32_t xSize = 64 - (bitOffset & bitmask); // bits remaining to set
    if (xSize > 0) {
      packBits64(buf, bitOffset, xSize, 0);
      bitOffset += xSize;
    }
  }
}
#if NOT_USING_JNI
double* vrt::PackUnpack::unpackAsDouble (PayloadFormat pf, const void *ptr, int32_t off, double *array, int32_t *chan, int32_t *evt, int32_t length) {
  const char *buf = (const char*)ptr;
  if (!pf.isValid()) {
    throw VRTException(pf.getValid());
  }
  int32_t            fSize  = pf.getItemPackingFieldSize();
  int32_t            dSize  = pf.getDataItemSize();
  int32_t            eSize  = pf.getEventTagSize();
  int32_t            cSize  = pf.getChannelTagSize();
  int32_t            xSize  = fSize; // effective fSize
  bool        proc   = pf.isProcessingEfficient();
  bool        sign   = pf.isSigned();
  DataItemFormat format = pf.getDataItemFormat();
  bool        noTags;
  if (eSize == 0) evt  = NULL; // no event tags, ignore any array passed in
  if (cSize == 0) chan = NULL; // no channel tags, ignore any array passed in
  noTags = ((chan == NULL) && (evt == NULL)) || (fSize == dSize);
  if (proc) {
         if (fSize >= 33) xSize = 64;
    else if (fSize >= 17) xSize = 32;
    if (noTags) fSize = xSize; // Use faster field size if ignoring Event & Channel Tags
  }
  switch (format) {
    case DataItemFormat_SignedInt:   // FALLTHROUGH
    case DataItemFormat_UnsignedInt:
      // ---- Try fast methods -------------------------------------------------
      if (noTags) {
        switch (fSize) {
          case  8: unpackBytes(  buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
          case 16: unpackShorts( buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
          case 24: unpackInt24s( buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
          case 32: unpackInts(   buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
          case 64: unpackLongs(  buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
        }
      }
      if (fSize == dSize) { // implicit noTags=true in this case
        switch (fSize) {
          case  1: if ((length % 8) == 0) { unpackBits(   buf, off, array, 0, length, fSize, dSize, proc, sign); return array; } break;
          case  4: if ((length % 2) == 0) { unpackNibbles(buf, off, array, 0, length, fSize, dSize, proc, sign); return array; } break;
          case 12: if ((length % 2) == 0) { unpackInt12s( buf, off, array, 0, length, fSize, dSize, proc, sign); return array; } break;
        }
      }
      if (xSize == 32) {
        unpackIntWord32(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return array;
      }
      if (xSize == 64) {
        unpackIntWord64(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return array;
      }
      // ---- Fall back to slow method -----------------------------------------
      if (fSize <= 32) {
        unpackIntBuffer32(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return array;
      }
      else {
        unpackIntBuffer64(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return array;
      }
    case DataItemFormat_Float:
      // ---- Try fast methods -------------------------------------------------
      if (fSize == dSize) { // implicit noTags=true in this case
        unpackFloats(buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
      }
      if (xSize == 64) {
        unpackFloatWord64(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return array;
      }
      // ---- Fall back to slow method -----------------------------------------
      unpackFloatBuffer64(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return array;
    case DataItemFormat_Double:
      // ---- Only fast method for double (yea!) -------------------------------
      unpackDoubles(buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
    default: // VRTFloat Types
      // ---- Try fast methods -------------------------------------------------
      if (xSize == 32) {
        unpackVRTFloatWord32(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return array;
      }
      if (xSize == 64) {
        unpackVRTFloatWord64(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return array;
      }
      // ---- Fall back to slow method -----------------------------------------
      if (fSize <= 32) {
        unpackVRTFloatBuffer32(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return array;
      }
      else {
        unpackVRTFloatBuffer64(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return array;
      }
  }
}
void vrt::PackUnpack::packAsDouble (PayloadFormat pf, void *ptr, int32_t off, const double *array, const int32_t *chan, const int32_t *evt, int32_t length) {
  char *buf = (char*)ptr;
  if (!pf.isValid()) {
    throw VRTException(pf.getValid());
  }
  int32_t            fSize  = pf.getItemPackingFieldSize();
  int32_t            dSize  = pf.getDataItemSize();
  int32_t            eSize  = pf.getEventTagSize();
  int32_t            cSize  = pf.getChannelTagSize();
  int32_t            xSize  = fSize; // effective fSize
  bool        proc   = pf.isProcessingEfficient();
  bool        sign   = pf.isSigned();
  DataItemFormat format = pf.getDataItemFormat();
  bool        noTags;
  if (eSize == 0) evt  = NULL; // no event tags, ignore any array passed in
  if (cSize == 0) chan = NULL; // no channel tags, ignore any array passed in
  noTags = ((chan == NULL) && (evt == NULL)) || (fSize == dSize);
  if (proc) {
         if (fSize >= 33) xSize = 64;
    else if (fSize >= 17) xSize = 32;
    if (noTags) fSize = xSize; // Use faster field size if ignoring Event & Channel Tags
  }
  switch (format) {
    case DataItemFormat_SignedInt:   // FALLTHROUGH
    case DataItemFormat_UnsignedInt:
      // ---- Try fast methods -------------------------------------------------
      if (noTags) {
        switch (fSize) {
          case  8: packBytes(  buf, off, array, 0, length, fSize, dSize, proc, sign); return;
          case 16: packShorts( buf, off, array, 0, length, fSize, dSize, proc, sign); return;
          case 24: packInt24s( buf, off, array, 0, length, fSize, dSize, proc, sign); return;
          case 32: packInts(   buf, off, array, 0, length, fSize, dSize, proc, sign); return;
          case 64: packLongs(  buf, off, array, 0, length, fSize, dSize, proc, sign); return;
        }
      }
      if (fSize == dSize) { // implicit noTags=true in this case
        switch (fSize) {
          case  1: if ((length % 8) == 0) { packBits(   buf, off, array, 0, length, fSize, dSize, proc, sign); return; } break;
          case  4: if ((length % 2) == 0) { packNibbles(buf, off, array, 0, length, fSize, dSize, proc, sign); return; } break;
          case 12: if ((length % 2) == 0) { packInt12s( buf, off, array, 0, length, fSize, dSize, proc, sign); return; } break;
        }
      }
      if (xSize == 32) {
        packIntWord32(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return;
      }
      if (xSize == 64) {
        packIntWord64(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return;
      }
      // ---- Fall back to slow method -----------------------------------------
      if (fSize <= 32) {
        packIntBuffer32(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return;
      }
      else {
        packIntBuffer64(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return;
      }
    case DataItemFormat_Float:
      // ---- Try fast methods -------------------------------------------------
      if (fSize == dSize) { // implicit noTags=true in this case
        packFloats(buf, off, array, 0, length, fSize, dSize, proc, sign); return;
      }
      if (xSize == 64) {
        packFloatWord64(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return;
      }
      // ---- Fall back to slow method -----------------------------------------
      packFloatBuffer64(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return;
    case DataItemFormat_Double:
      // ---- Only fast method for double (yea!) -------------------------------
      packDoubles(buf, off, array, 0, length, fSize, dSize, proc, sign); return;
    default: // VRTFloat Types
      // ---- Try fast methods -------------------------------------------------
      if (xSize == 32) {
        packVRTFloatWord32(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return;
      }
      if (xSize == 64) {
        packVRTFloatWord64(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return;
      }
      // ---- Fall back to slow method -----------------------------------------
      if (fSize <= 32) {
        packVRTFloatBuffer32(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return;
      }
      else {
        packVRTFloatBuffer64(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return;
      }
  }
}
#endif /* NOT_USING_JNI */
/** <b>Internal Use Only:</b> Fast unpack method for double data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits (must be 64)
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed? (must be true)
 */
static __attribute__((flatten)) void unpackDoubles (const void *ptr, int32_t off, float *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  // All floating-point values are signed, so 'sign' is unused, but is included
  // in function definition for consistency.
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(align);
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  // When using Double data, it is required that dSize=64 and fSize=64 so we can
  // skip some of the normal checks we would otherwise do here.
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=8) {
    array[offset++] = (float)unpackDouble(buf, off);
  }
}
/** <b>Internal Use Only:</b>
 *  Fast pack method for double data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits (must be 64)
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed? (must be true)
 */
static __attribute__((flatten)) void packDoubles (void *ptr, int32_t off, const float *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  // All floating-point values are signed, so 'sign' is unused, but is included
  // in function definition for consistency.
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(align);
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  // When using Double data, it is required that dSize=64 and fSize=64 so we can
  // skip some of the normal checks we would otherwise do here.
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=8) {
    packDouble(buf, off, (double)array[offset++]);
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for float data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits (must be 32)
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed? (must be true)
 */
static __attribute__((flatten)) void unpackFloats (const void *ptr, int32_t off, float *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  // All floating-point values are signed, so 'sign' is unused, but is included
  // in function definition for consistency.
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(dSize);
  int32_t delta; // int8_t offset delta
  if (align) delta = (fSize <= 32)? 4 : 8;
  else       delta = fSize / 8;
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=delta) {
    array[offset++] = (float)unpackFloat(buf, off);
  }
}
/** <b>Internal Use Only:</b>
 *  Fast pack method for float data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits (must be 32)
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed? (must be true)
 */
static __attribute__((flatten)) void packFloats (void *ptr, int32_t off, const float *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  // All floating-point values are signed, so 'sign' is unused, but is included
  // in function definition for consistency.
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(dSize);
  int32_t delta; // int8_t offset delta
  if (align) delta = (fSize <= 32)? 4 : 8;
  else       delta = fSize / 8;
  if (delta == 4) {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=4) {
      packFloat(buf, off, (float)array[offset++]);
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++) {
      packFloat(buf, off, (float)array[offset++]);
      off += 4;
      _Intel_Pragma("loop_count min=1, max=4")
      for (int32_t j = 4; j < delta; j++) {
        buf[off++] = 0; // set unused bits to zero
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed int64_t data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void unpackLongs (const void *ptr, int32_t off, float *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  int32_t delta = (align)? ((fSize <= 32)? 4 : 8) : (fSize/8); // int8_t offset delta
  if (sign) {
    if (dSize == 64) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (float)unpackLong(buf, off);
      }
    }
    else {
      int32_t shift  = 64 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (float)(unpackLong(buf, off) >> shift);
      }
    }
  }
  else {
    if (dSize == 64) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (float)(unpackLong(buf, off) & __INT64_C(0xFFFFFFFFFFFFFFFF));
      }
    }
    else {
      int32_t shift  = 64 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (float)((unpackLong(buf, off) & __INT64_C(0xFFFFFFFFFFFFFFFF)) >> shift);
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for unsigned int64_t data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void packLongs (void *ptr, int32_t off, const float *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  int32_t delta = (align)? ((fSize <= 32)? 4 : 8) : (fSize/8); // int8_t offset delta
  int64_t valMask = (sign)? ((int64_t)-1) : ((int64_t)__INT64_C(0xFFFFFFFF)); // Bit mask for input value (controls sign-extend)
  if (dSize == 64) {
    if (delta == 8) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=8) {
        int64_t val = (int64_t)(((int64_t)(int64_t)array[offset++]) & valMask);
        packLong(buf, off, val);
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int64_t val = (int64_t)(((int64_t)(int64_t)array[offset++]) & valMask);
        packLong(buf, off, val);
        off += 8;
        _Intel_Pragma("loop_count min=1, max=0")
        for (int32_t j = 8; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
  else {
    int32_t shift = 64 - dSize;
    if (delta == 8) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=8) {
        int64_t val = (int64_t)(((int64_t)(int64_t)array[offset++]) & valMask);
        packLong(buf, off, (int64_t)(val << shift));
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int64_t val = (int64_t)(((int64_t)(int64_t)array[offset++]) & valMask);
        packLong(buf, off, (int64_t)(val << shift));
        off += 8;
        _Intel_Pragma("loop_count min=1, max=0")
        for (int32_t j = 8; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed int32_t data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void unpackInts (const void *ptr, int32_t off, float *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  int32_t delta = (align)? ((fSize <= 32)? 4 : 8) : (fSize/8); // int8_t offset delta
  if (sign) {
    if (dSize == 32) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (float)unpackInt(buf, off);
      }
    }
    else {
      int32_t shift  = 32 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (float)(unpackInt(buf, off) >> shift);
      }
    }
  }
  else {
    if (dSize == 32) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (float)(unpackInt(buf, off) & __INT64_C(0xFFFFFFFF));
      }
    }
    else {
      int32_t shift  = 32 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (float)((unpackInt(buf, off) & __INT64_C(0xFFFFFFFF)) >> shift);
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for unsigned int32_t data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void packInts (void *ptr, int32_t off, const float *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  int32_t delta = (align)? ((fSize <= 32)? 4 : 8) : (fSize/8); // int8_t offset delta
  int32_t valMask = (sign)? ((int32_t)-1) : ((int32_t)__INT64_C(0xFFFFFFFF)); // Bit mask for input value (controls sign-extend)
  if (dSize == 32) {
    if (delta == 4) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=4) {
        int32_t val = (int32_t)(((int32_t)(int64_t)array[offset++]) & valMask);
        packInt(buf, off, val);
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int32_t val = (int32_t)(((int32_t)(int64_t)array[offset++]) & valMask);
        packInt(buf, off, val);
        off += 4;
        _Intel_Pragma("loop_count min=1, max=4")
        for (int32_t j = 4; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
  else {
    int32_t shift = 32 - dSize;
    if (delta == 4) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=4) {
        int32_t val = (int32_t)(((int32_t)(int64_t)array[offset++]) & valMask);
        packInt(buf, off, (int32_t)(val << shift));
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int32_t val = (int32_t)(((int32_t)(int64_t)array[offset++]) & valMask);
        packInt(buf, off, (int32_t)(val << shift));
        off += 4;
        _Intel_Pragma("loop_count min=1, max=4")
        for (int32_t j = 4; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed int32_t data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void unpackInt24s (const void *ptr, int32_t off, float *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  int32_t delta = (align)? ((fSize <= 32)? 4 : 8) : (fSize/8); // int8_t offset delta
  if (sign) {
    if (dSize == 24) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (float)unpackInt24(buf, off);
      }
    }
    else {
      int32_t shift  = 24 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (float)(unpackInt24(buf, off) >> shift);
      }
    }
  }
  else {
    if (dSize == 24) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (float)(unpackInt24(buf, off) & __INT64_C(0xFFFFFF));
      }
    }
    else {
      int32_t shift  = 24 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (float)((unpackInt24(buf, off) & __INT64_C(0xFFFFFF)) >> shift);
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for unsigned int32_t data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void packInt24s (void *ptr, int32_t off, const float *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  int32_t delta = (align)? ((fSize <= 32)? 4 : 8) : (fSize/8); // int8_t offset delta
  int32_t valMask = (sign)? ((int32_t)-1) : ((int32_t)__INT64_C(0xFFFFFFFF)); // Bit mask for input value (controls sign-extend)
  if (dSize == 24) {
    if (delta == 3) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=3) {
        int32_t val = (int32_t)(((int32_t)(int64_t)array[offset++]) & valMask);
        packInt24(buf, off, val);
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int32_t val = (int32_t)(((int32_t)(int64_t)array[offset++]) & valMask);
        packInt24(buf, off, val);
        off += 3;
        _Intel_Pragma("loop_count min=1, max=5")
        for (int32_t j = 3; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
  else {
    int32_t shift = 24 - dSize;
    if (delta == 3) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=3) {
        int32_t val = (int32_t)(((int32_t)(int64_t)array[offset++]) & valMask);
        packInt24(buf, off, (int32_t)(val << shift));
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int32_t val = (int32_t)(((int32_t)(int64_t)array[offset++]) & valMask);
        packInt24(buf, off, (int32_t)(val << shift));
        off += 3;
        _Intel_Pragma("loop_count min=1, max=5")
        for (int32_t j = 3; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed int16_t data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void unpackShorts (const void *ptr, int32_t off, float *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  int32_t delta = fSize / 8; // int8_t offset delta
  if (sign) {
    if (dSize == 16) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (float)unpackShort(buf, off);
      }
    }
    else {
      int32_t shift  = 16 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (float)(unpackShort(buf, off) >> shift);
      }
    }
  }
  else {
    if (dSize == 16) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (float)(unpackShort(buf, off) & __INT64_C(0xFFFF));
      }
    }
    else {
      int32_t shift  = 16 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (float)((unpackShort(buf, off) & __INT64_C(0xFFFF)) >> shift);
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for unsigned int16_t data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void packShorts (void *ptr, int32_t off, const float *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(align);
  int32_t delta = fSize / 8; // int8_t offset delta
  int16_t valMask = (sign)? ((int16_t)-1) : ((int16_t)__INT64_C(0xFFFFFFFF)); // Bit mask for input value (controls sign-extend)
  if (dSize == 16) {
    if (delta == 2) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=2) {
        int16_t val = (int16_t)(((int16_t)(int64_t)array[offset++]) & valMask);
        packShort(buf, off, val);
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int16_t val = (int16_t)(((int16_t)(int64_t)array[offset++]) & valMask);
        packShort(buf, off, val);
        off += 2;
        _Intel_Pragma("loop_count min=1, max=6")
        for (int32_t j = 2; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
  else {
    int32_t shift = 16 - dSize;
    if (delta == 2) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=2) {
        int16_t val = (int16_t)(((int16_t)(int64_t)array[offset++]) & valMask);
        packShort(buf, off, (int16_t)(val << shift));
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int16_t val = (int16_t)(((int16_t)(int64_t)array[offset++]) & valMask);
        packShort(buf, off, (int16_t)(val << shift));
        off += 2;
        _Intel_Pragma("loop_count min=1, max=6")
        for (int32_t j = 2; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed Int12 data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert (must be a multiple of 2)
 *  @param fSize  (IN)  Item packing field size in bits (must be 12)
 *  @param dSize  (IN)  Data item size in bits (must be 12)
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackInt12s (const void *ptr, int32_t off, float *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  int32_t skip = (align)? 1 : 0;
  if (sign) {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i+=2) {
      int32_t a = (int32_t)buf[off++];
      int32_t b = (int32_t)buf[off++];
      int32_t c = (int32_t)buf[off++];
      int32_t x = (a << 4) | ((b >> 4) & 0x000F);
      int32_t y = (b << 8) | ((c     ) & 0x00FF);
      array[offset++] = (float)((x << 20) >> 20); // sign extend
      array[offset++] = (float)((y << 20) >> 20); // sign extend
      off += skip; // skip extra int8_t in processing-efficient mode
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i+=2) {
      int32_t a = (int32_t)buf[off++];
      int32_t b = (int32_t)buf[off++];
      int32_t c = (int32_t)buf[off++];
      int32_t x = (a << 4) | ((b >> 4) & 0x000F);
      int32_t y = (b << 8) | ((c     ) & 0x00FF);
      array[offset++] = (float)(x & 0x0FFF); // clear high-order bits
      array[offset++] = (float)(y & 0x0FFF); // clear high-order bits
      off += skip; // skip extra int8_t in processing-efficient mode
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for signed Int12 data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert (must be a multiple of 2)
 *  @param fSize  (IN)  Item packing field size in bits (must be 12)
 *  @param dSize  (IN)  Data item size in bits (must be 12)
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packInt12s (void *ptr, int32_t off, const float *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  int32_t skip = (align)?  1 : 0;
  int32_t mask = (sign )? -1 : ((int32_t)__INT64_C(0xFFFFFFFF));
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i+=2) {
    int32_t first  = ((int32_t)(int64_t)array[offset++]) & mask;
    int32_t second = ((int32_t)(int64_t)array[offset++]) & mask;
    buf[off++] = (int8_t)(first >> 4);
    buf[off++] = (int8_t)((first << 4) | ((second >> 8) & 0x0F));
    buf[off++] = (int8_t)(second);
    off += skip; // skip extra int8_t in processing-efficient mode
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed int8_t data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void unpackBytes (const void *ptr, int32_t off, float *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  int32_t delta = fSize / 8; // int8_t offset delta
  if (sign) {
    if (dSize == 8) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (float)unpackByte(buf, off);
      }
    }
    else {
      int32_t shift  = 8 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (float)(unpackByte(buf, off) >> shift);
      }
    }
  }
  else {
    if (dSize == 8) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (float)(unpackByte(buf, off) & __INT64_C(0xFF));
      }
    }
    else {
      int32_t shift  = 8 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (float)((unpackByte(buf, off) & __INT64_C(0xFF)) >> shift);
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for unsigned int8_t data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void packBytes (void *ptr, int32_t off, const float *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(align);
  int32_t delta = fSize / 8; // int8_t offset delta
  int8_t valMask = (sign)? ((int8_t)-1) : ((int8_t)__INT64_C(0xFFFFFFFF)); // Bit mask for input value (controls sign-extend)
  if (dSize == 8) {
    if (delta == 1) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=1) {
        int8_t val = (int8_t)(((int8_t)(int64_t)array[offset++]) & valMask);
        packByte(buf, off, val);
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int8_t val = (int8_t)(((int8_t)(int64_t)array[offset++]) & valMask);
        packByte(buf, off, val);
        off += 1;
        _Intel_Pragma("loop_count min=1, max=7")
        for (int32_t j = 1; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
  else {
    int32_t shift = 8 - dSize;
    if (delta == 1) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=1) {
        int8_t val = (int8_t)(((int8_t)(int64_t)array[offset++]) & valMask);
        packByte(buf, off, (int8_t)(val << shift));
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int8_t val = (int8_t)(((int8_t)(int64_t)array[offset++]) & valMask);
        packByte(buf, off, (int8_t)(val << shift));
        off += 1;
        _Intel_Pragma("loop_count min=1, max=7")
        for (int32_t j = 1; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed Int4 (nibble) data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert (must be a multiple of 2)
 *  @param fSize  (IN)  Item packing field size in bits (must be 4)
 *  @param dSize  (IN)  Data item size in bits (must be 4)
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackNibbles (const void *ptr, int32_t off, float *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  if (sign) {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i+=2) {
      int32_t b = (int32_t)buf[off++];
      array[offset++] = (float)((b << 24) >> 28); // shift & sign extend
      array[offset++] = (float)((b << 28) >> 28); // shift & sign extend
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i+=2) {
      int32_t b = (int32_t)buf[off++];
      array[offset++] = (float)((b >> 4) & 0xF);
      array[offset++] = (float)((b     ) & 0xF);
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for signed Int4 (nibble) data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert (must be a multiple of 2)
 *  @param fSize  (IN)  Item packing field size in bits (must be 4)
 *  @param dSize  (IN)  Data item size in bits (must be 4)
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packNibbles (void *ptr, int32_t off, const float *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(align);
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  // Pack function for signed and unsigned are identical for nibble data, hence
  // 'sign' is unused here, but included in function definition for consistency.
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i+=2) {
    int32_t first  = (int32_t)(int64_t)array[offset++];
    int32_t second = (int32_t)(int64_t)array[offset++];
    buf[off++] = (int8_t)((first << 4) | (second & 0xF));
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for bit data. Each bit in the
 *  input buffer will be checked, a 0 bit will correspond to the value 0 and a
 *  1 bit will correspond to the value 1.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert (must be a multiple of 8)
 *  @param fSize  (IN)  Item packing field size in bits (must be 1)
 *  @param dSize  (IN)  Data item size in bits (must be 1)
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackBits (const void *ptr, int32_t off, float *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(align);
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  if (sign) {
    // This uses sign-extending principles to get each bit as 0 or -1 which reflects
    // the fact that 0/-1 and not 0/+1 are the available values in a one-bit use of
    // twos-complement.
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i+=8) {
      int32_t b = (int32_t)buf[off++];
      array[offset++] = (float)((b << 24) >> 31);
      array[offset++] = (float)((b << 25) >> 31);
      array[offset++] = (float)((b << 26) >> 31);
      array[offset++] = (float)((b << 27) >> 31);
      array[offset++] = (float)((b << 28) >> 31);
      array[offset++] = (float)((b << 29) >> 31);
      array[offset++] = (float)((b << 30) >> 31);
      array[offset++] = (float)((b << 31) >> 31);
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i+=8) {
      int32_t b = (int32_t)buf[off++];
      array[offset++] = (float)((b >> 7) & 0x1);
      array[offset++] = (float)((b >> 6) & 0x1);
      array[offset++] = (float)((b >> 5) & 0x1);
      array[offset++] = (float)((b >> 4) & 0x1);
      array[offset++] = (float)((b >> 3) & 0x1);
      array[offset++] = (float)((b >> 2) & 0x1);
      array[offset++] = (float)((b >> 1) & 0x1);
      array[offset++] = (float)((b     ) & 0x1);
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for bit data. Each value in the
 *  input array will be checked, a zero value will correspond to the bit 0 and
 *  any non-zero value will correspond to the bit 1.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert (must be a multiple of 8)
 *  @param fSize  (IN)  Item packing field size in bits (must be 1)
 *  @param dSize  (IN)  Data item size in bits (must be 1)
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packBits (void *ptr, int32_t off, const float *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(align);
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  // Pack function for signed and unsigned are identical for bit data, hence
  // 'sign' is unused here, but included in function definition for consistency.
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i+=8) {
    int32_t b = 0;
    b |= (((int64_t)array[offset++]) << 7) & 0x80;
    b |= (((int64_t)array[offset++]) << 6) & 0x40;
    b |= (((int64_t)array[offset++]) << 5) & 0x20;
    b |= (((int64_t)array[offset++]) << 4) & 0x10;
    b |= (((int64_t)array[offset++]) << 3) & 0x08;
    b |= (((int64_t)array[offset++]) << 2) & 0x04;
    b |= (((int64_t)array[offset++]) << 1) & 0x02;
    b |= (((int64_t)array[offset++])     ) & 0x01;
    buf[off++] = (int8_t)b;
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array, where the data is
 *  aligned to 32-bit word boundaries.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Bit offset into buf (must be multiple of 32 bits)
 *  @param array  (OUT) Output value array (not null)
 *  @param chan   (OUT) Output channel tag array (null if n/a)
 *  @param evt    (OUT) Output event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackIntWord32 (const void *ptr, int32_t off, float *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 32 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  int32_t valMask  = (sign)? ((int32_t)-1) : (~(((int32_t)-1) << dSize));
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=4) {
    int32_t value = unpackInt(buf, off);
    array[i] = (float)((value >> (32-dSize)) & valMask);
    if (chan != NULL) chan[i] = (value >> (pSize      )) & chanMask;
    if (evt  != NULL) evt[i]  = (value >> (pSize+cSize)) & evtMask;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array, where the data is
 *  aligned to 32-bit word boundaries.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input value array
 *  @param chan   (IN)  Input channel tag array (null if n/a)
 *  @param evt    (IN)  Input event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packIntWord32 (void *ptr, int32_t off, const float *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 32 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  int32_t valMask  = (sign)? ((int32_t)-1) : ((int32_t)__INT64_C(0xFFFFFFFF));
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=4) {
    int32_t val   = (int32_t)(((int32_t)(int64_t)array[i]) & valMask);
    int32_t value = val << (32-dSize);
    if (chan != NULL) value |= (chan[i] & chanMask) << (pSize);
    if (evt  != NULL) value |= (evt[i]  & evtMask ) << (pSize+cSize);
    packInt(buf, off, value);
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array, where the data is
 *  aligned to 64-bit word boundaries.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Bit offset into buf (must be multiple of 64 bits)
 *  @param array  (OUT) Output value array (not null)
 *  @param chan   (OUT) Output channel tag array (null if n/a)
 *  @param evt    (OUT) Output event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackIntWord64 (const void *ptr, int32_t off, float *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 64 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  int64_t valMask  = (sign)? ((int64_t)-1) : (~(((int64_t)-1) << dSize));
  if (dSize == 32) {
    // When using Int32/UInt32 the first 32-bits will always the the Int32/UInt32
    // value, so use 32-bit operations to improve performance
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int32_t val   = unpackInt(buf, off);
      int32_t value = unpackInt(buf, off+4);
      array[i] = (float)(val & valMask);
      if (chan != NULL) chan[i] = ((int32_t)(value >> (pSize      ))) & chanMask;
      if (evt  != NULL) evt[i]  = ((int32_t)(value >> (pSize+cSize))) & evtMask;
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int64_t value = unpackLong(buf, off);
      array[i] = (float)((value >> (64-dSize)) & valMask);
      if (chan != NULL) chan[i] = ((int32_t)(value >> (pSize      ))) & chanMask;
      if (evt  != NULL) evt[i]  = ((int32_t)(value >> (pSize+cSize))) & evtMask;
    }
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array, where the data is
 *  aligned to 64-bit word boundaries.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input value array
 *  @param chan   (IN)  Input channel tag array (null if n/a)
 *  @param evt    (IN)  Input event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packIntWord64 (void *ptr, int32_t off, const float *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 64 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  int64_t valMask  = (sign)? ((int64_t)-1) : ((int64_t)__INT64_C(0xFFFFFFFF));
  if (dSize == 32) {
    // When using Int32/UInt32 the first 32-bits will always the the Int32/UInt32
    // value, so use 32-bit operations to improve performance
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int32_t val   = (int32_t)(((int64_t)(int64_t)array[i]) & valMask);
      int32_t value = 0;
      if (chan != NULL) value  = (chan[i] & chanMask) << (pSize);
      if (evt  != NULL) value |= (evt[i]  & evtMask ) << (pSize+cSize);
      packInt(buf, off,   val);
      packInt(buf, off+4, value);
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int64_t val   = (int64_t)(((int64_t)(int64_t)array[i]) & valMask);
      int64_t value = val << (64-dSize);
      if (chan != NULL) value |= ((int64_t)(chan[i] & chanMask)) << (pSize);
      if (evt  != NULL) value |= ((int64_t)(evt[i]  & evtMask )) << (pSize+cSize);
      packLong(buf, off, value);
    }
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array, where the data is
 *  aligned to 64-bit word boundaries.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Bit offset into buf (must be multiple of 64 bits)
 *  @param array  (OUT) Output value array (not null)
 *  @param chan   (OUT) Output channel tag array (null if n/a)
 *  @param evt    (OUT) Output event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackFloatWord64 (const void *ptr, int32_t off, float *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 64 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(dSize);
  // When using Float32 the first 32-bits will always the the Float32 value, so
  // use 32-bit operations to improve performance
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=8) {
    array[i] = (float)unpackFloat(buf, off);
    int32_t value = unpackInt(buf, off+4);
    if (chan != NULL) chan[i] = (value >> (pSize      )) & chanMask;
    if (evt  != NULL) evt[i]  = (value >> (pSize+cSize)) & evtMask;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array, where the data is
 *  aligned to 64-bit word boundaries.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input value array
 *  @param chan   (IN)  Input channel tag array (null if n/a)
 *  @param evt    (IN)  Input event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packFloatWord64 (void *ptr, int32_t off, const float *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 64 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(dSize);
  // When using Float32 the first 32-bits will always the the Float32 value, so
  // use 32-bit operations to improve performance
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=8) {
    float val   = (float)array[i];
    int32_t   value = 0;
    if (chan != NULL) value  = (chan[i] & chanMask) << (pSize);
    if (evt  != NULL) value |= (evt[i]  & evtMask ) << (pSize+cSize);
    packFloat(buf, off,   val);
    packInt(  buf, off+4, value);
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array, where the data is
 *  aligned to 32-bit word boundaries.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Bit offset into buf (must be multiple of 32 bits)
 *  @param array  (OUT) Output value array (not null)
 *  @param chan   (OUT) Output channel tag array (null if n/a)
 *  @param evt    (OUT) Output event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackVRTFloatWord32 (const void *ptr, int32_t off, float *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 32 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=4) {
    int32_t value = unpackInt(buf, off);
    int32_t bits  = (value >> (32-dSize)); // fromVRTFloat32(..) will mask off any extra bits
    array[i] = (float)fromVRTFloat32(sign, expBits, dSize, bits);
    if (chan != NULL) chan[i] = (value >> (pSize      )) & chanMask;
    if (evt  != NULL) evt[i]  = (value >> (pSize+cSize)) & evtMask;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array, where the data is
 *  aligned to 32-bit word boundaries.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input value array
 *  @param chan   (IN)  Input channel tag array (null if n/a)
 *  @param evt    (IN)  Input event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packVRTFloatWord32 (void *ptr, int32_t off, const float *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 32 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=4) {
    int32_t val   = toVRTFloat32(sign, expBits, dSize, (double)array[i]);
    int32_t value = val << (32-dSize);
    if (chan != NULL) value |= (chan[i] & chanMask) << (pSize);
    if (evt  != NULL) value |= (evt[i]  & evtMask ) << (pSize+cSize);
    packInt(buf, off, value);
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array, where the data is
 *  aligned to 64-bit word boundaries.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Bit offset into buf (must be multiple of 64 bits)
 *  @param array  (OUT) Output value array (not null)
 *  @param chan   (OUT) Output channel tag array (null if n/a)
 *  @param evt    (OUT) Output event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackVRTFloatWord64 (const void *ptr, int32_t off, float *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 64 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  if (dSize == 32) {
    // When using Int32/UInt32 the first 32-bits will always the the Int32/UInt32
    // value, so use 32-bit operations to improve performance
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int32_t val   = unpackInt(buf, off);
      int32_t value = unpackInt(buf, off+4);
      array[i] = (float)fromVRTFloat32(sign, expBits, dSize, val);
      if (chan != NULL) chan[i] = ((int32_t)(value >> (pSize      ))) & chanMask;
      if (evt  != NULL) evt[i]  = ((int32_t)(value >> (pSize+cSize))) & evtMask;
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int64_t value = unpackLong(buf, off);
      int64_t bits  = (value >> (64-dSize)); // fromVRTFloat64(..) will mask off any extra bits
      array[i] = (float)fromVRTFloat64(sign, expBits, dSize, bits);
      if (chan != NULL) chan[i] = ((int32_t)(value >> (pSize      ))) & chanMask;
      if (evt  != NULL) evt[i]  = ((int32_t)(value >> (pSize+cSize))) & evtMask;
    }
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array, where the data is
 *  aligned to 64-bit word boundaries.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input value array
 *  @param chan   (IN)  Input channel tag array (null if n/a)
 *  @param evt    (IN)  Input event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packVRTFloatWord64 (void *ptr, int32_t off, const float *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 64 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  if (dSize == 32) {
    // When using Int32/UInt32 the first 32-bits will always the the Int32/UInt32
    // value, so use 32-bit operations to improve performance
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int32_t val   = toVRTFloat32(sign, expBits, dSize, (double)array[i]);
      int32_t value = 0;
      if (chan != NULL) value  = (chan[i] & chanMask) << (pSize);
      if (evt  != NULL) value |= (evt[i]  & evtMask ) << (pSize+cSize);
      packInt(buf, off,   val);
      packInt(buf, off+4, value);
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int64_t val   = toVRTFloat64(sign, expBits, dSize, (double)array[i]);
      int64_t value = val << (64-dSize);
      if (chan != NULL) value |= ((int64_t)(chan[i] & chanMask)) << (pSize);
      if (evt  != NULL) value |= ((int64_t)(evt[i]  & evtMask )) << (pSize+cSize);
      packLong(buf, off, value);
    }
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array.
 *  @param ptr       (IN)  Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf (must be multiple of 32 bits)
 *  @param array     (OUT) Output value array (not null)
 *  @param chan      (OUT) Output channel tag array (null if n/a)
 *  @param evt       (OUT) Output event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackIntBuffer32 (const void *ptr, int32_t bitOffset, float *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  const int32_t bitmask      = 32 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t _bits_dSize  = 32 - dSize;    // pre-compute "32 - dSize" for efficiency
  int32_t _bits_fSize  = 32 - fSize;    // pre-compute "32 - fSize" for efficiency
  int32_t _fSize_dSize = fSize - dSize;         // pre-compute "fSize - dSize" for efficiency
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      // Not enough bits remaining in this word, move to next word (skipping pad bits)
      bitOffset = (bitOffset + 32) & bitmaskInv;
    }
    int32_t bits  = unpackBits32(buf, bitOffset, fSize);
    int32_t value = bits >> _fSize_dSize;
    // sign extend as needed (otherwise leave top bits 0)
    array[i] = (sign)? (float)((value << _bits_dSize) >>  _bits_dSize)
                     : (float)value;
    if (evt  != NULL) evt[i]  = ((int32_t)(bits >> cSize)) & eMask;
    if (chan != NULL) chan[i] = ((int32_t)(bits         )) & cMask;
    bitOffset += fSize;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array.
 *  @param ptr       (I/O) Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf
 *  @param array     (IN)  Input value array
 *  @param chan      (IN)  Input channel tag array (null if n/a)
 *  @param evt       (IN)  Input event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void packIntBuffer32 (void *ptr, int32_t bitOffset, const float *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  const int32_t bitmask      = 32 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t uSize        = fSize - dSize - eSize - cSize;
  int32_t _bits_fSize  = 32 - fSize;    // pre-compute "32 - fSize" for efficiency
  int32_t valMask      = (sign)? ((int32_t)-1) : ((int32_t)__INT64_C(0xFFFFFFFF)); // Bit mask for input value (controls sign-extend)
  if ((evt == NULL) || (eSize == 0)) {
    // Treat event tag as unused since we have nothing to insert there (this
    // will be slightly faster in the loop below).
    uSize = uSize + eSize;
    eSize = 0;
    evt   = NULL;
    if (chan == NULL) {
      // Treat channel tag as unused since we have nothing to insert there (this
      // will be slightly faster in the loop below).
      uSize = uSize + cSize;
      cSize = 0;
      chan  = NULL;
    }
  }
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  int32_t _uSize_eSize = uSize + eSize;         // pre-compute "uSize + eSize" for efficiency
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      if (true) { //TODO: make this optional
        // Not enough bits remaining in this word, move to next word (clearing pad bits)
        int32_t zSize = 32 - (bitOffset & bitmask); // bits remaining to set
        packBits32(buf, bitOffset, zSize, 0);
        bitOffset += zSize;
      }
      else {
        // Not enough bits remaining in this word, move to next word (skipping pad bits)
        bitOffset = (bitOffset + 32) & bitmaskInv;
      }
    }
    int32_t value = (int32_t)(((int32_t)(int64_t)array[i]) & valMask);
    value = value << _uSize_eSize;
    if (evt != NULL) value = value | (evt[i] & eMask);
    value = value << cSize;
    if (chan != NULL) value = value | (chan[i] & cMask);
    packBits32(buf, bitOffset, fSize, value);
    bitOffset += fSize;
  }
  // ==== End of Packet Pad Bits ===============================================
  // Clear out any bits remaining in the current word that would be at the end
  // of the packet, this applies to both link and processing efficient.
  if (true) { //TODO: make this optional
    int32_t xSize = 32 - (bitOffset & bitmask); // bits remaining to set
    if (xSize > 0) {
      packBits32(buf, bitOffset, xSize, 0);
      bitOffset += xSize;
    }
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array.
 *  @param ptr       (IN)  Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf (must be multiple of 64 bits)
 *  @param array     (OUT) Output value array (not null)
 *  @param chan      (OUT) Output channel tag array (null if n/a)
 *  @param evt       (OUT) Output event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackIntBuffer64 (const void *ptr, int32_t bitOffset, float *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  const int32_t bitmask      = 64 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t _bits_dSize  = 64 - dSize;    // pre-compute "64 - dSize" for efficiency
  int32_t _bits_fSize  = 64 - fSize;    // pre-compute "64 - fSize" for efficiency
  int32_t _fSize_dSize = fSize - dSize;         // pre-compute "fSize - dSize" for efficiency
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      // Not enough bits remaining in this word, move to next word (skipping pad bits)
      bitOffset = (bitOffset + 32) & bitmaskInv;
    }
    int64_t bits  = unpackBits64(buf, bitOffset, fSize);
    int64_t value = bits >> _fSize_dSize;
    // sign extend as needed (otherwise leave top bits 0)
    array[i] = (sign)? (float)((value << _bits_dSize) >>  _bits_dSize)
                     : (float)value;
    if (evt  != NULL) evt[i]  = ((int32_t)(bits >> cSize)) & eMask;
    if (chan != NULL) chan[i] = ((int32_t)(bits         )) & cMask;
    bitOffset += fSize;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array.
 *  @param ptr       (I/O) Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf
 *  @param array     (IN)  Input value array
 *  @param chan      (IN)  Input channel tag array (null if n/a)
 *  @param evt       (IN)  Input event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void packIntBuffer64 (void *ptr, int32_t bitOffset, const float *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  const int32_t bitmask      = 64 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t uSize        = fSize - dSize - eSize - cSize;
  int32_t _bits_fSize  = 64 - fSize;    // pre-compute "64 - fSize" for efficiency
  int64_t valMask      = (sign)? ((int64_t)-1) : ((int64_t)__INT64_C(0xFFFFFFFF)); // Bit mask for input value (controls sign-extend)
  if ((evt == NULL) || (eSize == 0)) {
    // Treat event tag as unused since we have nothing to insert there (this
    // will be slightly faster in the loop below).
    uSize = uSize + eSize;
    eSize = 0;
    evt   = NULL;
    if (chan == NULL) {
      // Treat channel tag as unused since we have nothing to insert there (this
      // will be slightly faster in the loop below).
      uSize = uSize + cSize;
      cSize = 0;
      chan  = NULL;
    }
  }
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  int32_t _uSize_eSize = uSize + eSize;         // pre-compute "uSize + eSize" for efficiency
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      if (true) { //TODO: make this optional
        // Not enough bits remaining in this word, move to next word (clearing pad bits)
        int32_t zSize = 64 - (bitOffset & bitmask); // bits remaining to set
        packBits64(buf, bitOffset, zSize, 0);
        bitOffset += zSize;
      }
      else {
        // Not enough bits remaining in this word, move to next word (skipping pad bits)
        bitOffset = (bitOffset + 32) & bitmaskInv;
      }
    }
    int64_t value = (int64_t)(((int64_t)(int64_t)array[i]) & valMask);
    value = value << _uSize_eSize;
    if (evt != NULL) value = value | (evt[i] & eMask);
    value = value << cSize;
    if (chan != NULL) value = value | (chan[i] & cMask);
    packBits64(buf, bitOffset, fSize, value);
    bitOffset += fSize;
  }
  // ==== End of Packet Pad Bits ===============================================
  // Clear out any bits remaining in the current word that would be at the end
  // of the packet, this applies to both link and processing efficient.
  if (true) { //TODO: make this optional
    int32_t xSize = 64 - (bitOffset & bitmask); // bits remaining to set
    if (xSize > 0) {
      packBits64(buf, bitOffset, xSize, 0);
      bitOffset += xSize;
    }
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array.
 *  @param ptr       (IN)  Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf (must be multiple of 64 bits)
 *  @param array     (OUT) Output value array (not null)
 *  @param chan      (OUT) Output channel tag array (null if n/a)
 *  @param evt       (OUT) Output event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackFloatBuffer64 (const void *ptr, int32_t bitOffset, float *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  const int32_t bitmask      = 64 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t _bits_fSize  = 64 - fSize;    // pre-compute "64 - fSize" for efficiency
  int32_t _fSize_dSize = fSize - dSize;         // pre-compute "fSize - dSize" for efficiency
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      // Not enough bits remaining in this word, move to next word (skipping pad bits)
      bitOffset = (bitOffset + 32) & bitmaskInv;
    }
    int64_t bits  = unpackBits64(buf, bitOffset, fSize);
    int64_t value = bits >> _fSize_dSize;
    UNUSED_VARIABLE(sign);
    array[i] = (float)intBitsToFloat((int32_t)value);
    if (evt  != NULL) evt[i]  = ((int32_t)(bits >> cSize)) & eMask;
    if (chan != NULL) chan[i] = ((int32_t)(bits         )) & cMask;
    bitOffset += fSize;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array.
 *  @param ptr       (I/O) Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf
 *  @param array     (IN)  Input value array
 *  @param chan      (IN)  Input channel tag array (null if n/a)
 *  @param evt       (IN)  Input event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void packFloatBuffer64 (void *ptr, int32_t bitOffset, const float *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  const int32_t bitmask      = 64 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t uSize        = fSize - dSize - eSize - cSize;
  int32_t _bits_fSize  = 64 - fSize;    // pre-compute "64 - fSize" for efficiency
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  if ((evt == NULL) || (eSize == 0)) {
    // Treat event tag as unused since we have nothing to insert there (this
    // will be slightly faster in the loop below).
    uSize = uSize + eSize;
    eSize = 0;
    evt   = NULL;
    if (chan == NULL) {
      // Treat channel tag as unused since we have nothing to insert there (this
      // will be slightly faster in the loop below).
      uSize = uSize + cSize;
      cSize = 0;
      chan  = NULL;
    }
  }
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  int32_t _uSize_eSize = uSize + eSize;         // pre-compute "uSize + eSize" for efficiency
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      if (true) { //TODO: make this optional
        // Not enough bits remaining in this word, move to next word (clearing pad bits)
        int32_t zSize = 64 - (bitOffset & bitmask); // bits remaining to set
        packBits64(buf, bitOffset, zSize, 0);
        bitOffset += zSize;
      }
      else {
        // Not enough bits remaining in this word, move to next word (skipping pad bits)
        bitOffset = (bitOffset + 32) & bitmaskInv;
      }
    }
    int64_t value = (int64_t)floatToRawIntBits((float)array[i]);
    value = value << _uSize_eSize;
    if (evt != NULL) value = value | (evt[i] & eMask);
    value = value << cSize;
    if (chan != NULL) value = value | (chan[i] & cMask);
    packBits64(buf, bitOffset, fSize, value);
    bitOffset += fSize;
  }
  // ==== End of Packet Pad Bits ===============================================
  // Clear out any bits remaining in the current word that would be at the end
  // of the packet, this applies to both link and processing efficient.
  if (true) { //TODO: make this optional
    int32_t xSize = 64 - (bitOffset & bitmask); // bits remaining to set
    if (xSize > 0) {
      packBits64(buf, bitOffset, xSize, 0);
      bitOffset += xSize;
    }
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array.
 *  @param ptr       (IN)  Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf (must be multiple of 32 bits)
 *  @param array     (OUT) Output value array (not null)
 *  @param chan      (OUT) Output channel tag array (null if n/a)
 *  @param evt       (OUT) Output event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackVRTFloatBuffer32 (const void *ptr, int32_t bitOffset, float *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  const char *buf = (const char*)ptr;
  const int32_t bitmask      = 32 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t _bits_fSize  = 32 - fSize;    // pre-compute "32 - fSize" for efficiency
  int32_t _fSize_dSize = fSize - dSize;         // pre-compute "fSize - dSize" for efficiency
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      // Not enough bits remaining in this word, move to next word (skipping pad bits)
      bitOffset = (bitOffset + 32) & bitmaskInv;
    }
    int32_t bits  = unpackBits32(buf, bitOffset, fSize);
    int32_t value = bits >> _fSize_dSize;
    array[i] = (float)fromVRTFloat32(sign, expBits, dSize, value);
    if (evt  != NULL) evt[i]  = ((int32_t)(bits >> cSize)) & eMask;
    if (chan != NULL) chan[i] = ((int32_t)(bits         )) & cMask;
    bitOffset += fSize;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array.
 *  @param ptr       (I/O) Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf
 *  @param array     (IN)  Input value array
 *  @param chan      (IN)  Input channel tag array (null if n/a)
 *  @param evt       (IN)  Input event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void packVRTFloatBuffer32 (void *ptr, int32_t bitOffset, const float *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  char *buf = (char*)ptr;
  const int32_t bitmask      = 32 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t uSize        = fSize - dSize - eSize - cSize;
  int32_t _bits_fSize  = 32 - fSize;    // pre-compute "32 - fSize" for efficiency
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  if ((evt == NULL) || (eSize == 0)) {
    // Treat event tag as unused since we have nothing to insert there (this
    // will be slightly faster in the loop below).
    uSize = uSize + eSize;
    eSize = 0;
    evt   = NULL;
    if (chan == NULL) {
      // Treat channel tag as unused since we have nothing to insert there (this
      // will be slightly faster in the loop below).
      uSize = uSize + cSize;
      cSize = 0;
      chan  = NULL;
    }
  }
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  int32_t _uSize_eSize = uSize + eSize;         // pre-compute "uSize + eSize" for efficiency
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      if (true) { //TODO: make this optional
        // Not enough bits remaining in this word, move to next word (clearing pad bits)
        int32_t zSize = 32 - (bitOffset & bitmask); // bits remaining to set
        packBits32(buf, bitOffset, zSize, 0);
        bitOffset += zSize;
      }
      else {
        // Not enough bits remaining in this word, move to next word (skipping pad bits)
        bitOffset = (bitOffset + 32) & bitmaskInv;
      }
    }
    int32_t value = toVRTFloat32(sign, expBits, dSize, (double)array[i]);
    value = value << _uSize_eSize;
    if (evt != NULL) value = value | (evt[i] & eMask);
    value = value << cSize;
    if (chan != NULL) value = value | (chan[i] & cMask);
    packBits32(buf, bitOffset, fSize, value);
    bitOffset += fSize;
  }
  // ==== End of Packet Pad Bits ===============================================
  // Clear out any bits remaining in the current word that would be at the end
  // of the packet, this applies to both link and processing efficient.
  if (true) { //TODO: make this optional
    int32_t xSize = 32 - (bitOffset & bitmask); // bits remaining to set
    if (xSize > 0) {
      packBits32(buf, bitOffset, xSize, 0);
      bitOffset += xSize;
    }
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array.
 *  @param ptr       (IN)  Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf (must be multiple of 64 bits)
 *  @param array     (OUT) Output value array (not null)
 *  @param chan      (OUT) Output channel tag array (null if n/a)
 *  @param evt       (OUT) Output event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackVRTFloatBuffer64 (const void *ptr, int32_t bitOffset, float *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  const char *buf = (const char*)ptr;
  const int32_t bitmask      = 64 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t _bits_fSize  = 64 - fSize;    // pre-compute "64 - fSize" for efficiency
  int32_t _fSize_dSize = fSize - dSize;         // pre-compute "fSize - dSize" for efficiency
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      // Not enough bits remaining in this word, move to next word (skipping pad bits)
      bitOffset = (bitOffset + 32) & bitmaskInv;
    }
    int64_t bits  = unpackBits64(buf, bitOffset, fSize);
    int64_t value = bits >> _fSize_dSize;
    array[i] = (float)fromVRTFloat64(sign, expBits, dSize, value);
    if (evt  != NULL) evt[i]  = ((int32_t)(bits >> cSize)) & eMask;
    if (chan != NULL) chan[i] = ((int32_t)(bits         )) & cMask;
    bitOffset += fSize;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array.
 *  @param ptr       (I/O) Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf
 *  @param array     (IN)  Input value array
 *  @param chan      (IN)  Input channel tag array (null if n/a)
 *  @param evt       (IN)  Input event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void packVRTFloatBuffer64 (void *ptr, int32_t bitOffset, const float *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  char *buf = (char*)ptr;
  const int32_t bitmask      = 64 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t uSize        = fSize - dSize - eSize - cSize;
  int32_t _bits_fSize  = 64 - fSize;    // pre-compute "64 - fSize" for efficiency
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  if ((evt == NULL) || (eSize == 0)) {
    // Treat event tag as unused since we have nothing to insert there (this
    // will be slightly faster in the loop below).
    uSize = uSize + eSize;
    eSize = 0;
    evt   = NULL;
    if (chan == NULL) {
      // Treat channel tag as unused since we have nothing to insert there (this
      // will be slightly faster in the loop below).
      uSize = uSize + cSize;
      cSize = 0;
      chan  = NULL;
    }
  }
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  int32_t _uSize_eSize = uSize + eSize;         // pre-compute "uSize + eSize" for efficiency
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      if (true) { //TODO: make this optional
        // Not enough bits remaining in this word, move to next word (clearing pad bits)
        int32_t zSize = 64 - (bitOffset & bitmask); // bits remaining to set
        packBits64(buf, bitOffset, zSize, 0);
        bitOffset += zSize;
      }
      else {
        // Not enough bits remaining in this word, move to next word (skipping pad bits)
        bitOffset = (bitOffset + 32) & bitmaskInv;
      }
    }
    int64_t value = toVRTFloat64(sign, expBits, dSize, (double)array[i]);
    value = value << _uSize_eSize;
    if (evt != NULL) value = value | (evt[i] & eMask);
    value = value << cSize;
    if (chan != NULL) value = value | (chan[i] & cMask);
    packBits64(buf, bitOffset, fSize, value);
    bitOffset += fSize;
  }
  // ==== End of Packet Pad Bits ===============================================
  // Clear out any bits remaining in the current word that would be at the end
  // of the packet, this applies to both link and processing efficient.
  if (true) { //TODO: make this optional
    int32_t xSize = 64 - (bitOffset & bitmask); // bits remaining to set
    if (xSize > 0) {
      packBits64(buf, bitOffset, xSize, 0);
      bitOffset += xSize;
    }
  }
}
#if NOT_USING_JNI
float* vrt::PackUnpack::unpackAsFloat (PayloadFormat pf, const void *ptr, int32_t off, float *array, int32_t *chan, int32_t *evt, int32_t length) {
  const char *buf = (const char*)ptr;
  if (!pf.isValid()) {
    throw VRTException(pf.getValid());
  }
  int32_t            fSize  = pf.getItemPackingFieldSize();
  int32_t            dSize  = pf.getDataItemSize();
  int32_t            eSize  = pf.getEventTagSize();
  int32_t            cSize  = pf.getChannelTagSize();
  int32_t            xSize  = fSize; // effective fSize
  bool        proc   = pf.isProcessingEfficient();
  bool        sign   = pf.isSigned();
  DataItemFormat format = pf.getDataItemFormat();
  bool        noTags;
  if (eSize == 0) evt  = NULL; // no event tags, ignore any array passed in
  if (cSize == 0) chan = NULL; // no channel tags, ignore any array passed in
  noTags = ((chan == NULL) && (evt == NULL)) || (fSize == dSize);
  if (proc) {
         if (fSize >= 33) xSize = 64;
    else if (fSize >= 17) xSize = 32;
    if (noTags) fSize = xSize; // Use faster field size if ignoring Event & Channel Tags
  }
  switch (format) {
    case DataItemFormat_SignedInt:   // FALLTHROUGH
    case DataItemFormat_UnsignedInt:
      // ---- Try fast methods -------------------------------------------------
      if (noTags) {
        switch (fSize) {
          case  8: unpackBytes(  buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
          case 16: unpackShorts( buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
          case 24: unpackInt24s( buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
          case 32: unpackInts(   buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
          case 64: unpackLongs(  buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
        }
      }
      if (fSize == dSize) { // implicit noTags=true in this case
        switch (fSize) {
          case  1: if ((length % 8) == 0) { unpackBits(   buf, off, array, 0, length, fSize, dSize, proc, sign); return array; } break;
          case  4: if ((length % 2) == 0) { unpackNibbles(buf, off, array, 0, length, fSize, dSize, proc, sign); return array; } break;
          case 12: if ((length % 2) == 0) { unpackInt12s( buf, off, array, 0, length, fSize, dSize, proc, sign); return array; } break;
        }
      }
      if (xSize == 32) {
        unpackIntWord32(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return array;
      }
      if (xSize == 64) {
        unpackIntWord64(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return array;
      }
      // ---- Fall back to slow method -----------------------------------------
      if (fSize <= 32) {
        unpackIntBuffer32(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return array;
      }
      else {
        unpackIntBuffer64(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return array;
      }
    case DataItemFormat_Float:
      // ---- Try fast methods -------------------------------------------------
      if (fSize == dSize) { // implicit noTags=true in this case
        unpackFloats(buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
      }
      if (xSize == 64) {
        unpackFloatWord64(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return array;
      }
      // ---- Fall back to slow method -----------------------------------------
      unpackFloatBuffer64(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return array;
    case DataItemFormat_Double:
      // ---- Only fast method for double (yea!) -------------------------------
      unpackDoubles(buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
    default: // VRTFloat Types
      // ---- Try fast methods -------------------------------------------------
      if (xSize == 32) {
        unpackVRTFloatWord32(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return array;
      }
      if (xSize == 64) {
        unpackVRTFloatWord64(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return array;
      }
      // ---- Fall back to slow method -----------------------------------------
      if (fSize <= 32) {
        unpackVRTFloatBuffer32(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return array;
      }
      else {
        unpackVRTFloatBuffer64(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return array;
      }
  }
}
void vrt::PackUnpack::packAsFloat (PayloadFormat pf, void *ptr, int32_t off, const float *array, const int32_t *chan, const int32_t *evt, int32_t length) {
  char *buf = (char*)ptr;
  if (!pf.isValid()) {
    throw VRTException(pf.getValid());
  }
  int32_t            fSize  = pf.getItemPackingFieldSize();
  int32_t            dSize  = pf.getDataItemSize();
  int32_t            eSize  = pf.getEventTagSize();
  int32_t            cSize  = pf.getChannelTagSize();
  int32_t            xSize  = fSize; // effective fSize
  bool        proc   = pf.isProcessingEfficient();
  bool        sign   = pf.isSigned();
  DataItemFormat format = pf.getDataItemFormat();
  bool        noTags;
  if (eSize == 0) evt  = NULL; // no event tags, ignore any array passed in
  if (cSize == 0) chan = NULL; // no channel tags, ignore any array passed in
  noTags = ((chan == NULL) && (evt == NULL)) || (fSize == dSize);
  if (proc) {
         if (fSize >= 33) xSize = 64;
    else if (fSize >= 17) xSize = 32;
    if (noTags) fSize = xSize; // Use faster field size if ignoring Event & Channel Tags
  }
  switch (format) {
    case DataItemFormat_SignedInt:   // FALLTHROUGH
    case DataItemFormat_UnsignedInt:
      // ---- Try fast methods -------------------------------------------------
      if (noTags) {
        switch (fSize) {
          case  8: packBytes(  buf, off, array, 0, length, fSize, dSize, proc, sign); return;
          case 16: packShorts( buf, off, array, 0, length, fSize, dSize, proc, sign); return;
          case 24: packInt24s( buf, off, array, 0, length, fSize, dSize, proc, sign); return;
          case 32: packInts(   buf, off, array, 0, length, fSize, dSize, proc, sign); return;
          case 64: packLongs(  buf, off, array, 0, length, fSize, dSize, proc, sign); return;
        }
      }
      if (fSize == dSize) { // implicit noTags=true in this case
        switch (fSize) {
          case  1: if ((length % 8) == 0) { packBits(   buf, off, array, 0, length, fSize, dSize, proc, sign); return; } break;
          case  4: if ((length % 2) == 0) { packNibbles(buf, off, array, 0, length, fSize, dSize, proc, sign); return; } break;
          case 12: if ((length % 2) == 0) { packInt12s( buf, off, array, 0, length, fSize, dSize, proc, sign); return; } break;
        }
      }
      if (xSize == 32) {
        packIntWord32(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return;
      }
      if (xSize == 64) {
        packIntWord64(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return;
      }
      // ---- Fall back to slow method -----------------------------------------
      if (fSize <= 32) {
        packIntBuffer32(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return;
      }
      else {
        packIntBuffer64(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return;
      }
    case DataItemFormat_Float:
      // ---- Try fast methods -------------------------------------------------
      if (fSize == dSize) { // implicit noTags=true in this case
        packFloats(buf, off, array, 0, length, fSize, dSize, proc, sign); return;
      }
      if (xSize == 64) {
        packFloatWord64(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return;
      }
      // ---- Fall back to slow method -----------------------------------------
      packFloatBuffer64(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return;
    case DataItemFormat_Double:
      // ---- Only fast method for double (yea!) -------------------------------
      packDoubles(buf, off, array, 0, length, fSize, dSize, proc, sign); return;
    default: // VRTFloat Types
      // ---- Try fast methods -------------------------------------------------
      if (xSize == 32) {
        packVRTFloatWord32(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return;
      }
      if (xSize == 64) {
        packVRTFloatWord64(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return;
      }
      // ---- Fall back to slow method -----------------------------------------
      if (fSize <= 32) {
        packVRTFloatBuffer32(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return;
      }
      else {
        packVRTFloatBuffer64(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return;
      }
  }
}
#endif /* NOT_USING_JNI */
/** <b>Internal Use Only:</b> Fast unpack method for double data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits (must be 64)
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed? (must be true)
 */
static __attribute__((flatten)) void unpackDoubles (const void *ptr, int32_t off, int64_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  // All floating-point values are signed, so 'sign' is unused, but is included
  // in function definition for consistency.
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(align);
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  // When using Double data, it is required that dSize=64 and fSize=64 so we can
  // skip some of the normal checks we would otherwise do here.
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=8) {
    array[offset++] = (int64_t)unpackDouble(buf, off);
  }
}
/** <b>Internal Use Only:</b>
 *  Fast pack method for double data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits (must be 64)
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed? (must be true)
 */
static __attribute__((flatten)) void packDoubles (void *ptr, int32_t off, const int64_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  // All floating-point values are signed, so 'sign' is unused, but is included
  // in function definition for consistency.
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(align);
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  // When using Double data, it is required that dSize=64 and fSize=64 so we can
  // skip some of the normal checks we would otherwise do here.
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=8) {
    packDouble(buf, off, (double)array[offset++]);
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for float data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits (must be 32)
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed? (must be true)
 */
static __attribute__((flatten)) void unpackFloats (const void *ptr, int32_t off, int64_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  // All floating-point values are signed, so 'sign' is unused, but is included
  // in function definition for consistency.
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(dSize);
  int32_t delta; // int8_t offset delta
  if (align) delta = (fSize <= 32)? 4 : 8;
  else       delta = fSize / 8;
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=delta) {
    array[offset++] = (int64_t)unpackFloat(buf, off);
  }
}
/** <b>Internal Use Only:</b>
 *  Fast pack method for float data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits (must be 32)
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed? (must be true)
 */
static __attribute__((flatten)) void packFloats (void *ptr, int32_t off, const int64_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  // All floating-point values are signed, so 'sign' is unused, but is included
  // in function definition for consistency.
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(dSize);
  int32_t delta; // int8_t offset delta
  if (align) delta = (fSize <= 32)? 4 : 8;
  else       delta = fSize / 8;
  if (delta == 4) {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=4) {
      packFloat(buf, off, (float)array[offset++]);
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++) {
      packFloat(buf, off, (float)array[offset++]);
      off += 4;
      _Intel_Pragma("loop_count min=1, max=4")
      for (int32_t j = 4; j < delta; j++) {
        buf[off++] = 0; // set unused bits to zero
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed int64_t data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void unpackLongs (const void *ptr, int32_t off, int64_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
#if (8 == 1) || (BYTE_ORDER == BIG_ENDIAN)
  if ((fSize == 64) && (dSize == 64)) {
    // Fast array copy
    memcpy(&array[offset], &buf[off], length*8);
    return;
  }
#endif
  int32_t delta = (align)? ((fSize <= 32)? 4 : 8) : (fSize/8); // int8_t offset delta
  if (sign) {
    if (dSize == 64) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int64_t)unpackLong(buf, off);
      }
    }
    else {
      int32_t shift  = 64 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int64_t)(unpackLong(buf, off) >> shift);
      }
    }
  }
  else {
    if (dSize == 64) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int64_t)(unpackLong(buf, off) & __INT64_C(0xFFFFFFFFFFFFFFFF));
      }
    }
    else {
      int32_t shift  = 64 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int64_t)((unpackLong(buf, off) & __INT64_C(0xFFFFFFFFFFFFFFFF)) >> shift);
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for unsigned int64_t data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void packLongs (void *ptr, int32_t off, const int64_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
#if (8 == 1) || (BYTE_ORDER == BIG_ENDIAN)
  if ((fSize == 64) && (dSize == 64)) {
    // Fast array copy
    memcpy(&buf[off], &array[offset], length*8);
    return;
  }
#endif
  int32_t delta = (align)? ((fSize <= 32)? 4 : 8) : (fSize/8); // int8_t offset delta
  int64_t valMask = (sign)? ((int64_t)-1) : ((int64_t)__INT64_C(0xFFFFFFFFFFFFFFFF)); // Bit mask for input value (controls sign-extend)
  if (dSize == 64) {
    if (delta == 8) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=8) {
        int64_t val = (int64_t)(((int64_t)array[offset++]) & valMask);
        packLong(buf, off, val);
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int64_t val = (int64_t)(((int64_t)array[offset++]) & valMask);
        packLong(buf, off, val);
        off += 8;
        _Intel_Pragma("loop_count min=1, max=0")
        for (int32_t j = 8; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
  else {
    int32_t shift = 64 - dSize;
    if (delta == 8) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=8) {
        int64_t val = (int64_t)(((int64_t)array[offset++]) & valMask);
        packLong(buf, off, (int64_t)(val << shift));
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int64_t val = (int64_t)(((int64_t)array[offset++]) & valMask);
        packLong(buf, off, (int64_t)(val << shift));
        off += 8;
        _Intel_Pragma("loop_count min=1, max=0")
        for (int32_t j = 8; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed int32_t data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void unpackInts (const void *ptr, int32_t off, int64_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  int32_t delta = (align)? ((fSize <= 32)? 4 : 8) : (fSize/8); // int8_t offset delta
  if (sign) {
    if (dSize == 32) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int64_t)unpackInt(buf, off);
      }
    }
    else {
      int32_t shift  = 32 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int64_t)(unpackInt(buf, off) >> shift);
      }
    }
  }
  else {
    if (dSize == 32) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int64_t)(unpackInt(buf, off) & __INT64_C(0xFFFFFFFF));
      }
    }
    else {
      int32_t shift  = 32 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int64_t)((unpackInt(buf, off) & __INT64_C(0xFFFFFFFF)) >> shift);
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for unsigned int32_t data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void packInts (void *ptr, int32_t off, const int64_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  int32_t delta = (align)? ((fSize <= 32)? 4 : 8) : (fSize/8); // int8_t offset delta
  int32_t valMask = (sign)? ((int32_t)-1) : ((int32_t)__INT64_C(0xFFFFFFFFFFFFFFFF)); // Bit mask for input value (controls sign-extend)
  if (dSize == 32) {
    if (delta == 4) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=4) {
        int32_t val = (int32_t)(((int32_t)array[offset++]) & valMask);
        packInt(buf, off, val);
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int32_t val = (int32_t)(((int32_t)array[offset++]) & valMask);
        packInt(buf, off, val);
        off += 4;
        _Intel_Pragma("loop_count min=1, max=4")
        for (int32_t j = 4; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
  else {
    int32_t shift = 32 - dSize;
    if (delta == 4) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=4) {
        int32_t val = (int32_t)(((int32_t)array[offset++]) & valMask);
        packInt(buf, off, (int32_t)(val << shift));
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int32_t val = (int32_t)(((int32_t)array[offset++]) & valMask);
        packInt(buf, off, (int32_t)(val << shift));
        off += 4;
        _Intel_Pragma("loop_count min=1, max=4")
        for (int32_t j = 4; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed int32_t data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void unpackInt24s (const void *ptr, int32_t off, int64_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  int32_t delta = (align)? ((fSize <= 32)? 4 : 8) : (fSize/8); // int8_t offset delta
  if (sign) {
    if (dSize == 24) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int64_t)unpackInt24(buf, off);
      }
    }
    else {
      int32_t shift  = 24 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int64_t)(unpackInt24(buf, off) >> shift);
      }
    }
  }
  else {
    if (dSize == 24) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int64_t)(unpackInt24(buf, off) & __INT64_C(0xFFFFFF));
      }
    }
    else {
      int32_t shift  = 24 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int64_t)((unpackInt24(buf, off) & __INT64_C(0xFFFFFF)) >> shift);
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for unsigned int32_t data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void packInt24s (void *ptr, int32_t off, const int64_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  int32_t delta = (align)? ((fSize <= 32)? 4 : 8) : (fSize/8); // int8_t offset delta
  int32_t valMask = (sign)? ((int32_t)-1) : ((int32_t)__INT64_C(0xFFFFFFFFFFFFFFFF)); // Bit mask for input value (controls sign-extend)
  if (dSize == 24) {
    if (delta == 3) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=3) {
        int32_t val = (int32_t)(((int32_t)array[offset++]) & valMask);
        packInt24(buf, off, val);
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int32_t val = (int32_t)(((int32_t)array[offset++]) & valMask);
        packInt24(buf, off, val);
        off += 3;
        _Intel_Pragma("loop_count min=1, max=5")
        for (int32_t j = 3; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
  else {
    int32_t shift = 24 - dSize;
    if (delta == 3) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=3) {
        int32_t val = (int32_t)(((int32_t)array[offset++]) & valMask);
        packInt24(buf, off, (int32_t)(val << shift));
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int32_t val = (int32_t)(((int32_t)array[offset++]) & valMask);
        packInt24(buf, off, (int32_t)(val << shift));
        off += 3;
        _Intel_Pragma("loop_count min=1, max=5")
        for (int32_t j = 3; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed int16_t data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void unpackShorts (const void *ptr, int32_t off, int64_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  int32_t delta = fSize / 8; // int8_t offset delta
  if (sign) {
    if (dSize == 16) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int64_t)unpackShort(buf, off);
      }
    }
    else {
      int32_t shift  = 16 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int64_t)(unpackShort(buf, off) >> shift);
      }
    }
  }
  else {
    if (dSize == 16) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int64_t)(unpackShort(buf, off) & __INT64_C(0xFFFF));
      }
    }
    else {
      int32_t shift  = 16 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int64_t)((unpackShort(buf, off) & __INT64_C(0xFFFF)) >> shift);
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for unsigned int16_t data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void packShorts (void *ptr, int32_t off, const int64_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(align);
  int32_t delta = fSize / 8; // int8_t offset delta
  int16_t valMask = (sign)? ((int16_t)-1) : ((int16_t)__INT64_C(0xFFFFFFFFFFFFFFFF)); // Bit mask for input value (controls sign-extend)
  if (dSize == 16) {
    if (delta == 2) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=2) {
        int16_t val = (int16_t)(((int16_t)array[offset++]) & valMask);
        packShort(buf, off, val);
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int16_t val = (int16_t)(((int16_t)array[offset++]) & valMask);
        packShort(buf, off, val);
        off += 2;
        _Intel_Pragma("loop_count min=1, max=6")
        for (int32_t j = 2; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
  else {
    int32_t shift = 16 - dSize;
    if (delta == 2) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=2) {
        int16_t val = (int16_t)(((int16_t)array[offset++]) & valMask);
        packShort(buf, off, (int16_t)(val << shift));
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int16_t val = (int16_t)(((int16_t)array[offset++]) & valMask);
        packShort(buf, off, (int16_t)(val << shift));
        off += 2;
        _Intel_Pragma("loop_count min=1, max=6")
        for (int32_t j = 2; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed Int12 data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert (must be a multiple of 2)
 *  @param fSize  (IN)  Item packing field size in bits (must be 12)
 *  @param dSize  (IN)  Data item size in bits (must be 12)
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackInt12s (const void *ptr, int32_t off, int64_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  int32_t skip = (align)? 1 : 0;
  if (sign) {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i+=2) {
      int32_t a = (int32_t)buf[off++];
      int32_t b = (int32_t)buf[off++];
      int32_t c = (int32_t)buf[off++];
      int32_t x = (a << 4) | ((b >> 4) & 0x000F);
      int32_t y = (b << 8) | ((c     ) & 0x00FF);
      array[offset++] = (int64_t)((x << 20) >> 20); // sign extend
      array[offset++] = (int64_t)((y << 20) >> 20); // sign extend
      off += skip; // skip extra int8_t in processing-efficient mode
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i+=2) {
      int32_t a = (int32_t)buf[off++];
      int32_t b = (int32_t)buf[off++];
      int32_t c = (int32_t)buf[off++];
      int32_t x = (a << 4) | ((b >> 4) & 0x000F);
      int32_t y = (b << 8) | ((c     ) & 0x00FF);
      array[offset++] = (int64_t)(x & 0x0FFF); // clear high-order bits
      array[offset++] = (int64_t)(y & 0x0FFF); // clear high-order bits
      off += skip; // skip extra int8_t in processing-efficient mode
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for signed Int12 data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert (must be a multiple of 2)
 *  @param fSize  (IN)  Item packing field size in bits (must be 12)
 *  @param dSize  (IN)  Data item size in bits (must be 12)
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packInt12s (void *ptr, int32_t off, const int64_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  int32_t skip = (align)?  1 : 0;
  int32_t mask = (sign )? -1 : ((int32_t)__INT64_C(0xFFFFFFFFFFFFFFFF));
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i+=2) {
    int32_t first  = ((int32_t)array[offset++]) & mask;
    int32_t second = ((int32_t)array[offset++]) & mask;
    buf[off++] = (int8_t)(first >> 4);
    buf[off++] = (int8_t)((first << 4) | ((second >> 8) & 0x0F));
    buf[off++] = (int8_t)(second);
    off += skip; // skip extra int8_t in processing-efficient mode
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed int8_t data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void unpackBytes (const void *ptr, int32_t off, int64_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  int32_t delta = fSize / 8; // int8_t offset delta
  if (sign) {
    if (dSize == 8) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int64_t)unpackByte(buf, off);
      }
    }
    else {
      int32_t shift  = 8 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int64_t)(unpackByte(buf, off) >> shift);
      }
    }
  }
  else {
    if (dSize == 8) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int64_t)(unpackByte(buf, off) & __INT64_C(0xFF));
      }
    }
    else {
      int32_t shift  = 8 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int64_t)((unpackByte(buf, off) & __INT64_C(0xFF)) >> shift);
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for unsigned int8_t data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void packBytes (void *ptr, int32_t off, const int64_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(align);
  int32_t delta = fSize / 8; // int8_t offset delta
  int8_t valMask = (sign)? ((int8_t)-1) : ((int8_t)__INT64_C(0xFFFFFFFFFFFFFFFF)); // Bit mask for input value (controls sign-extend)
  if (dSize == 8) {
    if (delta == 1) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=1) {
        int8_t val = (int8_t)(((int8_t)array[offset++]) & valMask);
        packByte(buf, off, val);
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int8_t val = (int8_t)(((int8_t)array[offset++]) & valMask);
        packByte(buf, off, val);
        off += 1;
        _Intel_Pragma("loop_count min=1, max=7")
        for (int32_t j = 1; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
  else {
    int32_t shift = 8 - dSize;
    if (delta == 1) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=1) {
        int8_t val = (int8_t)(((int8_t)array[offset++]) & valMask);
        packByte(buf, off, (int8_t)(val << shift));
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int8_t val = (int8_t)(((int8_t)array[offset++]) & valMask);
        packByte(buf, off, (int8_t)(val << shift));
        off += 1;
        _Intel_Pragma("loop_count min=1, max=7")
        for (int32_t j = 1; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed Int4 (nibble) data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert (must be a multiple of 2)
 *  @param fSize  (IN)  Item packing field size in bits (must be 4)
 *  @param dSize  (IN)  Data item size in bits (must be 4)
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackNibbles (const void *ptr, int32_t off, int64_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  if (sign) {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i+=2) {
      int32_t b = (int32_t)buf[off++];
      array[offset++] = (int64_t)((b << 24) >> 28); // shift & sign extend
      array[offset++] = (int64_t)((b << 28) >> 28); // shift & sign extend
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i+=2) {
      int32_t b = (int32_t)buf[off++];
      array[offset++] = (int64_t)((b >> 4) & 0xF);
      array[offset++] = (int64_t)((b     ) & 0xF);
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for signed Int4 (nibble) data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert (must be a multiple of 2)
 *  @param fSize  (IN)  Item packing field size in bits (must be 4)
 *  @param dSize  (IN)  Data item size in bits (must be 4)
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packNibbles (void *ptr, int32_t off, const int64_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(align);
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  // Pack function for signed and unsigned are identical for nibble data, hence
  // 'sign' is unused here, but included in function definition for consistency.
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i+=2) {
    int32_t first  = (int32_t)array[offset++];
    int32_t second = (int32_t)array[offset++];
    buf[off++] = (int8_t)((first << 4) | (second & 0xF));
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for bit data. Each bit in the
 *  input buffer will be checked, a 0 bit will correspond to the value 0 and a
 *  1 bit will correspond to the value 1.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert (must be a multiple of 8)
 *  @param fSize  (IN)  Item packing field size in bits (must be 1)
 *  @param dSize  (IN)  Data item size in bits (must be 1)
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackBits (const void *ptr, int32_t off, int64_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(align);
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  if (sign) {
    // This uses sign-extending principles to get each bit as 0 or -1 which reflects
    // the fact that 0/-1 and not 0/+1 are the available values in a one-bit use of
    // twos-complement.
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i+=8) {
      int32_t b = (int32_t)buf[off++];
      array[offset++] = (int64_t)((b << 24) >> 31);
      array[offset++] = (int64_t)((b << 25) >> 31);
      array[offset++] = (int64_t)((b << 26) >> 31);
      array[offset++] = (int64_t)((b << 27) >> 31);
      array[offset++] = (int64_t)((b << 28) >> 31);
      array[offset++] = (int64_t)((b << 29) >> 31);
      array[offset++] = (int64_t)((b << 30) >> 31);
      array[offset++] = (int64_t)((b << 31) >> 31);
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i+=8) {
      int32_t b = (int32_t)buf[off++];
      array[offset++] = (int64_t)((b >> 7) & 0x1);
      array[offset++] = (int64_t)((b >> 6) & 0x1);
      array[offset++] = (int64_t)((b >> 5) & 0x1);
      array[offset++] = (int64_t)((b >> 4) & 0x1);
      array[offset++] = (int64_t)((b >> 3) & 0x1);
      array[offset++] = (int64_t)((b >> 2) & 0x1);
      array[offset++] = (int64_t)((b >> 1) & 0x1);
      array[offset++] = (int64_t)((b     ) & 0x1);
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for bit data. Each value in the
 *  input array will be checked, a zero value will correspond to the bit 0 and
 *  any non-zero value will correspond to the bit 1.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert (must be a multiple of 8)
 *  @param fSize  (IN)  Item packing field size in bits (must be 1)
 *  @param dSize  (IN)  Data item size in bits (must be 1)
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packBits (void *ptr, int32_t off, const int64_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(align);
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  // Pack function for signed and unsigned are identical for bit data, hence
  // 'sign' is unused here, but included in function definition for consistency.
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i+=8) {
    int32_t b = 0;
    b |= (((int64_t)array[offset++]) << 7) & 0x80;
    b |= (((int64_t)array[offset++]) << 6) & 0x40;
    b |= (((int64_t)array[offset++]) << 5) & 0x20;
    b |= (((int64_t)array[offset++]) << 4) & 0x10;
    b |= (((int64_t)array[offset++]) << 3) & 0x08;
    b |= (((int64_t)array[offset++]) << 2) & 0x04;
    b |= (((int64_t)array[offset++]) << 1) & 0x02;
    b |= (((int64_t)array[offset++])     ) & 0x01;
    buf[off++] = (int8_t)b;
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array, where the data is
 *  aligned to 32-bit word boundaries.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Bit offset into buf (must be multiple of 32 bits)
 *  @param array  (OUT) Output value array (not null)
 *  @param chan   (OUT) Output channel tag array (null if n/a)
 *  @param evt    (OUT) Output event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackIntWord32 (const void *ptr, int32_t off, int64_t *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 32 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  int32_t valMask  = (sign)? ((int32_t)-1) : (~(((int32_t)-1) << dSize));
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=4) {
    int32_t value = unpackInt(buf, off);
    array[i] = (int64_t)((value >> (32-dSize)) & valMask);
    if (chan != NULL) chan[i] = (value >> (pSize      )) & chanMask;
    if (evt  != NULL) evt[i]  = (value >> (pSize+cSize)) & evtMask;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array, where the data is
 *  aligned to 32-bit word boundaries.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input value array
 *  @param chan   (IN)  Input channel tag array (null if n/a)
 *  @param evt    (IN)  Input event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packIntWord32 (void *ptr, int32_t off, const int64_t *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 32 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  int32_t valMask  = (sign)? ((int32_t)-1) : ((int32_t)__INT64_C(0xFFFFFFFFFFFFFFFF));
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=4) {
    int32_t val   = (int32_t)(((int32_t)array[i]) & valMask);
    int32_t value = val << (32-dSize);
    if (chan != NULL) value |= (chan[i] & chanMask) << (pSize);
    if (evt  != NULL) value |= (evt[i]  & evtMask ) << (pSize+cSize);
    packInt(buf, off, value);
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array, where the data is
 *  aligned to 64-bit word boundaries.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Bit offset into buf (must be multiple of 64 bits)
 *  @param array  (OUT) Output value array (not null)
 *  @param chan   (OUT) Output channel tag array (null if n/a)
 *  @param evt    (OUT) Output event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackIntWord64 (const void *ptr, int32_t off, int64_t *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 64 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  int64_t valMask  = (sign)? ((int64_t)-1) : (~(((int64_t)-1) << dSize));
  if (dSize == 32) {
    // When using Int32/UInt32 the first 32-bits will always the the Int32/UInt32
    // value, so use 32-bit operations to improve performance
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int32_t val   = unpackInt(buf, off);
      int32_t value = unpackInt(buf, off+4);
      array[i] = (int64_t)(val & valMask);
      if (chan != NULL) chan[i] = ((int32_t)(value >> (pSize      ))) & chanMask;
      if (evt  != NULL) evt[i]  = ((int32_t)(value >> (pSize+cSize))) & evtMask;
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int64_t value = unpackLong(buf, off);
      array[i] = (int64_t)((value >> (64-dSize)) & valMask);
      if (chan != NULL) chan[i] = ((int32_t)(value >> (pSize      ))) & chanMask;
      if (evt  != NULL) evt[i]  = ((int32_t)(value >> (pSize+cSize))) & evtMask;
    }
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array, where the data is
 *  aligned to 64-bit word boundaries.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input value array
 *  @param chan   (IN)  Input channel tag array (null if n/a)
 *  @param evt    (IN)  Input event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packIntWord64 (void *ptr, int32_t off, const int64_t *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 64 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  int64_t valMask  = (sign)? ((int64_t)-1) : ((int64_t)__INT64_C(0xFFFFFFFFFFFFFFFF));
  if (dSize == 32) {
    // When using Int32/UInt32 the first 32-bits will always the the Int32/UInt32
    // value, so use 32-bit operations to improve performance
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int32_t val   = (int32_t)(((int64_t)array[i]) & valMask);
      int32_t value = 0;
      if (chan != NULL) value  = (chan[i] & chanMask) << (pSize);
      if (evt  != NULL) value |= (evt[i]  & evtMask ) << (pSize+cSize);
      packInt(buf, off,   val);
      packInt(buf, off+4, value);
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int64_t val   = (int64_t)(((int64_t)array[i]) & valMask);
      int64_t value = val << (64-dSize);
      if (chan != NULL) value |= ((int64_t)(chan[i] & chanMask)) << (pSize);
      if (evt  != NULL) value |= ((int64_t)(evt[i]  & evtMask )) << (pSize+cSize);
      packLong(buf, off, value);
    }
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array, where the data is
 *  aligned to 64-bit word boundaries.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Bit offset into buf (must be multiple of 64 bits)
 *  @param array  (OUT) Output value array (not null)
 *  @param chan   (OUT) Output channel tag array (null if n/a)
 *  @param evt    (OUT) Output event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackFloatWord64 (const void *ptr, int32_t off, int64_t *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 64 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(dSize);
  // When using Float32 the first 32-bits will always the the Float32 value, so
  // use 32-bit operations to improve performance
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=8) {
    array[i] = (int64_t)unpackFloat(buf, off);
    int32_t value = unpackInt(buf, off+4);
    if (chan != NULL) chan[i] = (value >> (pSize      )) & chanMask;
    if (evt  != NULL) evt[i]  = (value >> (pSize+cSize)) & evtMask;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array, where the data is
 *  aligned to 64-bit word boundaries.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input value array
 *  @param chan   (IN)  Input channel tag array (null if n/a)
 *  @param evt    (IN)  Input event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packFloatWord64 (void *ptr, int32_t off, const int64_t *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 64 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(dSize);
  // When using Float32 the first 32-bits will always the the Float32 value, so
  // use 32-bit operations to improve performance
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=8) {
    float val   = (float)array[i];
    int32_t   value = 0;
    if (chan != NULL) value  = (chan[i] & chanMask) << (pSize);
    if (evt  != NULL) value |= (evt[i]  & evtMask ) << (pSize+cSize);
    packFloat(buf, off,   val);
    packInt(  buf, off+4, value);
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array, where the data is
 *  aligned to 32-bit word boundaries.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Bit offset into buf (must be multiple of 32 bits)
 *  @param array  (OUT) Output value array (not null)
 *  @param chan   (OUT) Output channel tag array (null if n/a)
 *  @param evt    (OUT) Output event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackVRTFloatWord32 (const void *ptr, int32_t off, int64_t *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 32 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=4) {
    int32_t value = unpackInt(buf, off);
    int32_t bits  = (value >> (32-dSize)); // fromVRTFloat32(..) will mask off any extra bits
    array[i] = (int64_t)fromVRTFloat32(sign, expBits, dSize, bits);
    if (chan != NULL) chan[i] = (value >> (pSize      )) & chanMask;
    if (evt  != NULL) evt[i]  = (value >> (pSize+cSize)) & evtMask;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array, where the data is
 *  aligned to 32-bit word boundaries.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input value array
 *  @param chan   (IN)  Input channel tag array (null if n/a)
 *  @param evt    (IN)  Input event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packVRTFloatWord32 (void *ptr, int32_t off, const int64_t *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 32 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=4) {
    int32_t val   = toVRTFloat32(sign, expBits, dSize, (double)array[i]);
    int32_t value = val << (32-dSize);
    if (chan != NULL) value |= (chan[i] & chanMask) << (pSize);
    if (evt  != NULL) value |= (evt[i]  & evtMask ) << (pSize+cSize);
    packInt(buf, off, value);
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array, where the data is
 *  aligned to 64-bit word boundaries.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Bit offset into buf (must be multiple of 64 bits)
 *  @param array  (OUT) Output value array (not null)
 *  @param chan   (OUT) Output channel tag array (null if n/a)
 *  @param evt    (OUT) Output event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackVRTFloatWord64 (const void *ptr, int32_t off, int64_t *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 64 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  if (dSize == 32) {
    // When using Int32/UInt32 the first 32-bits will always the the Int32/UInt32
    // value, so use 32-bit operations to improve performance
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int32_t val   = unpackInt(buf, off);
      int32_t value = unpackInt(buf, off+4);
      array[i] = (int64_t)fromVRTFloat32(sign, expBits, dSize, val);
      if (chan != NULL) chan[i] = ((int32_t)(value >> (pSize      ))) & chanMask;
      if (evt  != NULL) evt[i]  = ((int32_t)(value >> (pSize+cSize))) & evtMask;
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int64_t value = unpackLong(buf, off);
      int64_t bits  = (value >> (64-dSize)); // fromVRTFloat64(..) will mask off any extra bits
      array[i] = (int64_t)fromVRTFloat64(sign, expBits, dSize, bits);
      if (chan != NULL) chan[i] = ((int32_t)(value >> (pSize      ))) & chanMask;
      if (evt  != NULL) evt[i]  = ((int32_t)(value >> (pSize+cSize))) & evtMask;
    }
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array, where the data is
 *  aligned to 64-bit word boundaries.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input value array
 *  @param chan   (IN)  Input channel tag array (null if n/a)
 *  @param evt    (IN)  Input event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packVRTFloatWord64 (void *ptr, int32_t off, const int64_t *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 64 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  if (dSize == 32) {
    // When using Int32/UInt32 the first 32-bits will always the the Int32/UInt32
    // value, so use 32-bit operations to improve performance
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int32_t val   = toVRTFloat32(sign, expBits, dSize, (double)array[i]);
      int32_t value = 0;
      if (chan != NULL) value  = (chan[i] & chanMask) << (pSize);
      if (evt  != NULL) value |= (evt[i]  & evtMask ) << (pSize+cSize);
      packInt(buf, off,   val);
      packInt(buf, off+4, value);
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int64_t val   = toVRTFloat64(sign, expBits, dSize, (double)array[i]);
      int64_t value = val << (64-dSize);
      if (chan != NULL) value |= ((int64_t)(chan[i] & chanMask)) << (pSize);
      if (evt  != NULL) value |= ((int64_t)(evt[i]  & evtMask )) << (pSize+cSize);
      packLong(buf, off, value);
    }
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array.
 *  @param ptr       (IN)  Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf (must be multiple of 32 bits)
 *  @param array     (OUT) Output value array (not null)
 *  @param chan      (OUT) Output channel tag array (null if n/a)
 *  @param evt       (OUT) Output event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackIntBuffer32 (const void *ptr, int32_t bitOffset, int64_t *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  const int32_t bitmask      = 32 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t _bits_dSize  = 32 - dSize;    // pre-compute "32 - dSize" for efficiency
  int32_t _bits_fSize  = 32 - fSize;    // pre-compute "32 - fSize" for efficiency
  int32_t _fSize_dSize = fSize - dSize;         // pre-compute "fSize - dSize" for efficiency
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      // Not enough bits remaining in this word, move to next word (skipping pad bits)
      bitOffset = (bitOffset + 32) & bitmaskInv;
    }
    int32_t bits  = unpackBits32(buf, bitOffset, fSize);
    int32_t value = bits >> _fSize_dSize;
    // sign extend as needed (otherwise leave top bits 0)
    array[i] = (sign)? (int64_t)((value << _bits_dSize) >>  _bits_dSize)
                     : (int64_t)value;
    if (evt  != NULL) evt[i]  = ((int32_t)(bits >> cSize)) & eMask;
    if (chan != NULL) chan[i] = ((int32_t)(bits         )) & cMask;
    bitOffset += fSize;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array.
 *  @param ptr       (I/O) Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf
 *  @param array     (IN)  Input value array
 *  @param chan      (IN)  Input channel tag array (null if n/a)
 *  @param evt       (IN)  Input event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void packIntBuffer32 (void *ptr, int32_t bitOffset, const int64_t *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  const int32_t bitmask      = 32 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t uSize        = fSize - dSize - eSize - cSize;
  int32_t _bits_fSize  = 32 - fSize;    // pre-compute "32 - fSize" for efficiency
  int32_t valMask      = (sign)? ((int32_t)-1) : ((int32_t)__INT64_C(0xFFFFFFFFFFFFFFFF)); // Bit mask for input value (controls sign-extend)
  if ((evt == NULL) || (eSize == 0)) {
    // Treat event tag as unused since we have nothing to insert there (this
    // will be slightly faster in the loop below).
    uSize = uSize + eSize;
    eSize = 0;
    evt   = NULL;
    if (chan == NULL) {
      // Treat channel tag as unused since we have nothing to insert there (this
      // will be slightly faster in the loop below).
      uSize = uSize + cSize;
      cSize = 0;
      chan  = NULL;
    }
  }
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  int32_t _uSize_eSize = uSize + eSize;         // pre-compute "uSize + eSize" for efficiency
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      if (true) { //TODO: make this optional
        // Not enough bits remaining in this word, move to next word (clearing pad bits)
        int32_t zSize = 32 - (bitOffset & bitmask); // bits remaining to set
        packBits32(buf, bitOffset, zSize, 0);
        bitOffset += zSize;
      }
      else {
        // Not enough bits remaining in this word, move to next word (skipping pad bits)
        bitOffset = (bitOffset + 32) & bitmaskInv;
      }
    }
    int32_t value = (int32_t)(((int32_t)array[i]) & valMask);
    value = value << _uSize_eSize;
    if (evt != NULL) value = value | (evt[i] & eMask);
    value = value << cSize;
    if (chan != NULL) value = value | (chan[i] & cMask);
    packBits32(buf, bitOffset, fSize, value);
    bitOffset += fSize;
  }
  // ==== End of Packet Pad Bits ===============================================
  // Clear out any bits remaining in the current word that would be at the end
  // of the packet, this applies to both link and processing efficient.
  if (true) { //TODO: make this optional
    int32_t xSize = 32 - (bitOffset & bitmask); // bits remaining to set
    if (xSize > 0) {
      packBits32(buf, bitOffset, xSize, 0);
      bitOffset += xSize;
    }
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array.
 *  @param ptr       (IN)  Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf (must be multiple of 64 bits)
 *  @param array     (OUT) Output value array (not null)
 *  @param chan      (OUT) Output channel tag array (null if n/a)
 *  @param evt       (OUT) Output event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackIntBuffer64 (const void *ptr, int32_t bitOffset, int64_t *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  const int32_t bitmask      = 64 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t _bits_dSize  = 64 - dSize;    // pre-compute "64 - dSize" for efficiency
  int32_t _bits_fSize  = 64 - fSize;    // pre-compute "64 - fSize" for efficiency
  int32_t _fSize_dSize = fSize - dSize;         // pre-compute "fSize - dSize" for efficiency
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      // Not enough bits remaining in this word, move to next word (skipping pad bits)
      bitOffset = (bitOffset + 32) & bitmaskInv;
    }
    int64_t bits  = unpackBits64(buf, bitOffset, fSize);
    int64_t value = bits >> _fSize_dSize;
    // sign extend as needed (otherwise leave top bits 0)
    array[i] = (sign)? (int64_t)((value << _bits_dSize) >>  _bits_dSize)
                     : (int64_t)value;
    if (evt  != NULL) evt[i]  = ((int32_t)(bits >> cSize)) & eMask;
    if (chan != NULL) chan[i] = ((int32_t)(bits         )) & cMask;
    bitOffset += fSize;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array.
 *  @param ptr       (I/O) Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf
 *  @param array     (IN)  Input value array
 *  @param chan      (IN)  Input channel tag array (null if n/a)
 *  @param evt       (IN)  Input event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void packIntBuffer64 (void *ptr, int32_t bitOffset, const int64_t *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  const int32_t bitmask      = 64 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t uSize        = fSize - dSize - eSize - cSize;
  int32_t _bits_fSize  = 64 - fSize;    // pre-compute "64 - fSize" for efficiency
  int64_t valMask      = (sign)? ((int64_t)-1) : ((int64_t)__INT64_C(0xFFFFFFFFFFFFFFFF)); // Bit mask for input value (controls sign-extend)
  if ((evt == NULL) || (eSize == 0)) {
    // Treat event tag as unused since we have nothing to insert there (this
    // will be slightly faster in the loop below).
    uSize = uSize + eSize;
    eSize = 0;
    evt   = NULL;
    if (chan == NULL) {
      // Treat channel tag as unused since we have nothing to insert there (this
      // will be slightly faster in the loop below).
      uSize = uSize + cSize;
      cSize = 0;
      chan  = NULL;
    }
  }
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  int32_t _uSize_eSize = uSize + eSize;         // pre-compute "uSize + eSize" for efficiency
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      if (true) { //TODO: make this optional
        // Not enough bits remaining in this word, move to next word (clearing pad bits)
        int32_t zSize = 64 - (bitOffset & bitmask); // bits remaining to set
        packBits64(buf, bitOffset, zSize, 0);
        bitOffset += zSize;
      }
      else {
        // Not enough bits remaining in this word, move to next word (skipping pad bits)
        bitOffset = (bitOffset + 32) & bitmaskInv;
      }
    }
    int64_t value = (int64_t)(((int64_t)array[i]) & valMask);
    value = value << _uSize_eSize;
    if (evt != NULL) value = value | (evt[i] & eMask);
    value = value << cSize;
    if (chan != NULL) value = value | (chan[i] & cMask);
    packBits64(buf, bitOffset, fSize, value);
    bitOffset += fSize;
  }
  // ==== End of Packet Pad Bits ===============================================
  // Clear out any bits remaining in the current word that would be at the end
  // of the packet, this applies to both link and processing efficient.
  if (true) { //TODO: make this optional
    int32_t xSize = 64 - (bitOffset & bitmask); // bits remaining to set
    if (xSize > 0) {
      packBits64(buf, bitOffset, xSize, 0);
      bitOffset += xSize;
    }
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array.
 *  @param ptr       (IN)  Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf (must be multiple of 64 bits)
 *  @param array     (OUT) Output value array (not null)
 *  @param chan      (OUT) Output channel tag array (null if n/a)
 *  @param evt       (OUT) Output event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackFloatBuffer64 (const void *ptr, int32_t bitOffset, int64_t *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  const int32_t bitmask      = 64 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t _bits_fSize  = 64 - fSize;    // pre-compute "64 - fSize" for efficiency
  int32_t _fSize_dSize = fSize - dSize;         // pre-compute "fSize - dSize" for efficiency
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      // Not enough bits remaining in this word, move to next word (skipping pad bits)
      bitOffset = (bitOffset + 32) & bitmaskInv;
    }
    int64_t bits  = unpackBits64(buf, bitOffset, fSize);
    int64_t value = bits >> _fSize_dSize;
    UNUSED_VARIABLE(sign);
    array[i] = (int64_t)intBitsToFloat((int32_t)value);
    if (evt  != NULL) evt[i]  = ((int32_t)(bits >> cSize)) & eMask;
    if (chan != NULL) chan[i] = ((int32_t)(bits         )) & cMask;
    bitOffset += fSize;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array.
 *  @param ptr       (I/O) Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf
 *  @param array     (IN)  Input value array
 *  @param chan      (IN)  Input channel tag array (null if n/a)
 *  @param evt       (IN)  Input event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void packFloatBuffer64 (void *ptr, int32_t bitOffset, const int64_t *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  const int32_t bitmask      = 64 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t uSize        = fSize - dSize - eSize - cSize;
  int32_t _bits_fSize  = 64 - fSize;    // pre-compute "64 - fSize" for efficiency
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  if ((evt == NULL) || (eSize == 0)) {
    // Treat event tag as unused since we have nothing to insert there (this
    // will be slightly faster in the loop below).
    uSize = uSize + eSize;
    eSize = 0;
    evt   = NULL;
    if (chan == NULL) {
      // Treat channel tag as unused since we have nothing to insert there (this
      // will be slightly faster in the loop below).
      uSize = uSize + cSize;
      cSize = 0;
      chan  = NULL;
    }
  }
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  int32_t _uSize_eSize = uSize + eSize;         // pre-compute "uSize + eSize" for efficiency
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      if (true) { //TODO: make this optional
        // Not enough bits remaining in this word, move to next word (clearing pad bits)
        int32_t zSize = 64 - (bitOffset & bitmask); // bits remaining to set
        packBits64(buf, bitOffset, zSize, 0);
        bitOffset += zSize;
      }
      else {
        // Not enough bits remaining in this word, move to next word (skipping pad bits)
        bitOffset = (bitOffset + 32) & bitmaskInv;
      }
    }
    int64_t value = (int64_t)floatToRawIntBits((float)array[i]);
    value = value << _uSize_eSize;
    if (evt != NULL) value = value | (evt[i] & eMask);
    value = value << cSize;
    if (chan != NULL) value = value | (chan[i] & cMask);
    packBits64(buf, bitOffset, fSize, value);
    bitOffset += fSize;
  }
  // ==== End of Packet Pad Bits ===============================================
  // Clear out any bits remaining in the current word that would be at the end
  // of the packet, this applies to both link and processing efficient.
  if (true) { //TODO: make this optional
    int32_t xSize = 64 - (bitOffset & bitmask); // bits remaining to set
    if (xSize > 0) {
      packBits64(buf, bitOffset, xSize, 0);
      bitOffset += xSize;
    }
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array.
 *  @param ptr       (IN)  Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf (must be multiple of 32 bits)
 *  @param array     (OUT) Output value array (not null)
 *  @param chan      (OUT) Output channel tag array (null if n/a)
 *  @param evt       (OUT) Output event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackVRTFloatBuffer32 (const void *ptr, int32_t bitOffset, int64_t *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  const char *buf = (const char*)ptr;
  const int32_t bitmask      = 32 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t _bits_fSize  = 32 - fSize;    // pre-compute "32 - fSize" for efficiency
  int32_t _fSize_dSize = fSize - dSize;         // pre-compute "fSize - dSize" for efficiency
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      // Not enough bits remaining in this word, move to next word (skipping pad bits)
      bitOffset = (bitOffset + 32) & bitmaskInv;
    }
    int32_t bits  = unpackBits32(buf, bitOffset, fSize);
    int32_t value = bits >> _fSize_dSize;
    array[i] = (int64_t)fromVRTFloat32(sign, expBits, dSize, value);
    if (evt  != NULL) evt[i]  = ((int32_t)(bits >> cSize)) & eMask;
    if (chan != NULL) chan[i] = ((int32_t)(bits         )) & cMask;
    bitOffset += fSize;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array.
 *  @param ptr       (I/O) Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf
 *  @param array     (IN)  Input value array
 *  @param chan      (IN)  Input channel tag array (null if n/a)
 *  @param evt       (IN)  Input event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void packVRTFloatBuffer32 (void *ptr, int32_t bitOffset, const int64_t *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  char *buf = (char*)ptr;
  const int32_t bitmask      = 32 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t uSize        = fSize - dSize - eSize - cSize;
  int32_t _bits_fSize  = 32 - fSize;    // pre-compute "32 - fSize" for efficiency
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  if ((evt == NULL) || (eSize == 0)) {
    // Treat event tag as unused since we have nothing to insert there (this
    // will be slightly faster in the loop below).
    uSize = uSize + eSize;
    eSize = 0;
    evt   = NULL;
    if (chan == NULL) {
      // Treat channel tag as unused since we have nothing to insert there (this
      // will be slightly faster in the loop below).
      uSize = uSize + cSize;
      cSize = 0;
      chan  = NULL;
    }
  }
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  int32_t _uSize_eSize = uSize + eSize;         // pre-compute "uSize + eSize" for efficiency
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      if (true) { //TODO: make this optional
        // Not enough bits remaining in this word, move to next word (clearing pad bits)
        int32_t zSize = 32 - (bitOffset & bitmask); // bits remaining to set
        packBits32(buf, bitOffset, zSize, 0);
        bitOffset += zSize;
      }
      else {
        // Not enough bits remaining in this word, move to next word (skipping pad bits)
        bitOffset = (bitOffset + 32) & bitmaskInv;
      }
    }
    int32_t value = toVRTFloat32(sign, expBits, dSize, (double)array[i]);
    value = value << _uSize_eSize;
    if (evt != NULL) value = value | (evt[i] & eMask);
    value = value << cSize;
    if (chan != NULL) value = value | (chan[i] & cMask);
    packBits32(buf, bitOffset, fSize, value);
    bitOffset += fSize;
  }
  // ==== End of Packet Pad Bits ===============================================
  // Clear out any bits remaining in the current word that would be at the end
  // of the packet, this applies to both link and processing efficient.
  if (true) { //TODO: make this optional
    int32_t xSize = 32 - (bitOffset & bitmask); // bits remaining to set
    if (xSize > 0) {
      packBits32(buf, bitOffset, xSize, 0);
      bitOffset += xSize;
    }
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array.
 *  @param ptr       (IN)  Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf (must be multiple of 64 bits)
 *  @param array     (OUT) Output value array (not null)
 *  @param chan      (OUT) Output channel tag array (null if n/a)
 *  @param evt       (OUT) Output event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackVRTFloatBuffer64 (const void *ptr, int32_t bitOffset, int64_t *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  const char *buf = (const char*)ptr;
  const int32_t bitmask      = 64 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t _bits_fSize  = 64 - fSize;    // pre-compute "64 - fSize" for efficiency
  int32_t _fSize_dSize = fSize - dSize;         // pre-compute "fSize - dSize" for efficiency
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      // Not enough bits remaining in this word, move to next word (skipping pad bits)
      bitOffset = (bitOffset + 32) & bitmaskInv;
    }
    int64_t bits  = unpackBits64(buf, bitOffset, fSize);
    int64_t value = bits >> _fSize_dSize;
    array[i] = (int64_t)fromVRTFloat64(sign, expBits, dSize, value);
    if (evt  != NULL) evt[i]  = ((int32_t)(bits >> cSize)) & eMask;
    if (chan != NULL) chan[i] = ((int32_t)(bits         )) & cMask;
    bitOffset += fSize;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array.
 *  @param ptr       (I/O) Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf
 *  @param array     (IN)  Input value array
 *  @param chan      (IN)  Input channel tag array (null if n/a)
 *  @param evt       (IN)  Input event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void packVRTFloatBuffer64 (void *ptr, int32_t bitOffset, const int64_t *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  char *buf = (char*)ptr;
  const int32_t bitmask      = 64 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t uSize        = fSize - dSize - eSize - cSize;
  int32_t _bits_fSize  = 64 - fSize;    // pre-compute "64 - fSize" for efficiency
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  if ((evt == NULL) || (eSize == 0)) {
    // Treat event tag as unused since we have nothing to insert there (this
    // will be slightly faster in the loop below).
    uSize = uSize + eSize;
    eSize = 0;
    evt   = NULL;
    if (chan == NULL) {
      // Treat channel tag as unused since we have nothing to insert there (this
      // will be slightly faster in the loop below).
      uSize = uSize + cSize;
      cSize = 0;
      chan  = NULL;
    }
  }
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  int32_t _uSize_eSize = uSize + eSize;         // pre-compute "uSize + eSize" for efficiency
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      if (true) { //TODO: make this optional
        // Not enough bits remaining in this word, move to next word (clearing pad bits)
        int32_t zSize = 64 - (bitOffset & bitmask); // bits remaining to set
        packBits64(buf, bitOffset, zSize, 0);
        bitOffset += zSize;
      }
      else {
        // Not enough bits remaining in this word, move to next word (skipping pad bits)
        bitOffset = (bitOffset + 32) & bitmaskInv;
      }
    }
    int64_t value = toVRTFloat64(sign, expBits, dSize, (double)array[i]);
    value = value << _uSize_eSize;
    if (evt != NULL) value = value | (evt[i] & eMask);
    value = value << cSize;
    if (chan != NULL) value = value | (chan[i] & cMask);
    packBits64(buf, bitOffset, fSize, value);
    bitOffset += fSize;
  }
  // ==== End of Packet Pad Bits ===============================================
  // Clear out any bits remaining in the current word that would be at the end
  // of the packet, this applies to both link and processing efficient.
  if (true) { //TODO: make this optional
    int32_t xSize = 64 - (bitOffset & bitmask); // bits remaining to set
    if (xSize > 0) {
      packBits64(buf, bitOffset, xSize, 0);
      bitOffset += xSize;
    }
  }
}
#if NOT_USING_JNI
int64_t* vrt::PackUnpack::unpackAsLong (PayloadFormat pf, const void *ptr, int32_t off, int64_t *array, int32_t *chan, int32_t *evt, int32_t length) {
  const char *buf = (const char*)ptr;
  if (!pf.isValid()) {
    throw VRTException(pf.getValid());
  }
  int32_t            fSize  = pf.getItemPackingFieldSize();
  int32_t            dSize  = pf.getDataItemSize();
  int32_t            eSize  = pf.getEventTagSize();
  int32_t            cSize  = pf.getChannelTagSize();
  int32_t            xSize  = fSize; // effective fSize
  bool        proc   = pf.isProcessingEfficient();
  bool        sign   = pf.isSigned();
  DataItemFormat format = pf.getDataItemFormat();
  bool        noTags;
  if (eSize == 0) evt  = NULL; // no event tags, ignore any array passed in
  if (cSize == 0) chan = NULL; // no channel tags, ignore any array passed in
  noTags = ((chan == NULL) && (evt == NULL)) || (fSize == dSize);
  if (proc) {
         if (fSize >= 33) xSize = 64;
    else if (fSize >= 17) xSize = 32;
    if (noTags) fSize = xSize; // Use faster field size if ignoring Event & Channel Tags
  }
  switch (format) {
    case DataItemFormat_SignedInt:   // FALLTHROUGH
    case DataItemFormat_UnsignedInt:
      // ---- Try fast methods -------------------------------------------------
      if (noTags) {
        switch (fSize) {
          case  8: unpackBytes(  buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
          case 16: unpackShorts( buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
          case 24: unpackInt24s( buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
          case 32: unpackInts(   buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
          case 64: unpackLongs(  buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
        }
      }
      if (fSize == dSize) { // implicit noTags=true in this case
        switch (fSize) {
          case  1: if ((length % 8) == 0) { unpackBits(   buf, off, array, 0, length, fSize, dSize, proc, sign); return array; } break;
          case  4: if ((length % 2) == 0) { unpackNibbles(buf, off, array, 0, length, fSize, dSize, proc, sign); return array; } break;
          case 12: if ((length % 2) == 0) { unpackInt12s( buf, off, array, 0, length, fSize, dSize, proc, sign); return array; } break;
        }
      }
      if (xSize == 32) {
        unpackIntWord32(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return array;
      }
      if (xSize == 64) {
        unpackIntWord64(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return array;
      }
      // ---- Fall back to slow method -----------------------------------------
      if (fSize <= 32) {
        unpackIntBuffer32(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return array;
      }
      else {
        unpackIntBuffer64(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return array;
      }
    case DataItemFormat_Float:
      // ---- Try fast methods -------------------------------------------------
      if (fSize == dSize) { // implicit noTags=true in this case
        unpackFloats(buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
      }
      if (xSize == 64) {
        unpackFloatWord64(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return array;
      }
      // ---- Fall back to slow method -----------------------------------------
      unpackFloatBuffer64(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return array;
    case DataItemFormat_Double:
      // ---- Only fast method for double (yea!) -------------------------------
      unpackDoubles(buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
    default: // VRTFloat Types
      // ---- Try fast methods -------------------------------------------------
      if (xSize == 32) {
        unpackVRTFloatWord32(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return array;
      }
      if (xSize == 64) {
        unpackVRTFloatWord64(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return array;
      }
      // ---- Fall back to slow method -----------------------------------------
      if (fSize <= 32) {
        unpackVRTFloatBuffer32(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return array;
      }
      else {
        unpackVRTFloatBuffer64(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return array;
      }
  }
}
void vrt::PackUnpack::packAsLong (PayloadFormat pf, void *ptr, int32_t off, const int64_t *array, const int32_t *chan, const int32_t *evt, int32_t length) {
  char *buf = (char*)ptr;
  if (!pf.isValid()) {
    throw VRTException(pf.getValid());
  }
  int32_t            fSize  = pf.getItemPackingFieldSize();
  int32_t            dSize  = pf.getDataItemSize();
  int32_t            eSize  = pf.getEventTagSize();
  int32_t            cSize  = pf.getChannelTagSize();
  int32_t            xSize  = fSize; // effective fSize
  bool        proc   = pf.isProcessingEfficient();
  bool        sign   = pf.isSigned();
  DataItemFormat format = pf.getDataItemFormat();
  bool        noTags;
  if (eSize == 0) evt  = NULL; // no event tags, ignore any array passed in
  if (cSize == 0) chan = NULL; // no channel tags, ignore any array passed in
  noTags = ((chan == NULL) && (evt == NULL)) || (fSize == dSize);
  if (proc) {
         if (fSize >= 33) xSize = 64;
    else if (fSize >= 17) xSize = 32;
    if (noTags) fSize = xSize; // Use faster field size if ignoring Event & Channel Tags
  }
  switch (format) {
    case DataItemFormat_SignedInt:   // FALLTHROUGH
    case DataItemFormat_UnsignedInt:
      // ---- Try fast methods -------------------------------------------------
      if (noTags) {
        switch (fSize) {
          case  8: packBytes(  buf, off, array, 0, length, fSize, dSize, proc, sign); return;
          case 16: packShorts( buf, off, array, 0, length, fSize, dSize, proc, sign); return;
          case 24: packInt24s( buf, off, array, 0, length, fSize, dSize, proc, sign); return;
          case 32: packInts(   buf, off, array, 0, length, fSize, dSize, proc, sign); return;
          case 64: packLongs(  buf, off, array, 0, length, fSize, dSize, proc, sign); return;
        }
      }
      if (fSize == dSize) { // implicit noTags=true in this case
        switch (fSize) {
          case  1: if ((length % 8) == 0) { packBits(   buf, off, array, 0, length, fSize, dSize, proc, sign); return; } break;
          case  4: if ((length % 2) == 0) { packNibbles(buf, off, array, 0, length, fSize, dSize, proc, sign); return; } break;
          case 12: if ((length % 2) == 0) { packInt12s( buf, off, array, 0, length, fSize, dSize, proc, sign); return; } break;
        }
      }
      if (xSize == 32) {
        packIntWord32(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return;
      }
      if (xSize == 64) {
        packIntWord64(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return;
      }
      // ---- Fall back to slow method -----------------------------------------
      if (fSize <= 32) {
        packIntBuffer32(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return;
      }
      else {
        packIntBuffer64(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return;
      }
    case DataItemFormat_Float:
      // ---- Try fast methods -------------------------------------------------
      if (fSize == dSize) { // implicit noTags=true in this case
        packFloats(buf, off, array, 0, length, fSize, dSize, proc, sign); return;
      }
      if (xSize == 64) {
        packFloatWord64(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return;
      }
      // ---- Fall back to slow method -----------------------------------------
      packFloatBuffer64(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return;
    case DataItemFormat_Double:
      // ---- Only fast method for double (yea!) -------------------------------
      packDoubles(buf, off, array, 0, length, fSize, dSize, proc, sign); return;
    default: // VRTFloat Types
      // ---- Try fast methods -------------------------------------------------
      if (xSize == 32) {
        packVRTFloatWord32(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return;
      }
      if (xSize == 64) {
        packVRTFloatWord64(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return;
      }
      // ---- Fall back to slow method -----------------------------------------
      if (fSize <= 32) {
        packVRTFloatBuffer32(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return;
      }
      else {
        packVRTFloatBuffer64(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return;
      }
  }
}
#endif /* NOT_USING_JNI */
/** <b>Internal Use Only:</b> Fast unpack method for double data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits (must be 64)
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed? (must be true)
 */
static __attribute__((flatten)) void unpackDoubles (const void *ptr, int32_t off, int32_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  // All floating-point values are signed, so 'sign' is unused, but is included
  // in function definition for consistency.
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(align);
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  // When using Double data, it is required that dSize=64 and fSize=64 so we can
  // skip some of the normal checks we would otherwise do here.
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=8) {
    array[offset++] = (int32_t)unpackDouble(buf, off);
  }
}
/** <b>Internal Use Only:</b>
 *  Fast pack method for double data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits (must be 64)
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed? (must be true)
 */
static __attribute__((flatten)) void packDoubles (void *ptr, int32_t off, const int32_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  // All floating-point values are signed, so 'sign' is unused, but is included
  // in function definition for consistency.
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(align);
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  // When using Double data, it is required that dSize=64 and fSize=64 so we can
  // skip some of the normal checks we would otherwise do here.
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=8) {
    packDouble(buf, off, (double)array[offset++]);
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for float data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits (must be 32)
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed? (must be true)
 */
static __attribute__((flatten)) void unpackFloats (const void *ptr, int32_t off, int32_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  // All floating-point values are signed, so 'sign' is unused, but is included
  // in function definition for consistency.
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(dSize);
  int32_t delta; // int8_t offset delta
  if (align) delta = (fSize <= 32)? 4 : 8;
  else       delta = fSize / 8;
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=delta) {
    array[offset++] = (int32_t)unpackFloat(buf, off);
  }
}
/** <b>Internal Use Only:</b>
 *  Fast pack method for float data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits (must be 32)
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed? (must be true)
 */
static __attribute__((flatten)) void packFloats (void *ptr, int32_t off, const int32_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  // All floating-point values are signed, so 'sign' is unused, but is included
  // in function definition for consistency.
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(dSize);
  int32_t delta; // int8_t offset delta
  if (align) delta = (fSize <= 32)? 4 : 8;
  else       delta = fSize / 8;
  if (delta == 4) {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=4) {
      packFloat(buf, off, (float)array[offset++]);
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++) {
      packFloat(buf, off, (float)array[offset++]);
      off += 4;
      _Intel_Pragma("loop_count min=1, max=4")
      for (int32_t j = 4; j < delta; j++) {
        buf[off++] = 0; // set unused bits to zero
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed int64_t data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void unpackLongs (const void *ptr, int32_t off, int32_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  int32_t delta = (align)? ((fSize <= 32)? 4 : 8) : (fSize/8); // int8_t offset delta
  if (sign) {
    if (dSize == 64) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int32_t)unpackLong(buf, off);
      }
    }
    else {
      int32_t shift  = 64 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int32_t)(unpackLong(buf, off) >> shift);
      }
    }
  }
  else {
    if (dSize == 64) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int32_t)(unpackLong(buf, off) & __INT64_C(0xFFFFFFFFFFFFFFFF));
      }
    }
    else {
      int32_t shift  = 64 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int32_t)((unpackLong(buf, off) & __INT64_C(0xFFFFFFFFFFFFFFFF)) >> shift);
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for unsigned int64_t data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void packLongs (void *ptr, int32_t off, const int32_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  int32_t delta = (align)? ((fSize <= 32)? 4 : 8) : (fSize/8); // int8_t offset delta
  int64_t valMask = (sign)? ((int64_t)-1) : ((int64_t)__INT64_C(0xFFFFFFFF)); // Bit mask for input value (controls sign-extend)
  if (dSize == 64) {
    if (delta == 8) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=8) {
        int64_t val = (int64_t)(((int64_t)array[offset++]) & valMask);
        packLong(buf, off, val);
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int64_t val = (int64_t)(((int64_t)array[offset++]) & valMask);
        packLong(buf, off, val);
        off += 8;
        _Intel_Pragma("loop_count min=1, max=0")
        for (int32_t j = 8; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
  else {
    int32_t shift = 64 - dSize;
    if (delta == 8) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=8) {
        int64_t val = (int64_t)(((int64_t)array[offset++]) & valMask);
        packLong(buf, off, (int64_t)(val << shift));
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int64_t val = (int64_t)(((int64_t)array[offset++]) & valMask);
        packLong(buf, off, (int64_t)(val << shift));
        off += 8;
        _Intel_Pragma("loop_count min=1, max=0")
        for (int32_t j = 8; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed int32_t data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void unpackInts (const void *ptr, int32_t off, int32_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
#if (4 == 1) || (BYTE_ORDER == BIG_ENDIAN)
  if ((fSize == 32) && (dSize == 32)) {
    // Fast array copy
    memcpy(&array[offset], &buf[off], length*4);
    return;
  }
#endif
  int32_t delta = (align)? ((fSize <= 32)? 4 : 8) : (fSize/8); // int8_t offset delta
  if (sign) {
    if (dSize == 32) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int32_t)unpackInt(buf, off);
      }
    }
    else {
      int32_t shift  = 32 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int32_t)(unpackInt(buf, off) >> shift);
      }
    }
  }
  else {
    if (dSize == 32) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int32_t)(unpackInt(buf, off) & 0xFFFFFFFF);
      }
    }
    else {
      int32_t shift  = 32 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int32_t)((unpackInt(buf, off) & 0xFFFFFFFF) >> shift);
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for unsigned int32_t data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void packInts (void *ptr, int32_t off, const int32_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
#if (4 == 1) || (BYTE_ORDER == BIG_ENDIAN)
  if ((fSize == 32) && (dSize == 32)) {
    // Fast array copy
    memcpy(&buf[off], &array[offset], length*4);
    return;
  }
#endif
  int32_t delta = (align)? ((fSize <= 32)? 4 : 8) : (fSize/8); // int8_t offset delta
  int32_t valMask = (sign)? ((int32_t)-1) : ((int32_t)__INT64_C(0xFFFFFFFF)); // Bit mask for input value (controls sign-extend)
  if (dSize == 32) {
    if (delta == 4) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=4) {
        int32_t val = (int32_t)(((int32_t)array[offset++]) & valMask);
        packInt(buf, off, val);
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int32_t val = (int32_t)(((int32_t)array[offset++]) & valMask);
        packInt(buf, off, val);
        off += 4;
        _Intel_Pragma("loop_count min=1, max=4")
        for (int32_t j = 4; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
  else {
    int32_t shift = 32 - dSize;
    if (delta == 4) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=4) {
        int32_t val = (int32_t)(((int32_t)array[offset++]) & valMask);
        packInt(buf, off, (int32_t)(val << shift));
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int32_t val = (int32_t)(((int32_t)array[offset++]) & valMask);
        packInt(buf, off, (int32_t)(val << shift));
        off += 4;
        _Intel_Pragma("loop_count min=1, max=4")
        for (int32_t j = 4; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed int32_t data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void unpackInt24s (const void *ptr, int32_t off, int32_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  int32_t delta = (align)? ((fSize <= 32)? 4 : 8) : (fSize/8); // int8_t offset delta
  if (sign) {
    if (dSize == 24) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int32_t)unpackInt24(buf, off);
      }
    }
    else {
      int32_t shift  = 24 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int32_t)(unpackInt24(buf, off) >> shift);
      }
    }
  }
  else {
    if (dSize == 24) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int32_t)(unpackInt24(buf, off) & 0xFFFFFF);
      }
    }
    else {
      int32_t shift  = 24 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int32_t)((unpackInt24(buf, off) & 0xFFFFFF) >> shift);
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for unsigned int32_t data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void packInt24s (void *ptr, int32_t off, const int32_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  int32_t delta = (align)? ((fSize <= 32)? 4 : 8) : (fSize/8); // int8_t offset delta
  int32_t valMask = (sign)? ((int32_t)-1) : ((int32_t)__INT64_C(0xFFFFFFFF)); // Bit mask for input value (controls sign-extend)
  if (dSize == 24) {
    if (delta == 3) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=3) {
        int32_t val = (int32_t)(((int32_t)array[offset++]) & valMask);
        packInt24(buf, off, val);
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int32_t val = (int32_t)(((int32_t)array[offset++]) & valMask);
        packInt24(buf, off, val);
        off += 3;
        _Intel_Pragma("loop_count min=1, max=5")
        for (int32_t j = 3; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
  else {
    int32_t shift = 24 - dSize;
    if (delta == 3) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=3) {
        int32_t val = (int32_t)(((int32_t)array[offset++]) & valMask);
        packInt24(buf, off, (int32_t)(val << shift));
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int32_t val = (int32_t)(((int32_t)array[offset++]) & valMask);
        packInt24(buf, off, (int32_t)(val << shift));
        off += 3;
        _Intel_Pragma("loop_count min=1, max=5")
        for (int32_t j = 3; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed int16_t data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void unpackShorts (const void *ptr, int32_t off, int32_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  int32_t delta = fSize / 8; // int8_t offset delta
  if (sign) {
    if (dSize == 16) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int32_t)unpackShort(buf, off);
      }
    }
    else {
      int32_t shift  = 16 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int32_t)(unpackShort(buf, off) >> shift);
      }
    }
  }
  else {
    if (dSize == 16) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int32_t)(unpackShort(buf, off) & 0xFFFF);
      }
    }
    else {
      int32_t shift  = 16 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int32_t)((unpackShort(buf, off) & 0xFFFF) >> shift);
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for unsigned int16_t data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void packShorts (void *ptr, int32_t off, const int32_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(align);
  int32_t delta = fSize / 8; // int8_t offset delta
  int16_t valMask = (sign)? ((int16_t)-1) : ((int16_t)__INT64_C(0xFFFFFFFF)); // Bit mask for input value (controls sign-extend)
  if (dSize == 16) {
    if (delta == 2) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=2) {
        int16_t val = (int16_t)(((int16_t)array[offset++]) & valMask);
        packShort(buf, off, val);
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int16_t val = (int16_t)(((int16_t)array[offset++]) & valMask);
        packShort(buf, off, val);
        off += 2;
        _Intel_Pragma("loop_count min=1, max=6")
        for (int32_t j = 2; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
  else {
    int32_t shift = 16 - dSize;
    if (delta == 2) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=2) {
        int16_t val = (int16_t)(((int16_t)array[offset++]) & valMask);
        packShort(buf, off, (int16_t)(val << shift));
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int16_t val = (int16_t)(((int16_t)array[offset++]) & valMask);
        packShort(buf, off, (int16_t)(val << shift));
        off += 2;
        _Intel_Pragma("loop_count min=1, max=6")
        for (int32_t j = 2; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed Int12 data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert (must be a multiple of 2)
 *  @param fSize  (IN)  Item packing field size in bits (must be 12)
 *  @param dSize  (IN)  Data item size in bits (must be 12)
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackInt12s (const void *ptr, int32_t off, int32_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  int32_t skip = (align)? 1 : 0;
  if (sign) {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i+=2) {
      int32_t a = (int32_t)buf[off++];
      int32_t b = (int32_t)buf[off++];
      int32_t c = (int32_t)buf[off++];
      int32_t x = (a << 4) | ((b >> 4) & 0x000F);
      int32_t y = (b << 8) | ((c     ) & 0x00FF);
      array[offset++] = (int32_t)((x << 20) >> 20); // sign extend
      array[offset++] = (int32_t)((y << 20) >> 20); // sign extend
      off += skip; // skip extra int8_t in processing-efficient mode
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i+=2) {
      int32_t a = (int32_t)buf[off++];
      int32_t b = (int32_t)buf[off++];
      int32_t c = (int32_t)buf[off++];
      int32_t x = (a << 4) | ((b >> 4) & 0x000F);
      int32_t y = (b << 8) | ((c     ) & 0x00FF);
      array[offset++] = (int32_t)(x & 0x0FFF); // clear high-order bits
      array[offset++] = (int32_t)(y & 0x0FFF); // clear high-order bits
      off += skip; // skip extra int8_t in processing-efficient mode
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for signed Int12 data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert (must be a multiple of 2)
 *  @param fSize  (IN)  Item packing field size in bits (must be 12)
 *  @param dSize  (IN)  Data item size in bits (must be 12)
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packInt12s (void *ptr, int32_t off, const int32_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  int32_t skip = (align)?  1 : 0;
  int32_t mask = (sign )? -1 : ((int32_t)__INT64_C(0xFFFFFFFF));
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i+=2) {
    int32_t first  = ((int32_t)array[offset++]) & mask;
    int32_t second = ((int32_t)array[offset++]) & mask;
    buf[off++] = (int8_t)(first >> 4);
    buf[off++] = (int8_t)((first << 4) | ((second >> 8) & 0x0F));
    buf[off++] = (int8_t)(second);
    off += skip; // skip extra int8_t in processing-efficient mode
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed int8_t data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void unpackBytes (const void *ptr, int32_t off, int32_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  int32_t delta = fSize / 8; // int8_t offset delta
  if (sign) {
    if (dSize == 8) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int32_t)unpackByte(buf, off);
      }
    }
    else {
      int32_t shift  = 8 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int32_t)(unpackByte(buf, off) >> shift);
      }
    }
  }
  else {
    if (dSize == 8) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int32_t)(unpackByte(buf, off) & 0xFF);
      }
    }
    else {
      int32_t shift  = 8 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int32_t)((unpackByte(buf, off) & 0xFF) >> shift);
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for unsigned int8_t data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void packBytes (void *ptr, int32_t off, const int32_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(align);
  int32_t delta = fSize / 8; // int8_t offset delta
  int8_t valMask = (sign)? ((int8_t)-1) : ((int8_t)__INT64_C(0xFFFFFFFF)); // Bit mask for input value (controls sign-extend)
  if (dSize == 8) {
    if (delta == 1) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=1) {
        int8_t val = (int8_t)(((int8_t)array[offset++]) & valMask);
        packByte(buf, off, val);
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int8_t val = (int8_t)(((int8_t)array[offset++]) & valMask);
        packByte(buf, off, val);
        off += 1;
        _Intel_Pragma("loop_count min=1, max=7")
        for (int32_t j = 1; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
  else {
    int32_t shift = 8 - dSize;
    if (delta == 1) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=1) {
        int8_t val = (int8_t)(((int8_t)array[offset++]) & valMask);
        packByte(buf, off, (int8_t)(val << shift));
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int8_t val = (int8_t)(((int8_t)array[offset++]) & valMask);
        packByte(buf, off, (int8_t)(val << shift));
        off += 1;
        _Intel_Pragma("loop_count min=1, max=7")
        for (int32_t j = 1; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed Int4 (nibble) data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert (must be a multiple of 2)
 *  @param fSize  (IN)  Item packing field size in bits (must be 4)
 *  @param dSize  (IN)  Data item size in bits (must be 4)
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackNibbles (const void *ptr, int32_t off, int32_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  if (sign) {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i+=2) {
      int32_t b = (int32_t)buf[off++];
      array[offset++] = (int32_t)((b << 24) >> 28); // shift & sign extend
      array[offset++] = (int32_t)((b << 28) >> 28); // shift & sign extend
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i+=2) {
      int32_t b = (int32_t)buf[off++];
      array[offset++] = (int32_t)((b >> 4) & 0xF);
      array[offset++] = (int32_t)((b     ) & 0xF);
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for signed Int4 (nibble) data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert (must be a multiple of 2)
 *  @param fSize  (IN)  Item packing field size in bits (must be 4)
 *  @param dSize  (IN)  Data item size in bits (must be 4)
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packNibbles (void *ptr, int32_t off, const int32_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(align);
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  // Pack function for signed and unsigned are identical for nibble data, hence
  // 'sign' is unused here, but included in function definition for consistency.
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i+=2) {
    int32_t first  = (int32_t)array[offset++];
    int32_t second = (int32_t)array[offset++];
    buf[off++] = (int8_t)((first << 4) | (second & 0xF));
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for bit data. Each bit in the
 *  input buffer will be checked, a 0 bit will correspond to the value 0 and a
 *  1 bit will correspond to the value 1.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert (must be a multiple of 8)
 *  @param fSize  (IN)  Item packing field size in bits (must be 1)
 *  @param dSize  (IN)  Data item size in bits (must be 1)
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackBits (const void *ptr, int32_t off, int32_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(align);
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  if (sign) {
    // This uses sign-extending principles to get each bit as 0 or -1 which reflects
    // the fact that 0/-1 and not 0/+1 are the available values in a one-bit use of
    // twos-complement.
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i+=8) {
      int32_t b = (int32_t)buf[off++];
      array[offset++] = (int32_t)((b << 24) >> 31);
      array[offset++] = (int32_t)((b << 25) >> 31);
      array[offset++] = (int32_t)((b << 26) >> 31);
      array[offset++] = (int32_t)((b << 27) >> 31);
      array[offset++] = (int32_t)((b << 28) >> 31);
      array[offset++] = (int32_t)((b << 29) >> 31);
      array[offset++] = (int32_t)((b << 30) >> 31);
      array[offset++] = (int32_t)((b << 31) >> 31);
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i+=8) {
      int32_t b = (int32_t)buf[off++];
      array[offset++] = (int32_t)((b >> 7) & 0x1);
      array[offset++] = (int32_t)((b >> 6) & 0x1);
      array[offset++] = (int32_t)((b >> 5) & 0x1);
      array[offset++] = (int32_t)((b >> 4) & 0x1);
      array[offset++] = (int32_t)((b >> 3) & 0x1);
      array[offset++] = (int32_t)((b >> 2) & 0x1);
      array[offset++] = (int32_t)((b >> 1) & 0x1);
      array[offset++] = (int32_t)((b     ) & 0x1);
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for bit data. Each value in the
 *  input array will be checked, a zero value will correspond to the bit 0 and
 *  any non-zero value will correspond to the bit 1.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert (must be a multiple of 8)
 *  @param fSize  (IN)  Item packing field size in bits (must be 1)
 *  @param dSize  (IN)  Data item size in bits (must be 1)
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packBits (void *ptr, int32_t off, const int32_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(align);
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  // Pack function for signed and unsigned are identical for bit data, hence
  // 'sign' is unused here, but included in function definition for consistency.
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i+=8) {
    int32_t b = 0;
    b |= (((int32_t)array[offset++]) << 7) & 0x80;
    b |= (((int32_t)array[offset++]) << 6) & 0x40;
    b |= (((int32_t)array[offset++]) << 5) & 0x20;
    b |= (((int32_t)array[offset++]) << 4) & 0x10;
    b |= (((int32_t)array[offset++]) << 3) & 0x08;
    b |= (((int32_t)array[offset++]) << 2) & 0x04;
    b |= (((int32_t)array[offset++]) << 1) & 0x02;
    b |= (((int32_t)array[offset++])     ) & 0x01;
    buf[off++] = (int8_t)b;
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array, where the data is
 *  aligned to 32-bit word boundaries.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Bit offset into buf (must be multiple of 32 bits)
 *  @param array  (OUT) Output value array (not null)
 *  @param chan   (OUT) Output channel tag array (null if n/a)
 *  @param evt    (OUT) Output event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackIntWord32 (const void *ptr, int32_t off, int32_t *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 32 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  int32_t valMask  = (sign)? ((int32_t)-1) : (~(((int32_t)-1) << dSize));
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=4) {
    int32_t value = unpackInt(buf, off);
    array[i] = (int32_t)((value >> (32-dSize)) & valMask);
    if (chan != NULL) chan[i] = (value >> (pSize      )) & chanMask;
    if (evt  != NULL) evt[i]  = (value >> (pSize+cSize)) & evtMask;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array, where the data is
 *  aligned to 32-bit word boundaries.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input value array
 *  @param chan   (IN)  Input channel tag array (null if n/a)
 *  @param evt    (IN)  Input event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packIntWord32 (void *ptr, int32_t off, const int32_t *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 32 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  int32_t valMask  = (sign)? ((int32_t)-1) : ((int32_t)__INT64_C(0xFFFFFFFF));
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=4) {
    int32_t val   = (int32_t)(((int32_t)array[i]) & valMask);
    int32_t value = val << (32-dSize);
    if (chan != NULL) value |= (chan[i] & chanMask) << (pSize);
    if (evt  != NULL) value |= (evt[i]  & evtMask ) << (pSize+cSize);
    packInt(buf, off, value);
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array, where the data is
 *  aligned to 64-bit word boundaries.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Bit offset into buf (must be multiple of 64 bits)
 *  @param array  (OUT) Output value array (not null)
 *  @param chan   (OUT) Output channel tag array (null if n/a)
 *  @param evt    (OUT) Output event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackIntWord64 (const void *ptr, int32_t off, int32_t *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 64 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  int64_t valMask  = (sign)? ((int64_t)-1) : (~(((int64_t)-1) << dSize));
  if (dSize == 32) {
    // When using Int32/UInt32 the first 32-bits will always the the Int32/UInt32
    // value, so use 32-bit operations to improve performance
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int32_t val   = unpackInt(buf, off);
      int32_t value = unpackInt(buf, off+4);
      array[i] = (int32_t)(val & valMask);
      if (chan != NULL) chan[i] = ((int32_t)(value >> (pSize      ))) & chanMask;
      if (evt  != NULL) evt[i]  = ((int32_t)(value >> (pSize+cSize))) & evtMask;
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int64_t value = unpackLong(buf, off);
      array[i] = (int32_t)((value >> (64-dSize)) & valMask);
      if (chan != NULL) chan[i] = ((int32_t)(value >> (pSize      ))) & chanMask;
      if (evt  != NULL) evt[i]  = ((int32_t)(value >> (pSize+cSize))) & evtMask;
    }
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array, where the data is
 *  aligned to 64-bit word boundaries.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input value array
 *  @param chan   (IN)  Input channel tag array (null if n/a)
 *  @param evt    (IN)  Input event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packIntWord64 (void *ptr, int32_t off, const int32_t *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 64 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  int64_t valMask  = (sign)? ((int64_t)-1) : ((int64_t)__INT64_C(0xFFFFFFFF));
  if (dSize == 32) {
    // When using Int32/UInt32 the first 32-bits will always the the Int32/UInt32
    // value, so use 32-bit operations to improve performance
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int32_t val   = (int32_t)(((int64_t)array[i]) & valMask);
      int32_t value = 0;
      if (chan != NULL) value  = (chan[i] & chanMask) << (pSize);
      if (evt  != NULL) value |= (evt[i]  & evtMask ) << (pSize+cSize);
      packInt(buf, off,   val);
      packInt(buf, off+4, value);
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int64_t val   = (int64_t)(((int64_t)array[i]) & valMask);
      int64_t value = val << (64-dSize);
      if (chan != NULL) value |= ((int64_t)(chan[i] & chanMask)) << (pSize);
      if (evt  != NULL) value |= ((int64_t)(evt[i]  & evtMask )) << (pSize+cSize);
      packLong(buf, off, value);
    }
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array, where the data is
 *  aligned to 64-bit word boundaries.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Bit offset into buf (must be multiple of 64 bits)
 *  @param array  (OUT) Output value array (not null)
 *  @param chan   (OUT) Output channel tag array (null if n/a)
 *  @param evt    (OUT) Output event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackFloatWord64 (const void *ptr, int32_t off, int32_t *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 64 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(dSize);
  // When using Float32 the first 32-bits will always the the Float32 value, so
  // use 32-bit operations to improve performance
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=8) {
    array[i] = (int32_t)unpackFloat(buf, off);
    int32_t value = unpackInt(buf, off+4);
    if (chan != NULL) chan[i] = (value >> (pSize      )) & chanMask;
    if (evt  != NULL) evt[i]  = (value >> (pSize+cSize)) & evtMask;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array, where the data is
 *  aligned to 64-bit word boundaries.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input value array
 *  @param chan   (IN)  Input channel tag array (null if n/a)
 *  @param evt    (IN)  Input event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packFloatWord64 (void *ptr, int32_t off, const int32_t *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 64 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(dSize);
  // When using Float32 the first 32-bits will always the the Float32 value, so
  // use 32-bit operations to improve performance
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=8) {
    float val   = (float)array[i];
    int32_t   value = 0;
    if (chan != NULL) value  = (chan[i] & chanMask) << (pSize);
    if (evt  != NULL) value |= (evt[i]  & evtMask ) << (pSize+cSize);
    packFloat(buf, off,   val);
    packInt(  buf, off+4, value);
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array, where the data is
 *  aligned to 32-bit word boundaries.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Bit offset into buf (must be multiple of 32 bits)
 *  @param array  (OUT) Output value array (not null)
 *  @param chan   (OUT) Output channel tag array (null if n/a)
 *  @param evt    (OUT) Output event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackVRTFloatWord32 (const void *ptr, int32_t off, int32_t *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 32 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=4) {
    int32_t value = unpackInt(buf, off);
    int32_t bits  = (value >> (32-dSize)); // fromVRTFloat32(..) will mask off any extra bits
    array[i] = (int32_t)fromVRTFloat32(sign, expBits, dSize, bits);
    if (chan != NULL) chan[i] = (value >> (pSize      )) & chanMask;
    if (evt  != NULL) evt[i]  = (value >> (pSize+cSize)) & evtMask;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array, where the data is
 *  aligned to 32-bit word boundaries.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input value array
 *  @param chan   (IN)  Input channel tag array (null if n/a)
 *  @param evt    (IN)  Input event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packVRTFloatWord32 (void *ptr, int32_t off, const int32_t *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 32 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=4) {
    int32_t val   = toVRTFloat32(sign, expBits, dSize, (double)array[i]);
    int32_t value = val << (32-dSize);
    if (chan != NULL) value |= (chan[i] & chanMask) << (pSize);
    if (evt  != NULL) value |= (evt[i]  & evtMask ) << (pSize+cSize);
    packInt(buf, off, value);
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array, where the data is
 *  aligned to 64-bit word boundaries.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Bit offset into buf (must be multiple of 64 bits)
 *  @param array  (OUT) Output value array (not null)
 *  @param chan   (OUT) Output channel tag array (null if n/a)
 *  @param evt    (OUT) Output event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackVRTFloatWord64 (const void *ptr, int32_t off, int32_t *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 64 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  if (dSize == 32) {
    // When using Int32/UInt32 the first 32-bits will always the the Int32/UInt32
    // value, so use 32-bit operations to improve performance
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int32_t val   = unpackInt(buf, off);
      int32_t value = unpackInt(buf, off+4);
      array[i] = (int32_t)fromVRTFloat32(sign, expBits, dSize, val);
      if (chan != NULL) chan[i] = ((int32_t)(value >> (pSize      ))) & chanMask;
      if (evt  != NULL) evt[i]  = ((int32_t)(value >> (pSize+cSize))) & evtMask;
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int64_t value = unpackLong(buf, off);
      int64_t bits  = (value >> (64-dSize)); // fromVRTFloat64(..) will mask off any extra bits
      array[i] = (int32_t)fromVRTFloat64(sign, expBits, dSize, bits);
      if (chan != NULL) chan[i] = ((int32_t)(value >> (pSize      ))) & chanMask;
      if (evt  != NULL) evt[i]  = ((int32_t)(value >> (pSize+cSize))) & evtMask;
    }
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array, where the data is
 *  aligned to 64-bit word boundaries.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input value array
 *  @param chan   (IN)  Input channel tag array (null if n/a)
 *  @param evt    (IN)  Input event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packVRTFloatWord64 (void *ptr, int32_t off, const int32_t *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 64 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  if (dSize == 32) {
    // When using Int32/UInt32 the first 32-bits will always the the Int32/UInt32
    // value, so use 32-bit operations to improve performance
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int32_t val   = toVRTFloat32(sign, expBits, dSize, (double)array[i]);
      int32_t value = 0;
      if (chan != NULL) value  = (chan[i] & chanMask) << (pSize);
      if (evt  != NULL) value |= (evt[i]  & evtMask ) << (pSize+cSize);
      packInt(buf, off,   val);
      packInt(buf, off+4, value);
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int64_t val   = toVRTFloat64(sign, expBits, dSize, (double)array[i]);
      int64_t value = val << (64-dSize);
      if (chan != NULL) value |= ((int64_t)(chan[i] & chanMask)) << (pSize);
      if (evt  != NULL) value |= ((int64_t)(evt[i]  & evtMask )) << (pSize+cSize);
      packLong(buf, off, value);
    }
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array.
 *  @param ptr       (IN)  Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf (must be multiple of 32 bits)
 *  @param array     (OUT) Output value array (not null)
 *  @param chan      (OUT) Output channel tag array (null if n/a)
 *  @param evt       (OUT) Output event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackIntBuffer32 (const void *ptr, int32_t bitOffset, int32_t *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  const int32_t bitmask      = 32 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t _bits_dSize  = 32 - dSize;    // pre-compute "32 - dSize" for efficiency
  int32_t _bits_fSize  = 32 - fSize;    // pre-compute "32 - fSize" for efficiency
  int32_t _fSize_dSize = fSize - dSize;         // pre-compute "fSize - dSize" for efficiency
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      // Not enough bits remaining in this word, move to next word (skipping pad bits)
      bitOffset = (bitOffset + 32) & bitmaskInv;
    }
    int32_t bits  = unpackBits32(buf, bitOffset, fSize);
    int32_t value = bits >> _fSize_dSize;
    // sign extend as needed (otherwise leave top bits 0)
    array[i] = (sign)? (int32_t)((value << _bits_dSize) >>  _bits_dSize)
                     : (int32_t)value;
    if (evt  != NULL) evt[i]  = ((int32_t)(bits >> cSize)) & eMask;
    if (chan != NULL) chan[i] = ((int32_t)(bits         )) & cMask;
    bitOffset += fSize;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array.
 *  @param ptr       (I/O) Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf
 *  @param array     (IN)  Input value array
 *  @param chan      (IN)  Input channel tag array (null if n/a)
 *  @param evt       (IN)  Input event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void packIntBuffer32 (void *ptr, int32_t bitOffset, const int32_t *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  const int32_t bitmask      = 32 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t uSize        = fSize - dSize - eSize - cSize;
  int32_t _bits_fSize  = 32 - fSize;    // pre-compute "32 - fSize" for efficiency
  int32_t valMask      = (sign)? ((int32_t)-1) : ((int32_t)__INT64_C(0xFFFFFFFF)); // Bit mask for input value (controls sign-extend)
  if ((evt == NULL) || (eSize == 0)) {
    // Treat event tag as unused since we have nothing to insert there (this
    // will be slightly faster in the loop below).
    uSize = uSize + eSize;
    eSize = 0;
    evt   = NULL;
    if (chan == NULL) {
      // Treat channel tag as unused since we have nothing to insert there (this
      // will be slightly faster in the loop below).
      uSize = uSize + cSize;
      cSize = 0;
      chan  = NULL;
    }
  }
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  int32_t _uSize_eSize = uSize + eSize;         // pre-compute "uSize + eSize" for efficiency
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      if (true) { //TODO: make this optional
        // Not enough bits remaining in this word, move to next word (clearing pad bits)
        int32_t zSize = 32 - (bitOffset & bitmask); // bits remaining to set
        packBits32(buf, bitOffset, zSize, 0);
        bitOffset += zSize;
      }
      else {
        // Not enough bits remaining in this word, move to next word (skipping pad bits)
        bitOffset = (bitOffset + 32) & bitmaskInv;
      }
    }
    int32_t value = (int32_t)(((int32_t)array[i]) & valMask);
    value = value << _uSize_eSize;
    if (evt != NULL) value = value | (evt[i] & eMask);
    value = value << cSize;
    if (chan != NULL) value = value | (chan[i] & cMask);
    packBits32(buf, bitOffset, fSize, value);
    bitOffset += fSize;
  }
  // ==== End of Packet Pad Bits ===============================================
  // Clear out any bits remaining in the current word that would be at the end
  // of the packet, this applies to both link and processing efficient.
  if (true) { //TODO: make this optional
    int32_t xSize = 32 - (bitOffset & bitmask); // bits remaining to set
    if (xSize > 0) {
      packBits32(buf, bitOffset, xSize, 0);
      bitOffset += xSize;
    }
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array.
 *  @param ptr       (IN)  Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf (must be multiple of 64 bits)
 *  @param array     (OUT) Output value array (not null)
 *  @param chan      (OUT) Output channel tag array (null if n/a)
 *  @param evt       (OUT) Output event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackIntBuffer64 (const void *ptr, int32_t bitOffset, int32_t *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  const int32_t bitmask      = 64 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t _bits_dSize  = 64 - dSize;    // pre-compute "64 - dSize" for efficiency
  int32_t _bits_fSize  = 64 - fSize;    // pre-compute "64 - fSize" for efficiency
  int32_t _fSize_dSize = fSize - dSize;         // pre-compute "fSize - dSize" for efficiency
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      // Not enough bits remaining in this word, move to next word (skipping pad bits)
      bitOffset = (bitOffset + 32) & bitmaskInv;
    }
    int64_t bits  = unpackBits64(buf, bitOffset, fSize);
    int64_t value = bits >> _fSize_dSize;
    // sign extend as needed (otherwise leave top bits 0)
    array[i] = (sign)? (int32_t)((value << _bits_dSize) >>  _bits_dSize)
                     : (int32_t)value;
    if (evt  != NULL) evt[i]  = ((int32_t)(bits >> cSize)) & eMask;
    if (chan != NULL) chan[i] = ((int32_t)(bits         )) & cMask;
    bitOffset += fSize;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array.
 *  @param ptr       (I/O) Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf
 *  @param array     (IN)  Input value array
 *  @param chan      (IN)  Input channel tag array (null if n/a)
 *  @param evt       (IN)  Input event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void packIntBuffer64 (void *ptr, int32_t bitOffset, const int32_t *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  const int32_t bitmask      = 64 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t uSize        = fSize - dSize - eSize - cSize;
  int32_t _bits_fSize  = 64 - fSize;    // pre-compute "64 - fSize" for efficiency
  int64_t valMask      = (sign)? ((int64_t)-1) : ((int64_t)__INT64_C(0xFFFFFFFF)); // Bit mask for input value (controls sign-extend)
  if ((evt == NULL) || (eSize == 0)) {
    // Treat event tag as unused since we have nothing to insert there (this
    // will be slightly faster in the loop below).
    uSize = uSize + eSize;
    eSize = 0;
    evt   = NULL;
    if (chan == NULL) {
      // Treat channel tag as unused since we have nothing to insert there (this
      // will be slightly faster in the loop below).
      uSize = uSize + cSize;
      cSize = 0;
      chan  = NULL;
    }
  }
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  int32_t _uSize_eSize = uSize + eSize;         // pre-compute "uSize + eSize" for efficiency
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      if (true) { //TODO: make this optional
        // Not enough bits remaining in this word, move to next word (clearing pad bits)
        int32_t zSize = 64 - (bitOffset & bitmask); // bits remaining to set
        packBits64(buf, bitOffset, zSize, 0);
        bitOffset += zSize;
      }
      else {
        // Not enough bits remaining in this word, move to next word (skipping pad bits)
        bitOffset = (bitOffset + 32) & bitmaskInv;
      }
    }
    int64_t value = (int64_t)(((int64_t)array[i]) & valMask);
    value = value << _uSize_eSize;
    if (evt != NULL) value = value | (evt[i] & eMask);
    value = value << cSize;
    if (chan != NULL) value = value | (chan[i] & cMask);
    packBits64(buf, bitOffset, fSize, value);
    bitOffset += fSize;
  }
  // ==== End of Packet Pad Bits ===============================================
  // Clear out any bits remaining in the current word that would be at the end
  // of the packet, this applies to both link and processing efficient.
  if (true) { //TODO: make this optional
    int32_t xSize = 64 - (bitOffset & bitmask); // bits remaining to set
    if (xSize > 0) {
      packBits64(buf, bitOffset, xSize, 0);
      bitOffset += xSize;
    }
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array.
 *  @param ptr       (IN)  Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf (must be multiple of 64 bits)
 *  @param array     (OUT) Output value array (not null)
 *  @param chan      (OUT) Output channel tag array (null if n/a)
 *  @param evt       (OUT) Output event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackFloatBuffer64 (const void *ptr, int32_t bitOffset, int32_t *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  const int32_t bitmask      = 64 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t _bits_fSize  = 64 - fSize;    // pre-compute "64 - fSize" for efficiency
  int32_t _fSize_dSize = fSize - dSize;         // pre-compute "fSize - dSize" for efficiency
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      // Not enough bits remaining in this word, move to next word (skipping pad bits)
      bitOffset = (bitOffset + 32) & bitmaskInv;
    }
    int64_t bits  = unpackBits64(buf, bitOffset, fSize);
    int64_t value = bits >> _fSize_dSize;
    UNUSED_VARIABLE(sign);
    array[i] = (int32_t)intBitsToFloat((int32_t)value);
    if (evt  != NULL) evt[i]  = ((int32_t)(bits >> cSize)) & eMask;
    if (chan != NULL) chan[i] = ((int32_t)(bits         )) & cMask;
    bitOffset += fSize;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array.
 *  @param ptr       (I/O) Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf
 *  @param array     (IN)  Input value array
 *  @param chan      (IN)  Input channel tag array (null if n/a)
 *  @param evt       (IN)  Input event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void packFloatBuffer64 (void *ptr, int32_t bitOffset, const int32_t *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  const int32_t bitmask      = 64 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t uSize        = fSize - dSize - eSize - cSize;
  int32_t _bits_fSize  = 64 - fSize;    // pre-compute "64 - fSize" for efficiency
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  if ((evt == NULL) || (eSize == 0)) {
    // Treat event tag as unused since we have nothing to insert there (this
    // will be slightly faster in the loop below).
    uSize = uSize + eSize;
    eSize = 0;
    evt   = NULL;
    if (chan == NULL) {
      // Treat channel tag as unused since we have nothing to insert there (this
      // will be slightly faster in the loop below).
      uSize = uSize + cSize;
      cSize = 0;
      chan  = NULL;
    }
  }
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  int32_t _uSize_eSize = uSize + eSize;         // pre-compute "uSize + eSize" for efficiency
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      if (true) { //TODO: make this optional
        // Not enough bits remaining in this word, move to next word (clearing pad bits)
        int32_t zSize = 64 - (bitOffset & bitmask); // bits remaining to set
        packBits64(buf, bitOffset, zSize, 0);
        bitOffset += zSize;
      }
      else {
        // Not enough bits remaining in this word, move to next word (skipping pad bits)
        bitOffset = (bitOffset + 32) & bitmaskInv;
      }
    }
    int64_t value = (int64_t)floatToRawIntBits((float)array[i]);
    value = value << _uSize_eSize;
    if (evt != NULL) value = value | (evt[i] & eMask);
    value = value << cSize;
    if (chan != NULL) value = value | (chan[i] & cMask);
    packBits64(buf, bitOffset, fSize, value);
    bitOffset += fSize;
  }
  // ==== End of Packet Pad Bits ===============================================
  // Clear out any bits remaining in the current word that would be at the end
  // of the packet, this applies to both link and processing efficient.
  if (true) { //TODO: make this optional
    int32_t xSize = 64 - (bitOffset & bitmask); // bits remaining to set
    if (xSize > 0) {
      packBits64(buf, bitOffset, xSize, 0);
      bitOffset += xSize;
    }
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array.
 *  @param ptr       (IN)  Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf (must be multiple of 32 bits)
 *  @param array     (OUT) Output value array (not null)
 *  @param chan      (OUT) Output channel tag array (null if n/a)
 *  @param evt       (OUT) Output event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackVRTFloatBuffer32 (const void *ptr, int32_t bitOffset, int32_t *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  const char *buf = (const char*)ptr;
  const int32_t bitmask      = 32 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t _bits_fSize  = 32 - fSize;    // pre-compute "32 - fSize" for efficiency
  int32_t _fSize_dSize = fSize - dSize;         // pre-compute "fSize - dSize" for efficiency
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      // Not enough bits remaining in this word, move to next word (skipping pad bits)
      bitOffset = (bitOffset + 32) & bitmaskInv;
    }
    int32_t bits  = unpackBits32(buf, bitOffset, fSize);
    int32_t value = bits >> _fSize_dSize;
    array[i] = (int32_t)fromVRTFloat32(sign, expBits, dSize, value);
    if (evt  != NULL) evt[i]  = ((int32_t)(bits >> cSize)) & eMask;
    if (chan != NULL) chan[i] = ((int32_t)(bits         )) & cMask;
    bitOffset += fSize;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array.
 *  @param ptr       (I/O) Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf
 *  @param array     (IN)  Input value array
 *  @param chan      (IN)  Input channel tag array (null if n/a)
 *  @param evt       (IN)  Input event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void packVRTFloatBuffer32 (void *ptr, int32_t bitOffset, const int32_t *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  char *buf = (char*)ptr;
  const int32_t bitmask      = 32 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t uSize        = fSize - dSize - eSize - cSize;
  int32_t _bits_fSize  = 32 - fSize;    // pre-compute "32 - fSize" for efficiency
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  if ((evt == NULL) || (eSize == 0)) {
    // Treat event tag as unused since we have nothing to insert there (this
    // will be slightly faster in the loop below).
    uSize = uSize + eSize;
    eSize = 0;
    evt   = NULL;
    if (chan == NULL) {
      // Treat channel tag as unused since we have nothing to insert there (this
      // will be slightly faster in the loop below).
      uSize = uSize + cSize;
      cSize = 0;
      chan  = NULL;
    }
  }
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  int32_t _uSize_eSize = uSize + eSize;         // pre-compute "uSize + eSize" for efficiency
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      if (true) { //TODO: make this optional
        // Not enough bits remaining in this word, move to next word (clearing pad bits)
        int32_t zSize = 32 - (bitOffset & bitmask); // bits remaining to set
        packBits32(buf, bitOffset, zSize, 0);
        bitOffset += zSize;
      }
      else {
        // Not enough bits remaining in this word, move to next word (skipping pad bits)
        bitOffset = (bitOffset + 32) & bitmaskInv;
      }
    }
    int32_t value = toVRTFloat32(sign, expBits, dSize, (double)array[i]);
    value = value << _uSize_eSize;
    if (evt != NULL) value = value | (evt[i] & eMask);
    value = value << cSize;
    if (chan != NULL) value = value | (chan[i] & cMask);
    packBits32(buf, bitOffset, fSize, value);
    bitOffset += fSize;
  }
  // ==== End of Packet Pad Bits ===============================================
  // Clear out any bits remaining in the current word that would be at the end
  // of the packet, this applies to both link and processing efficient.
  if (true) { //TODO: make this optional
    int32_t xSize = 32 - (bitOffset & bitmask); // bits remaining to set
    if (xSize > 0) {
      packBits32(buf, bitOffset, xSize, 0);
      bitOffset += xSize;
    }
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array.
 *  @param ptr       (IN)  Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf (must be multiple of 64 bits)
 *  @param array     (OUT) Output value array (not null)
 *  @param chan      (OUT) Output channel tag array (null if n/a)
 *  @param evt       (OUT) Output event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackVRTFloatBuffer64 (const void *ptr, int32_t bitOffset, int32_t *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  const char *buf = (const char*)ptr;
  const int32_t bitmask      = 64 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t _bits_fSize  = 64 - fSize;    // pre-compute "64 - fSize" for efficiency
  int32_t _fSize_dSize = fSize - dSize;         // pre-compute "fSize - dSize" for efficiency
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      // Not enough bits remaining in this word, move to next word (skipping pad bits)
      bitOffset = (bitOffset + 32) & bitmaskInv;
    }
    int64_t bits  = unpackBits64(buf, bitOffset, fSize);
    int64_t value = bits >> _fSize_dSize;
    array[i] = (int32_t)fromVRTFloat64(sign, expBits, dSize, value);
    if (evt  != NULL) evt[i]  = ((int32_t)(bits >> cSize)) & eMask;
    if (chan != NULL) chan[i] = ((int32_t)(bits         )) & cMask;
    bitOffset += fSize;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array.
 *  @param ptr       (I/O) Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf
 *  @param array     (IN)  Input value array
 *  @param chan      (IN)  Input channel tag array (null if n/a)
 *  @param evt       (IN)  Input event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void packVRTFloatBuffer64 (void *ptr, int32_t bitOffset, const int32_t *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  char *buf = (char*)ptr;
  const int32_t bitmask      = 64 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t uSize        = fSize - dSize - eSize - cSize;
  int32_t _bits_fSize  = 64 - fSize;    // pre-compute "64 - fSize" for efficiency
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  if ((evt == NULL) || (eSize == 0)) {
    // Treat event tag as unused since we have nothing to insert there (this
    // will be slightly faster in the loop below).
    uSize = uSize + eSize;
    eSize = 0;
    evt   = NULL;
    if (chan == NULL) {
      // Treat channel tag as unused since we have nothing to insert there (this
      // will be slightly faster in the loop below).
      uSize = uSize + cSize;
      cSize = 0;
      chan  = NULL;
    }
  }
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  int32_t _uSize_eSize = uSize + eSize;         // pre-compute "uSize + eSize" for efficiency
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      if (true) { //TODO: make this optional
        // Not enough bits remaining in this word, move to next word (clearing pad bits)
        int32_t zSize = 64 - (bitOffset & bitmask); // bits remaining to set
        packBits64(buf, bitOffset, zSize, 0);
        bitOffset += zSize;
      }
      else {
        // Not enough bits remaining in this word, move to next word (skipping pad bits)
        bitOffset = (bitOffset + 32) & bitmaskInv;
      }
    }
    int64_t value = toVRTFloat64(sign, expBits, dSize, (double)array[i]);
    value = value << _uSize_eSize;
    if (evt != NULL) value = value | (evt[i] & eMask);
    value = value << cSize;
    if (chan != NULL) value = value | (chan[i] & cMask);
    packBits64(buf, bitOffset, fSize, value);
    bitOffset += fSize;
  }
  // ==== End of Packet Pad Bits ===============================================
  // Clear out any bits remaining in the current word that would be at the end
  // of the packet, this applies to both link and processing efficient.
  if (true) { //TODO: make this optional
    int32_t xSize = 64 - (bitOffset & bitmask); // bits remaining to set
    if (xSize > 0) {
      packBits64(buf, bitOffset, xSize, 0);
      bitOffset += xSize;
    }
  }
}
#if NOT_USING_JNI
int32_t* vrt::PackUnpack::unpackAsInt (PayloadFormat pf, const void *ptr, int32_t off, int32_t *array, int32_t *chan, int32_t *evt, int32_t length) {
  const char *buf = (const char*)ptr;
  if (!pf.isValid()) {
    throw VRTException(pf.getValid());
  }
  int32_t            fSize  = pf.getItemPackingFieldSize();
  int32_t            dSize  = pf.getDataItemSize();
  int32_t            eSize  = pf.getEventTagSize();
  int32_t            cSize  = pf.getChannelTagSize();
  int32_t            xSize  = fSize; // effective fSize
  bool        proc   = pf.isProcessingEfficient();
  bool        sign   = pf.isSigned();
  DataItemFormat format = pf.getDataItemFormat();
  bool        noTags;
  if (eSize == 0) evt  = NULL; // no event tags, ignore any array passed in
  if (cSize == 0) chan = NULL; // no channel tags, ignore any array passed in
  noTags = ((chan == NULL) && (evt == NULL)) || (fSize == dSize);
  if (proc) {
         if (fSize >= 33) xSize = 64;
    else if (fSize >= 17) xSize = 32;
    if (noTags) fSize = xSize; // Use faster field size if ignoring Event & Channel Tags
  }
  switch (format) {
    case DataItemFormat_SignedInt:   // FALLTHROUGH
    case DataItemFormat_UnsignedInt:
      // ---- Try fast methods -------------------------------------------------
      if (noTags) {
        switch (fSize) {
          case  8: unpackBytes(  buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
          case 16: unpackShorts( buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
          case 24: unpackInt24s( buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
          case 32: unpackInts(   buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
          case 64: unpackLongs(  buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
        }
      }
      if (fSize == dSize) { // implicit noTags=true in this case
        switch (fSize) {
          case  1: if ((length % 8) == 0) { unpackBits(   buf, off, array, 0, length, fSize, dSize, proc, sign); return array; } break;
          case  4: if ((length % 2) == 0) { unpackNibbles(buf, off, array, 0, length, fSize, dSize, proc, sign); return array; } break;
          case 12: if ((length % 2) == 0) { unpackInt12s( buf, off, array, 0, length, fSize, dSize, proc, sign); return array; } break;
        }
      }
      if (xSize == 32) {
        unpackIntWord32(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return array;
      }
      if (xSize == 64) {
        unpackIntWord64(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return array;
      }
      // ---- Fall back to slow method -----------------------------------------
      if (fSize <= 32) {
        unpackIntBuffer32(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return array;
      }
      else {
        unpackIntBuffer64(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return array;
      }
    case DataItemFormat_Float:
      // ---- Try fast methods -------------------------------------------------
      if (fSize == dSize) { // implicit noTags=true in this case
        unpackFloats(buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
      }
      if (xSize == 64) {
        unpackFloatWord64(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return array;
      }
      // ---- Fall back to slow method -----------------------------------------
      unpackFloatBuffer64(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return array;
    case DataItemFormat_Double:
      // ---- Only fast method for double (yea!) -------------------------------
      unpackDoubles(buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
    default: // VRTFloat Types
      // ---- Try fast methods -------------------------------------------------
      if (xSize == 32) {
        unpackVRTFloatWord32(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return array;
      }
      if (xSize == 64) {
        unpackVRTFloatWord64(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return array;
      }
      // ---- Fall back to slow method -----------------------------------------
      if (fSize <= 32) {
        unpackVRTFloatBuffer32(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return array;
      }
      else {
        unpackVRTFloatBuffer64(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return array;
      }
  }
}
void vrt::PackUnpack::packAsInt (PayloadFormat pf, void *ptr, int32_t off, const int32_t *array, const int32_t *chan, const int32_t *evt, int32_t length) {
  char *buf = (char*)ptr;
  if (!pf.isValid()) {
    throw VRTException(pf.getValid());
  }
  int32_t            fSize  = pf.getItemPackingFieldSize();
  int32_t            dSize  = pf.getDataItemSize();
  int32_t            eSize  = pf.getEventTagSize();
  int32_t            cSize  = pf.getChannelTagSize();
  int32_t            xSize  = fSize; // effective fSize
  bool        proc   = pf.isProcessingEfficient();
  bool        sign   = pf.isSigned();
  DataItemFormat format = pf.getDataItemFormat();
  bool        noTags;
  if (eSize == 0) evt  = NULL; // no event tags, ignore any array passed in
  if (cSize == 0) chan = NULL; // no channel tags, ignore any array passed in
  noTags = ((chan == NULL) && (evt == NULL)) || (fSize == dSize);
  if (proc) {
         if (fSize >= 33) xSize = 64;
    else if (fSize >= 17) xSize = 32;
    if (noTags) fSize = xSize; // Use faster field size if ignoring Event & Channel Tags
  }
  switch (format) {
    case DataItemFormat_SignedInt:   // FALLTHROUGH
    case DataItemFormat_UnsignedInt:
      // ---- Try fast methods -------------------------------------------------
      if (noTags) {
        switch (fSize) {
          case  8: packBytes(  buf, off, array, 0, length, fSize, dSize, proc, sign); return;
          case 16: packShorts( buf, off, array, 0, length, fSize, dSize, proc, sign); return;
          case 24: packInt24s( buf, off, array, 0, length, fSize, dSize, proc, sign); return;
          case 32: packInts(   buf, off, array, 0, length, fSize, dSize, proc, sign); return;
          case 64: packLongs(  buf, off, array, 0, length, fSize, dSize, proc, sign); return;
        }
      }
      if (fSize == dSize) { // implicit noTags=true in this case
        switch (fSize) {
          case  1: if ((length % 8) == 0) { packBits(   buf, off, array, 0, length, fSize, dSize, proc, sign); return; } break;
          case  4: if ((length % 2) == 0) { packNibbles(buf, off, array, 0, length, fSize, dSize, proc, sign); return; } break;
          case 12: if ((length % 2) == 0) { packInt12s( buf, off, array, 0, length, fSize, dSize, proc, sign); return; } break;
        }
      }
      if (xSize == 32) {
        packIntWord32(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return;
      }
      if (xSize == 64) {
        packIntWord64(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return;
      }
      // ---- Fall back to slow method -----------------------------------------
      if (fSize <= 32) {
        packIntBuffer32(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return;
      }
      else {
        packIntBuffer64(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return;
      }
    case DataItemFormat_Float:
      // ---- Try fast methods -------------------------------------------------
      if (fSize == dSize) { // implicit noTags=true in this case
        packFloats(buf, off, array, 0, length, fSize, dSize, proc, sign); return;
      }
      if (xSize == 64) {
        packFloatWord64(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return;
      }
      // ---- Fall back to slow method -----------------------------------------
      packFloatBuffer64(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return;
    case DataItemFormat_Double:
      // ---- Only fast method for double (yea!) -------------------------------
      packDoubles(buf, off, array, 0, length, fSize, dSize, proc, sign); return;
    default: // VRTFloat Types
      // ---- Try fast methods -------------------------------------------------
      if (xSize == 32) {
        packVRTFloatWord32(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return;
      }
      if (xSize == 64) {
        packVRTFloatWord64(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return;
      }
      // ---- Fall back to slow method -----------------------------------------
      if (fSize <= 32) {
        packVRTFloatBuffer32(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return;
      }
      else {
        packVRTFloatBuffer64(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return;
      }
  }
}
#endif /* NOT_USING_JNI */
/** <b>Internal Use Only:</b> Fast unpack method for double data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits (must be 64)
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed? (must be true)
 */
static __attribute__((flatten)) void unpackDoubles (const void *ptr, int32_t off, int16_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  // All floating-point values are signed, so 'sign' is unused, but is included
  // in function definition for consistency.
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(align);
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  // When using Double data, it is required that dSize=64 and fSize=64 so we can
  // skip some of the normal checks we would otherwise do here.
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=8) {
    array[offset++] = (int16_t)unpackDouble(buf, off);
  }
}
/** <b>Internal Use Only:</b>
 *  Fast pack method for double data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits (must be 64)
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed? (must be true)
 */
static __attribute__((flatten)) void packDoubles (void *ptr, int32_t off, const int16_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  // All floating-point values are signed, so 'sign' is unused, but is included
  // in function definition for consistency.
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(align);
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  // When using Double data, it is required that dSize=64 and fSize=64 so we can
  // skip some of the normal checks we would otherwise do here.
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=8) {
    packDouble(buf, off, (double)array[offset++]);
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for float data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits (must be 32)
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed? (must be true)
 */
static __attribute__((flatten)) void unpackFloats (const void *ptr, int32_t off, int16_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  // All floating-point values are signed, so 'sign' is unused, but is included
  // in function definition for consistency.
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(dSize);
  int32_t delta; // int8_t offset delta
  if (align) delta = (fSize <= 32)? 4 : 8;
  else       delta = fSize / 8;
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=delta) {
    array[offset++] = (int16_t)unpackFloat(buf, off);
  }
}
/** <b>Internal Use Only:</b>
 *  Fast pack method for float data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits (must be 32)
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed? (must be true)
 */
static __attribute__((flatten)) void packFloats (void *ptr, int32_t off, const int16_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  // All floating-point values are signed, so 'sign' is unused, but is included
  // in function definition for consistency.
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(dSize);
  int32_t delta; // int8_t offset delta
  if (align) delta = (fSize <= 32)? 4 : 8;
  else       delta = fSize / 8;
  if (delta == 4) {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=4) {
      packFloat(buf, off, (float)array[offset++]);
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++) {
      packFloat(buf, off, (float)array[offset++]);
      off += 4;
      _Intel_Pragma("loop_count min=1, max=4")
      for (int32_t j = 4; j < delta; j++) {
        buf[off++] = 0; // set unused bits to zero
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed int64_t data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void unpackLongs (const void *ptr, int32_t off, int16_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  int32_t delta = (align)? ((fSize <= 32)? 4 : 8) : (fSize/8); // int8_t offset delta
  if (sign) {
    if (dSize == 64) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int16_t)unpackLong(buf, off);
      }
    }
    else {
      int32_t shift  = 64 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int16_t)(unpackLong(buf, off) >> shift);
      }
    }
  }
  else {
    if (dSize == 64) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int16_t)(unpackLong(buf, off) & __INT64_C(0xFFFFFFFFFFFFFFFF));
      }
    }
    else {
      int32_t shift  = 64 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int16_t)((unpackLong(buf, off) & __INT64_C(0xFFFFFFFFFFFFFFFF)) >> shift);
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for unsigned int64_t data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void packLongs (void *ptr, int32_t off, const int16_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  int32_t delta = (align)? ((fSize <= 32)? 4 : 8) : (fSize/8); // int8_t offset delta
  int64_t valMask = (sign)? ((int64_t)-1) : ((int64_t)__INT64_C(0xFFFF)); // Bit mask for input value (controls sign-extend)
  if (dSize == 64) {
    if (delta == 8) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=8) {
        int64_t val = (int64_t)(((int64_t)array[offset++]) & valMask);
        packLong(buf, off, val);
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int64_t val = (int64_t)(((int64_t)array[offset++]) & valMask);
        packLong(buf, off, val);
        off += 8;
        _Intel_Pragma("loop_count min=1, max=0")
        for (int32_t j = 8; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
  else {
    int32_t shift = 64 - dSize;
    if (delta == 8) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=8) {
        int64_t val = (int64_t)(((int64_t)array[offset++]) & valMask);
        packLong(buf, off, (int64_t)(val << shift));
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int64_t val = (int64_t)(((int64_t)array[offset++]) & valMask);
        packLong(buf, off, (int64_t)(val << shift));
        off += 8;
        _Intel_Pragma("loop_count min=1, max=0")
        for (int32_t j = 8; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed int32_t data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void unpackInts (const void *ptr, int32_t off, int16_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  int32_t delta = (align)? ((fSize <= 32)? 4 : 8) : (fSize/8); // int8_t offset delta
  if (sign) {
    if (dSize == 32) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int16_t)unpackInt(buf, off);
      }
    }
    else {
      int32_t shift  = 32 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int16_t)(unpackInt(buf, off) >> shift);
      }
    }
  }
  else {
    if (dSize == 32) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int16_t)(unpackInt(buf, off) & 0xFFFFFFFF);
      }
    }
    else {
      int32_t shift  = 32 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int16_t)((unpackInt(buf, off) & 0xFFFFFFFF) >> shift);
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for unsigned int32_t data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void packInts (void *ptr, int32_t off, const int16_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  int32_t delta = (align)? ((fSize <= 32)? 4 : 8) : (fSize/8); // int8_t offset delta
  int32_t valMask = (sign)? ((int32_t)-1) : ((int32_t)__INT64_C(0xFFFF)); // Bit mask for input value (controls sign-extend)
  if (dSize == 32) {
    if (delta == 4) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=4) {
        int32_t val = (int32_t)(((int32_t)array[offset++]) & valMask);
        packInt(buf, off, val);
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int32_t val = (int32_t)(((int32_t)array[offset++]) & valMask);
        packInt(buf, off, val);
        off += 4;
        _Intel_Pragma("loop_count min=1, max=4")
        for (int32_t j = 4; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
  else {
    int32_t shift = 32 - dSize;
    if (delta == 4) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=4) {
        int32_t val = (int32_t)(((int32_t)array[offset++]) & valMask);
        packInt(buf, off, (int32_t)(val << shift));
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int32_t val = (int32_t)(((int32_t)array[offset++]) & valMask);
        packInt(buf, off, (int32_t)(val << shift));
        off += 4;
        _Intel_Pragma("loop_count min=1, max=4")
        for (int32_t j = 4; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed int32_t data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void unpackInt24s (const void *ptr, int32_t off, int16_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  int32_t delta = (align)? ((fSize <= 32)? 4 : 8) : (fSize/8); // int8_t offset delta
  if (sign) {
    if (dSize == 24) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int16_t)unpackInt24(buf, off);
      }
    }
    else {
      int32_t shift  = 24 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int16_t)(unpackInt24(buf, off) >> shift);
      }
    }
  }
  else {
    if (dSize == 24) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int16_t)(unpackInt24(buf, off) & 0xFFFFFF);
      }
    }
    else {
      int32_t shift  = 24 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int16_t)((unpackInt24(buf, off) & 0xFFFFFF) >> shift);
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for unsigned int32_t data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void packInt24s (void *ptr, int32_t off, const int16_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  int32_t delta = (align)? ((fSize <= 32)? 4 : 8) : (fSize/8); // int8_t offset delta
  int32_t valMask = (sign)? ((int32_t)-1) : ((int32_t)__INT64_C(0xFFFF)); // Bit mask for input value (controls sign-extend)
  if (dSize == 24) {
    if (delta == 3) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=3) {
        int32_t val = (int32_t)(((int32_t)array[offset++]) & valMask);
        packInt24(buf, off, val);
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int32_t val = (int32_t)(((int32_t)array[offset++]) & valMask);
        packInt24(buf, off, val);
        off += 3;
        _Intel_Pragma("loop_count min=1, max=5")
        for (int32_t j = 3; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
  else {
    int32_t shift = 24 - dSize;
    if (delta == 3) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=3) {
        int32_t val = (int32_t)(((int32_t)array[offset++]) & valMask);
        packInt24(buf, off, (int32_t)(val << shift));
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int32_t val = (int32_t)(((int32_t)array[offset++]) & valMask);
        packInt24(buf, off, (int32_t)(val << shift));
        off += 3;
        _Intel_Pragma("loop_count min=1, max=5")
        for (int32_t j = 3; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed int16_t data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void unpackShorts (const void *ptr, int32_t off, int16_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
#if (2 == 1) || (BYTE_ORDER == BIG_ENDIAN)
  if ((fSize == 16) && (dSize == 16)) {
    // Fast array copy
    memcpy(&array[offset], &buf[off], length*2);
    return;
  }
#endif
  UNUSED_VARIABLE(align);
  int32_t delta = fSize / 8; // int8_t offset delta
  if (sign) {
    if (dSize == 16) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int16_t)unpackShort(buf, off);
      }
    }
    else {
      int32_t shift  = 16 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int16_t)(unpackShort(buf, off) >> shift);
      }
    }
  }
  else {
    if (dSize == 16) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int16_t)(unpackShort(buf, off) & 0xFFFF);
      }
    }
    else {
      int32_t shift  = 16 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int16_t)((unpackShort(buf, off) & 0xFFFF) >> shift);
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for unsigned int16_t data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void packShorts (void *ptr, int32_t off, const int16_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
#if (2 == 1) || (BYTE_ORDER == BIG_ENDIAN)
  if ((fSize == 16) && (dSize == 16)) {
    // Fast array copy
    memcpy(&buf[off], &array[offset], length*2);
    return;
  }
#endif
  UNUSED_VARIABLE(align);
  int32_t delta = fSize / 8; // int8_t offset delta
  int16_t valMask = (sign)? ((int16_t)-1) : ((int16_t)__INT64_C(0xFFFF)); // Bit mask for input value (controls sign-extend)
  if (dSize == 16) {
    if (delta == 2) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=2) {
        int16_t val = (int16_t)(((int16_t)array[offset++]) & valMask);
        packShort(buf, off, val);
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int16_t val = (int16_t)(((int16_t)array[offset++]) & valMask);
        packShort(buf, off, val);
        off += 2;
        _Intel_Pragma("loop_count min=1, max=6")
        for (int32_t j = 2; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
  else {
    int32_t shift = 16 - dSize;
    if (delta == 2) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=2) {
        int16_t val = (int16_t)(((int16_t)array[offset++]) & valMask);
        packShort(buf, off, (int16_t)(val << shift));
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int16_t val = (int16_t)(((int16_t)array[offset++]) & valMask);
        packShort(buf, off, (int16_t)(val << shift));
        off += 2;
        _Intel_Pragma("loop_count min=1, max=6")
        for (int32_t j = 2; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed Int12 data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert (must be a multiple of 2)
 *  @param fSize  (IN)  Item packing field size in bits (must be 12)
 *  @param dSize  (IN)  Data item size in bits (must be 12)
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackInt12s (const void *ptr, int32_t off, int16_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  int32_t skip = (align)? 1 : 0;
  if (sign) {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i+=2) {
      int32_t a = (int32_t)buf[off++];
      int32_t b = (int32_t)buf[off++];
      int32_t c = (int32_t)buf[off++];
      int32_t x = (a << 4) | ((b >> 4) & 0x000F);
      int32_t y = (b << 8) | ((c     ) & 0x00FF);
      array[offset++] = (int16_t)((x << 20) >> 20); // sign extend
      array[offset++] = (int16_t)((y << 20) >> 20); // sign extend
      off += skip; // skip extra int8_t in processing-efficient mode
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i+=2) {
      int32_t a = (int32_t)buf[off++];
      int32_t b = (int32_t)buf[off++];
      int32_t c = (int32_t)buf[off++];
      int32_t x = (a << 4) | ((b >> 4) & 0x000F);
      int32_t y = (b << 8) | ((c     ) & 0x00FF);
      array[offset++] = (int16_t)(x & 0x0FFF); // clear high-order bits
      array[offset++] = (int16_t)(y & 0x0FFF); // clear high-order bits
      off += skip; // skip extra int8_t in processing-efficient mode
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for signed Int12 data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert (must be a multiple of 2)
 *  @param fSize  (IN)  Item packing field size in bits (must be 12)
 *  @param dSize  (IN)  Data item size in bits (must be 12)
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packInt12s (void *ptr, int32_t off, const int16_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  int32_t skip = (align)?  1 : 0;
  int32_t mask = (sign )? -1 : ((int32_t)__INT64_C(0xFFFF));
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i+=2) {
    int32_t first  = ((int32_t)array[offset++]) & mask;
    int32_t second = ((int32_t)array[offset++]) & mask;
    buf[off++] = (int8_t)(first >> 4);
    buf[off++] = (int8_t)((first << 4) | ((second >> 8) & 0x0F));
    buf[off++] = (int8_t)(second);
    off += skip; // skip extra int8_t in processing-efficient mode
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed int8_t data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void unpackBytes (const void *ptr, int32_t off, int16_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  int32_t delta = fSize / 8; // int8_t offset delta
  if (sign) {
    if (dSize == 8) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int16_t)unpackByte(buf, off);
      }
    }
    else {
      int32_t shift  = 8 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int16_t)(unpackByte(buf, off) >> shift);
      }
    }
  }
  else {
    if (dSize == 8) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int16_t)(unpackByte(buf, off) & 0xFF);
      }
    }
    else {
      int32_t shift  = 8 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int16_t)((unpackByte(buf, off) & 0xFF) >> shift);
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for unsigned int8_t data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void packBytes (void *ptr, int32_t off, const int16_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(align);
  int32_t delta = fSize / 8; // int8_t offset delta
  int8_t valMask = (sign)? ((int8_t)-1) : ((int8_t)__INT64_C(0xFFFF)); // Bit mask for input value (controls sign-extend)
  if (dSize == 8) {
    if (delta == 1) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=1) {
        int8_t val = (int8_t)(((int8_t)array[offset++]) & valMask);
        packByte(buf, off, val);
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int8_t val = (int8_t)(((int8_t)array[offset++]) & valMask);
        packByte(buf, off, val);
        off += 1;
        _Intel_Pragma("loop_count min=1, max=7")
        for (int32_t j = 1; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
  else {
    int32_t shift = 8 - dSize;
    if (delta == 1) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=1) {
        int8_t val = (int8_t)(((int8_t)array[offset++]) & valMask);
        packByte(buf, off, (int8_t)(val << shift));
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int8_t val = (int8_t)(((int8_t)array[offset++]) & valMask);
        packByte(buf, off, (int8_t)(val << shift));
        off += 1;
        _Intel_Pragma("loop_count min=1, max=7")
        for (int32_t j = 1; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed Int4 (nibble) data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert (must be a multiple of 2)
 *  @param fSize  (IN)  Item packing field size in bits (must be 4)
 *  @param dSize  (IN)  Data item size in bits (must be 4)
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackNibbles (const void *ptr, int32_t off, int16_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  if (sign) {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i+=2) {
      int32_t b = (int32_t)buf[off++];
      array[offset++] = (int16_t)((b << 24) >> 28); // shift & sign extend
      array[offset++] = (int16_t)((b << 28) >> 28); // shift & sign extend
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i+=2) {
      int32_t b = (int32_t)buf[off++];
      array[offset++] = (int16_t)((b >> 4) & 0xF);
      array[offset++] = (int16_t)((b     ) & 0xF);
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for signed Int4 (nibble) data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert (must be a multiple of 2)
 *  @param fSize  (IN)  Item packing field size in bits (must be 4)
 *  @param dSize  (IN)  Data item size in bits (must be 4)
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packNibbles (void *ptr, int32_t off, const int16_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(align);
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  // Pack function for signed and unsigned are identical for nibble data, hence
  // 'sign' is unused here, but included in function definition for consistency.
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i+=2) {
    int32_t first  = (int32_t)array[offset++];
    int32_t second = (int32_t)array[offset++];
    buf[off++] = (int8_t)((first << 4) | (second & 0xF));
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for bit data. Each bit in the
 *  input buffer will be checked, a 0 bit will correspond to the value 0 and a
 *  1 bit will correspond to the value 1.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert (must be a multiple of 8)
 *  @param fSize  (IN)  Item packing field size in bits (must be 1)
 *  @param dSize  (IN)  Data item size in bits (must be 1)
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackBits (const void *ptr, int32_t off, int16_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(align);
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  if (sign) {
    // This uses sign-extending principles to get each bit as 0 or -1 which reflects
    // the fact that 0/-1 and not 0/+1 are the available values in a one-bit use of
    // twos-complement.
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i+=8) {
      int32_t b = (int32_t)buf[off++];
      array[offset++] = (int16_t)((b << 24) >> 31);
      array[offset++] = (int16_t)((b << 25) >> 31);
      array[offset++] = (int16_t)((b << 26) >> 31);
      array[offset++] = (int16_t)((b << 27) >> 31);
      array[offset++] = (int16_t)((b << 28) >> 31);
      array[offset++] = (int16_t)((b << 29) >> 31);
      array[offset++] = (int16_t)((b << 30) >> 31);
      array[offset++] = (int16_t)((b << 31) >> 31);
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i+=8) {
      int32_t b = (int32_t)buf[off++];
      array[offset++] = (int16_t)((b >> 7) & 0x1);
      array[offset++] = (int16_t)((b >> 6) & 0x1);
      array[offset++] = (int16_t)((b >> 5) & 0x1);
      array[offset++] = (int16_t)((b >> 4) & 0x1);
      array[offset++] = (int16_t)((b >> 3) & 0x1);
      array[offset++] = (int16_t)((b >> 2) & 0x1);
      array[offset++] = (int16_t)((b >> 1) & 0x1);
      array[offset++] = (int16_t)((b     ) & 0x1);
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for bit data. Each value in the
 *  input array will be checked, a zero value will correspond to the bit 0 and
 *  any non-zero value will correspond to the bit 1.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert (must be a multiple of 8)
 *  @param fSize  (IN)  Item packing field size in bits (must be 1)
 *  @param dSize  (IN)  Data item size in bits (must be 1)
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packBits (void *ptr, int32_t off, const int16_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(align);
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  // Pack function for signed and unsigned are identical for bit data, hence
  // 'sign' is unused here, but included in function definition for consistency.
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i+=8) {
    int32_t b = 0;
    b |= (((int32_t)array[offset++]) << 7) & 0x80;
    b |= (((int32_t)array[offset++]) << 6) & 0x40;
    b |= (((int32_t)array[offset++]) << 5) & 0x20;
    b |= (((int32_t)array[offset++]) << 4) & 0x10;
    b |= (((int32_t)array[offset++]) << 3) & 0x08;
    b |= (((int32_t)array[offset++]) << 2) & 0x04;
    b |= (((int32_t)array[offset++]) << 1) & 0x02;
    b |= (((int32_t)array[offset++])     ) & 0x01;
    buf[off++] = (int8_t)b;
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array, where the data is
 *  aligned to 32-bit word boundaries.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Bit offset into buf (must be multiple of 32 bits)
 *  @param array  (OUT) Output value array (not null)
 *  @param chan   (OUT) Output channel tag array (null if n/a)
 *  @param evt    (OUT) Output event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackIntWord32 (const void *ptr, int32_t off, int16_t *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 32 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  int32_t valMask  = (sign)? ((int32_t)-1) : (~(((int32_t)-1) << dSize));
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=4) {
    int32_t value = unpackInt(buf, off);
    array[i] = (int16_t)((value >> (32-dSize)) & valMask);
    if (chan != NULL) chan[i] = (value >> (pSize      )) & chanMask;
    if (evt  != NULL) evt[i]  = (value >> (pSize+cSize)) & evtMask;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array, where the data is
 *  aligned to 32-bit word boundaries.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input value array
 *  @param chan   (IN)  Input channel tag array (null if n/a)
 *  @param evt    (IN)  Input event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packIntWord32 (void *ptr, int32_t off, const int16_t *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 32 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  int32_t valMask  = (sign)? ((int32_t)-1) : ((int32_t)__INT64_C(0xFFFF));
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=4) {
    int32_t val   = (int32_t)(((int32_t)array[i]) & valMask);
    int32_t value = val << (32-dSize);
    if (chan != NULL) value |= (chan[i] & chanMask) << (pSize);
    if (evt  != NULL) value |= (evt[i]  & evtMask ) << (pSize+cSize);
    packInt(buf, off, value);
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array, where the data is
 *  aligned to 64-bit word boundaries.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Bit offset into buf (must be multiple of 64 bits)
 *  @param array  (OUT) Output value array (not null)
 *  @param chan   (OUT) Output channel tag array (null if n/a)
 *  @param evt    (OUT) Output event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackIntWord64 (const void *ptr, int32_t off, int16_t *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 64 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  int64_t valMask  = (sign)? ((int64_t)-1) : (~(((int64_t)-1) << dSize));
  if (dSize == 32) {
    // When using Int32/UInt32 the first 32-bits will always the the Int32/UInt32
    // value, so use 32-bit operations to improve performance
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int32_t val   = unpackInt(buf, off);
      int32_t value = unpackInt(buf, off+4);
      array[i] = (int16_t)(val & valMask);
      if (chan != NULL) chan[i] = ((int32_t)(value >> (pSize      ))) & chanMask;
      if (evt  != NULL) evt[i]  = ((int32_t)(value >> (pSize+cSize))) & evtMask;
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int64_t value = unpackLong(buf, off);
      array[i] = (int16_t)((value >> (64-dSize)) & valMask);
      if (chan != NULL) chan[i] = ((int32_t)(value >> (pSize      ))) & chanMask;
      if (evt  != NULL) evt[i]  = ((int32_t)(value >> (pSize+cSize))) & evtMask;
    }
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array, where the data is
 *  aligned to 64-bit word boundaries.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input value array
 *  @param chan   (IN)  Input channel tag array (null if n/a)
 *  @param evt    (IN)  Input event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packIntWord64 (void *ptr, int32_t off, const int16_t *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 64 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  int64_t valMask  = (sign)? ((int64_t)-1) : ((int64_t)__INT64_C(0xFFFF));
  if (dSize == 32) {
    // When using Int32/UInt32 the first 32-bits will always the the Int32/UInt32
    // value, so use 32-bit operations to improve performance
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int32_t val   = (int32_t)(((int64_t)array[i]) & valMask);
      int32_t value = 0;
      if (chan != NULL) value  = (chan[i] & chanMask) << (pSize);
      if (evt  != NULL) value |= (evt[i]  & evtMask ) << (pSize+cSize);
      packInt(buf, off,   val);
      packInt(buf, off+4, value);
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int64_t val   = (int64_t)(((int64_t)array[i]) & valMask);
      int64_t value = val << (64-dSize);
      if (chan != NULL) value |= ((int64_t)(chan[i] & chanMask)) << (pSize);
      if (evt  != NULL) value |= ((int64_t)(evt[i]  & evtMask )) << (pSize+cSize);
      packLong(buf, off, value);
    }
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array, where the data is
 *  aligned to 64-bit word boundaries.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Bit offset into buf (must be multiple of 64 bits)
 *  @param array  (OUT) Output value array (not null)
 *  @param chan   (OUT) Output channel tag array (null if n/a)
 *  @param evt    (OUT) Output event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackFloatWord64 (const void *ptr, int32_t off, int16_t *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 64 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(dSize);
  // When using Float32 the first 32-bits will always the the Float32 value, so
  // use 32-bit operations to improve performance
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=8) {
    array[i] = (int16_t)unpackFloat(buf, off);
    int32_t value = unpackInt(buf, off+4);
    if (chan != NULL) chan[i] = (value >> (pSize      )) & chanMask;
    if (evt  != NULL) evt[i]  = (value >> (pSize+cSize)) & evtMask;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array, where the data is
 *  aligned to 64-bit word boundaries.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input value array
 *  @param chan   (IN)  Input channel tag array (null if n/a)
 *  @param evt    (IN)  Input event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packFloatWord64 (void *ptr, int32_t off, const int16_t *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 64 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(dSize);
  // When using Float32 the first 32-bits will always the the Float32 value, so
  // use 32-bit operations to improve performance
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=8) {
    float val   = (float)array[i];
    int32_t   value = 0;
    if (chan != NULL) value  = (chan[i] & chanMask) << (pSize);
    if (evt  != NULL) value |= (evt[i]  & evtMask ) << (pSize+cSize);
    packFloat(buf, off,   val);
    packInt(  buf, off+4, value);
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array, where the data is
 *  aligned to 32-bit word boundaries.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Bit offset into buf (must be multiple of 32 bits)
 *  @param array  (OUT) Output value array (not null)
 *  @param chan   (OUT) Output channel tag array (null if n/a)
 *  @param evt    (OUT) Output event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackVRTFloatWord32 (const void *ptr, int32_t off, int16_t *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 32 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=4) {
    int32_t value = unpackInt(buf, off);
    int32_t bits  = (value >> (32-dSize)); // fromVRTFloat32(..) will mask off any extra bits
    array[i] = (int16_t)fromVRTFloat32(sign, expBits, dSize, bits);
    if (chan != NULL) chan[i] = (value >> (pSize      )) & chanMask;
    if (evt  != NULL) evt[i]  = (value >> (pSize+cSize)) & evtMask;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array, where the data is
 *  aligned to 32-bit word boundaries.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input value array
 *  @param chan   (IN)  Input channel tag array (null if n/a)
 *  @param evt    (IN)  Input event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packVRTFloatWord32 (void *ptr, int32_t off, const int16_t *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 32 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=4) {
    int32_t val   = toVRTFloat32(sign, expBits, dSize, (double)array[i]);
    int32_t value = val << (32-dSize);
    if (chan != NULL) value |= (chan[i] & chanMask) << (pSize);
    if (evt  != NULL) value |= (evt[i]  & evtMask ) << (pSize+cSize);
    packInt(buf, off, value);
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array, where the data is
 *  aligned to 64-bit word boundaries.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Bit offset into buf (must be multiple of 64 bits)
 *  @param array  (OUT) Output value array (not null)
 *  @param chan   (OUT) Output channel tag array (null if n/a)
 *  @param evt    (OUT) Output event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackVRTFloatWord64 (const void *ptr, int32_t off, int16_t *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 64 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  if (dSize == 32) {
    // When using Int32/UInt32 the first 32-bits will always the the Int32/UInt32
    // value, so use 32-bit operations to improve performance
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int32_t val   = unpackInt(buf, off);
      int32_t value = unpackInt(buf, off+4);
      array[i] = (int16_t)fromVRTFloat32(sign, expBits, dSize, val);
      if (chan != NULL) chan[i] = ((int32_t)(value >> (pSize      ))) & chanMask;
      if (evt  != NULL) evt[i]  = ((int32_t)(value >> (pSize+cSize))) & evtMask;
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int64_t value = unpackLong(buf, off);
      int64_t bits  = (value >> (64-dSize)); // fromVRTFloat64(..) will mask off any extra bits
      array[i] = (int16_t)fromVRTFloat64(sign, expBits, dSize, bits);
      if (chan != NULL) chan[i] = ((int32_t)(value >> (pSize      ))) & chanMask;
      if (evt  != NULL) evt[i]  = ((int32_t)(value >> (pSize+cSize))) & evtMask;
    }
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array, where the data is
 *  aligned to 64-bit word boundaries.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input value array
 *  @param chan   (IN)  Input channel tag array (null if n/a)
 *  @param evt    (IN)  Input event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packVRTFloatWord64 (void *ptr, int32_t off, const int16_t *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 64 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  if (dSize == 32) {
    // When using Int32/UInt32 the first 32-bits will always the the Int32/UInt32
    // value, so use 32-bit operations to improve performance
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int32_t val   = toVRTFloat32(sign, expBits, dSize, (double)array[i]);
      int32_t value = 0;
      if (chan != NULL) value  = (chan[i] & chanMask) << (pSize);
      if (evt  != NULL) value |= (evt[i]  & evtMask ) << (pSize+cSize);
      packInt(buf, off,   val);
      packInt(buf, off+4, value);
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int64_t val   = toVRTFloat64(sign, expBits, dSize, (double)array[i]);
      int64_t value = val << (64-dSize);
      if (chan != NULL) value |= ((int64_t)(chan[i] & chanMask)) << (pSize);
      if (evt  != NULL) value |= ((int64_t)(evt[i]  & evtMask )) << (pSize+cSize);
      packLong(buf, off, value);
    }
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array.
 *  @param ptr       (IN)  Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf (must be multiple of 32 bits)
 *  @param array     (OUT) Output value array (not null)
 *  @param chan      (OUT) Output channel tag array (null if n/a)
 *  @param evt       (OUT) Output event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackIntBuffer32 (const void *ptr, int32_t bitOffset, int16_t *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  const int32_t bitmask      = 32 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t _bits_dSize  = 32 - dSize;    // pre-compute "32 - dSize" for efficiency
  int32_t _bits_fSize  = 32 - fSize;    // pre-compute "32 - fSize" for efficiency
  int32_t _fSize_dSize = fSize - dSize;         // pre-compute "fSize - dSize" for efficiency
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      // Not enough bits remaining in this word, move to next word (skipping pad bits)
      bitOffset = (bitOffset + 32) & bitmaskInv;
    }
    int32_t bits  = unpackBits32(buf, bitOffset, fSize);
    int32_t value = bits >> _fSize_dSize;
    // sign extend as needed (otherwise leave top bits 0)
    array[i] = (sign)? (int16_t)((value << _bits_dSize) >>  _bits_dSize)
                     : (int16_t)value;
    if (evt  != NULL) evt[i]  = ((int32_t)(bits >> cSize)) & eMask;
    if (chan != NULL) chan[i] = ((int32_t)(bits         )) & cMask;
    bitOffset += fSize;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array.
 *  @param ptr       (I/O) Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf
 *  @param array     (IN)  Input value array
 *  @param chan      (IN)  Input channel tag array (null if n/a)
 *  @param evt       (IN)  Input event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void packIntBuffer32 (void *ptr, int32_t bitOffset, const int16_t *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  const int32_t bitmask      = 32 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t uSize        = fSize - dSize - eSize - cSize;
  int32_t _bits_fSize  = 32 - fSize;    // pre-compute "32 - fSize" for efficiency
  int32_t valMask      = (sign)? ((int32_t)-1) : ((int32_t)__INT64_C(0xFFFF)); // Bit mask for input value (controls sign-extend)
  if ((evt == NULL) || (eSize == 0)) {
    // Treat event tag as unused since we have nothing to insert there (this
    // will be slightly faster in the loop below).
    uSize = uSize + eSize;
    eSize = 0;
    evt   = NULL;
    if (chan == NULL) {
      // Treat channel tag as unused since we have nothing to insert there (this
      // will be slightly faster in the loop below).
      uSize = uSize + cSize;
      cSize = 0;
      chan  = NULL;
    }
  }
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  int32_t _uSize_eSize = uSize + eSize;         // pre-compute "uSize + eSize" for efficiency
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      if (true) { //TODO: make this optional
        // Not enough bits remaining in this word, move to next word (clearing pad bits)
        int32_t zSize = 32 - (bitOffset & bitmask); // bits remaining to set
        packBits32(buf, bitOffset, zSize, 0);
        bitOffset += zSize;
      }
      else {
        // Not enough bits remaining in this word, move to next word (skipping pad bits)
        bitOffset = (bitOffset + 32) & bitmaskInv;
      }
    }
    int32_t value = (int32_t)(((int32_t)array[i]) & valMask);
    value = value << _uSize_eSize;
    if (evt != NULL) value = value | (evt[i] & eMask);
    value = value << cSize;
    if (chan != NULL) value = value | (chan[i] & cMask);
    packBits32(buf, bitOffset, fSize, value);
    bitOffset += fSize;
  }
  // ==== End of Packet Pad Bits ===============================================
  // Clear out any bits remaining in the current word that would be at the end
  // of the packet, this applies to both link and processing efficient.
  if (true) { //TODO: make this optional
    int32_t xSize = 32 - (bitOffset & bitmask); // bits remaining to set
    if (xSize > 0) {
      packBits32(buf, bitOffset, xSize, 0);
      bitOffset += xSize;
    }
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array.
 *  @param ptr       (IN)  Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf (must be multiple of 64 bits)
 *  @param array     (OUT) Output value array (not null)
 *  @param chan      (OUT) Output channel tag array (null if n/a)
 *  @param evt       (OUT) Output event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackIntBuffer64 (const void *ptr, int32_t bitOffset, int16_t *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  const int32_t bitmask      = 64 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t _bits_dSize  = 64 - dSize;    // pre-compute "64 - dSize" for efficiency
  int32_t _bits_fSize  = 64 - fSize;    // pre-compute "64 - fSize" for efficiency
  int32_t _fSize_dSize = fSize - dSize;         // pre-compute "fSize - dSize" for efficiency
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      // Not enough bits remaining in this word, move to next word (skipping pad bits)
      bitOffset = (bitOffset + 32) & bitmaskInv;
    }
    int64_t bits  = unpackBits64(buf, bitOffset, fSize);
    int64_t value = bits >> _fSize_dSize;
    // sign extend as needed (otherwise leave top bits 0)
    array[i] = (sign)? (int16_t)((value << _bits_dSize) >>  _bits_dSize)
                     : (int16_t)value;
    if (evt  != NULL) evt[i]  = ((int32_t)(bits >> cSize)) & eMask;
    if (chan != NULL) chan[i] = ((int32_t)(bits         )) & cMask;
    bitOffset += fSize;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array.
 *  @param ptr       (I/O) Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf
 *  @param array     (IN)  Input value array
 *  @param chan      (IN)  Input channel tag array (null if n/a)
 *  @param evt       (IN)  Input event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void packIntBuffer64 (void *ptr, int32_t bitOffset, const int16_t *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  const int32_t bitmask      = 64 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t uSize        = fSize - dSize - eSize - cSize;
  int32_t _bits_fSize  = 64 - fSize;    // pre-compute "64 - fSize" for efficiency
  int64_t valMask      = (sign)? ((int64_t)-1) : ((int64_t)__INT64_C(0xFFFF)); // Bit mask for input value (controls sign-extend)
  if ((evt == NULL) || (eSize == 0)) {
    // Treat event tag as unused since we have nothing to insert there (this
    // will be slightly faster in the loop below).
    uSize = uSize + eSize;
    eSize = 0;
    evt   = NULL;
    if (chan == NULL) {
      // Treat channel tag as unused since we have nothing to insert there (this
      // will be slightly faster in the loop below).
      uSize = uSize + cSize;
      cSize = 0;
      chan  = NULL;
    }
  }
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  int32_t _uSize_eSize = uSize + eSize;         // pre-compute "uSize + eSize" for efficiency
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      if (true) { //TODO: make this optional
        // Not enough bits remaining in this word, move to next word (clearing pad bits)
        int32_t zSize = 64 - (bitOffset & bitmask); // bits remaining to set
        packBits64(buf, bitOffset, zSize, 0);
        bitOffset += zSize;
      }
      else {
        // Not enough bits remaining in this word, move to next word (skipping pad bits)
        bitOffset = (bitOffset + 32) & bitmaskInv;
      }
    }
    int64_t value = (int64_t)(((int64_t)array[i]) & valMask);
    value = value << _uSize_eSize;
    if (evt != NULL) value = value | (evt[i] & eMask);
    value = value << cSize;
    if (chan != NULL) value = value | (chan[i] & cMask);
    packBits64(buf, bitOffset, fSize, value);
    bitOffset += fSize;
  }
  // ==== End of Packet Pad Bits ===============================================
  // Clear out any bits remaining in the current word that would be at the end
  // of the packet, this applies to both link and processing efficient.
  if (true) { //TODO: make this optional
    int32_t xSize = 64 - (bitOffset & bitmask); // bits remaining to set
    if (xSize > 0) {
      packBits64(buf, bitOffset, xSize, 0);
      bitOffset += xSize;
    }
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array.
 *  @param ptr       (IN)  Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf (must be multiple of 64 bits)
 *  @param array     (OUT) Output value array (not null)
 *  @param chan      (OUT) Output channel tag array (null if n/a)
 *  @param evt       (OUT) Output event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackFloatBuffer64 (const void *ptr, int32_t bitOffset, int16_t *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  const int32_t bitmask      = 64 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t _bits_fSize  = 64 - fSize;    // pre-compute "64 - fSize" for efficiency
  int32_t _fSize_dSize = fSize - dSize;         // pre-compute "fSize - dSize" for efficiency
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      // Not enough bits remaining in this word, move to next word (skipping pad bits)
      bitOffset = (bitOffset + 32) & bitmaskInv;
    }
    int64_t bits  = unpackBits64(buf, bitOffset, fSize);
    int64_t value = bits >> _fSize_dSize;
    UNUSED_VARIABLE(sign);
    array[i] = (int16_t)intBitsToFloat((int32_t)value);
    if (evt  != NULL) evt[i]  = ((int32_t)(bits >> cSize)) & eMask;
    if (chan != NULL) chan[i] = ((int32_t)(bits         )) & cMask;
    bitOffset += fSize;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array.
 *  @param ptr       (I/O) Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf
 *  @param array     (IN)  Input value array
 *  @param chan      (IN)  Input channel tag array (null if n/a)
 *  @param evt       (IN)  Input event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void packFloatBuffer64 (void *ptr, int32_t bitOffset, const int16_t *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  const int32_t bitmask      = 64 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t uSize        = fSize - dSize - eSize - cSize;
  int32_t _bits_fSize  = 64 - fSize;    // pre-compute "64 - fSize" for efficiency
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  if ((evt == NULL) || (eSize == 0)) {
    // Treat event tag as unused since we have nothing to insert there (this
    // will be slightly faster in the loop below).
    uSize = uSize + eSize;
    eSize = 0;
    evt   = NULL;
    if (chan == NULL) {
      // Treat channel tag as unused since we have nothing to insert there (this
      // will be slightly faster in the loop below).
      uSize = uSize + cSize;
      cSize = 0;
      chan  = NULL;
    }
  }
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  int32_t _uSize_eSize = uSize + eSize;         // pre-compute "uSize + eSize" for efficiency
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      if (true) { //TODO: make this optional
        // Not enough bits remaining in this word, move to next word (clearing pad bits)
        int32_t zSize = 64 - (bitOffset & bitmask); // bits remaining to set
        packBits64(buf, bitOffset, zSize, 0);
        bitOffset += zSize;
      }
      else {
        // Not enough bits remaining in this word, move to next word (skipping pad bits)
        bitOffset = (bitOffset + 32) & bitmaskInv;
      }
    }
    int64_t value = (int64_t)floatToRawIntBits((float)array[i]);
    value = value << _uSize_eSize;
    if (evt != NULL) value = value | (evt[i] & eMask);
    value = value << cSize;
    if (chan != NULL) value = value | (chan[i] & cMask);
    packBits64(buf, bitOffset, fSize, value);
    bitOffset += fSize;
  }
  // ==== End of Packet Pad Bits ===============================================
  // Clear out any bits remaining in the current word that would be at the end
  // of the packet, this applies to both link and processing efficient.
  if (true) { //TODO: make this optional
    int32_t xSize = 64 - (bitOffset & bitmask); // bits remaining to set
    if (xSize > 0) {
      packBits64(buf, bitOffset, xSize, 0);
      bitOffset += xSize;
    }
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array.
 *  @param ptr       (IN)  Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf (must be multiple of 32 bits)
 *  @param array     (OUT) Output value array (not null)
 *  @param chan      (OUT) Output channel tag array (null if n/a)
 *  @param evt       (OUT) Output event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackVRTFloatBuffer32 (const void *ptr, int32_t bitOffset, int16_t *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  const char *buf = (const char*)ptr;
  const int32_t bitmask      = 32 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t _bits_fSize  = 32 - fSize;    // pre-compute "32 - fSize" for efficiency
  int32_t _fSize_dSize = fSize - dSize;         // pre-compute "fSize - dSize" for efficiency
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      // Not enough bits remaining in this word, move to next word (skipping pad bits)
      bitOffset = (bitOffset + 32) & bitmaskInv;
    }
    int32_t bits  = unpackBits32(buf, bitOffset, fSize);
    int32_t value = bits >> _fSize_dSize;
    array[i] = (int16_t)fromVRTFloat32(sign, expBits, dSize, value);
    if (evt  != NULL) evt[i]  = ((int32_t)(bits >> cSize)) & eMask;
    if (chan != NULL) chan[i] = ((int32_t)(bits         )) & cMask;
    bitOffset += fSize;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array.
 *  @param ptr       (I/O) Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf
 *  @param array     (IN)  Input value array
 *  @param chan      (IN)  Input channel tag array (null if n/a)
 *  @param evt       (IN)  Input event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void packVRTFloatBuffer32 (void *ptr, int32_t bitOffset, const int16_t *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  char *buf = (char*)ptr;
  const int32_t bitmask      = 32 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t uSize        = fSize - dSize - eSize - cSize;
  int32_t _bits_fSize  = 32 - fSize;    // pre-compute "32 - fSize" for efficiency
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  if ((evt == NULL) || (eSize == 0)) {
    // Treat event tag as unused since we have nothing to insert there (this
    // will be slightly faster in the loop below).
    uSize = uSize + eSize;
    eSize = 0;
    evt   = NULL;
    if (chan == NULL) {
      // Treat channel tag as unused since we have nothing to insert there (this
      // will be slightly faster in the loop below).
      uSize = uSize + cSize;
      cSize = 0;
      chan  = NULL;
    }
  }
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  int32_t _uSize_eSize = uSize + eSize;         // pre-compute "uSize + eSize" for efficiency
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      if (true) { //TODO: make this optional
        // Not enough bits remaining in this word, move to next word (clearing pad bits)
        int32_t zSize = 32 - (bitOffset & bitmask); // bits remaining to set
        packBits32(buf, bitOffset, zSize, 0);
        bitOffset += zSize;
      }
      else {
        // Not enough bits remaining in this word, move to next word (skipping pad bits)
        bitOffset = (bitOffset + 32) & bitmaskInv;
      }
    }
    int32_t value = toVRTFloat32(sign, expBits, dSize, (double)array[i]);
    value = value << _uSize_eSize;
    if (evt != NULL) value = value | (evt[i] & eMask);
    value = value << cSize;
    if (chan != NULL) value = value | (chan[i] & cMask);
    packBits32(buf, bitOffset, fSize, value);
    bitOffset += fSize;
  }
  // ==== End of Packet Pad Bits ===============================================
  // Clear out any bits remaining in the current word that would be at the end
  // of the packet, this applies to both link and processing efficient.
  if (true) { //TODO: make this optional
    int32_t xSize = 32 - (bitOffset & bitmask); // bits remaining to set
    if (xSize > 0) {
      packBits32(buf, bitOffset, xSize, 0);
      bitOffset += xSize;
    }
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array.
 *  @param ptr       (IN)  Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf (must be multiple of 64 bits)
 *  @param array     (OUT) Output value array (not null)
 *  @param chan      (OUT) Output channel tag array (null if n/a)
 *  @param evt       (OUT) Output event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackVRTFloatBuffer64 (const void *ptr, int32_t bitOffset, int16_t *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  const char *buf = (const char*)ptr;
  const int32_t bitmask      = 64 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t _bits_fSize  = 64 - fSize;    // pre-compute "64 - fSize" for efficiency
  int32_t _fSize_dSize = fSize - dSize;         // pre-compute "fSize - dSize" for efficiency
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      // Not enough bits remaining in this word, move to next word (skipping pad bits)
      bitOffset = (bitOffset + 32) & bitmaskInv;
    }
    int64_t bits  = unpackBits64(buf, bitOffset, fSize);
    int64_t value = bits >> _fSize_dSize;
    array[i] = (int16_t)fromVRTFloat64(sign, expBits, dSize, value);
    if (evt  != NULL) evt[i]  = ((int32_t)(bits >> cSize)) & eMask;
    if (chan != NULL) chan[i] = ((int32_t)(bits         )) & cMask;
    bitOffset += fSize;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array.
 *  @param ptr       (I/O) Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf
 *  @param array     (IN)  Input value array
 *  @param chan      (IN)  Input channel tag array (null if n/a)
 *  @param evt       (IN)  Input event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void packVRTFloatBuffer64 (void *ptr, int32_t bitOffset, const int16_t *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  char *buf = (char*)ptr;
  const int32_t bitmask      = 64 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t uSize        = fSize - dSize - eSize - cSize;
  int32_t _bits_fSize  = 64 - fSize;    // pre-compute "64 - fSize" for efficiency
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  if ((evt == NULL) || (eSize == 0)) {
    // Treat event tag as unused since we have nothing to insert there (this
    // will be slightly faster in the loop below).
    uSize = uSize + eSize;
    eSize = 0;
    evt   = NULL;
    if (chan == NULL) {
      // Treat channel tag as unused since we have nothing to insert there (this
      // will be slightly faster in the loop below).
      uSize = uSize + cSize;
      cSize = 0;
      chan  = NULL;
    }
  }
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  int32_t _uSize_eSize = uSize + eSize;         // pre-compute "uSize + eSize" for efficiency
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      if (true) { //TODO: make this optional
        // Not enough bits remaining in this word, move to next word (clearing pad bits)
        int32_t zSize = 64 - (bitOffset & bitmask); // bits remaining to set
        packBits64(buf, bitOffset, zSize, 0);
        bitOffset += zSize;
      }
      else {
        // Not enough bits remaining in this word, move to next word (skipping pad bits)
        bitOffset = (bitOffset + 32) & bitmaskInv;
      }
    }
    int64_t value = toVRTFloat64(sign, expBits, dSize, (double)array[i]);
    value = value << _uSize_eSize;
    if (evt != NULL) value = value | (evt[i] & eMask);
    value = value << cSize;
    if (chan != NULL) value = value | (chan[i] & cMask);
    packBits64(buf, bitOffset, fSize, value);
    bitOffset += fSize;
  }
  // ==== End of Packet Pad Bits ===============================================
  // Clear out any bits remaining in the current word that would be at the end
  // of the packet, this applies to both link and processing efficient.
  if (true) { //TODO: make this optional
    int32_t xSize = 64 - (bitOffset & bitmask); // bits remaining to set
    if (xSize > 0) {
      packBits64(buf, bitOffset, xSize, 0);
      bitOffset += xSize;
    }
  }
}
#if NOT_USING_JNI
int16_t* vrt::PackUnpack::unpackAsShort (PayloadFormat pf, const void *ptr, int32_t off, int16_t *array, int32_t *chan, int32_t *evt, int32_t length) {
  const char *buf = (const char*)ptr;
  if (!pf.isValid()) {
    throw VRTException(pf.getValid());
  }
  int32_t            fSize  = pf.getItemPackingFieldSize();
  int32_t            dSize  = pf.getDataItemSize();
  int32_t            eSize  = pf.getEventTagSize();
  int32_t            cSize  = pf.getChannelTagSize();
  int32_t            xSize  = fSize; // effective fSize
  bool        proc   = pf.isProcessingEfficient();
  bool        sign   = pf.isSigned();
  DataItemFormat format = pf.getDataItemFormat();
  bool        noTags;
  if (eSize == 0) evt  = NULL; // no event tags, ignore any array passed in
  if (cSize == 0) chan = NULL; // no channel tags, ignore any array passed in
  noTags = ((chan == NULL) && (evt == NULL)) || (fSize == dSize);
  if (proc) {
         if (fSize >= 33) xSize = 64;
    else if (fSize >= 17) xSize = 32;
    if (noTags) fSize = xSize; // Use faster field size if ignoring Event & Channel Tags
  }
  switch (format) {
    case DataItemFormat_SignedInt:   // FALLTHROUGH
    case DataItemFormat_UnsignedInt:
      // ---- Try fast methods -------------------------------------------------
      if (noTags) {
        switch (fSize) {
          case  8: unpackBytes(  buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
          case 16: unpackShorts( buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
          case 24: unpackInt24s( buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
          case 32: unpackInts(   buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
          case 64: unpackLongs(  buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
        }
      }
      if (fSize == dSize) { // implicit noTags=true in this case
        switch (fSize) {
          case  1: if ((length % 8) == 0) { unpackBits(   buf, off, array, 0, length, fSize, dSize, proc, sign); return array; } break;
          case  4: if ((length % 2) == 0) { unpackNibbles(buf, off, array, 0, length, fSize, dSize, proc, sign); return array; } break;
          case 12: if ((length % 2) == 0) { unpackInt12s( buf, off, array, 0, length, fSize, dSize, proc, sign); return array; } break;
        }
      }
      if (xSize == 32) {
        unpackIntWord32(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return array;
      }
      if (xSize == 64) {
        unpackIntWord64(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return array;
      }
      // ---- Fall back to slow method -----------------------------------------
      if (fSize <= 32) {
        unpackIntBuffer32(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return array;
      }
      else {
        unpackIntBuffer64(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return array;
      }
    case DataItemFormat_Float:
      // ---- Try fast methods -------------------------------------------------
      if (fSize == dSize) { // implicit noTags=true in this case
        unpackFloats(buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
      }
      if (xSize == 64) {
        unpackFloatWord64(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return array;
      }
      // ---- Fall back to slow method -----------------------------------------
      unpackFloatBuffer64(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return array;
    case DataItemFormat_Double:
      // ---- Only fast method for double (yea!) -------------------------------
      unpackDoubles(buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
    default: // VRTFloat Types
      // ---- Try fast methods -------------------------------------------------
      if (xSize == 32) {
        unpackVRTFloatWord32(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return array;
      }
      if (xSize == 64) {
        unpackVRTFloatWord64(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return array;
      }
      // ---- Fall back to slow method -----------------------------------------
      if (fSize <= 32) {
        unpackVRTFloatBuffer32(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return array;
      }
      else {
        unpackVRTFloatBuffer64(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return array;
      }
  }
}
void vrt::PackUnpack::packAsShort (PayloadFormat pf, void *ptr, int32_t off, const int16_t *array, const int32_t *chan, const int32_t *evt, int32_t length) {
  char *buf = (char*)ptr;
  if (!pf.isValid()) {
    throw VRTException(pf.getValid());
  }
  int32_t            fSize  = pf.getItemPackingFieldSize();
  int32_t            dSize  = pf.getDataItemSize();
  int32_t            eSize  = pf.getEventTagSize();
  int32_t            cSize  = pf.getChannelTagSize();
  int32_t            xSize  = fSize; // effective fSize
  bool        proc   = pf.isProcessingEfficient();
  bool        sign   = pf.isSigned();
  DataItemFormat format = pf.getDataItemFormat();
  bool        noTags;
  if (eSize == 0) evt  = NULL; // no event tags, ignore any array passed in
  if (cSize == 0) chan = NULL; // no channel tags, ignore any array passed in
  noTags = ((chan == NULL) && (evt == NULL)) || (fSize == dSize);
  if (proc) {
         if (fSize >= 33) xSize = 64;
    else if (fSize >= 17) xSize = 32;
    if (noTags) fSize = xSize; // Use faster field size if ignoring Event & Channel Tags
  }
  switch (format) {
    case DataItemFormat_SignedInt:   // FALLTHROUGH
    case DataItemFormat_UnsignedInt:
      // ---- Try fast methods -------------------------------------------------
      if (noTags) {
        switch (fSize) {
          case  8: packBytes(  buf, off, array, 0, length, fSize, dSize, proc, sign); return;
          case 16: packShorts( buf, off, array, 0, length, fSize, dSize, proc, sign); return;
          case 24: packInt24s( buf, off, array, 0, length, fSize, dSize, proc, sign); return;
          case 32: packInts(   buf, off, array, 0, length, fSize, dSize, proc, sign); return;
          case 64: packLongs(  buf, off, array, 0, length, fSize, dSize, proc, sign); return;
        }
      }
      if (fSize == dSize) { // implicit noTags=true in this case
        switch (fSize) {
          case  1: if ((length % 8) == 0) { packBits(   buf, off, array, 0, length, fSize, dSize, proc, sign); return; } break;
          case  4: if ((length % 2) == 0) { packNibbles(buf, off, array, 0, length, fSize, dSize, proc, sign); return; } break;
          case 12: if ((length % 2) == 0) { packInt12s( buf, off, array, 0, length, fSize, dSize, proc, sign); return; } break;
        }
      }
      if (xSize == 32) {
        packIntWord32(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return;
      }
      if (xSize == 64) {
        packIntWord64(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return;
      }
      // ---- Fall back to slow method -----------------------------------------
      if (fSize <= 32) {
        packIntBuffer32(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return;
      }
      else {
        packIntBuffer64(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return;
      }
    case DataItemFormat_Float:
      // ---- Try fast methods -------------------------------------------------
      if (fSize == dSize) { // implicit noTags=true in this case
        packFloats(buf, off, array, 0, length, fSize, dSize, proc, sign); return;
      }
      if (xSize == 64) {
        packFloatWord64(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return;
      }
      // ---- Fall back to slow method -----------------------------------------
      packFloatBuffer64(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return;
    case DataItemFormat_Double:
      // ---- Only fast method for double (yea!) -------------------------------
      packDoubles(buf, off, array, 0, length, fSize, dSize, proc, sign); return;
    default: // VRTFloat Types
      // ---- Try fast methods -------------------------------------------------
      if (xSize == 32) {
        packVRTFloatWord32(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return;
      }
      if (xSize == 64) {
        packVRTFloatWord64(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return;
      }
      // ---- Fall back to slow method -----------------------------------------
      if (fSize <= 32) {
        packVRTFloatBuffer32(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return;
      }
      else {
        packVRTFloatBuffer64(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return;
      }
  }
}
#endif /* NOT_USING_JNI */
/** <b>Internal Use Only:</b> Fast unpack method for double data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits (must be 64)
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed? (must be true)
 */
static __attribute__((flatten)) void unpackDoubles (const void *ptr, int32_t off, int8_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  // All floating-point values are signed, so 'sign' is unused, but is included
  // in function definition for consistency.
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(align);
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  // When using Double data, it is required that dSize=64 and fSize=64 so we can
  // skip some of the normal checks we would otherwise do here.
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=8) {
    array[offset++] = (int8_t)unpackDouble(buf, off);
  }
}
/** <b>Internal Use Only:</b>
 *  Fast pack method for double data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits (must be 64)
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed? (must be true)
 */
static __attribute__((flatten)) void packDoubles (void *ptr, int32_t off, const int8_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  // All floating-point values are signed, so 'sign' is unused, but is included
  // in function definition for consistency.
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(align);
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  // When using Double data, it is required that dSize=64 and fSize=64 so we can
  // skip some of the normal checks we would otherwise do here.
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=8) {
    packDouble(buf, off, (double)array[offset++]);
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for float data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits (must be 32)
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed? (must be true)
 */
static __attribute__((flatten)) void unpackFloats (const void *ptr, int32_t off, int8_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  // All floating-point values are signed, so 'sign' is unused, but is included
  // in function definition for consistency.
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(dSize);
  int32_t delta; // int8_t offset delta
  if (align) delta = (fSize <= 32)? 4 : 8;
  else       delta = fSize / 8;
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=delta) {
    array[offset++] = (int8_t)unpackFloat(buf, off);
  }
}
/** <b>Internal Use Only:</b>
 *  Fast pack method for float data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits (must be 32)
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed? (must be true)
 */
static __attribute__((flatten)) void packFloats (void *ptr, int32_t off, const int8_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  // All floating-point values are signed, so 'sign' is unused, but is included
  // in function definition for consistency.
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(dSize);
  int32_t delta; // int8_t offset delta
  if (align) delta = (fSize <= 32)? 4 : 8;
  else       delta = fSize / 8;
  if (delta == 4) {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=4) {
      packFloat(buf, off, (float)array[offset++]);
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++) {
      packFloat(buf, off, (float)array[offset++]);
      off += 4;
      _Intel_Pragma("loop_count min=1, max=4")
      for (int32_t j = 4; j < delta; j++) {
        buf[off++] = 0; // set unused bits to zero
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed int64_t data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void unpackLongs (const void *ptr, int32_t off, int8_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  int32_t delta = (align)? ((fSize <= 32)? 4 : 8) : (fSize/8); // int8_t offset delta
  if (sign) {
    if (dSize == 64) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int8_t)unpackLong(buf, off);
      }
    }
    else {
      int32_t shift  = 64 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int8_t)(unpackLong(buf, off) >> shift);
      }
    }
  }
  else {
    if (dSize == 64) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int8_t)(unpackLong(buf, off) & __INT64_C(0xFFFFFFFFFFFFFFFF));
      }
    }
    else {
      int32_t shift  = 64 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int8_t)((unpackLong(buf, off) & __INT64_C(0xFFFFFFFFFFFFFFFF)) >> shift);
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for unsigned int64_t data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void packLongs (void *ptr, int32_t off, const int8_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  int32_t delta = (align)? ((fSize <= 32)? 4 : 8) : (fSize/8); // int8_t offset delta
  int64_t valMask = (sign)? ((int64_t)-1) : ((int64_t)__INT64_C(0xFF)); // Bit mask for input value (controls sign-extend)
  if (dSize == 64) {
    if (delta == 8) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=8) {
        int64_t val = (int64_t)(((int64_t)array[offset++]) & valMask);
        packLong(buf, off, val);
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int64_t val = (int64_t)(((int64_t)array[offset++]) & valMask);
        packLong(buf, off, val);
        off += 8;
        _Intel_Pragma("loop_count min=1, max=0")
        for (int32_t j = 8; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
  else {
    int32_t shift = 64 - dSize;
    if (delta == 8) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=8) {
        int64_t val = (int64_t)(((int64_t)array[offset++]) & valMask);
        packLong(buf, off, (int64_t)(val << shift));
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int64_t val = (int64_t)(((int64_t)array[offset++]) & valMask);
        packLong(buf, off, (int64_t)(val << shift));
        off += 8;
        _Intel_Pragma("loop_count min=1, max=0")
        for (int32_t j = 8; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed int32_t data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void unpackInts (const void *ptr, int32_t off, int8_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  int32_t delta = (align)? ((fSize <= 32)? 4 : 8) : (fSize/8); // int8_t offset delta
  if (sign) {
    if (dSize == 32) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int8_t)unpackInt(buf, off);
      }
    }
    else {
      int32_t shift  = 32 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int8_t)(unpackInt(buf, off) >> shift);
      }
    }
  }
  else {
    if (dSize == 32) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int8_t)(unpackInt(buf, off) & 0xFFFFFFFF);
      }
    }
    else {
      int32_t shift  = 32 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int8_t)((unpackInt(buf, off) & 0xFFFFFFFF) >> shift);
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for unsigned int32_t data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void packInts (void *ptr, int32_t off, const int8_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  int32_t delta = (align)? ((fSize <= 32)? 4 : 8) : (fSize/8); // int8_t offset delta
  int32_t valMask = (sign)? ((int32_t)-1) : ((int32_t)__INT64_C(0xFF)); // Bit mask for input value (controls sign-extend)
  if (dSize == 32) {
    if (delta == 4) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=4) {
        int32_t val = (int32_t)(((int32_t)array[offset++]) & valMask);
        packInt(buf, off, val);
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int32_t val = (int32_t)(((int32_t)array[offset++]) & valMask);
        packInt(buf, off, val);
        off += 4;
        _Intel_Pragma("loop_count min=1, max=4")
        for (int32_t j = 4; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
  else {
    int32_t shift = 32 - dSize;
    if (delta == 4) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=4) {
        int32_t val = (int32_t)(((int32_t)array[offset++]) & valMask);
        packInt(buf, off, (int32_t)(val << shift));
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int32_t val = (int32_t)(((int32_t)array[offset++]) & valMask);
        packInt(buf, off, (int32_t)(val << shift));
        off += 4;
        _Intel_Pragma("loop_count min=1, max=4")
        for (int32_t j = 4; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed int32_t data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void unpackInt24s (const void *ptr, int32_t off, int8_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  int32_t delta = (align)? ((fSize <= 32)? 4 : 8) : (fSize/8); // int8_t offset delta
  if (sign) {
    if (dSize == 24) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int8_t)unpackInt24(buf, off);
      }
    }
    else {
      int32_t shift  = 24 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int8_t)(unpackInt24(buf, off) >> shift);
      }
    }
  }
  else {
    if (dSize == 24) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int8_t)(unpackInt24(buf, off) & 0xFFFFFF);
      }
    }
    else {
      int32_t shift  = 24 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int8_t)((unpackInt24(buf, off) & 0xFFFFFF) >> shift);
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for unsigned int32_t data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void packInt24s (void *ptr, int32_t off, const int8_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  int32_t delta = (align)? ((fSize <= 32)? 4 : 8) : (fSize/8); // int8_t offset delta
  int32_t valMask = (sign)? ((int32_t)-1) : ((int32_t)__INT64_C(0xFF)); // Bit mask for input value (controls sign-extend)
  if (dSize == 24) {
    if (delta == 3) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=3) {
        int32_t val = (int32_t)(((int32_t)array[offset++]) & valMask);
        packInt24(buf, off, val);
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int32_t val = (int32_t)(((int32_t)array[offset++]) & valMask);
        packInt24(buf, off, val);
        off += 3;
        _Intel_Pragma("loop_count min=1, max=5")
        for (int32_t j = 3; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
  else {
    int32_t shift = 24 - dSize;
    if (delta == 3) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=3) {
        int32_t val = (int32_t)(((int32_t)array[offset++]) & valMask);
        packInt24(buf, off, (int32_t)(val << shift));
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int32_t val = (int32_t)(((int32_t)array[offset++]) & valMask);
        packInt24(buf, off, (int32_t)(val << shift));
        off += 3;
        _Intel_Pragma("loop_count min=1, max=5")
        for (int32_t j = 3; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed int16_t data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void unpackShorts (const void *ptr, int32_t off, int8_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  int32_t delta = fSize / 8; // int8_t offset delta
  if (sign) {
    if (dSize == 16) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int8_t)unpackShort(buf, off);
      }
    }
    else {
      int32_t shift  = 16 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int8_t)(unpackShort(buf, off) >> shift);
      }
    }
  }
  else {
    if (dSize == 16) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int8_t)(unpackShort(buf, off) & 0xFFFF);
      }
    }
    else {
      int32_t shift  = 16 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int8_t)((unpackShort(buf, off) & 0xFFFF) >> shift);
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for unsigned int16_t data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void packShorts (void *ptr, int32_t off, const int8_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(align);
  int32_t delta = fSize / 8; // int8_t offset delta
  int16_t valMask = (sign)? ((int16_t)-1) : ((int16_t)__INT64_C(0xFF)); // Bit mask for input value (controls sign-extend)
  if (dSize == 16) {
    if (delta == 2) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=2) {
        int16_t val = (int16_t)(((int16_t)array[offset++]) & valMask);
        packShort(buf, off, val);
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int16_t val = (int16_t)(((int16_t)array[offset++]) & valMask);
        packShort(buf, off, val);
        off += 2;
        _Intel_Pragma("loop_count min=1, max=6")
        for (int32_t j = 2; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
  else {
    int32_t shift = 16 - dSize;
    if (delta == 2) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=2) {
        int16_t val = (int16_t)(((int16_t)array[offset++]) & valMask);
        packShort(buf, off, (int16_t)(val << shift));
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int16_t val = (int16_t)(((int16_t)array[offset++]) & valMask);
        packShort(buf, off, (int16_t)(val << shift));
        off += 2;
        _Intel_Pragma("loop_count min=1, max=6")
        for (int32_t j = 2; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed Int12 data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert (must be a multiple of 2)
 *  @param fSize  (IN)  Item packing field size in bits (must be 12)
 *  @param dSize  (IN)  Data item size in bits (must be 12)
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackInt12s (const void *ptr, int32_t off, int8_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  int32_t skip = (align)? 1 : 0;
  if (sign) {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i+=2) {
      int32_t a = (int32_t)buf[off++];
      int32_t b = (int32_t)buf[off++];
      int32_t c = (int32_t)buf[off++];
      int32_t x = (a << 4) | ((b >> 4) & 0x000F);
      int32_t y = (b << 8) | ((c     ) & 0x00FF);
      array[offset++] = (int8_t)((x << 20) >> 20); // sign extend
      array[offset++] = (int8_t)((y << 20) >> 20); // sign extend
      off += skip; // skip extra int8_t in processing-efficient mode
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i+=2) {
      int32_t a = (int32_t)buf[off++];
      int32_t b = (int32_t)buf[off++];
      int32_t c = (int32_t)buf[off++];
      int32_t x = (a << 4) | ((b >> 4) & 0x000F);
      int32_t y = (b << 8) | ((c     ) & 0x00FF);
      array[offset++] = (int8_t)(x & 0x0FFF); // clear high-order bits
      array[offset++] = (int8_t)(y & 0x0FFF); // clear high-order bits
      off += skip; // skip extra int8_t in processing-efficient mode
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for signed Int12 data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert (must be a multiple of 2)
 *  @param fSize  (IN)  Item packing field size in bits (must be 12)
 *  @param dSize  (IN)  Data item size in bits (must be 12)
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packInt12s (void *ptr, int32_t off, const int8_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  int32_t skip = (align)?  1 : 0;
  int32_t mask = (sign )? -1 : ((int32_t)__INT64_C(0xFF));
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i+=2) {
    int32_t first  = ((int32_t)array[offset++]) & mask;
    int32_t second = ((int32_t)array[offset++]) & mask;
    buf[off++] = (int8_t)(first >> 4);
    buf[off++] = (int8_t)((first << 4) | ((second >> 8) & 0x0F));
    buf[off++] = (int8_t)(second);
    off += skip; // skip extra int8_t in processing-efficient mode
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed int8_t data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void unpackBytes (const void *ptr, int32_t off, int8_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
#if (1 == 1) || (BYTE_ORDER == BIG_ENDIAN)
  if ((fSize == 8) && (dSize == 8)) {
    // Fast array copy
    memcpy(&array[offset], &buf[off], length*1);
    return;
  }
#endif
  UNUSED_VARIABLE(align);
  int32_t delta = fSize / 8; // int8_t offset delta
  if (sign) {
    if (dSize == 8) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int8_t)unpackByte(buf, off);
      }
    }
    else {
      int32_t shift  = 8 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int8_t)(unpackByte(buf, off) >> shift);
      }
    }
  }
  else {
    if (dSize == 8) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int8_t)(unpackByte(buf, off) & 0xFF);
      }
    }
    else {
      int32_t shift  = 8 - dSize;
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=delta) {
        array[offset++] = (int8_t)((unpackByte(buf, off) & 0xFF) >> shift);
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for unsigned int8_t data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits (must be multiple of 8)
 *  @param dSize  (IN)  Data item size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static __attribute__((flatten)) void packBytes (void *ptr, int32_t off, const int8_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
#if (1 == 1) || (BYTE_ORDER == BIG_ENDIAN)
  if ((fSize == 8) && (dSize == 8)) {
    // Fast array copy
    memcpy(&buf[off], &array[offset], length*1);
    return;
  }
#endif
  UNUSED_VARIABLE(align);
  int32_t delta = fSize / 8; // int8_t offset delta
  int8_t valMask = (sign)? ((int8_t)-1) : ((int8_t)__INT64_C(0xFF)); // Bit mask for input value (controls sign-extend)
  if (dSize == 8) {
    if (delta == 1) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=1) {
        int8_t val = (int8_t)(((int8_t)array[offset++]) & valMask);
        packByte(buf, off, val);
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int8_t val = (int8_t)(((int8_t)array[offset++]) & valMask);
        packByte(buf, off, val);
        off += 1;
        _Intel_Pragma("loop_count min=1, max=7")
        for (int32_t j = 1; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
  else {
    int32_t shift = 8 - dSize;
    if (delta == 1) {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++,off+=1) {
        int8_t val = (int8_t)(((int8_t)array[offset++]) & valMask);
        packByte(buf, off, (int8_t)(val << shift));
      }
    }
    else {
      _Intel_Pragma("parallel")
      for (int32_t i = 0; i < length; i++) {
        int8_t val = (int8_t)(((int8_t)array[offset++]) & valMask);
        packByte(buf, off, (int8_t)(val << shift));
        off += 1;
        _Intel_Pragma("loop_count min=1, max=7")
        for (int32_t j = 1; j < delta; j++) {
          buf[off++] = 0; // set unused bits to zero
        }
      }
    }
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for signed Int4 (nibble) data.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert (must be a multiple of 2)
 *  @param fSize  (IN)  Item packing field size in bits (must be 4)
 *  @param dSize  (IN)  Data item size in bits (must be 4)
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackNibbles (const void *ptr, int32_t off, int8_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  if (sign) {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i+=2) {
      int32_t b = (int32_t)buf[off++];
      array[offset++] = (int8_t)((b << 24) >> 28); // shift & sign extend
      array[offset++] = (int8_t)((b << 28) >> 28); // shift & sign extend
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i+=2) {
      int32_t b = (int32_t)buf[off++];
      array[offset++] = (int8_t)((b >> 4) & 0xF);
      array[offset++] = (int8_t)((b     ) & 0xF);
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for signed Int4 (nibble) data.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert (must be a multiple of 2)
 *  @param fSize  (IN)  Item packing field size in bits (must be 4)
 *  @param dSize  (IN)  Data item size in bits (must be 4)
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packNibbles (void *ptr, int32_t off, const int8_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(align);
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  // Pack function for signed and unsigned are identical for nibble data, hence
  // 'sign' is unused here, but included in function definition for consistency.
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i+=2) {
    int32_t first  = (int32_t)array[offset++];
    int32_t second = (int32_t)array[offset++];
    buf[off++] = (int8_t)((first << 4) | (second & 0xF));
  }
}
/** <b>Internal Use Only:</b> Fast unpack method for bit data. Each bit in the
 *  input buffer will be checked, a 0 bit will correspond to the value 0 and a
 *  1 bit will correspond to the value 1.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (OUT) Output array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert (must be a multiple of 8)
 *  @param fSize  (IN)  Item packing field size in bits (must be 1)
 *  @param dSize  (IN)  Data item size in bits (must be 1)
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackBits (const void *ptr, int32_t off, int8_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(align);
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  if (sign) {
    // This uses sign-extending principles to get each bit as 0 or -1 which reflects
    // the fact that 0/-1 and not 0/+1 are the available values in a one-bit use of
    // twos-complement.
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i+=8) {
      int32_t b = (int32_t)buf[off++];
      array[offset++] = (int8_t)((b << 24) >> 31);
      array[offset++] = (int8_t)((b << 25) >> 31);
      array[offset++] = (int8_t)((b << 26) >> 31);
      array[offset++] = (int8_t)((b << 27) >> 31);
      array[offset++] = (int8_t)((b << 28) >> 31);
      array[offset++] = (int8_t)((b << 29) >> 31);
      array[offset++] = (int8_t)((b << 30) >> 31);
      array[offset++] = (int8_t)((b << 31) >> 31);
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i+=8) {
      int32_t b = (int32_t)buf[off++];
      array[offset++] = (int8_t)((b >> 7) & 0x1);
      array[offset++] = (int8_t)((b >> 6) & 0x1);
      array[offset++] = (int8_t)((b >> 5) & 0x1);
      array[offset++] = (int8_t)((b >> 4) & 0x1);
      array[offset++] = (int8_t)((b >> 3) & 0x1);
      array[offset++] = (int8_t)((b >> 2) & 0x1);
      array[offset++] = (int8_t)((b >> 1) & 0x1);
      array[offset++] = (int8_t)((b     ) & 0x1);
    }
  }
}
/** <b>Internal Use Only:</b> Fast pack method for bit data. Each value in the
 *  input array will be checked, a zero value will correspond to the bit 0 and
 *  any non-zero value will correspond to the bit 1.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input array
 *  @param offset (IN)  Offset into array
 *  @param length (IN)  Number of elements to convert (must be a multiple of 8)
 *  @param fSize  (IN)  Item packing field size in bits (must be 1)
 *  @param dSize  (IN)  Data item size in bits (must be 1)
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packBits (void *ptr, int32_t off, const int8_t *array, int32_t offset, int32_t length, int32_t fSize, int32_t dSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(align);
  UNUSED_VARIABLE(dSize);
  UNUSED_VARIABLE(fSize);
  // Pack function for signed and unsigned are identical for bit data, hence
  // 'sign' is unused here, but included in function definition for consistency.
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i+=8) {
    int32_t b = 0;
    b |= (((int32_t)array[offset++]) << 7) & 0x80;
    b |= (((int32_t)array[offset++]) << 6) & 0x40;
    b |= (((int32_t)array[offset++]) << 5) & 0x20;
    b |= (((int32_t)array[offset++]) << 4) & 0x10;
    b |= (((int32_t)array[offset++]) << 3) & 0x08;
    b |= (((int32_t)array[offset++]) << 2) & 0x04;
    b |= (((int32_t)array[offset++]) << 1) & 0x02;
    b |= (((int32_t)array[offset++])     ) & 0x01;
    buf[off++] = (int8_t)b;
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array, where the data is
 *  aligned to 32-bit word boundaries.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Bit offset into buf (must be multiple of 32 bits)
 *  @param array  (OUT) Output value array (not null)
 *  @param chan   (OUT) Output channel tag array (null if n/a)
 *  @param evt    (OUT) Output event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackIntWord32 (const void *ptr, int32_t off, int8_t *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 32 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  int32_t valMask  = (sign)? ((int32_t)-1) : (~(((int32_t)-1) << dSize));
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=4) {
    int32_t value = unpackInt(buf, off);
    array[i] = (int8_t)((value >> (32-dSize)) & valMask);
    if (chan != NULL) chan[i] = (value >> (pSize      )) & chanMask;
    if (evt  != NULL) evt[i]  = (value >> (pSize+cSize)) & evtMask;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array, where the data is
 *  aligned to 32-bit word boundaries.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input value array
 *  @param chan   (IN)  Input channel tag array (null if n/a)
 *  @param evt    (IN)  Input event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packIntWord32 (void *ptr, int32_t off, const int8_t *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 32 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  int32_t valMask  = (sign)? ((int32_t)-1) : ((int32_t)__INT64_C(0xFF));
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=4) {
    int32_t val   = (int32_t)(((int32_t)array[i]) & valMask);
    int32_t value = val << (32-dSize);
    if (chan != NULL) value |= (chan[i] & chanMask) << (pSize);
    if (evt  != NULL) value |= (evt[i]  & evtMask ) << (pSize+cSize);
    packInt(buf, off, value);
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array, where the data is
 *  aligned to 64-bit word boundaries.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Bit offset into buf (must be multiple of 64 bits)
 *  @param array  (OUT) Output value array (not null)
 *  @param chan   (OUT) Output channel tag array (null if n/a)
 *  @param evt    (OUT) Output event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackIntWord64 (const void *ptr, int32_t off, int8_t *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 64 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  int64_t valMask  = (sign)? ((int64_t)-1) : (~(((int64_t)-1) << dSize));
  if (dSize == 32) {
    // When using Int32/UInt32 the first 32-bits will always the the Int32/UInt32
    // value, so use 32-bit operations to improve performance
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int32_t val   = unpackInt(buf, off);
      int32_t value = unpackInt(buf, off+4);
      array[i] = (int8_t)(val & valMask);
      if (chan != NULL) chan[i] = ((int32_t)(value >> (pSize      ))) & chanMask;
      if (evt  != NULL) evt[i]  = ((int32_t)(value >> (pSize+cSize))) & evtMask;
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int64_t value = unpackLong(buf, off);
      array[i] = (int8_t)((value >> (64-dSize)) & valMask);
      if (chan != NULL) chan[i] = ((int32_t)(value >> (pSize      ))) & chanMask;
      if (evt  != NULL) evt[i]  = ((int32_t)(value >> (pSize+cSize))) & evtMask;
    }
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array, where the data is
 *  aligned to 64-bit word boundaries.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input value array
 *  @param chan   (IN)  Input channel tag array (null if n/a)
 *  @param evt    (IN)  Input event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packIntWord64 (void *ptr, int32_t off, const int8_t *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 64 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  int64_t valMask  = (sign)? ((int64_t)-1) : ((int64_t)__INT64_C(0xFF));
  if (dSize == 32) {
    // When using Int32/UInt32 the first 32-bits will always the the Int32/UInt32
    // value, so use 32-bit operations to improve performance
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int32_t val   = (int32_t)(((int64_t)array[i]) & valMask);
      int32_t value = 0;
      if (chan != NULL) value  = (chan[i] & chanMask) << (pSize);
      if (evt  != NULL) value |= (evt[i]  & evtMask ) << (pSize+cSize);
      packInt(buf, off,   val);
      packInt(buf, off+4, value);
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int64_t val   = (int64_t)(((int64_t)array[i]) & valMask);
      int64_t value = val << (64-dSize);
      if (chan != NULL) value |= ((int64_t)(chan[i] & chanMask)) << (pSize);
      if (evt  != NULL) value |= ((int64_t)(evt[i]  & evtMask )) << (pSize+cSize);
      packLong(buf, off, value);
    }
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array, where the data is
 *  aligned to 64-bit word boundaries.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Bit offset into buf (must be multiple of 64 bits)
 *  @param array  (OUT) Output value array (not null)
 *  @param chan   (OUT) Output channel tag array (null if n/a)
 *  @param evt    (OUT) Output event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackFloatWord64 (const void *ptr, int32_t off, int8_t *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 64 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(dSize);
  // When using Float32 the first 32-bits will always the the Float32 value, so
  // use 32-bit operations to improve performance
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=8) {
    array[i] = (int8_t)unpackFloat(buf, off);
    int32_t value = unpackInt(buf, off+4);
    if (chan != NULL) chan[i] = (value >> (pSize      )) & chanMask;
    if (evt  != NULL) evt[i]  = (value >> (pSize+cSize)) & evtMask;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array, where the data is
 *  aligned to 64-bit word boundaries.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input value array
 *  @param chan   (IN)  Input channel tag array (null if n/a)
 *  @param evt    (IN)  Input event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packFloatWord64 (void *ptr, int32_t off, const int8_t *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 64 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  UNUSED_VARIABLE(sign);
  UNUSED_VARIABLE(dSize);
  // When using Float32 the first 32-bits will always the the Float32 value, so
  // use 32-bit operations to improve performance
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=8) {
    float val   = (float)array[i];
    int32_t   value = 0;
    if (chan != NULL) value  = (chan[i] & chanMask) << (pSize);
    if (evt  != NULL) value |= (evt[i]  & evtMask ) << (pSize+cSize);
    packFloat(buf, off,   val);
    packInt(  buf, off+4, value);
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array, where the data is
 *  aligned to 32-bit word boundaries.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Bit offset into buf (must be multiple of 32 bits)
 *  @param array  (OUT) Output value array (not null)
 *  @param chan   (OUT) Output channel tag array (null if n/a)
 *  @param evt    (OUT) Output event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackVRTFloatWord32 (const void *ptr, int32_t off, int8_t *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 32 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=4) {
    int32_t value = unpackInt(buf, off);
    int32_t bits  = (value >> (32-dSize)); // fromVRTFloat32(..) will mask off any extra bits
    array[i] = (int8_t)fromVRTFloat32(sign, expBits, dSize, bits);
    if (chan != NULL) chan[i] = (value >> (pSize      )) & chanMask;
    if (evt  != NULL) evt[i]  = (value >> (pSize+cSize)) & evtMask;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array, where the data is
 *  aligned to 32-bit word boundaries.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input value array
 *  @param chan   (IN)  Input channel tag array (null if n/a)
 *  @param evt    (IN)  Input event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packVRTFloatWord32 (void *ptr, int32_t off, const int8_t *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 32 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  _Intel_Pragma("parallel")
  for (int32_t i = 0; i < length; i++,off+=4) {
    int32_t val   = toVRTFloat32(sign, expBits, dSize, (double)array[i]);
    int32_t value = val << (32-dSize);
    if (chan != NULL) value |= (chan[i] & chanMask) << (pSize);
    if (evt  != NULL) value |= (evt[i]  & evtMask ) << (pSize+cSize);
    packInt(buf, off, value);
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array, where the data is
 *  aligned to 64-bit word boundaries.
 *  @param ptr    (IN)  Packet payload buffer pointer
 *  @param off    (IN)  Bit offset into buf (must be multiple of 64 bits)
 *  @param array  (OUT) Output value array (not null)
 *  @param chan   (OUT) Output channel tag array (null if n/a)
 *  @param evt    (OUT) Output event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackVRTFloatWord64 (const void *ptr, int32_t off, int8_t *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  const char *buf = (const char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 64 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  if (dSize == 32) {
    // When using Int32/UInt32 the first 32-bits will always the the Int32/UInt32
    // value, so use 32-bit operations to improve performance
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int32_t val   = unpackInt(buf, off);
      int32_t value = unpackInt(buf, off+4);
      array[i] = (int8_t)fromVRTFloat32(sign, expBits, dSize, val);
      if (chan != NULL) chan[i] = ((int32_t)(value >> (pSize      ))) & chanMask;
      if (evt  != NULL) evt[i]  = ((int32_t)(value >> (pSize+cSize))) & evtMask;
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int64_t value = unpackLong(buf, off);
      int64_t bits  = (value >> (64-dSize)); // fromVRTFloat64(..) will mask off any extra bits
      array[i] = (int8_t)fromVRTFloat64(sign, expBits, dSize, bits);
      if (chan != NULL) chan[i] = ((int32_t)(value >> (pSize      ))) & chanMask;
      if (evt  != NULL) evt[i]  = ((int32_t)(value >> (pSize+cSize))) & evtMask;
    }
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array, where the data is
 *  aligned to 64-bit word boundaries.
 *  @param ptr    (I/O) Packet payload buffer pointer
 *  @param off    (IN)  Offset into buf
 *  @param array  (IN)  Input value array
 *  @param chan   (IN)  Input channel tag array (null if n/a)
 *  @param evt    (IN)  Input event tag array (null if n/a)
 *  @param length (IN)  Number of elements to convert
 *  @param fSize  (IN)  Item packing field size in bits
 *  @param dSize  (IN)  Data item size in bits
 *  @param eSize  (IN)  Event tag size in bits
 *  @param cSize  (IN)  Channel tag size in bits
 *  @param align  (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign   (IN)  Is value signed (true) or unsigned (false)?
 */
static void packVRTFloatWord64 (void *ptr, int32_t off, const int8_t *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  char *buf = (char*)ptr;
  UNUSED_VARIABLE(align);
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+- - - - - +
  //   | Data Item | Unused | Event Tag | Channel Tag | Pad Bits |
  //   +-----------+--------+-----------+-------------+- - - - - +
  //       dSize      uSize     eSize       cSize        pSize
  //   |------------------ fSize ---------------------|
  //   |---------------------- Word Size ------------------------|
  int32_t pSize    = 64 - fSize;
  int32_t evtMask  = ~(((int32_t)-1) << eSize);
  int32_t chanMask = ~(((int32_t)-1) << cSize);
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  if (dSize == 32) {
    // When using Int32/UInt32 the first 32-bits will always the the Int32/UInt32
    // value, so use 32-bit operations to improve performance
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int32_t val   = toVRTFloat32(sign, expBits, dSize, (double)array[i]);
      int32_t value = 0;
      if (chan != NULL) value  = (chan[i] & chanMask) << (pSize);
      if (evt  != NULL) value |= (evt[i]  & evtMask ) << (pSize+cSize);
      packInt(buf, off,   val);
      packInt(buf, off+4, value);
    }
  }
  else {
    _Intel_Pragma("parallel")
    for (int32_t i = 0; i < length; i++,off+=8) {
      int64_t val   = toVRTFloat64(sign, expBits, dSize, (double)array[i]);
      int64_t value = val << (64-dSize);
      if (chan != NULL) value |= ((int64_t)(chan[i] & chanMask)) << (pSize);
      if (evt  != NULL) value |= ((int64_t)(evt[i]  & evtMask )) << (pSize+cSize);
      packLong(buf, off, value);
    }
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array.
 *  @param ptr       (IN)  Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf (must be multiple of 32 bits)
 *  @param array     (OUT) Output value array (not null)
 *  @param chan      (OUT) Output channel tag array (null if n/a)
 *  @param evt       (OUT) Output event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackIntBuffer32 (const void *ptr, int32_t bitOffset, int8_t *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  const int32_t bitmask      = 32 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t _bits_dSize  = 32 - dSize;    // pre-compute "32 - dSize" for efficiency
  int32_t _bits_fSize  = 32 - fSize;    // pre-compute "32 - fSize" for efficiency
  int32_t _fSize_dSize = fSize - dSize;         // pre-compute "fSize - dSize" for efficiency
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      // Not enough bits remaining in this word, move to next word (skipping pad bits)
      bitOffset = (bitOffset + 32) & bitmaskInv;
    }
    int32_t bits  = unpackBits32(buf, bitOffset, fSize);
    int32_t value = bits >> _fSize_dSize;
    // sign extend as needed (otherwise leave top bits 0)
    array[i] = (sign)? (int8_t)((value << _bits_dSize) >>  _bits_dSize)
                     : (int8_t)value;
    if (evt  != NULL) evt[i]  = ((int32_t)(bits >> cSize)) & eMask;
    if (chan != NULL) chan[i] = ((int32_t)(bits         )) & cMask;
    bitOffset += fSize;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array.
 *  @param ptr       (I/O) Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf
 *  @param array     (IN)  Input value array
 *  @param chan      (IN)  Input channel tag array (null if n/a)
 *  @param evt       (IN)  Input event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void packIntBuffer32 (void *ptr, int32_t bitOffset, const int8_t *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  const int32_t bitmask      = 32 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t uSize        = fSize - dSize - eSize - cSize;
  int32_t _bits_fSize  = 32 - fSize;    // pre-compute "32 - fSize" for efficiency
  int32_t valMask      = (sign)? ((int32_t)-1) : ((int32_t)__INT64_C(0xFF)); // Bit mask for input value (controls sign-extend)
  if ((evt == NULL) || (eSize == 0)) {
    // Treat event tag as unused since we have nothing to insert there (this
    // will be slightly faster in the loop below).
    uSize = uSize + eSize;
    eSize = 0;
    evt   = NULL;
    if (chan == NULL) {
      // Treat channel tag as unused since we have nothing to insert there (this
      // will be slightly faster in the loop below).
      uSize = uSize + cSize;
      cSize = 0;
      chan  = NULL;
    }
  }
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  int32_t _uSize_eSize = uSize + eSize;         // pre-compute "uSize + eSize" for efficiency
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      if (true) { //TODO: make this optional
        // Not enough bits remaining in this word, move to next word (clearing pad bits)
        int32_t zSize = 32 - (bitOffset & bitmask); // bits remaining to set
        packBits32(buf, bitOffset, zSize, 0);
        bitOffset += zSize;
      }
      else {
        // Not enough bits remaining in this word, move to next word (skipping pad bits)
        bitOffset = (bitOffset + 32) & bitmaskInv;
      }
    }
    int32_t value = (int32_t)(((int32_t)array[i]) & valMask);
    value = value << _uSize_eSize;
    if (evt != NULL) value = value | (evt[i] & eMask);
    value = value << cSize;
    if (chan != NULL) value = value | (chan[i] & cMask);
    packBits32(buf, bitOffset, fSize, value);
    bitOffset += fSize;
  }
  // ==== End of Packet Pad Bits ===============================================
  // Clear out any bits remaining in the current word that would be at the end
  // of the packet, this applies to both link and processing efficient.
  if (true) { //TODO: make this optional
    int32_t xSize = 32 - (bitOffset & bitmask); // bits remaining to set
    if (xSize > 0) {
      packBits32(buf, bitOffset, xSize, 0);
      bitOffset += xSize;
    }
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array.
 *  @param ptr       (IN)  Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf (must be multiple of 64 bits)
 *  @param array     (OUT) Output value array (not null)
 *  @param chan      (OUT) Output channel tag array (null if n/a)
 *  @param evt       (OUT) Output event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackIntBuffer64 (const void *ptr, int32_t bitOffset, int8_t *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  const int32_t bitmask      = 64 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t _bits_dSize  = 64 - dSize;    // pre-compute "64 - dSize" for efficiency
  int32_t _bits_fSize  = 64 - fSize;    // pre-compute "64 - fSize" for efficiency
  int32_t _fSize_dSize = fSize - dSize;         // pre-compute "fSize - dSize" for efficiency
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      // Not enough bits remaining in this word, move to next word (skipping pad bits)
      bitOffset = (bitOffset + 32) & bitmaskInv;
    }
    int64_t bits  = unpackBits64(buf, bitOffset, fSize);
    int64_t value = bits >> _fSize_dSize;
    // sign extend as needed (otherwise leave top bits 0)
    array[i] = (sign)? (int8_t)((value << _bits_dSize) >>  _bits_dSize)
                     : (int8_t)value;
    if (evt  != NULL) evt[i]  = ((int32_t)(bits >> cSize)) & eMask;
    if (chan != NULL) chan[i] = ((int32_t)(bits         )) & cMask;
    bitOffset += fSize;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array.
 *  @param ptr       (I/O) Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf
 *  @param array     (IN)  Input value array
 *  @param chan      (IN)  Input channel tag array (null if n/a)
 *  @param evt       (IN)  Input event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void packIntBuffer64 (void *ptr, int32_t bitOffset, const int8_t *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  const int32_t bitmask      = 64 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t uSize        = fSize - dSize - eSize - cSize;
  int32_t _bits_fSize  = 64 - fSize;    // pre-compute "64 - fSize" for efficiency
  int64_t valMask      = (sign)? ((int64_t)-1) : ((int64_t)__INT64_C(0xFF)); // Bit mask for input value (controls sign-extend)
  if ((evt == NULL) || (eSize == 0)) {
    // Treat event tag as unused since we have nothing to insert there (this
    // will be slightly faster in the loop below).
    uSize = uSize + eSize;
    eSize = 0;
    evt   = NULL;
    if (chan == NULL) {
      // Treat channel tag as unused since we have nothing to insert there (this
      // will be slightly faster in the loop below).
      uSize = uSize + cSize;
      cSize = 0;
      chan  = NULL;
    }
  }
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  int32_t _uSize_eSize = uSize + eSize;         // pre-compute "uSize + eSize" for efficiency
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      if (true) { //TODO: make this optional
        // Not enough bits remaining in this word, move to next word (clearing pad bits)
        int32_t zSize = 64 - (bitOffset & bitmask); // bits remaining to set
        packBits64(buf, bitOffset, zSize, 0);
        bitOffset += zSize;
      }
      else {
        // Not enough bits remaining in this word, move to next word (skipping pad bits)
        bitOffset = (bitOffset + 32) & bitmaskInv;
      }
    }
    int64_t value = (int64_t)(((int64_t)array[i]) & valMask);
    value = value << _uSize_eSize;
    if (evt != NULL) value = value | (evt[i] & eMask);
    value = value << cSize;
    if (chan != NULL) value = value | (chan[i] & cMask);
    packBits64(buf, bitOffset, fSize, value);
    bitOffset += fSize;
  }
  // ==== End of Packet Pad Bits ===============================================
  // Clear out any bits remaining in the current word that would be at the end
  // of the packet, this applies to both link and processing efficient.
  if (true) { //TODO: make this optional
    int32_t xSize = 64 - (bitOffset & bitmask); // bits remaining to set
    if (xSize > 0) {
      packBits64(buf, bitOffset, xSize, 0);
      bitOffset += xSize;
    }
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array.
 *  @param ptr       (IN)  Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf (must be multiple of 64 bits)
 *  @param array     (OUT) Output value array (not null)
 *  @param chan      (OUT) Output channel tag array (null if n/a)
 *  @param evt       (OUT) Output event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackFloatBuffer64 (const void *ptr, int32_t bitOffset, int8_t *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  const char *buf = (const char*)ptr;
  const int32_t bitmask      = 64 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t _bits_fSize  = 64 - fSize;    // pre-compute "64 - fSize" for efficiency
  int32_t _fSize_dSize = fSize - dSize;         // pre-compute "fSize - dSize" for efficiency
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      // Not enough bits remaining in this word, move to next word (skipping pad bits)
      bitOffset = (bitOffset + 32) & bitmaskInv;
    }
    int64_t bits  = unpackBits64(buf, bitOffset, fSize);
    int64_t value = bits >> _fSize_dSize;
    UNUSED_VARIABLE(sign);
    array[i] = (int8_t)intBitsToFloat((int32_t)value);
    if (evt  != NULL) evt[i]  = ((int32_t)(bits >> cSize)) & eMask;
    if (chan != NULL) chan[i] = ((int32_t)(bits         )) & cMask;
    bitOffset += fSize;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array.
 *  @param ptr       (I/O) Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf
 *  @param array     (IN)  Input value array
 *  @param chan      (IN)  Input channel tag array (null if n/a)
 *  @param evt       (IN)  Input event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void packFloatBuffer64 (void *ptr, int32_t bitOffset, const int8_t *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign) {
  char *buf = (char*)ptr;
  const int32_t bitmask      = 64 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t uSize        = fSize - dSize - eSize - cSize;
  int32_t _bits_fSize  = 64 - fSize;    // pre-compute "64 - fSize" for efficiency
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  if ((evt == NULL) || (eSize == 0)) {
    // Treat event tag as unused since we have nothing to insert there (this
    // will be slightly faster in the loop below).
    uSize = uSize + eSize;
    eSize = 0;
    evt   = NULL;
    if (chan == NULL) {
      // Treat channel tag as unused since we have nothing to insert there (this
      // will be slightly faster in the loop below).
      uSize = uSize + cSize;
      cSize = 0;
      chan  = NULL;
    }
  }
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  int32_t _uSize_eSize = uSize + eSize;         // pre-compute "uSize + eSize" for efficiency
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      if (true) { //TODO: make this optional
        // Not enough bits remaining in this word, move to next word (clearing pad bits)
        int32_t zSize = 64 - (bitOffset & bitmask); // bits remaining to set
        packBits64(buf, bitOffset, zSize, 0);
        bitOffset += zSize;
      }
      else {
        // Not enough bits remaining in this word, move to next word (skipping pad bits)
        bitOffset = (bitOffset + 32) & bitmaskInv;
      }
    }
    int64_t value = (int64_t)floatToRawIntBits((float)array[i]);
    value = value << _uSize_eSize;
    if (evt != NULL) value = value | (evt[i] & eMask);
    value = value << cSize;
    if (chan != NULL) value = value | (chan[i] & cMask);
    packBits64(buf, bitOffset, fSize, value);
    bitOffset += fSize;
  }
  // ==== End of Packet Pad Bits ===============================================
  // Clear out any bits remaining in the current word that would be at the end
  // of the packet, this applies to both link and processing efficient.
  if (true) { //TODO: make this optional
    int32_t xSize = 64 - (bitOffset & bitmask); // bits remaining to set
    if (xSize > 0) {
      packBits64(buf, bitOffset, xSize, 0);
      bitOffset += xSize;
    }
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array.
 *  @param ptr       (IN)  Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf (must be multiple of 32 bits)
 *  @param array     (OUT) Output value array (not null)
 *  @param chan      (OUT) Output channel tag array (null if n/a)
 *  @param evt       (OUT) Output event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackVRTFloatBuffer32 (const void *ptr, int32_t bitOffset, int8_t *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  const char *buf = (const char*)ptr;
  const int32_t bitmask      = 32 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t _bits_fSize  = 32 - fSize;    // pre-compute "32 - fSize" for efficiency
  int32_t _fSize_dSize = fSize - dSize;         // pre-compute "fSize - dSize" for efficiency
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      // Not enough bits remaining in this word, move to next word (skipping pad bits)
      bitOffset = (bitOffset + 32) & bitmaskInv;
    }
    int32_t bits  = unpackBits32(buf, bitOffset, fSize);
    int32_t value = bits >> _fSize_dSize;
    array[i] = (int8_t)fromVRTFloat32(sign, expBits, dSize, value);
    if (evt  != NULL) evt[i]  = ((int32_t)(bits >> cSize)) & eMask;
    if (chan != NULL) chan[i] = ((int32_t)(bits         )) & cMask;
    bitOffset += fSize;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array.
 *  @param ptr       (I/O) Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf
 *  @param array     (IN)  Input value array
 *  @param chan      (IN)  Input channel tag array (null if n/a)
 *  @param evt       (IN)  Input event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void packVRTFloatBuffer32 (void *ptr, int32_t bitOffset, const int8_t *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  char *buf = (char*)ptr;
  const int32_t bitmask      = 32 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t uSize        = fSize - dSize - eSize - cSize;
  int32_t _bits_fSize  = 32 - fSize;    // pre-compute "32 - fSize" for efficiency
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  if ((evt == NULL) || (eSize == 0)) {
    // Treat event tag as unused since we have nothing to insert there (this
    // will be slightly faster in the loop below).
    uSize = uSize + eSize;
    eSize = 0;
    evt   = NULL;
    if (chan == NULL) {
      // Treat channel tag as unused since we have nothing to insert there (this
      // will be slightly faster in the loop below).
      uSize = uSize + cSize;
      cSize = 0;
      chan  = NULL;
    }
  }
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  int32_t _uSize_eSize = uSize + eSize;         // pre-compute "uSize + eSize" for efficiency
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      if (true) { //TODO: make this optional
        // Not enough bits remaining in this word, move to next word (clearing pad bits)
        int32_t zSize = 32 - (bitOffset & bitmask); // bits remaining to set
        packBits32(buf, bitOffset, zSize, 0);
        bitOffset += zSize;
      }
      else {
        // Not enough bits remaining in this word, move to next word (skipping pad bits)
        bitOffset = (bitOffset + 32) & bitmaskInv;
      }
    }
    int32_t value = toVRTFloat32(sign, expBits, dSize, (double)array[i]);
    value = value << _uSize_eSize;
    if (evt != NULL) value = value | (evt[i] & eMask);
    value = value << cSize;
    if (chan != NULL) value = value | (chan[i] & cMask);
    packBits32(buf, bitOffset, fSize, value);
    bitOffset += fSize;
  }
  // ==== End of Packet Pad Bits ===============================================
  // Clear out any bits remaining in the current word that would be at the end
  // of the packet, this applies to both link and processing efficient.
  if (true) { //TODO: make this optional
    int32_t xSize = 32 - (bitOffset & bitmask); // bits remaining to set
    if (xSize > 0) {
      packBits32(buf, bitOffset, xSize, 0);
      bitOffset += xSize;
    }
  }
}
/** <b>Internal Use Only:</b> Gets the data into an array.
 *  @param ptr       (IN)  Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf (must be multiple of 64 bits)
 *  @param array     (OUT) Output value array (not null)
 *  @param chan      (OUT) Output channel tag array (null if n/a)
 *  @param evt       (OUT) Output event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or link efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void unpackVRTFloatBuffer64 (const void *ptr, int32_t bitOffset, int8_t *array, int32_t *chan, int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  const char *buf = (const char*)ptr;
  const int32_t bitmask      = 64 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t _bits_fSize  = 64 - fSize;    // pre-compute "64 - fSize" for efficiency
  int32_t _fSize_dSize = fSize - dSize;         // pre-compute "fSize - dSize" for efficiency
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      // Not enough bits remaining in this word, move to next word (skipping pad bits)
      bitOffset = (bitOffset + 32) & bitmaskInv;
    }
    int64_t bits  = unpackBits64(buf, bitOffset, fSize);
    int64_t value = bits >> _fSize_dSize;
    array[i] = (int8_t)fromVRTFloat64(sign, expBits, dSize, value);
    if (evt  != NULL) evt[i]  = ((int32_t)(bits >> cSize)) & eMask;
    if (chan != NULL) chan[i] = ((int32_t)(bits         )) & cMask;
    bitOffset += fSize;
  }
}
/** <b>Internal Use Only:</b> Sets the data from an array.
 *  @param ptr       (I/O) Packet payload buffer pointer
 *  @param bitOffset (IN)  Bit offset into buf
 *  @param array     (IN)  Input value array
 *  @param chan      (IN)  Input channel tag array (null if n/a)
 *  @param evt       (IN)  Input event tag array (null if n/a)
 *  @param length    (IN)  Number of elements to convert
 *  @param fSize     (IN)  Item packing field size in bits
 *  @param dSize     (IN)  Data item size in bits
 *  @param eSize     (IN)  Event tag size in bits
 *  @param cSize     (IN)  Channel tag size in bits
 *  @param align     (IN)  Is processing efficient mode used (true) or packing efficient (false)?
 *  @param sign      (IN)  Is value signed (true) or unsigned (false)?
 */
static void packVRTFloatBuffer64 (void *ptr, int32_t bitOffset, const int8_t *array, const int32_t *chan, const int32_t *evt, int32_t length, int32_t fSize, int32_t dSize, int32_t eSize, int32_t cSize, bool align, bool sign, int32_t expBits) {
  char *buf = (char*)ptr;
  const int32_t bitmask      = 64 - 1;        // Bit mask for container (e.g. 0x0000001F for 32)
  const int32_t bitmaskInv   = ~bitmask;              // Bit mask for container (e.g. 0xFFFFFFE0 for 32)
  // Item Packing Field
  //   +-----------+--------+-----------+-------------+
  //   | Data Item | Unused | Event Tag | Channel Tag |
  //   +-----------+--------+-----------+-------------+
  //       dSize      uSize     eSize       cSize
  //   |------------------ fSize ---------------------|
  int32_t uSize        = fSize - dSize - eSize - cSize;
  int32_t _bits_fSize  = 64 - fSize;    // pre-compute "64 - fSize" for efficiency
  UNUSED_VARIABLE(sign); // don't need to worry about sign-extend
  if ((evt == NULL) || (eSize == 0)) {
    // Treat event tag as unused since we have nothing to insert there (this
    // will be slightly faster in the loop below).
    uSize = uSize + eSize;
    eSize = 0;
    evt   = NULL;
    if (chan == NULL) {
      // Treat channel tag as unused since we have nothing to insert there (this
      // will be slightly faster in the loop below).
      uSize = uSize + cSize;
      cSize = 0;
      chan  = NULL;
    }
  }
  int32_t eMask = (0x1 << eSize) - 1;
  int32_t cMask = (0x1 << cSize) - 1;
  int32_t _uSize_eSize = uSize + eSize;         // pre-compute "uSize + eSize" for efficiency
  for (int32_t i = 0; i < length; i++) {
    if (align && ((bitOffset & bitmask) > _bits_fSize)) {
      if (true) { //TODO: make this optional
        // Not enough bits remaining in this word, move to next word (clearing pad bits)
        int32_t zSize = 64 - (bitOffset & bitmask); // bits remaining to set
        packBits64(buf, bitOffset, zSize, 0);
        bitOffset += zSize;
      }
      else {
        // Not enough bits remaining in this word, move to next word (skipping pad bits)
        bitOffset = (bitOffset + 32) & bitmaskInv;
      }
    }
    int64_t value = toVRTFloat64(sign, expBits, dSize, (double)array[i]);
    value = value << _uSize_eSize;
    if (evt != NULL) value = value | (evt[i] & eMask);
    value = value << cSize;
    if (chan != NULL) value = value | (chan[i] & cMask);
    packBits64(buf, bitOffset, fSize, value);
    bitOffset += fSize;
  }
  // ==== End of Packet Pad Bits ===============================================
  // Clear out any bits remaining in the current word that would be at the end
  // of the packet, this applies to both link and processing efficient.
  if (true) { //TODO: make this optional
    int32_t xSize = 64 - (bitOffset & bitmask); // bits remaining to set
    if (xSize > 0) {
      packBits64(buf, bitOffset, xSize, 0);
      bitOffset += xSize;
    }
  }
}
#if NOT_USING_JNI
int8_t* vrt::PackUnpack::unpackAsByte (PayloadFormat pf, const void *ptr, int32_t off, int8_t *array, int32_t *chan, int32_t *evt, int32_t length) {
  const char *buf = (const char*)ptr;
  if (!pf.isValid()) {
    throw VRTException(pf.getValid());
  }
  int32_t            fSize  = pf.getItemPackingFieldSize();
  int32_t            dSize  = pf.getDataItemSize();
  int32_t            eSize  = pf.getEventTagSize();
  int32_t            cSize  = pf.getChannelTagSize();
  int32_t            xSize  = fSize; // effective fSize
  bool        proc   = pf.isProcessingEfficient();
  bool        sign   = pf.isSigned();
  DataItemFormat format = pf.getDataItemFormat();
  bool        noTags;
  if (eSize == 0) evt  = NULL; // no event tags, ignore any array passed in
  if (cSize == 0) chan = NULL; // no channel tags, ignore any array passed in
  noTags = ((chan == NULL) && (evt == NULL)) || (fSize == dSize);
  if (proc) {
         if (fSize >= 33) xSize = 64;
    else if (fSize >= 17) xSize = 32;
    if (noTags) fSize = xSize; // Use faster field size if ignoring Event & Channel Tags
  }
  switch (format) {
    case DataItemFormat_SignedInt:   // FALLTHROUGH
    case DataItemFormat_UnsignedInt:
      // ---- Try fast methods -------------------------------------------------
      if (noTags) {
        switch (fSize) {
          case  8: unpackBytes(  buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
          case 16: unpackShorts( buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
          case 24: unpackInt24s( buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
          case 32: unpackInts(   buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
          case 64: unpackLongs(  buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
        }
      }
      if (fSize == dSize) { // implicit noTags=true in this case
        switch (fSize) {
          case  1: if ((length % 8) == 0) { unpackBits(   buf, off, array, 0, length, fSize, dSize, proc, sign); return array; } break;
          case  4: if ((length % 2) == 0) { unpackNibbles(buf, off, array, 0, length, fSize, dSize, proc, sign); return array; } break;
          case 12: if ((length % 2) == 0) { unpackInt12s( buf, off, array, 0, length, fSize, dSize, proc, sign); return array; } break;
        }
      }
      if (xSize == 32) {
        unpackIntWord32(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return array;
      }
      if (xSize == 64) {
        unpackIntWord64(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return array;
      }
      // ---- Fall back to slow method -----------------------------------------
      if (fSize <= 32) {
        unpackIntBuffer32(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return array;
      }
      else {
        unpackIntBuffer64(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return array;
      }
    case DataItemFormat_Float:
      // ---- Try fast methods -------------------------------------------------
      if (fSize == dSize) { // implicit noTags=true in this case
        unpackFloats(buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
      }
      if (xSize == 64) {
        unpackFloatWord64(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return array;
      }
      // ---- Fall back to slow method -----------------------------------------
      unpackFloatBuffer64(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return array;
    case DataItemFormat_Double:
      // ---- Only fast method for double (yea!) -------------------------------
      unpackDoubles(buf, off, array, 0, length, fSize, dSize, proc, sign); return array;
    default: // VRTFloat Types
      // ---- Try fast methods -------------------------------------------------
      if (xSize == 32) {
        unpackVRTFloatWord32(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return array;
      }
      if (xSize == 64) {
        unpackVRTFloatWord64(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return array;
      }
      // ---- Fall back to slow method -----------------------------------------
      if (fSize <= 32) {
        unpackVRTFloatBuffer32(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return array;
      }
      else {
        unpackVRTFloatBuffer64(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return array;
      }
  }
}
void vrt::PackUnpack::packAsByte (PayloadFormat pf, void *ptr, int32_t off, const int8_t *array, const int32_t *chan, const int32_t *evt, int32_t length) {
  char *buf = (char*)ptr;
  if (!pf.isValid()) {
    throw VRTException(pf.getValid());
  }
  int32_t            fSize  = pf.getItemPackingFieldSize();
  int32_t            dSize  = pf.getDataItemSize();
  int32_t            eSize  = pf.getEventTagSize();
  int32_t            cSize  = pf.getChannelTagSize();
  int32_t            xSize  = fSize; // effective fSize
  bool        proc   = pf.isProcessingEfficient();
  bool        sign   = pf.isSigned();
  DataItemFormat format = pf.getDataItemFormat();
  bool        noTags;
  if (eSize == 0) evt  = NULL; // no event tags, ignore any array passed in
  if (cSize == 0) chan = NULL; // no channel tags, ignore any array passed in
  noTags = ((chan == NULL) && (evt == NULL)) || (fSize == dSize);
  if (proc) {
         if (fSize >= 33) xSize = 64;
    else if (fSize >= 17) xSize = 32;
    if (noTags) fSize = xSize; // Use faster field size if ignoring Event & Channel Tags
  }
  switch (format) {
    case DataItemFormat_SignedInt:   // FALLTHROUGH
    case DataItemFormat_UnsignedInt:
      // ---- Try fast methods -------------------------------------------------
      if (noTags) {
        switch (fSize) {
          case  8: packBytes(  buf, off, array, 0, length, fSize, dSize, proc, sign); return;
          case 16: packShorts( buf, off, array, 0, length, fSize, dSize, proc, sign); return;
          case 24: packInt24s( buf, off, array, 0, length, fSize, dSize, proc, sign); return;
          case 32: packInts(   buf, off, array, 0, length, fSize, dSize, proc, sign); return;
          case 64: packLongs(  buf, off, array, 0, length, fSize, dSize, proc, sign); return;
        }
      }
      if (fSize == dSize) { // implicit noTags=true in this case
        switch (fSize) {
          case  1: if ((length % 8) == 0) { packBits(   buf, off, array, 0, length, fSize, dSize, proc, sign); return; } break;
          case  4: if ((length % 2) == 0) { packNibbles(buf, off, array, 0, length, fSize, dSize, proc, sign); return; } break;
          case 12: if ((length % 2) == 0) { packInt12s( buf, off, array, 0, length, fSize, dSize, proc, sign); return; } break;
        }
      }
      if (xSize == 32) {
        packIntWord32(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return;
      }
      if (xSize == 64) {
        packIntWord64(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return;
      }
      // ---- Fall back to slow method -----------------------------------------
      if (fSize <= 32) {
        packIntBuffer32(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return;
      }
      else {
        packIntBuffer64(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return;
      }
    case DataItemFormat_Float:
      // ---- Try fast methods -------------------------------------------------
      if (fSize == dSize) { // implicit noTags=true in this case
        packFloats(buf, off, array, 0, length, fSize, dSize, proc, sign); return;
      }
      if (xSize == 64) {
        packFloatWord64(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return;
      }
      // ---- Fall back to slow method -----------------------------------------
      packFloatBuffer64(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign); return;
    case DataItemFormat_Double:
      // ---- Only fast method for double (yea!) -------------------------------
      packDoubles(buf, off, array, 0, length, fSize, dSize, proc, sign); return;
    default: // VRTFloat Types
      // ---- Try fast methods -------------------------------------------------
      if (xSize == 32) {
        packVRTFloatWord32(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return;
      }
      if (xSize == 64) {
        packVRTFloatWord64(buf, off, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return;
      }
      // ---- Fall back to slow method -----------------------------------------
      if (fSize <= 32) {
        packVRTFloatBuffer32(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return;
      }
      else {
        packVRTFloatBuffer64(buf, off*8, array, chan, evt, length, fSize, dSize, eSize, cSize, proc, sign, DataItemFormat_getExponentBits(format)); return;
      }
  }
}
#endif /* NOT_USING_JNI */


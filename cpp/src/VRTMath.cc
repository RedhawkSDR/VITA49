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

#include "VRTMath.h"
#include <wchar.h>
#include "BasicVRTPacket.h"
#if NOT_USING_JNI
# include "InetAddress.h"
# include "Record.h"
# include "TimeStamp.h"
# include "VRTConfig.h"
#endif

using namespace vrt;
using namespace vrt::VRTMath;

#if __INTEL_COMPILER
  // Some of the Intel C++ documentation says the function is "_rotl" while
  // other places say "__rotl" (extra underscore). So far, this is the only
  // version I've seen work.
# define rotateLeft(val,shift) _rotl(val,shift)
#else
/** Rotate left by the specified number of bits. */
inline unsigned int rotateLeft (unsigned int val, int shift) {
  return (val << shift) | (val >> (32 - shift));
}
#endif

static const int8_t UTF8_1BYTE_M = (int8_t)0x80; // Mask value for UTF8_1BYTE_0 check
static const int8_t UTF8_1BYTE_0 = (int8_t)0x00; // 0xxxxxxx
static const int8_t UTF8_2BYTE_M = (int8_t)0xE0; // Mask value for UTF8_2BYTE_0 check
static const int8_t UTF8_2BYTE_0 = (int8_t)0xC0; // 110xxxxx
static const int8_t UTF8_2BYTE_1 = (int8_t)0x80; // 10xxxxxx
static const int8_t UTF8_3BYTE_M = (int8_t)0xF0; // Mask value for UTF8_3BYTE_0 check
static const int8_t UTF8_3BYTE_0 = (int8_t)0xE0; // 1110xxxx
static const int8_t UTF8_3BYTE_1 = (int8_t)0x80; // 10xxxxxx
static const int8_t UTF8_3BYTE_2 = (int8_t)0x80; // 10xxxxxx
static const int8_t UTF8_4BYTE_M = (int8_t)0xF8; // Mask value for UTF8_4BYTE_0 check
static const int8_t UTF8_4BYTE_0 = (int8_t)0xF0; // 11110xxx
static const int8_t UTF8_4BYTE_1 = (int8_t)0x80; // 10xxxxxx
static const int8_t UTF8_4BYTE_2 = (int8_t)0x80; // 10xxxxxx
static const int8_t UTF8_4BYTE_3 = (int8_t)0x80; // 10xxxxxx

static bool    staticInitDone = false;
static int32_t FLOAT16_BASE_TABLE[512];
static int32_t FLOAT16_SHIFT_TABLE[512];
static int8_t  FLOAT16_OVER_TABLE[512];
static int32_t FLOAT16_MAN_TABLE[2048];
static int32_t FLOAT16_EXP_TABLE[64];
static int16_t FLOAT16_OFF_TABLE[64];

/** Only used in staticInit(). */
static int32_t convertmantissa (int32_t i){
  int32_t m = i<<13;            // Zero pad mantissa bits
  int32_t e = 0;                // Zero exponent
  while ((m&0x00800000) == 0) { // While not normalized
    e = e - 0x00800000;         // Decrement exponent (1<<23)
    m = m <<1;                  // Shift mantissa
  }
  m = m & ~0x00800000;          // Clear leading 1 bit
  e = e + 0x38800000;           // Adjust bias ((127-14)<<23)
  return m | e;                 // Return combined number
}

__attribute__((noinline))
__intelattr__((noinline))
static bool staticInit () {
  if (!staticInitDone) {
    // The algorithm used here is based on the code provided in [2], with
    // modifications to address the problem in the algorithm provided in [2]
    // where some NaN values mapped to Inf.

    // ==== Initialize the tables used by toHalf(..) ===========================
    //  Unlike [2] we include the FLOAT16_OVER_TABLE stage. This makes sure that
    //  any float NaN will become a half NaN. It does this by taking the *all*
    //  of the mantissa bits in the float and overlapping them within the
    //  mantissa of the half. If the result is a mantissa of zero the value is
    //  Inf, if it is non-zero it is an NaN, though no effort is made to preserve
    //  "signaling" vs "non-signaling" NaN since such is not applicable to half
    //  anyhow (and fairly useless within a float).
    for (int32_t i = 0; i < 256; i++) {
      int32_t e  = i - 127;
      int32_t ii = i | 0x100;
      if (e < -24){ // Very small numbers map to zero
        FLOAT16_BASE_TABLE[i]   = 0x0000;
        FLOAT16_BASE_TABLE[ii]  = 0x8000;
        FLOAT16_SHIFT_TABLE[i]  = 24; // clear mantissa bits
        FLOAT16_SHIFT_TABLE[ii] = 24; // clear mantissa bits
        FLOAT16_OVER_TABLE[i]   = 0x00; // clear overlap bits
        FLOAT16_OVER_TABLE[ii]  = 0x00; // clear overlap bits
      }
      else if (e < -14) { // Small numbers map to denorms
        FLOAT16_BASE_TABLE[i]   = (0x0400>>(-e-14));
        FLOAT16_BASE_TABLE[ii]  = (0x0400>>(-e-14)) | 0x8000;
        FLOAT16_SHIFT_TABLE[i]  = -e-1;
        FLOAT16_SHIFT_TABLE[ii] = -e-1;
        FLOAT16_OVER_TABLE[i]   = 0x00; // clear overlap bits
        FLOAT16_OVER_TABLE[ii]  = 0x00; // clear overlap bits
      }
      else if (e <= 15) { // Normal numbers just lose precision
        FLOAT16_BASE_TABLE[i]   = ((e+15)<<10);
        FLOAT16_BASE_TABLE[ii]  = ((e+15)<<10) | 0x8000;
        FLOAT16_SHIFT_TABLE[i]  = 13;
        FLOAT16_SHIFT_TABLE[ii] = 13;
        FLOAT16_OVER_TABLE[i]   = 0x00; // clear overlap bits
        FLOAT16_OVER_TABLE[ii]  = 0x00; // clear overlap bits
      }
      else if (e < 128) { // Large numbers map to Infinity
        FLOAT16_BASE_TABLE[i]   = 0x7C00;
        FLOAT16_BASE_TABLE[ii]  = 0xFC00;
        FLOAT16_SHIFT_TABLE[i]  = 24;
        FLOAT16_SHIFT_TABLE[ii] = 24;
        FLOAT16_OVER_TABLE[i]   = 0x00; // clear overlap bits
        FLOAT16_OVER_TABLE[ii]  = 0x00; // clear overlap bits
      }
      else { // Infinity and NaN's stay Infinity and NaN's
        FLOAT16_BASE_TABLE[i]   = 0x7C00;
        FLOAT16_BASE_TABLE[ii]  = 0xFC00;
        FLOAT16_SHIFT_TABLE[i]  = 13;
        FLOAT16_SHIFT_TABLE[ii] = 13;
        FLOAT16_OVER_TABLE[i]   = 0x7F; // overlap mantissa bits
        FLOAT16_OVER_TABLE[ii]  = 0x7F; // overlap mantissa bits
      }
    }

    // ==== Initialize the tables used by fromHalf(..) =========================
    FLOAT16_MAN_TABLE[0] = 0;
    for (int32_t i = 1; i <= 1023; i++) {
      FLOAT16_MAN_TABLE[i] = convertmantissa(i);
    }
    for (int32_t i = 1024; i <= 2047; i++) {
      FLOAT16_MAN_TABLE[i] = 0x38000000 + ((i-1024)<<13);
    }

    FLOAT16_EXP_TABLE[0] = 0;
    for (int32_t i = 1; i <= 30; i++) {
      FLOAT16_EXP_TABLE[i] = i << 23;
    }
    FLOAT16_EXP_TABLE[31]= 0x47800000;
    FLOAT16_EXP_TABLE[32]= 0x80000000;
    for (int32_t i = 33; i <= 62; i++) {
      FLOAT16_EXP_TABLE[i] = 0x80000000 + ((i-32)<<23);
    }
    FLOAT16_EXP_TABLE[63]= 0xC7800000;

    FLOAT16_OFF_TABLE[0] = 0;
    for (int32_t i = 1; i <= 31; i++) {
      FLOAT16_OFF_TABLE[i] = 1024;
    }
    FLOAT16_OFF_TABLE[32] = 0;
    for (int32_t i = 33; i <= 63; i++) {
      FLOAT16_OFF_TABLE[i] = 1024;
    }

    staticInitDone = true;
  }
  return staticInitDone;
}

int16_t VRTMath::_toHalfInternal (float val) {
  if (!staticInitDone) staticInit();
  // The algorithm used here is based on the code provided in [2], with
  // modifications to address the problem in the algorithm provided in [2]
  // where some NaN values mapped to Inf.
  int32_t floatBits = safecastT1ToT2<float,int32_t>(val);
  int32_t index     = (floatBits>>23) & 0x1FF;
  int32_t halfBits  = FLOAT16_BASE_TABLE[index]
                    | ((floatBits&0x007FFFFF) >> FLOAT16_SHIFT_TABLE[index])
                    | (((floatBits >> 6) | floatBits) & FLOAT16_OVER_TABLE[index]);
  return (int16_t)halfBits;
}

float VRTMath::_fromHalfInternal (int16_t bits) {
  if (!staticInitDone) staticInit();
  // The algorithm used here is based on the code provided in [2], with
  // modifications to address the problem in the algorithm provided in [2]
  // where some NaN values mapped to Inf.
  int32_t halfBits  = ((int32_t)bits) & 0xFFFF;
  int32_t index     = halfBits >> 10;
  int32_t floatBits = FLOAT16_MAN_TABLE[FLOAT16_OFF_TABLE[index]+(halfBits&0x3FF)]
                    + FLOAT16_EXP_TABLE[index];
  return safecastT1ToT2<int32_t,float>(floatBits);
}

/** <b>Internal Use Only:</b> Converts to VRTFloat. */
static int64_t _toVRTFloat64 (bool sign, int32_t eSize, int32_t dSize, double val) {
  int64_t maxExp    = (__INT64_C(1) << eSize) - 1;            // Max output exponent value
  int32_t mSize     = dSize - eSize;                          // Output mantissa size
  int64_t bits      = doubleToRawLongBits(val);               // Input IEEE-754 bits
  int64_t expo      = ((bits >> 52) & 0x7FF) - 1023;          // Input exponent
  int64_t mant      = (bits & __INT64_C(0x000FFFFFFFFFFFFF)); // Input mantissa
  int64_t exponent;                                           // Output exponent
  int64_t mantissa;                                           // Output mantissa

  if (expo != 0) {
    // If not denormal, add back the top bit
    mant = mant | __INT64_C(0x0010000000000000);
  }

  mantissa = (sign)? (mant >> (54 - mSize)) // 54 not 53 so top bit is effectively "sign bit"
                   : (mant >> (53 - mSize));
  exponent = maxExp + expo + __INT64_C(1);

  if (exponent < 0) {
    // Output is (effectively) a denormal
    mantissa = mantissa >> -exponent;
    exponent = 0;
  }

  if (val < 0) {
    mantissa = -mantissa;
  }

  return (((mantissa << eSize) | exponent) & ~(__INT64_C(-1) << dSize));
}

/** <b>Internal Use Only:</b> Converts from VRTFloat. */
static double _fromVRTFloat64 (bool sign, int32_t eSize, int32_t dSize, int64_t bits) {
  int32_t maxExp = (0x1 << eSize) - 1;     // = all exp bits set
  int32_t exp    = ((int32_t)bits) & maxExp;
  int32_t mSize  = dSize - eSize;

  if (sign) {
    // Previously we used 'pow(2, mSize + maxExp - exp - 1)' for the divisor,
    // but since we are using powers of 2 below 2^64 we can do bit-shifts in
    // place of the cal to pow(2,N) if we break them up.
    int64_t man = (bits << (64 - dSize)) >> (64 - mSize); // sign extend and shift
    double  a   = (double)(__UINT64_C(0x01) << (mSize  - 1  )); // -1 for effective "sign bit"
    double  b   = (double)(__UINT64_C(0x01) << (maxExp - exp));
    // No special case of maxExp=63 and exp=0 in C++ since using UINT64_C
    return man / a / b;
  }
  else {
    int64_t man = (bits >> eSize) & ((__INT64_C(0x1) << mSize) - 1); // mask off upper bits
    double  a   = (double)(__UINT64_C(0x01) << (mSize       ));
    double  b   = (double)(__UINT64_C(0x01) << (maxExp - exp));
    // No special case of maxExp=63 and exp=0 in C++ since using UINT64_C
    return man / a / b;
  }
}

int64_t VRTMath::toVRTFloat (DataItemFormat form, int32_t dSize, double val) {
  bool    sign   = DataItemFormat_isSigned(form);        // Is output signed?
  int32_t eSize  = DataItemFormat_getExponentBits(form); // Number of output exponent bits (1 to 6)

  if (eSize < 1) {
    throw VRTException("Expected VRT floating-point format");
  }
  if (((dSize-eSize) < 1) || (dSize > 64)) {
    // Use snprintf(..) for formatting to be compatible with JNI usage
    char msg[128];
    snprintf(msg, 128, "Expected VRT data size in range of [%d,64] but given %d", (eSize+1), dSize);
    throw VRTException(msg);
  }
  return toVRTFloat64(sign, eSize, dSize, val);
}

int32_t VRTMath::toVRTFloat32 (bool sign, int32_t eSize, int32_t dSize, double val) {
  // Need to disable Intel compiler warning 1572 (floating-point equality and
  // inequality comparisons are unreliable) since it will otherwise flag the
  // following check even though it is a correct checks for 0.0.
  _Intel_Pragma("warning push")
  _Intel_Pragma("warning disable 1572")
  if (val == 0.0) return 0; // <-- Fast handling for common case
  _Intel_Pragma("warning pop")

  if (val >= +1.0) {
    if (sign) return ~(-1 << (dSize-1));
    // special case for dSize=32 since (-1<<32) gives 0 *or* -1
    return (dSize == 32)? -1 : ~(-1 << dSize);
  }
  if (!sign && (val < 0.0)) return 0;
  if (val <= -1.0) return (0x1 << (dSize-1)) | ((1 << eSize) - 1);
  if (isNull(val)) return 0; // <-- checks for NaN

  return (int32_t)_toVRTFloat64(sign, eSize, dSize, val);
}

int64_t VRTMath::toVRTFloat64 (bool sign, int32_t eSize, int32_t dSize, double val) {
  // Need to disable Intel compiler warning 1572 (floating-point equality and
  // inequality comparisons are unreliable) since it will otherwise flag the
  // following check even though it is a correct checks for 0.0.
  _Intel_Pragma("warning push")
  _Intel_Pragma("warning disable 1572")
  if (val == 0.0) return __INT64_C(0); // <-- Fast handling for common case
  _Intel_Pragma("warning pop")

  if (val >= +1.0) return (sign)? ~(__INT64_C(-1) << (dSize-1)) : ~(__INT64_C(-1) << dSize);
  if (!sign && (val < 0.0)) return __INT64_C(0);
  if (val <= -1.0) return (__INT64_C(0x1) << (dSize-1)) | ((__INT64_C(1) << eSize) - 1);
  if (isNull(val)) return __INT64_C(0); // <-- checks for NaN

  return _toVRTFloat64(sign, eSize, dSize, val);
}

double VRTMath::fromVRTFloat (DataItemFormat form, int32_t dSize, int64_t bits) {
  bool    sign   = DataItemFormat_isSigned(form);
  int32_t eSize  = DataItemFormat_getExponentBits(form);
  if (eSize < 1) {
    throw VRTException("Expected VRT floating-point format");
  }
  if (((dSize-eSize) < 1) || (dSize > 64)) {
    // Use snprintf(..) for formatting to be compatible with JNI usage
    char msg[128];
    snprintf(msg, 128, "Expected VRT data size in range of [%d,64] but given %d", (eSize+1), dSize);
    throw VRTException(msg);
  }
  return fromVRTFloat64(sign, eSize, dSize, bits);
}

double VRTMath::fromVRTFloat32 (bool sign, int32_t eSize, int32_t dSize, int32_t bits) {
  if (bits == 0) return 0.0; // <-- Fast handling for common case

  return _fromVRTFloat64(sign, eSize, dSize, bits);
}

double VRTMath::fromVRTFloat64 (bool sign, int32_t eSize, int32_t dSize, int64_t bits) {
  if (bits == __INT64_C(0)) return 0.0; // <-- Fast handling for common case

  return _fromVRTFloat64(sign, eSize, dSize, bits);
}

int32_t VRTMath::unpackBits32 (const void *ptr, int32_t bitOffset, int32_t bitCount) {
  const char *buf = (const char*)ptr;

  int32_t mask = (bitCount == 32)? -1 : ((0x1 << bitCount) - 1);
  int32_t off  = bitOffset >> 3;
  int32_t skip = bitOffset & 0x7;
  int32_t bits = 0;   // Bit accumulator

  if ((bitCount < 8) && ((skip + bitCount) <= 8)) {
    // Special case since we need to consider the "skip bits" at the top of
    // the byte *AND* any "extra bits" at the bottom of the byte.
    return (buf[off] >> (8 - bitCount - skip)) & mask;
  }

  bitCount += skip; // read in "skip bits" as we will discard them at the end

  _Intel_Pragma("loop_count min=0, max=4")
  while (bitCount >= 8) {
    bits = (bits << 8) | (0xFF & buf[off++]);
    bitCount -= 8;
  }

  if (bitCount > 0) {
    bits = (bits << bitCount) | ((0xFF & buf[off]) >> (8-bitCount));
  }

  return bits & mask;
}

int64_t VRTMath::unpackBits64 (const void *ptr, int32_t bitOffset, int32_t bitCount) {
  const char *buf = (const char*)ptr;
  // If < 32 bits, use the 32-bit version which can utilize the 32-bit registers
  // on the system. However, unlike packBits64(..), simply calling unpackBits32(..)
  // twice and combining the results is slower than this version since that
  // form requires converting values from 32-bits to 64-bits twice and two
  // logical ANDs to prevent accidental sign-extension. Note that this is NOT
  // used for exactly 32-bit values since it can cause values to be sign-extended
  // wrong.
  if (bitCount < 32) return unpackBits32(buf, bitOffset, bitCount);

  int64_t mask = (bitCount == 64)? -1L : ((__INT64_C(0x1) << bitCount) - __INT64_C(1));
  int32_t off  = bitOffset >> 3;
  int32_t skip = bitOffset & 0x7;
  int64_t bits = 0;   // Bit accumulator

  bitCount += skip; // read in "skip bits" as we will discard them at the end

  _Intel_Pragma("loop_count min=0, max=8")
  while (bitCount >= 8) {
    bits = (bits << 8) | (0xFF & buf[off++]);
    bitCount -= 8;
  }

  if (bitCount > 0) {
    bits = (bits << bitCount) | ((0xFF & buf[off]) >> (8-bitCount));
  }

  return bits & mask;
}

void VRTMath::packBits32 (void *ptr, int32_t bitOffset, int32_t bitCount, int32_t bits) {
  char *buf = (char*)ptr;
  // This is a bit messier than the unpackBits32(..) function since we need to
  // save any existing bits and we are "working backwards" (i.e. pulling bits
  // off the top rather than the bottom). The rotateLeft(..) function used here
  // should correspond to an intrinsic function in the Java VM (Java 6+) or
  // the Intel C++ compiler (GCC 4.8+ should have it as an intrinsic too, once
  // it is available).
  int32_t off     = bitOffset >> 3;                  // 8-bit offset into buffer
  int32_t skipTop = bitOffset & 0x7;                 // Bits to skip at top of first octet
  int32_t width   = (skipTop + bitCount + 7) / 8;    // Number of octets to modify (+7 to "round up")
  int32_t skipBtm = (width*8) - skipTop - bitCount;  // Bits to skip at bottom of last octet
  int32_t maskTop = 0xFF00 >> skipTop;               // Mask for top bits to save (extra bits ignored)
  int32_t maskBtm = ~(-1 << skipBtm);                // Mask for bottom bits to save

  if (width == 1) {
    int32_t maskKeep = maskTop | maskBtm;
    buf[off] = (char)((buf[off] & maskKeep) | ((bits << skipBtm) & ~maskKeep));
  }
  else {
    // The next line is effectively 2 separate shifts:
    //   bitsToSet = bits << (32 - bitCount);       // shift bits to left of register
    //   bitsToSet = rotateLeft(bits, 8 - skipTop); // rotate first 8 bits to set
    int bitsToSet = rotateLeft(bits, 40 - bitCount - skipTop);
    buf[off] = (char)((buf[off] & maskTop) | (bitsToSet & ~maskTop));
    off++;

    _Intel_Pragma("loop_count min=1, max=3")
    for (int32_t i = 2; i < width; i++) { // i=2 since first and last are special
      buf[off++] = (char)(bitsToSet = rotateLeft(bitsToSet, 8));
    }

    bitsToSet = rotateLeft(bitsToSet, 8);
    buf[off] = (char)((buf[off] & maskBtm) | (bitsToSet & ~maskBtm));
  }
}

void VRTMath::packBits64 (void *ptr, int32_t bitOffset, int32_t bitCount, int64_t bits) {
  // If <=32 bits, use the 32-bit version which can utilize the 32-bit registers
  // on the system. Since we do this, we don't need to worry about the special
  // width=1 case here.
  if (bitCount <= 32) {
    packBits32(ptr, bitOffset, bitCount, (int32_t)bits);
  }
  else {
    // Even though this ends up setting one octet twice (top bits then lower
    // bits of middle octet), testing revealed no significant performance hit
    // for doing this, presumably due to the faster math possible when using
    // 32-bit registers (even on a 64-bit system).
    int32_t n = bitCount - 32;
    packBits32(ptr, bitOffset,   n,  (int32_t)(bits >> 32));
    packBits32(ptr, bitOffset+n, 32, (int32_t)(bits      ));
  }
}

////////////////////////////////////////////////////////////////////////////////
#if NOT_USING_JNI
////////////////////////////////////////////////////////////////////////////////

int32_t VRTMath::packAscii (void *ptr, int32_t off, const string &val, int32_t length) {
  char    *buf = (char*)ptr;
  int32_t  len = 0;
  int32_t  end = off+length;

  for (size_t i = 0; i < val.length(); i++,off++) {
    char c = val[i];
    if ((c < 0x20) || (c > 0x7E)) c = '?';
    buf[off] = (char)c;
    len++;
  }
  if (off < end) {
    memset(&buf[off], 0, length-len);
  }
  return len;
}

int32_t VRTMath::packAscii (void *ptr, int32_t off, const wstring &val, int32_t length) {
  char    *buf = (char*)ptr;
  int32_t  len = 0;
  int32_t  end = off+length;

  for (size_t i = 0; i < val.length(); i++,off++) {
    int32_t c = val[i];
    if ((c < 0x20) || (c > 0x7E)) c = '?';
    buf[off] = (char)c;
    len++;
  }
  if (off < end) {
    memset(&buf[off], 0, length-len);
  }
  return len;
}

int32_t VRTMath::packUTF8 (void *ptr, int32_t off, const string &val, int32_t length) {
  char    *buf = (char*)ptr;
  int32_t  len = 0;
  int32_t  end = (length < 0)? 0x7FFFFFFF : off+length;

  for (size_t i = 0; i < val.length(); i++) {
    int32_t c = val[i];

    if (c >= 0x01) {                                     // ASCII (except embedded NUL)
      if (off < end) {
        buf[off++] = (int8_t)c;
      }
      len++;
    }
    else {                                               // 0x0080 to 0x07FF range (or embedded NUL)
      if (off+1 < end) {
        buf[off++] = (int8_t)(UTF8_2BYTE_0 | ((c >> 6) & 0x1F));
        buf[off++] = (int8_t)(UTF8_2BYTE_1 | (   c     & 0x3F));
      }
      else {
        if (off < end) buf[off++] = 0;
      }
      len+=2;
    }
  }
  if (off < end) {
    memset(&buf[off], 0, end-off);
  }
  return len;
}

int32_t VRTMath::packUTF8 (void *ptr, int32_t off, const wstring &val, int32_t length) {
  char    *buf = (char*)ptr;
  int32_t  len = 0;
  int32_t  end = (length < 0)? 0x7FFFFFFF : off+length;

  for (size_t i = 0; i < val.length(); i++) {
#if (WCHAR_MAX == 0xFFFF)
    // Found a rare case where wchar_t is defined in such a way that it can not hold all of the
    // UTF-8 values, likely an issue with it only supporting the older "basic multilingual plane"
    // (0x0000 to 0xFFFF).
    int32_t c = (unsigned int)val[i];
#else
    int32_t c = val[i];
#endif

    if (c < 0x0000) {
      // On many platforms wchar_t is typedef'ed the same as int32_t. Since negative values are
      // invalid in Unicode, just error out if a negative value is seen.
      throw VRTException("Invalid character 0x%x found in '%ls'", c, val.c_str());
    }

    if ((c >= 0x0001) && (c <= 0x007F)) {                // ASCII (except embedded NUL)
      if (off < end) {
        buf[off++] = (int8_t)c;
      }
      len++;
    }
    else if (c <= 0x07FF) {                              // 0x0080 to 0x07FF range (or embedded NUL)
      if (off+1 < end) {
        buf[off++] = (int8_t)(UTF8_2BYTE_0 | ((c >> 6) & 0x1F));
        buf[off++] = (int8_t)(UTF8_2BYTE_1 | (   c     & 0x3F));
      }
      else {
        if (off < end) buf[off++] = 0;
      }
      len+=2;
    }
    else if (c < 0xFFFF) {                               // 0x0800 to 0xFFFF range
      if (off+2 < end) {
        buf[off++] = (int8_t)(UTF8_3BYTE_0 | ((c >> 12) & 0x0F));
        buf[off++] = (int8_t)(UTF8_3BYTE_1 | ((c >>  6) & 0x3F));
        buf[off++] = (int8_t)(UTF8_3BYTE_2 | (    c     & 0x3F));
      }
      else {
        _Intel_Pragma("loop_count min=0, max=2")
        while (off < end) buf[off++] = 0;
      }
      len+=3;
    }
    else if (c < 0x10FFFF) {                             // 0x010000 to 0x10FFFF range
      if (off+3 < end) {
        buf[off++] = (int8_t)(UTF8_4BYTE_0 | ((c >> 18) & 0x07));
        buf[off++] = (int8_t)(UTF8_4BYTE_1 | ((c >> 12) & 0x3F));
        buf[off++] = (int8_t)(UTF8_4BYTE_2 | ((c >>  6) & 0x3F));
        buf[off++] = (int8_t)(UTF8_4BYTE_3 | (    c     & 0x3F));
      }
      else {
        _Intel_Pragma("loop_count min=0, max=3")
        while (off < end) buf[off++] = 0;
      }
      len+=4;
    }
    else {
      throw VRTException("Invalid character 0x%x found in '%ls'", c, val.c_str());
    }
  }
  memset(&buf[off], 0, end-off);
  return len;
}

int32_t VRTMath::packMetadata (vector<char> &buf, int32_t off, const MetadataBlock &val, int32_t len) {
  int32_t length = val.writeBytes(buf, off, len);
  if (length > len) {
    throw VRTException("Incompatible length (%d) expected %d", length, len);
  }
  return length;
}

void VRTMath::packRecord (vector<char> &buf, int32_t off, const Record &val) {
  val.readBytes(&buf[off]);
}

boolNull VRTMath::unpackBoolNull (const void *ptr, int32_t off) {
  const char *buf = (const char*)ptr;
  char b = buf[off];
  if (VRTConfig::getStrict()) {
    switch (b) {
      case -1: return _FALSE;
      case  0: return _NULL;
      case +1: return _TRUE;
      default: throw VRTException("Illegal value (%d) when expecting boolean (0=null, -1=false, +1=true)", b);
    }
  }
  else {
    if (b == 0) return _NULL;
    if (b <  0) return _FALSE;
                return _TRUE;
  }
}

bool VRTMath::unpackBoolean (const void *ptr, int32_t off) {
  const char *buf = (const char*)ptr;
  char b = buf[off];
  if (VRTConfig::getStrict()) {
    switch (b) {
      case -1: return false;
      case  0: return false;
      case +1: return true;
      default: throw VRTException("Illegal value (%d) when expecting boolean (0=false, -1=false, +1=true)", b);
    }
  }
  else {
    return (b > 0);
  }
}

string VRTMath::unpackAscii (const void *ptr, int32_t off, int32_t len) {
  const char *buf = (const char*)ptr;
  string str(&buf[off], len);

  int32_t size = 0;
  for (int32_t i = 0; i < len; i++) {
    char c = str[i];
    if (c == '\0') break;
    if ((c < 0x20) || (c > 0x7E)) str[i] = '?';
    size++;
  }
  str.resize(size);

  return str;
}

wstring VRTMath::unpackUTF8 (const void *ptr, int32_t off, int32_t len, wchar_t replacement) {
  const char *buf = (const char*)ptr;
  if ((WCHAR_MIN > 0) || (WCHAR_MAX < 0x10FFFF)) {
    // Found a rare case where wchar_t is defined in such a way that it can not hold all of the
    // UTF-8 values, likely an issue with it only supporting the older "basic multilingual plane"
    // (0x0000 to 0xFFFF). Just error out rather than guess at what it does and doesn't support.
    throw VRTException("This build does not support UTF-8 strings (found WCHAR_MIN=%d and WCHAR_MAX=%d)", WCHAR_MIN, WCHAR_MAX);
  }
  wstring str;
  wchar_t chrs[] = { 0, 0 };

  int32_t end = off + len;
  while (off < end) {
    int8_t  b = buf[off++];
    int32_t c;

    if (b == 0) {                                                      // End-of-string NUL
      return str;
    }
    else if ((b & UTF8_1BYTE_M) == UTF8_1BYTE_0) {                     // ASCII (except embedded NUL)
      c = (b & 0x7F);
    }
    else if (((b & UTF8_2BYTE_M) == UTF8_2BYTE_0) && (off < len)) {    // 0x0080 to 0x07FF range (or embedded NUL)
      c  = (b          & 0x1F) << 6;
      c |= (buf[off++] & 0x3F);
    }
    else if (((b & UTF8_3BYTE_M) == UTF8_3BYTE_0) && (off+1 < len)) {  // 0x0800 to 0xFFFF range
      c  = (b          & 0x0F) << 12;
      c |= (buf[off++] & 0x3F) << 6;
      c |= (buf[off++] & 0x3F);
    }
    else if (((b & UTF8_4BYTE_M) == UTF8_4BYTE_0) && (off+2 < len)) {  // 0x010000 to 0x10FFFF range
      c  = (b          & 0x07) << 18;
      c |= (buf[off++] & 0x3F) << 12;
      c |= (buf[off++] & 0x3F) <<  6;
      c |= (buf[off++] & 0x3F);
    }
    else if (replacement != 0) {
      c = replacement;
    }
    else {
      throw VRTException("Invalid UTF-8 character found after '%ls'.", str.c_str());
    }
    chrs[0] = c;
    str.append(chrs);
  }
  return str;
}

MetadataBlock VRTMath::unpackMetadata (const vector<char> &buf, int32_t off, int32_t len) {
  MetadataBlock meta;
  meta.readBytes(buf, off, len);
  return meta;
}

void VRTMath::unpackRecord (const vector<char> &buf, int32_t off, Record &val) {
  val.writeBytes(&buf[off]);
}

////////////////////////////////////////////////////////////////////////////////
#endif /* NOT_USING_JNI */
////////////////////////////////////////////////////////////////////////////////

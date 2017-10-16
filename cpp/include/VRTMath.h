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

#ifndef VRTMath_h
#define VRTMath_h

#include "VRTObject.h"
#include <cmath>
#include <cctype>
#include <string>
#include <vector>
#include <string.h>    // required for memcpy(..) on GCC4.4/libc6 2.11.1

/** The INTERNAL_FLOAT16_LIB setting can be used to override use of compiler-specific
 *  conversion for IEEE 754-2008 half-precision floating-point routines. Options:
 *  <pre>
 *    INTERNAL_FLOAT16_LIB=0   Use compiler-specific libraries (error out
 *                             if not found)
 *    INTERNAL_FLOAT16_LIB=1   Use internal library calls regardless of the
 *                             availability of compiler-specific libraries.
 *    INTERNAL_FLOAT16_LIB=-1  Automatically select best option based on
 *                             availability.
 *  </pre>
 */
#ifndef INTERNAL_FLOAT16_LIB
# define INTERNAL_FLOAT16_LIB 1
#endif
#if (INTERNAL_FLOAT16_LIB == 0)
# if defined(__INTEL_COMPILER)
#  include <emmintrin.h>
# elif defined(__GNU_COMPILER)
#  if (__GCC_VERSION >= 40500)
#   warning "Use of the GCC 4.5+ libraries for half-precision floats is untested, \
             recommend building with INTERNAL_FLOAT16_LIB=0 for now."
     // Note that GCC must include the "-mfp16-format=ieee" option to select the
     // IEEE 754-2008 version of half-precision (rather than an out-dated ARM
     // format that is similar but omits NaN and +/-Inf). When this option is
     // set the "__fp16" data type becomes available for storage and conversion
     // to/from "float".
#  else
#   error "GNU C++ does not provide compiler-specific FLOAT16 libraries prior to \
           version 4.5, try re-building with INTERNAL_FLOAT16_LIB=0"
#  endif
# else
#  error "Could not find compiler-specific FLOAT16 libraries, try re-building \
          with INTERNAL_FLOAT16_LIB=0"
# endif
#endif


#if defined(__GNUC__) && !(defined(__APPLE__) && defined(__MACH__))
// GCC 4.1.2 provides low-level implementations of __bswap_16/__bswap_32/__bswap_64
// via byteswap.h, however these are not present under OS X (__APPLE__ & __MACH__)
// so we omit the include file there and just use the definitions appearing below.
// Note that it is possible these are missing under BSD too, but this is untested.
# include <byteswap.h>
#else

#ifndef __bswap_16
inline uint16_t __bswap_16 (uint16_t val) {
  return ((val & 0xff00) >> 8)
       | ((val & 0x00ff) << 8);
}
inline int16_t __bswap_16 (int16_t val) {
  uint16_t v = __bswap_16(*((uint16_t*)&val));
  return *((int16_t*)&v);
}
#endif

#ifndef __bswap_32
# if defined(__INTEL_COMPILER)
#  define __bswap_32(val) bswap(val)
# else
inline uint32_t __bswap_32 (uint32_t val) {
  return ((val & 0xff000000) >> 24)
       | ((val & 0x00ff0000) >>  8)
       | ((val & 0x0000ff00) <<  8)
       | ((val & 0x000000ff) << 24);
}
inline int32_t __bswap_32 (int32_t val) {
  uint32_t v = __bswap_32(*((uint32_t*)&val));
  return *((int32_t*)&v);
}
inline float __bswap_32 (float val) {
  uint32_t v = __bswap_32(*((uint32_t*)&val));
  return *((float*)&v);
}
# endif
#endif

#ifndef __bswap_64
# if defined(__INTEL_COMPILER)
#  define __bswap_64(val) bswap64(val)
# else
inline uint64_t __bswap_64 (uint64_t val) {
  return ((val & __UINT64_C(0xff00000000000000)) >> 56)
       | ((val & __UINT64_C(0x00ff000000000000)) >> 40)
       | ((val & __UINT64_C(0x0000ff0000000000)) >> 24)
       | ((val & __UINT64_C(0x000000ff00000000)) >>  8)
       | ((val & __UINT64_C(0x00000000ff000000)) <<  8)
       | ((val & __UINT64_C(0x0000000000ff0000)) << 24)
       | ((val & __UINT64_C(0x000000000000ff00)) << 40)
       | ((val & __UINT64_C(0x00000000000000ff)) << 56);
}
inline int64_t __bswap_64 (int64_t val) {
  uint64_t v = __bswap_64(*((uint64_t*)&val));
  return *((int64_t*)&v);
}
inline double __bswap_64 (double val) {
  uint64_t v = __bswap_64(*((uint64_t*)&val));
  return *((double*)&v);
}
# endif
#endif
#endif

// Avoid warnings caused by the __bswap_XX(..) functions being unsigned-only on some platforms
_Clang_Pragma("clang diagnostic push")
_Clang_Pragma("clang diagnostic ignored \"-Wsign-compare\"")

namespace vrt {
  // Note: Due to include file dependencies the following functions are defined
  // in another include file in C++:
  //    packTimeStamp(..)   - TimeStamp.h
  //    unpackTimeStamp(..) - TimeStamp.h
  //    packRecord(..)      - Record.h
  //    unpackRecord(..)    - Record.h
  //    toVRTFloat(..)      - BasicVRTPacket.h
  //    fromVRTFloat(..)    - BasicVRTPacket.h

  /** Type definition indicating that the input must be <tt>BIG_ENDIAN</tt> or <tt>LITTLE_ENDIAN</tt>.
   *  The values of <tt>BIG_ENDIAN</tt> and <tt>LITTLE_ENDIAN</tt> are defined in <tt>/usr/include/endian.h</tt>.
   */
  typedef int32_t ByteOrder;

  /** Supports conversions to/from the various numeric representations used within
   *  a VITA 49 packet. The name <tt>VRTMath</tt> is a bit of an anachronism in
   *  that the only "math" done on the special data types used in a VITA 49 (VRT)
   *  packet is in the functions that convert between these special types and
   *  standard Java/C++ data types. <br>
   *  <br>
   *  <h2>IEEE 754-2008 16-bit Floating-Point Numbers</h2>
   *  <b><i>The use of 16-bit IEEE 754-2008 floating-point numbers is not currently
   *  part of the VITA 49 standard, but is under consideration for inclusion in a
   *  future revision. Their inclusion here is to support the necessary analysis
   *  work that would precede any such inclusion.</i></b><br>
   *  <br>
   *  Since IEEE 754-2008 16-bit floating-point numbers are a storage-only format
   *  and are not supported by Java or C++ <i>(though some C++ compilers have
   *  non-standard extensions for them)</i> the routines necessary for converting
   *  between 16-bit floating-point numbers (called "half-precision") and
   *  single-precision numbers are included here. <br>
   *  <br>
   *  <h2>VRT Floating-Point Numbers</h2>
   *  Supports conversion between an IEEE-754 floating-point value and the packed
   *  packed bits for a VRT floating-point. <br>
   *  <br>
   *  Although VRT floating-point values can express many values in multiple
   *  ways within the binary form (e.g. 1/4 or 2/8) this function outputs the
   *  values in a "normalized form" where the output value will have a 1 in
   *  the most-significant-(non-sign-)bit of the mantissa, excepting for small
   *  values close to zero where the exponent is zero. This mirrors the behavior
   *  of IEEE-754 floating-point values, excepting that the highest bit is
   *  always present (not just for "denormal" values). <br>
   *  <br>
   *  Since the range of VRT floating-point numbers is [0,1) (signed) or [-1,1)
   *  (unsigned) any values below/above that range will be converted to the
   *  smallest/largest value possible in that range. Any NaN values passed in
   *  will be converted to zero. <i>This is similar to the standard behavior for
   *  IEEE 754 numbers when converting them to an integer.</i> <br>
   *  <br>
   *  <h2>VRT Fixed-Point Numbers</h2>
   *  Supports fixed-point numbers compatible with VRT.
   *  <pre>
   *    Example:
   *
   *    31                                  0
   *    [00000000|00000000|0000oooo|oooooooo]
   *                           ^ radixPoint = 12
   *
   *    20-bit whole number (2's complement) + 12 bit binary fraction
   *
   *    Convert to/from double: toDouble32(20, bits)  -> val
   *                            fromDouble32(20, val) -> bits
   *  </pre>
   *  <b>Note that all of the VRT fixed-point conversions assume that a valid
   *  radix point has been given, and will have unspecified behavior if the
   *  radix point is invalid.</b> <br>
   *  <br>
   *  Any values given that are out-of-range with respect to the output will be
   *  automatically converted to the nearest in-range value, and any floating-point
   *  NaN values given will become zero. Where the fixed-point value has less
   *  precision than the input value, the input value will be "rounded" to the
   *  nearest fixed-point value.<br>
   *  <br>
   *  <i>Implementation Note: Prior versions of this library took a "garbage in
   *  garbage out" approach to out-of-range values in the interest of maximizing
   *  performance. Unfortunately the rounding involved in converting the numbers
   *  to fixed point created a situation where input values extremely close to the
   *  min/max limits could end up rounded to an out-of-range value and result in
   *  a bogus result. In the end the code to perform a conversion to the nearest
   *  in-range value (for all input values) is no more complex than the code
   *  required to support this special-case situation, hence the current approach.
   *  </i><br>
   *  <br>
   *  <h2>Summary</h2>
   *  <table border='1'>
   *    <tr>
   *      <th>&nbsp;</th>
   *      <th colspan='2'>Convert to/from <tt>long</tt></th>
   *      <th colspan='2'>Convert to/from <tt>int</tt></th>
   *      <th colspan='2'>Convert to/from <tt>double</tt></th>
   *      <th colspan='2'>Convert to/from <tt>float</tt></th>
   *    </tr>
   *    <tr>
   *      <th>IEEE 754 16-bit Floating-Point</th>
   *      <td>n/a</td>
   *      <td>n/a</td>
   *      <td>n/a</td>
   *      <td>n/a</td>
   *      <td>n/a</td>
   *      <td>n/a</td>
   *      <td>{@link #toHalf toHalf(..)}</td>
   *      <td>{@link #fromHalf fromHalf(..)}</td>
   *    </tr>
   *    <tr>
   *      <th>VRT Floating-Point</th>
   *      <td>n/a</td>
   *      <td>n/a</td>
   *      <td>n/a</td>
   *      <td>n/a</td>
   *      <td>{@link #toVRTFloat toVRTFloat(..)}</td>
   *      <td>{@link #fromVRTFloat fromVRTFloat(..)}</td>
   *      <td>TBD</td>
   *      <td>TBD</td>
   *    </tr>
   *    <tr>
   *      <th>VRT 64-bit Fixed Point</th>
   *      <td>{@link #toLong64 toLong64(..)}</td>
   *      <td>{@link #fromLong64 fromLong64(..)}</td>
   *      <td>{@link #toInt64 toInt64(..)}</td>
   *      <td>{@link #fromInt64 fromInt64(..)}</td>
   *      <td>{@link #toDouble64 toDouble64(..)}</td>
   *      <td>{@link #fromDouble64 fromDouble64(..)}</td>
   *      <td>{@link #toFloat64 toFloat64(..)}</td>
   *      <td>{@link #fromFloat64 fromFloat64(..)}</td>
   *    </tr>
   *    <tr>
   *      <th>VRT 32-bit Fixed Point</th>
   *      <td>{@link #toLong32 toLong32(..)}</td>
   *      <td>{@link #fromLong32 fromLong32(..)}</td>
   *      <td>{@link #toInt32 toInt32(..)}</td>
   *      <td>{@link #fromInt32 fromInt32(..)}</td>
   *      <td>{@link #toDouble32 toDouble32(..)}</td>
   *      <td>{@link #fromDouble32 fromDouble32(..)}</td>
   *      <td>{@link #toFloat32 toFloat32(..)}</td>
   *      <td>{@link #fromFloat32 fromFloat32(..)}</td>
   *    </tr>
   *    <tr>
   *      <th>VRT 16-bit Fixed Point</th>
   *      <td>{@link #toLong16 toLong16(..)}</td>
   *      <td>{@link #fromLong16 fromLong16(..)}</td>
   *      <td>{@link #toInt16 toInt16(..)}</td>
   *      <td>{@link #fromInt16 fromInt16(..)}</td>
   *      <td>{@link #toDouble16 toDouble16(..)}</td>
   *      <td>{@link #fromDouble16 fromDouble16(..)}</td>
   *      <td>{@link #toFloat16 toFloat16(..)}</td>
   *      <td>{@link #fromFloat16 fromFloat16(..)}</td>
   *    </tr>
   *  </table>
   */
  namespace VRTMath {

    /** Used to perform compile-time type check as follows:
     *    requirement<sizeof(int32_t)==sizeof(float), int32_t>::type ret;
     *  which passes, where as the following will fail at compile time:
     *    requirement<sizeof(int32_t)==sizeof(double), int32_t>::type ret;
     *  Use where the first argument is false results in a compilation error
     *  reporting invalid use of incomplete type.
     */
    template <bool, typename T>
    struct type_requirement;

    template <typename T>
    struct type_requirement<true, T>
    {
      typedef T type;
    };

    /** Converts an from one type to another type of same size using memcpy.
     *  templateparam T1 From-type
     *  templateparam T2 To-type
     *  @param val Value to convert from (should have type T1)
     *  @return Value as type T2
     */
    template <typename T1, typename T2>
    T2 safecastT1ToT2 (T1 val) {
      typename type_requirement<sizeof(T1)==sizeof(T2), T2>::type ret;
      memcpy(&ret, &val, sizeof(ret));
      return ret;
    };

    /** Converts an IEEE-764 double into the corresponding 64-bits contained
     *  in a 64-bit integer. <br>
     *  <br>
     *  The name of this function is patterned after the Java function (as this
     *  provides some consistency between the Java and C++ version), but the
     *  implementation is entirely different.
     */
    inline int64_t doubleToRawLongBits (double val) {
      return safecastT1ToT2<double,int64_t>(val);
    }

    /** Converts an IEEE-764 float into the corresponding 32-bits contained
     *  in a 32-bit integer. <br>
     *  <br>
     *  The name of this function is patterned after the Java function (as this
     *  provides some consistency between the Java and C++ version), but the
     *  implementation is entirely different.
     */
    inline int32_t floatToRawIntBits (float val) {
      return safecastT1ToT2<float,int32_t>(val);
    }

    /** Converts the 64-bits corresponding to an IEEE-764 double, as contained
     *  in a 64-bit integer, into an IEEE-764 double. <br>
     *  <br>
     *  The name of this function is patterned after the Java function (as this
     *  provides some consistency between the Java and C++ version), but the
     *  implementation is entirely different.
     */
    inline double longBitsToDouble (int64_t bits) {
      return safecastT1ToT2<int64_t,double>(bits);
    }

    /** Converts the 32-bits corresponding to an IEEE-764 float, as contained
     *  in a 32-bit integer, into an IEEE-764 float. <br>
     *  <br>
     *  The name of this function is patterned after the Java function (as this
     *  provides some consistency between the Java and C++ version), but the
     *  implementation is entirely different.
     */
    inline float intBitsToFloat (int32_t bits) {
      return safecastT1ToT2<int32_t,float>(bits);
    }

    // We have checked and the compiler (GCC 4.4.5 with -O3), when in-lining any
    // calls in the pack/unpack methods will optimize out the "if (rep == BYTE_ORDER) {"
    // branch in situations where the byte order is already indicated to be a
    // compile-time constant (i.e. explicitly BIG_ENDIAN or LITTLE_ENDIAN).

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // IEEE 754-2008
    //////////////////////////////////////////////////////////////////////////////////////////////////
    /** <b>Internal Use Only:</b> Converts an IEEE 754 single-precision
     *  floating-point value to the bits equating to an IEEE 754-2008
     *  half-precision (storage only) value. <br>
     *  <br>
     *  This is the "internal library" call provided as a companion to
     *  <tt>toHalf(..)</tt> and should never be called directly.
     *  @param val The single-precision value.
     *  @return The bits representing the half-precision value.
     */
    int16_t _toHalfInternal (float val);

    /** Converts an IEEE 754 single-precision floating-point value to the bits
     *  equating to an IEEE 754-2008 half-precision (storage only) value.
     *  <pre>
     *    References:
     *      [1] IEEE 754-2008.
     *      [2] Jeroen van der Zijp. "Fast Half Float Conversions." November
     *          2008 (Revised September 2010).
     *          ftp://ftp.fox-toolkit.org/pub/fasthalffloatconversion.pdf
     *      [3] Free Software Foundation, Inc. "Using the GNU Compiler Collection
     *          (GCC)." 2013. [Section 6.12 "Half-Precision Floating Point."]
     *          http://gcc.gnu.org/onlinedocs/gcc/Half_002dPrecision.html
     *          [Last Accessed 5 Aug 2013]
     *      [4] Intel. "Intel C++ Compiler XE 13.1 User and Reference Guides."
     *          Document Number 323273-131US. 2013. ["Intrinsics for Converting
     *          Half Floats."]
     *          http://software.intel.com/sites/products/documentation/doclib/iss/<!--
     *          -->2013/compiler/cpp-lin/GUID-23C8FF6D-3D66-4961-A3A0-E2A7DC874304.htm
     *          [Last Accessed 5 Aug 2013]
     *  </pre>
     *  @param val The single-precision value.
     *  @return The bits representing the half-precision value.
     */
    inline int16_t toHalf (float val) {
#if defined(__INTEL_COMPILER) && (INTERNAL_FLOAT16_LIB == 0)
      return (int16_t)_cvtss_sh(val, 0); // 0 = IMM8 value with no overrides set
#elif defined(__GNU_COMPILER) && (INTERNAL_FLOAT16_LIB == 0)
      __fp16 value = val;  // <-- __fp16 is defined data type in GCC
      return *((int16_t*)(void*)&value);
#else
      return _toHalfInternal(val);
#endif
    }

    /** Converts an IEEE 754-2008 half-precision floating-point (storage only)
     *  bit representation to an IEEE 754 single-precision floating-point value. <br>
     *  <br>
     *  This is the "internal library" call provided as a companion to
     *  <tt>fromHalf(..)</tt> and should never be called directly.
     *  @param bits The bits representing the half-precision value.
     *  @return The single-precision value.
     */
    float _fromHalfInternal (int16_t bits);

    /** Converts an IEEE 754-2008 half-precision floating-point (storage only)
     *  bit representation to an IEEE 754 single-precision floating-point value.
     *  <pre>
     *    References:
     *      [1] IEEE 754-2008.
     *      [2] Jeroen van der Zijp. "Fast Half Float Conversions." November
     *          2008 (Revised September 2010).
     *          ftp://ftp.fox-toolkit.org/pub/fasthalffloatconversion.pdf
     *      [3] Free Software Foundation, Inc. "Using the GNU Compiler Collection
     *          (GCC)." 2013. [Section 6.12 "Half-Precision Floating Point."]
     *          http://gcc.gnu.org/onlinedocs/gcc/Half_002dPrecision.html
     *          [Last Accessed 5 Aug 2013]
     *      [4] Intel. "Intel C++ Compiler XE 13.1 User and Reference Guides."
     *          Document Number 323273-131US. 2013. ["Intrinsics for Converting
     *          Half Floats."]
     *          http://software.intel.com/sites/products/documentation/doclib/iss/<!--
     *          -->2013/compiler/cpp-lin/GUID-23C8FF6D-3D66-4961-A3A0-E2A7DC874304.htm
     *          [Last Accessed 5 Aug 2013]
     *  </pre>
     *  @param bits The bits representing the half-precision value.
     *  @return The single-precision value.
     */
    inline float fromHalf (int16_t bits) {
#if defined(__INTEL_COMPILER) && (INTERNAL_FLOAT16_LIB == 0)
      // The docs say to use the following call, but all of the versions of
      // <emmintrin.h> that I've seen omit the 'imm' argument.
      // return _cvtsh_ss((uint16_t)bits, 0); // 0 = IMM8 value with no overrides set
      return _cvtsh_ss((uint16_t)bits);
#elif defined(__GNU_COMPILER) && (INTERNAL_FLOAT16_LIB == 0)
      __fp16 value = *((__fp16*)(void*)&bits);  // <-- __fp16 is defined data type in GCC
      return (float)value;
#else
      return _fromHalfInternal(bits);
#endif
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // 64-BIT
    //////////////////////////////////////////////////////////////////////////////////////////////////
    /** Converts a 64-bit fixed point number to a float.
     *  @param radixPoint The radix point of the number (1..63).
     *  @param bits       The bits representing the fixed-point number.
     *  @return The floating-point value of the fixed point number.
     */
    inline double toDouble64 (int32_t radixPoint, int64_t bits) {
      double divisor = (double)(__INT64_C(0x1) << radixPoint);
      return bits / divisor;
    }

    /** Converts a float to a 64-bit fixed point number.
     *  @param radixPoint The radix point of the number (1..63).
     *  @param val        The floating-point value.
     *  @return The bits representing the fixed-point number.
     */
    inline int64_t fromDouble64 (int32_t radixPoint, double val) {
      double divisor = (double)(__INT64_C(0x1) << radixPoint);
      double bitVal  = rint(val * divisor);
      if (bitVal > numeric_limits<int64_t>::max()) return __INT64_C(0x7FFFFFFFFFFFFFFF);
      if (bitVal < numeric_limits<int64_t>::min()) return __INT64_C(0x8000000000000000);
      return (int64_t)bitVal;
    }

    /** Converts a 64-bit fixed point number to a float.
     *  @param radixPoint The radix point of the number (1..63).
     *  @param bits       The bits representing the fixed-point number.
     *  @return The floating-point value of the fixed point number.
     */
    inline float toFloat64 (int32_t radixPoint, int64_t bits) {
      return (float)toDouble64(radixPoint, bits);
    }

    /** Converts a float to a 64-bit fixed point number.
     *  @param radixPoint The radix point of the number (1..63).
     *  @param val        The floating-point value.
     *  @return The bits representing the fixed-point number.
     */
    inline int64_t fromFloat64 (int32_t radixPoint, float val) {
      return fromDouble64(radixPoint, val);
    }

    /** Converts a 64-bit fixed point number to a long.
     *  @param radixPoint The radix point of the number (1..63).
     *  @param bits       The bits representing the fixed-point number.
     *  @return The integer value of the fixed point number.
     */
    inline int64_t toLong64 (int32_t radixPoint, int64_t bits) {
      // We use the double-negative trick to make rounding work for negative values;
      // however, in the rare case that bits=MIN_VALUE we can't do this, but that's
      // OK since such a value doesn't need to be rounded.
      return ((bits < 0) && (bits != numeric_limits<int64_t>::min()))? -(-bits >> radixPoint) : (bits >> radixPoint);
    }

    /** Converts a int64_t to a 64-bit fixed point number.
     *  @param radixPoint The radix point of the number (1..63).
     *  @param val        The integer value.
     *  @return The bits representing the fixed-point number.
     */
    inline int64_t fromLong64 (int32_t radixPoint, int64_t val) {
      if (val > (numeric_limits<int64_t>::max() >> radixPoint)) return __INT64_C(0x7FFFFFFFFFFFFFFF);
      if (val < (numeric_limits<int64_t>::min() >> radixPoint)) return __INT64_C(0x8000000000000000);
      return (val << radixPoint);
    }

    /** Converts a 64-bit fixed point number to an int.
     *  @param radixPoint The radix point of the number (1..63).
     *  @param bits       The bits representing the fixed-point number.
     *  @return The integer value of the fixed point number.
     */
    inline int32_t toInt64 (int32_t radixPoint, int64_t bits) {
      return (int32_t)toLong64(radixPoint, bits);
    }

    /** Converts an int32_t to a 64-bit fixed point number.
     *  @param radixPoint The radix point of the number (1..63).
     *  @param val        The integer value.
     *  @return The bits representing the fixed-point number.
     */
    inline int64_t fromInt64 (int32_t radixPoint, int32_t val) {
      return fromLong64(radixPoint, (int64_t)val);
    }


    //////////////////////////////////////////////////////////////////////////////////////////////////
    // 32-BIT
    //////////////////////////////////////////////////////////////////////////////////////////////////
    /** Converts a 32-bit fixed point number to a float.
     *  @param radixPoint The radix point of the number (1..31).
     *  @param bits       The bits representing the fixed-point number.
     *  @return The floating-point value of the fixed point number.
     */
    inline double toDouble32 (int32_t radixPoint, int32_t bits) {
      double divisor = (double)(__INT64_C(0x1) << radixPoint);
      return bits / divisor;
    }

    /** Converts a float to a 32-bit fixed point number.
     *  @param radixPoint The radix point of the number (1..31).
     *  @param val        The floating-point value.
     *  @return The bits representing the fixed-point number.
     */
    inline int32_t fromDouble32 (int32_t radixPoint, double val) {
      double divisor = (double)(__INT64_C(0x1) << radixPoint);
      double bitVal  = rint(val * divisor);
      if (bitVal > numeric_limits<int32_t>::max()) return 0x7FFFFFFF;
      if (bitVal < numeric_limits<int32_t>::min()) return 0x80000000;
      return (int32_t)bitVal;
    }

    /** Converts a 32-bit fixed point number to a float.
     *  @param radixPoint The radix point of the number (1..31).
     *  @param bits       The bits representing the fixed-point number.
     *  @return The floating-point value of the fixed point number.
     */
    inline float toFloat32 (int32_t radixPoint, int32_t bits) {
      return (float)toDouble32(radixPoint, bits);
    }

    /** Converts a float to a 32-bit fixed point number.
     *  @param radixPoint The radix point of the number (1..31).
     *  @param val        The floating-point value.
     *  @return The bits representing the fixed-point number.
     */
    inline int32_t fromFloat32 (int32_t radixPoint, float val) {
      return fromDouble32(radixPoint, val);
    }

    /** Converts a 32-bit fixed point number to an int.
     *  @param radixPoint The radix point of the number (1..31).
     *  @param bits       The bits representing the fixed-point number.
     *  @return The integer value of the fixed point number.
     */
    inline int32_t toInt32 (int32_t radixPoint, int32_t bits) {
      // We use the double-negative trick to make rounding work for negative values;
      // however, in the rare case that bits=MIN_VALUE we can't do this, but that's
      // OK since such a value doesn't need to be rounded.
      return ((bits < 0) && (bits != numeric_limits<int32_t>::min()))? -(-bits >> radixPoint) : (bits >> radixPoint);
    }

    /** Converts an int32_t to a 32-bit fixed point number.
     *  @param radixPoint The radix point of the number (1..31).
     *  @param val        The integer value.
     *  @return The bits representing the fixed-point number.
     */
    inline int32_t fromInt32 (int32_t radixPoint, int32_t val) {
      if (val > (numeric_limits<int32_t>::max() >> radixPoint)) return 0x7FFFFFFF;
      if (val < (numeric_limits<int32_t>::min() >> radixPoint)) return 0x80000000;
      return (val << radixPoint);
    }

    /** Converts a 32-bit fixed point number to a long.
     *  @param radixPoint The radix point of the number (1..31).
     *  @param bits       The bits representing the fixed-point number.
     *  @return The integer value of the fixed point number.
     */
    inline int64_t toLong32 (int32_t radixPoint, int32_t bits) {
      return toInt32(radixPoint, bits);
    }

    /** Converts a int64_t to a 32-bit fixed point number.
     *  @param radixPoint The radix point of the number (1..31).
     *  @param val        The integer value.
     *  @return The bits representing the fixed-point number.
     */
    inline int32_t fromLong32 (int32_t radixPoint, int64_t val) {
      if (val > (numeric_limits<int32_t>::max() >> radixPoint)) return 0x7FFFFFFF;
      if (val < (numeric_limits<int32_t>::min() >> radixPoint)) return 0x80000000;
      return (int32_t)(val << radixPoint);
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // 16-BIT
    //////////////////////////////////////////////////////////////////////////////////////////////////
    /** Converts a 16-bit fixed point number to a float.
     *  @param radixPoint The radix point of the number (1..15).
     *  @param bits       The bits representing the fixed-point number.
     *  @return The floating-point value of the fixed point number.
     */
    inline double toDouble16 (int32_t radixPoint, int16_t bits) {
      double divisor = (double)(__INT64_C(0x1) << radixPoint);
      return bits / divisor;
    }

    /** Converts a float to a 32-bit fixed point number.
     *  @param radixPoint The radix point of the number (1..15).
     *  @param val        The floating-point value.
     *  @return The bits representing the fixed-point number.
     */
    inline int16_t fromDouble16 (int32_t radixPoint, double val) {
      double divisor = (double)(__INT64_C(0x1) << radixPoint);
      double bitVal  = rint(val * divisor);
      if (bitVal > numeric_limits<int16_t>::max()) return (int16_t)0x7FFF;
      if (bitVal < numeric_limits<int16_t>::min()) return (int16_t)0x8000;
      return (int16_t)bitVal;
    }

    /** Converts a 16-bit fixed point number to a float.
     *  @param radixPoint The radix point of the number (1..15).
     *  @param bits       The bits representing the fixed-point number.
     *  @return The floating-point value of the fixed point number.
     */
    inline float toFloat16 (int32_t radixPoint, int16_t bits) {
      return (float)toDouble16(radixPoint, bits);
    }

    /** Converts a float to a 16-bit fixed point number.
     *  @param radixPoint The radix point of the number (1..15).
     *  @param val        The floating-point value.
     *  @return The bits representing the fixed-point number.
     */
    inline int16_t fromFloat16 (int32_t radixPoint, float val) {
      return fromDouble16(radixPoint, val);
    }

    /** Converts a 16-bit fixed point number to an int.
     *  @param radixPoint The radix point of the number (1..15).
     *  @param bits       The bits representing the fixed-point number.
     *  @return The integer value of the fixed point number.
     */
    inline int32_t toInt16 (int32_t radixPoint, int16_t bits) {
      // Unlike the 32-/64-bit versions, we can avoid the strange bits=MIN_VALUE
      // case by just doing 32-bit math (which is what the host system prefers
      // anyhow).
      int32_t _bits = bits;
      return (_bits < 0)? -(-_bits >> radixPoint) : (_bits >> radixPoint);
    }

    /** Converts an int32_t to a 16-bit fixed point number.
     *  @param radixPoint The radix point of the number (1..15).
     *  @param val        The integer value.
     *  @return The bits representing the fixed-point number.
     */
    inline int16_t fromInt16 (int32_t radixPoint, int32_t val) {
      if (val > (numeric_limits<int16_t>::max() >> radixPoint)) return (int16_t)0x7FFF;
      if (val < (numeric_limits<int16_t>::min() >> radixPoint)) return (int16_t)0x8000;
      return (int16_t)(val << radixPoint);
    }

    /** Converts a 16-bit fixed point number to a long.
     *  @param radixPoint The radix point of the number (1..15).
     *  @param bits       The bits representing the fixed-point number.
     *  @return The integer value of the fixed point number.
     */
    inline int64_t toLong16 (int32_t radixPoint, int16_t bits) {
      return toInt16(radixPoint, bits);
    }

    /** Converts a int64_t to a 16-bit fixed point number.
     *  @param radixPoint The radix point of the number (1..15).
     *  @param val        The integer value.
     *  @return The bits representing the fixed-point number.
     */
    inline int16_t fromLong16 (int32_t radixPoint, int64_t val) {
      if (val > (numeric_limits<int16_t>::max() >> radixPoint)) return (int16_t)0x7FFF;
      if (val < (numeric_limits<int16_t>::min() >> radixPoint)) return (int16_t)0x8000;
      return (int16_t)(val << radixPoint);
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // SPECIAL BIT PACK/UNPACK METHODS
    //////////////////////////////////////////////////////////////////////////////////////////////////
    /** <b>Internal Use Only:</b> Reads bits from a buffer without safety checks. */
    int32_t unpackBits32 (const void *ptr, int32_t bitOffset, int32_t bitCount);

    /** <b>Internal Use Only:</b> Reads bits from a buffer without safety checks. */
    int64_t unpackBits64 (const void *ptr, int32_t bitOffset, int32_t bitCount);

    /** <b>Internal Use Only:</b> Writes bits to a buffer without safety checks. */
    void packBits32 (void *ptr, int32_t bitOffset, int32_t bitCount, int32_t bits);

    /** <b>Internal Use Only:</b> Writes bits to a buffer without safety checks. */
    void packBits64 (void *ptr, int32_t bitOffset, int32_t bitCount, int64_t bits);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // UNPACK VALUE
    //////////////////////////////////////////////////////////////////////////////////////////////////
    /** Unpack byte data from a byte buffer
     *  @param ptr pointer to byte array of data
     *  @param off Offset into array
     *  @return short value
     */
    inline char unpackByte (const void *ptr, int32_t off) {
      const char *buf = (const char*)ptr;
      return buf[off];
    }

    /** Unpack byte data from a byte buffer
     *  @param buf byte array of data
     *  @param off Offset into array
     *  @return short value
     */
    inline char unpackByte (const vector<char> &buf, int32_t off) {
      return unpackByte(&buf[0], off);
    }

    /** Unpack short data from a byte buffer
     *  @param ptr pointer to byte array of data
     *  @param off Offset into array
     *  @param rep The byte representation to use (BIG_ENDIAN or LITTLE_ENDIAN)
     *  @return short value
     */
    inline int16_t unpackShort (const void *ptr, int32_t off, ByteOrder rep=BIG_ENDIAN) {
      const char *buf = (const char*)ptr;
      return (rep == BYTE_ORDER)? *((int16_t*)&buf[off])
                     : (int16_t)__bswap_16(*((int16_t*)&buf[off]));
    }

    /** Unpack short data from a byte buffer
     *  @param buf byte array of data
     *  @param off Offset into array
     *  @param rep The byte representation to use (BIG_ENDIAN or LITTLE_ENDIAN)
     *  @return short value
     */
    inline int16_t unpackShort (const vector<char> &buf, int32_t off, ByteOrder rep=BIG_ENDIAN) {
      return unpackShort(&buf[0], off, rep);
    }

    /** <b>Internal Use Only:</b> Unpack a 24-bit integer value as an int data from a byte buffer
     *  @param ptr pointer to byte array of data
     *  @param off Offset into array
     *  @param rep The byte representation to use (BIG_ENDIAN or LITTLE_ENDIAN)
     *  @return int value
     */
    inline int32_t unpackInt24 (const void *ptr, int32_t off, ByteOrder rep=BIG_ENDIAN) {
      const char *buf = (const char*)ptr;
      int32_t val = (rep==BIG_ENDIAN)?
                      ((int32_t)buf[off+2]&0xFF) | (((int32_t)buf[off+1]&0xFF)<<8) | (((int32_t)buf[off+0]&0xFF)<<16):
                      ((int32_t)buf[off+0]&0xFF) | (((int32_t)buf[off+1]&0xFF)<<8) | (((int32_t)buf[off+2]&0xFF)<<16);
      return ((val << 8) >> 8); // sign-extend
    }

    /** <b>Internal Use Only:</b> Unpack a 24-bit integer value as an int data from a byte buffer
     *  @param buf byte array of data
     *  @param off Offset into array
     *  @param rep The byte representation to use (BIG_ENDIAN or LITTLE_ENDIAN)
     *  @return int value
     */
    inline int32_t unpackInt24 (const vector<char> &buf, int32_t off, ByteOrder rep=BIG_ENDIAN) {
      return unpackInt24(&buf[0], off, rep);
    }

    /** Unpack int data from a byte buffer
     *  @param ptr pointer to byte array of data
     *  @param off Offset into array
     *  @param rep The byte representation to use (BIG_ENDIAN or LITTLE_ENDIAN)
     *  @return int value
     */
    inline int32_t unpackInt (const void *ptr, int32_t off, ByteOrder rep=BIG_ENDIAN) {
      const char *buf = (const char*)ptr;
      return (rep == BYTE_ORDER)? *((int32_t*)&buf[off])
                     : __bswap_32(*((int32_t*)&buf[off]));
    }

    /** Unpack int data from a byte buffer
     *  @param buf byte array of data
     *  @param off Offset into array
     *  @param rep The byte representation to use (BIG_ENDIAN or LITTLE_ENDIAN)
     *  @return int value
     */
    inline int32_t unpackInt (const vector<char> &buf, int32_t off, ByteOrder rep=BIG_ENDIAN) {
      return unpackInt(&buf[0], off, rep);
    }

    /** Unpack long data from a byte buffer
     *  @param ptr pointer to byte array of data
     *  @param off Offset into array
     *  @param rep The byte representation to use (BIG_ENDIAN or LITTLE_ENDIAN)
     *  @return long value
     */
    inline int64_t unpackLong (const void *ptr, int32_t off, ByteOrder rep=BIG_ENDIAN) {
      const char *buf = (const char*)ptr;
      return (rep == BYTE_ORDER)? *((int64_t*)&buf[off])
                     : __bswap_64(*((int64_t*)&buf[off]));
    }

    /** Unpack long data from a byte buffer
     *  @param buf byte array of data
     *  @param off Offset into array
     *  @param rep The byte representation to use (BIG_ENDIAN or LITTLE_ENDIAN)
     *  @return long value
     */
    inline int64_t unpackLong (const vector<char> &buf, int32_t off, ByteOrder rep=BIG_ENDIAN) {
      return unpackLong(&buf[0], off, rep);
    }

    /** Unpack unsigned byte data from a byte buffer
     *  @param ptr pointer to byte array of data
     *  @param off Offset into array
     *  @return short value
     */
    inline unsigned char unpackUByte (const void *ptr, int32_t off) {
      unsigned const char *buf = (unsigned const char*)ptr;
      return buf[off];
    }

    /** Unpack unsigned byte data from a byte buffer
     *  @param buf byte array of data
     *  @param off Offset into array
     *  @return short value
     */
    inline unsigned char unpackUByte (const vector<char> &buf, int32_t off) {
      return unpackUByte(&buf[0], off);
    }

    /** Unpack unsigned short data from a byte buffer
     *  @param ptr pointer to byte array of data
     *  @param off Offset into array
     *  @param rep The byte representation to use (BIG_ENDIAN or LITTLE_ENDIAN)
     *  @return short value
     */
    inline uint16_t unpackUShort (const void *ptr, int32_t off, ByteOrder rep=BIG_ENDIAN) {
      const char *buf = (const char*)ptr;
      return (rep == BYTE_ORDER)? *((uint16_t*)&buf[off])
                     : (uint16_t)__bswap_16(*((uint16_t*)&buf[off]));
    }

    /** Unpack unsigned short data from a byte buffer
     *  @param buf byte array of data
     *  @param off Offset into array
     *  @param rep The byte representation to use (BIG_ENDIAN or LITTLE_ENDIAN)
     *  @return short value
     */
    inline uint16_t unpackUShort (const vector<char> &buf, int32_t off, ByteOrder rep=BIG_ENDIAN) {
      return unpackShort(&buf[0], off, rep);
    }

    /** <b>Internal Use Only:</b> Unpack an unsigned  24-bit integer value as an int data from a byte buffer
     *  @param ptr pointer to byte array of data
     *  @param off Offset into array
     *  @param rep The byte representation to use (BIG_ENDIAN or LITTLE_ENDIAN)
     *  @return int value
     */
    inline uint32_t unpackUInt24 (const void *ptr, int32_t off, ByteOrder rep=BIG_ENDIAN) {
      return unpackInt24(ptr, off, rep) & 0x00FFFFFF;
    }

    /** <b>Internal Use Only:</b> Unpack an unsigned 24-bit integer value as an int data from a byte buffer
     *  @param buf byte array of data
     *  @param off Offset into array
     *  @param rep The byte representation to use (BIG_ENDIAN or LITTLE_ENDIAN)
     *  @return int value
     */
    inline uint32_t unpackUInt24 (const vector<char> &buf, int32_t off, ByteOrder rep=BIG_ENDIAN) {
      return unpackUInt24(&buf[0], off, rep);
    }

    /** Unpack unsigned int data from a byte buffer
     *  @param ptr pointer to byte array of data
     *  @param off Offset into array
     *  @param rep The byte representation to use (BIG_ENDIAN or LITTLE_ENDIAN)
     *  @return int value
     */
    inline uint32_t unpackUInt (const void *ptr, int32_t off, ByteOrder rep=BIG_ENDIAN) {
      const char *buf = (const char*)ptr;
      return (rep == BYTE_ORDER)? *((uint32_t*)&buf[off])
                     : __bswap_32(*((uint32_t*)&buf[off]));
    }

    /** Unpack unsigned int data from a byte buffer
     *  @param buf byte array of data
     *  @param off Offset into array
     *  @param rep The byte representation to use (BIG_ENDIAN or LITTLE_ENDIAN)
     *  @return int value
     */
    inline uint32_t unpackUInt (const vector<char> &buf, int32_t off, ByteOrder rep=BIG_ENDIAN) {
      return unpackInt(&buf[0], off, rep);
    }

    /** Unpack unsigned long data from a byte buffer
     *  @param ptr pointer to byte array of data
     *  @param off Offset into array
     *  @param rep The byte representation to use (BIG_ENDIAN or LITTLE_ENDIAN)
     *  @return long value
     */
    inline uint64_t unpackULong (const void *ptr, int32_t off, ByteOrder rep=BIG_ENDIAN) {
      const char *buf = (const char*)ptr;
      return (rep == BYTE_ORDER)? *((uint64_t*)&buf[off])
                     : __bswap_64(*((uint64_t*)&buf[off]));
    }

    /** Unpack unsigned long data from a byte buffer
     *  @param buf byte array of data
     *  @param off Offset into array
     *  @param rep The byte representation to use (BIG_ENDIAN or LITTLE_ENDIAN)
     *  @return long value
     */
    inline uint64_t unpackULong (const vector<char> &buf, int32_t off, ByteOrder rep=BIG_ENDIAN) {
      return unpackULong(&buf[0], off, rep);
    }

    /** Unpack float data from a byte buffer
     *  @param ptr pointer to byte array of data
     *  @param off Offset into array
     *  @param rep The byte representation to use (BIG_ENDIAN or LITTLE_ENDIAN)
     *  @return float value
     */
    inline float unpackFloat (const void *ptr, int32_t off, ByteOrder rep=BIG_ENDIAN) {
      const char *buf = (const char*)ptr;
      if (rep == BYTE_ORDER) {
        return *((float*)&buf[off]);
      }
      else {
        uint32_t val = __bswap_32(*(uint32_t*)&buf[off]);
        void* v = &val;
        return *(float*)v;
      }
    }

    /** Unpack float data from a byte buffer
     *  @param buf byte array of data
     *  @param off Offset into array
     *  @param rep The byte representation to use (BIG_ENDIAN or LITTLE_ENDIAN)
     *  @return float value
     */
    inline float unpackFloat (const vector<char> &buf, int32_t off, ByteOrder rep=BIG_ENDIAN) {
      return unpackFloat(&buf[0], off, rep);
    }

    /** Unpack double data from a byte buffer
     *  @param ptr pointer to byte array of data
     *  @param off Offset into array
     *  @param rep The byte representation to use (BIG_ENDIAN or LITTLE_ENDIAN)
     *  @return double value
     */
    inline double unpackDouble (const void *ptr, int32_t off, ByteOrder rep=BIG_ENDIAN) {
      const char *buf = (const char*)ptr;
      if (rep == BYTE_ORDER) {
        return *((double*)&buf[off]);
      }
      else {
        uint64_t val = __bswap_64(*(uint64_t*)&buf[off]);
        void* v = &val;
        return *(double*)v;
      }
    }

    /** Unpack double data from a byte buffer
     *  @param buf byte array of data
     *  @param off Offset into array
     *  @param rep The byte representation to use (BIG_ENDIAN or LITTLE_ENDIAN)
     *  @return double value
     */
    inline double unpackDouble (const vector<char> &buf, int32_t off, ByteOrder rep=BIG_ENDIAN) {
      return unpackDouble(&buf[0], off, rep);
    }

////////////////////////////////////////////////////////////////////////////////
#if NOT_USING_JNI
////////////////////////////////////////////////////////////////////////////////
    /** Unpack a boolean from a buffer.
     *  @param ptr pointer to byte array of data
     *  @param off Offset into array
     *  @return The unpacked value
     *  @throws VRTException If {@link VRTConfig#getStrict()} is <tt>true</tt> and the stored value is
     *          something other than <tt>-1</tt>, <tt>0</tt> or <tt>1</tt>.
     */
    boolNull unpackBoolNull (const void *ptr, int32_t off);

    /** Unpack a boolean from a buffer.
     *  @param buf byte array of data
     *  @param off Offset into array
     *  @return The unpacked value
     *  @throws VRTException If {@link VRTConfig#getStrict()} is <tt>true</tt> and the stored value is
     *          something other than <tt>-1</tt>, <tt>0</tt> or <tt>1</tt>.
     */
    inline boolNull unpackBoolNull (const vector<char> &buf, int32_t off) {
      return unpackBoolNull(&buf[0], off);
    }

    /** Unpack a boolean from a buffer.
     *  @param ptr pointer to byte array of data
     *  @param off Offset into array
     *  @return The unpacked value
     *  @throws VRTException If {@link VRTConfig#getStrict()} is <tt>true</tt> and the stored value is
     *          something other than <tt>-1</tt>, <tt>0</tt> or <tt>1</tt>.
     */
    bool unpackBoolean (const void *ptr, int32_t off);

    /** Unpack a boolean from a buffer.
     *  @param buf byte array of data
     *  @param off Offset into array
     *  @return The unpacked value
     *  @throws VRTException If {@link VRTConfig#getStrict()} is <tt>true</tt> and the stored value is
     *          something other than <tt>-1</tt>, <tt>0</tt> or <tt>1</tt>.
     */
    inline bool unpackBoolean (const vector<char> &buf, int32_t off) {
      return unpackBoolean(&buf[0], off);
    }

    /** Unpack a ASCII string from a buffer. The string is assumed to be null-terminated in the event
     *  that the string is shorter than the allowable length.
     *  @param ptr pointer to byte array of data
     *  @param off Offset into array
     *  @param len length of the string in the buffer
     *  @return The unpacked value
     */
    string unpackAscii (const void *ptr, int32_t off, int32_t len);

    /** Unpack a ASCII string from a buffer. The string is assumed to be null-terminated in the event
     *  that the string is shorter than the allowable length.
     *  @param buf byte array of data
     *  @param off Offset into array
     *  @param len length of the string in the buffer
     *  @return The unpacked value
     */
    inline string unpackAscii (const vector<char> &buf, int32_t off, int32_t len) {
      return unpackAscii(&buf[0], off, len);
    }

    /** Unpack a modified UTF-8 string from a buffer. The string is assumed to be null-terminated
     *  in the event that the string is shorter than the allowable length. <br>
     *  <br>
     *  The modified UTF-8 format is identical to "normal" UTF-8 except that any embedded NUL
     *  values within the string are encoded as two-byte values (similar to what is done in the
     *  Java VM). Note that unlike the Java VM and some other 16-bit UTF representations in C++ this
     *  does NOT rely on "surrogate pairs" and supports the 1-/2-/3-/4-octet forms of UTF-8:
     *  <pre>
     *    Values in the 0x0001 to 0x007F range (ASCII, except NUL):
     *      Octet 1: [ 0 x x x x x x x ]
     *
     *    Values in the 0x0080 to 0x07FF range and 0x0000 (ASCII NUL):
     *      Octet 1: [ 1 1 0 x x x x x ]
     *      Octet 2: [ 1 0 x x x x x x ]
     *
     *    Values in the 0x0800 to 0xFFFF range:
     *      Octet 1: [ 1 1 1 0 x x x x ]
     *      Octet 2: [ 1 0 x x x x x x ]
     *      Octet 3: [ 1 0 x x x x x x ]
     *
     *    Values in the 0x10000 to 0x10FFFF range:
     *      Octet 1: [ 1 1 1 1 0 x x x ]
     *      Octet 2: [ 1 0 x x x x x x ]
     *      Octet 3: [ 1 0 x x x x x x ]
     *      Octet 4: [ 1 0 x x x x x x ]
     *  </pre>
     *  @param ptr         Pointer to byte array of data
     *  @param off         Offset into array
     *  @param len         Length of the string in the buffer
     *  @param replacement The replacement character to use whenever a non UTF-8 character is
     *                     encountered (typically '?'), if null an exception is thrown rather than
     *                     replacing erroneous characters.
     *  @return The unpacked value.
     *  @throws VRTException If the string found is not proper UTF-8 and replacement is null.
     */
    wstring unpackUTF8 (const void *ptr, int32_t off, int32_t len, wchar_t replacement='?');

    /** Unpack a modified UTF-8 string from a buffer. See the version of this method that
     *  takes in a void* for more details.
     *  @param buf         Byte array of data
     *  @param off         Offset into array
     *  @param len         Length of the string in the buffer
     *  @param replacement The replacement character to use whenever a non UTF-8 character is
     *                     encountered (typically '?'), if null an exception is thrown rather than
     *                     replacing erroneous characters.
     *  @return The unpacked value.
     *  @throws VRTException If the string found is not proper UTF-8 and replacement is null.
     */
    inline wstring unpackUTF8 (const vector<char> &buf, int32_t off, int32_t len, wchar_t replacement='?') {
      return unpackUTF8(&buf[0], off, len, replacement);
    }

    /** Unpack an array of bytes from a buffer.
     *  @param buf byte array of data
     *  @param off Offset into array
     *  @param len length of the array in the buffer
     *  @return The unpacked value
     */
    inline vector<char> unpackBytes (const vector<char> &buf, int32_t off, int32_t len) {
      vector<char> buffer(len);
      memcpy(&buffer[0], &buf[off], len);
      return buffer;
    }

    /** Unpack an array of bytes from a buffer.
     *  @param buf    byte array of data [INPUT]
     *  @param off    Offset into array
     *  @param buffer the output buffer [OUTPUT]
     *  @param len    length of the array in the buffer
     *  @return The output buffer.
     */
    inline void *unpackBytes (const vector<char> &buf, int32_t off, void *buffer, int32_t len) {
      memcpy(buffer, &buf[off], len);
      return buffer;
    }

////////////////////////////////////////////////////////////////////////////////
#endif /* NOT_USING_JNI */
////////////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // PACK VALUE
    //////////////////////////////////////////////////////////////////////////////////////////////////
    /** Pack byte data into byte buffer.
     *  @param ptr Pointer to packed byte array [OUTPUT]
     *  @param off Offset into output array
     *  @param val value to pack [INPUT]
     */
    inline void packByte (void *ptr, int32_t off, char val) {
      char *buf = (char*)ptr;
      buf[off] = val;
    }

    /** Pack byte data into byte buffer.
     *  @param buf Packed byte array [OUTPUT]
     *  @param off Offset into output array
     *  @param val value to pack [INPUT]
     */
    inline void packByte (vector<char> &buf, int32_t off, char val) {
      packByte(&buf[0], off, val);
    }

    /** Pack short data into byte buffer.
     *  @param ptr Pointer to packed byte array [OUTPUT]
     *  @param off Offset into output array
     *  @param val value to pack [INPUT]
     *  @param rep The byte representation to use (BIG_ENDIAN or LITTLE_ENDIAN)
     */
    inline void packShort (void *ptr, int32_t off, int16_t val, ByteOrder rep=BIG_ENDIAN) {
      char *buf = (char*)ptr;
      *((int16_t*)(&buf[off])) = (rep == BYTE_ORDER)? val : (int16_t)__bswap_16(val);
    }

    /** Pack short data into byte buffer.
     *  @param buf Packed byte array [OUTPUT]
     *  @param off Offset into output array
     *  @param val value to pack [INPUT]
     *  @param rep The byte representation to use (BIG_ENDIAN or LITTLE_ENDIAN)
     */
    inline void packShort (vector<char> &buf, int32_t off, int16_t val, ByteOrder rep=BIG_ENDIAN) {
      packShort(&buf[0], off, val, rep);
    }

    /** <b>Internal Use Only:</b> Pack a 24-bit integer value as an int data from
     *  a byte buffer. Any high-order bits (i.e. bits 24 to 31) are ignored.
     *  @param ptr Pointer to packed byte array [OUTPUT]
     *  @param off Offset into output array
     *  @param val value to pack [INPUT]
     *  @param rep The byte representation to use (BIG_ENDIAN or LITTLE_ENDIAN)
     */
    inline void packInt24 (void *ptr, int32_t off, int32_t val, ByteOrder rep=BIG_ENDIAN) {
      char *buf = (char*)ptr;
      if (rep==BIG_ENDIAN) {
        buf[off+2] = (char)(val);
        buf[off+1] = (char)(val>>8);
        buf[off+0] = (char)(val>>16);
      } else {
        buf[off+0] = (char)(val);
        buf[off+1] = (char)(val>>8);
        buf[off+2] = (char)(val>>16);
      }
    }

    /** <b>Internal Use Only:</b> Pack a 24-bit integer value as an int data from
     *  a byte buffer. Any high-order bits (i.e. bits 24 to 31) are ignored.
     *  @param buf Packed byte array [OUTPUT]
     *  @param off Offset into output array
     *  @param val value to pack [INPUT]
     *  @param rep The byte representation to use (BIG_ENDIAN or LITTLE_ENDIAN)
     */
    inline void packInt24 (vector<char> &buf, int32_t off, int32_t val, ByteOrder rep=BIG_ENDIAN) {
      packInt24(&buf[0], off, val, rep);
    }

    /** Pack int data into byte buffer.
     *  @param ptr Pointer to packed byte array [OUTPUT]
     *  @param off Offset into output array
     *  @param val value to pack [INPUT]
     *  @param rep The byte representation to use (BIG_ENDIAN or LITTLE_ENDIAN)
     */
    inline void packInt (void *ptr, int32_t off, int32_t val, ByteOrder rep=BIG_ENDIAN) {
      char *buf = (char*)ptr;
      *((int32_t*)(&buf[off])) = (rep == BYTE_ORDER)? val : __bswap_32(val);
    }

    /** Pack int data into byte buffer.
     *  @param buf Packed byte array [OUTPUT]
     *  @param off Offset into output array
     *  @param val value to pack [INPUT]
     *  @param rep The byte representation to use (BIG_ENDIAN or LITTLE_ENDIAN)
     */
    inline void packInt (vector<char> &buf, int32_t off, int32_t val, ByteOrder rep=BIG_ENDIAN) {
      packInt(&buf[0], off, val, rep);
    }

    /** Pack long data into byte buffer.
     *  @param ptr Pointer to packed byte array [OUTPUT]
     *  @param off Offset into output array
     *  @param val value to pack [INPUT]
     *  @param rep The byte representation to use (BIG_ENDIAN or LITTLE_ENDIAN)
     */
    inline void packLong (void *ptr, int32_t off, int64_t val, ByteOrder rep=BIG_ENDIAN) {
      char *buf = (char*)ptr;
      *((int64_t*)(&buf[off])) = (rep == BYTE_ORDER)? val : __bswap_64(val);
    }

    /** Pack long data into byte buffer.
     *  @param buf Packed byte array [OUTPUT]
     *  @param off Offset into output array
     *  @param val value to pack [INPUT]
     *  @param rep The byte representation to use (BIG_ENDIAN or LITTLE_ENDIAN)
     */
    inline void packLong (vector<char> &buf, int32_t off, int64_t val, ByteOrder rep=BIG_ENDIAN) {
      packLong(&buf[0], off, val, rep);
    }

    /** Pack unsigned byte data into byte buffer.
     *  @param ptr Pointer to packed byte array [OUTPUT]
     *  @param off Offset into output array
     *  @param val value to pack [INPUT]
     */
    inline void packUByte (void *ptr, int32_t off, unsigned char val) {
      char *buf = (char*)ptr;
      *((unsigned char*)(&buf[off])) = val;
    }

    /** Pack unsigned byte data into byte buffer.
     *  @param buf Packed byte array [OUTPUT]
     *  @param off Offset into output array
     *  @param val value to pack [INPUT]
     */
    inline void packUByte (vector<char> &buf, int32_t off, unsigned char val) {
      packUByte(&buf[0], off, val);
    }

    /** Pack unsigned short data into byte buffer.
     *  @param ptr Pointer to packed byte array [OUTPUT]
     *  @param off Offset into output array
     *  @param val value to pack [INPUT]
     *  @param rep The byte representation to use (BIG_ENDIAN or LITTLE_ENDIAN)
     */
    inline void packUShort (void *ptr, int32_t off, uint16_t val, ByteOrder rep=BIG_ENDIAN) {
      char *buf = (char*)ptr;
      *((uint16_t*)(&buf[off])) = (rep == BYTE_ORDER)? val : (uint16_t)__bswap_16(val);
    }

    /** Pack unsigned short data into byte buffer.
     *  @param buf Packed byte array [OUTPUT]
     *  @param off Offset into output array
     *  @param val value to pack [INPUT]
     *  @param rep The byte representation to use (BIG_ENDIAN or LITTLE_ENDIAN)
     */
    inline void packUShort (vector<char> &buf, int32_t off, uint16_t val, ByteOrder rep=BIG_ENDIAN) {
      packUShort(&buf[0], off, val, rep);
    }

    /** <b>Internal Use Only:</b> Pack a 24-bit integer value as an int data from
     *  a byte buffer. Any high-order bits (i.e. bits 24 to 31) are ignored.
     *  @param ptr Pointer to packed byte array [OUTPUT]
     *  @param off Offset into output array
     *  @param val value to pack [INPUT]
     *  @param rep The byte representation to use (BIG_ENDIAN or LITTLE_ENDIAN)
     */
    inline void packUInt24 (void *ptr, int32_t off, int32_t val, ByteOrder rep=BIG_ENDIAN) {
      packInt24(ptr, off, val, rep);
    }

    /** <b>Internal Use Only:</b> Pack a 24-bit integer value as an int data from
     *  a byte buffer. Any high-order bits (i.e. bits 24 to 31) are ignored.
     *  @param buf Packed byte array [OUTPUT]
     *  @param off Offset into output array
     *  @param val value to pack [INPUT]
     *  @param rep The byte representation to use (BIG_ENDIAN or LITTLE_ENDIAN)
     */
    inline void packUInt24 (vector<char> &buf, int32_t off, int32_t val, ByteOrder rep=BIG_ENDIAN) {
      packUInt24(&buf[0], off, val, rep);
    }

    /** Pack unsigned int data into byte buffer.
     *  @param ptr Pointer to packed byte array [OUTPUT]
     *  @param off Offset into output array
     *  @param val value to pack [INPUT]
     *  @param rep The byte representation to use (BIG_ENDIAN or LITTLE_ENDIAN)
     */
    inline void packUInt (void *ptr, int32_t off, uint32_t val, ByteOrder rep=BIG_ENDIAN) {
      char *buf = (char*)ptr;
      *((uint32_t*)(&buf[off])) = (rep == BYTE_ORDER)? val : __bswap_32(val);
    }

    /** Pack unsigned int data into byte buffer.
     *  @param buf Packed byte array [OUTPUT]
     *  @param off Offset into output array
     *  @param val value to pack [INPUT]
     *  @param rep The byte representation to use (BIG_ENDIAN or LITTLE_ENDIAN)
     */
    inline void packUInt (vector<char> &buf, int32_t off, uint32_t val, ByteOrder rep=BIG_ENDIAN) {
      packUInt(&buf[0], off, val, rep);
    }

    /** Pack unsigned long data into byte buffer.
     *  @param ptr Pointer to packed byte array [OUTPUT]
     *  @param off Offset into output array
     *  @param val value to pack [INPUT]
     *  @param rep The byte representation to use (BIG_ENDIAN or LITTLE_ENDIAN)
     */
    inline void packULong (void *ptr, int32_t off, uint64_t val, ByteOrder rep=BIG_ENDIAN) {
      char *buf = (char*)ptr;
      *((uint64_t*)(&buf[off])) = (rep == BYTE_ORDER)? val : __bswap_64(val);
    }

    /** Pack unsigned long data into byte buffer.
     *  @param buf Packed byte array [OUTPUT]
     *  @param off Offset into output array
     *  @param val value to pack [INPUT]
     *  @param rep The byte representation to use (BIG_ENDIAN or LITTLE_ENDIAN)
     */
    inline void packULong (vector<char> &buf, int32_t off, uint64_t val, ByteOrder rep=BIG_ENDIAN) {
      packULong(&buf[0], off, val, rep);
    }

    /** Pack float data into byte buffer.
     *  @param ptr Pointer to packed byte array [OUTPUT]
     *  @param off Offset into output array
     *  @param val value to pack [INPUT]
     *  @param rep The byte representation to use (BIG_ENDIAN or LITTLE_ENDIAN)
     */
    inline void packFloat (void *ptr, int32_t off, float val, ByteOrder rep=BIG_ENDIAN) {
      char *buf = (char*)ptr;
      if (rep == BYTE_ORDER) {
        *((float*)(&buf[off])) = val;
      }
      else {
        void * v = &val;
        *((uint32_t*)(&buf[off])) = __bswap_32(*(uint32_t*)v);
      }
    }

    /** Pack float data into byte buffer.
     *  @param buf Packed byte array [OUTPUT]
     *  @param off Offset into output array
     *  @param val value to pack [INPUT]
     *  @param rep The byte representation to use (BIG_ENDIAN or LITTLE_ENDIAN)
     */
    inline void packFloat (vector<char> &buf, int32_t off, float val, ByteOrder rep=BIG_ENDIAN) {
      packFloat(&buf[0], off, val, rep);
    }

    /** Pack double data into byte buffer.
     *  @param ptr Pointer to packed byte array [OUTPUT]
     *  @param off Offset into output array
     *  @param val value to pack [INPUT]
     *  @param rep The byte representation to use (BIG_ENDIAN or LITTLE_ENDIAN)
     */
    inline void packDouble (void *ptr, int32_t off, double val, ByteOrder rep=BIG_ENDIAN) {
      char *buf = (char*)ptr;
      if (rep == BYTE_ORDER) {
        *((double*)(&buf[off])) = val;
      }
      else {
        void * v = &val;
        *((uint64_t*)(&buf[off])) = __bswap_64(*(uint64_t*)v);
      }
    }

    /** Pack double data into byte buffer.
     *  @param buf Packed byte array [OUTPUT]
     *  @param off Offset into output array
     *  @param val value to pack [INPUT]
     *  @param rep The byte representation to use (BIG_ENDIAN or LITTLE_ENDIAN)
     */
    inline void packDouble (vector<char> &buf, int32_t off, double val, ByteOrder rep=BIG_ENDIAN) {
      packDouble(&buf[0], off, val, rep);
    }
////////////////////////////////////////////////////////////////////////////////
#if NOT_USING_JNI
////////////////////////////////////////////////////////////////////////////////

    /** Pack a boolean into a buffer. The boolean is stored as:
     *  <pre>
     *    null  =  0
     *    true  = +1
     *    false = -1
     *  </pre>
     *  @param ptr Pointer to packed byte array [OUTPUT]
     *  @param off Offset into array
     *  @param val value to pack [INPUT]
     */
    inline void packBoolNull (void *ptr, int32_t off, boolNull val) {
      char *buf = (char*)ptr;
      buf[off] = (char)val;
    }

    /** Pack a boolean into a buffer. The boolean is stored as:
     *  <pre>
     *    null  =  0
     *    true  = +1
     *    false = -1
     *  </pre>
     *  @param buf byte array of data [OUTPUT]
     *  @param off Offset into array
     *  @param val value to pack [INPUT]
     */
    inline void packBoolNull (vector<char> &buf, int32_t off, boolNull val) {
      packBoolNull(&buf[0], off, val);
    }

    /** Pack a boolean into a buffer. The boolean is stored as:
     *  <pre>
     *    true  = +1
     *    false = -1
     *  </pre>
     *  @param ptr Pointer to packed byte array [OUTPUT]
     *  @param off Offset into array
     *  @param val value to pack [INPUT]
     */
    inline void packBoolean (void *ptr, int32_t off, bool val) {
      char *buf = (char*)ptr;
      if (val) buf[off] = (char)+1; // true
      else     buf[off] = (char)-1; // false
    }

    /** Pack a boolean into a buffer. The boolean is stored as:
     *  <pre>
     *    true  = +1
     *    false = -1
     *  </pre>
     *  @param buf byte array of data [OUTPUT]
     *  @param off Offset into array
     *  @param val value to pack [INPUT]
     */
    inline void packBoolean (vector<char> &buf, int32_t off, bool val) {
      packBoolean(&buf[0], off, val);
    }

    /** Pack a ASCII string into a buffer. Any non-ASCII characters or ASCII control characters will
     *  be replaces with '?' upon insertion. If <tt>val</tt> is longer than <tt>len</tt> it will be
     *  truncated without explicit warning. If <tt>val</tt> is shorter than <tt>len</tt> it will be
     *  padded out with ASCII NUL characters to fill the length.
     *  @param ptr    Pointer to packed byte array [OUTPUT]
     *  @param off    Offset into array
     *  @param val    Value to pack [INPUT]
     *  @param length Length of the string in the buffer.
     *  @return The actual length required to hold the entire string. (This is for compatibility with
     *          <tt>packUTF8(..)</tt>, but in practice just returns <tt>val.length()</tt>.)
     */
    int32_t packAscii (void *ptr, int32_t off, const string &val, int32_t length);

    /** Pack a ASCII string into a buffer. Any non-ASCII characters or ASCII control characters will
     *  be replaces with '?' upon insertion. If <tt>val</tt> is longer than <tt>len</tt> it will be
     *  truncated without explicit warning. If <tt>val</tt> is shorter than <tt>len</tt> it will be
     *  padded out with ASCII NUL characters to fill the length.
     *  @param buf    Byte array of data [OUTPUT]
     *  @param off    Offset into array
     *  @param val    Value to pack [INPUT]
     *  @param length Length of the string in the buffer.
     *  @return The actual length required to hold the entire string. (This is for compatibility with
     *          <tt>packUTF8(..)</tt>, but in practice just returns <tt>val.length()</tt>.)
     */
    inline int32_t packAscii (vector<char> &buf, int32_t off, const string &val, int32_t length) {
      return packAscii(&buf[0], off, val, length);
    }

    /** Pack a ASCII string into a buffer. Any non-ASCII characters or ASCII control characters will
     *  be replaces with '?' upon insertion. If <tt>val</tt> is longer than <tt>len</tt> it will be
     *  truncated without explicit warning. If <tt>val</tt> is shorter than <tt>len</tt> it will be
     *  padded out with ASCII NUL characters to fill the length.
     *  @param ptr    Pointer to packed byte array [OUTPUT]
     *  @param off    Offset into array
     *  @param val    Value to pack [INPUT]
     *  @param length Length of the string in the buffer.
     *  @return The actual length required to hold the entire string. (This is for compatibility with
     *          <tt>packUTF8(..)</tt>, but in practice just returns <tt>val.length()</tt>.)
     */
    int32_t packAscii (void *ptr, int32_t off, const wstring &val, int32_t length);

    /** Pack a ASCII string into a buffer. Any non-ASCII characters or ASCII control characters will
     *  be replaces with '?' upon insertion. If <tt>val</tt> is longer than <tt>len</tt> it will be
     *  truncated without explicit warning. If <tt>val</tt> is shorter than <tt>len</tt> it will be
     *  padded out with ASCII NUL characters to fill the length.
     *  @param buf    Byte array of data [OUTPUT]
     *  @param off    Offset into array
     *  @param val    Value to pack [INPUT]
     *  @param length Length of the string in the buffer.
     *  @return The actual length required to hold the entire string. (This is for compatibility with
     *          <tt>packUTF8(..)</tt>, but in practice just returns <tt>val.length()</tt>.)
     */
    inline int32_t packAscii (vector<char> &buf, int32_t off, const wstring &val, int32_t length) {
      return packAscii(&buf[0], off, val, length);
    }

    /** Pack a UTF-8 string into a buffer. If <tt>val</tt> is longer than <tt>len</tt> it will be
     *  truncated without explicit warning. If <tt>val</tt> is shorter than <tt>len</tt> it will be
     *  padded out with ASCII NUL characters to fill the length.
     *  @param ptr    Pointer to packed byte array [OUTPUT]
     *  @param off    Offset into array
     *  @param val    Value to pack [INPUT]
     *  @param length Length of the string in the buffer.
     *  @return The length required to hold the entire UTF-8 string. If this is less than or equal to
     *          <tt>length</tt>, it means that the entire string has been stored in the buffer. If this
     *          is greater than <tt>length</tt>, it means that only part of the string has been stored
     *          in the buffer and that a buffer with the specified size would be required to hold the
     *          entire string. Note that since the individual characters UTF-8 can take from 1 to 4
     *          bytes when stored, this can be used to indicate the required size for a re-sized buffer.
     */
    int32_t packUTF8 (void *ptr, int32_t off, const wstring &val, int32_t length);

    /** Pack a UTF-8 string into a buffer. If <tt>val</tt> is longer than <tt>len</tt> it will be
     *  truncated without explicit warning. If <tt>val</tt> is shorter than <tt>len</tt> it will be
     *  padded out with ASCII NUL characters to fill the length.
     *  @param buf    Byte array of data [OUTPUT]
     *  @param off    Offset into array
     *  @param val    Value to pack [INPUT]
     *  @param length Length of the string in the buffer.
     *  @return The length required to hold the entire UTF-8 string. If this is less than or equal to
     *          <tt>length</tt>, it means that the entire string has been stored in the buffer. If this
     *          is greater than <tt>length</tt>, it means that only part of the string has been stored
     *          in the buffer and that a buffer with the specified size would be required to hold the
     *          entire string. Note that since the individual characters UTF-8 can take from 1 to 4
     *          bytes when stored, this can be used to indicate the required size for a re-sized buffer.
     */
    inline int32_t packUTF8 (vector<char> &buf, int32_t off, const wstring &val, int32_t length) {
      return packUTF8(&buf[0], off, val, length);
    }

    /** Pack a UTF-8 string into a buffer. If <tt>val</tt> is longer than <tt>len</tt> it will be
     *  truncated without explicit warning. If <tt>val</tt> is shorter than <tt>len</tt> it will be
     *  padded out with ASCII NUL characters to fill the length.
     *  @param ptr    Pointer to packed byte array [OUTPUT]
     *  @param off    Offset into array
     *  @param val    Value to pack [INPUT]
     *  @param length Length of the string in the buffer.
     *  @return The length required to hold the entire UTF-8 string. If this is less than or equal to
     *          <tt>length</tt>, it means that the entire string has been stored in the buffer. If this
     *          is greater than <tt>length</tt>, it means that only part of the string has been stored
     *          in the buffer and that a buffer with the specified size would be required to hold the
     *          entire string. Note that since the individual characters UTF-8 can take from 1 to 4
     *          bytes when stored, this can be used to indicate the required size for a re-sized buffer.
     */
    int32_t packUTF8 (void *ptr, int32_t off, const string &val, int32_t length);

    /** Pack a UTF-8 string into a buffer. If <tt>val</tt> is longer than <tt>len</tt> it will be
     *  truncated without explicit warning. If <tt>val</tt> is shorter than <tt>len</tt> it will be
     *  padded out with ASCII NUL characters to fill the length.
     *  @param buf    Byte array of data [OUTPUT]
     *  @param off    Offset into array
     *  @param val    Value to pack [INPUT]
     *  @param length Length of the string in the buffer.
     *  @return The length required to hold the entire UTF-8 string. If this is less than or equal to
     *          <tt>length</tt>, it means that the entire string has been stored in the buffer. If this
     *          is greater than <tt>length</tt>, it means that only part of the string has been stored
     *          in the buffer and that a buffer with the specified size would be required to hold the
     *          entire string. Note that since the individual characters UTF-8 can take from 1 to 4
     *          bytes when stored, this can be used to indicate the required size for a re-sized buffer.
     */
    inline int32_t packUTF8 (vector<char> &buf, int32_t off, const string &val, int32_t length) {
      return packUTF8(&buf[0], off, val, length);
    }

    /** Computes the length a UTF-8 string will need in a buffer.
     *  @param val The string
     *  @return The length required to hold the entire UTF-8 string.
     */
    inline int32_t lengthUTF8 (const wstring &val) {
      return packUTF8(NULL, 0, val, 0);
    }

    /** Computes the length a UTF-8 string will need in a buffer.
     *  @param val The string
     *  @return The length required to hold the entire UTF-8 string.
     */
    inline int32_t lengthUTF8 (const string &val) {
      return packUTF8(NULL, 0, val, 0);
    }

    /** Pack an array of bytes into the buffer.
     *  @param buf    byte array of data [OUTPUT]
     *  @param off    Offset into array
     *  @param buffer The buffer to pack into the array [INPUT]
     *  @param len    The length to pack.
     *  @throws VRTException If <tt>buffer.length != len</tt>.
     */
    inline void packBytes (vector<char> &buf, int32_t off, const vector<char> &buffer, int32_t len) {
      memcpy(&buf[off], &buffer[0], len);
    }

    /** Pack an array of bytes into the buffer.
     *  @param buf    byte array of data [OUTPUT]
     *  @param off    Offset into array
     *  @param buffer The buffer to pack into the array [INPUT]
     *  @param len    The length to pack.
     *  @throws VRTException If <tt>off</tt>, <tt>boff</tt> or <tt>len</tt> are invalid.
     */
    inline void packBytes (vector<char> &buf, int32_t off, const void *buffer, int32_t len) {
      memcpy(&buf[off], buffer, len);
    }
////////////////////////////////////////////////////////////////////////////////
#endif /* NOT_USING_JNI */
////////////////////////////////////////////////////////////////////////////////
  } END_NAMESPACE
} END_NAMESPACE

_Clang_Pragma("clang diagnostic pop")

#endif /* VRTMath_h */

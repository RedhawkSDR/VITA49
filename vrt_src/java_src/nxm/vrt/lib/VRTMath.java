/**
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

package nxm.vrt.lib;

import java.net.InetAddress;
import java.nio.ByteOrder;
import java.util.UUID;
import nxm.vrt.lib.VRTPacket.DataItemFormat;

import static java.nio.ByteOrder.BIG_ENDIAN;
import static java.lang.Boolean.FALSE;
import static java.lang.Boolean.TRUE;
import static java.util.Arrays.fill;

/** Supports conversions to/from the various numeric representations used within
 *  a VITA 49 packet. <br>
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
 *    Convert to/from double: FixedPointMath.toDouble32(20, bits)  -> val
 *                            FixedPointMath.fromDouble32(20, val) -> bits
 *  </pre>
 *  In order to allow for fast implementations that omit special/error handling, the following
 *  situations may produce unspecified results (or exceptions):
 *  <ul>
 *   <li>Using an invalid radix point.</li>
 *   <li>Setting the fixed-point value using the floating-point +Inf, -Inf, NaN</li>
 *   <li>Setting the fixed-point value using a floating-point or integer value that is
 *       outside the numeric limits of the fixed point number.
 *  </ul>
 *  This class is intended to be used in cases where fixed point is the storage/transmission
 *  format. Hence there are no fixed-point math methods here with it, only conversions to common
 *  integer and floating-point formats that can be used when performing computations. <br>
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
 *
 *  @author 
 */
public final class VRTMath {
  private static final byte UTF8_1BYTE_M = (byte)0x80; // Mask value for UTF8_1BYTE_0 check
  private static final byte UTF8_1BYTE_0 = (byte)0x00; // 0xxxxxxx
  private static final byte UTF8_2BYTE_M = (byte)0xE0; // Mask value for UTF8_2BYTE_0 check
  private static final byte UTF8_2BYTE_0 = (byte)0xC0; // 110xxxxx
  private static final byte UTF8_2BYTE_1 = (byte)0x80; // 10xxxxxx
  private static final byte UTF8_3BYTE_M = (byte)0xF0; // Mask value for UTF8_3BYTE_0 check
  private static final byte UTF8_3BYTE_0 = (byte)0xE0; // 1110xxxx
  private static final byte UTF8_3BYTE_1 = (byte)0x80; // 10xxxxxx
  private static final byte UTF8_3BYTE_2 = (byte)0x80; // 10xxxxxx
  private static final byte UTF8_4BYTE_M = (byte)0xF8; // Mask value for UTF8_4BYTE_0 check
  private static final byte UTF8_4BYTE_0 = (byte)0xF0; // 11110xxx
  private static final byte UTF8_4BYTE_1 = (byte)0x80; // 10xxxxxx
  private static final byte UTF8_4BYTE_2 = (byte)0x80; // 10xxxxxx
  private static final byte UTF8_4BYTE_3 = (byte)0x80; // 10xxxxxx

  /** The mask used when using an IPv4 mapped address in IPv6. */
  private static final byte[] IPV4_MAPPED = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, 0, 0, 0 };
  
  private VRTMath () { } // prevent instantiation
  
  //////////////////////////////////////////////////////////////////////////////////////////////////
  // IEEE 754-2008
  //////////////////////////////////////////////////////////////////////////////////////////////////
  private static final int[]   FLOAT16_BASE_TABLE  = new int[512];
  private static final int[]   FLOAT16_SHIFT_TABLE = new int[512];
  private static final byte[]  FLOAT16_OVER_TABLE  = new byte[512];
  private static final int[]   FLOAT16_MAN_TABLE   = new int[2048];
  private static final int[]   FLOAT16_EXP_TABLE   = new int[64];
  private static final short[] FLOAT16_OFF_TABLE   = new short[64];
  
  static {
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
    for (int i = 0; i < 256; i++) {
      int e  = i - 127;
      int ii = i | 0x100;
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
    for (int i = 1; i <= 1023; i++) {
      FLOAT16_MAN_TABLE[i] = convertmantissa(i);
    }
    for (int i = 1024; i <= 2047; i++) {
      FLOAT16_MAN_TABLE[i] = 0x38000000 + ((i-1024)<<13);
    }

    FLOAT16_EXP_TABLE[0] = 0;
    for (int i = 1; i <= 30; i++) {
      FLOAT16_EXP_TABLE[i] = i << 23;
    }
    FLOAT16_EXP_TABLE[31]= 0x47800000;
    FLOAT16_EXP_TABLE[32]= 0x80000000;
    for (int i = 33; i <= 62; i++) {
      FLOAT16_EXP_TABLE[i] = 0x80000000 + ((i-32)<<23);
    }
    FLOAT16_EXP_TABLE[63]= 0xC7800000;

    FLOAT16_OFF_TABLE[0] = 0;
    for (int i = 1; i <= 31; i++) {
      FLOAT16_OFF_TABLE[i] = 1024;
    }
    FLOAT16_OFF_TABLE[32] = 0;
    for (int i = 33; i <= 63; i++) {
      FLOAT16_OFF_TABLE[i] = 1024;
    }
  }
  
  /** Only used in the above static initializer. */
  private static int convertmantissa (int i) {
    int m = i<<13;                // Zero pad mantissa bits
    int e = 0;                    // Zero exponent
    while ((m&0x00800000) == 0) { // While not normalized
      e = e - 0x00800000;         // Decrement exponent (1<<23)
      m = m << 1;                 // Shift mantissa
    }
    m = m & ~0x00800000;          // Clear leading 1 bit
    e = e + 0x38800000;           // Adjust bias ((127-14)<<23)
    return m | e;                 // Return combined number
  }

  /** Converts an IEEE 754 single-precision floating-point value to the bits
   *  equating to an IEEE 754-2008 half-precision (storage only) value.
   *  <pre>
   *    References:
   *      [1] IEEE 754-2008.
   *      [2] Jeroen van der Zijp. "Fast Half Float Conversions." November
   *          2008 (Revised September 2010).
   *          ftp://ftp.fox-toolkit.org/pub/fasthalffloatconversion.pdf
   *  </pre>
   *  @param val The single-precision value.
   *  @return The bits representing the half-precision value.
   */
  public static short toHalf (float val) {
    // The algorithm used here is based on the code provided in [2], with
    // modifications to address the problem in the algorithm provided in [2]
    // where some NaN values mapped to Inf.
    int floatBits = Float.floatToRawIntBits(val);
    int index     = (floatBits>>23) & 0x1FF;
    int halfBits  = FLOAT16_BASE_TABLE[index]
                  | ((floatBits&0x007FFFFF) >> FLOAT16_SHIFT_TABLE[index])
                  | (((floatBits >> 6) | floatBits) & FLOAT16_OVER_TABLE[index]);
    return (short)halfBits;
  }

  /** Converts an IEEE 754-2008 half-precision floating-point (storage only)
   *  bit representation to an IEEE 754 single-precision floating-point value.
   *  <pre>
   *    References:
   *      [1] IEEE 754-2008.
   *      [2] Jeroen van der Zijp. "Fast Half Float Conversions." November
   *          2008 (Revised September 2010).
   *          ftp://ftp.fox-toolkit.org/pub/fasthalffloatconversion.pdf
   *  </pre>
   *  @param bits The bits representing the half-precision value.
   *  @return The single-precision value.
   */
  public static float fromHalf (short bits) {
    // The algorithm used here is based on the code provided in [2], with
    // modifications to address the problem in the algorithm provided in [2]
    // where some NaN values mapped to Inf.
    int halfBits  = bits & 0xFFFF;
    int index     = halfBits >> 10;
    int floatBits = FLOAT16_MAN_TABLE[FLOAT16_OFF_TABLE[index]+(halfBits&0x3FF)]
                  + FLOAT16_EXP_TABLE[index];
    return Float.intBitsToFloat(floatBits);
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // 64-BIT
  //////////////////////////////////////////////////////////////////////////////////////////////////
  /** Converts a 64-bit fixed point number to a long.
   *  @param radixPoint The radix point of the number (1..63).
   *  @param bits       The bits representing the fixed-point number. 
   *  @return The integer value of the fixed point number.
   */
  public static long toLong64 (int radixPoint, long bits) {
    return (bits < 0)? -(-bits >> radixPoint) : (bits >> radixPoint);
  }
  
  /** Converts a long to a 64-bit fixed point number.
   *  @param radixPoint The radix point of the number (1..63).
   *  @param val        The integer value.
   *  @return The bits representing the fixed-point number.
   */
  public static long fromLong64 (int radixPoint, long val) {
    return (val << radixPoint);
  }
  
  /** Converts a 64-bit fixed point number to an int.
   *  @param radixPoint The radix point of the number (1..63).
   *  @param bits       The bits representing the fixed-point number. 
   *  @return The integer value of the fixed point number.
   */
  public static int toInt64 (int radixPoint, long bits) {
    return (int)toLong64(radixPoint, bits);
  }
  
  /** Converts an int to a 64-bit fixed point number.
   *  @param radixPoint The radix point of the number (1..63).
   *  @param val        The integer value.
   *  @return The bits representing the fixed-point number.
   */
  public static long fromInt64 (int radixPoint, int val) {
    return (((long)val) << radixPoint);
  }
  
  /** Converts a 64-bit fixed point number to a float.
   *  @param radixPoint The radix point of the number (1..63).
   *  @param bits       The bits representing the fixed-point number. 
   *  @return The floating-point value of the fixed point number.
   */
  public static float toFloat64 (int radixPoint, long bits) {
    return (float)toDouble64(radixPoint, bits);
  }
  
  /** Converts a float to a 64-bit fixed point number.
   *  @param radixPoint The radix point of the number (1..63).
   *  @param val        The floating-point value.
   *  @return The bits representing the fixed-point number.
   */
  public static long fromFloat64 (int radixPoint, float val) {
    return fromDouble64(radixPoint, val);
  }
  
  /** Converts a 64-bit fixed point number to a float.
   *  @param radixPoint The radix point of the number (1..63).
   *  @param bits       The bits representing the fixed-point number. 
   *  @return The floating-point value of the fixed point number.
   */
  public static double toDouble64 (int radixPoint, long bits) {
    double divisor = (double)(0x1L << radixPoint);
    return bits / divisor;
  }
  
  /** Converts a float to a 64-bit fixed point number.
   *  @param radixPoint The radix point of the number (1..63).
   *  @param val        The floating-point value.
   *  @return The bits representing the fixed-point number.
   */
  public static long fromDouble64 (int radixPoint, double val) {
    double divisor = (double)(0x1L << radixPoint);
    return (long)Math.rint(val * divisor);
  }
  
  //////////////////////////////////////////////////////////////////////////////////////////////////
  // 32-BIT
  //////////////////////////////////////////////////////////////////////////////////////////////////
  /** Converts a 32-bit fixed point number to a long.
   *  @param radixPoint The radix point of the number (1..63).
   *  @param bits       The bits representing the fixed-point number. 
   *  @return The integer value of the fixed point number.
   */
  public static long toLong32 (int radixPoint, int bits) {
    return toInt32(radixPoint, bits);
  }
  
  /** Converts a long to a 32-bit fixed point number.
   *  @param radixPoint The radix point of the number (1..63).
   *  @param val        The integer value.
   *  @return The bits representing the fixed-point number.
   */
  public static int fromLong32 (int radixPoint, long val) {
    return (int)(val << radixPoint);
  }
  
  /** Converts a 32-bit fixed point number to an int.
   *  @param radixPoint The radix point of the number (1..63).
   *  @param bits       The bits representing the fixed-point number. 
   *  @return The integer value of the fixed point number.
   */
  public static int toInt32 (int radixPoint, int bits) {
    return (bits < 0)? -(-bits >> radixPoint) : (bits >> radixPoint);
  }
  
  /** Converts an int to a 32-bit fixed point number.
   *  @param radixPoint The radix point of the number (1..63).
   *  @param val        The integer value.
   *  @return The bits representing the fixed-point number.
   */
  public static int fromInt32 (int radixPoint, int val) {
    return (val << radixPoint);
  }
  
  /** Converts a 32-bit fixed point number to a float.
   *  @param radixPoint The radix point of the number (1..63).
   *  @param bits       The bits representing the fixed-point number. 
   *  @return The floating-point value of the fixed point number.
   */
  public static float toFloat32 (int radixPoint, int bits) {
    return (float)toDouble32(radixPoint, bits);
  }
  
  /** Converts a float to a 32-bit fixed point number.
   *  @param radixPoint The radix point of the number (1..63).
   *  @param val        The floating-point value.
   *  @return The bits representing the fixed-point number.
   */
  public static int fromFloat32 (int radixPoint, float val) {
    return fromDouble32(radixPoint, val);
  }
  
  /** Converts a 32-bit fixed point number to a float.
   *  @param radixPoint The radix point of the number (1..63).
   *  @param bits       The bits representing the fixed-point number. 
   *  @return The floating-point value of the fixed point number.
   */
  public static double toDouble32 (int radixPoint, int bits) {
    double divisor = (double)(0x1L << radixPoint);
    return bits / divisor;
  }
  
  /** Converts a float to a 32-bit fixed point number.
   *  @param radixPoint The radix point of the number (1..63).
   *  @param val        The floating-point value.
   *  @return The bits representing the fixed-point number.
   */
  public static int fromDouble32 (int radixPoint, double val) {
    double divisor = (double)(0x1L << radixPoint);
    return (int)Math.rint(val * divisor);
  }
  
  //////////////////////////////////////////////////////////////////////////////////////////////////
  // 16-BIT
  //////////////////////////////////////////////////////////////////////////////////////////////////
  /** Converts a 16-bit fixed point number to a long.
   *  @param radixPoint The radix point of the number (1..63).
   *  @param bits       The bits representing the fixed-point number. 
   *  @return The integer value of the fixed point number.
   */
  public static long toLong16 (int radixPoint, short bits) {
    return toInt16(radixPoint, bits);
  }
  
  /** Converts a long to a 16-bit fixed point number.
   *  @param radixPoint The radix point of the number (1..63).
   *  @param val        The integer value.
   *  @return The bits representing the fixed-point number.
   */
  public static short fromLong16 (int radixPoint, long val) {
    return (short)(val << radixPoint);
  }
  
  /** Converts a 16-bit fixed point number to an int.
   *  @param radixPoint The radix point of the number (1..63).
   *  @param bits       The bits representing the fixed-point number. 
   *  @return The integer value of the fixed point number.
   */
  public static int toInt16 (int radixPoint, short bits) {
    return (bits < 0)? -(-bits >> radixPoint) : (bits >> radixPoint);
  }
  
  /** Converts an int to a 16-bit fixed point number.
   *  @param radixPoint The radix point of the number (1..63).
   *  @param val        The integer value.
   *  @return The bits representing the fixed-point number.
   */
  public static short fromInt16 (int radixPoint, int val) {
    return (short)(val << radixPoint);
  }
  
  /** Converts a 16-bit fixed point number to a float.
   *  @param radixPoint The radix point of the number (1..63).
   *  @param bits       The bits representing the fixed-point number. 
   *  @return The floating-point value of the fixed point number.
   */
  public static float toFloat16 (int radixPoint, short bits) {
    return (float)toDouble16(radixPoint, bits);
  }
  
  /** Converts a float to a 16-bit fixed point number.
   *  @param radixPoint The radix point of the number (1..63).
   *  @param val        The floating-point value.
   *  @return The bits representing the fixed-point number.
   */
  public static short fromFloat16 (int radixPoint, float val) {
    return fromDouble16(radixPoint, val);
  }
  
  /** Converts a 16-bit fixed point number to a float.
   *  @param radixPoint The radix point of the number (1..63).
   *  @param bits       The bits representing the fixed-point number. 
   *  @return The floating-point value of the fixed point number.
   */
  public static double toDouble16 (int radixPoint, short bits) {
    double divisor = (double)(0x1L << radixPoint);
    return bits / divisor;
  }
  
  /** Converts a float to a 32-bit fixed point number.
   *  @param radixPoint The radix point of the number (1..63).
   *  @param val        The floating-point value.
   *  @return The bits representing the fixed-point number.
   */
  public static short fromDouble16 (int radixPoint, double val) {
    double divisor = (double)(0x1L << radixPoint);
    return (short)Math.rint(val * divisor);
  }
  
  //////////////////////////////////////////////////////////////////////////////////////////////////
  // TO/FROM VRT FLOAT TYPES
  //////////////////////////////////////////////////////////////////////////////////////////////////
  /** Converts from an IEEE-754 double-precision floating-point value to the
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
   *  will be converted to zero.
   *  <pre>
   *    References:
   *      [1] ANSI/VITA 49.0, Appendix D
   *      [2] IEEE-754
   *  </pre>
   * @param form  The data item format. This must be one of the VRT
   *              floating-point formats.
   * @param dSize The data size in bits (2..63).
   * @param val   The value to convert.
   * @return The packed bits for the VRT floating-point number.
   */
  public static long toVRTFloat (DataItemFormat form, int dSize, double val) {
    boolean  sign   = form.isSigned();        // Is output signed?
    int      eSize  = form.getExponentBits(); // Number of output exponent bits (1 to 6)
    long     maxExp = (0x1 << eSize) - 1;     // Max output exponent value
    long     mSize  = dSize - eSize;          // Output mantissa size
    
    // ==== SANITY CHECKS ======================================================
    if (eSize < 1) {
      throw new IllegalArgumentException("Expected VRT floating-point format but given "+form);
    }
    if ((mSize < 1) || (dSize > 64)) {
      throw new IllegalArgumentException("Expected VRT data size in range of ["+(eSize+1)+",64] "
                                       + "for use with "+form+" but given "+dSize);
    }
    if (Double.isNaN(val)) return 0;
    if (!sign && (val < 0)) return 0;
    if (val ==  0) return 0; // <-- Fast handling for common case
    if (val >= +1) return (sign)? ~(-1L << (dSize-1)) : ~(-1L << dSize);
    if (val <= -1) return (0x1L << (dSize-1)) | maxExp;
    
    // ==== CONVERT TO VRT FLOAT ===============================================
    long bits      = Double.doubleToRawLongBits(val); // Input IEEE-754 bits
    long expo      = ((bits >> 52) & 0x7FF) - 1023;   // Input exponent
    long mant      = (bits & 0x000FFFFFFFFFFFFFL);    // Input mantissa
    long exponent;                                    // Output exponent
    long mantissa;                                    // Output mantissa
    
    if (expo != 0) {
      // If not denormal, add back the top bit
      mant = mant | 0x0010000000000000L;
    }

    if (sign) {
      mantissa = (mant >> (54 - mSize));   // 54 not 53 so top bit is effectively "sign bit"
      exponent = maxExp + expo + 1;  // +2 not +1 to match mantissa 54 not 53
    }
    else {
      mantissa = (mant >> (53 - mSize));
      exponent = maxExp + expo + 1;
    }
    
    if (exponent < 0) {
      // Output is (effectively) a denormal
      mantissa = mantissa >> -exponent;
      exponent = 0;
    }
    
    if (val < 0) { 
      mantissa = -mantissa;
    }

    return (((mantissa << eSize) | exponent) & ~(-1L << dSize));
  }
  
  /** Converts from the packed bits for a VRT floating-point to an IEEE-754
   *  double-precision floating-point value. <br>
   *  <br>
   *  Despite the fact that {@link #toVRTFloat} normalizes its output values,
   *  this function will support all of the valid VRT forms and convert them to
   *  their respective IEEE-754 value. Since IEEE-754 values are normalized this
   *  process results not reflective.
   *  <pre>
   *    References:
   *      [1] ANSI/VITA 49.0, Appendix D
   *      [2] IEEE-754
   *  </pre>
   *  @param form  The data item format. This must be one of the VRT
   *               floating-point formats.
   *  @param dSize The data size in bits (2..63).
   *  @param bits  The packed bits to convert.
   *  @return The IEEE-754 floating-point number.
   */
  public static double fromVRTFloat (DataItemFormat form, int dSize, long bits) {
    boolean sign   = form.isSigned();
    int     eSize  = form.getExponentBits();
    int     maxExp = (0x1 << eSize) - 1;     // = all exp bits set
    int     exp    = ((int)bits) & maxExp;
    int     mSize  = dSize - eSize;
    
    // ==== SANITY CHECKS ======================================================
    if (eSize < 1) {
      throw new IllegalArgumentException("Expected VRT floating-point format but given "+form);
    }
    if ((mSize < 1) || (dSize > 64)) {
      throw new IllegalArgumentException("Expected VRT data size in range of ["+(eSize+1)+",64] "
                                       + "for use with "+form+" but given "+dSize);
    }
    
    // ==== CONVERT TO IEEE-754 FLOAT ==========================================
    if (bits == 0) return 0.0; // <-- Fast handling for common case
    
    if (sign) {
      // Previously we used 'pow(2, mSize + maxExp - exp - 1)' for the divisor,
      // but since we are using powers of 2 below 2^64 we can do bit-shifts in
      // place of the cal to pow(2,N) if we break them up.
      long   man = (bits << (64 - dSize)) >> (64 - mSize); // sign extend and shift
      double a   = (0x01L << (mSize  - 1  )); // -1 for effective "sign bit"
      double b   = (0x01L << (maxExp - exp));
      if (b < 0) b = 18446744073709551616.0; // special case of maxExp=63 and exp=0
      return man / a / b;
    }
    else {
      long man = (bits >> eSize) & ((0x1L << mSize) - 1); // mask off upper bits
      double a   = (0x01L << (mSize       ));
      double b   = (0x01L << (maxExp - exp));
      if (b < 0) b = 18446744073709551616.0; // special case of maxExp=63 and exp=0
      return man / a / b;
    }
  }
  
  
  //////////////////////////////////////////////////////////////////////////////////////////////////
  // UNPACK METHODS
  //////////////////////////////////////////////////////////////////////////////////////////////////

  /** Unpack byte data from a byte buffer.
   *  @param buf byte array of data
   *  @param off Offset into array
   *  @return short value
   */
  public static byte unpackByte (byte[] buf, int off) {
    return buf[off];
  }

  /** Unpack short data from a byte buffer.
   *  @param buf byte array of data
   *  @param off Offset into array
   *  @param rep The byte representation to use
   *  @return short value
   */
  public static short unpackShort (byte[] buf, int off, ByteOrder rep) {
    return (short)((rep==BIG_ENDIAN)? (buf[off+1]&0xFF) | ((buf[off  ]&0xFF)<<8): 
                                      (buf[off  ]&0xFF) | ((buf[off+1]&0xFF)<<8));
  }

  /** Unpack short data from a byte buffer in {@link ByteOrder#BIG_ENDIAN}.
   *  @param buf byte array of data
   *  @param off Offset into array
   *  @return short value
   */
  public static short unpackShort (byte[] buf, int off) {
    return (short)((buf[off+1]&0xFF) | ((buf[off  ]&0xFF)<<8));
  }

  /** <b>Internal Use Only:</b> Unpack a 24-bit integer value as an int data
   *  from a byte buffer. The returned value will be sign-extended.
   *  @param buf byte array of data
   *  @param off Offset into array
   *  @param rep The byte representation to use
   *  @return int value
   */
  public static int unpackInt24 (byte[] buf, int off, ByteOrder rep) {
    int val = (rep==BIG_ENDIAN)?
                (buf[off+2]&0xFF) | ((buf[off+1]&0xFF)<<8) | ((buf[off  ]&0xFF)<<16):
                (buf[off  ]&0xFF) | ((buf[off+1]&0xFF)<<8) | ((buf[off+2]&0xFF)<<16);
    return ((val << 8) >> 8); // sign-extend
  }

  /** <b>Internal Use Only:</b> Unpack a 24-bit integer value as an int data
   *  from a byte buffer in {@link ByteOrder#BIG_ENDIAN}. The returned value will be
   *  sign-extended.
   *  @param buf byte array of data
   *  @param off Offset into array
   *  @return int value
   */
  public static int unpackInt24 (byte[] buf, int off) {
    int val = (buf[off+2]&0xFF) | ((buf[off+1]&0xFF)<<8) | ((buf[off  ]&0xFF)<<16);
    return ((val << 8) >> 8); // sign-extend
  }

  /** Unpack int data from a byte buffer.
   *  @param buf byte array of data
   *  @param off Offset into array
   *  @param rep The byte representation to use
   *  @return int value
   */
  public static int unpackInt (byte[] buf, int off, ByteOrder rep) {
    return (rep==BIG_ENDIAN)?
             (buf[off+3]&0xFF) | ((buf[off+2]&0xFF)<<8) | ((buf[off+1]&0xFF)<<16) | ((buf[off  ])<<24):
             (buf[off  ]&0xFF) | ((buf[off+1]&0xFF)<<8) | ((buf[off+2]&0xFF)<<16) | ((buf[off+3])<<24);
  }

  /** Unpack int data from a byte buffer in {@link ByteOrder#BIG_ENDIAN}.
   *  @param buf byte array of data
   *  @param off Offset into array
   *  @return int value
   */
  public static int unpackInt (byte[] buf, int off) {
    return (buf[off+3]&0xFF) | ((buf[off+2]&0xFF)<<8) | ((buf[off+1]&0xFF)<<16) | ((buf[off  ])<<24);
  }

  /** Unpack long data from a byte buffer.
   *  @param buf byte array of data
   *  @param off Offset into array
   *  @param rep The byte representation to use
   *  @return long value
   */
  public static long unpackLong (byte[] buf, int off, ByteOrder rep) {
    return (rep==BIG_ENDIAN)?
             ((buf[off  ]&0xFFL)<<56) | ((buf[off+1]&0xFFL)<<48) | ((buf[off+2]&0xFFL)<<40) | ((buf[off+3]&0xFFL)<<32)
           | ((buf[off+4]&0xFFL)<<24) | ((buf[off+5]&0xFFL)<<16) | ((buf[off+6]&0xFFL)<< 8) | ((buf[off+7]&0xFFL)    ):
             ((buf[off  ]&0xFFL)    ) | ((buf[off+1]&0xFFL)<< 8) | ((buf[off+2]&0xFFL)<<16) | ((buf[off+3]&0xFFL)<<24)
           | ((buf[off+4]&0xFFL)<<32) | ((buf[off+5]&0xFFL)<<40) | ((buf[off+6]&0xFFL)<<48) | ((buf[off+7]&0xFFL)<<56);
  }

  /** Unpack long data from a byte buffer in {@link ByteOrder#BIG_ENDIAN}.
   *  @param buf byte array of data
   *  @param off Offset into array
   *  @return long value
   */
  public static long unpackLong (byte[] buf, int off) {
    return ((buf[off  ]&0xFFL)<<56) | ((buf[off+1]&0xFFL)<<48) | ((buf[off+2]&0xFFL)<<40) | ((buf[off+3]&0xFFL)<<32)
         | ((buf[off+4]&0xFFL)<<24) | ((buf[off+5]&0xFFL)<<16) | ((buf[off+6]&0xFFL)<< 8) | ((buf[off+7]&0xFFL)    );
  }

  /** Unpack float data from a byte buffer.
   *  @param buf byte array of data
   *  @param off Offset into array
   *  @param rep The byte representation to use
   *  @return float value
   */
  public static float unpackFloat (byte[] buf, int off, ByteOrder rep) {
    return Float.intBitsToFloat(unpackInt(buf,off,rep));
  }

  /** Unpack float data from a byte buffer in {@link ByteOrder#BIG_ENDIAN}.
   *  @param buf byte array of data
   *  @param off Offset into array
   *  @return float value
   */
  public static float unpackFloat (byte[] buf, int off) {
    return Float.intBitsToFloat(unpackInt(buf,off));
  }

  /** Unpack double data from a byte buffer.
   *  @param buf byte array of data
   *  @param off Offset into array
   *  @param rep The byte representation to use
   *  @return double value
   */
  public static double unpackDouble (byte[] buf, int off, ByteOrder rep) {
    return Double.longBitsToDouble(unpackLong(buf,off,rep));
  }

  /** Unpack double data from a byte buffer in {@link ByteOrder#BIG_ENDIAN}.
   *  @param buf byte array of data
   *  @param off Offset into array
   *  @return double value
   */
  public static double unpackDouble (byte[] buf, int off) {
    return Double.longBitsToDouble(unpackLong(buf,off));
  }

  /** Unpack a boolean from a buffer.
   *  @param buf byte array of data
   *  @param off Offset into array
   *  @return The unpacked value
   *  @throws RuntimeException If {@link VRTConfig#STRICT} is <tt>true</tt> and the stored value is
   *          something other than <tt>-1</tt>, <tt>0</tt> or <tt>1</tt>.
   */
  public static Boolean unpackBoolNull (byte[] buf, int off) {
    byte b = buf[off];
    if (VRTConfig.STRICT) {
      switch (b) {
        case -1: return FALSE;
        case  0: return null;
        case +1: return TRUE;
        default: throw new RuntimeException("Illegal value ("+b+") when expecting boolean (0=null, -1=false, +1=true)");
      }
    }
    else {
      return (b == 0)? null : (b > 0);
    }
  }

  /** Unpack a boolean from a buffer.
   *  @param buf byte array of data
   *  @param off Offset into array
   *  @return The unpacked value
   *  @throws RuntimeException If {@link VRTConfig#STRICT} is <tt>true</tt> and the stored value is
   *          something other than <tt>-1</tt>, <tt>0</tt> or <tt>1</tt>.
   */
  public static boolean unpackBoolean (byte[] buf, int off) {
    byte b = buf[off];
    if (VRTConfig.STRICT) {
      switch (b) {
        case -1: return false;
        case  0: return false;
        case +1: return true;
        default: throw new RuntimeException("Illegal value ("+b+") when expecting boolean (0=false, -1=false, +1=true)");
      }
    }
    else {
      return (b > 0);
    }
  }

  /** Unpack a ASCII string from a buffer. The string is assumed to be null-terminated in the event
   *  that the string is shorter than the allowable length.
   *  @param buf byte array of data
   *  @param off Offset into array
   *  @param len length of the string in the buffer
   *  @return The unpacked value
   */
  public static AsciiCharSequence unpackAscii (byte[] buf, int off, int len) {
    return new AsciiCharSequence(buf, off, len, true);
  }

  /** Unpack a modified UTF-8 string from a buffer. The string is assumed to be null-terminated
   *  in the event that the string is shorter than the allowable length. This is identical to using
   *  <tt>unpackUTF8(buf,off,len,'?')</tt>.
   *  @param buf         Byte array of data
   *  @param off         Offset into array
   *  @param len         Length of the string in the buffer
   *  @return The unpacked value.
   */
  public static CharSequence unpackUTF8 (byte[] buf, int off, int len) {
    return unpackUTF8(buf,off,len,'?');
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
   *  @param buf         Byte array of data
   *  @param off         Offset into array
   *  @param len         Length of the string in the buffer
   *  @param replacement The replacement character to use whenever a non UTF-8 character is
   *                     encountered (typically '?'), if null an exception is thrown rather than
   *                     replacing erroneous characters.
   *  @return The unpacked value.
   *  @throws IllegalArgumentException If the string found is not proper UTF-8 and replacement is
   *                                   null.
   */
  public static CharSequence unpackUTF8 (byte[] buf, int off, int len, Character replacement) {
    StringBuilder str = new StringBuilder();

    int end = off + len;
    while (off < end) {
      byte b = buf[off++];
      int  c;
      
      if (b == 0) {                                                      // End-of-string NUL
        return str;
      }
      else if ((b & UTF8_1BYTE_M) == UTF8_1BYTE_0) {                     // ASCII (except embedded NUL)
        c = (b & 0x7F);
      }
      else if (((b & UTF8_2BYTE_M) == UTF8_2BYTE_0) && (off+1 < len)) {  // 0x0080 to 0x07FF range (or embedded NUL)
        c = ((b & 0x1F) << 6)
          | (buf[off++] & 0x3F);
      }
      else if (((b & UTF8_3BYTE_M) == UTF8_3BYTE_0) && (off+2 < len)) {  // 0x0800 to 0xFFFF range
        c = ((b & 0x0F) << 12)
          | ((buf[off++] & 0x3F) << 6)
          |  (buf[off++] & 0x3F);
      }
      else if (((b & UTF8_4BYTE_M) == UTF8_4BYTE_0) && (off+2 < len)) {  // 0x010000 to 0x10FFFF range
        c = ((b & 0x0F) << 18)
          | ((buf[off++] & 0x3F) << 6)
          | ((buf[off++] & 0x3F) << 6)
          |  (buf[off++] & 0x3F);
      }
      else if (replacement != null) {
        c = replacement;
      }
      else {
        throw new IllegalArgumentException("Invalid UTF-8 character found after '"+str+"'.");
      }
      str.appendCodePoint(c);
    }
    return str;
  }

  /** Pack a block of metadata into the buffer.
   *  @param buf    byte array of data
   *  @param off    Offset into array
   *  @param len    Length of the array in the buffer
   *  @throws IllegalArgumentException If the metadata block is invalid.
   */
  public static MetadataBlock unpackMetadata (byte[] buf, int off, int len) {
    MetadataBlock meta = new MetadataBlock();
    meta.readBytes(buf, off, len);
    return meta;
  }

  /** Unpack an array of bytes from a buffer.
   *  @param buf byte array of data
   *  @param off Offset into array
   *  @param len length of the array in the buffer
   *  @return The unpacked value
   */
  public static byte[] unpackBytes (byte[] buf, int off, int len) {
    return unpackBytes(buf, off, new byte[len], 0, buf.length);
  }

  /** Unpack an array of bytes from a buffer.
   *  @param buf    byte array of data [INPUT]
   *  @param off    Offset into array
   *  @param buffer the output buffer [OUTPUT]
   *  @param boff   the output buffer offset
   *  @param len    length of the array in the buffer
   *  @return <tt>buffer</tt>
   */
  public static byte[] unpackBytes (byte[] buf, int off, byte[] buffer, int boff, int len) {
    System.arraycopy(buf, off, buffer, boff, len);
    return buffer;
  }

  /** Unpack an VITA 49.0 time stamp from a buffer.
   *  @param buf   byte array of data
   *  @param off   Offset into array
   *  @param epoch The epoch to use (must be either {@link TimeStamp.IntegerMode#UTC} or
   *               {@link TimeStamp.IntegerMode#GPS}).
   *  @param rep   The byte representation to use
   *  @return The unpacked value
   */
  public static TimeStamp unpackTimeStamp (byte[] buf, int off, TimeStamp.IntegerMode epoch, ByteOrder rep) {
    int  sec = unpackInt(buf, off, rep);
    long ps  = unpackLong(buf, off+4, rep);
    return new TimeStamp(epoch, sec, ps);
  }

  /** Unpack an VITA 49.0 time stamp from a buffer in {@link ByteOrder#BIG_ENDIAN}.
   *  @param buf   byte array of data
   *  @param off   Offset into array
   *  @param epoch The epoch to use (must be either {@link TimeStamp.IntegerMode#UTC} or
   *               {@link TimeStamp.IntegerMode#GPS}).
   *  @return The unpacked value
   */
  public static TimeStamp unpackTimeStamp (byte[] buf, int off, TimeStamp.IntegerMode epoch) {
    int  sec = unpackInt(buf, off);
    long ps  = unpackLong(buf, off+4);
    return new TimeStamp(epoch, sec, ps);
  }

  /** Unpack 16-byte IPv6 address from a buffer.
   *  @param buf   byte array of data
   *  @param off   Offset into array
   *  @return The unpacked value
   */
  public static InetAddress unpackInetAddr (byte[] buf, int off) {
    try {
      byte[] addr = new byte[16];
      System.arraycopy(buf, off, addr, 0, 16);
      return InetAddress.getByAddress(addr);
    }
    catch (java.net.UnknownHostException e) {
      // This should be impossible since we declare a 16-byte array above
      throw new AssertionError("Invalid IPv6 address: "+e);
    }
  }

  /** Unpack 16-byte UUID address from a buffer.
   *  @param buf   byte array of data
   *  @param off   Offset into array
   *  @return The unpacked value
   */
  public static UUID unpackUUID (byte[] buf, int off) {
    long msb = unpackLong(buf,  off);
    long lsb = unpackLong(buf, off+8);
    return new UUID(msb,lsb);
  }

  /** Unpack a record from a buffer.
   *  @param buf   byte array of data
   *  @param off   Offset into array
   *  @param rec   The record to unpack to [OUTPUT]
   *  @return <tt>rec</tt>
   */
  public static <T extends Record> T unpackRecord (byte[] buf, int off, T rec) {
    rec.writeBytes(buf, off);
    return rec;
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // PACK METHODS
  //////////////////////////////////////////////////////////////////////////////////////////////////

  /** Pack byte data into byte buffer.
   *  @param buf Packed byte array [OUTPUT]
   *  @param off Offset into output array
   *  @param val value to pack [INPUT]
   */
  public static void packByte (byte[] buf, int off, byte val) {
    buf[off] = val;
  }

  /** Pack short data into byte buffer.
   *  @param buf Packed byte array [OUTPUT]
   *  @param off Offset into output array
   *  @param val value to pack [INPUT]
   *  @param rep The byte representation to use
   */
  public static void packShort (byte[] buf, int off, short val, ByteOrder rep) {
    if (rep==BIG_ENDIAN) {
      buf[off  ] = (byte)(val>>8);
      buf[off+1] = (byte)(val   );
    } else {
      buf[off  ] = (byte)(val   );
      buf[off+1] = (byte)(val>>8);
    }
  }

  /** Pack short data into byte buffer in {@link ByteOrder#BIG_ENDIAN}.
   *  @param buf Packed byte array [OUTPUT]
   *  @param off Offset into output array
   *  @param val value to pack [INPUT]
   */
  public static void packShort (byte[] buf, int off, short val) {
    buf[off  ] = (byte)(val>>8);
    buf[off+1] = (byte)(val   );
  }

  /** <b>Internal Use Only:</b> Pack a 24-bit integer value as an int data from
   *  a byte buffer. Any high-order bits (i.e. bits 24 to 31) are ignored.
   *  @param buf Packed byte array [OUTPUT]
   *  @param off Offset into output array
   *  @param val value to pack [INPUT]
   *  @param rep The byte representation to use
   */
  public static void packInt24 (byte[] buf, int off, int val, ByteOrder rep) {
    if (rep==BIG_ENDIAN) {
      buf[off  ] = (byte)(val>>16);
      buf[off+1] = (byte)(val>> 8);
      buf[off+2] = (byte)(val    );
    } else {
      buf[off  ] = (byte)(val    );
      buf[off+1] = (byte)(val>> 8);
      buf[off+2] = (byte)(val>>16);
    }
  }

  /** <b>Internal Use Only:</b> Pack a 24-bit integer value as an int data from
   *  a byte buffer in {@link ByteOrder#BIG_ENDIAN}. Any high-order bits (i.e. bits 24
   *  to 31) are ignored.
   *  @param buf Packed byte array [OUTPUT]
   *  @param off Offset into output array
   *  @param val value to pack [INPUT]
   */
  public static void packInt24 (byte[] buf, int off, int val) {
    buf[off  ] = (byte)(val>>16);
    buf[off+1] = (byte)(val>> 8);
    buf[off+2] = (byte)(val    );
  }
  
  /** Pack int data into byte buffer.
   *  @param buf Packed byte array [OUTPUT]
   *  @param off Offset into output array
   *  @param val value to pack [INPUT]
   *  @param rep The byte representation to use
   */
  public static void packInt (byte[] buf, int off, int val, ByteOrder rep) {
    if (rep==BIG_ENDIAN) {
      buf[off  ] = (byte)(val>>24);
      buf[off+1] = (byte)(val>>16);
      buf[off+2] = (byte)(val>> 8);
      buf[off+3] = (byte)(val    );
    } else {
      buf[off+0] = (byte)(val    );
      buf[off+1] = (byte)(val>> 8);
      buf[off+2] = (byte)(val>>16);
      buf[off  ] = (byte)(val>>24);
    }
  }
  
  /** Pack int data into byte buffer in {@link ByteOrder#BIG_ENDIAN}.
   *  @param buf Packed byte array [OUTPUT]
   *  @param off Offset into output array
   *  @param val value to pack [INPUT]
   */
  public static void packInt (byte[] buf, int off, int val) {
    buf[off  ] = (byte)(val>>24);
    buf[off+1] = (byte)(val>>16);
    buf[off+2] = (byte)(val>> 8);
    buf[off+3] = (byte)(val    );
  }

  /** Pack long data into byte buffer.
   *  @param buf Packed byte array [OUTPUT]
   *  @param off Offset into output array
   *  @param val value to pack [INPUT]
   *  @param rep The byte representation to use
   */
  public static void packLong (byte[] buf, int off, long val, ByteOrder rep) {
    if (rep==BIG_ENDIAN) {
      buf[off  ] = (byte)(val>>56);
      buf[off+1] = (byte)(val>>48);
      buf[off+2] = (byte)(val>>40);
      buf[off+3] = (byte)(val>>32);
      buf[off+4] = (byte)(val>>24);
      buf[off+5] = (byte)(val>>16);
      buf[off+6] = (byte)(val>> 8);
      buf[off+7] = (byte)(val    );
    } else {
      buf[off  ] = (byte)(val    );
      buf[off+1] = (byte)(val>> 8);
      buf[off+2] = (byte)(val>>16);
      buf[off+3] = (byte)(val>>24);
      buf[off+4] = (byte)(val>>32);
      buf[off+5] = (byte)(val>>40);
      buf[off+6] = (byte)(val>>48);
      buf[off+7] = (byte)(val>>56);
    }
  }

  /** Pack long data into byte buffer in {@link ByteOrder#BIG_ENDIAN}.
   *  @param buf Packed byte array [OUTPUT]
   *  @param off Offset into output array
   *  @param val value to pack [INPUT]
   */
  public static void packLong (byte[] buf, int off, long val) {
    buf[off  ] = (byte)(val>>56);
    buf[off+1] = (byte)(val>>48);
    buf[off+2] = (byte)(val>>40);
    buf[off+3] = (byte)(val>>32);
    buf[off+4] = (byte)(val>>24);
    buf[off+5] = (byte)(val>>16);
    buf[off+6] = (byte)(val>> 8);
    buf[off+7] = (byte)(val    );
  }

  /** Pack float data into byte buffer.
   *  @param buf Packed byte array [OUTPUT]
   *  @param off Offset into output array
   *  @param val value to pack [INPUT]
   *  @param rep The byte representation to use
   */
  public static void packFloat (byte[] buf, int off, float val, ByteOrder rep) {
    packInt(buf,off, Float.floatToRawIntBits(val), rep);
  }

  /** Pack float data into byte buffer in {@link ByteOrder#BIG_ENDIAN}.
   *  @param buf Packed byte array [OUTPUT]
   *  @param off Offset into output array
   *  @param val value to pack [INPUT]
   */
  public static void packFloat (byte[] buf, int off, float val) {
    packInt(buf,off, Float.floatToRawIntBits(val));
  }

  /** Pack double data into byte buffer.
   *  @param buf Packed byte array [OUTPUT]
   *  @param off Offset into output array
   *  @param val value to pack [INPUT]
   *  @param rep The byte representation to use
   */
  public static void packDouble (byte[] buf, int off, double val, ByteOrder rep) {
    packLong(buf,off, Double.doubleToRawLongBits(val), rep);
  }

  /** Pack double data into byte buffer in {@link ByteOrder#BIG_ENDIAN}.
   *  @param buf Packed byte array [OUTPUT]
   *  @param off Offset into output array
   *  @param val value to pack [INPUT]
   */
  public static void packDouble (byte[] buf, int off, double val) {
    packLong(buf,off, Double.doubleToRawLongBits(val));
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
  public static void packBoolNull (byte[] buf, int off, Boolean val) {
         if (val == null) buf[off] = (byte) 0; // null
    else if (    val    ) buf[off] = (byte)+1; // true
    else                  buf[off] = (byte)-1; // false
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
  public static void packBoolean (byte[] buf, int off, boolean val) {
    if (val) buf[off] = (byte)+1; // true
    else     buf[off] = (byte)-1; // false
  }

  /** Pack a ASCII string into a buffer. Any non-ASCII characters or ASCII control characters will
   *  be replaces with '?' upon insertion. If <tt>val</tt> is longer than <tt>len</tt> it will be
   *  truncated without explicit warning. If <tt>val</tt> is shorter than <tt>len</tt> it will be
   *  padded out with ASCII NUL ('\0') characters to fill the length.
   *  @param buf    Byte array of data [OUTPUT]
   *  @param off    Offset into array
   *  @param val    Value to pack [INPUT]
   *  @param length Length of the string in the buffer.
   *  @return The actual length required to hold the entire string. (This is for compatibility with
   *          <tt>packUTF8(..)</tt>, but in practice just returns <tt>val.length()</tt>.)
   */
  public static int packAscii (byte[] buf, int off, CharSequence val, int length) {
    int end = off+length;
    for (int i = 0; (i < val.length()) && (off < end); i++,off++) {
      char c = val.charAt(i);
      if ((c < 0x20) || (c > 0x7E)) c = '?';
      buf[off] = (byte)c;
    }
    fill(buf, off, end, (byte)0);
    return val.length();
  }

  /** Computes the length a UTF-8 string will need in a buffer.
   *  @param val The string
   *  @return The length required to hold the entire UTF-8 string.
   */
  public static int lengthUTF8 (CharSequence val) {
    if (val instanceof AsciiCharSequence) return ((AsciiCharSequence)val).length(); // fast ASCII version

    String str = val.toString();
    int    len = 0;
    
    for (int i = 0; i < str.length(); i++) {
      int c = str.codePointAt(i);

      if ((c >= 0x0001) && (c <= 0x007F)) {                // ASCII (except embedded NUL)
        len++;
      }
      else if (c <= 0x07FF) {                              // 0x0080 to 0x07FF range (or embedded NUL)
        len+=2;
      }
      else if (c < 0xFFFF) {                               // 0x0800 to 0xFFFF range
        len+=3;
      }
      else if (c < 0x10FFFF) {                             // 0x010000 to 0x10FFFF range
        i++; // <-- skip the low-surrogate in the string since i is 16-bit based
        len+=4;
      }
      else {
        throw new IllegalArgumentException("Invalid character 0x"+Integer.toHexString(c)+" found in '"+val+"'");
      }
    }
    return len;
  }

  /** Pack a UTF-8 string into a buffer. If <tt>val</tt> is longer than <tt>len</tt> it will be
   *  truncated without explicit warning. If <tt>val</tt> is shorter than <tt>len</tt> it will be
   *  padded out with ASCII NUL ('\0') characters to fill the length.
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
  public static int packUTF8 (byte[] buf, int off, CharSequence val, int length) {
    if (val instanceof AsciiCharSequence) return packAscii(buf, off, val, length); // fast ASCII version

    String str = val.toString();
    int    len = 0;
    int    end = off+length;

    for (int i = 0; (i < str.length()) && (off < end); i++) {
      int c = str.codePointAt(i);

      if ((c >= 0x0001) && (c <= 0x007F)) {                // ASCII (except embedded NUL)
        buf[off++] = (byte)c;
        len++;
      }
      else if (c <= 0x07FF) {                              // 0x0080 to 0x07FF range (or embedded NUL)
        if (off+1 < end) {
          buf[off++] = (byte)(UTF8_2BYTE_0 | ((c >> 6) & 0x1F));
          buf[off++] = (byte)(UTF8_2BYTE_1 | (   c     & 0x3F));
        }
        else {
          while (off < end) buf[off++] = 0;
        }
        len+=2;
      }
      else if (c < 0xFFFF) {                               // 0x0800 to 0xFFFF range
        if (off+2 < end) {
          buf[off++] = (byte)(UTF8_3BYTE_0 | ((c >> 12) & 0x0F));
          buf[off++] = (byte)(UTF8_3BYTE_1 | ((c >>  6) & 0x3F));
          buf[off++] = (byte)(UTF8_3BYTE_2 | (    c     & 0x3F));
        }
        len+=3;
      }
      else if (c < 0x10FFFF) {                             // 0x010000 to 0x10FFFF range
        i++; // <-- skip the low-surrogate in the string since i is 16-bit based
        if (off+3 < end) {
          buf[off++] = (byte)(UTF8_4BYTE_0 | ((c >> 18) & 0x07));
          buf[off++] = (byte)(UTF8_4BYTE_1 | ((c >> 12) & 0x3F));
          buf[off++] = (byte)(UTF8_4BYTE_2 | ((c >>  6) & 0x3F));
          buf[off++] = (byte)(UTF8_4BYTE_3 | (    c     & 0x3F));
        }
        else {
          while (off < end) buf[off++] = 0;
        }
        len+=4;
      }
      else {
        throw new IllegalArgumentException("Invalid character 0x"+Integer.toHexString(c)+" found in '"+val+"'");
      }
    }
    fill(buf, off, end, (byte)0);
    return len;
  }

  /** Pack a block of metadata into the buffer.
   *  @param buf    byte array of data [OUTPUT]
   *  @param off    Offset into array
   *  @param meta   The metadata to pack into the array [INPUT]
   *  @param len    The length to pack.
   *  @throws IllegalArgumentException If <tt>buffer.length != len</tt>.
   */
  public static int packMetadata (byte[] buf, int off, MetadataBlock meta, int len) {
    int length = meta.writeBytes(buf, off, len);
    if (length > len) {
      throw new IllegalArgumentException("Incompatible length ("+length+") expected "+len);
    }
    return length;
  }

  /** Pack an array of bytes into the buffer.
   *  @param buf    byte array of data [OUTPUT]
   *  @param off    Offset into array
   *  @param buffer The buffer to pack into the array [INPUT]
   *  @param len    The length to pack.
   *  @throws IllegalArgumentException If <tt>buffer.length != len</tt>.
   */
  public static void packBytes (byte[] buf, int off, byte[] buffer, int len) {
    if (buf.length != len) {
      throw new IllegalArgumentException("Incompatible length ("+buf.length+") expected "+len);
    }
    packBytes(buf, off, buffer, 0, buf.length);
  }

  /** Pack an array of bytes into the buffer.
   *  @param buf    byte array of data [OUTPUT]
   *  @param off    Offset into array
   *  @param buffer The buffer to pack into the array [INPUT]
   *  @param boff   The offset into the buffer to pack
   *  @param len    The length to pack.
   *  @throws ArrayIndexOutOfBoundsException If <tt>off</tt>, <tt>boff</tt> or <tt>len</tt> are invalid.
   */
  public static void packBytes (byte[] buf, int off, byte[] buffer, int boff, int len) {
    System.arraycopy(buffer, boff, buf, off, len);
  }

  /** Pack a VITA 49.0 time stamp into a buffer.
   *  @param buf   byte array of data [OUTPUT]
   *  @param off   Offset into array
   *  @param val   value to pack [INPUT]
   *  @param epoch The epoch to use (must be either {@link TimeStamp.IntegerMode#UTC} or
   *               {@link TimeStamp.IntegerMode#GPS}).
   *  @param rep   The byte representation to use
   */
  public static void packTimeStamp (byte[] buf, int off, TimeStamp val, TimeStamp.IntegerMode epoch, ByteOrder rep) {
    if (epoch == TimeStamp.IntegerMode.UTC) {
      packInt(buf, off, val.getSecondsUTC(), rep);
      packLong(buf, off+4, val.getPicoSeconds(), rep);
    }
    else if (epoch == TimeStamp.IntegerMode.GPS) {
      packInt(buf, off, val.getSecondsGPS(), rep);
      packLong(buf, off+4, val.getPicoSeconds(), rep);
    }
    else {
      throw new IllegalArgumentException("Unsupported epoch "+epoch);
    }
  }

  /** Pack a VITA 49.0 time stamp into a buffer in {@link ByteOrder#BIG_ENDIAN}.
   *  @param buf   byte array of data [OUTPUT]
   *  @param off   Offset into array
   *  @param val   value to pack [INPUT]
   *  @param epoch The epoch to use (must be either {@link TimeStamp.IntegerMode#UTC} or
   *               {@link TimeStamp.IntegerMode#GPS}).
   */
  public static void packTimeStamp (byte[] buf, int off, TimeStamp val, TimeStamp.IntegerMode epoch) {
    if (epoch == TimeStamp.IntegerMode.UTC) {
      packInt(buf, off, val.getSecondsUTC());
      packLong(buf, off+4, val.getPicoSeconds());
    }
    else if (epoch == TimeStamp.IntegerMode.GPS) {
      packInt(buf, off, val.getSecondsGPS());
      packLong(buf, off+4, val.getPicoSeconds());
    }
    else {
      throw new IllegalArgumentException("Unsupported epoch "+epoch);
    }
  }

  /** Pack a 16-byte IPv6 address into a buffer.
   *  @param buf   byte array of data [OUTPUT]
   *  @param off   Offset into array
   *  @param val   value to pack [INPUT]
   */
  public static void packInetAddr (byte[] buf, int off, InetAddress val) {
    byte[] addr = val.getAddress();
    if (addr.length == 4) {
      System.arraycopy(IPV4_MAPPED, 0, buf, off,    12);
      System.arraycopy(addr,        0, buf, off+12,  4);
    }
    else if (addr.length == 16) {
      System.arraycopy(addr, 0, buf, off, 16);
    }
    else {
      throw new IllegalArgumentException("Unsupported InetAddress "+val);
    }
  }

  /** Pack a 16-byte UUID into a buffer.
   *  @param buf   byte array of data [OUTPUT]
   *  @param off   Offset into array
   *  @param val   value to pack [INPUT]
   */
  public static void packUUID (byte[] buf, int off, UUID val) {
    packLong(buf,  off,  val.getMostSignificantBits());
    packLong(buf, off+8, val.getLeastSignificantBits());
  }

  /** Pack a record into a buffer.
   *  @param buf   byte array of data
   *  @param off   Offset into array
   *  @param val   The record to pack
   */
  public static void packRecord (byte[] buf, int off, Record val) {
    val.readBytes(buf, off);
  }
}

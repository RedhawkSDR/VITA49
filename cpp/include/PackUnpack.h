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

#ifndef _PackUnpack_h
#define _PackUnpack_h

#include "VRTMath.h"
#include "VRTObject.h"
//#include "BasicVRTPacket.h" // TODO is this needed still? or just needed for PayloadFormat?
#include "PayloadFormat.h"
#include <vector>

using namespace std;
using namespace vrt;

namespace vrt {
  /** <b>Internal Use Only:</b> Pack/unpack methods used by {@link BasicDataPacket}. <br>
   *  <br>
   *  <h2>Usage Notes</h2>
   *  <ul>
   *    <li>If the data type of the array given does not match the PayloadFormat,
   *        the handling of any values outside of the mutual range limits is
   *        undefined. <i>(For example inserting the value 1024 from an Int32
   *        array into a PayloadFormat of Int8.)</i></li>
   *    <li>If the data type of the array given does not match the PayloadFormat,
   *        the handling of any values outside of the mutual precision limits
   *        will result in a loss of precision. (For example inserting the value
   *        3.14 from an Float32 array into a PayloadFormat of Int8 will result in
   *        the value 3 being set.)</li>
   *    <li>The handling of NaN, +Inf, and -Inf when converting from a floating-point
   *        type to an integer type is undefined.</li>
   *    <li>All floating-point NaN values are considered equal any NaN may be
   *        converted to any other NaN (i.e. "signaling NaN values" are not
   *        preserved).</li>
   *    <li>All floating-point 0.0 values (i.e. -0.0 and +0.0) are considered equal
   *        and any 0.0 may be converted to any other 0.0 (i.e. the sign bit on
   *        a 0.0 is not preserved).</li>
   *  </ul>
   *  <h2>Performance Characteristics</h2>
   *  Great effort has been made to optimize the most commonly used pack/unpack
   *  conversions. The table below shows the relative speed at which a given
   *  PayloadFormat can be packed/unpacked from a buffer.
   *  <pre>
   *       Speed   | Item Packing | Data Item |  Event   | Channel  | Efficient | Notes |
   *        [0]    |  Field Size  | Size [1]  | Tag Size | Tag Size |   Mode    |       |
   *    -----------+--------------+-----------+----------+----------+-----------+-------+
   *         A+    |       8      |     8     |    0     |     0    | Link/Proc |       |
   *     (Fastest) |              |           |          |          |           |       |
   *    -----------+--------------+-----------+----------+----------+-----------+-------+
   *         A     |       1      |     1     |    0     |     0    | Link/Proc |  [2]  |
   *               |       4      |     4     |    0     |     0    | Link/Proc |  [2]  |
   *               |      12      |    12     |    0     |     0    | Link/Proc |  [2]  |
   *               |      16      |    16     |    0     |     0    | Link/Proc |       |
   *               |      24      |    24     |    0     |     0    | Link/Proc |       |
   *               |      32      |    32     |    0     |     0    | Link/Proc |       |
   *               |      64      |    64     |    0     |     0    | Link/Proc |       |
   *               |      32      |   float   |    0     |     0    | Link/Proc |       |
   *               |      64      |   double  |    0     |     0    | Link/Proc |       |
   *    -----------+--------------+-----------+----------+----------+-----------+-------+
   *         A-    |       8      |  1 to  7  |    -     |     -    | Link/Proc |  [3]  |
   *               |      16      |  1 to 15  |    -     |     -    | Link/Proc |  [3]  |
   *               |      24      |  1 to 23  |    -     |     -    | Link/Proc |  [3]  |
   *               |      32      |  1 to 31  |    -     |     -    | Link/Proc |  [3]  |
   *               |      64      |  1 to 63  |    -     |     -    | Link/Proc |  [3]  |
   *               |   17 to 63   |  1 to 63  |    -     |     -    |    Proc   |       |
   *    -----------+--------------+-----------+----------+----------+-----------+-------+
   *         B     |      32      |  1 to 31  |  0 to 7  |  0 to 15 | Link/Proc |       |
   *               |      64      |  1 to 63  |  0 to 7  |  0 to 15 | Link/Proc |       |
   *               |      64      |   float   |  0 to 7  |  0 to 15 | Link/Proc |       |
   *               |   17 to 63   |  1 to 63  |  0 to 7  |  0 to 15 |    Proc   |       |
   *               |   33 to 63   |   float   |  0 to 7  |  0 to 15 |    Proc   |       |
   *    -----------+--------------+-----------+----------+----------+-----------+-------+
   *         D+    |    1 to 15   |  1 to 15  |    -     |     -    | Link/Proc | [3,4] |
   *               |   17 to 63   |  1 to 63  |    -     |     -    |    Link   |  [3]  |
   *               |   33 to 63   |   float   |    -     |     -    |    Link   |  [3]  |
   *    -----------+--------------+-----------+----------+----------+-----------+-------+
   *         D     |    1 to 15   |  1 to 15  |  0 to 7  |  0 to 15 | Link/Proc |  [4]  |
   *     (Slowest) |   17 to 63   |  1 to 63  |  0 to 7  |  0 to 15 |    Link   |       |
   *               |   33 to 63   |   float   |  0 to 7  |  0 to 15 |    Link   |       |
   *    -----------+--------------+-----------+----------+----------+-----------+-------+
   *
   *    Notes:
   *      [0] The "speed" rating is an arbitrary "grade" indicating how fast the accessing
   *          of a given Payload Format is relative to other Payload Formats, where an
   *          "A+" equates to a simple memcpy in C++ and a "D" equates to a complex
   *          processing algorithm with nested loops and heavy use of bit manipulation.
   *          In practice the speed difference in speed between a "B" and an "A" is
   *          small whereas the difference between a "B" and a "D" is many orders of
   *          magnitude slower. <i>(For example, on one test of ~2 million iterations using
   *          both link efficient ("D") and processing efficient ("B") and with Packing
   *          Field Sizes between 33 and 63 bits it took 38.2 sec for link efficient vs
   *          &lt;0.1 sec for processing efficient.)</i>
   *      [1] All Data items are signed/unsigned integers with the bit count as shown,
   *          except for "float" and "double" which represent IEEE-754 floating-point
   *          values.
   *      [2] Fast handling of Int1 and Int4/Int12 data by unpacking eight/two elements
   *          at a time. Total length must be a multiple of 8/2 to use this mode.
   *      [3] Event/Channel tags (if present) are not read and set to zero on write.
   *      [4] Future versions may provide a faster implementation for 1 to 15 bit
   *          field sizes when in Processing Efficient mode.
   *  </pre>
   *  Currently all of the VRT floating-point formats are using algorithms that
   *  are similar to the "D+" and "D" ones used for IEEE-754 float data. Future
   *  versions may provide a faster implementation for situations where the data
   *  item size is 8/16/32/64 bits and/or processing-efficient mode is used. <br>
   *  <br>
   *  Regardless of the algorithm used herein, the accessing of IEEE-754
   *  floating-point values will always be faster than the accessing of VRT
   *  floating-point values since the former benefit from hardware acceleration.
   */
  namespace PackUnpack {
#if NOT_USING_JNI
    /** <b>Internal Use Only:</b> Unpacks values with the given format specifier as
     *  an array of <tt>double</tt>. <br>
     *  <br>
     *  Setting the channel tags (<tt>chan</tt>) and/or event tags (<tt>evt</tt>)
     *  arrays to null will cause any channel or event tags present to be ignored.
     *  In most cases this results in a performance improvement. <br>
     *  <br>
     *  If the channel tags (<tt>chan</tt>) and/or event tags (<tt>evt</tt>) arrays
     *  are non-null when there are no channel or event tags present, the values in
     *  the arrays will not be modified.
     *  @param pf     (IN)  The data format
     *  @param ptr    (IN)  The buffer pointer
     *  @param off    (IN)  The buffer offset
     *  @param array  (OUT) The output array for the values
     *  @param chan   (OUT) The output array for the channel tags (null if n/a)
     *  @param evt    (OUT) The output array for the event tags (null if n/a)
     *  @param length (IN)  Number of elements to convert
     *  @return The output array (same as <tt>array</tt>).
     */
    double* unpackAsDouble (PayloadFormat pf, const void *ptr, int32_t off, double *array, int32_t *chan, int32_t *evt, int32_t length);
    /** <b>Internal Use Only:</b> Packs values with the given format specifier from
     *  an array of double. <br>
     *  <br>
     *  Setting the channel tags (<tt>chan</tt>) and/or event tags (<tt>evt</tt>)
     *  arrays to null will cause any channel or event tags present to be set to
     *  zero. In most cases this results in a performance improvement over passing
     *  in an array of zeros. <br>
     *  <br>
     *  If the channel tags (<tt>chan</tt>) and/or event tags (<tt>evt</tt>) arrays
     *  are non-null when there are no channel or event tags present, the values in
     *  the arrays will be ignored.
     *  @param pf     (IN)  The data format
     *  @param ptr    (I/O) The buffer pointer
     *  @param off    (IN)  The buffer offset
     *  @param array  (IN)  The values to pack
     *  @param chan   (OUT) The output array for the channel tags (null if n/a)
     *  @param evt    (OUT) The output array for the event tags (null if n/a)
     *  @param length (IN)  Number of elements to convert
     */
    void packAsDouble (PayloadFormat pf, void *ptr, int32_t off, const double *array, const int32_t *chan, const int32_t *evt, int32_t length);
#endif /* NOT_USING_JNI */
#if NOT_USING_JNI
    /** <b>Internal Use Only:</b> Unpacks values with the given format specifier as
     *  an array of <tt>float</tt>. <br>
     *  <br>
     *  Setting the channel tags (<tt>chan</tt>) and/or event tags (<tt>evt</tt>)
     *  arrays to null will cause any channel or event tags present to be ignored.
     *  In most cases this results in a performance improvement. <br>
     *  <br>
     *  If the channel tags (<tt>chan</tt>) and/or event tags (<tt>evt</tt>) arrays
     *  are non-null when there are no channel or event tags present, the values in
     *  the arrays will not be modified.
     *  @param pf     (IN)  The data format
     *  @param ptr    (IN)  The buffer pointer
     *  @param off    (IN)  The buffer offset
     *  @param array  (OUT) The output array for the values
     *  @param chan   (OUT) The output array for the channel tags (null if n/a)
     *  @param evt    (OUT) The output array for the event tags (null if n/a)
     *  @param length (IN)  Number of elements to convert
     *  @return The output array (same as <tt>array</tt>).
     */
    float* unpackAsFloat (PayloadFormat pf, const void *ptr, int32_t off, float *array, int32_t *chan, int32_t *evt, int32_t length);
    /** <b>Internal Use Only:</b> Packs values with the given format specifier from
     *  an array of float. <br>
     *  <br>
     *  Setting the channel tags (<tt>chan</tt>) and/or event tags (<tt>evt</tt>)
     *  arrays to null will cause any channel or event tags present to be set to
     *  zero. In most cases this results in a performance improvement over passing
     *  in an array of zeros. <br>
     *  <br>
     *  If the channel tags (<tt>chan</tt>) and/or event tags (<tt>evt</tt>) arrays
     *  are non-null when there are no channel or event tags present, the values in
     *  the arrays will be ignored.
     *  @param pf     (IN)  The data format
     *  @param ptr    (I/O) The buffer pointer
     *  @param off    (IN)  The buffer offset
     *  @param array  (IN)  The values to pack
     *  @param chan   (OUT) The output array for the channel tags (null if n/a)
     *  @param evt    (OUT) The output array for the event tags (null if n/a)
     *  @param length (IN)  Number of elements to convert
     */
    void packAsFloat (PayloadFormat pf, void *ptr, int32_t off, const float *array, const int32_t *chan, const int32_t *evt, int32_t length);
#endif /* NOT_USING_JNI */
#if NOT_USING_JNI
    /** <b>Internal Use Only:</b> Unpacks values with the given format specifier as
     *  an array of <tt>int64_t</tt>. <br>
     *  <br>
     *  Setting the channel tags (<tt>chan</tt>) and/or event tags (<tt>evt</tt>)
     *  arrays to null will cause any channel or event tags present to be ignored.
     *  In most cases this results in a performance improvement. <br>
     *  <br>
     *  If the channel tags (<tt>chan</tt>) and/or event tags (<tt>evt</tt>) arrays
     *  are non-null when there are no channel or event tags present, the values in
     *  the arrays will not be modified.
     *  @param pf     (IN)  The data format
     *  @param ptr    (IN)  The buffer pointer
     *  @param off    (IN)  The buffer offset
     *  @param array  (OUT) The output array for the values
     *  @param chan   (OUT) The output array for the channel tags (null if n/a)
     *  @param evt    (OUT) The output array for the event tags (null if n/a)
     *  @param length (IN)  Number of elements to convert
     *  @return The output array (same as <tt>array</tt>).
     */
    int64_t* unpackAsLong (PayloadFormat pf, const void *ptr, int32_t off, int64_t *array, int32_t *chan, int32_t *evt, int32_t length);
    /** <b>Internal Use Only:</b> Packs values with the given format specifier from
     *  an array of int64_t. <br>
     *  <br>
     *  Setting the channel tags (<tt>chan</tt>) and/or event tags (<tt>evt</tt>)
     *  arrays to null will cause any channel or event tags present to be set to
     *  zero. In most cases this results in a performance improvement over passing
     *  in an array of zeros. <br>
     *  <br>
     *  If the channel tags (<tt>chan</tt>) and/or event tags (<tt>evt</tt>) arrays
     *  are non-null when there are no channel or event tags present, the values in
     *  the arrays will be ignored.
     *  @param pf     (IN)  The data format
     *  @param ptr    (I/O) The buffer pointer
     *  @param off    (IN)  The buffer offset
     *  @param array  (IN)  The values to pack
     *  @param chan   (OUT) The output array for the channel tags (null if n/a)
     *  @param evt    (OUT) The output array for the event tags (null if n/a)
     *  @param length (IN)  Number of elements to convert
     */
    void packAsLong (PayloadFormat pf, void *ptr, int32_t off, const int64_t *array, const int32_t *chan, const int32_t *evt, int32_t length);
#endif /* NOT_USING_JNI */
#if NOT_USING_JNI
    /** <b>Internal Use Only:</b> Unpacks values with the given format specifier as
     *  an array of <tt>int32_t</tt>. <br>
     *  <br>
     *  Setting the channel tags (<tt>chan</tt>) and/or event tags (<tt>evt</tt>)
     *  arrays to null will cause any channel or event tags present to be ignored.
     *  In most cases this results in a performance improvement. <br>
     *  <br>
     *  If the channel tags (<tt>chan</tt>) and/or event tags (<tt>evt</tt>) arrays
     *  are non-null when there are no channel or event tags present, the values in
     *  the arrays will not be modified.
     *  @param pf     (IN)  The data format
     *  @param ptr    (IN)  The buffer pointer
     *  @param off    (IN)  The buffer offset
     *  @param array  (OUT) The output array for the values
     *  @param chan   (OUT) The output array for the channel tags (null if n/a)
     *  @param evt    (OUT) The output array for the event tags (null if n/a)
     *  @param length (IN)  Number of elements to convert
     *  @return The output array (same as <tt>array</tt>).
     */
    int32_t* unpackAsInt (PayloadFormat pf, const void *ptr, int32_t off, int32_t *array, int32_t *chan, int32_t *evt, int32_t length);
    /** <b>Internal Use Only:</b> Packs values with the given format specifier from
     *  an array of int32_t. <br>
     *  <br>
     *  Setting the channel tags (<tt>chan</tt>) and/or event tags (<tt>evt</tt>)
     *  arrays to null will cause any channel or event tags present to be set to
     *  zero. In most cases this results in a performance improvement over passing
     *  in an array of zeros. <br>
     *  <br>
     *  If the channel tags (<tt>chan</tt>) and/or event tags (<tt>evt</tt>) arrays
     *  are non-null when there are no channel or event tags present, the values in
     *  the arrays will be ignored.
     *  @param pf     (IN)  The data format
     *  @param ptr    (I/O) The buffer pointer
     *  @param off    (IN)  The buffer offset
     *  @param array  (IN)  The values to pack
     *  @param chan   (OUT) The output array for the channel tags (null if n/a)
     *  @param evt    (OUT) The output array for the event tags (null if n/a)
     *  @param length (IN)  Number of elements to convert
     */
    void packAsInt (PayloadFormat pf, void *ptr, int32_t off, const int32_t *array, const int32_t *chan, const int32_t *evt, int32_t length);
#endif /* NOT_USING_JNI */
#if NOT_USING_JNI
    /** <b>Internal Use Only:</b> Unpacks values with the given format specifier as
     *  an array of <tt>int16_t</tt>. <br>
     *  <br>
     *  Setting the channel tags (<tt>chan</tt>) and/or event tags (<tt>evt</tt>)
     *  arrays to null will cause any channel or event tags present to be ignored.
     *  In most cases this results in a performance improvement. <br>
     *  <br>
     *  If the channel tags (<tt>chan</tt>) and/or event tags (<tt>evt</tt>) arrays
     *  are non-null when there are no channel or event tags present, the values in
     *  the arrays will not be modified.
     *  @param pf     (IN)  The data format
     *  @param ptr    (IN)  The buffer pointer
     *  @param off    (IN)  The buffer offset
     *  @param array  (OUT) The output array for the values
     *  @param chan   (OUT) The output array for the channel tags (null if n/a)
     *  @param evt    (OUT) The output array for the event tags (null if n/a)
     *  @param length (IN)  Number of elements to convert
     *  @return The output array (same as <tt>array</tt>).
     */
    int16_t* unpackAsShort (PayloadFormat pf, const void *ptr, int32_t off, int16_t *array, int32_t *chan, int32_t *evt, int32_t length);
    /** <b>Internal Use Only:</b> Packs values with the given format specifier from
     *  an array of int16_t. <br>
     *  <br>
     *  Setting the channel tags (<tt>chan</tt>) and/or event tags (<tt>evt</tt>)
     *  arrays to null will cause any channel or event tags present to be set to
     *  zero. In most cases this results in a performance improvement over passing
     *  in an array of zeros. <br>
     *  <br>
     *  If the channel tags (<tt>chan</tt>) and/or event tags (<tt>evt</tt>) arrays
     *  are non-null when there are no channel or event tags present, the values in
     *  the arrays will be ignored.
     *  @param pf     (IN)  The data format
     *  @param ptr    (I/O) The buffer pointer
     *  @param off    (IN)  The buffer offset
     *  @param array  (IN)  The values to pack
     *  @param chan   (OUT) The output array for the channel tags (null if n/a)
     *  @param evt    (OUT) The output array for the event tags (null if n/a)
     *  @param length (IN)  Number of elements to convert
     */
    void packAsShort (PayloadFormat pf, void *ptr, int32_t off, const int16_t *array, const int32_t *chan, const int32_t *evt, int32_t length);
#endif /* NOT_USING_JNI */
#if NOT_USING_JNI
    /** <b>Internal Use Only:</b> Unpacks values with the given format specifier as
     *  an array of <tt>int8_t</tt>. <br>
     *  <br>
     *  Setting the channel tags (<tt>chan</tt>) and/or event tags (<tt>evt</tt>)
     *  arrays to null will cause any channel or event tags present to be ignored.
     *  In most cases this results in a performance improvement. <br>
     *  <br>
     *  If the channel tags (<tt>chan</tt>) and/or event tags (<tt>evt</tt>) arrays
     *  are non-null when there are no channel or event tags present, the values in
     *  the arrays will not be modified.
     *  @param pf     (IN)  The data format
     *  @param ptr    (IN)  The buffer pointer
     *  @param off    (IN)  The buffer offset
     *  @param array  (OUT) The output array for the values
     *  @param chan   (OUT) The output array for the channel tags (null if n/a)
     *  @param evt    (OUT) The output array for the event tags (null if n/a)
     *  @param length (IN)  Number of elements to convert
     *  @return The output array (same as <tt>array</tt>).
     */
    int8_t* unpackAsByte (PayloadFormat pf, const void *ptr, int32_t off, int8_t *array, int32_t *chan, int32_t *evt, int32_t length);
    /** <b>Internal Use Only:</b> Packs values with the given format specifier from
     *  an array of int8_t. <br>
     *  <br>
     *  Setting the channel tags (<tt>chan</tt>) and/or event tags (<tt>evt</tt>)
     *  arrays to null will cause any channel or event tags present to be set to
     *  zero. In most cases this results in a performance improvement over passing
     *  in an array of zeros. <br>
     *  <br>
     *  If the channel tags (<tt>chan</tt>) and/or event tags (<tt>evt</tt>) arrays
     *  are non-null when there are no channel or event tags present, the values in
     *  the arrays will be ignored.
     *  @param pf     (IN)  The data format
     *  @param ptr    (I/O) The buffer pointer
     *  @param off    (IN)  The buffer offset
     *  @param array  (IN)  The values to pack
     *  @param chan   (OUT) The output array for the channel tags (null if n/a)
     *  @param evt    (OUT) The output array for the event tags (null if n/a)
     *  @param length (IN)  Number of elements to convert
     */
    void packAsByte (PayloadFormat pf, void *ptr, int32_t off, const int8_t *array, const int32_t *chan, const int32_t *evt, int32_t length);
#endif /* NOT_USING_JNI */
  } END_NAMESPACE
} END_NAMESPACE
#endif /* _PackUnpack_h */


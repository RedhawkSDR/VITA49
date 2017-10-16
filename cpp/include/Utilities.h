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

#ifndef _Utilities_h
#define _Utilities_h

#include <stdarg.h>
#include "VRTObject.h"

using namespace std;
namespace vrt {
  /** Various utility methods.
   *
   *  @author 
   */
  namespace Utilities {
    /** Creates a string based on a format specifier and values. This is
     *  essentially a variant of <tt>snprintf(..)</tt> that returns a new
     *  string rather than setting a <tt>char*</tt> buffer.
     *  @param fmt The format specifier.
     *  @param ... The remaining format values.
     */
#ifdef DOXYGEN
    string format (const char *fmt, ...);
    // The above line is to work-around doxygen bug 643056
#else
    _Intel_Pragma("__printf_args")
    string format (const char *fmt, ...) __attribute__((format(printf,1,2)));
#endif
    
    /** Appends a value, assuming it is not null.
     *  @param s      (OUT) The output stream.
     *  @param prefix (IN)  The prefix to include ("" if n/a).
     *  @param val    (IN)  The value.
     *  @param suffix (IN)  The suffix to include ("" if n/a).
     */
    template <typename T>
    inline void append (ostream &s, const string &prefix, T val, const string &suffix=string("")) {
      if (!isNull(val)) s << prefix << val << suffix;
    }

    /** Appends a value, assuming it is not null.
     *  @param s      (OUT) The output stream.
     *  @param prefix (IN)  The prefix to include ("" if n/a).
     *  @param val    (IN)  The value.
     *  @param suffix (IN)  The suffix to include ("" if n/a).
     */
    template <const string &>
    inline void append (ostream &s, const string &prefix, const string &val, const string &suffix=string("")) {
      if (!isNull(val)) s << prefix << val << suffix;
    }

    /** Appends a value, assuming it is not null.
     *  @param s      (OUT) The output stream.
     *  @param prefix (IN)  The prefix to include ("" if n/a).
     *  @param val    (IN)  The value.
     *  @param suffix (IN)  The suffix to include ("" if n/a).
     */
    template <const VRTObject &>
    inline void append (ostream &s, const string &prefix, const VRTObject &val, const string &suffix=string("")) {
      if (!isNull(val)) s << prefix << val << suffix;
    }

    /** Converts a "boolean" string to a boolean value. The following (case-insensitive) conversions
     *  are used:
     *  <pre>
     *    _NULL:       ""
     *    _TRUE:       "true",  "t", "1", "yes", "y", "on"
     *    _FALSE:      "false", "f", "0", "no",  "n", "off"
     *  </pre>
     *  @param obj The object to convert.
     *  @return <tt>_NULL</tt>, <tt>_TRUE</tt>, or <tt>_FALSE</tt> as appropriate.
     */
    boolNull toBoolean (const string &obj);

    /** Converts a "boolean" string to a boolean value. The is identical to {@link #toBoolean}
     *  except that it returns <tt>false</tt> in place of <tt>null</tt>.
     *  @param obj The object to convert.
     *  @return <tt>true</tt> or <tt>false</tt> as appropriate.
     */
    inline bool toBooleanValue (const string &obj) {
      boolNull b = toBoolean(obj);
      return ((b != _NULL) && (b == _TRUE));
    }

    /** Trims excess whitespace off the front and end of a string and then returns <tt>""</tt> if
     *  the result is "NA" or "N/A" (case insensitive).
     *  @param str The input string.
     *  @return The trimmed output or <tt>str</tt> if no trimming is required.
     */
    string trimNA (const string &str);

    /** Trims excess whitespace off the front and end of a string. This is modeled after
     *  <tt>java.lang.String.trim()</tt> and is used within the auto-generated packet code.
     *  @param str The input string.
     *  @return The trimmed output or <tt>str</tt> if no trimming is required.
     */
    string trim (const string &str);

    /** Quick conversion to hex string.
     *  @param val   The value to convert.
     *  @param bytes The number of bytes to consider.
     */
    string toHexString (int32_t val, int32_t bytes=4);

    /** Quick conversion to hex string.
     *  @param val   The value to convert.
     *  @param bytes The number of bytes to consider.
     */
    string toHexString (int64_t val, int32_t bytes=8);

    /** Converts the numeric form of an IEEE Organizationally Unique Identifier (OUI) to a string.
     *  @param oui The 24-bit OUI (top 8 bits of the int must be 0x00).
     *  @return The string form of the OUI (IEEE standard format).
     */
    string toStringOUI (int32_t oui);

    /** Converts the string form of an IEEE Organizationally Unique Identifier (OUI) to a numeric.
     *  @param oui The string form of the OUI (IEEE standard format).
     *  @return The 24-bit OUI.
     */
    int32_t fromStringOUI (string oui);

    /** Converts the numeric form of a VRT class identifier to a string.
     *  @param id The 56-bit class ID (top 8 bits of the long must be 0x00).
     *  @return The string form of the class identifier.
     */
    string toStringClassID (int64_t id);

    /** Converts the numeric form of a VRT class identifier to a string.
     *  @param oui The 24-bit OUI (top 8 bits of the int must be 0x00).
     *  @param icc The 16-bit (unsigned) information class code (ICC).
     *  @param pcc The 16-bit (unsigned) packet class code (PCC).
     *  @return The string form of the class identifier.
     */
    inline string toStringClassID (int32_t oui, int16_t icc, int16_t pcc) {
      return toStringOUI(oui) + ":" + toHexString(icc,2) + "." + toHexString(pcc,2);
    }

    /** Converts the string form of a VRT class identifier to a numeric.
     *  @param id The string form of the class identifier.
     *  @return The 56-bit class ID.
     */
    int64_t fromStringClassID (string id);

    /** Converts the numeric form of a device identifier to a string.
     *  @param id The 40-bit manufacturer OUI stored in a 64-bit space as:
     *            <tt>0x00xxxxxx0000yyyy</tt> where <tt>xxxxxx</tt> is the 24-bit manufacturer
     *            OUI and <tt>yyyy</tt> is the 16-bit device ID.
     *  @return The string form of the device identifier.
     */
    string toStringDeviceID (int64_t id);

    /** Converts the numeric form of a device identifier to a string.
     *  @param oui The 24-bit manufacturer OUI (top 8 bits of the int must be 0x00).
     *  @param dev The 16-bit (unsigned) device identifier.
     *  @return The string form of the device identifier.
     */
    inline string toStringDeviceID (int32_t oui, int16_t dev) {
      return toStringOUI(oui) + ":" + toHexString(dev,2);
    }

    /** Converts the string form of a device identifier to a numeric.
     *  @param id The string form of the device identifier.
     *  @return The numeric form of the device identifier.
     */
    int64_t fromStringDeviceID (string id);
    
    /** Normalizes an angle such that it is on the range of [0,360).
     *  @param deg The input angle in degrees.
     *  @return The corresponding angle in the range [0,360).
     */
    double normalizeAngle360 (double deg);
    
    /** Normalizes an angle such that it is on the range of [-180,+180).
     *  @param deg The input angle in degrees.
     *  @return The corresponding angle in the range [-180,+180).
     */
    double normalizeAngle180 (double deg);
    
    /** Converts an IEEE-764 double into the corresponding 64-bits contained
     *  in a 64-bit integer. <br>
     *  <br>
     *  The name of this function is patterned after the Java function (as this
     *  provides some consistency between the Java and C++ version), but the
     *  implementation is entirely different.
     */
    inline int64_t doubleToRawLongBits (double val) {
      return *((int64_t*)(void*)&val);
    }
    
    /** Converts an IEEE-764 float into the corresponding 32-bits contained
     *  in a 32-bit integer. <br>
     *  <br>
     *  The name of this function is patterned after the Java function (as this
     *  provides some consistency between the Java and C++ version), but the
     *  implementation is entirely different.
     */
    inline int32_t floatToRawIntBits (float val) {
      return *((int32_t*)(void*)&val);
    }

    /** Converts the 64-bits corresponding to an IEEE-764 double, as contained
     *  in a 64-bit integer, into an IEEE-764 double. <br>
     *  <br>
     *  The name of this function is patterned after the Java function (as this
     *  provides some consistency between the Java and C++ version), but the
     *  implementation is entirely different.
     */
    inline double longBitsToDouble (int64_t bits) {
      return *((double*)(void*)&bits);
    }

    /** Converts the 32-bits corresponding to an IEEE-764 float, as contained
     *  in a 32-bit integer, into an IEEE-764 float. <br>
     *  <br>
     *  The name of this function is patterned after the Java function (as this
     *  provides some consistency between the Java and C++ version), but the
     *  implementation is entirely different.
     */
    inline float intBitsToFloat (int32_t bits) {
      return *((float*)(void*)&bits);
    }
    
    /** Searches a vector for a specified element. <br>
     *  <br>
     *  The name of this function is patterned after the Java function (as this
     *  provides some consistency between the Java and C++ version), but the
     *  implementation is entirely different.
     *  @param vec The vector to search.
     *  @param val The value to look for.
     *  @return Non-negative value indicating the index if found or a negative
     *          value indicating one less than the insertion point times -1.
     */
    template<typename T>
    inline int32_t binarySearch (const vector<T> &vec, T val) {
      // TODO: Change this to a binary rather than linear search. 
      int32_t len = (int32_t)vec.size();
      for (int32_t i = 0; i < len; i++) {
        if (vec[i] == val) return i;
        if (vec[i] >  val) return -i-1;
      }
      return -vec.size();
    }
  };
};
#endif /* _Utilities_h */

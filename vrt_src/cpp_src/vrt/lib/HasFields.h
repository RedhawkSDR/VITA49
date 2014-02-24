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

#ifndef _HasFields_h
#define _HasFields_h
#include "VRTObject.h"
#include "VRTMath.h"
#include "Value.h"
#include <sstream>
#include <iostream>
#include <map>
#include <string>

namespace vrt {
  /** Interface indicating that an object has queriable fields. This is intended
   *  for use in automated processing where the names of the fields that need to
   *  be accessed aren't known at compile-time. <br>
   *  <br>
   *  <b>WARNING: This class is not intended for "general purpose" use as a super
   *  class and should only be extended by instances of {@link VRTObject}.</b>
   *  @author  / 
   */
  class HasFields {
    public: virtual ~HasFields () { }
    
    /** Gets the number of fields. */
    public: virtual inline int32_t getFieldCount () const {
      return 0;
    }
    
    /** Gets the name of the given field. In 90% of cases the field name will be
     *  identical to the name used in the applicable get/set function except
     *  without the get/set prefix (e.g. "SourceContext" for "getSourceContext").
     *  It is possible to access the names of all fields via the following:
     *  <pre>
     *    for (int32_t id = 0; id &lt; getFieldCount(); id++) {
     *      // id is the field ID
     *      getFieldName(id);
     *    }
     *  </pre>
     *  In situations where it is desirable to access all available fields, it
     *  is faster to use the field ID as this eliminates any string comparisons.
     *  @param id The field ID (on range <tt>[0,getFieldCount())</tt>).
     *  @return   The field name.
     *  @throws VRTException if the field id is invalid.
     */
    public: virtual inline string getFieldName (int32_t id) const {
      throw VRTException("Unknown field ID #%d", id);
    }
    
    /** Gets the type associated with a given field.
     *  @param id The field ID (on range <tt>[0,getFieldCount())</tt>).
     *  @return The field type (n &gt; 0) or -N if the field is a vector
     *          of values where N is the component type of the vector.
     */
    public: virtual inline ValueType getFieldType (int32_t id) const {
      throw VRTException("Unknown field ID #%d", id);
    }

    /** Gets the value of a field, based on the field's id. <br>
     *  <br>
     *  <b>WARNING: The caller is responsible for deleting the memory referenced
     *  by the returned pointer.</b>
     *  @param id The field ID (on range <tt>[0,getFieldCount())</tt>).
     *  @return Pointer to the given value.
     */
    public: virtual inline Value* getField (int32_t id) const  {
      throw VRTException("Unknown field ID #%d", id);
    }

    /** Sets the value of a field, based on the field's id.
     *  @param id  The field ID (on range <tt>[0,getFieldCount())</tt>).
     *  @param val Pointer to the given value (must be of the applicable type).
     */
    public: virtual inline void setField (int32_t id, const Value* val) {
      throw VRTException("Unknown field ID #%d", id);
    }

    /** Sets the value of a field, based on the field's id.
     *  @param id  The field ID (on range <tt>[0,getFieldCount())</tt>).
     *  @param val The given value (must be of the applicable type).
     */
    public: inline void setField (int32_t id, const Value &val) {
      setField(id, &val);
    }
    
    /** Gets the ID of the given field. There is a 1:1 mapping of field ID's and
     *  names such that for all <tt>n = [0,getFieldCount())</tt>,
     *  <tt>n == getFieldID(getFieldName(n))</tt> is true.
     *  @param name The field name.
     *  @return     The field ID (on range <tt>[0,getFieldCount())</tt>).
     *  @throws VRTException if the field name is invalid.
     */
    public: int32_t getFieldID (const string &name) const;
    
    /** Gets the value of a field, based on the field's name. This will accept
     *  array entries and sub-fields. Examples:
     *  <pre>
     *    getFieldByName("FOO")
     *    getFieldByName("BAR[2]")
     *    getFieldByName("FOO.BAZ")
     *    getFieldByName("BAR[2].BAZ")
     *    getFieldByName("FOO.BAR[2].BAZ")
     *  </pre>
     *  When handling sub-entries (e.g. <tt>getFieldByName("FOO.BAZ")</tt>) the
     *  following decision tree is used:
     *  <pre>
     *       +------------------------+  no
     *       | Is "FOO" a valid name? |----&gt; throw exception
     *       +------------------------+
     *                  |yes
     *                  V
     *       +------------------------+  yes
     *       | Is FOO a null value?   |----&gt; return a null value
     *       +------------------------+
     *                  |no
     *                  V
     *       +------------------------+  no
     *       | Does FOO implement     |----&gt; throw exception
     *       | HasFields?             |
     *       +------------------------+
     *                  |yes
     *                  V
     *       +------------------------+  no
     *       | Is "BAR" a valid name? |----&gt; throw exception
     *       +------------------------+
     *                  |yes
     *                  V
     *       +------------------------+  yes
     *       | Is BAR a null value?   |----&gt; return a null value
     *       +------------------------+
     *                  |no
     *                  V
     *        return value of FOO.BAR
     *  </pre>
     *  Note in the above decision tree that the case of "FOO" being null results
     *  in a null value being returned which bypasses the checks for the validity
     *  of "BAZ". Indexed values (e.g. <tt>getFieldByName("BAR[2].BAZ")</tt>) are
     *  similarly handled with a null value being returned if a higher-up entry
     *  is null without checking the validity of the lower entries.<br>
     *  <br>
     *  <b>WARNING: The caller is responsible for deleting the memory referenced
     *  by the returned pointer.</b>
     *  @param name The name of the field (see {@link #getFieldName}).
     *  @return Pointer to the given value (see {@link #getField} for details).
     *          This will never be <tt>NULL</tt> but may be a pointer a value
     *          for which <tt>isNullValue()</tt> returns true -- this is
     *          intentionally done so that code in the following form will be
     *          valid:
     *  <pre>
     *    Value *val = x.getFieldByName(name);
     *    if (!isNull(val)) {
     *      ...
     *    }
     *    delete val;
     *  </pre>
     */
    public: Value* getFieldByName (const string &name) const;

    /** Sets the value of a field, based on the field's name. This will accept
     *  array entries and sub-fields similar to {@link #getFieldByName}.
     *  @param name The name of the field (see {@link #getFieldName}).
     *  @param val  Pointer to the given value (must be of the applicable type).
     */
    public: void setFieldByName (const string &name, const Value* val);

    /** Sets the value of a field, based on the field's name. This will accept
     *  array entries and sub-fields similar to {@link #getFieldByName}.
     *  @param name The name of the field (see {@link #getFieldName}).
     *  @param val  The given value (must be of the applicable type).
     */
    public: inline void setFieldByName (const string &name, const Value &val) {
      setFieldByName(name, &val);
    }
  };
  
  /** Checks to see if a {@link HasFields} pointer is null. This is the same as 
   *  calling <tt>isNull(val)</tt> following a conversion to <tt>VRTObject*</tt>.
   */
  inline bool isNull (const HasFields *val) {
    return isNull(checked_dynamic_cast<const VRTObject*>(val));
  }
}

#endif /* _HasFields_h */

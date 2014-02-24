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

/** Interface indicating that an object has queriable fields. This is intended
 *  for use in automated processing where the names of the fields that need to
 *  be accessed aren't known at compile-time.
 *  @author  / 
 */
public interface HasFields {
  /** Gets the number of fields. */
  public int getFieldCount ();

  /** Gets the name of the given field. In 90% of cases the field name will be
   *  identical to the name used in the applicable get/set function except
   *  without the get/set prefix (e.g. "SourceContext" for "getSourceContext").
   *  It is possible to access the names of all fields via the following:
   *  <pre>
   *    for (int id = 0; id &lt; getFieldCount(); id++) {
   *      // id is the field ID
   *      getFieldName(id);
   *    }
   *  </pre>
   *  In situations where it is desirable to access all available fields, it
   *  is faster to use the field ID as this eliminates any string comparisons.
   *  @param id The field ID (on range <tt>[0,getFieldCount())</tt>).
   *  @return   The field name.
   *  @throws IllegalArgumentException if the field id is invalid.
   */
  public String getFieldName (int id);

  /** Gets the type associated with a given field. <br>
   *  <br>
   *  For fields that hold primitive types this may return the "type" (e.g.
   *  <tt>Integer.TYPE</tt>) or the wrapper object (e.g. <tt>Integer.class</tt>).
   *  For fields that hold an array (or list) of values, this will return the
   *  class of array used (e.g. <tt>int[].class</tt> or <tt>Integer[].class</tt>).
   *  @param id The field ID (on range <tt>[0,getFieldCount())</tt>).
   *  @return The field type (n &gt; 0) or -N if the field is a vector
   *          of values where N is the component type of the vector.
   */
  public Class<?> getFieldType (int id);

  /** Gets the value of a field, based on the field's id. <br>
   *  <br>
   *  Users of this method should consider using {@link Number} and
   *  {@link CharSequence} as intermediaries rather than assuming that a
   *  particular field will be of a given numeric or string type.
   *  @param id The field ID (on range <tt>[0,getFieldCount())</tt>).
   *  @return Pointer to the given value.
   */
  public Object getField (int id);

  /** Sets the value of a field, based on the field's id. <br>
   *  <br>
   *  The Java version of this method should use {@link Utilities#castValue} to
   *  convert the value passed in to the applicable type as this will support
   *  automatic conversion to various numeric and string types.
   *  @param id  The field ID (on range <tt>[0,getFieldCount())</tt>).
   *  @param val Pointer to the given value (must be of the applicable type).
   */
  public void setField (int id, Object val);

  /** Gets the ID of the given field. There is a 1:1 mapping of field ID's and
   *  names such that for all <tt>n = [0,getFieldCount())</tt>,
   *  <tt>n == getFieldID(getFieldName(n))</tt> is true. <br>
   *  <br>
   *  In Java this must be implemented as:
   *  <pre>
   *    public final int getFieldID (String name) {
   *      return Utilities.getFieldID(this, name);
   *    }
   *  </pre>
   *  @param name The field name.
   *  @return     The field ID (on range <tt>[0,getFieldCount())</tt>).
   *  @throws IllegalArgumentException if the field name is invalid.
   *  @throws NullPointerException     if the field name is null.
   */
  public int getFieldID (String name);

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
   *  is null without checking the validity of the lower entries. <br>
   *  <br>
   *  In Java this must be implemented as:
   *  <pre>
   *    public final Object getFieldByName (String name) {
   *      return Utilities.getFieldByName(this, name);
   *    }
   *  </pre>
   *  @param name The name of the field (see {@link #getFieldName}).
   *  @return The given value (see {@link #getField} for details); may be null.
   *  @throws IllegalArgumentException if the field name is invalid.
   *  @throws NullPointerException     if the field name is null.
   */
  public Object getFieldByName (String name);

  /** Sets the value of a field, based on the field's name. This will accept
   *  array entries and sub-fields similar to {@link #getFieldByName}. <br>
   *  <br>
   *  In Java this must be implemented as:
   *  <pre>
   *    public final void setFieldByName (String name, Object val) {
   *      Utilities.setFieldByName(this, name, val);
   *    }
   *  </pre>
   *  @param name The name of the field (see {@link #getFieldName}).
   *  @param val  The given value (must be of the applicable type).
   *  @throws IllegalArgumentException if the field name is invalid or the value
   *                                   is of the wrong type.
   *  @throws NullPointerException     if the field name is null.
   */
  public void setFieldByName (String name, Object val);
}

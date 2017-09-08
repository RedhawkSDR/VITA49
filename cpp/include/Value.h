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

#ifndef _Value_h
#define _Value_h

#include "VRTObject.h"
#include <map>
#include <vector>

using namespace std;

namespace vrt {
  /** The type used with the {@link Value} methods. Note that the numeric
   *  types names are consistent with <tt>DataType_*</tt> where possible. The
   *  other types are those commonly used in context packets. Values that are
   *  arrays are indicated by using -N where N is the type of the enclosed
   *  type.<br>
   *  <br>
   *  In nearly all cases a value of type <tt>ValueType_VRTObject</tt> will
   *  also implement {@link HasFields}.
   */
  typedef enum {
    /**  8-bit signed integer.                     */  ValueType_Int8        =  1,
    /** 16-bit signed integer.                     */  ValueType_Int16       =  2,
    /** 32-bit signed integer.                     */  ValueType_Int32       =  3,
    /** 64-bit signed integer.                     */  ValueType_Int64       =  4,
    /** 32-bit single-precision floating-point.    */  ValueType_Float       =  5,
    /** 64-bit double-precision floating-point.    */  ValueType_Double      =  6,
    /** bool       value.                          */  ValueType_Bool        =  7,
    /** boolNull   value.                          */  ValueType_BoolNull    =  8,
    /** string*    object (ASCII).                 */  ValueType_String      =  9,
    /** wstring*   object (UTF).                   */  ValueType_WString     = 10,
    /** VRTObject* object (usually a HasFields).   */  ValueType_VRTObject   = 11
  } ValueType;

  class HasFields;

  /** A generic value used in conjunction with {@link HasFields}. For each of
   *  the constructors that takes in a pointer, there is a flag indicating if
   *  the new {@link Value} is the "owner" and responsible for deleting any
   *  underlying pointer references. <br>
   *  <br>
   *  Due to memory-management issues, there is no copy constructor for this
   *  class.
   */
  class Value : public VRTObject {
    public: static size_t npos;

    private: char type;   // Data type indicator.
    private: bool owner;  // Responsible for deleting memory (true) or not (false)?
    private: union {
      int8_t     bval;    vector<int8_t    > *barray;
      int16_t    ival;    vector<int16_t   > *iarray;
      int32_t    lval;    vector<int32_t   > *larray;
      int64_t    xval;    vector<int64_t   > *xarray;
      float      fval;    vector<float     > *farray;
      double     dval;    vector<double    > *darray;
      bool       zval;    vector<bool      > *zarray;
      boolNull   qval;    vector<boolNull  > *qarray;
      string    *aval;    vector<string*   > *aarray;
      wstring   *sval;    vector<wstring*  > *sarray;
      VRTObject *oval;    vector<VRTObject*> *oarray;
      Value     *vval;    vector<Value*    > *varray;
    } value;

    /** Basic destructor. */
    public: ~Value ();

    /** Basic no-argument constructor. */
    public: Value ();

    /** Creates value as an <tt>int8_t</tt>. */
    public: explicit Value (int8_t val);

    /** Creates value as an <tt>int16_t</tt>. */
    public: explicit Value (int16_t val);

    /** Creates value as an <tt>int32_t</tt>. */
    public: explicit Value (int32_t val);

    /** Creates value as an <tt>int64_t</tt>. */
    public: explicit Value (int64_t val);

    /** Creates value as a <tt>float</tt>. */
    public: explicit Value (float val);

    /** Creates value as a <tt>double</tt>. */
    public: explicit Value (double val);

    /** Creates value as a <tt>bool</tt>. */
    public: explicit Value (bool val);

    /** Creates value as a <tt>boolNull</tt>. */
    public: explicit Value (boolNull val);

    /** Creates value as a <tt>string*</tt> that is a copy of the one passed in. */
    public: explicit Value (const string &val);

    /** Creates value as a <tt>wstring*</tt> that is a copy of the one passed in. */
    public: explicit Value (const wstring &val);

    /** Creates value as a <tt>string*</tt> that is a copy of the one passed in. */
    public: explicit Value (const char *val);

    /** Creates value as a <tt>wstring*</tt> that is a copy of the one passed in. */
    public: explicit Value (const wchar_t *val);

    /** <b>Internal Use Only:</b> Creates value as a <tt>HasFields*</tt>object
     *  that maps the keys/values passed in.
     */
    public: explicit Value (map<string,Value*> &val, bool owner=false);

    /** <b>Internal Use Only:</b> Creates value that wraps another value. This
     *  is used in a few rare situations where we need to restrict deletion of
     *  the given pointer (i.e. user can delete the new Value but not the one
     *  passed in).
     */
    public: explicit Value (Value* val);

    /** Creates value as a <tt>string*</tt>. */
    public: explicit Value (string *val, bool owner=false);

    /** Creates value as a <tt>wstring*</tt>. */
    public: explicit Value (wstring *val, bool owner=false);

    /** Creates value as a <tt>VRTObject*</tt>. */
    public: explicit Value (VRTObject *val, bool owner=false);

    /** Creates value as a vector of <tt>int8_t</tt>. */
    public: explicit Value (vector<int8_t> *val, bool owner=false);

    /** Creates value as a vector of <tt>int16_t</tt>. */
    public: explicit Value (vector<int16_t> *val, bool owner=false);

    /** Creates value as a vector of <tt>int32_t</tt>. */
    public: explicit Value (vector<int32_t> *val, bool owner=false);

    /** Creates value as a vector of <tt>int64_t</tt>. */
    public: explicit Value (vector<int64_t> *val, bool owner=false);

    /** Creates value as a vector of <tt>float</tt>. */
    public: explicit Value (vector<float> *val, bool owner=false);

    /** Creates value as a vector of <tt>double</tt>. */
    public: explicit Value (vector<double> *val, bool owner=false);

    /** Creates value as a vector of <tt>bool</tt>. */
    public: explicit Value (vector<bool> *val, bool owner=false);

    /** Creates value as a vector of <tt>boolNull</tt>. */
    public: explicit Value (vector<boolNull> *val, bool owner=false);

    /** Creates value as a vector of <tt>string</tt>. */
    public: explicit Value (vector<string*> *val, bool owner=false);

    /** Creates value as a vector of <tt>wstring</tt>. */
    public: explicit Value (vector<wstring*> *val, bool owner=false);

    /** Creates value as a vector of <tt>VRTObject*</tt>. */
    public: explicit Value (vector<VRTObject*> *val, bool owner=false);

    /** <b>Internal Use Only:</b> Creates value that wraps an array of values.
     *  Generally use of the version that takes an array of <tt>VRTObject*</tt>
     *  is preferred.
     */
    public: Value (vector<Value*> *val, bool owner=false);

    /** Initializer. */
    private: void init ();

    /** Converts this class its string form. */
    public: virtual string toString () const;

    /** Tests this object for equality with another one. */
    public: virtual inline bool equals (const VRTObject &o) const {
      try {
        return _equals(checked_dynamic_cast<const Value*>(&o));
      }
      catch (bad_cast &e) {
        UNUSED_VARIABLE(e);
        return false;
      }
    }

    /** Tests this object for equality with another one. */
    public: virtual inline bool equals (const VRTObject *o) const {
      try {
        return _equals(checked_dynamic_cast<const Value*>(o));
      }
      catch (bad_cast &e) {
        UNUSED_VARIABLE(e);
        return false;
      }
    }

    /** Tests this object for equality with another one. */
    private: virtual bool _equals (const Value *o) const;

    /** Is this object equal to null. */
    public: virtual bool isNullValue () const;

    /** Gets value as an <tt>int8_t</tt> making any applicable conversions, required.*/
    public: operator int8_t () const;
    public: operator int8_t ();

    /** Gets value as an <tt>int16_t</tt> making any applicable conversions, required.*/
    public: operator int16_t () const;
    public: operator int16_t ();

    /** Gets value as an <tt>int32_t</tt> making any applicable conversions, required.*/
    public: operator int32_t () const;
    public: operator int32_t ();

    /** Gets value as an <tt>int64_t</tt> making any applicable conversions, required.*/
    public: operator int64_t () const;
    public: operator int64_t ();

    /** Gets value as an <tt>float</tt> making any applicable conversions, required.*/
    public: operator float () const;
    public: operator float ();

    /** Gets value as an <tt>double</tt> making any applicable conversions, required.*/
    public: operator double () const;
    public: operator double ();

    /** Gets value as an <tt>bool</tt> making any applicable conversions, required.*/
    public: operator bool () const;
    public: operator bool ();

    /** Gets value as an <tt>boolNull</tt> making any applicable conversions, required.*/
    public: operator boolNull () const;
    public: operator boolNull ();

    /** Gets value as an <tt>string</tt>, same as calling {@link #toString}. */
    public: operator string () const;
    public: operator string ();

    /** Gets value as a <tt>wstring</tt>. */
    public: operator wstring () const;
    public: operator wstring ();

    /** Gets value as an <tt>string*</tt>, returns NULL if not a string. */
    public: operator const string* () const;

    /** Gets value as an <tt>wstring*</tt>, returns NULL if not a wstring. */
    public: operator const wstring* () const;

    /** Gets value as an <tt>VRTObject*</tt>, returns NULL if not a VRTObject. */
    public: operator const VRTObject* () const;

    /** Gets value as an <tt>HasFields*</tt>, returns NULL if not a VRTObject. */
    public: operator const HasFields* () const;

    /** Gets value as an <tt>string*</tt>, returns NULL if not a string. */
    public: operator string* ();

    /** Gets value as an <tt>wstring*</tt>, returns NULL if not a wstring. */
    public: operator wstring* ();

    /** Gets value as an <tt>VRTObject*</tt>, returns NULL if not a VRTObject. */
    public: operator VRTObject* ();

    /** Gets value as an <tt>HasFields*</tt>, returns NULL if not a VRTObject. */
    public: operator HasFields* ();

    /** Converts value to pointer. */
    public: inline operator const Value* () const {
      return this;
    }

    /** Converts value to pointer. */
    public: inline operator Value* () {
      return this;
    }

    /** Casts to the given <tt>VRTObject*</tt> type. This is identical to using:
     *  <tt>*checked_dynamic_cast&lt;T&gt;(as&lt;VRTObject*&gt;())</tt>. <br>
     *  <br>
     *  <b>Note that the type used here MUST be a subclass of {@link VRTObject}
     *  in pointer form.</b>
     */
    public: template <typename T> T cast () {
      return checked_dynamic_cast<T>(as<VRTObject*>());
    }
    public: template <typename T> T cast () const {
      return checked_dynamic_cast<T>(_as<VRTObject*>());
    }

    /** Casts to the given type. This will make it more likely to get a compile
     *  error rather than a core dump in cases like the error shown below:
     *  <pre>
     *    Value     *val = ...;
     *    HasFields *hf  = NULL;
     *
     *    hf = (HasFields*)val;           // **ERROR**
     *    hf = (HasFields*)*val;          // **OK**
     *    hf = val->as<HasFields*>();     // **OK**
     *
     *    hf = val->as<TimeStamp*>();     // **ERROR**
     *    hf = val->cast<TimeStamp*>();   // **OK**
     *    hf = val->cast<double*>();      // **ERROR**
     *  </pre>
     */
    public: template <typename T> T as () {
      return (T)(*this);
    }
    public: template <typename T> T as () const {
      return _as<T>();
    }

    /** Internal Use Only: version of as() that strips the 'const' off. */
    private: template <typename T> T _as () const {
      return const_cast<Value*>(this)->as<T>();
    }

    /** Gets the size of the internal vector (if a vector) otherwise returns <tt>Value::npos</tt>. */
    public: size_t size () const;

    /** Gets an entry from the internal vector (if a vector) otherwise returns a null value. */
    public: Value* at (size_t i) const __attribute__((warn_unused_result));

    /** Gets an entry from the internal vector (if a vector) otherwise returns a null value. */
    public: inline Value* operator [] (size_t i) const __attribute__((warn_unused_result)) {
      return at(i);
    }

    /** Gets the value's type. */
    public: ValueType getType ();
  };
} END_NAMESPACE

/** Supports appending a {@link ValueType} value to a string. */
string operator+ (string &s, vrt::ValueType val);

/** Supports writing a {@link ValueType} value to a stream. */
ostream& operator<< (ostream &s, vrt::ValueType val);

#endif /* _Value_h */

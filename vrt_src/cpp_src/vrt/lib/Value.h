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

#ifndef _Value_h
#define _Value_h

#include "VRTObject.h"
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
    /** VRTObject* object (usually a HasFields).   */  ValueType_VRTObject   = 11,
  } ValueType;
  
  class HasFields;
  
  /** A generic value used in conjunction with {@link HasFields}. For each of
   *  the constructors that takes in a pointer, there is a flag indicating if
   *  the new {@link Value} is the "owner" and responsible for deleting any
   *  underlying pointer references. <br>
   *  <br>
   *  Due to memory-management issues, there is no copy constructor for this
   *  class.
   * 
   *  @author 
   */
  class Value : public VRTObject {
    public: static size_t npos;
    
    private: char type;   // Data type indicator.
    private: bool owner;  // Responsible for deleting memory (true) or not (false)?
    private: union {
      void      *ptr;
      int8_t     bval;
      int16_t    ival;
      int32_t    lval;
      int64_t    xval;
      float      fval;
      double     dval;
      bool       zval;
      boolNull   qval;
    } value;
    
    /** Basic no-argument constructor. */
    public: Value ();
    
    /** Basic destructor. */
    public: virtual ~Value() { free(); }

    /** Converts this class its string form. */
    public: virtual string toString () const;

    /** Tests this object for equality with another one. */
    public: virtual inline bool equals (const VRTObject &o) const {
      try {
        return equals(*checked_dynamic_cast<const Value*>(&o));
      }
      catch (bad_cast &e) {
        return false;
      }
    }
    
    /** Tests this object for equality with another one. */
    public: virtual bool equals (const Value &o) const;

    /** Is this object equal to null. */
    public: virtual bool isNullValue () const;
    
    /** Creates value as an <tt>int8_t</tt>. */
    public: Value (int8_t val);
    
    /** Creates value as an <tt>int16_t</tt>. */
    public: Value (int16_t val);
    
    /** Creates value as an <tt>int32_t</tt>. */
    public: Value (int32_t val);
    
    /** Creates value as an <tt>int64_t</tt>. */
    public: Value (int64_t val);
    
    /** Creates value as a <tt>float</tt>. */
    public: Value (float val);
    
    /** Creates value as a <tt>double</tt>. */
    public: Value (double val);
    
    /** Creates value as a <tt>bool</tt>. */
    public: Value (bool val);
    
    /** Creates value as a <tt>boolNull</tt>. */
    public: Value (boolNull val);
    
    /** Creates value as a <tt>string*</tt> that is a copy of the one passed in. */
    public: Value (const string &val);
    
    /** Creates value as a <tt>wstring*</tt> that is a copy of the one passed in. */
    public: Value (const wstring &val);
    
    /** Creates value as a <tt>string*</tt> that is a copy of the one passed in. */
    public: Value (const char *val);
    
    /** Creates value as a <tt>wstring*</tt> that is a copy of the one passed in. */
    public: Value (const wchar_t *val);
    
    /** Creates value as a <tt>string*</tt>. */
    public: Value (string *val, bool owner=false);
    
    /** Creates value as a <tt>wstring*</tt>. */
    public: Value (wstring *val, bool owner=false);
    
    /** Creates value as a <tt>VRTObject*</tt>. */
    public: Value (VRTObject *val, bool owner=false);
    
    /** Creates value as a vector of <tt>int8_t</tt>. */
    public: Value (vector<int8_t> *val, bool owner=false);
    
    /** Creates value as a vector of <tt>int16_t</tt>. */
    public: Value (vector<int16_t> *val, bool owner=false);
    
    /** Creates value as a vector of <tt>int32_t</tt>. */
    public: Value (vector<int32_t> *val, bool owner=false);
    
    /** Creates value as a vector of <tt>int64_t</tt>. */
    public: Value (vector<int64_t> *val, bool owner=false);
    
    /** Creates value as a vector of <tt>float</tt>. */
    public: Value (vector<float> *val, bool owner=false);
    
    /** Creates value as a vector of <tt>double</tt>. */
    public: Value (vector<double> *val, bool owner=false);
    
    /** Creates value as a vector of <tt>bool</tt>. */
    public: Value (vector<bool> *val, bool owner=false);
    
    /** Creates value as a vector of <tt>boolNull</tt>. */
    public: Value (vector<boolNull> *val, bool owner=false);
    
    /** Creates value as a vector of <tt>string</tt>. */
    public: Value (vector<string*> *val, bool owner=false);
    
    /** Creates value as a vector of <tt>wstring</tt>. */
    public: Value (vector<wstring*> *val, bool owner=false);
    
    /** Creates value as a vector of <tt>VRTObject*</tt>. */
    public: Value (vector<VRTObject*> *val, bool owner=false);
    
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
      return (T)(*const_cast<Value*>(this));
    }
    
    /** Gets the size of the internal vector (if a vector) otherwise returns <tt>Value::npos</tt>. */
    public: size_t size () const;

    /** Gets an entry from the internal vector (if a vector) otherwise returns a null value. */
    public: Value* at (size_t i) const;
    
    /** Gets an entry from the internal vector (if a vector) otherwise returns a null value. */
    public: inline Value* operator [] (size_t i) const {
      return at(i);
    }
    
    /** Free any memory held by this value. */
    protected: void free ();
    
    /** Gets the value's type. */
    public: ValueType getType ();
  };
};

/** Supports appending a {@link ValueType} value to a string. */
string operator+ (string &s, vrt::ValueType val);

/** Supports writing a {@link ValueType} value to a stream. */
ostream& operator<< (ostream &s, vrt::ValueType val);

#endif /* _Value_h */

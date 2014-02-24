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

#include "Value.h"
#include "HasFields.h"
#include <cstring>
#include <sstream>
#include <iostream>
#include <map>
#include <string>

using namespace vrt;

size_t Value::npos = string::npos;

Value::Value (                               ) : type('o'), owner(false) { value.ptr  = NULL; }
Value::Value (int8_t              val        ) : type('B'), owner(false) { value.bval = val; }
Value::Value (int16_t             val        ) : type('I'), owner(false) { value.ival = val; }
Value::Value (int32_t             val        ) : type('L'), owner(false) { value.lval = val; }
Value::Value (int64_t             val        ) : type('X'), owner(false) { value.xval = val; }
Value::Value (float               val        ) : type('F'), owner(false) { value.fval = val; }
Value::Value (double              val        ) : type('D'), owner(false) { value.dval = val; }
Value::Value (bool                val        ) : type('Z'), owner(false) { value.zval = val; }
Value::Value (boolNull            val        ) : type('Q'), owner(false) { value.qval = val; }
Value::Value (const string       &val        ) : type('A'), owner(true)  { value.ptr  = new string(val);  }
Value::Value (const wstring      &val        ) : type('S'), owner(true)  { value.ptr  = new wstring(val); }
Value::Value (const char         *val        ) : type('A'), owner(true)  { value.ptr  = new string(val);  }
Value::Value (const wchar_t      *val        ) : type('S'), owner(true)  { value.ptr  = new wstring(val); }
Value::Value (string             *val, bool o) : type('A'), owner(o)     { value.ptr  = val; }
Value::Value (wstring            *val, bool o) : type('S'), owner(o)     { value.ptr  = val; }
Value::Value (VRTObject          *val, bool o) : type('O'), owner(o)     { value.ptr  = val; }
Value::Value (vector<int8_t   >  *val, bool o) : type('b'), owner(o)     { value.ptr  = val; }
Value::Value (vector<int16_t  >  *val, bool o) : type('i'), owner(o)     { value.ptr  = val; }
Value::Value (vector<int32_t  >  *val, bool o) : type('l'), owner(o)     { value.ptr  = val; }
Value::Value (vector<int64_t  >  *val, bool o) : type('x'), owner(o)     { value.ptr  = val; }
Value::Value (vector<float    >  *val, bool o) : type('f'), owner(o)     { value.ptr  = val; }
Value::Value (vector<double   >  *val, bool o) : type('d'), owner(o)     { value.ptr  = val; }
Value::Value (vector<bool     >  *val, bool o) : type('z'), owner(o)     { value.ptr  = val; }
Value::Value (vector<boolNull >  *val, bool o) : type('q'), owner(o)     { value.ptr  = val; }
Value::Value (vector<string*  >  *val, bool o) : type('a'), owner(o)     { value.ptr  = val; }
Value::Value (vector<wstring* >  *val, bool o) : type('s'), owner(o)     { value.ptr  = val; }
Value::Value (vector<VRTObject*> *val, bool o) : type('o'), owner(o)     { value.ptr  = val; }

Value::operator int8_t () const {
  if (isNullValue()) return INT8_NULL;
  switch (type) {
    case 'B': return (int8_t)value.bval;
    case 'I': return (int8_t)value.ival;
    case 'L': return (int8_t)value.lval;
    case 'X': return (int8_t)value.xval;
    case 'F': return (int8_t)value.fval;
    case 'D': return (int8_t)value.dval;
    case 'Z': return (int8_t)((value.zval    )? 1 : 0);
    case 'Q': return (int8_t)((value.qval > 0)? 1 : 0);
    default:  return INT8_NULL;
  }
}

Value::operator int8_t () {
  if (isNullValue()) return INT8_NULL;
  switch (type) {
    case 'B': return (int8_t)value.bval;
    case 'I': return (int8_t)value.ival;
    case 'L': return (int8_t)value.lval;
    case 'X': return (int8_t)value.xval;
    case 'F': return (int8_t)value.fval;
    case 'D': return (int8_t)value.dval;
    case 'Z': return (int8_t)((value.zval    )? 1 : 0);
    case 'Q': return (int8_t)((value.qval > 0)? 1 : 0);
    default:  return INT8_NULL;
  }
}

Value::operator int16_t () const {
  if (isNullValue()) return INT16_NULL;
  switch (type) {
    case 'B': return (int16_t)value.bval;
    case 'I': return (int16_t)value.ival;
    case 'L': return (int16_t)value.lval;
    case 'X': return (int16_t)value.xval;
    case 'F': return (int16_t)value.fval;
    case 'D': return (int16_t)value.dval;
    case 'Z': return (int16_t)((value.zval    )? 1 : 0);
    case 'Q': return (int16_t)((value.qval > 0)? 1 : 0);
    default:  return INT16_NULL;
  }
}

Value::operator int16_t () {
  if (isNullValue()) return INT16_NULL;
  switch (type) {
    case 'B': return (int16_t)value.bval;
    case 'I': return (int16_t)value.ival;
    case 'L': return (int16_t)value.lval;
    case 'X': return (int16_t)value.xval;
    case 'F': return (int16_t)value.fval;
    case 'D': return (int16_t)value.dval;
    case 'Z': return (int16_t)((value.zval    )? 1 : 0);
    case 'Q': return (int16_t)((value.qval > 0)? 1 : 0);
    default:  return INT16_NULL;
  }
}

Value::operator int32_t () const {
  if (isNullValue()) return INT32_NULL;
  switch (type) {
    case 'B': return (int32_t)value.bval;
    case 'I': return (int32_t)value.ival;
    case 'L': return (int32_t)value.lval;
    case 'X': return (int32_t)value.xval;
    case 'F': return (int32_t)value.fval;
    case 'D': return (int32_t)value.dval;
    case 'Z': return (int32_t)((value.zval    )? 1 : 0);
    case 'Q': return (int32_t)((value.qval > 0)? 1 : 0);
    default:  return INT32_NULL;
  }
}

Value::operator int32_t () {
  if (isNullValue()) return INT32_NULL;
  switch (type) {
    case 'B': return (int32_t)value.bval;
    case 'I': return (int32_t)value.ival;
    case 'L': return (int32_t)value.lval;
    case 'X': return (int32_t)value.xval;
    case 'F': return (int32_t)value.fval;
    case 'D': return (int32_t)value.dval;
    case 'Z': return (int32_t)((value.zval    )? 1 : 0);
    case 'Q': return (int32_t)((value.qval > 0)? 1 : 0);
    default:  return INT32_NULL;
  }
}

Value::operator int64_t () const {
  if (isNullValue()) return INT64_NULL;
  switch (type) {
    case 'B': return (int64_t)value.bval;
    case 'I': return (int64_t)value.ival;
    case 'L': return (int64_t)value.lval;
    case 'X': return (int64_t)value.xval;
    case 'F': return (int64_t)value.fval;
    case 'D': return (int64_t)value.dval;
    case 'Z': return (int64_t)((value.zval    )? 1 : 0);
    case 'Q': return (int64_t)((value.qval > 0)? 1 : 0);
    default:  return INT64_NULL;
  }
}

Value::operator int64_t () {
  if (isNullValue()) return INT64_NULL;
  switch (type) {
    case 'B': return (int64_t)value.bval;
    case 'I': return (int64_t)value.ival;
    case 'L': return (int64_t)value.lval;
    case 'X': return (int64_t)value.xval;
    case 'F': return (int64_t)value.fval;
    case 'D': return (int64_t)value.dval;
    case 'Z': return (int64_t)((value.zval    )? 1 : 0);
    case 'Q': return (int64_t)((value.qval > 0)? 1 : 0);
    default:  return INT64_NULL;
  }
}

Value::operator float () const {
  if (isNullValue()) return FLOAT_NAN;
  switch (type) {
    case 'B': return (float)value.bval;
    case 'I': return (float)value.ival;
    case 'L': return (float)value.lval;
    case 'X': return (float)value.xval;
    case 'F': return (float)value.fval;
    case 'D': return (float)value.dval;
    case 'Z': return (float)((value.zval    )? 1 : 0);
    case 'Q': return (float)((value.qval > 0)? 1 : 0);
    default:  return FLOAT_NAN;
  }
}

Value::operator float () {
  if (isNullValue()) return FLOAT_NAN;
  switch (type) {
    case 'B': return (float)value.bval;
    case 'I': return (float)value.ival;
    case 'L': return (float)value.lval;
    case 'X': return (float)value.xval;
    case 'F': return (float)value.fval;
    case 'D': return (float)value.dval;
    case 'Z': return (float)((value.zval    )? 1 : 0);
    case 'Q': return (float)((value.qval > 0)? 1 : 0);
    default:  return FLOAT_NAN;
  }
}

Value::operator double () const {
  if (isNullValue()) return DOUBLE_NAN;
  switch (type) {
    case 'B': return (double)value.bval;
    case 'I': return (double)value.ival;
    case 'L': return (double)value.lval;
    case 'X': return (double)value.xval;
    case 'F': return (double)value.fval;
    case 'D': return (double)value.dval;
    case 'Z': return (double)((value.zval    )? 1 : 0);
    case 'Q': return (double)((value.qval > 0)? 1 : 0);
    default:  return DOUBLE_NAN;
  }
}

Value::operator double () {
  if (isNullValue()) return DOUBLE_NAN;
  switch (type) {
    case 'B': return (double)value.bval;
    case 'I': return (double)value.ival;
    case 'L': return (double)value.lval;
    case 'X': return (double)value.xval;
    case 'F': return (double)value.fval;
    case 'D': return (double)value.dval;
    case 'Z': return (double)((value.zval    )? 1 : 0);
    case 'Q': return (double)((value.qval > 0)? 1 : 0);
    default:  return DOUBLE_NAN;
  }
}

Value::operator bool () const {
  if (isNullValue()) return false;
  switch (type) {
    case 'B': return (value.bval == 0)? false : true;
    case 'I': return (value.ival == 0)? false : true;
    case 'L': return (value.lval == 0)? false : true;
    case 'X': return (value.xval == 0)? false : true;
    case 'F': return (value.fval == 0)? false : true;
    case 'D': return (value.dval == 0)? false : true;
    case 'Z': return (value.zval == 0)? false : true;
    case 'Q': return (value.qval <= 0)? false : true;
    default:  return false;
  }
}

Value::operator bool () {
  if (isNullValue()) return false;
  switch (type) {
    case 'B': return (value.bval == 0)? false : true;
    case 'I': return (value.ival == 0)? false : true;
    case 'L': return (value.lval == 0)? false : true;
    case 'X': return (value.xval == 0)? false : true;
    case 'F': return (value.fval == 0)? false : true;
    case 'D': return (value.dval == 0)? false : true;
    case 'Z': return (value.zval == 0)? false : true;
    case 'Q': return (value.qval <= 0)? false : true;
    default:  return false;
  }
}

Value::operator boolNull () const {
  if (isNullValue()) return _NULL;
  switch (type) {
    case 'B': return (value.bval == 0)? _FALSE : _TRUE;
    case 'I': return (value.ival == 0)? _FALSE : _TRUE;
    case 'L': return (value.lval == 0)? _FALSE : _TRUE;
    case 'X': return (value.xval == 0)? _FALSE : _TRUE;
    case 'F': return (value.fval == 0)? _FALSE : _TRUE;
    case 'D': return (value.dval == 0)? _FALSE : _TRUE;
    case 'Z': return (value.zval == 0)? _FALSE : _TRUE;
    case 'Q': return (value.qval <= 0)? _FALSE : _TRUE;
    default:  return _NULL;
  }
}

Value::operator boolNull () {
  if (isNullValue()) return _NULL;
  switch (type) {
    case 'B': return (value.bval == 0)? _FALSE : _TRUE;
    case 'I': return (value.ival == 0)? _FALSE : _TRUE;
    case 'L': return (value.lval == 0)? _FALSE : _TRUE;
    case 'X': return (value.xval == 0)? _FALSE : _TRUE;
    case 'F': return (value.fval == 0)? _FALSE : _TRUE;
    case 'D': return (value.dval == 0)? _FALSE : _TRUE;
    case 'Z': return (value.zval == 0)? _FALSE : _TRUE;
    case 'Q': return (value.qval <= 0)? _FALSE : _TRUE;
    default:  return _NULL;
  }
}

Value::operator string () const {
  return toString();
}

Value::operator string () {
  return toString();
}

Value::operator wstring () const {
  Value   *self = const_cast<Value*>(this);  
  wstring *ptr  = (isNullValue())? NULL : self->as<wstring*>();
  wstring  str;
  
  if (ptr != NULL) {
    str = *ptr;
  }
  else {
    string s = toString();
    for (size_t i = 0; i < s.size(); i++) {
      str += (wchar_t)s[i];
    }
  }
  return str;
}

Value::operator wstring () {
  wstring *ptr = (isNullValue())? NULL : this->as<wstring*>();
  wstring  str;
  
  if (ptr != NULL) {
    str = *ptr;
  }
  else {
    string s = toString();
    for (size_t i = 0; i < s.size(); i++) {
      str += (wchar_t)s[i];
    }
  }
  return str;
}

Value::operator const string* () const {
  return const_cast<const string*>(_as<string*>());
}

Value::operator const wstring* () const {
  return const_cast<const wstring*>(_as<wstring*>());
}

Value::operator const VRTObject* () const {
  return const_cast<const VRTObject*>(_as<VRTObject*>());
}

Value::operator const HasFields* () const {
  return const_cast<const HasFields*>(_as<HasFields*>());
}

Value::operator string* () {
  return (type == 'A')? ((string*)value.ptr) : NULL;
}

Value::operator wstring* () {
  return (type == 'S')? ((wstring*)value.ptr) : NULL;
}

Value::operator VRTObject* () {
  return (type == 'O')? ((VRTObject*)value.ptr) : NULL;
}

Value::operator HasFields* () {
  return checked_dynamic_cast<HasFields*>(as<VRTObject*>());
}

string Value::toString () const {
  if (isNullValue()) return "null";
  ostringstream str;
  if (size() == npos) {
    switch (type) {
      case 'B': str << value.bval; break;
      case 'I': str << value.ival; break;
      case 'L': str << value.lval; break;
      case 'X': str << value.xval; break;
      case 'F': str << value.fval; break;
      case 'D': str << value.dval; break;
      case 'Z': str << value.zval; break;
      case 'Q': str << value.qval; break;
      case 'A': str << *((string*   )value.ptr); break;
      case 'S': str << *((wstring*  )value.ptr); break;
      case 'O': str << *((VRTObject*)value.ptr); break;
    }
  }
  else {
    str << "[ ";
    for (size_t i = 0; i < size(); i++) {
      if (i > 0) str << ", ";
      Value* v = at(i);
      str << *v;
      delete v;
    }
    str << " ]";
  }
  return str.str();
}

bool Value::equals (const Value &v) const {
  if (type != v.type) return false;
  
  switch (type) {
    case 'B': return (value.bval == v.value.bval);
    case 'I': return (value.ival == v.value.ival);
    case 'L': return (value.lval == v.value.lval);
    case 'X': return (value.xval == v.value.xval);
    case 'F': return (value.fval == v.value.fval);
    case 'D': return (value.dval == v.value.dval);
    case 'Z': return (value.zval == v.value.zval);
    case 'Q': return (value.qval == v.value.qval);
    case 'A': return (*((string*   )value.ptr)) == (*((string*   )v.value.ptr));
    case 'S': return (*((wstring*  )value.ptr)) == (*((wstring*  )v.value.ptr));
    case 'O': return (*((VRTObject*)value.ptr)) == (*((VRTObject*)v.value.ptr));
  }
  
  if (size() != v.size()) return false;
  for (size_t i = 0; i < size(); i++) {
    Value* a = at(i);
    Value* b = v.at(i);
    bool   ok = a == b;
    delete a;
    delete b;
    if (!ok) return false;
  }
  return true;
}

bool Value::isNullValue () const  {
  switch (type) {
    case 'B': return isNull(value.bval);
    case 'I': return isNull(value.ival);
    case 'L': return isNull(value.lval);
    case 'X': return isNull(value.xval);
    case 'F': return isNull(value.fval);
    case 'D': return isNull(value.dval);
    case 'Z': return isNull(value.zval);
    case 'Q': return isNull(value.qval);
    case 'A': return (value.ptr == NULL) || (((string*   )value.ptr)->size() == 0);
    case 'S': return (value.ptr == NULL) || (((wstring*  )value.ptr)->size() == 0);
    case 'O': return (value.ptr == NULL) || (((VRTObject*)value.ptr)->isNullValue());
    default:  return (size() == 0);
  }
}

size_t Value::size () const {
  switch (type) {
    case 'b': return (value.ptr == NULL)? 0 : ((vector<int8_t    >*)value.ptr)->size();
    case 'i': return (value.ptr == NULL)? 0 : ((vector<int16_t   >*)value.ptr)->size();
    case 'l': return (value.ptr == NULL)? 0 : ((vector<int32_t   >*)value.ptr)->size();
    case 'x': return (value.ptr == NULL)? 0 : ((vector<int64_t   >*)value.ptr)->size();
    case 'f': return (value.ptr == NULL)? 0 : ((vector<float     >*)value.ptr)->size();
    case 'd': return (value.ptr == NULL)? 0 : ((vector<double    >*)value.ptr)->size();
    case 'z': return (value.ptr == NULL)? 0 : ((vector<bool      >*)value.ptr)->size();
    case 'q': return (value.ptr == NULL)? 0 : ((vector<boolNull  >*)value.ptr)->size();
    case 'a': return (value.ptr == NULL)? 0 : ((vector<string*   >*)value.ptr)->size();
    case 's': return (value.ptr == NULL)? 0 : ((vector<wstring*  >*)value.ptr)->size();
    case 'o': return (value.ptr == NULL)? 0 : ((vector<VRTObject*>*)value.ptr)->size();
    default:  return npos;
  }
}

Value* Value::at (size_t i) const {
  if (isNullValue()) return new Value();
  switch (type) {
    case 'b': return new Value(((vector<int8_t    >*)value.ptr)->at(i));
    case 'i': return new Value(((vector<int16_t   >*)value.ptr)->at(i));
    case 'l': return new Value(((vector<int32_t   >*)value.ptr)->at(i));
    case 'x': return new Value(((vector<int64_t   >*)value.ptr)->at(i));
    case 'f': return new Value(((vector<float     >*)value.ptr)->at(i));
    case 'd': return new Value(((vector<double    >*)value.ptr)->at(i));
    case 'z': return new Value(((vector<bool      >*)value.ptr)->at(i));
    case 'q': return new Value(((vector<boolNull  >*)value.ptr)->at(i));
    case 'a': return new Value(((vector<string*   >*)value.ptr)->at(i), false);
    case 's': return new Value(((vector<wstring*  >*)value.ptr)->at(i), false);
    case 'o': return new Value(((vector<VRTObject*>*)value.ptr)->at(i), false);
    default:  return new Value();
  }
}

void Value::free () {
  if (isNullValue() || !owner) return;
  
  if (type == 'a') {
    vector<string*> *vec = (vector<string*>*)value.ptr;
    for (size_t i = 0; i < size(); i++) {
      if (vec->at(i) != NULL) delete vec->at(i);
    }
  }
  else if (type == 's') {
    vector<wstring*> *vec = (vector<wstring*>*)value.ptr;
    for (size_t i = 0; i < size(); i++) {
      if (vec->at(i) != NULL) delete vec->at(i);
    }
  }
  else if (type == 'o') {
    vector<VRTObject*> *vec = (vector<VRTObject*>*)value.ptr;
    for (size_t i = 0; i < size(); i++) {
      if (vec->at(i) != NULL) delete vec->at(i);
    }
  }
  
  switch (type) {
    case 'b': delete ((vector<int8_t    >*)value.ptr); break;
    case 'i': delete ((vector<int16_t   >*)value.ptr); break;
    case 'l': delete ((vector<int32_t   >*)value.ptr); break;
    case 'x': delete ((vector<int64_t   >*)value.ptr); break;
    case 'f': delete ((vector<float     >*)value.ptr); break;
    case 'd': delete ((vector<double    >*)value.ptr); break;
    case 'z': delete ((vector<bool      >*)value.ptr); break;
    case 'q': delete ((vector<boolNull  >*)value.ptr); break;
    case 'a': delete ((vector<string*   >*)value.ptr); break;
    case 's': delete ((vector<wstring*  >*)value.ptr); break;
    case 'o': delete ((vector<VRTObject*>*)value.ptr); break;
  }
  value.ptr = NULL;
}

ValueType Value::getType () {
  switch (type) {
    case 'B': return ValueType_Int8;
    case 'I': return ValueType_Int16;
    case 'L': return ValueType_Int32;
    case 'X': return ValueType_Int64;
    case 'F': return ValueType_Float;
    case 'D': return ValueType_Double;
    case 'Z': return ValueType_Bool;
    case 'Q': return ValueType_BoolNull;
    case 'A': return ValueType_String;
    case 'S': return ValueType_WString;
    case 'O': return ValueType_VRTObject;
    case 'b': return (ValueType)-ValueType_Int8;
    case 'i': return (ValueType)-ValueType_Int16;
    case 'l': return (ValueType)-ValueType_Int32;
    case 'x': return (ValueType)-ValueType_Int64;
    case 'f': return (ValueType)-ValueType_Float;
    case 'd': return (ValueType)-ValueType_Double;
    case 'z': return (ValueType)-ValueType_Bool;
    case 'q': return (ValueType)-ValueType_BoolNull;
    case 'a': return (ValueType)-ValueType_String;
    case 's': return (ValueType)-ValueType_WString;
    case 'o': return (ValueType)-ValueType_VRTObject;
    default:  return ValueType_VRTObject;
  }
}

string operator+ (string &s, vrt::ValueType val) {
  ostringstream str;
  str << s << val;
  return str.str();
}

ostream& operator<< (ostream &s, vrt::ValueType val) {
  if (val < 0) {
    return s << ((vrt::ValueType)-val) << "[]";
  }
  
  switch (val) {
    case ValueType_Int8        : return s << "Int8";
    case ValueType_Int16       : return s << "Int16";
    case ValueType_Int32       : return s << "Int32";
    case ValueType_Int64       : return s << "Int64";
    case ValueType_Float       : return s << "Float";
    case ValueType_Double      : return s << "Double";
    case ValueType_Bool        : return s << "Bool";
    case ValueType_BoolNull    : return s << "BoolNull";
    case ValueType_String      : return s << "String";
    case ValueType_WString     : return s << "WString";
    case ValueType_VRTObject   : return s << "VRTObject";
    default                    : return s << "Unknown FieldType " << (int32_t)val;
  }
}

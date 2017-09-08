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

#include "Value.h"
#include "HasFields.h"
#include <cstring>
#include <sstream>
#include <iostream>
#include <map>
#include <string>

using namespace vrt;

// Need to disable Intel compiler warning 1572 (floating-point equality and
// inequality comparisons are unreliable) since it will otherwise flag many
// of the correct checks for 0.0.
_Intel_Pragma("warning push")
_Intel_Pragma("warning disable 1572")

/** <b>Internal Use Only:</b> A mapping of fields. */
class FieldMap : public VRTObject, public HasFields {
  private: bool               owner;
  private: vector<string>     names;
  private: map<string,Value*> values;

  /** Creates a new instance. */
  public: FieldMap (const map<string,Value*> &vals, bool owner);

  /** Destructor */
  public: virtual ~FieldMap () { free(); }

  public: virtual void    free ();
  public: virtual string  toString () const;
  public: virtual int32_t getFieldCount () const;
  public: virtual string  getFieldName (int32_t id) const;
  public: virtual void    setField (int32_t id, const Value* val);
  public: virtual Value*  getField (int32_t id) const  __attribute__((warn_unused_result));
};

FieldMap::FieldMap (const map<string,Value*>& vals, bool owner) :
  owner(owner)
{
  for (map<string,Value*>::const_iterator it = vals.begin(); it != vals.end(); ++it) {
    names.push_back(it->first);
    values[it->first] = it->second;
  }
}

void FieldMap::free () {
  if (!owner) return;

  for (int32_t id = 0; id < getFieldCount(); id++) {
    delete values[getFieldName(id)];
  }
  names.clear();
  values.clear();
}

string FieldMap::toString () const {
  string str = "{";
  FieldMap *self = const_cast<FieldMap*>(this);

  for (int32_t id = 0; id < getFieldCount(); id++) {
    if (id != 0) str += ",";
    str += " '" + getFieldName(id) + "'=";
    if (self->values[getFieldName(id)] == NULL) {
      str += "null";
    }
    else {
      str += self->values[getFieldName(id)]->toString();
    }
  }
  str += " }";
  return str;
}

int32_t FieldMap::getFieldCount () const {
  return (int32_t)names.size();
}

string FieldMap::getFieldName (int32_t id) const {
  if ((id < 0) || (id >= (int32_t)names.size())) {
    throw VRTException("Unknown field ID #%d", id);
  }
  return names[id];
}

void FieldMap::setField (int32_t id, const Value* val) {
  UNUSED_VARIABLE(id);
  UNUSED_VARIABLE(val);
  throw VRTException(getClassName() + " is read-only");
}

Value* FieldMap::getField (int32_t id) const  {
  FieldMap *self = const_cast<FieldMap*>(this);

  return new Value(self->values[getFieldName(id)]);
}

size_t Value::npos = string::npos;

Value::Value (                               ) : type('_'), owner(false) { value.xval   = 0;   init(); }
Value::Value (int8_t              val        ) : type('B'), owner(false) { value.bval   = val; init(); }
Value::Value (int16_t             val        ) : type('I'), owner(false) { value.ival   = val; init(); }
Value::Value (int32_t             val        ) : type('L'), owner(false) { value.lval   = val; init(); }
Value::Value (int64_t             val        ) : type('X'), owner(false) { value.xval   = val; init(); }
Value::Value (float               val        ) : type('F'), owner(false) { value.fval   = val; init(); }
Value::Value (double              val        ) : type('D'), owner(false) { value.dval   = val; init(); }
Value::Value (bool                val        ) : type('Z'), owner(false) { value.zval   = val; init(); }
Value::Value (boolNull            val        ) : type('Q'), owner(false) { value.qval   = val; init(); }
Value::Value (const string       &val        ) : type('A'), owner(true)  { value.aval   = new string(val);  init(); }
Value::Value (const wstring      &val        ) : type('S'), owner(true)  { value.sval   = new wstring(val); init(); }
Value::Value (const char         *val        ) : type('A'), owner(true)  { value.aval   = (val == NULL)? new string()  : new string(val);  init(); }
Value::Value (const wchar_t      *val        ) : type('S'), owner(true)  { value.sval   = (val == NULL)? new wstring() : new wstring(val); init(); }
Value::Value (map<string,Value*> &val, bool o) : type('O'), owner(true)  { value.oval   = (VRTObject*)new FieldMap(val,o); init(); }
Value::Value (Value              *val        ) : type('V'), owner(false) { value.vval   = (Value*    )val; init(); }
Value::Value (string             *val, bool o) : type('A'), owner(o)     { value.aval   = (string*   )val; init(); }
Value::Value (wstring            *val, bool o) : type('S'), owner(o)     { value.sval   = (wstring*  )val; init(); }
Value::Value (VRTObject          *val, bool o) : type('O'), owner(o)     { value.oval   = (VRTObject*)val; init(); }
Value::Value (vector<int8_t    > *val, bool o) : type('b'), owner(o)     { value.barray = val; init(); }
Value::Value (vector<int16_t   > *val, bool o) : type('i'), owner(o)     { value.iarray = val; init(); }
Value::Value (vector<int32_t   > *val, bool o) : type('l'), owner(o)     { value.larray = val; init(); }
Value::Value (vector<int64_t   > *val, bool o) : type('x'), owner(o)     { value.xarray = val; init(); }
Value::Value (vector<float     > *val, bool o) : type('f'), owner(o)     { value.farray = val; init(); }
Value::Value (vector<double    > *val, bool o) : type('d'), owner(o)     { value.darray = val; init(); }
Value::Value (vector<bool      > *val, bool o) : type('z'), owner(o)     { value.zarray = val; init(); }
Value::Value (vector<boolNull  > *val, bool o) : type('q'), owner(o)     { value.qarray = val; init(); }
Value::Value (vector<string*   > *val, bool o) : type('a'), owner(o)     { value.aarray = val; init(); }
Value::Value (vector<wstring*  > *val, bool o) : type('s'), owner(o)     { value.sarray = val; init(); }
Value::Value (vector<VRTObject*> *val, bool o) : type('o'), owner(o)     { value.oarray = val; init(); }
Value::Value (vector<Value*    > *val, bool o) : type('v'), owner(o)     { value.varray = val; init(); }

void Value::init () {
  switch (type) {
    case '_':                                       break;
    case 'B':                                       break;
    case 'I':                                       break;
    case 'L':                                       break;
    case 'X':                                       break;
    case 'F':                                       break;
    case 'D':                                       break;
    case 'Z':                                       break;
    case 'Q':                                       break;
    case 'A': if (value.aval   == NULL) type = '_'; break;
    case 'S': if (value.sval   == NULL) type = '_'; break;
    case 'O': if (value.oval   == NULL) type = '_'; break;
    case 'V': if (value.vval   == NULL) type = '_'; break;
    case 'b': if (value.barray == NULL) type = '_'; break;
    case 'i': if (value.iarray == NULL) type = '_'; break;
    case 'l': if (value.larray == NULL) type = '_'; break;
    case 'x': if (value.xarray == NULL) type = '_'; break;
    case 'f': if (value.farray == NULL) type = '_'; break;
    case 'd': if (value.darray == NULL) type = '_'; break;
    case 'z': if (value.zarray == NULL) type = '_'; break;
    case 'q': if (value.qarray == NULL) type = '_'; break;
    case 'a': if (value.aarray == NULL) type = '_'; break;
    case 's': if (value.sarray == NULL) type = '_'; break;
    case 'o': if (value.oarray == NULL) type = '_'; break;
    case 'v': if (value.varray == NULL) type = '_'; break;
    default:  throw VRTException("Unexpected Value type %c", type);
  }
}

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
    case 'V': return value.vval->as<int8_t>();
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
    case 'V': return value.vval->as<int8_t>();
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
    case 'V': return value.vval->as<int16_t>();
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
    case 'V': return value.vval->as<int16_t>();
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
    case 'V': return value.vval->as<int32_t>();
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
    case 'V': return value.vval->as<int32_t>();
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
    case 'V': return value.vval->as<int64_t>();
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
    case 'V': return value.vval->as<int64_t>();
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
    case 'V': return value.vval->as<float>();
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
    case 'V': return value.vval->as<float>();
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
    case 'V': return value.vval->as<double>();
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
    case 'V': return value.vval->as<double>();
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
    case 'V': return value.vval->as<bool>();
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
    case 'V': return value.vval->as<bool>();
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
    case 'V': return value.vval->as<boolNull>();
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
    case 'V': return value.vval->as<boolNull>();
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
  if (type == 'V') return value.vval->as<string*>();
  return (type == 'A')? value.aval : NULL;
}

Value::operator wstring* () {
  if (type == 'V') return value.vval->as<wstring*>();
  return (type == 'S')? value.sval : NULL;
}

Value::operator VRTObject* () {
  if (type == 'V') return value.vval->as<VRTObject*>();
  return (type == 'O')? value.oval : NULL;
}

Value::operator HasFields* () {
  return checked_dynamic_cast<HasFields*>(as<VRTObject*>());
}

string Value::toString () const {
  if (type == 'V') return value.vval->toString();
  if (isNullValue()) return "null";
  ostringstream str;
  if (size() == npos) {
    switch (type) {
      case 'B': str <<  value.bval; break;
      case 'I': str <<  value.ival; break;
      case 'L': str <<  value.lval; break;
      case 'X': str <<  value.xval; break;
      case 'F': str <<  value.fval; break;
      case 'D': str <<  value.dval; break;
      case 'Z': str <<  value.zval; break;
      case 'Q': str <<  value.qval; break;
      case 'A': str << *value.aval; break;
      case 'S': str << *value.sval; break;
      case 'O': str << *value.oval; break;
    }
  }
  else {
    str << "[ ";
    for (size_t i = 0; i < size(); i++) {
      if (i > 0) str << ", ";
      Value* v = at(i);
      str << *v;
      safe_delete(v);
    }
    str << " ]";
  }
  return str.str();
}

bool Value::_equals (const Value *v) const {
  if (type    == 'V') return value.vval->equals(v);
  if (v->type == 'V') return equals(v->value.vval);

  if (type != v->type) return false;

  switch (type) {
    case 'B': return (value.bval == v->value.bval);
    case 'I': return (value.ival == v->value.ival);
    case 'L': return (value.lval == v->value.lval);
    case 'X': return (value.xval == v->value.xval);
    case 'F': return (value.fval == v->value.fval);
    case 'D': return (value.dval == v->value.dval);
    case 'Z': return (value.zval == v->value.zval);
    case 'Q': return (value.qval == v->value.qval);
    case 'A': return (*value.aval == *v->value.aval);
    case 'S': return (*value.sval == *v->value.sval);
    case 'O': return (*value.oval == *v->value.oval);
  }

  if (size() != v->size()) return false;
  for (size_t i = 0; i < size(); i++) {
    Value* a  = at(i);
    Value* b  = v->at(i);
    bool   ok = (a == b);
    safe_delete(a);
    safe_delete(b);
    if (!ok) return false;
  }
  return true;
}

bool Value::isNullValue () const  {
  switch (type) {
    case '_': return true;
    case 'B': return isNull(value.bval);
    case 'I': return isNull(value.ival);
    case 'L': return isNull(value.lval);
    case 'X': return isNull(value.xval);
    case 'F': return isNull(value.fval);
    case 'D': return isNull(value.dval);
    case 'Z': return isNull(value.zval);
    case 'Q': return isNull(value.qval);
    case 'A': return isNull(value.aval);
    case 'S': return isNull(value.sval);
    case 'O': return isNull(value.oval);
    case 'V': return isNull(value.vval);
    default:  return (size() == 0);
  }
}

size_t Value::size () const {
  switch (type) {
    case '_': return npos;
    case 'b': return value.barray->size();
    case 'i': return value.iarray->size();
    case 'l': return value.larray->size();
    case 'x': return value.xarray->size();
    case 'f': return value.farray->size();
    case 'd': return value.darray->size();
    case 'z': return value.zarray->size();
    case 'q': return value.qarray->size();
    case 'a': return value.aarray->size();
    case 's': return value.sarray->size();
    case 'o': return value.oarray->size();
    case 'v': return value.varray->size();
    case 'V': return value.vval->size();
    default:  return npos;
  }
}

Value* Value::at (size_t i) const {
  switch (type) {
    case 'b': return new Value(value.barray->at(i));
    case 'i': return new Value(value.iarray->at(i));
    case 'l': return new Value(value.larray->at(i));
    case 'x': return new Value(value.xarray->at(i));
    case 'f': return new Value(value.farray->at(i));
    case 'd': return new Value(value.darray->at(i));
    case 'z': return new Value(value.zarray->at(i));
    case 'q': return new Value(value.qarray->at(i));
    case 'a': return new Value(value.aarray->at(i), false);
    case 's': return new Value(value.sarray->at(i), false);
    case 'o': return new Value(value.oarray->at(i), false);
    case 'v': return new Value(value.varray->at(i), false);
    case 'V': return value.vval->at(i);
    default:  return new Value();
  }
}

Value::~Value () {
  if (!owner) return;

  if (type == 'a') {
    for (size_t i = 0; i < size(); i++) {
      if (value.aarray->at(i) != NULL) delete value.aarray->at(i);
    }
    value.aarray->clear();
  }
  else if (type == 's') {
    for (size_t i = 0; i < size(); i++) {
      if (value.sarray->at(i) != NULL) delete value.sarray->at(i);
    }
    value.sarray->clear();
  }
  else if (type == 'o') {
    for (size_t i = 0; i < size(); i++) {
      if (value.oarray->at(i) != NULL) value.oarray->at(i)->_delete();
    }
    value.oarray->clear();
  }
  else if (type == 'v') {
    for (size_t i = 0; i < size(); i++) {
      if (value.varray->at(i) != NULL) value.varray->at(i)->_delete();
    }
    value.varray->clear();
  }

  switch (type) {
    case 'A': safe_delete(value.aval);   break;
    case 'S': safe_delete(value.sval);   break;
    case 'O': safe_delete(value.oval);   break;
    case 'V': safe_delete(value.vval);   break;
    case 'b': safe_delete(value.barray); break;
    case 'i': safe_delete(value.iarray); break;
    case 'l': safe_delete(value.larray); break;
    case 'x': safe_delete(value.xarray); break;
    case 'f': safe_delete(value.farray); break;
    case 'd': safe_delete(value.darray); break;
    case 'z': safe_delete(value.zarray); break;
    case 'q': safe_delete(value.qarray); break;
    case 'a': safe_delete(value.aarray); break;
    case 's': safe_delete(value.sarray); break;
    case 'o': safe_delete(value.oarray); break;
    case 'v': safe_delete(value.varray); break;
  }
  type = '_';
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
    case 'V': return value.vval->getType();
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
    case 'v': return (ValueType)-ValueType_VRTObject;
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

_Intel_Pragma("warning pop")

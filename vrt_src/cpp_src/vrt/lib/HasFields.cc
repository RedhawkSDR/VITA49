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

#include "HasFields.h"
#include <stdlib.h> // for atoi(..)

using namespace vrt;


/** Internal use only: A parsed field name. */
class ParsedFieldName {
  /** Internal use only. */ public: string  first;
  /** Internal use only. */ public: int32_t idx;
  /** Internal use only. */ public: string  next;
  
  /** Internal use only. */
  public: ParsedFieldName (const string &name);
};

ParsedFieldName::ParsedFieldName (const string &name) :
  first(""),
  idx(-1),
  next("")
{
  size_t dotIndex   = name.find(".");
  size_t startIndex = name.find("[");
  
  if ((dotIndex == string::npos) && (startIndex == string::npos)) {
    first = name;
    idx   = -1;
    next  = "";
  }
  else if (startIndex == string::npos) {
    first = name.substr(0,dotIndex);
    idx   = -1;
    next  = name.substr(dotIndex+1);
  }
  else if ((dotIndex == string::npos) ||  (startIndex < dotIndex)) {
    size_t endIndex = name.find("]");
    
    if ((endIndex == string::npos) || (endIndex < startIndex)) {
      throw VRTException("Invalid field name '%s'", name.c_str());
    }
    
    string num = name.substr(startIndex+1,endIndex-startIndex-2);
    
    first = name.substr(0,startIndex);
    idx   = atoi(num.c_str());
    next  = name.substr(endIndex+1);
  }
  else {
    first = name.substr(0,dotIndex);
    idx   = -1;
    next  = name.substr(dotIndex+1);
  }
}

int32_t HasFields::getFieldID (const string &name) const {
  for (int32_t i = 0; i < getFieldCount(); i++) {
    if (name == getFieldName(i)) return i;
  }
  throw VRTException("Invalid field name '%s' in %s", name.c_str(),
                     checked_dynamic_cast<const VRTObject*>(this)->getClassName().c_str());
}

/** Internal Use Only: Only used within setFieldByName(..). */
static inline void setValIn (Value *array, size_t i, const Value *value) {
  switch ((int32_t)array->getType()) {
    case -ValueType_Int8      : ((vector<int8_t    >*)array)->at(i) = ((int8_t    )(*value)); break;
    case -ValueType_Int16     : ((vector<int16_t   >*)array)->at(i) = ((int16_t   )(*value)); break;
    case -ValueType_Int32     : ((vector<int32_t   >*)array)->at(i) = ((int32_t   )(*value)); break;
    case -ValueType_Int64     : ((vector<int64_t   >*)array)->at(i) = ((int64_t   )(*value)); break;
    case -ValueType_Float     : ((vector<float     >*)array)->at(i) = ((float     )(*value)); break;
    case -ValueType_Double    : ((vector<double    >*)array)->at(i) = ((double    )(*value)); break;
    case -ValueType_Bool      : ((vector<bool      >*)array)->at(i) = ((bool      )(*value)); break;
    case -ValueType_BoolNull  : ((vector<boolNull  >*)array)->at(i) = ((boolNull  )(*value)); break;
    case -ValueType_String    : ((vector<string*   >*)array)->at(i) = const_cast<string*   >(((const string*   )(*value))); break;
    case -ValueType_WString   : ((vector<wstring*  >*)array)->at(i) = const_cast<wstring*  >(((const wstring*  )(*value))); break;
    case -ValueType_VRTObject : ((vector<VRTObject*>*)array)->at(i) = const_cast<VRTObject*>(((const VRTObject*)(*value))); break;
    default                   : throw VRTException("Invalid array type "+array->getType());
  }
}

Value* HasFields::getFieldByName (const string &name) const {
  ParsedFieldName pfn(name);
  int32_t         id  = getFieldID(pfn.first);
  Value          *val = getField(id);
  
  if (val      == NULL  ) return new Value(); // <-- null value
  if (pfn.next == ""    ) return val;         // <-- use current value (may be null)
  if (val->isNullValue()) return val;         // <-- null value (already allocated)
  if (pfn.idx  >= 0     ) val = val->at(pfn.idx);
  
  HasFields *hf  = val->as<HasFields*>();
  if (hf == NULL) {
    // Throw exception if trying to get a sub-field on something that doesn't
    // have sub-fields (e.g. a double). If the value was null the above code
    // will hide this error by simply returning null; but there isn't much
    // we can do to avoid this.
    throw VRTException("Can not get '%s' in %s when %s is %s", name.c_str(),
                       checked_dynamic_cast<const VRTObject*>(this)->getClassName().c_str(),
                       pfn.first.c_str(), val->toString().c_str());
  }
  return hf->getFieldByName(pfn.next);
}

void HasFields::setFieldByName (const string &name, const Value* value) {
  ParsedFieldName pfn(name);
  int32_t         id = getFieldID(name);
  
  if ((pfn.idx < 0) && (pfn.next == "")) { // no next, no idx
    setField(id, value);
    return;
  }
  
  if (pfn.idx < 0) { // next only, no idx
    Value     *val = getField(id);
    HasFields *hf  = val->as<HasFields*>();
    if (val == NULL) {
      throw VRTException("Can not set '%s' in %s when %s is %s", name.c_str(),
                         checked_dynamic_cast<const VRTObject*>(this)->getClassName().c_str(),
                         pfn.first.c_str(), "null");
    }
    if (hf == NULL) {
      throw VRTException("Can not set '%s' in %s when %s is %s", name.c_str(),
                         checked_dynamic_cast<const VRTObject*>(this)->getClassName().c_str(),
                         pfn.first.c_str(), val->toString().c_str());
    }
    hf->setFieldByName(pfn.next, value);
    setField(id, val);
    delete val;
    return;
  }
  
  if (pfn.next == "") { // idx only, no next
    Value *val = getField(id);
    setValIn(val, pfn.idx, value);
    setField(id, val);
    delete val;
    return;
  }
    
  // idx and next
  Value *val = getField(id);
  if (val == NULL) {
    throw VRTException("Can not set '%s' in %s when %s is %s", name.c_str(),
                       checked_dynamic_cast<const VRTObject*>(this)->getClassName().c_str(),
                       pfn.first.c_str(), "null");
  }
  Value     *v   = val->at(pfn.idx);
  HasFields *hf  = v->as<HasFields*>();
  if (v == NULL) {
    throw VRTException("Can not set '%s' in %s when %s[%d] is %s", name.c_str(),
                       checked_dynamic_cast<const VRTObject*>(this)->getClassName().c_str(),
                       pfn.first.c_str(), pfn.idx, "null");
  }
  if (hf == NULL) {
    throw VRTException("Can not set '%s' in %s when %s[%d] is %s", name.c_str(),
                       checked_dynamic_cast<const VRTObject*>(this)->getClassName().c_str(),
                       pfn.first.c_str(), pfn.idx, v->toString().c_str());
  }
  hf->setFieldByName(pfn.next, value);
  setValIn(val, pfn.idx, v);
  setField(id, val);
  delete val;
  delete v;
}

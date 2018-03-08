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

#include "PayloadFormat.h"
#include <cstring>
//#include <stdlib.h>         // required for atoi(..) on GCC4.4/libc6 2.11.1

using namespace vrt;

// Data Sample formats (see V49.2 section 6.1.1.4 rule #1)
ostream& operator<<(ostream &s, RealComplexType val) {
  switch (val) {
    case RealComplexType_Real:             return s << "Real";
    case RealComplexType_ComplexCartesian: return s << "ComplexCartesian";
    case RealComplexType_ComplexPolar:     return s << "ComplexPolar";
    case RealComplexType_reserved3:        return s << "reserved3";
    default:                               return s << "Unknown RealComplexType " << (int32_t)val;
  }
}

string operator+(string &s, RealComplexType val) {
  ostringstream str;
  str << s << val;
  return str.str();
}

// Data Item formats (see V49.2 section 6.1.1.4 rule #2)
ostream& operator<<(ostream &s, DataItemFormat val) {
  switch (val) {
    case DataItemFormat_SignedInt    : return s << "SignedInt";      case DataItemFormat_SignedVRT1    : return s << "SignedVRT1";
    case DataItemFormat_SignedVRT2   : return s << "SignedVRT2";     case DataItemFormat_SignedVRT3    : return s << "SignedVRT3";
    case DataItemFormat_SignedVRT4   : return s << "SignedVRT4";     case DataItemFormat_SignedVRT5    : return s << "SignedVRT5";
    case DataItemFormat_SignedVRT6   : return s << "SignedVRT6";     case DataItemFormat_SignedIntNN   : return s << "SignedIntNN";
    case DataItemFormat_reserved8    : return s << "reserved8";      case DataItemFormat_reserved9     : return s << "reserved9";
    case DataItemFormat_reserved10   : return s << "reserved10";     case DataItemFormat_reserved11    : return s << "reserved11";
    case DataItemFormat_reserved12   : return s << "reserved12";     case DataItemFormat_Float16       : return s << "Float16";
    case DataItemFormat_Float        : return s << "Float";          case DataItemFormat_Double        : return s << "Double";
    case DataItemFormat_UnsignedInt  : return s << "UnsignedInt";    case DataItemFormat_UnsignedVRT1  : return s << "UnsignedVRT1";
    case DataItemFormat_UnsignedVRT2 : return s << "UnsignedVRT2";   case DataItemFormat_UnsignedVRT3  : return s << "UnsignedVRT3";
    case DataItemFormat_UnsignedVRT4 : return s << "UnsignedVRT4";   case DataItemFormat_UnsignedVRT5  : return s << "UnsignedVRT5";
    case DataItemFormat_UnsignedVRT6 : return s << "UnsignedVRT6";   case DataItemFormat_UnsignedIntNN : return s << "UnsignedIntNN";
    case DataItemFormat_reserved24   : return s << "reserved24";     case DataItemFormat_reserved25    : return s << "reserved25";
    case DataItemFormat_reserved26   : return s << "reserved26";     case DataItemFormat_reserved27    : return s << "reserved27";
    case DataItemFormat_reserved28   : return s << "reserved28";     case DataItemFormat_reserved29    : return s << "reserved29";
    case DataItemFormat_reserved30   : return s << "reserved30";     case DataItemFormat_reserved31    : return s << "reserved31";
    default                          : return s << "Unknown DataItemFormat " << (int32_t)val;
  }
}

string operator+(string &s, DataItemFormat val) {
  ostringstream str;
  str << s << val;
  return str.str();
}

ostream& operator<<(ostream &s, DataType val) {
  if (((int32_t)val) == -1) return s << "null";
  switch (val) {
    case DataType_Int4       : return s << "Int4";
    case DataType_Int8       : return s << "Int8";
    case DataType_Int16      : return s << "Int16";
    case DataType_Int32      : return s << "Int32";
    case DataType_Int64      : return s << "Int64";
    //case DataType_Float16    : return s << "Float16";
    case DataType_Float      : return s << "Float";
    case DataType_Double     : return s << "Double";
    case DataType_UInt1      : return s << "UInt1";
    case DataType_UInt4      : return s << "UInt4";
    case DataType_UInt8      : return s << "UInt8";
    case DataType_UInt16     : return s << "UInt16";
    case DataType_UInt32     : return s << "UInt32";
    case DataType_UInt64     : return s << "UInt64";
    default                  : return s << "Unknown DataType " << (int64_t)val;
  }
}

string operator+(string &s, DataType val) {
  ostringstream str;
  str << s << val;
  return str.str();
}

PayloadFormat::PayloadFormat () :
  hi(PayloadFormat::NULL_VAL_HI),
  lo(PayloadFormat::NULL_VAL_LO)
{
  // done
}

PayloadFormat::PayloadFormat (const PayloadFormat &pf) :
  VRTObject(pf), // <-- Used to avoid warnings under GCC with -Wextra turned on
  hi(pf.hi),
  lo(pf.lo)
{
  // done
}

PayloadFormat::PayloadFormat (int64_t bits) : hi(0), lo(0) {
  setBits(bits);
}

PayloadFormat::PayloadFormat (DataItemFormat format, int32_t size) : hi(0), lo(0) {
  setProcessingEfficient(true);
  setRealComplexType(RealComplexType_Real);
  setDataItemFormat(format);
  setRepeating(false);
  setEventTagSize(0);
  setChannelTagSize(0);
  setDataItemFracSize(0);
  setItemPackingFieldSize(size);
  setDataItemSize(size);
  setRepeatCount(1);
  setVectorSize(1);
}

PayloadFormat::PayloadFormat (RealComplexType type, DataItemFormat format, int32_t size) : hi(0), lo(0) {
  setProcessingEfficient(true);
  setRealComplexType(type);
  setDataItemFormat(format);
  setRepeating(false);
  setEventTagSize(0);
  setChannelTagSize(0);
  setDataItemFracSize(0);
  setItemPackingFieldSize(size);
  setDataItemSize(size);
  setRepeatCount(1);
  setVectorSize(1);
}

PayloadFormat::PayloadFormat (DataType dataType, RealComplexType realComplex) : hi(0), lo(0) {
  setRealComplexType(realComplex);
  setDataType(dataType);
}

PayloadFormat::PayloadFormat (DataItemFormat format, int32_t size, int32_t fracSize) : hi(0), lo(0) {
  setProcessingEfficient(true);
  setRealComplexType(RealComplexType_Real);
  setDataItemFormat(format);
  setRepeating(false);
  setEventTagSize(0);
  setChannelTagSize(0);
  setDataItemFracSize(fracSize);
  setItemPackingFieldSize(size);
  setDataItemSize(size);
  setRepeatCount(1);
  setVectorSize(1);
}

PayloadFormat::PayloadFormat (RealComplexType type, DataItemFormat format, int32_t size, int32_t fracSize) : hi(0), lo(0) {
  setProcessingEfficient(true);
  setRealComplexType(type);
  setDataItemFormat(format);
  setRepeating(false);
  setEventTagSize(0);
  setChannelTagSize(0);
  setDataItemFracSize(fracSize);
  setItemPackingFieldSize(size);
  setDataItemSize(size);
  setRepeatCount(1);
  setVectorSize(1);
}

PayloadFormat::PayloadFormat (bool procEfficient, RealComplexType type, DataItemFormat format,
                              bool repeating, int32_t eventTagSize, int32_t chanTagSize, int32_t fieldSize,
                              int32_t itemSize, int32_t repeatCount, int32_t vectorSize, int32_t fracSize) : hi(0), lo(0) {
  setProcessingEfficient(procEfficient);
  setRealComplexType(type);
  setDataItemFormat(format);
  setRepeating(repeating);
  setEventTagSize(eventTagSize);
  setChannelTagSize(chanTagSize);
  setDataItemFracSize(fracSize);
  setItemPackingFieldSize(fieldSize);
  setDataItemSize(itemSize);
  setRepeatCount(repeatCount);
  setVectorSize(vectorSize);
}

string PayloadFormat::getValid () const {
  int            fSize  = getItemPackingFieldSize();
  int            dSize  = getDataItemSize();
  int            eSize  = getEventTagSize();
  int            cSize  = getChannelTagSize();
  int         fracSize  = getDataItemFracSize();
  DataItemFormat format = getDataItemFormat();

  if (fSize < (dSize+eSize+cSize)) {
    return "Given ItemPackingFieldSize is incompatible with DataItemSize, "
           "EventTagSize, and ChannelTagSize.";
  }

  switch (format) {
    case DataItemFormat_SignedVRT1:   if (dSize <   2) return "Invalid use of DataItemFormat=SignedVRT1 with DataItemSize<2"; break;
    case DataItemFormat_SignedVRT2:   if (dSize <   3) return "Invalid use of DataItemFormat=SignedVRT2 with DataItemSize<3"; break;
    case DataItemFormat_SignedVRT3:   if (dSize <   4) return "Invalid use of DataItemFormat=SignedVRT3 with DataItemSize<4"; break;
    case DataItemFormat_SignedVRT4:   if (dSize <   5) return "Invalid use of DataItemFormat=SignedVRT4 with DataItemSize<5"; break;
    case DataItemFormat_SignedVRT5:   if (dSize <   6) return "Invalid use of DataItemFormat=SignedVRT5 with DataItemSize<6"; break;
    case DataItemFormat_SignedVRT6:   if (dSize <   7) return "Invalid use of DataItemFormat=SignedVRT6 with DataItemSize<7"; break;
    case DataItemFormat_UnsignedVRT1: if (dSize <   2) return "Invalid use of DataItemFormat=UnsignedVRT1 with DataItemSize<2"; break;
    case DataItemFormat_UnsignedVRT2: if (dSize <   3) return "Invalid use of DataItemFormat=UnsignedVRT2 with DataItemSize<3"; break;
    case DataItemFormat_UnsignedVRT3: if (dSize <   4) return "Invalid use of DataItemFormat=UnsignedVRT3 with DataItemSize<4"; break;
    case DataItemFormat_UnsignedVRT4: if (dSize <   5) return "Invalid use of DataItemFormat=UnsignedVRT4 with DataItemSize<5"; break;
    case DataItemFormat_UnsignedVRT5: if (dSize <   6) return "Invalid use of DataItemFormat=UnsignedVRT5 with DataItemSize<6"; break;
    case DataItemFormat_UnsignedVRT6: if (dSize <   7) return "Invalid use of DataItemFormat=UnsignedVRT6 with DataItemSize<7"; break;
    case DataItemFormat_Float16:      if (dSize != 16) return "Invalid use of DataItemFormat=Float16 with DataItemSize!=16"; break;
    case DataItemFormat_Float:        if (dSize != 32) return "Invalid use of DataItemFormat=Float with DataItemSize!=32"; break;
    case DataItemFormat_Double:       if (dSize != 64) return "Invalid use of DataItemFormat=Double with DataItemSize!=64"; break;
    default: break;
  }
  
  // Special check of DataItemFracSize
  //if (format & 0x07 == 0x07) // DataItemFormat_SignedIntNN (0x07) or DataItemFormat_UnsignedIntNN (0x17)
  if (format == DataItemFormat_SignedIntNN || format == DataItemFormat_UnsignedIntNN) {
    if (fracSize > dSize-1) return "Given DataItemFracSize is incompatible with DataItemSize";
  }
  else if (fracSize != 0) {
    return "DataItemFracSize must be zero for all normalized data types";
  }

  return "";
}

string PayloadFormat::toString () const {
  if (isNullValue()) return "null";
  ostringstream str;
  str <<  "ProcessingEfficient="  << isProcessingEfficient();
  str << " RealComplexType="      << getRealComplexType();
  str << " DataItemFormat="       << getDataItemFormat();
  str << " Repeating="            << isRepeating();
  str << " EventTagSize="         << getEventTagSize();
  str << " ChannelTagSize="       << getChannelTagSize();
  str << " DataItemFracSize="     << getDataItemFracSize();
  str << " ItemPackingFieldSize=" << getItemPackingFieldSize();
  str << " DataItemSize="         << getDataItemSize();
  str << " RepeatCount="          << getRepeatCount();
  str << " VectorSize="           << getVectorSize();
  str << " DataType="             << getDataType();
  return str.str();
}

bool PayloadFormat::equals (const VRTObject &o) const {
  if (getClass() != o.getClass()) return false;
  const PayloadFormat pf = *checked_dynamic_cast<const PayloadFormat*>(&o);
  return (pf.hi == hi) && (pf.lo == lo);
}

DataType PayloadFormat::getDataType () const {
  int size = getDataItemSize();
  if (getItemPackingFieldSize() != size) return (DataType)-1; // ERROR

  switch (getDataItemFormat()) {
    case DataItemFormat_SignedInt:
      switch (size) {
        case  1:                 return DataType_UInt1; // allow packed bits as signed or unsigned
        case  4:                 return DataType_Int4;
        case  8:                 return DataType_Int8;
        case 16:                 return DataType_Int16;
        case 32:                 return DataType_Int32;
        case 64:                 return DataType_Int64;
        default:                 return (DataType)-1; // ERROR
      }
    case DataItemFormat_UnsignedInt:
      switch (size) {
        case  1:                 return DataType_UInt1; // allow packed bits as signed or unsigned
        case  4:                 return DataType_UInt4;
        case  8:                 return DataType_UInt8;
        case 16:                 return DataType_UInt16;
        case 32:                 return DataType_UInt32;
        case 64:                 return DataType_UInt64;
        default:                 return (DataType)-1; // ERROR
      }
    // For same reason the VRT types aren't included, the non-normalized should not be included:
    //   there is no pre-defined fraction size
    //case DataItemFormat_SignedIntNN:
    //case DataItemFormat_UnsignedIntNN:
    //case DataItemFormat_Float16: return DataType_Float16;
    case DataItemFormat_Float:   return DataType_Float;
    case DataItemFormat_Double:  return DataType_Double;
    default:                     return (DataType)-1; // ERROR
  }
}

void PayloadFormat::setDataType (DataType type) {
  // clearNull(); // unnecessary since this will be called by all three function calls below.
  int size, fracSize = 0;
  switch (type) {
    case DataType_Int4:       size =  4; setDataItemFormat(DataItemFormat_SignedInt  ); break; // nibble
    case DataType_Int8:       size =  8; setDataItemFormat(DataItemFormat_SignedInt  ); break; // byte
    case DataType_Int16:      size = 16; setDataItemFormat(DataItemFormat_SignedInt  ); break; // short
    case DataType_Int32:      size = 32; setDataItemFormat(DataItemFormat_SignedInt  ); break; // int
    case DataType_Int64:      size = 64; setDataItemFormat(DataItemFormat_SignedInt  ); break; // long
    case DataType_UInt1:      size =  1; setDataItemFormat(DataItemFormat_UnsignedInt); break; // bit
    case DataType_UInt4:      size =  4; setDataItemFormat(DataItemFormat_UnsignedInt); break; // nibble
    case DataType_UInt8:      size =  8; setDataItemFormat(DataItemFormat_UnsignedInt); break; // byte
    case DataType_UInt16:     size = 16; setDataItemFormat(DataItemFormat_UnsignedInt); break; // short
    case DataType_UInt32:     size = 32; setDataItemFormat(DataItemFormat_UnsignedInt); break; // int
    case DataType_UInt64:     size = 64; setDataItemFormat(DataItemFormat_UnsignedInt); break; // long
    //case DataType_Float16:    size = 16; setDataItemFormat(DataItemFormat_Float16    ); break; // short
    case DataType_Float:      size = 32; setDataItemFormat(DataItemFormat_Float      ); break; // int
    case DataType_Double:     size = 64; setDataItemFormat(DataItemFormat_Double     ); break; // long
    // For same reason the VRT types aren't included, the non-normalized should not be included:
    //   there is no pre-defined fraction size
    //case DataItemFormat_SignedIntNN:
    //case DataItemFormat_UnsignedIntNN:
    default:                  throw VRTException("Unsupported data type %d", type);
  }
  setItemPackingFieldSize(size);
  setDataItemSize(size);
  setDataItemFracSize(fracSize);
}

int32_t PayloadFormat::getFieldCount () const {
  return 12;
}

string PayloadFormat::getFieldName (int32_t id) const {
  switch (id) {
    case  0: return "ProcessingEfficient";
    case  1: return "RealComplexType";
    case  2: return "DataItemFormat";
    case  3: return "Repeating";
    case  4: return "EventTagSize";
    case  5: return "ChannelTagSize";
    case  6: return "DataItemFracSize";
    case  7: return "ItemPackingFieldSize";
    case  8: return "DataItemSize";
    case  9: return "RepeatCount";
    case 10: return "VectorSize";
    case 11: return "DataType";
    default: throw VRTException("Invalid field #%d in %s", id, getClassName().c_str());
  }
}

ValueType PayloadFormat::getFieldType (int32_t id) const {
  switch (id) {
    case  0: return ValueType_Bool;
    case  1: return ValueType_Int8;
    case  2: return ValueType_Int8;
    case  3: return ValueType_Bool;
    case  4: return ValueType_Int32;
    case  5: return ValueType_Int32;
    case  6: return ValueType_Int32;
    case  7: return ValueType_Int32;
    case  8: return ValueType_Int32;
    case  9: return ValueType_Int32;
    case 10: return ValueType_Int32;
    case 11: return ValueType_Int64;
    default: throw VRTException("Invalid field #%d in %s", id, getClassName().c_str());
  }
}

Value* PayloadFormat::getField (int32_t id) const {
  switch (id) {
    case  0: return new Value(isProcessingEfficient());
    case  1: return new Value((int8_t)getRealComplexType());
    case  2: return new Value((int8_t)getDataItemFormat());
    case  3: return new Value(isRepeating());
    case  4: return new Value(getEventTagSize());
    case  5: return new Value(getChannelTagSize());
    case  6: return new Value(getDataItemFracSize());
    case  7: return new Value(getItemPackingFieldSize());
    case  8: return new Value(getDataItemSize());
    case  9: return new Value(getRepeatCount());
    case 10: return new Value(getVectorSize());
    case 11: return new Value((int64_t)getDataType());
    default: return new Value(); // null value
  }
}

void PayloadFormat::setField (int32_t id, const Value* val) {
  switch (id) {
    case  0: setProcessingEfficient(                  val->as<bool   >()); break;
    case  1: setRealComplexType(     (RealComplexType)val->as<int32_t>()); break;
    case  2: setDataItemFormat(      (DataItemFormat )val->as<int32_t>()); break;
    case  3: setRepeating(                            val->as<bool   >()); break;
    case  4: setEventTagSize(                         val->as<int32_t>()); break;
    case  5: setChannelTagSize(                       val->as<int32_t>()); break;
    case  6: setDataItemFracSize(                     val->as<int32_t>()); break;
    case  7: setItemPackingFieldSize(                 val->as<int32_t>()); break;
    case  8: setDataItemSize(                         val->as<int32_t>()); break;
    case  9: setRepeatCount(                          val->as<int32_t>()); break;
    case 10: setVectorSize(                           val->as<int32_t>()); break;
    case 11: setDataType(            (DataType       )val->as<int64_t>()); break;
    default: throw VRTException("Invalid field #%d in %s", id, getClassName().c_str());
  }
}

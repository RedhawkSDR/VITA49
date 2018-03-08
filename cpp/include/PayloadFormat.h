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

#ifndef PayloadFormat_h
#define PayloadFormat_h

#include "VRTObject.h"
#include "VRTMath.h"
#if NOT_USING_JNI
//# include "MetadataBlock.h"
//# include "Utilities.h"
//# include "TimeStamp.h"
//# include "Record.h"
//# include "InetAddress.h"
//# include "UUID.h"
# include "HasFields.h"
//# include "Record.h"
#endif /* NOT_USING_JNI */

using namespace std;

namespace vrt {
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  // Enumerated Types
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  /** Available the real/complex types for data items. */
  enum RealComplexType {
    /** Real samples.                                                   */   RealComplexType_Real             = 0,
    /** Complex cartesian samples (in-phase and quadrature components). */   RealComplexType_ComplexCartesian = 1,
    /** Complex polar samples (amplitude and phase components).         */   RealComplexType_ComplexPolar     = 2,
    /** <b>Do Not Use:</b> Reserved for future use.                     */   RealComplexType_reserved3        = 3
  };

  /** Available numeric formats for data items.*/
  enum DataItemFormat {
    /** Signed fixed-point.                         */    DataItemFormat_SignedInt     =  0,
    /** Signed VRT, 1-bit exponent.                 */    DataItemFormat_SignedVRT1    =  1,
    /** Signed VRT, 2-bit exponent.                 */    DataItemFormat_SignedVRT2    =  2,
    /** Signed VRT, 3-bit exponent.                 */    DataItemFormat_SignedVRT3    =  3,
    /** Signed VRT, 4-bit exponent.                 */    DataItemFormat_SignedVRT4    =  4,
    /** Signed VRT, 5-bit exponent.                 */    DataItemFormat_SignedVRT5    =  5,
    /** Signed VRT, 6-bit exponent.                 */    DataItemFormat_SignedVRT6    =  6,
    /** Signed fixed-point non-normalized           */    DataItemFormat_SignedIntNN   =  7, // NEW in V49.2
    /** <b>Do Not Use:</b> Reserved for future use. */    DataItemFormat_reserved8     =  8,
    /** <b>Do Not Use:</b> Reserved for future use. */    DataItemFormat_reserved9     =  9,
    /** <b>Do Not Use:</b> Reserved for future use. */    DataItemFormat_reserved10    = 10,
    /** <b>Do Not Use:</b> Reserved for future use. */    DataItemFormat_reserved11    = 11,
    /** <b>Do Not Use:</b> Reserved for future use. */    DataItemFormat_reserved12    = 12,
    /** 16-bit IEEE-754 floating-point.             */    DataItemFormat_Float16       = 13, // NEW in V49.2
    /** 32-bit IEEE-754 floating-point.             */    DataItemFormat_Float         = 14,
    /** 64-bit IEEE-754 floating-point.             */    DataItemFormat_Double        = 15,
    /** Unsigned fixed-point.                       */    DataItemFormat_UnsignedInt   = 16,
    /** Unsigned VRT, 1-bit exponent.               */    DataItemFormat_UnsignedVRT1  = 17,
    /** Unsigned VRT, 2-bit exponent.               */    DataItemFormat_UnsignedVRT2  = 18,
    /** Unsigned VRT, 3-bit exponent.               */    DataItemFormat_UnsignedVRT3  = 19,
    /** Unsigned VRT, 4-bit exponent.               */    DataItemFormat_UnsignedVRT4  = 20,
    /** Unsigned VRT, 5-bit exponent.               */    DataItemFormat_UnsignedVRT5  = 21,
    /** Unsigned VRT, 6-bit exponent.               */    DataItemFormat_UnsignedVRT6  = 22,
    /** Unsigned fixed-point non-normalized         */    DataItemFormat_UnsignedIntNN = 23, // NEW in V49.2
    /** <b>Do Not Use:</b> Reserved for future use. */    DataItemFormat_reserved24    = 24,
    /** <b>Do Not Use:</b> Reserved for future use. */    DataItemFormat_reserved25    = 25,
    /** <b>Do Not Use:</b> Reserved for future use. */    DataItemFormat_reserved26    = 26,
    /** <b>Do Not Use:</b> Reserved for future use. */    DataItemFormat_reserved27    = 27,
    /** <b>Do Not Use:</b> Reserved for future use. */    DataItemFormat_reserved28    = 28,
    /** <b>Do Not Use:</b> Reserved for future use. */    DataItemFormat_reserved29    = 29,
    /** <b>Do Not Use:</b> Reserved for future use. */    DataItemFormat_reserved30    = 30,
    /** <b>Do Not Use:</b> Reserved for future use. */    DataItemFormat_reserved31    = 31
  };

  /** <b>Internal Use Only:</b> Determines if value format is signed. */
  inline bool DataItemFormat_isSigned (DataItemFormat form) {
    return ((int32_t)form) < 16;
  }

  /** <b>Internal Use Only:</b> Determines number of exponent bits in VRT format. */
  inline int32_t DataItemFormat_getExponentBits (DataItemFormat form) {
    switch (form) {
      case DataItemFormat_SignedVRT1:   return 1;
      case DataItemFormat_SignedVRT2:   return 2;
      case DataItemFormat_SignedVRT3:   return 3;
      case DataItemFormat_SignedVRT4:   return 4;
      case DataItemFormat_SignedVRT5:   return 5;
      case DataItemFormat_SignedVRT6:   return 6;
      case DataItemFormat_UnsignedVRT1: return 1;
      case DataItemFormat_UnsignedVRT2: return 2;
      case DataItemFormat_UnsignedVRT3: return 3;
      case DataItemFormat_UnsignedVRT4: return 4;
      case DataItemFormat_UnsignedVRT5: return 5;
      case DataItemFormat_UnsignedVRT6: return 6;
      default: return -1;
    }
  }

  /** Data types supported by a <tt>StandardDataPacket</tt>. */
  enum DataType {
    // XXX - why the cryptic values? Something related to StandardDataPacket implementation
    /**  4-bit signed integer.                      */  DataType_Int4       = __INT64_C(0x00FFFFFA00010000),
    /**  8-bit signed integer.                      */  DataType_Int8       = __INT64_C(0x00FFFFFA00020000),
    /** 16-bit signed integer.                      */  DataType_Int16      = __INT64_C(0x00FFFFFA00030000),
    /** 32-bit signed integer.                      */  DataType_Int32      = __INT64_C(0x00FFFFFA00040000),
    /** 64-bit signed integer.                      */  DataType_Int64      = __INT64_C(0x00FFFFFA00050000),
    /** 32-bit single-precision floating-point.     */  DataType_Float      = __INT64_C(0x00FFFFFA00060000),
    /** 64-bit double-precision floating-point.     */  DataType_Double     = __INT64_C(0x00FFFFFA00070000),
    /**  1-bit unsigned integer ("packed bits").    */  DataType_UInt1      = __INT64_C(0x00FFFFFA00080000),
    /**  4-bit unsigned integer.                    */  DataType_UInt4      = __INT64_C(0x00FFFFFA00090000),
    /**  8-bit unsigned integer.                    */  DataType_UInt8      = __INT64_C(0x00FFFFFA000A0000),
    /** 16-bit unsigned integer.                    */  DataType_UInt16     = __INT64_C(0x00FFFFFA000B0000),
    /** 32-bit unsigned integer.                    */  DataType_UInt32     = __INT64_C(0x00FFFFFA000C0000),
    /** 64-bit unsigned integer.                    */  DataType_UInt64     = __INT64_C(0x00FFFFFA000D0000),
    // XXX - Add half-precision float? added to end rather than insert above and shift everything below it
    ///** 16-bit half-precision floating-point.       */  DataType_Float16    = __INT64_C(0x00FFFFFA000E0000)
  };

#if NOT_USING_JNI
  /** Specifies the payload format. <br>
   *  <br>
   *  The following implementation is used for the {@link HasFields} methods:
   *  <pre>
   *     ID | Name                 | Type
   *    ----+----------------------+---------------
   *     0  | ProcessingEfficient  | Bool
   *     1  | RealComplexType      | Int8
   *     2  | DataItemFormat       | Int8
   *     3  | Repeating            | Bool
   *     4  | EventTagSize         | Int32
   *     5  | ChannelTagSize       | Int32
   *     6  | DataItemFracSize     | Int32 - NEW in V49.2 and used only for Non-Normalized
   *     7  | ItemPackingFieldSize | Int32
   *     8  | DataItemSize         | Int32
   *     9  | RepeatCount          | Int32
   *     10 | VectorSize           | Int32
   *     11 | DataType             | Int64
   *    ----+----------------------+---------------
   *  </pre>
   */
  class PayloadFormat : public VRTObject, public HasFields {
    // TODO - there are no reserved bits in V49.2, which is new since V49.0
    //      - the bits that were reserved are now Data Item Frac Size

    // Since the values 0x0000000000000000 and 0x8000000000000000 are both valid and
    // 0xFFFFFFFFFFFFFFFF is invalid, this object uses -1 as the indicator for a null payload format.
    private: static const int32_t NULL_VAL_HI = -1; // 0xFFFFFFFFF;   // null value for hi (-1)
    private: static const int32_t NULL_VAL_LO = -1; // 0xFFFFFFFFF;   // null value for lo (-1)

    private: int32_t hi; // high-order bits
    private: int32_t lo; // low-order bits

    /** Creates new instance. */
    public: PayloadFormat ();

    /** Creates new instance. */
    public: PayloadFormat (const PayloadFormat &pf);

    /** Creates new instance. This is identical to
     *  <tt>new PayloadFormat(true,RealComplexType.Real,format,false,0,0,size,size,1,1)</tt>.
     *  @param format        The data format used.
     *  @param size          The data item size.
     */
    public: PayloadFormat (DataItemFormat format, int32_t size);

    /** Creates new instance. This is identical to
     *  <tt>new PayloadFormat(true,type,format,false,0,0,size,size,1,1)</tt>.
     *  @param type          The real/complex type used.
     *  @param format        The data format used.
     *  @param size          The data item size.
     */
    public: PayloadFormat (RealComplexType type, DataItemFormat format, int32_t size);

    /** Creates new instance. This is identical to
     *  <tt>new PayloadFormat(true,RealComplexType.Real,format,false,0,0,size,size,1,1,fracSize)</tt>.
     *  @param format        The data format used.
     *  @param size          The data item size.
     *  @param fracSize      The data item fraction size.
     */
    public: PayloadFormat (DataItemFormat format, int32_t size, int32_t fracSize);

    /** Creates new instance. This is identical to
     *  <tt>new PayloadFormat(true,type,format,false,0,0,size,size,1,1,fracSize)</tt>.
     *  @param type          The real/complex type used.
     *  @param format        The data format used.
     *  @param size          The data item size.
     *  @param fracSize      The data item fraction size.
     */
    public: PayloadFormat (RealComplexType type, DataItemFormat format, int32_t size, int32_t fracSize);

    /** Creates new instance.
     *  @param dataType     The data format used.
     *  @param realComplex  The real/complex type used.
     */
    public: PayloadFormat (DataType dataType, RealComplexType realComplex=RealComplexType_Real);

    /** Creates new instance.
     *  @param procEfficient true if Processing Efficient or false if Link Efficient.
     *  @param type          The real/complex type used.
     *  @param format        The data format used.
     *  @param repeating     Indicates if the sample-component repeat is used.
     *  @param eventTagSize  the event tag size.
     *  @param chanTagSize   the channel tag size.
     *  @param fieldSize     The item packing field size.
     *  @param itemSize      The data item size.
     *  @param repeatCount   The repeat count.
     *  @param vectorSize    The vector size.
     *  @param fracSize      The data item fraction size (default=0).
     */
    public: PayloadFormat (bool procEfficient, RealComplexType type, DataItemFormat format,
                           bool repeating, int32_t eventTagSize, int32_t chanTagSize, int32_t fieldSize,
                           int32_t itemSize, int32_t repeatCount, int32_t vectorSize, int32_t fracSize=0);

    /** Is this object equal to null? Since the values 0x0000000000000000 and 0x8000000000000000
     *  are both valid and 0xFFFFFFFFFFFFFFFF is invalid, this object uses -1 as the indicator for
     *  a null payload format.
     */
    public: inline virtual bool isNullValue () const {
      return (hi == NULL_VAL_HI)
          && (lo == NULL_VAL_LO);
    }

    /** <i>Internal Use Only:</i> Clear all bits if object is equal to null.
     * Only call this function from within a setter prior to setting desired bits.
     * TODO - functional for now, but perhaps there is a better way
     */
    private: inline virtual void clearNull () {
      if (isNullValue())
        hi = lo = 0;
    }

    /** <i>Internal Use Only:</i> Creates new instance. */
    public: PayloadFormat (int64_t bits);

    public: virtual string toString () const;

    using VRTObject::equals;
    public: virtual bool equals (const VRTObject &o) const;

    /** Checks to see if the payload format is valid.
     *  @return true if it is valid, false otherwise.
     */
    public: inline bool isValid () const {
      return (getValid() == "");
    }

    /** Checks to see if the payload format is valid. This is identical to
     *  {@link #isValid()} except that a description of the error is returned.
     *  @return "" if it is valid, a description of the error otherwise.
     */
    public: virtual string getValid () const;

    /** <i>Internal Use Only:</i> Gets the bits. */
    public: inline int64_t getBits () const {
      return (((int64_t)hi) << 32) | (((int64_t)lo) & __INT64_C(0xFFFFFFFF));
    }

    /** <i>Internal Use Only:</i> Sets the bits. */
    public: inline void setBits (int64_t bits) {
      // Note: previously reserved bits were forced to 0, but those bits are now DataItemFracSize
      //hi = ((int32_t)(bits >> 32)) & 0xFFFF0FFF;
      hi = (int32_t)(bits >> 32);
      lo = (int32_t)bits;
    }

    /** Indicates if the processing efficient packing method used.
     *  @return true if Processing Efficient (byte-aligned) or false if Link Efficient (fewest bytes used).
     */
    public: inline bool isProcessingEfficient () const {
      int32_t mask = 0x80000000;
      return ((hi & mask) == 0);
    }

    /** Indicates if the processing efficient packing method used.
     *  @param val true if Processing Efficient (byte-aligned) or false if Link Efficient (fewest bytes used).
     */
    public: inline void setProcessingEfficient (bool val) {
      clearNull();
      int32_t mask = 0x80000000;
      int32_t set  = (val)? 0 : mask;
      hi = (set | (hi & ~mask));
    }

    /** Gets the real/complex type used.
     *  @return The type used.
     */
    public: inline RealComplexType getRealComplexType () const {
      int32_t mask = 0x60000000;
      int32_t val  = (hi & mask) >> 29;
      return (RealComplexType)val;
    }

    /** Sets the real/complex type used.
     *  @param val The type used.
     *  @throws VRTException If the specified value is unsupported.
     */
    public: inline void setRealComplexType (RealComplexType val) {
      clearNull();
      int32_t mask = 0x60000000;
      int32_t set  = ((int32_t)val) << 29;
      hi = (set | (hi & ~mask));
    }

    /** <i>Utility method:</i> If this payload format matches one of the pre-defined data types,
     *  and is stored without any "extra bits", return that type. Although technically "unsigned"
     *  this method will recognize any 1-bit integer data as <tt>DataType_UInt1</tt>.
     *  Note: Considers all non-normalized data types as not pre-defined.
     *  @return The data type or null if not a pre-defined type.
     */
    public: DataType getDataType () const;

    /** <i>Utility method:</i> Sets the payload format to match one of the pre-defined data types.
     *  Note: Considers all non-normalized data types as not pre-defined.
     *  @param type The data type.
     *  @throws NullPointerException if type is null.
     */
    public: void setDataType (DataType type);

    /** <i>Utility method:</i> Indicates if the payload format is complex.
     *  @return <tt>true</tt> if the data format is complex, <tt>false</tt> if it is real.
     */
    public: inline bool isComplex () const {
      return (getRealComplexType() != RealComplexType_Real);
    }

    /** <i>Utility method:</i> Indicates if the payload format is signed.
     *  @return <tt>true</tt> if the data format is signed, <tt>false</tt> if it is unsigned.
     */
    public: inline bool isSigned () const {
      return (getDataItemFormat() < 16);
    }

    /** Gets the data format used.
     *  @return The format used.
     */
    public: inline DataItemFormat getDataItemFormat () const {
      int32_t mask = 0x1F000000;
      int32_t val  = (hi & mask) >> 24;
      return (DataItemFormat)val;
    }

    /** Sets the data format used.
     *  @param val The format used.
     *  @throws VRTException If the specified value is unsupported.
     */
    public: inline void setDataItemFormat (DataItemFormat val) {
      clearNull();
      int32_t mask = 0x1F000000;
      int32_t set  = ((int32_t)val) << 24;
      hi = (set | (hi & ~mask));
    }

    /** Indicates if the sample-component repeat is used.
     *  @return true if it is being used, false if not.
     */
    public: inline bool isRepeating () const {
      int32_t mask = 0x00800000;
      return ((hi & mask) != 0);
    }

    /** Indicates if the sample-component repeat is used.
     *  @param val true if it is being used, false if not.
     */
    public: inline void setRepeating (bool val) {
      clearNull();
      int32_t mask = 0x00800000;
      int32_t set  = (val)? mask : 0;
      hi = (set | (hi & ~mask));
    }

    /** Gets the event tag size.
     *  @return The event tag size (0..7).
     */
    public: inline int32_t getEventTagSize () const {
      int32_t mask = 0x00700000;
      int32_t val  = (hi & mask) >> 20;
      return val;
    }

    /** Sets the event tag size.
     *  @param val The event tag size (0..7).
     *  @throws VRTException If the specified value is not in the allowable range.
     */
    public: inline void setEventTagSize (int32_t val) {
      if ((val < 0) || (val > 7)) {
        throw VRTException("Value (%d) not in range of 0..7", val);
      }
      clearNull();
      int32_t mask = 0x00700000;
      int32_t set  = val << 20;
      hi = (set | (hi & ~mask));
    }

    /** Gets the channel tag size.
     *  @return The channel tag size (0..15).
     */
    public: inline int getChannelTagSize () const {
      int32_t mask = 0x000F0000;
      int32_t val  = (hi & mask) >> 16;
      return val;
    }

    /** Sets the channel tag size.
     *  @param val The channel tag size (0..15).
     *  @throws VRTException If the specified value is not in the allowable range.
     */
    public: inline void setChannelTagSize (int32_t val) {
      if ((val < 0) || (val > 15)) {
        throw VRTException("Value (%d) not in range of 0..15", val);
      }
      clearNull();
      int32_t mask = 0x000F0000;
      int32_t set  = val << 16;
      hi = (set | (hi & ~mask));
    }

    /** Gets the data item fractional size.
     *  @return The data item fractional size (0..15).
     */
    public: inline int32_t getDataItemFracSize () const {
      int32_t mask = 0x0000F000;
      int32_t val  = (hi & mask) >> 12;
      return val;
    }

    /** Sets the data item fractional size.
     *  @param val The data item fractional size (0..15).
     *  @throws VRTException If the specified value is not in the allowable range.
     */
    public: inline void setDataItemFracSize (int32_t val) {
      if ((val < 0) || (val > 15)) {
        throw VRTException("Value (%d) not in range of 0..15", val);
      }
      clearNull();
      int32_t mask = 0x0000F000;
      int32_t set  = val << 12;
      hi = (set | (hi & ~mask));
    }

    /** Gets the item packing field size.
     *  @return The field size (0..63).
     */
    public: inline int32_t getItemPackingFieldSize () const {
      int32_t mask = 0x00000FC0;
      int32_t val  = (hi & mask) >> 6;
      return val+1;
    }

    /** Sets the item packing field size.
     *  @param val The field size (0..63).
     *  @throws VRTException If the specified value is not in the allowable range.
     */
    public: inline void setItemPackingFieldSize (int32_t val) {
      if ((val < 1) || (val > 64)) {
        throw VRTException("Value (%d) not in range of 1..64", val);
      }
      clearNull();
      int32_t mask = 0x00000FC0;
      int32_t set  = (val-1) << 6;
      hi = (set | (hi & ~mask));
    }

    /** Gets the data item size.
     *  @return The data item size (0..63).
     */
    public: inline int32_t getDataItemSize () const {
      int32_t mask = 0x0000003F;
      int32_t val  = (hi & mask);
      return val+1;
    }

    /** Sets the data item size.
     *  @param val The data item size (0..63).
     *  @throws VRTException If the specified value is not in the allowable range.
     */
    public: inline void setDataItemSize (int32_t val) {
      if ((val < 1) || (val > 64)) {
        throw VRTException("Value (%d) not in range of 1..64", val);
      }
      clearNull();
      int32_t mask = 0x0000003F;
      int32_t set  = (val-1);
      hi = (set | (hi & ~mask));
    }

    /** Gets the repeat count.
     *  @return The repeat count (1..65536).
     */
    public: inline int32_t getRepeatCount () const {
      return (0x0000FFFF&(lo >> 16))+1;
    }

    /** Sets the repeat count.
     *  @param val The repeat count (1..65536).
     *  @throws VRTException If the specified value is not in the allowable range.
     */
    public: inline void setRepeatCount (int32_t val) {
      if ((val < 1) || (val > 65536)) {
        throw VRTException("Value (%d) not in range of 1..65536", val);
      }
      clearNull();
      lo = (((val-1) << 16) | (lo & 0x0000FFFF));
    }

    /** Gets the vector size.
     *  @return The vector size (1..65536).
     */
    public: inline int32_t getVectorSize () const {
      return (lo & 0x0000FFFF)+1;
    }

    /** Sets the vector size.
     *  @param val The vector size (1..65536).
     *  @throws VRTException If the specified value is not in the allowable range.
     */
    public: inline void setVectorSize (int32_t val) {
      if ((val < 1) || (val > 65536)) {
        throw VRTException("Value (%d) not in range of 1..65536", val);
      }
      clearNull();
      lo = ((val-1) | (lo & 0xFFFF0000));
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Implement HasFields
    //////////////////////////////////////////////////////////////////////////////////////////////////
    public: virtual int32_t   getFieldCount () const;
    public: virtual string    getFieldName  (int32_t id) const;
    public: virtual ValueType getFieldType  (int32_t id) const;
    public: virtual Value*    getField      (int32_t id) const __attribute__((warn_unused_result));
    public: virtual void      setField      (int32_t id, const Value* val);
  };
#endif /* NOT_USING_JNI */

#if NOT_USING_JNI
  /** Standard payload format: Real, signed 4-bit integer. */
  const PayloadFormat PayloadFormat_INT4        (DataType_Int4);
  /** Standard payload format: Real, signed 8-bit integer. */
  const PayloadFormat PayloadFormat_INT8        (DataType_Int8);
  /** Standard payload format: Real, signed 16-bit integer. */
  const PayloadFormat PayloadFormat_INT16       (DataType_Int16);
  /** Standard payload format: Real, signed 32-bit integer. */
  const PayloadFormat PayloadFormat_INT32       (DataType_Int32);
  /** Standard payload format: Real, signed 64-bit integer. */
  const PayloadFormat PayloadFormat_INT64       (DataType_Int64);
  ///** Standard payload format: Real, 16-bit float. */
  //const PayloadFormat PayloadFormat_FLOAT16     (DataType_Float16);
  /** Standard payload format: Real, 32-bit float. */
  const PayloadFormat PayloadFormat_FLOAT32     (DataType_Float);
  /** Standard payload format: Real, 64-bit double. */
  const PayloadFormat PayloadFormat_DOUBLE64    (DataType_Double);
  /** Standard payload format: Real, unsigned 1-bit packed bits. */
  const PayloadFormat PayloadFormat_UINT1       (DataType_UInt1);
  /** Standard payload format: Real, unsigned 4-bit integer. */
  const PayloadFormat PayloadFormat_UINT4       (DataType_UInt4);
  /** Standard payload format: Real, unsigned 8-bit integer. */
  const PayloadFormat PayloadFormat_UINT8       (DataType_UInt8);
  /** Standard payload format: Real, unsigned 16-bit integer. */
  const PayloadFormat PayloadFormat_UINT16      (DataType_UInt16);
  /** Standard payload format: Real, unsigned 32-bit integer. */
  const PayloadFormat PayloadFormat_UINT32      (DataType_UInt32);
  /** Standard payload format: Real, unsigned 64-bit integer. */
  const PayloadFormat PayloadFormat_UINT64      (DataType_UInt64);
#endif /* NOT_USING_JNI */

  namespace VRTMath {
    /** Converts from an IEEE-754 double-precision floating-point value to the
     *  packed bits for a VRT floating-point. <br>
     *  <br>
     *  Although VRT floating-point values can express many values in multiple
     *  ways within the binary form (e.g. 1/4 or 2/8) this function outputs the
     *  values in a "normalized form" where the output value will have a 1 in
     *  the most-significant-(non-sign-)bit of the mantissa, excepting for small
     *  values close to zero where the exponent is zero. This mirrors the behavior
     *  of IEEE-754 floating-point values, excepting that the highest bit is
     *  always present (not just for "denormal" values). <br>
     *  <br>
     *  Since the range of VRT floating-point numbers is [0,1) (signed) or [-1,1)
     *  (unsigned) any values below/above that range will be converted to the
     *  smallest/largest value possible in that range. Any NaN values passed in
     *  will be converted to zero.
     *  <pre>
     *    References:
     *      [1] ANSI/VITA 49.0, Appendix D
     *      [2] IEEE-754
     *  </pre>
     * @param form  The data item format. This must be one of the VRT
     *              floating-point formats.
     * @param dSize The data size in bits (2..63).
     * @param val   The value to convert.
     * @return The packed bits for the VRT floating-point number.
     */
    int64_t toVRTFloat (DataItemFormat form, int32_t dSize, double val); // code in VRTMath.cc

    /** <b>Internal Use Only:</b> Converts from an IEEE-754 double-precision
     *  floating-point value to the packed bits for a VRT floating-point
     *  <b>without normal safety checks</b>.
     *  @param sign  Is output signed?
     *  @param eSize The number of exponent bits (1 to 6).
     *  @param dSize The data size in bits (2..63).
     *  @param val   The value to convert.
     */
    int32_t toVRTFloat32 (bool sign, int32_t eSize, int32_t dSize, double val); // code in VRTMath.cc

    /** <b>Internal Use Only:</b> Converts from an IEEE-754 double-precision
     *  floating-point value to the packed bits for a VRT floating-point
     *  <b>without normal safety checks</b>.
     *  @param sign  Is output signed?
     *  @param eSize The number of exponent bits (1 to 6).
     *  @param dSize The data size in bits (2..63).
     *  @param val   The value to convert.
     */
    int64_t toVRTFloat64 (bool sign, int32_t eSize, int32_t dSize, double val); // code in VRTMath.cc

    /** Converts from the packed bits for a VRT floating-point to an IEEE-754
     *  double-precision floating-point value. <br>
     *  <br>
     *  Despite the fact that {@link #toVRTFloat} normalizes its output values,
     *  this function will support all of the valid VRT forms and convert them to
     *  their respective IEEE-754 value. Since IEEE-754 values are normalized this
     *  process results not reflective.
     *  <pre>
     *    References:
     *      [1] ANSI/VITA 49.0, Appendix D
     *      [2] IEEE-754
     *  </pre>
     *  @param form  The data item format. This must be one of the VRT
     *               floating-point formats.
     *  @param dSize The data size in bits (2..63).
     *  @param bits  The packed bits to convert.
     *  @return The IEEE-754 floating-point number.
     */
    double fromVRTFloat (DataItemFormat form, int32_t dSize, int64_t bits); // code in VRTMath.cc

    /** <b>Internal Use Only:</b> Converts from the packed bits for a VRT
     *  floating-point to an IEEE-754 double-precision floating-point value
     *  <b>without normal safety checks</b>.
     *  @param sign  Is output signed?
     *  @param eSize The number of exponent bits (1 to 6).
     *  @param dSize The data size in bits (2..63).
     *  @param bits  The packed bits to convert.
     */
    double fromVRTFloat32 (bool sign, int32_t eSize, int32_t dSize, int32_t bits); // code in VRTMath.cc

    /** <b>Internal Use Only:</b> Converts from the packed bits for a VRT
     *  floating-point to an IEEE-754 double-precision floating-point value
     *  <b>without normal safety checks</b>.
     *  @param sign  Is output signed?
     *  @param eSize The number of exponent bits (1 to 6).
     *  @param dSize The data size in bits (2..63).
     *  @param bits  The packed bits to convert.
     */
    double fromVRTFloat64 (bool sign, int32_t eSize, int32_t dSize, int64_t bits); // code in VRTMath.cc
  } END_NAMESPACE
} END_NAMESPACE

#if NOT_USING_JNI
string   operator+  (string  &s, vrt::RealComplexType val);
ostream& operator<< (ostream &s, vrt::RealComplexType val);
string   operator+  (string  &s, vrt::DataItemFormat val);
ostream& operator<< (ostream &s, vrt::DataItemFormat val);
string   operator+  (string  &s, vrt::DataType val);
ostream& operator<< (ostream &s, vrt::DataType val);
#endif /* NOT_USING_JNI */

#endif /* PayloadFormat_h */

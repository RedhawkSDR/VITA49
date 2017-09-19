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

#ifndef _BasicAcknowledgePacket_h
#define _BasicAcknowledgePacket_h

#include "BasicCommandPacket.h"
#include "IndicatorFields.h"
#include <cstring>
#include <vector>
#include <sstream>
#include <iomanip>


namespace vrt {
  using namespace std;
  using namespace VRTMath;
  using namespace IndicatorFields;

  namespace WarningErrorTypes {
    static const int32_t WEF_NULL                    = 0x00000000;
    static const int32_t WEF_NO_WARNING_ERROR        = 0x00000000;
    static const int32_t WEF_RESERVED_0              = 0x00000001;
    static const int32_t WEF_USER_DEFINED_1          = 0x00000002;
    static const int32_t WEF_USER_DEFINED_2          = 0x00000004;
    static const int32_t WEF_USER_DEFINED_3          = 0x00000008;
    static const int32_t WEF_USER_DEFINED_4          = 0x00000010;
    static const int32_t WEF_USER_DEFINED_5          = 0x00000020;
    static const int32_t WEF_USER_DEFINED_6          = 0x00000040;
    static const int32_t WEF_USER_DEFINED_7          = 0x00000080;
    static const int32_t WEF_USER_DEFINED_8          = 0x00000100;
    static const int32_t WEF_USER_DEFINED_9          = 0x00000200;
    static const int32_t WEF_USER_DEFINED_10         = 0x00000400;
    static const int32_t WEF_USER_DEFINED_11         = 0x00000800;
    static const int32_t WEF_USER_DEFINED_12         = 0x00001000;
    static const int32_t WEF_USER_DEFINED_13         = 0x00002000;
    static const int32_t WEF_USER_DEFINED_14         = 0x00004000;
    static const int32_t WEF_USER_DEFINED_15         = 0x00008000;
    static const int32_t WEF_USER_DEFINED_16         = 0x00010000;
    static const int32_t WEF_USER_DEFINED_17         = 0x00020000;
    static const int32_t WEF_USER_DEFINED_18         = 0x00040000;
    static const int32_t WEF_USER_DEFINED_19         = 0x00080000;
    static const int32_t WEF_USER_DEFINED_20         = 0x00100000;
    static const int32_t WEF_USER_DEFINED_21         = 0x00200000;
    static const int32_t WEF_USER_DEFINED_22         = 0x00400000;
    static const int32_t WEF_USER_DEFINED_23         = 0x00800000;
    static const int32_t WEF_RESERVED_24             = 0x01000000;
    static const int32_t TIMESTAMP_PROBLEM           = 0x02000000;
    static const int32_t PARAM_INVALID               = 0x04000000;
    static const int32_t PARAM_UNSUPPORTED_PRECISION = 0x08000000;
    static const int32_t PARAM_OUT_OF_RANGE          = 0x10000000;
    static const int32_t ERRONEOUS_FIELD             = 0x20000000;
    static const int32_t HARDWARE_DEVICE_FAILURE     = 0x40000000;
    static const int32_t FIELD_NOT_EXECUTED          = 0x80000000;
  }
  using namespace WarningErrorTypes;

  // Only needed for Acknowledge Packet sub-type (AckX and AckV)
  typedef struct WarningErrorField {
    IndicatorFieldEnum_t field;
    int32_t responseField;
  } WarningErrorField_t;

  /*****************************************************************************/
  /*****************************************************************************/
  /**                                                                         **/
  /**                      BasicAcknowledgePacket (AckV and AckX)             **/
  /**                                                                         **/
  /*****************************************************************************/
  /*****************************************************************************/
  class BasicAcknowledgePacket : public BasicCommandPacket, public IndicatorFieldProvider {

    //======================================================================
    // CONSTRUCTORS
    //======================================================================

    /** Basic destructor. */
    public: ~BasicAcknowledgePacket () { }

    /** Basic copy constructor. */
    public: BasicAcknowledgePacket (const BasicVRTPacket &p);

    /** Internal constructor, for use by subclasses.
     *  @param p       The packet (already initialized).
     *  @param classID The expected classID.
     *  @throws VRTException If packet type or classID of the packet are invalid.
     */
    public: BasicAcknowledgePacket (const BasicVRTPacket &p, int64_t classID);

    /** Creates a new instance accessing the given data buffer.
     *  @param buf      The pointer to the packet buffer.
     *  @param len      The length  of the packet buffer.
     *  @param readOnly Should users of this instance be able to modify the underlying data buffer?
     */
    public: BasicAcknowledgePacket (const void *buf, size_t len, bool readOnly=false);

    /** Creates a new instance that can be written to. Initially this will just be a simple context
     *  packet with no fields set (other than the required packet length of 8 and a default stream ID
     *  of 0).
     */
    public: BasicAcknowledgePacket ();

    /** Creates a new instance with a default anticipated length that can be written to.
     *  Initially this will just be a simple context packet with no fields set (other than
     *  the required packet length of 8 and a default stream ID  of 0), but will have the
     *  underlying buffers pre-allocated as required.
     *  @param bufsize The anticipated buffer size.
     */
    public: BasicAcknowledgePacket (int32_t bufsize);

    public: BasicAcknowledgePacket (const vector<char> &buf, ssize_t start, ssize_t end, bool readOnly = false);

    /** Creates a new instance accessing the given data buffer. Note that when the buffer lengths
     *  are given, only the most minimal of error checking is done. Users should call
     *  <tt>isPacketValid()</tt> to verify that the packet is valid. Invalid packets can result
     *  unpredictable behavior, but this is explicitly allowed (to the extent possible) so that
     *  applications creating packets can use this class even if the packet isn't yet "valid".
     *  @param buf      The data buffer to use.
     *  @param readOnly Should users of this instance be able to modify the underlying data buffer?
     *  @throws VRTException If the offset is negative or greater than the
     *          length of the buffer given. Also thrown if the buffer length is -1 and a copy
     *          can not be made because the sizes in the header of the packet are invalid.
     */
    public: BasicAcknowledgePacket (const vector<char> &buf, bool readOnly = false);

    public: BasicAcknowledgePacket (vector<char> *buf_ptr, bool readOnly = false);

    /** Internal constructor, for use by subclasses.
     *  @param p                The packet (already initialized).
     *  @param type             The expected packet type.
     *  @param classID          The expected classID.
     *  @param minPayloadLength The minimum payload length.
     *  @param maxPayloadLength The maximum payload length (&lt;0 if not pre-defined).
     *  @throws VRTException If packet type or classID of the packet are invalid, or if
     *                       the payload length is invalid.
     */
    protected: BasicAcknowledgePacket (const BasicVRTPacket &p, PacketType type, int64_t classID,
                                   int32_t minPayloadLength, int32_t maxPayloadLength);


    //======================================================================
    // UTILITY METHODS
    //======================================================================

    protected: virtual void      toStringStream (ostringstream &str) const;
    /*public:    virtual string    getPacketValid (bool strict, int32_t length) const {
      return BasicCommandPacket::getPacketValid(strict, length);
    }*/
    public: virtual inline void setPacketType (PacketType t) {
      if (t != PacketType_Command) {
        throw VRTException("Can not change packet type to non-Command type when using BasicAcknowledgePacket");
      }
      BasicVRTPacket::setPacketType(t);
    }

    // TODO - what new things need to be reset?
    /** Prepares a packet for resend resetting any non-persistent values and clearing the context
     *  change flags. This is implemented as follows:
     *  <!--
     *  <pre>
     *    bool BasicAcknowledgePacket::resetForResend (const TimeStamp &t) {
     *      BasicCommandPacket::resetForResend(t);
     *      return true;
     *    }
     *  </pre>
     *  @param t The new time stamp.
     *  @return true
     *  -->
     */
    //public: virtual bool resetForResend (const TimeStamp &t);



    //======================================================================
    // INDICATOR FIELD PROVIDER METHODS
    //======================================================================

    /** Gets the offset for the given context indicator field relative to the location of the first
     *  occurrence of CIF0. CIF7 is considered an invalid CIF number.
     *  For access to the second occurrence of CIFs (i.e. Error fields in Ack packets), add 8
     *  to the CIF number, effectively setting the 4th bit (mod8 gives field, div8 gives occurrence).
     *  @param cifNum number of CIF that field belongs to.
     *  @param field bitmask associated with field of interest.
     *  @return offset in bytes from beginning of CIF payload, or CIF7 as described above.
     *  @throws VRTException If the CIF number is invalid.
     */
    protected: virtual int32_t getOffset (int8_t cifNum, int32_t field) const;

    /** Gets the offset of the CIF7 attribute
     *  Offset is from the start of a field of size specified.
     *  @param attr CIF7 attribute bitmask
     *  @param len Length of the field
     *  @return Offset from start of field
     */
    protected: virtual int32_t getCif7Offset (int32_t attr, int32_t len) const;

    /** Gets the length of the given field when present (-1 if variable, -2 if not found).
     *  @param cifNum number of CIF that field belongs to.
     *  @param field bitmask associated with field of interest.
     *  @return size of field of interest in bytes.
     *  @throws VRTException If the CIF number is invalid.
     */
    // TODO - do CIF7 attributes affect this function?
    protected: virtual int32_t getFieldLen (int8_t cifNum, int32_t field) const;

    /** Get 32-bits from buffer as a 32-bit integer.
     *  Note: INT32_NULL is a valid value (0x80000000==FIELD_NOT_EXECUTED) in
     *  the context of Warning and Error fields. The value zero (0x0==WEF_NULL)
     *  has the same meaning as NULL in this context. This function returns 0x0
     *  (which is WEF_NULL==WEF_NO_WARNING_ERROR==0x0) when the field does not
     *  exist. This function returns 0x80000000 (i.e. INT32_NULL) when the field
     *  contains that value.
     *  @param cifNum Indicator field number
     *  @param bit Bit mask of field.
     *  @return 32-bit integer from buffer.
     */
    protected: virtual int32_t getL (int8_t cifNum, int32_t bit) const;

    /** Set 32-bits of buffer from a 32-bit integer.
     *  Note: INT32_NULL is a valid value (0x80000000==FIELD_NOT_EXECUTED) in
     *  the context of Warning and Error fields. The value zero (0x0==WEF_NULL)
     *  has the same meaning as NULL in this context. This function removes the
     *  field when set to 0x0 (which is WEF_NULL==WEF_NO_WARNING_ERROR==0x0).
     *  This function sets the value 0x80000000 (i.e. INT32_NULL) when the 
     *  argument contains that value.
     *  @param cifNum Indicator field number
     *  @param bit Bit mask of field.
     *  @param val 32-bit integer.
     */
    protected: virtual void setL (int8_t cifNum, int32_t bit, int32_t val);

    // Warnings and Errors are always a single 32-bit field. Throw VRTException("Not Implemented b/c not useful.");
    protected: virtual int8_t getB (int8_t cifNum, int32_t bit, int32_t xoff) const {
      UNUSED_VARIABLE(cifNum); UNUSED_VARIABLE(bit); UNUSED_VARIABLE(xoff);
      throw VRTException("All Warn/Error fields are 32 bits; BasicAcknowledgePacket only supports getL.");
    }
    protected: virtual void setB (int8_t cifNum, int32_t bit, int32_t xoff, int8_t val) {
      UNUSED_VARIABLE(cifNum); UNUSED_VARIABLE(bit); UNUSED_VARIABLE(xoff); UNUSED_VARIABLE(val);
      throw VRTException("All Warn/Error fields are 32 bits; BasicAcknowledgePacket only supports setL.");
    }
    protected: virtual int16_t getI (int8_t cifNum, int32_t bit, int32_t xoff) const {
      UNUSED_VARIABLE(cifNum); UNUSED_VARIABLE(bit); UNUSED_VARIABLE(xoff);
      throw VRTException("All Warn/Error fields are 32 bits; BasicAcknowledgePacket only supports getL.");
    }
    protected: virtual void setI (int8_t cifNum, int32_t bit, int32_t xoff, int16_t val) {
      UNUSED_VARIABLE(cifNum); UNUSED_VARIABLE(bit); UNUSED_VARIABLE(xoff); UNUSED_VARIABLE(val);
      throw VRTException("All Warn/Error fields are 32 bits; BasicAcknowledgePacket only supports setL.");
    }
    protected: virtual int32_t getL24 (int8_t cifNum, int32_t bit, int32_t offset) const {
      UNUSED_VARIABLE(cifNum); UNUSED_VARIABLE(bit); UNUSED_VARIABLE(offset);
      throw VRTException("All Warn/Error fields are 32 bits; BasicAcknowledgePacket only supports getL.");
    }
    protected: virtual int64_t getX (int8_t cifNum, int32_t bit) const {
      UNUSED_VARIABLE(cifNum); UNUSED_VARIABLE(bit);
      throw VRTException("All Warn/Error fields are 32 bits; BasicAcknowledgePacket only supports getL.");
    }
    protected: virtual void setX (int8_t cifNum, int32_t bit, int64_t val) {
      UNUSED_VARIABLE(cifNum); UNUSED_VARIABLE(bit); UNUSED_VARIABLE(val);
      throw VRTException("All Warn/Error fields are 32 bits; BasicAcknowledgePacket only supports setL.");
    }
    protected: virtual void setRecord (int8_t cifNum, int32_t bit, const Record *val, int32_t oldLen) {
      UNUSED_VARIABLE(cifNum); UNUSED_VARIABLE(bit); UNUSED_VARIABLE(val); UNUSED_VARIABLE(oldLen);
      throw VRTException("All Warn/Error fields are 32 bits; BasicAcknowledgePacket only supports setL.");
    }
    protected: virtual Geolocation getGeolocation (int8_t cifNum, int32_t field) const {
      UNUSED_VARIABLE(cifNum); UNUSED_VARIABLE(field);
      throw VRTException("All Warn/Error fields are 32 bits; BasicAcknowledgePacket only supports getL.");
    }
    protected: virtual Ephemeris getEphemeris (int8_t cifNum, int32_t field) const {
      UNUSED_VARIABLE(cifNum); UNUSED_VARIABLE(field);
      throw VRTException("All Warn/Error fields are 32 bits; BasicAcknowledgePacket only supports getL.");
    }
    public: virtual GeoSentences getGeoSentences () const {
      throw VRTException("All Warn/Error fields are 32 bits; BasicAcknowledgePacket only supports getL.");
    }
    public: virtual ContextAssocLists getContextAssocLists () const {
      throw VRTException("All Warn/Error fields are 32 bits; BasicAcknowledgePacket only supports getL.");
    }
    protected: virtual boolNull getStateEventBit (int32_t enable, int32_t indicator) const {
      UNUSED_VARIABLE(enable); UNUSED_VARIABLE(indicator);
      throw VRTException("All Warn/Error fields are 32 bits; BasicAcknowledgePacket only supports getL.");
    }
    protected: virtual void setStateEventBit (int32_t enable, int32_t indicator, boolNull value) {
      UNUSED_VARIABLE(enable); UNUSED_VARIABLE(indicator); UNUSED_VARIABLE(value);
      throw VRTException("All Warn/Error fields are 32 bits; BasicAcknowledgePacket only supports setL.");
    }

    protected: virtual int32_t getContextIndicatorField0 (bool occurrence=0) const;
    protected: virtual int32_t getContextIndicatorField1 (bool occurrence=0) const;
    protected: virtual int32_t getContextIndicatorField2 (bool occurrence=0) const;
    protected: virtual int32_t getContextIndicatorField3 (bool occurrence=0) const;
    protected: virtual int32_t getContextIndicatorField7 (bool occurrence=0) const;

    protected: virtual void setContextIndicatorField0Bit (int32_t bit, bool set, bool occurrence=0);
    protected: virtual void setContextIndicatorField1Bit (int32_t bit, bool set, bool occurrence=0);
    protected: virtual void setContextIndicatorField2Bit (int32_t bit, bool set, bool occurrence=0);
    protected: virtual void setContextIndicatorField3Bit (int32_t bit, bool set, bool occurrence=0);
    protected: virtual void setContextIndicatorField7Bit (int32_t bit, bool set, bool occurrence=0);


    /** Adds or removes a CIF
     *  Sets the Enable Indicator of CIF0 and adds or removes 4-bytes for the CIF.
     *  @param add True = add CIF (default), False = remove CIF
     *  @param occurrence False (0) if first occurrence (default, Warnings), True (1) if second
     *  occurrence (Errors)
     */
    public: virtual void addCIF1 (bool add=true, bool occurrence=false);
    public: virtual void addCIF2 (bool add=true, bool occurrence=false);
    public: virtual void addCIF3 (bool add=true, bool occurrence=false);
    public: virtual void addCIF7 (bool add=true, bool occurrence=false);

    //Acknowledge-W
    public: virtual void setWarningsGenerated (bool set);

    //Acknowledge-E
    public: virtual void setErrorsGenerated (bool set);

    //======================================================================
    // ACKNOWLEDGE PACKET METHODS
    //======================================================================

    // Warning Indicator Fields
    protected: int32_t getWarningIndicatorField(int8_t wifNum) const {
      return getContextIndicatorField(wifNum);
    }
    protected: int32_t getWarningIndicatorField(IndicatorFieldEnum_t field) const {
      return getContextIndicatorField(field);
    }
    protected: boolNull getWarningIndicatorFieldBit(IndicatorFieldEnum_t field) const {
      return getContextIndicatorFieldBit(field);
    }

    // Error Indicator Fields
    protected: int32_t getErrorIndicatorField(int8_t wifNum) const {
      return getContextIndicatorField(wifNum | 0x8);
    }
    protected: int32_t getErrorIndicatorField(IndicatorFieldEnum_t field) const {
      return getContextIndicatorField(getCIFNumber(field) | 0x8);
    }
    protected: boolNull getErrorIndicatorFieldBit(IndicatorFieldEnum_t field) const {
      return getContextIndicatorFieldBit(getCIFNumber(field) | 0x8, getCIFBitMask(field));
    }

    // Warning Fields

    /** Gets the warning bit field for the specified indicator field.
     *  @param field Indicator field
     *  @return Warning bit field (0x0 or WEF_NULL or 
     *  WEF_NO_WARNING_ERROR if no warnings).
     */
    public: int32_t getWarning(IndicatorFieldEnum_t field) const {
      return getL(getCIFNumber(field), getCIFBitMask(field));
    }

    /** Sets the warning bit field for the specified indicator field.
     *  @param field Indicator field
     *  @param val Warning bit field (0x0 or WEF_NULL or 
     *  WEF_NO_WARNING_ERROR if no warnings).
     */
    public: void setWarning(IndicatorFieldEnum_t field, int32_t val) {
      setL(getCIFNumber(field), getCIFBitMask(field), val);
    }
    
    /** Adds one or more warnings to the warning bit field for the specified
     *  indicator field. 
     *  @param field Indicator field
     *  @param val Warning bit field to add (set) to the warning bit field. 
     */
    public: void addWarning(IndicatorFieldEnum_t field, int32_t val) {
      if (val == WEF_NULL) return;
      int32_t bits = getL(getCIFNumber(field), getCIFBitMask(field));
      setL(getCIFNumber(field), getCIFBitMask(field), val | bits);
    }

    /** Removes one or more warnings from the warning bit field for the
     *  specified indicator field.
     *  @param field Indicator field
     *  @param val Warning bit field to remove (unset) from the warning bit
     *  field.
     */
    public: void removeWarning(IndicatorFieldEnum_t field, int32_t val) {
      if (val == WEF_NULL) return;
      int32_t bits = getL(getCIFNumber(field), getCIFBitMask(field));
      setL(getCIFNumber(field), getCIFBitMask(field), ~val & bits);
    }

    /** Gets all warnings for all indicator fields.
     *  @return Vector of warnings.
     */
    public: std::vector<WarningErrorField_t> getWarnings() const;

    // Error Fields
    
    /** Gets the error bit field for the specified indicator field.
     *  @param field Indicator field
     *  @return Error bit field (0x0 or WEF_NULL or 
     *  WEF_NO_WARNING_ERROR if no errors).
     */
    public: int32_t getError(IndicatorFieldEnum_t field) const {
      return getL(getCIFNumber(field) | 0x8, getCIFBitMask(field));
    }

    /** Sets the error bit field for the specified indicator field.
     *  @param field Indicator field
     *  @param val Error bit field (0x0 or WEF_NULL or 
     *  WEF_NO_WARNING_ERROR if no errors).
     */
    public: void setError(IndicatorFieldEnum_t field, int32_t val){
      setL(getCIFNumber(field) | 0x8, getCIFBitMask(field), val);
    }
    
    /** Adds one or more errors to the error bit field for the specified
     *  indicator field.
     *  @param field Indicator field
     *  @param val Error bit field to add (set) to the error bit field.
     */
    public: void addError(IndicatorFieldEnum_t field, int32_t val) {
      if (val == WEF_NULL) return;
      int32_t bits = getL(getCIFNumber(field) | 0x8, getCIFBitMask(field));
      setL(getCIFNumber(field) | 0x8, getCIFBitMask(field), val | bits);
    }

    /** Removes one or more errors from the error bit field for the
     *  specified indicator field.
     *  @param field Indicator field
     *  @param val Error bit field to remove (unset) from the error bit
     *  field.
     */
    public: void removeError(IndicatorFieldEnum_t field, int32_t val) {
      if (val == WEF_NULL) return;
      int32_t bits = getL(getCIFNumber(field) | 0x8, getCIFBitMask(field));
      setL(getCIFNumber(field) | 0x8, getCIFBitMask(field), ~val & bits);
    }

    /** Gets all errors for all indicator fields.
     *  @return Vector of errors.
     */
    public: std::vector<WarningErrorField_t> getErrors() const;


    // TODO - get free-form (textual) message

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Implement HasFields
    //////////////////////////////////////////////////////////////////////////////////////////////////
    /* TODO
    public: virtual int32_t   getFieldCount () const;
    public: virtual string    getFieldName  (int32_t id) const;
    public: virtual ValueType getFieldType  (int32_t id) const;
    public: virtual Value*    getField      (int32_t id) const __attribute__((warn_unused_result));
    public: virtual void      setField      (int32_t id, const Value* val);
    */

  }; /*  BasicAcknowledgePacket */


} END_NAMESPACE /* vrt namespace */

#endif /* _BasicAcknowledgePacket_h */

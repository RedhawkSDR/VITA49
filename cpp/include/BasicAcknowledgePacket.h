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

  /** Namespace for Warning and Error definitions
   */
  namespace WarningErrorTypes {
    static const int32_t WEF_NULL                     = 0x00000000; //!< NULL Warning/Error
    static const int32_t WEF_NO_WARNING_ERROR         = 0x00000000; //!< No Warning/Error (same as NULL)
    static const int32_t WEF_RESERVED_0               = 0x00000001; //!< Reserved
    static const int32_t WEF_USER_DEFINED_1           = 0x00000002; //!< User Defined Warning/Error Bit 1
    static const int32_t WEF_USER_DEFINED_2           = 0x00000004; //!< User Defined Warning/Error Bit 2
    static const int32_t WEF_USER_DEFINED_3           = 0x00000008; //!< User Defined Warning/Error Bit 3
    static const int32_t WEF_USER_DEFINED_4           = 0x00000010; //!< User Defined Warning/Error Bit 4
    static const int32_t WEF_USER_DEFINED_5           = 0x00000020; //!< User Defined Warning/Error Bit 5
    static const int32_t WEF_USER_DEFINED_6           = 0x00000040; //!< User Defined Warning/Error Bit 6
    static const int32_t WEF_USER_DEFINED_7           = 0x00000080; //!< User Defined Warning/Error Bit 7
    static const int32_t WEF_USER_DEFINED_8           = 0x00000100; //!< User Defined Warning/Error Bit 8
    static const int32_t WEF_USER_DEFINED_9           = 0x00000200; //!< User Defined Warning/Error Bit 9
    static const int32_t WEF_USER_DEFINED_10          = 0x00000400; //!< User Defined Warning/Error Bit 10
    static const int32_t WEF_USER_DEFINED_11          = 0x00000800; //!< User Defined Warning/Error Bit 11
    static const int32_t WEF_USER_DEFINED_12          = 0x00001000; //!< User Defined Warning/Error Bit 12
    static const int32_t WEF_RESERVED_13              = 0x00002000; //!< Reserved Warning/Error Bit 13
    static const int32_t WEF_RESERVED_14              = 0x00004000; //!< Reserved Warning/Error Bit 14
    static const int32_t WEF_RESERVED_15              = 0x00008000; //!< Reserved Warning/Error Bit 15
    static const int32_t WEF_RESERVED_16              = 0x00010000; //!< Reserved Warning/Error Bit 16
    static const int32_t WEF_RESERVED_17              = 0x00020000; //!< Reserved Warning/Error Bit 17
    static const int32_t WEF_RESERVED_18              = 0x00040000; //!< Reserved Warning/Error Bit 18
    static const int32_t REGIONAL_INTERFERENCE        = 0x00080000; //!< Interference between devices in the same operational region
    static const int32_t CO_SITE_INTERFERENCE         = 0x00100000; //!< Co-site interference between tx and rx at same location
    static const int32_t OUT_OF_BAND_POWER_COMPLIANCE = 0x00200000; //!< Out-of-band power levels out of compliance
    static const int32_t IN_BAND_POWER_COMPLIANCE     = 0x00400000; //!< In-band power levels out of compliance
    static const int32_t DISTORTION                   = 0x00800000; //!< Components over driven leading to distortion
    static const int32_t HAZARDOUS_POWER_LEVELS       = 0x01000000; //!< Transmission of hazardous power levels
    static const int32_t TIMESTAMP_PROBLEM            = 0x02000000; //!< Controllee unable to meet tstamp requirements
    static const int32_t FIELD_VALUE_INVALID          = 0x04000000; //!< Invalid setting not covered by other code
    static const int32_t PARAM_UNSUPPORTED_PRECISION  = 0x08000000; //!< Level of precision beyond device capabilities
    static const int32_t PARAM_OUT_OF_RANGE           = 0x10000000; //!< Beyond capability or operational range of device
    static const int32_t ERRONEOUS_FIELD              = 0x20000000; //!< Device does not accept this control field
    static const int32_t DEVICE_FAILURE               = 0x40000000; //!< Not executed properly because of device failure
    static const int32_t FIELD_NOT_EXECUTED           = 0x80000000; //!< Not executed because of a Warning or Error
  }
  using namespace WarningErrorTypes;

  /** Struct to represent an indicator field and associated warning or error
   *  field. Only needed for Acknowledge Packet sub-type (AckX and AckV)
   */
  typedef struct WarningErrorField {
    IndicatorFieldEnum_t field; ///< Indicator field enumeration
    int32_t responseField;      ///< Warning/Error bit field
  } WarningErrorField_t;

  // Constant to use in place of CIF# for getting offset to Free-form Warning/Error message
  static const int8_t FREE_FORM_MESSAGE = (int8_t) 0xFF; // (int8_t) 0xFF == -1

  /** Struct to assist getting/setting Free-form Warning/Error Message Field */
  typedef struct FreeFormMessage {
    /** Default constructor for empty Free-form Warning/Error message. */
    FreeFormMessage () : bbuf(0) {
    }
    /** Constructor that accepts a Free-form Warning/Error message as a string.
     *  The string message is stored as a null-terminated byte-buffer. The size
     *  of the resulting byte-buffer is one byte more than the number of
     *  characters in the input message string.
     *  @param msg Message string
     */
    FreeFormMessage (const std::string msg) : 
          bbuf(msg.size()+1) { // one extra for null terminator
      if (msg.size()==0) {
        bbuf.resize(0);
      } else {
        memcpy(&bbuf[0], &msg[0], msg.size());
        bbuf[msg.size()] = '\0';
      }
    }
    /** Constructor that accepts a Free-form Warning/Error message as a byte-
     *  buffer. A null-terminator is NOT added to the byte sequence. If null-
     *  termination is desired, it must be included in the input byte-buffer.
     *  The size of the resulting byte-buffer is equal to the size of input
     *  message byte-buffer.
     *  @param msg Message byte-buffer
     *  @param len Size of byte-buffer
     */
    FreeFormMessage (const char* msg, const size_t len) : bbuf(len) {
      memcpy(&bbuf[0], msg, len);
    }
    /** Sets Free-form Warning/Error message from a string. The string message
     *  is stored as a null-terminated byte-buffer. The size of the resulting
     *  byte-buffer is one byte more than the number of characters in the input
     *  message string.
     *  @param msg Message string
     */
    void setMessage (const std::string msg) {
      if (msg.size()==0) {
        bbuf.resize(0);
      } else {
        bbuf.resize(msg.size()+1); // one extra for null terminator
        memcpy(&bbuf[0], &msg[0], msg.size());
        bbuf[msg.size()] = '\0';
      }
    }
    /** Sets Free-form Warning/Error message from a byte-buffer. A null-
     *  terminator is NOT added to the byte sequence. If null-termination is
     *  desired, it must be included in the input byte-buffer. The size of the
     *  resulting byte-buffer is equal to the size of input message byte-buffer.
     *  @param msg Message byte-buffer
     *  @param len Size of byte-buffer
     */
    void setMessage (const char* msg, const size_t len) {
      bbuf.resize(len);
      memcpy(&bbuf[0], msg, len);
    }
    /** Gets the string representation of the Free-form Warning/Error message.
     *  A message with a null-terminator as the last byte will not include the
     *  null-terminator in the string representation (i.e. the length of the
     *  string will be one less than the size of the message byte-buffer).
     *  Otherwise, the size of the string will be equal to the size of the byte-
     *  buffer. An empty message results in an empty string.
     *  @return String representation of the message
     */
    std::string text () const {
      if (bbuf.size() == 0) { // no/empty message
        return "";
      } else if (bbuf[bbuf.size()-1] == '\0') { // null terminated c-string
        return std::string((char*) &bbuf[0]);
      } else { // non null terminated char array
        return std::string((char*) &bbuf[0], bbuf.size());
      }
    }
    /** Gets a const pointer to the first byte of the Free-form Warning/Error
     *  message byte-buffer. This is intended as a read-only pointer that must
     *  be used in conjunction with size() method. Use one of the setMessage()
     *  methods to set or modify the message.
     *  Warning: the pointer is not guaranteed to be valid, and may in fact
     *  become invalid if either setMessage() method is called afterward. Use
     *  with caution.
     *  @return Message byte-buffer pointer
     */
    const char* buffer () const {
      return &bbuf[0];
    }
    /** Gets size of Free-form Warning/Error message.
     *  @return Size of message (byte-buffer)
     */
    size_t size () const {
      return bbuf.size();
    }
    /** Is Free-form Warning/Error message empty/blank?
     *  @return True if message is empty; False otherwise.
     */
    bool empty () const {
      return bbuf.size()==0;
    }
    std::vector<char> bbuf; ///> Internal byte-buffer containing the message
  } FreeFormMessage_t;

  /*****************************************************************************/
  /*****************************************************************************/
  /**                                                                         **/
  /**                      BasicAcknowledgePacket (AckV and AckX)             **/
  /**                                                                         **/
  /*****************************************************************************/
  /*****************************************************************************/
  class BasicAcknowledgePacket : public BasicCommandPacket, public IndicatorFieldProvider {
    using IndicatorFieldProvider::getOffset;

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

    /** Creates a new instance that can be written to. Initially this will just be a simple command
     *  packet with StreamID, ClassID, Ack Indicator set, TSI=UTC, TSF=Real-time, CAM field, MessageID,
     *  128-bit ControlleeID, 128-bit ControllerID, Execution Acknowledge bit set, and a packet size of
     *  17 (no warning/error indicator fields until warning/error generated bit(s) set).
     */
    public: BasicAcknowledgePacket ();

    /** Creates a new instance with a default anticipated length that can be written to.
     *  Initially this will just be a simple command packet with StreamID, ClassID, Ack Indicator set,
     *  TSI=UTC, TSF=Real-time, CAM field, MessageID, 128-bit ControlleeID, 128-bit ControllerID,
     *  Execution Acknowledge bit set, and a packet size of 17 (no warning/error indicator fields until
     *  warning/error generated bit(s) set). The underlying buffers will be pre-allocated as required.
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
     *  For the Free-form Warning/Error Message Field, set the CIF# to 0xFF (-1).
     *  @param cifNum number of CIF that field belongs to; or -1 for Free-form message field.
     *  @param field bitmask associated with field of interest; ignored for Free-form message field.
     *  @return offset in bytes from beginning of CIF payload.
     *  @throws VRTException If the CIF number is invalid.
     */
    protected: virtual int32_t getOffset (int8_t cifNum, int32_t field) const;

    /** Gets the offset of the CIF7 attribute
     *  Offset is from the start of a field of size specified.
     *  @param attr CIF7 attribute bitmask
     *  @param len Length of the field
     *  @param  occurrence False (0) if first occurrence (default; almost everything),
     *                     True (1) if second occurrence (only Errors for BasicAcknowledgePacket)
     *  @return Offset from start of field
     */
    protected: virtual int32_t getCIF7Offset (int32_t attr, int32_t len, bool occurrence=0) const;

    /** Gets the total size of the field, including all CIF7 attributes
     *  @param  fieldLen Length of the field w/o including CIF7 attributes
     *  @param  occurrence False (0) if first occurrence (default; almost everything),
     *                     True (1) if second occurrence (only Errors for BasicAcknowledgePacket)
     *  @return Length of the field including CIF7 attributes
     */
    protected: virtual int32_t getTotalFieldSize (int32_t fieldLen, bool occurrence=0) const;

    /** Gets the length of the given field when present (-1 if variable, -2 if not found).
     *  @param cifNum number of CIF that field belongs to.
     *  @param field bitmask associated with field of interest.
     *  @param parent length of parent field
     *  @return size of field of interest in bytes.
     *  @throws VRTException If the CIF number is invalid.
     */
    protected: virtual int32_t getFieldLen (int8_t cifNum, int32_t field, int32_t parent=0) const;

    /** Get 32-bits from buffer as a 32-bit integer.
     *  Note: INT32_NULL is a valid value (0x80000000==FIELD_NOT_EXECUTED) in
     *  the context of Warning and Error fields. The value zero (0x0==WEF_NULL)
     *  has the same meaning as NULL in this context. This function returns 0x0
     *  (which is WEF_NULL==WEF_NO_WARNING_ERROR==0x0) when the field does not
     *  exist. This function returns 0x80000000 (i.e. INT32_NULL) when the field
     *  contains that value.
     *  @param cifNum Indicator field number
     *  @param bit Bit mask of field.
     *  @param cif7bit bitmask of CIF7 attribute
     *  @return 32-bit integer from buffer.
     */
    protected: virtual int32_t getL (int8_t cifNum, int32_t bit, int32_t cif7bit) const;

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
     *  @param cif7bit bitmask of CIF7 attribute
     */
    protected: virtual void setL (int8_t cifNum, int32_t bit, int32_t val, int32_t cif7bit);

    // Warnings and Errors are always a single 32-bit field. Throw VRTException("Not Implemented b/c not useful.");
    protected: virtual int8_t getB (int8_t cifNum, int32_t bit, int32_t xoff, int32_t cif7bit) const {
      UNUSED_VARIABLE(cifNum); UNUSED_VARIABLE(bit); UNUSED_VARIABLE(xoff); UNUSED_VARIABLE(cif7bit);
      throw VRTException("All Warn/Error fields are 32 bits; BasicAcknowledgePacket only supports getL.");
    }
    protected: virtual void setB (int8_t cifNum, int32_t bit, int32_t xoff, int8_t val, int32_t cif7bit) {
      UNUSED_VARIABLE(cifNum); UNUSED_VARIABLE(bit); UNUSED_VARIABLE(xoff); UNUSED_VARIABLE(val); UNUSED_VARIABLE(cif7bit);
      throw VRTException("All Warn/Error fields are 32 bits; BasicAcknowledgePacket only supports setL.");
    }
    protected: virtual int16_t getI (int8_t cifNum, int32_t bit, int32_t xoff, int32_t cif7bit) const {
      UNUSED_VARIABLE(cifNum); UNUSED_VARIABLE(bit); UNUSED_VARIABLE(xoff); UNUSED_VARIABLE(cif7bit);
      throw VRTException("All Warn/Error fields are 32 bits; BasicAcknowledgePacket only supports getL.");
    }
    protected: virtual void setI (int8_t cifNum, int32_t bit, int32_t xoff, int16_t val, int32_t cif7bit) {
      UNUSED_VARIABLE(cifNum); UNUSED_VARIABLE(bit); UNUSED_VARIABLE(xoff); UNUSED_VARIABLE(val); UNUSED_VARIABLE(cif7bit);
      throw VRTException("All Warn/Error fields are 32 bits; BasicAcknowledgePacket only supports setL.");
    }
    protected: virtual int32_t getL24 (int8_t cifNum, int32_t bit, int32_t offset, int32_t cif7bit) const {
      UNUSED_VARIABLE(cifNum); UNUSED_VARIABLE(bit); UNUSED_VARIABLE(offset); UNUSED_VARIABLE(cif7bit);
      throw VRTException("All Warn/Error fields are 32 bits; BasicAcknowledgePacket only supports getL.");
    }
    protected: virtual int64_t getX (int8_t cifNum, int32_t bit, int32_t cif7bit) const {
      UNUSED_VARIABLE(cifNum); UNUSED_VARIABLE(bit); UNUSED_VARIABLE(cif7bit);
      throw VRTException("All Warn/Error fields are 32 bits; BasicAcknowledgePacket only supports getL.");
    }
    protected: virtual void setX (int8_t cifNum, int32_t bit, int64_t val, int32_t cif7bit) {
      UNUSED_VARIABLE(cifNum); UNUSED_VARIABLE(bit); UNUSED_VARIABLE(val); UNUSED_VARIABLE(cif7bit);
      throw VRTException("All Warn/Error fields are 32 bits; BasicAcknowledgePacket only supports setL.");
    }
    protected: virtual UUID getUUID (int8_t cifNum, int32_t bit, int32_t cif7bit) const {
      UNUSED_VARIABLE(cifNum); UNUSED_VARIABLE(bit); UNUSED_VARIABLE(cif7bit);
      throw VRTException("All Warn/Error fields are 32 bits; BasicAcknowledgePacket only supports getL.");
    }
    protected: virtual void setUUID (int8_t cifNum, int32_t bit, const UUID &val, int32_t cif7bit){
      UNUSED_VARIABLE(cifNum); UNUSED_VARIABLE(bit); UNUSED_VARIABLE(val); UNUSED_VARIABLE(cif7bit);
      throw VRTException("All Warn/Error fields are 32 bits; BasicAcknowledgePacket only supports setL.");
    }
    protected: virtual TimeStamp getTimeStampField (int8_t cifNum, int32_t bit, int32_t cif7bit) const {
      UNUSED_VARIABLE(cifNum); UNUSED_VARIABLE(bit); UNUSED_VARIABLE(cif7bit);
      throw VRTException("All Warn/Error fields are 32 bits; BasicAcknowledgePacket only supports getL.");
    }
    protected: virtual void setTimeStampField (int8_t cifNum, int32_t bit, const TimeStamp &val, int32_t cif7bit){
      UNUSED_VARIABLE(cifNum); UNUSED_VARIABLE(bit); UNUSED_VARIABLE(val); UNUSED_VARIABLE(cif7bit);
      throw VRTException("All Warn/Error fields are 32 bits; BasicAcknowledgePacket only supports setL.");
    }
    protected: virtual void setRecord (int8_t cifNum, int32_t bit, const Record *val, int32_t oldLen, int32_t cif7bit) {
      UNUSED_VARIABLE(cifNum); UNUSED_VARIABLE(bit); UNUSED_VARIABLE(val); UNUSED_VARIABLE(oldLen); UNUSED_VARIABLE(cif7bit);
      throw VRTException("All Warn/Error fields are 32 bits; BasicAcknowledgePacket only supports setL.");
    }
    protected: virtual Geolocation getGeolocation (int8_t cifNum, int32_t field, int32_t cif7bit) const {
      UNUSED_VARIABLE(cifNum); UNUSED_VARIABLE(field); UNUSED_VARIABLE(cif7bit);
      throw VRTException("All Warn/Error fields are 32 bits; BasicAcknowledgePacket only supports getL.");
    }
    protected: virtual Ephemeris getEphemeris (int8_t cifNum, int32_t field, int32_t cif7bit) const {
      UNUSED_VARIABLE(cifNum); UNUSED_VARIABLE(field); UNUSED_VARIABLE(cif7bit);
      throw VRTException("All Warn/Error fields are 32 bits; BasicAcknowledgePacket only supports getL.");
    }
    public: virtual GeoSentences getGeoSentences (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      UNUSED_VARIABLE(cif7field);
      throw VRTException("All Warn/Error fields are 32 bits; BasicAcknowledgePacket only supports getL.");
    }
    public: virtual ContextAssocLists getContextAssocLists (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      UNUSED_VARIABLE(cif7field);
      throw VRTException("All Warn/Error fields are 32 bits; BasicAcknowledgePacket only supports getL.");
    }
    public: virtual IndexFieldList getIndexList (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      UNUSED_VARIABLE(cif7field);
      throw VRTException("All Warn/Error fields are 32 bits; BasicAcknowledgePacket only supports getL.");
    }
    public: virtual ArrayOfRecords getSectorScanStep (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      UNUSED_VARIABLE(cif7field);
      throw VRTException("All Warn/Error fields are 32 bits; BasicAcknowledgePacket only supports getL.");
    }
    public: virtual ArrayOfRecords getCIFsArray (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      UNUSED_VARIABLE(cif7field);
      throw VRTException("All Warn/Error fields are 32 bits; BasicAcknowledgePacket only supports getL.");
    }
    public: virtual ArrayOfRecords get3DPointingVectorStructured (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      UNUSED_VARIABLE(cif7field);
      throw VRTException("All Warn/Error fields are 32 bits; BasicAcknowledgePacket only supports getL.");
    }
    public: virtual SpectrumField getSpectrumField (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      UNUSED_VARIABLE(cif7field);
      throw VRTException("All Warn/Error fields are 32 bits; BasicAcknowledgePacket only supports getL.");
    }
    public: virtual PayloadFormat getDataPayloadFormat (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      UNUSED_VARIABLE(cif7field);
      throw VRTException("All Warn/Error fields are 32 bits; BasicAcknowledgePacket only supports getL.");
    }
    public: virtual void setDataPayloadFormat (const PayloadFormat &val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      UNUSED_VARIABLE(val); UNUSED_VARIABLE(cif7field);
      throw VRTException("All Warn/Error fields are 32 bits; BasicAcknowledgePacket only supports setL.");
    }
    protected: virtual boolNull getStateEventBit (int32_t enable, int32_t indicator, int32_t cif7bit) const {
      UNUSED_VARIABLE(enable); UNUSED_VARIABLE(indicator); UNUSED_VARIABLE(cif7bit);
      throw VRTException("All Warn/Error fields are 32 bits; BasicAcknowledgePacket only supports getL.");
    }
    protected: virtual void setStateEventBit (int32_t enable, int32_t indicator, boolNull value, int32_t cif7bit) {
      UNUSED_VARIABLE(enable); UNUSED_VARIABLE(indicator); UNUSED_VARIABLE(value); UNUSED_VARIABLE(cif7bit);
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

    protected: inline bool isCIFEnable (int8_t cifNum) const {
      bool occurrence = cifNum&0x08; // false for warn / true for error
      switch(cifNum&0xF7) { // Ignore bit 3 (occurrence)
        case 0:
          if ((!occurrence && getWarningsGenerated()) || (occurrence && getErrorsGenerated()))
            return true;
          else
            return false;
        case 1: return isCIF1Enable(cifNum&0x08);
        case 2: return isCIF2Enable(cifNum&0x08);
        case 3: return isCIF3Enable(cifNum&0x08);
        case 7: return isCIF7Enable(cifNum&0x08);
        default:
          throw VRTException("Invalid Context Indicator Field number.");
      }
    }



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

    protected: virtual void setCIF7Bit (int32_t cif7bit, bool set, bool occurrence=false);

    //Acknowledge-W
    public: virtual void setWarningsGenerated (bool set);

    //Acknowledge-E
    public: virtual void setErrorsGenerated (bool set);

    //======================================================================
    // ACKNOWLEDGE PACKET METHODS
    //======================================================================

    // Warning Indicator Fields
    protected: int32_t getWarningIndicatorField (int8_t wifNum) const {
      return getContextIndicatorField(wifNum);
    }
    protected: int32_t getWarningIndicatorField (IndicatorFieldEnum_t field) const {
      return getContextIndicatorField(field);
    }
    protected: boolNull getWarningIndicatorFieldBit (IndicatorFieldEnum_t field) const {
      return getContextIndicatorFieldBit(field);
    }

    // Error Indicator Fields
    protected: int32_t getErrorIndicatorField (int8_t eifNum) const {
      return getContextIndicatorField(eifNum | 0x8);
    }
    protected: int32_t getErrorIndicatorField (IndicatorFieldEnum_t field) const {
      return getContextIndicatorField(getCIFNumber(field) | 0x8);
    }
    protected: boolNull getErrorIndicatorFieldBit (IndicatorFieldEnum_t field) const {
      return getContextIndicatorFieldBit(getCIFNumber(field) | 0x8, getCIFBitMask(field));
    }

    // Warning Fields

    /** Gets the warning bit field for the specified indicator field.
     *  @param field Indicator field
     *  @param cif7field (Optional) Indicator field for CIF7 attribute.
     *  @return Warning bit field (0x0 or WEF_NULL or 
     *  WEF_NO_WARNING_ERROR if no warnings).
     */
    public: int32_t getWarning (IndicatorFieldEnum_t field, IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      return getL(getCIFNumber(field), getCIFBitMask(field), cif7bit);
    }

    /** Sets the warning bit field for the specified indicator field.
     *  @param field Indicator field
     *  @param val Warning bit field (0x0 or WEF_NULL or 
     *  WEF_NO_WARNING_ERROR if no warnings).
     *  @param cif7field (Optional) Indicator field for CIF7 attribute.
     */
    public: void setWarning (IndicatorFieldEnum_t field, int32_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      setL(getCIFNumber(field), getCIFBitMask(field), val, cif7bit);
    }
    
    /** Adds one or more warnings to the warning bit field for the specified
     *  indicator field. 
     *  @param field Indicator field
     *  @param val Warning bit field to add (set) to the warning bit field. 
     *  @param cif7field (Optional) Indicator field for CIF7 attribute.
     */
    public: void addWarning (IndicatorFieldEnum_t field, int32_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      if (val == WEF_NULL) return;
      int32_t bits = getL(getCIFNumber(field), getCIFBitMask(field), cif7bit);
      setL(getCIFNumber(field), getCIFBitMask(field), val | bits, cif7bit);
    }

    /** Removes one or more warnings from the warning bit field for the
     *  specified indicator field.
     *  @param field Indicator field
     *  @param val Warning bit field to remove (unset) from the warning bit
     *  field.
     *  @param cif7field (Optional) Indicator field for CIF7 attribute.
     */
    public: void removeWarning (IndicatorFieldEnum_t field, int32_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      if (val == WEF_NULL) return;
      int32_t bits = getL(getCIFNumber(field), getCIFBitMask(field), cif7bit);
      setL(getCIFNumber(field), getCIFBitMask(field), ~val & bits, cif7bit);
    }

    /** Gets all warnings for all indicator fields.
     *  @return Vector of warnings.
     */
    public: std::vector<WarningErrorField_t> getWarnings () const;

    // Error Fields
    
    /** Gets the error bit field for the specified indicator field.
     *  @param field Indicator field
     *  @param cif7field (Optional) Indicator field for CIF7 attribute.
     *  @param cif7field (Optional) Indicator field for CIF7 attribute.
     *  @return Error bit field (0x0 or WEF_NULL or 
     *  WEF_NO_WARNING_ERROR if no errors).
     */
    public: int32_t getError (IndicatorFieldEnum_t field, IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      return getL(getCIFNumber(field) | 0x8, getCIFBitMask(field), cif7bit);
    }

    /** Sets the error bit field for the specified indicator field.
     *  @param field Indicator field
     *  @param val Error bit field (0x0 or WEF_NULL or 
     *  WEF_NO_WARNING_ERROR if no errors).
     *  @param cif7field (Optional) Indicator field for CIF7 attribute.
     */
    public: void setError (IndicatorFieldEnum_t field, int32_t val, IndicatorFieldEnum_t cif7field=CIF_NULL){
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      setL(getCIFNumber(field) | 0x8, getCIFBitMask(field), val, cif7bit);
    }
    
    /** Adds one or more errors to the error bit field for the specified
     *  indicator field.
     *  @param field Indicator field
     *  @param val Error bit field to add (set) to the error bit field.
     *  @param cif7field (Optional) Indicator field for CIF7 attribute.
     */
    public: void addError (IndicatorFieldEnum_t field, int32_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      if (val == WEF_NULL) return;
      int32_t bits = getL(getCIFNumber(field) | 0x8, getCIFBitMask(field), cif7bit);
      setL(getCIFNumber(field) | 0x8, getCIFBitMask(field), val | bits, cif7bit);
    }

    /** Removes one or more errors from the error bit field for the
     *  specified indicator field.
     *  @param field Indicator field
     *  @param val Error bit field to remove (unset) from the error bit
     *  field.
     *  @param cif7field (Optional) Indicator field for CIF7 attribute.
     */
    public: void removeError (IndicatorFieldEnum_t field, int32_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      if (val == WEF_NULL) return;
      int32_t bits = getL(getCIFNumber(field) | 0x8, getCIFBitMask(field), cif7bit);
      setL(getCIFNumber(field) | 0x8, getCIFBitMask(field), ~val & bits, cif7bit);
    }

    /** Gets all errors for all indicator fields.
     *  @return Vector of errors.
     */
    public: std::vector<WarningErrorField_t> getErrors () const;

    /** Is there a Free-form Warning/Error Message Field
     *  @return True if Free-form Message; False otherwise
     */
    public: bool hasFreeFormMessage () const;

    /** Gets Free-form Warning/Error Message Field
     *  @return Free-form message
     */
    public: FreeFormMessage_t getFreeFormMessage () const;

    /** Sets Free-form Warning/Error Message Field
     *  @param msg Free-form message
     */
    public: void setFreeFormMessage (const FreeFormMessage_t msg);

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

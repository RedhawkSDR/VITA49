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

#ifndef _BasicControlPacket_h
#define _BasicControlPacket_h

#include "BasicCommandPacket.h"
#include "BasicContextPacket.h"
#include <cstring>
#include <vector>
#include <sstream>
#include <iomanip>


namespace vrt {
  using namespace std;

  /*****************************************************************************/
  /*****************************************************************************/
  /**                                                                         **/
  /**                        BasicControlPacket                               **/
  /**                                                                         **/
  /*****************************************************************************/
  /*****************************************************************************/

  class BasicControlPacket : public BasicCommandPacket, public BasicContextPacket {

    //======================================================================
    // Constructors/Destructors
    //======================================================================
    // TODO - be careful due to diamond inheritance in the constructors
    // TODO - set header to control (not ack) and any other necessary settings
    //======================================================================

    /** Basic destructor. */
    public: ~BasicControlPacket () { }

    /** Basic copy constructor. */
    public: BasicControlPacket (const BasicVRTPacket &p);

    /** Internal constructor, for use by subclasses.
     *  @param p       The packet (already initialized).
     *  @param classID The expected classID.
     *  @throws VRTException If packet type or classID of the packet are invalid.
     */
    public: BasicControlPacket (const BasicVRTPacket &p, int64_t classID);

    /** Creates a new instance accessing the given data buffer.
     *  @param buf      The pointer to the packet buffer.
     *  @param len      The length  of the packet buffer.
     *  @param readOnly Should users of this instance be able to modify the underlying data buffer?
     */
    public: BasicControlPacket (const void *buf, size_t len, bool readOnly=false);

    /** Creates a new instance that can be written to. Initially this will just be a simple context
     *  packet with no fields set (other than the required packet length of 8 and a default stream ID
     *  of 0).
     */
    public: BasicControlPacket ();

    /** Creates a new instance with a default anticipated length that can be written to.
     *  Initially this will just be a simple context packet with no fields set (other than
     *  the required packet length of 8 and a default stream ID  of 0), but will have the
     *  underlying buffers pre-allocated as required.
     *  @param bufsize The anticipated buffer size.
     */
    public: BasicControlPacket (int32_t bufsize);

    public: BasicControlPacket (const vector<char> &buf, ssize_t start, ssize_t end, bool readOnly = false);

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
    public: BasicControlPacket (const vector<char> &buf, bool readOnly = false);

    public: BasicControlPacket (vector<char> *buf_ptr, bool readOnly = false);

    /** Internal constructor, for use by subclasses.
     *  @param p                The packet (already initialized).
     *  @param type             The expected packet type.
     *  @param classID          The expected classID.
     *  @param minPayloadLength The minimum payload length.
     *  @param maxPayloadLength The maximum payload length (&lt;0 if not pre-defined).
     *  @throws VRTException If packet type or classID of the packet are invalid, or if
     *                       the payload length is invalid.
     */
    protected: BasicControlPacket (const BasicVRTPacket &p, PacketType type, int64_t classID,
                                   int32_t minPayloadLength, int32_t maxPayloadLength);


    //======================================================================
    // IMPLEMENTATIONS OF VIRTUAL METHODS
    //======================================================================
    // Note: due to diamond inheritance, some could have ambiguous default implementations

    protected: virtual void      toStringStream (ostringstream &str) const;
    public:    virtual string    getPacketValid (bool strict, int32_t length) const {
      return BasicCommandPacket::getPacketValid(strict, length);
    }
    public: virtual inline void setPacketType (PacketType t) {
      if (t != PacketType_Command) {
        throw VRTException("Can not change packet type to non-Command type when using BasicControlPacket");
      }
      BasicVRTPacket::setPacketType(t);
    }

    // TODO - what new things need to be reset? if nothing, don't re-implement
    /** Prepares a packet for resend resetting any non-persistent values. This is implemented as follows:
     *  <!--
     *  <pre>
     *    bool BasicControlPacket::resetForResend (const TimeStamp &t) {
     *      BasicCommandPacket::resetForResend(t);
     *      return true;
     *    }
     *  </pre>
     *  @param t The new time stamp.
     *  @return true
     *  -->
     */
    //public: virtual bool resetForResend (const TimeStamp &t);

    // BasicVRTPacket originally provides this method, which returns true always,
    // as is appropriate for BasicCommandPacket.
    // IndicatorFieldProvider also provides this method, but returns value of Change Indicator
    // as is appropriate for BasicContextPacket.
    // BasicControlPackets should default to BasicCommandPacket's implementation; ensure that here.
    public: virtual bool isChangePacket() const {
      return BasicCommandPacket::isChangePacket(); // could just return true instead
    }

    // override prologue-related functions now that we have a PSP
    // both Context and Command have these functions, just point to Command's implementation

    /** Gets the packet specific prologue (PSP) length in bytes. The length of the PSP will be a value
     *  <b>up to</b> {@link #MAX_PKT_SPECIFIC_PROLOGUE_LENGTH}. The PSP length will always be a multiple of 4-bytes.
     *  @return The length of the packet specific prologue in bytes.
     */
    public: virtual int32_t getPktSpecificPrologueLength () const {
      return BasicCommandPacket::getPktSpecificPrologueLength();
    }

    /** Gets the prologue length in bytes (includes header and packet specific prologue, but does not
     *  include trailer). The length of the prologue will be a value <b>up to</b> {@link #MAX_PROLOGUE_LENGTH}.
     *  The prologue length will always be a multiple of 4-bytes.
     *  @return The length of the prologue in bytes.
     */
    public: virtual int32_t getPrologueLength () const {
      return BasicCommandPacket::getPrologueLength();
    }


    //======================================================================
    // CONTEXT PACKET METHODS
    //======================================================================

    /** Gets the specified bit from the State and Event Indicator field.
     *  @param enable    Bit position of the enable flag.
     *  @param indicator Bit position of the indicator flag.
     *  @return null if field is not present, null if the enable bit is not set, true if the enable
     *          bit is set and the indicator bit is set, false if the enable bit is set but the
     *          indicator bit is not set.
     */
    // ContextPacket impl ok; relies on virtual getPrologueLength to be accurate
    //protected: virtual boolNull getStateEventBit (int32_t enable, int32_t indicator) const;

    /** Sets the specified bit of the State and Event Indicator field.
     *  @param enable    Bit position of the enable flag.
     *  @param indicator Bit position of the indicator flag.
     *  @param value     The value of the bits: null if the enable bit should not be set, true if
     *                   the enable bit and the indicator bit should be set, false if the enable bit
     *                   should be set but the indicator bit should not be set.
     */
    // ContextPacket impl ok; relies on virtual getPrologueLength to be accurate
    //protected: virtual void setStateEventBit (int32_t enable, int32_t indicator, boolNull value);

    /** Gets the offset for the given context indicator field relative to the location of the first
     *  occurrence of CIF0. For CIF7 indicator fields, returns the byte offset from beginning of any
     *  arbitrary CIF[0-3] field offset. For example, 
     *  total offset = getOffset(<CIF[0-3] field>) + getOffset(<CIF7 attribute>)
     *  Also, for access to the second occurrence of CIFs (i.e. Error fields in Ack packets), add 8
     *  to the CIF number, effectively setting the 4th bit (mod8 gives field, div8 gives occurrence).
     *  @param cifNum number of CIF that field belongs to.
     *  @param field bitmask associated with field of interest.
     *  @return offset in bytes from beginning of CIF payload, or CIF7 as described above.
     *  @throws VRTException If the CIF number is invalid.
     */
    //  TODO - add ability to mod/div cifNum by 8 to get Error field offets in AckPacket class
    // ContextPacket impl ok; relies on virtual getPrologueLength to be accurate
    //protected: virtual int32_t getOffset (int8_t cifNum, int32_t field) const;

    /** Gets the length of the given field when present (-1 if variable, -2 if not found).
     *  @param cifNum number of CIF that field belongs to.
     *  @param field bitmask associated with field of interest.
     *  @return size of field of interest in bytes.
     *  @throws VRTException If the CIF number is invalid.
     */
    // ContextPacket impl ok; relies on virtual getPrologueLength to be accurate
    //protected: virtual int32_t getFieldLen (int8_t cifNum, int32_t field) const;

    /** Unpacks a 16-bit integer from the payload at the indicated position. */
    // ContextPacket impl ok; relies on virtual getPrologueLength to be accurate
    //protected: virtual int8_t getB (int8_t cifNum, int32_t bit, int32_t xoff) const;

    /** Packs a 16-bit integer from the payload at the indicated position. */
    // ContextPacket impl ok; relies on virtual getPrologueLength to be accurate
    //protected: virtual void setB (int8_t cifNum, int32_t bit, int32_t xoff, int8_t val);

    /** Unpacks a 16-bit integer from the payload at the indicated position. */
    // ContextPacket impl ok; relies on virtual getPrologueLength to be accurate
    //protected: virtual int16_t getI (int8_t cifNum, int32_t bit, int32_t xoff) const;

    /** Packs a 16-bit integer from the payload at the indicated position. */
    // ContextPacket impl ok; relies on virtual getPrologueLength to be accurate
    //protected: virtual void setI (int8_t cifNum, int32_t bit, int32_t xoff, int16_t val);

    /** Unpacks a 32-bit integer from the payload at the indicated position. */
    // ContextPacket impl ok; relies on virtual getPrologueLength to be accurate
    //protected: virtual int32_t getL (int8_t cifNum, int32_t bit) const;

    /** Unpacks a 32-bit integer from the payload at the indicated position. */
    // ContextPacket impl ok; relies on virtual getPrologueLength to be accurate
    //protected: virtual int32_t getL24 (int8_t cifNum, int32_t bit, int32_t offset) const;

    /** Packs a 32-bit integer from the payload at the indicated position. */
    // ContextPacket impl ok; relies on virtual getPrologueLength to be accurate
    //protected: virtual void setL (int8_t cifNum, int32_t bit, int32_t val);

    /** Unpacks a 64-bit integer from the payload at the indicated position. */
    // ContextPacket impl ok; relies on virtual getPrologueLength to be accurate
    //protected: virtual int64_t getX (int8_t cifNum, int32_t bit) const;

    /** Packs a 64-bit integer from the payload at the indicated position. */
    // ContextPacket impl ok; relies on virtual getPrologueLength to be accurate
    //protected: virtual void setX (int8_t cifNum, int32_t bit, int64_t val);

    // ContextPacket impl ok; relies on virtual getPrologueLength to be accurate
    //protected: virtual void setRecord (int8_t cifNum, int32_t bit, const Record *val, int32_t oldLen);

    // ContextPacket impl ok; relies on virtual getPrologueLength to be accurate
    /*protected: virtual int32_t getContextIndicatorField0 (bool occurrence=0) const {
      UNUSED_VARIABLE(occurrence);
      return VRTMath::unpackInt(bbuf, getPrologueLength());
    }*/
    // ContextPacket impl ok; relies on virtual getPrologueLength to be accurate
    /*protected: virtual int32_t getContextIndicatorField1 (bool occurrence=0) const {
      UNUSED_VARIABLE(occurrence);
      if (!isCIF1Enable()) return INT32_NULL;
      return VRTMath::unpackInt(bbuf, getPrologueLength()+4);
    }*/
    // ContextPacket impl ok; relies on virtual getPrologueLength to be accurate
    /*protected: virtual int32_t getContextIndicatorField2 (bool occurrence=0) const {
      UNUSED_VARIABLE(occurrence);
      if (!isCIF2Enable()) return INT32_NULL;
      int32_t off = 0;
      if (isCIF1Enable()) off+=4;
      return VRTMath::unpackInt(bbuf, getPrologueLength()+4+off);
    }*/
    // ContextPacket impl ok; relies on virtual getPrologueLength to be accurate
    /*protected: virtual int32_t getContextIndicatorField3 (bool occurrence=0) const {
      UNUSED_VARIABLE(occurrence);
      if (!isCIF3Enable()) return INT32_NULL;
      int32_t off = 0;
      if (isCIF1Enable()) off+=4;
      if (isCIF2Enable()) off+=4;
      return VRTMath::unpackInt(bbuf, getPrologueLength()+4+off);
    }*/
    // ContextPacket impl ok; relies on virtual getPrologueLength to be accurate
    /*protected: virtual int32_t getContextIndicatorField7 (bool occurrence=0) const {
      UNUSED_VARIABLE(occurrence);
      if (!isCIF7Enable()) return INT32_NULL;
      int32_t off = 0;
      if (isCIF1Enable()) off+=4;
      if (isCIF2Enable()) off+=4;
      if (isCIF3Enable()) off+=4;
      return VRTMath::unpackInt(bbuf, getPrologueLength()+4+off);
    }*/

    // ContextPacket impl ok; relies on virtual getPrologueLength to be accurate
    /*protected: virtual void setContextIndicatorField0Bit (int32_t bit, bool set, bool occurrence=0);
    protected: virtual void setContextIndicatorField1Bit (int32_t bit, bool set, bool occurrence=0);
    protected: virtual void setContextIndicatorField2Bit (int32_t bit, bool set, bool occurrence=0);
    protected: virtual void setContextIndicatorField3Bit (int32_t bit, bool set, bool occurrence=0);
    protected: virtual void setContextIndicatorField7Bit (int32_t bit, bool set, bool occurrence=0);
    */

    /** Functions that require pack/unpack functions from BasicVRTPacket **/

    /** Used for unpacking geolocation records. */
    // ContextPacket impl ok; relies on virtual getPrologueLength to be accurate
    /*protected: inline Geolocation getGeolocation (int8_t cifNum, int32_t field) const {
      int32_t     off = getOffset(cifNum, field);
      Geolocation val;
      if (off >= 0) unpackPayloadRecord(off,val);
      return val;
    }*/

    /** Used for unpacking ephemeris records. */
    // ContextPacket impl ok; relies on virtual getPrologueLength to be accurate
    /*protected: inline Ephemeris getEphemeris (int8_t cifNum, int32_t field) const {
      int32_t     off = getOffset(cifNum, field);
      Ephemeris val;
      if (off >= 0) unpackPayloadRecord(off,val);
      return val;
    }*/

    /** Gets the GPS ASCII "sentences". These are ASCII "sentences" from a GPS (Global
     *  Positioning System) receiver, such as those conforming to NMEA-0183. <i>Due to the great
     *  variety in GPS "sentences", this class makes little effort to do anything useful with them.
     *  Users are encouraged to use {@link #getGeolocationGPS()} which essentially provides the same
     *  information, but in a consistent form.</i>
     *  @return The geolocation information (null if not specified). <i>Note that changes to the
     *          returned object do not alter the packet.</i>
     */
    // ContextPacket impl ok; relies on virtual getPrologueLength to be accurate
    /*public: inline GeoSentences getGeoSentences () const {
      //int32_t      off = IndicatorFieldProvider::getOffset(GPS_ASCII);
      int32_t      off = getOffset(0, protected_CIF0::GPS_ASCII_mask);
      GeoSentences val;
      if (off >= 0) unpackPayloadRecord(off,val);
      return val;
    }*/

    /** Gets the Context Association Lists. These lists indicate the other context/data
     *  streams associated with this one.
     *  @return The context association lists (null if not specified). <i>Note that changes to the
     *          returned object do not alter the packet.</i>
     */
    // ContextPacket impl ok; relies on virtual getPrologueLength to be accurate
    /*public: inline ContextAssocLists getContextAssocLists () const {
      //int32_t           off = IndicatorFieldProvider::getOffset(CONTEXT_ASOC);
      int32_t           off = getOffset(0, protected_CIF0::CONTEXT_ASOC_mask);
      ContextAssocLists val;
      if (off >= 0) unpackPayloadRecord(off,val);
      return val;
    }*/

    /** Functions that require PayloadFormat type from BasicVRTPacket **/

    /** Gets the Data Packet Payload Format. This specifies the format of the data in the
     *  paired data packet stream.
     *  @return The payload format (null if not specified). <i>Note that changes to the returned
     *          object do not alter the packet.</i>
     */
    // ContextPacket impl ok; relies on virtual getPrologueLength to be accurate
    /*public: inline PayloadFormat getDataPayloadFormat () const {
      // INT64_NULL (0x8000000000000000) is actually a valid PayloadFormat bit arrangement, so we
      // can't use the normal isNull(bits) check. This isn't an issue in Java where null is
      // not equal to a legal 64-bit integer value.
      return (getOffset(0, protected_CIF0::DATA_FORMAT_mask) < 0)? PayloadFormat() // NULL
                                                                 : PayloadFormat(getX(0, protected_CIF0::DATA_FORMAT_mask));
    }*/

    /** Sets the Data Packet Payload Format. This specifies the format of the data in the
     *  paired data packet stream.
     *  @param val The payload format (null if not specified). 
     */
    // ContextPacket impl ok; relies on virtual getPrologueLength to be accurate
    /*public: inline void setDataPayloadFormat (const PayloadFormat &val) {
      setX(0, protected_CIF0::DATA_FORMAT_mask, val.getBits());
    }*/

    //======================================================================
    // CONTROL PACKET METHODS
    //======================================================================
    // TODO - are there any special functions, special cases?
    // TODO - may need additional class for Control-Cancel packet w/o CIF field payload



    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Implement HasFields
    //////////////////////////////////////////////////////////////////////////////////////////////////
    /* TODO - BasicContext and BasicCommand both implement these, and now we need to rectify them there
    public: virtual int32_t   getFieldCount () const;
    public: virtual string    getFieldName  (int32_t id) const;
    public: virtual ValueType getFieldType  (int32_t id) const;
    public: virtual Value*    getField      (int32_t id) const __attribute__((warn_unused_result));
    public: virtual void      setField      (int32_t id, const Value* val);
    */

  }; /* BasicControlPacket */


} END_NAMESPACE /* vrt namespace */

#endif /* _BasicControlPacket_h */


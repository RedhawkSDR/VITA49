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

#ifndef _BasicContextPacket_h
#define _BasicContextPacket_h

#include "BasicVRTPacket.h"
#include "IndicatorFields.h"
#include "VRTConfig.h"
#include <map>

namespace vrt {

  using namespace std;
  using namespace VRTMath;
  using namespace IndicatorFields;

  /** A basic but full-featured implementation of a VRT IF Context packet. <br>
   *  <br>
   *  The following implementation is used for the {@link HasFields} methods:
   *  <pre>
   *     ID | Name                     | Type
   *    ----+--------------------------+------------------
   *    N+0 | ChangePacket             | Bool
   *    N+1 | ReferencePointIdentifier | Int32
   *    N+2 | Bandwidth                | Double
   *    N+3 | FrequencyIF              | Double
   *    N+4 | FrequencyRF              | Double
   *    N+5 | FrequencyOffsetRF        | Double
   *    N+6 | BandOffsetIF             | Double
   *    N+7 | ReferenceLevel           | Float
   *    N+8 | Gain                     | Float
   *    N+9 | Gain1                    | Float (can not set to null)
   *    N+10| Gain2                    | Float (can not set to null)
   *    N+11| OverRangeCount           | Int64
   *    N+12| SampleRate               | Double
   *    N+13| SamplePeriod             | Double
   *    N+14| TimeStampAdjustment      | Int64
   *    N+15| TimeStampCalibration     | Int32
   *    N+16| Temperature              | Float
   *    N+17| DeviceID                 | String
   *    N+18| CalibratedTimeStamp      | BoolNull
   *    N+19| DataValid                | BoolNull
   *    N+20| ReferenceLocked          | BoolNull
   *    N+21| SignalDetected           | BoolNull
   *    N+22| InvertedSpectrum         | BoolNull
   *    N+23| OverRange                | BoolNull
   *    N+24| Discontinuous            | BoolNull
   *    N+25| UserDefinedBits          | Int8
   *    N+26| DataPayloadFormat        | PayloadFormat
   *    N+27| GeolocationGPS           | Geolocation
   *    N+28| GeolocationINS           | Geolocation
   *    N+29| EphemerisECEF            | Ephemeris
   *    N+30| EphemerisRelative        | Ephemeris
   *    N+31| EphemerisReference       | Int32
   *    N+32| GeoSentences             | GeoSentences
   *    N+33| ContextAssocLists        | ContextAssocLists
   *    // TODO - update comment w/ accurate CIF0 info, and additional CIFs
   *    // TODO - also update hasFields implementation, of course
   *    ----+--------------------------+------------------
   *    (N = number of fields in BasicVRTPacket)
   *  </pre>
   */
  class BasicContextPacket : public virtual BasicVRTPacket, public IndicatorFieldProvider {
    //friend class BasicVRTState; // Unnecessary class not updated for V49.2 yet
    using IndicatorFieldProvider::getOffset;

    //======================================================================
    // Constructors/Destructors
    //======================================================================

    /** Basic destructor. */
    public: ~BasicContextPacket () { }

    /** Basic copy constructor. */
    public: BasicContextPacket (const BasicVRTPacket &p);

    /** Internal constructor, for use by subclasses.
     *  @param p       The packet (already initialized).
     *  @param classID The expected classID.
     *  @throws VRTException If packet type or classID of the packet are invalid.
     */
    public: BasicContextPacket (const BasicVRTPacket &p, int64_t classID);

    /** Creates a new instance accessing the given data buffer.
     *  @param buf      The pointer to the packet buffer.
     *  @param len      The length  of the packet buffer.
     *  @param readOnly Should users of this instance be able to modify the underlying data buffer?
     */
    public: BasicContextPacket (const void *buf, size_t len, bool readOnly=false);

    /** Creates a new instance that can be written to. Initially this will just be a simple context
     *  packet with StreamID, ClassID, TSI=UTC, TSF=Real-Time, CIF0, and a packet length of 8.
     */
    public: BasicContextPacket ();

    /** Creates a new instance with a default anticipated length that can be written to.
     *  Initially this will just be a simple context packet with no fields set (other than
     *  the required packet length of 8 and a default stream ID  of 0), but will have the
     *  underlying buffers pre-allocated as required.
     *  @param bufsize The anticipated buffer size.
     */
    public: BasicContextPacket (int32_t bufsize);

    public: BasicContextPacket (const vector<char> &buf, ssize_t start, ssize_t end, bool readOnly = false);

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
    public: BasicContextPacket (const vector<char> &buf, bool readOnly = false);

    public: BasicContextPacket (vector<char> *buf_ptr, bool readOnly = false);

    /** Internal constructor, for use by subclasses.
     *  @param p                The packet (already initialized).
     *  @param type             The expected packet type.
     *  @param classID          The expected classID.
     *  @param minPayloadLength The minimum payload length.
     *  @param maxPayloadLength The maximum payload length (&lt;0 if not pre-defined).
     *  @throws VRTException If packet type or classID of the packet are invalid, or if
     *                       the payload length is invalid.
     */
    protected: BasicContextPacket (const BasicVRTPacket &p, PacketType type, int64_t classID,
                                   int32_t minPayloadLength, int32_t maxPayloadLength);

    //======================================================================
    // UTILITY METHODS
    //======================================================================

    public: virtual string getPacketValid (bool strict, int32_t length) const;

    protected: virtual void toStringStream (ostringstream &str) const;

    public: virtual inline void setPacketType (PacketType t) {
      if (t != PacketType_Context) {
        throw VRTException("Can not change packet type to non-Context type when using BasicContextPacket");
      }
      BasicVRTPacket::setPacketType(t);
    }

    // TODO - what new things need to be reset?
    /** Prepares a packet for resend resetting any non-persistent values and clearing the context
     *  change flags. This is implemented as follows:
     *  <pre>
     *    bool BasicContextPacket::resetForResend (const TimeStamp &t) {
     *      BasicVRTPacket::resetForResend(t);
     *      setChangePacket(false);
     *      setOverRangeCount(null);
     *      setOverRange(null);
     *      setDiscontinuous(null);
     *      return true;
     *    }
     *  </pre>
     *  @param t The new time stamp.
     *  @return true
     */
    public: virtual bool resetForResend (const TimeStamp &t);

    // XXX - overriding public: virtual bool BasicVRTPacket::isChangePacket () const
    //       since that is hard-coded to return true (valid for Data packets), but here we get that
    //       value from inline bool IndicatorFieldProvider::isChangePacket() const;
    public: virtual bool isChangePacket() const {
      return IndicatorFieldProvider::isChangePacket();
    }

    //======================================================================
    // CONTEXT PACKET METHODS (CIFs, etc.)
    //======================================================================

    /** Gets the adjusted time stamp of the packet. This is implemented as follows:
     *  <pre>
     *  public: TimeStamp getAdjustedTimeStamp () const {
     *    int64_t adj = getTimeStampAdjustment();
     *    return (adj == INT64_NULL)? getTimeStamp() : getTimeStamp().addPicoSeconds(adj);
     *  }
     *  </pre>
     *  @return The adjusted time stamp of the packet (never null).
     */
    // FIXME - this is probably incorrect. The spec says timestamp adjustment applies
    //         to the timestamp of each associated data packet, and NOT to the timestamp
    //         of the context packet. That means this function is impossible to ever
    //         return valid without access to a DataPacket and ContextPacket together.
    //         See Section 9.7.3.1 of V49.2 draft spec
    public: TimeStamp getAdjustedTimeStamp () const {
      int64_t adj = getTimeStampAdjustment();
      return (adj == INT64_NULL)? getTimeStamp() : getTimeStamp().addPicoSeconds(adj);
    }

    /** Gets the specified bit from the State and Event Indicator field.
     *  @param enable    Bit position of the enable flag.
     *  @param indicator Bit position of the indicator flag.
     *  @param cif7bit Bit position of the CIF7 indicator flag.
     *  @return null if field is not present, null if the enable bit is not set, true if the enable
     *          bit is set and the indicator bit is set, false if the enable bit is set but the
     *          indicator bit is not set.
     */
    protected: virtual boolNull getStateEventBit (int32_t enable, int32_t indicator, int32_t cif7bit) const;

    /** Sets the specified bit of the State and Event Indicator field.
     *  @param enable    Bit position of the enable flag.
     *  @param indicator Bit position of the indicator flag.
     *  @param value     The value of the bits: null if the enable bit should not be set, true if
     *                   the enable bit and the indicator bit should be set, false if the enable bit
     *                   should be set but the indicator bit should not be set.
     *  @param cif7bit Bit position of the CIF7 indicator flag.
     */
    protected: virtual void setStateEventBit (int32_t enable, int32_t indicator, boolNull value, int32_t cif7bit);

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
     *  @param occurrence False (0) if first occurrence (default; almost everything),
     *                    True (1) if second occurrence (only Errors for BasicAcknowledgePacket)
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

    /** Unpacks a 16-bit integer from the payload at the indicated position. */
    protected: virtual int8_t getB (int8_t cifNum, int32_t bit, int32_t xoff, int32_t cif7bit) const;

    /** Packs a 16-bit integer from the payload at the indicated position. */
    protected: virtual void setB (int8_t cifNum, int32_t bit, int32_t xoff, int8_t val, int32_t cif7bit);

    /** Unpacks a 16-bit integer from the payload at the indicated position. */
    protected: virtual int16_t getI (int8_t cifNum, int32_t bit, int32_t xoff, int32_t cif7bit) const;

    /** Packs a 16-bit integer from the payload at the indicated position. */
    protected: virtual void setI (int8_t cifNum, int32_t bit, int32_t xoff, int16_t val, int32_t cif7bit);

    /** Unpacks a 32-bit integer from the payload at the indicated position. */
    protected: virtual int32_t getL (int8_t cifNum, int32_t bit, int32_t cif7bit) const;

    /** Unpacks a 32-bit integer from the payload at the indicated position. */
    protected: virtual int32_t getL24 (int8_t cifNum, int32_t bit, int32_t offset, int32_t cif7bit) const;

    /** Packs a 32-bit integer from the payload at the indicated position. */
    protected: virtual void setL (int8_t cifNum, int32_t bit, int32_t val, int32_t cif7bit);

    /** Unpacks a 64-bit integer from the payload at the indicated position. */
    protected: virtual int64_t getX (int8_t cifNum, int32_t bit, int32_t cif7bit) const;

    /** Packs a 64-bit integer from the payload at the indicated position. */
    protected: virtual void setX (int8_t cifNum, int32_t bit, int64_t val, int32_t cif7bit);

    /** Unpacks a 128-bit UUID from the payload at the indicated position. */
    protected: virtual UUID getUUID (int8_t cifNum, int32_t bit, int32_t cif7bit) const;

    /** Packs a 128-bit UUID from the payload at the indicated position. */
    protected: virtual void setUUID (int8_t cifNum, int32_t bit, const UUID &val, int32_t cif7bit);

    /** Unpacks a TimeStamp from the payload at the indicated position. */
    protected: virtual TimeStamp getTimeStampField (int8_t cifNum, int32_t bit, int32_t cif7bit) const;

    /** Packs a TimeStamp from the payload at the indicated position. */
    protected: virtual void setTimeStampField (int8_t cifNum, int32_t bit, const TimeStamp &val, int32_t cif7bit);

    /** Gets a block of data. */
    protected: virtual void setRecord (int8_t cifNum, int32_t bit, const Record *val, int32_t oldLen, int32_t cif7bit);

    /** Gets the header type, used with getOffset(..) and OFFSET_TABLE. */
    protected: virtual int32_t getContextIndicatorField0 (bool occurrence=0) const {
      UNUSED_VARIABLE(occurrence);
      return VRTMath::unpackInt(bbuf, getPrologueLength());
    }
    protected: virtual int32_t getContextIndicatorField1 (bool occurrence=0) const {
      UNUSED_VARIABLE(occurrence);
      if (!isCIF1Enable()) return 0;
      return VRTMath::unpackInt(bbuf, getPrologueLength()+4);
    }
    protected: virtual int32_t getContextIndicatorField2 (bool occurrence=0) const {
      UNUSED_VARIABLE(occurrence);
      if (!isCIF2Enable()) return 0;
      int32_t off = 0;
      if (isCIF1Enable()) off+=4;
      return VRTMath::unpackInt(bbuf, getPrologueLength()+4+off);
    }
    protected: virtual int32_t getContextIndicatorField3 (bool occurrence=0) const {
      UNUSED_VARIABLE(occurrence);
      if (!isCIF3Enable()) return 0;
      int32_t off = 0;
      if (isCIF1Enable()) off+=4;
      if (isCIF2Enable()) off+=4;
      return VRTMath::unpackInt(bbuf, getPrologueLength()+4+off);
    }
    protected: virtual int32_t getContextIndicatorField7 (bool occurrence=0) const {
      UNUSED_VARIABLE(occurrence);
      if (!isCIF7Enable()) return 0;
      int32_t off = 0;
      if (isCIF1Enable()) off+=4;
      if (isCIF2Enable()) off+=4;
      if (isCIF3Enable()) off+=4;
      return VRTMath::unpackInt(bbuf, getPrologueLength()+4+off);
    }

    protected: virtual void setContextIndicatorField0Bit (int32_t bit, bool set, bool occurrence=0);
    protected: virtual void setContextIndicatorField1Bit (int32_t bit, bool set, bool occurrence=0);
    protected: virtual void setContextIndicatorField2Bit (int32_t bit, bool set, bool occurrence=0);
    protected: virtual void setContextIndicatorField3Bit (int32_t bit, bool set, bool occurrence=0);
    protected: virtual void setContextIndicatorField7Bit (int32_t bit, bool set, bool occurrence=0);

    /** Adds or removes a CIF
     *  Sets the Enable Indicator of CIF0 and adds or removes 4-bytes for the CIF.
     *  @param add True = add CIF (default), False = remove CIF
     *  @param occurrence Value is ignored and assumed to be 0.
     */
    public: virtual void addCIF1 (bool add=true, bool occurrence=false);
    public: virtual void addCIF2 (bool add=true, bool occurrence=false);
    public: virtual void addCIF3 (bool add=true, bool occurrence=false);
    public: virtual void addCIF7 (bool add=true, bool occurrence=false);

    protected: virtual void setCIF7Bit (int32_t cif7bit, bool set, bool occurrence=false);

    /** Functions that require pack/unpack functions from BasicVRTPacket **/

// XXX Commented out in favor of those below
//    /** Used for unpacking geolocation records. */
//    protected: inline Geolocation getGeolocation (IndicatorFieldEnum_t field) const {
//      int32_t     off = IndicatorFieldProvider::getOffset(field);
//      Geolocation val;
//      if (off >= 0) unpackPayloadRecord(off,val);
//      return val;
//    }
//
//    /** Used for unpacking ephemeris records. */
//    protected: inline Ephemeris getEphemeris (IndicatorFieldEnum_t field) const {
//      int32_t   off = IndicatorFieldProvider::getOffset(field);
//      Ephemeris val;
//      if (off >= 0) unpackPayloadRecord(off,val);
//      return val;
//    }

    /** Used for unpacking geolocation records. */
    protected: inline Geolocation getGeolocation (int8_t cifNum, int32_t field, int32_t cif7bit) const {
      int32_t off = getOffset(cifNum, field, cif7bit);
      Geolocation val;
      if (off >= 0) unpackPayloadRecord(off,val);
      return val;
    }

    /** Used for unpacking ephemeris records. */
    protected: inline Ephemeris getEphemeris (int8_t cifNum, int32_t field, int32_t cif7bit) const {
      int32_t off = getOffset(cifNum, field, cif7bit);
      Ephemeris val;
      if (off >= 0) unpackPayloadRecord(off,val);
      return val;
    }

    /** Gets the GPS ASCII "sentences". These are ASCII "sentences" from a GPS (Global
     *  Positioning System) receiver, such as those conforming to NMEA-0183. <i>Due to the great
     *  variety in GPS "sentences", this class makes little effort to do anything useful with them.
     *  Users are encouraged to use {@link #getGeolocationGPS()} which essentially provides the same
     *  information, but in a consistent form.</i>
     *  @param  cif7field Indicator field for the CIF7 attribute.
     *  @return The geolocation information (null if not specified). <i>Note that changes to the
     *          returned object do not alter the packet.</i>
     */
    public: inline GeoSentences getGeoSentences (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      int32_t off = getOffset(0, protected_CIF0::GPS_ASCII_mask, cif7bit);
      GeoSentences val;
      if (off >= 0) unpackPayloadRecord(off,val);
      return val;
    }

    /** Gets the Context Association Lists. These lists indicate the other context/data
     *  streams associated with this one.
     *  @param  cif7field Indicator field for the CIF7 attribute.
     *  @return The context association lists (null if not specified). <i>Note that changes to the
     *          returned object do not alter the packet.</i>
     */
    public: inline ContextAssocLists getContextAssocLists (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      int32_t off = getOffset(0, protected_CIF0::CONTEXT_ASOC_mask, cif7bit);
      ContextAssocLists val;
      if (off >= 0) unpackPayloadRecord(off,val);
      return val;
    }

    /** Gets the Index Field List.
     *  @param  cif7field Indicator field for the CIF7 attribute.
     *  @return The Index Field List (null if not specified). <i>Note that changes to the
     *          returned object do not alter the packet.</i>
     */
    public: inline IndexFieldList getIndexList (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      int32_t off = getOffset(1, protected_CIF1::INDEX_LIST_mask, cif7bit);
      IndexFieldList val;
      if (off >= 0) unpackPayloadRecord(off,val);
      return val;
    }

    /** Gets the Sector Scan/Step Field. 
     *  @param  cif7field Indicator field for the CIF7 attribute.
     *  @return The Sector Scan/Step Array-of-Records (null if not specified).
     *  <i>Note that changes to the returned object do not alter the packet.</i>
     */
    public: virtual ArrayOfRecords getSectorScanStep (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      int32_t off = getOffset(1, protected_CIF1::SECTOR_SCN_STP_mask, cif7bit);
      ArrayOfRecords val;
      if (off >= 0) unpackPayloadRecord(off,val);
      return val;
    }

    /** Gets the CIFs Array
     *  @param  cif7field Indicator field for the CIF7 attribute.
     *  @return The CIFs Array as an Array-of-Records (null if not specified).
     *  <i>Note that changes to the returned object do not alter the packet.</i>
     */
    public: virtual ArrayOfRecords getCIFsArray (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      int32_t off = getOffset(1, protected_CIF1::CIFS_ARRAY_mask, cif7bit);
      ArrayOfRecords val;
      if (off >= 0) unpackPayloadRecord(off,val);
      return val;
    }

    /** Gets the Spectrum Field 
     *  SpectrumField type
     *  (See V49.2 spec Section 9.6.1)
     *  @param  cif7field Indicator field for the CIF7 attribute.
     *  @return The Spectrum Field (null if not specified).
     *  <i>Note that changes to the returned object do not alter the packet.</i>
     */
    public: virtual SpectrumField getSpectrumField (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      int32_t off = getOffset(1, protected_CIF1::SPECTRUM_mask, cif7bit);
      SpectrumField val;
      if (off >= 0) unpackPayloadRecord(off,val);
      return val;
    }

    /** Gets the 3D Pointing Vector (Structured)
     *  @param  cif7field Indicator field for the CIF7 attribute.
     *  @return 3D Pointing Vector as an Array-of-Records (null if not specified).
     *  <i>Note that changes to the returned object do not alter the packet.</i>
     */
    public: virtual ArrayOfRecords get3DPointingVectorStructured (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      int32_t off = getOffset(1, protected_CIF1::PNT_VECT_3D_ST_mask, cif7bit);
      ArrayOfRecords val;
      if (off >= 0) unpackPayloadRecord(off,val);
      return val;
    }

    /** Functions that require PayloadFormat type from BasicVRTPacket **/

    /** Gets the Data Packet Payload Format. This specifies the format of the data in the
     *  paired data packet stream.
     *  @param  cif7field Indicator field for the CIF7 attribute.
     *  @return The payload format (null if not specified). <i>Note that changes to the returned
     *          object do not alter the packet.</i>
     */
    public: virtual PayloadFormat getDataPayloadFormat (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      // INT64_NULL (0x8000000000000000) is actually a valid PayloadFormat bit arrangement, so we
      // can't use the normal isNull(bits) check. This isn't an issue in Java where null is
      // not equal to a legal 64-bit integer value.
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      int32_t off = getOffset(0, protected_CIF0::DATA_FORMAT_mask, cif7bit);
      return (off < 0)? PayloadFormat() // NULL
                      : PayloadFormat(getX(0, protected_CIF0::DATA_FORMAT_mask, cif7bit));
    }

    /** Sets the Data Packet Payload Format. This specifies the format of the data in the
     *  paired data packet stream.
     *  @param val The payload format (null if not specified). 
     *  @param  cif7field Indicator field for the CIF7 attribute.
     */
    public: virtual void setDataPayloadFormat (const PayloadFormat &val, IndicatorFieldEnum_t cif7field=CIF_NULL);

// XXX - Commented out because I don't see how they're useful... or even make sense. What are they doing?
//    /** <!-- Gets the Data Packet Payload Format. This specifies the format of the data in the
//     *  paired data packet stream.
//     *  @param offset The offset.
//     *  @return The payload format (null if not specified). <i>Note that changes to the returned
//     *          object do not alter the packet.</i>
//     * -->
//     */
//    public: inline PayloadFormat getDataPayloadFormat (int8_t cifNum, int32_t field) const {
//      // INT64_NULL (0x8000000000000000) is actually a valid PayloadFormat bit arrangement, so we
//      // can't use the normal isNull(bits) check. This isn't an issue in Java where null is
//      // not equal to a legal 64-bit integer value.
//      return (getOffset(cifNum, field) < 0)? PayloadFormat() : PayloadFormat(getX(cifNum, field));
//    }

//    /** <!-- Gets the Data Packet Payload Format. This specifies the format of the data in the
//     *  paired data packet stream.
//     *  @param offset The offset.
//     *  @return The payload format (null if not specified). <i>Note that changes to the returned
//     *          object do not alter the packet.</i>
//     * -->
//     */
//    public: inline PayloadFormat getDataPayloadFormat (IndicatorFieldEnum_t field) const {
//      // INT64_NULL (0x8000000000000000) is actually a valid PayloadFormat bit arrangement, so we
//      // can't use the normal isNull(bits) check. This isn't an issue in Java where null is
//      // not equal to a legal 64-bit integer value.
//      return (IndicatorFieldProvider::getOffset(field) < 0)? PayloadFormat() : PayloadFormat(IndicatorFieldProvider::getX(field));
//    }




    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Implement HasFields
    //////////////////////////////////////////////////////////////////////////////////////////////////
    public: virtual int32_t   getFieldCount () const;
    public: virtual string    getFieldName  (int32_t id) const;
    public: virtual ValueType getFieldType  (int32_t id) const;
    public: virtual Value*    getField      (int32_t id) const __attribute__((warn_unused_result));
    public: virtual void      setField      (int32_t id, const Value* val);

  };
} END_NAMESPACE
#endif /* _BasicContextPacket_h */

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

#ifndef _BasicVRTState_h
#define _BasicVRTState_h

#include "VRTObject.h"
#include "Utilities.h"
#include "BasicContextPacket.h"
#include "BasicDataPacket.h"
#include <string>
#include <HasFields.h>

using namespace std;

namespace vrt {
  /** <b>Internal use only:</b> Used internal to <tt>sourcevrt</tt> and <tt>sinkvrt</tt>.
       Maintains state of VRT Stream.

      @author 
   */
  class CachedContextWrapper : public BasicContextPacket {
    friend class BasicVRTState;

    public: CachedContextWrapper () :
      BasicContextPacket()
    {
      // done
    }

    public: CachedContextWrapper (const BasicContextPacket& bcp) :
      BasicContextPacket(bcp)
    {
      // done
    }

    public: mutable int32_t cacheIndicator;
    private: mutable map<int32_t,int8_t> cache8;
    private: mutable map<int32_t,int16_t> cache16;
    private: mutable map<int32_t,int32_t> cache32;
    private: mutable map<int32_t,int64_t> cache64;

    /** Unpacks a 16-bit integer from the payload at the indicated position. */
    protected: virtual int8_t getB (int32_t bit, int32_t xoff) const {
      if (!((cacheIndicator & bit) > 0))  {
        cache8[bit] = BasicContextPacket::getB(bit, xoff);
        cacheIndicator = cacheIndicator | bit;
      }
      return cache8[bit];
    }

    /** Unpacks a 16-bit integer from the payload at the indicated position. */
    protected: virtual int16_t getI (int32_t bit, int32_t xoff) const {
      if (!((cacheIndicator & bit) > 0))  {
        cache16[bit] = BasicContextPacket::getI(bit, xoff);
        cacheIndicator = cacheIndicator | bit;
      }
      return cache16[bit];
    }

    /** Unpacks a 32-bit integer from the payload at the indicated position. */
    protected: virtual int32_t getL (int32_t bit) const {
      if (!((cacheIndicator & bit) > 0))  {
        cache32[bit] = BasicContextPacket::getL(bit);
        cacheIndicator = cacheIndicator | bit;
      }
      return cache32[bit];
    }

    /** Unpacks a 64-bit integer from the payload at the indicated position. */
    protected: virtual int64_t getX (int32_t bit) const {
      if (!((cacheIndicator & bit) > 0))  {
        cache64[bit] = BasicContextPacket::getX(bit);
        cacheIndicator = cacheIndicator | bit;
      }
      return cache64[bit];
    }
  };

  /** <b>Internal use only:</b> Used internal to <tt>sourcevrt</tt> and <tt>sinkvrt</tt>.
       Maintains state of VRT Stream.

      @author 
   */
  class BasicVRTState : public VRTObject, public HasFields {
    private: int32_t contextTrigger;
    private: int32_t eventTrigger;
    private: TimeStamp lastChanged;
    private: TimeStamp lastUpdated;
    private: bool initialized;
    private: int64_t totalOverRangeSamples;
    private: int64_t totalOverRangePackets;
    private: int64_t totalDiscontinuousPackets;
    private: BasicContextPacket latestContext;
    private: CachedContextWrapper currentContextState;


    /** Default destructor. */
    public: ~BasicVRTState ();

    /** Creates a new instance with the default context and event triggers.
     *  The triggers define the fields which determine when a streams data is incompatible.
     *
     */
    public: BasicVRTState ();

    /** Creates a new instance with the default context and event triggers and an initial context.
     *  The triggers define the fields which determine when a streams data is incompatible.
     *
     *  @param context         an initial context
     */
    public: BasicVRTState (const BasicContextPacket& context);

    /** Creates a new instance with the given context and event triggers.
     *  The triggers define the fields which determine when a streams data is incompatible.
     *
     *  @param contextTrigger  which context fields should be triggered upon
     *  @param eventTrigger    which state and event bits should be triggered upon
     */
    public: BasicVRTState (int32_t contextTrigger, int32_t eventTrigger);

    /** Creates a new instance with the given context and event triggers and an initial context.
     *  The triggers define the fields which determine when a streams data is incompatible.
     *
     *  @param contextTrigger  which context fields should be triggered upon
     *  @param eventTrigger    which state and event bits should be triggered upon
     *  @param context         an initial context
     */
    public: BasicVRTState (int32_t contextTrigger, int32_t eventTrigger, const BasicContextPacket& context);

    /** Helper method used to copy a field from the source packet to the destination packet.  An attempt is made to do this without
     *   decoding the underlying data, that is, by simply performing a direct copy of data.
     *   @param dest  destination packet
     *   @param src   source packet
     *   @param field field number to be copied
     */
    private: int8_t copy (BasicContextPacket& dest, const BasicContextPacket& src, int32_t& field);

    /** Initializes the state of the VRT Stream.
     *  @param ctx an initial context
     */
    public: void initState (BasicContextPacket& ctx);

    /** Updates the state of this VRT Stream from a Data Packet.
     *  @param data  data packet.
     *  @return true if state has changed and has an incompatible stream to that of before, or false indicating stream is compatible.
     *  <i>Note that the definition of "compatible" and "incompatible" is determined based upon the context trigger and event
     *  trigger fields set up in the constructor.</i>
     */
    public: bool updateState (const BasicDataPacket& data);

    /** Updates the state of this VRT Stream from a Context Packet.
     *  @param ctx  context packet.
     *  @return true if state has changed and has an incompatible stream to that of before, or false indicating stream is compatible.
     *  <i>Note that the definition of "compatible" and "incompatible" is determined based upon the context trigger and event
     *  trigger fields set up in the constructor.</i>
     */
    public: bool updateState (const BasicContextPacket& ctx);

    public: virtual string toString () const;

    // INHERITED FROM BASIC CONTEXT PACKET:
    public: virtual TimeStamp getAdjustedTimeStamp () const { return currentContextState.getAdjustedTimeStamp(); };

    /** Gets the calibrated time indicator flag.
     *  <pre>
     *    true  = The time stamp in this packet is calibrated to an external reference (e.g. atomic clock)
     *    false = The time stamp in this packet is *not* calibrated to an external reference
     *            and may be inaccurate
     *    null  = <i>This information is not available</i>
     *  </pre>
     *  @return true if the flag is set, false if it is not set, null if this optional flag isn't
     *          specified (null is returned for all context packets).
     */
    public: virtual boolNull isCalibratedTimeStamp () const { return currentContextState.isCalibratedTimeStamp(); };

    /** Gets the valid data indicator flag.
     *  <pre>
     *    true  = The data in the packet is valid
     *    false = A condition exists that *may* invalidate some or all of the data in the packet
     *    null  = <i>This information is not available</i>
     *  </pre>
     *  <i>Note that the definition of "valid" and "invalid" data is application specific, so it is
     *  important to conslut the documentation for the relevent packet class before using this field.</i>
     *  @return true if the flag is set, false if it is not set, null if this optional flag isn't
     *          specified (null is returned for all context packets).
     */
    public: virtual boolNull isDataValid () const { return currentContextState.isDataValid(); };

    /** Gets the reference lock indicator flag.
     *  <pre>
     *    true  = All of the phase-locked loops affecting the data are locked and stable
     *    false = At least one of the phase-locked loops affecting the data is not locked and stable
     *    null  = <i>This information is not available</i>
     *  </pre>
     *  @return true if the flag is set, false if it is not set, null if this optional flag isn't
     *          specified (null is returned for all context packets).
     */
    public: virtual boolNull isReferenceLocked () const { return currentContextState.isReferenceLocked(); };

    /** Gets the AGC/MGC indicator flag.
     *  <pre>
     *    true  = AGC (automatic gain control) is being used
     *    false = MGC (manual gain control) is being used
     *    null  = <i>This information is not available</i>
     *  </pre>
     *  @return true if the flag is set, false if it is not set, null if this optional flag isn't
     *          specified (null is returned for all context packets).
     */
    public: virtual boolNull isAutomaticGainControl () const { return currentContextState.isAutomaticGainControl(); };

    /** Gets the signal detected indicator flag.
     *  <pre>
     *    true  = Some signal detected in this packet
     *    false = No signal detected in this packet
     *    null  = <i>This information is not available</i>
     *  </pre>
     *  <i>Note that the definition of what constitutes a detected signal application specific, so it is
     *  important to conslut the documentation for the relevent packet class before using this field.</i>
     *  @return true if the flag is set, false if it is not set, null if this optional flag isn't
     *          specified (null is returned for all context packets).
     */
    public: virtual boolNull isSignalDetected () const { return currentContextState.isSignalDetected(); }

    /** Gets the spectral inversion indicator flag.
     *  <pre>
     *    true  = Signal conveyed in the payload has an inverted spectrum w.r.t. an up-stream
     *            reference point
     *    false = Spectrum is not inverted
     *    null  = <i>This information is not available</i>
     *  </pre>
     *  @return true if the flag is set, false if it is not set, null if this optional flag isn't
     *          specified (null is returned for all context packets).
     */
    public: inline boolNull isInvertedSpectrum () const { return currentContextState.isInvertedSpectrum(); }

    /** Gets the over-range indicator flag.
     *  <pre>
     *    true  = The data in the payload is invalid since one or more data samples exceeded the
     *            range of the data item
     *    false = No samples over range
     *    null  = <i>This information is not available</i>
     *  </pre>
     *  @return true if the flag is set, false if it is not set, null if this optional flag isn't
     *          specified (null is returned for all context packets).
     */
    public: inline boolNull isOverRange () const            { throw VRTException("State does not maintain non-persistent fields."); }

    /** Gets the sample loss indicator flag.
     *  <pre>
     *    true  = One or more sample discontinuities present in the data due to processing errors
     *            and/or data overflow
     *    false = No discontinuities present
     *    null  = <i>This information is not available</i>
     *  </pre>
     *  @return true if the flag is set, false if it is not set, null if this optional flag isn't
     *          specified (null is returned for all context packets).
     */
    public: inline boolNull isDiscontinuious () const       { throw VRTException("State does not maintain non-persistent fields."); }

    /** Gets the Context Field Change Indicator.
     *  @return true if <b>anything</b> in the packet has changed since the last context packet,
     *          false if <b>nothing</b> has changed since the last context packet.
     */
    public: inline bool              isChangePacket ()              const { throw VRTException("State cannot be change packet"); }

    /** Gets the Reference Point Identifier.
     *  @return The point in the system where this context applies (null if not specified).
     */
    public: inline int32_t           getReferencePointIdentifier () const { return currentContextState.getReferencePointIdentifier(); }

    /** Gets the Timestamp Adjustment in picoseconds. This is the required time adjustment
     *  between the time the signal was digitized (i.e. the timestamp) and the time at the reference
     *  point.
     *  @return The timestamp adjustment (null if not specified).
     */
    public: inline int64_t           getTimeStampAdjustment ()      const { return currentContextState.getTimeStampAdjustment(); }

    /** Gets the Timestamp Calibration Time in seconds. This is the most recent date and
     *  time when the timestamp in the Data and Context packets was known to be correct.
     *  @return The calibration time (null if not specified).
     */
    public: inline int32_t           getTimeStampCalibration ()     const { return currentContextState.getTimeStampCalibration(); }

    /** Gets the Ephemeris Reference Identifier. This is used specifies the process
     *  whose origin applies to the ephemeris returned by {@link #getEphemerisRelative()}.
     *  @return The ephemeris information (null if not specified). <i>Note that changes to the
     *          returned object do not alter the packet.</i>
     */
    public: virtual int32_t           getEphemerisReference ()       const { return currentContextState.getEphemerisReference(); }

    /** Gets the Formatted GPS (Global Positioning System) Geolocation for the collector.
     *  @return The geolocation information (null if not specified). <i>Note that changes to the
     *          returned object do not alter the packet.</i>
     */
    public: virtual Geolocation       getGeolocationGPS ()           const { return currentContextState.getGeolocationGPS(); }

    /** Gets the Formatted INS (Inertial Navigation System) Geolocation for the collector.
     *  @return The geolocation information (null if not specified). <i>Note that changes to the
     *          returned object do not alter the packet.</i>
     */
    public: virtual Geolocation       getGeolocationINS ()           const { return currentContextState.getGeolocationINS(); }

    /** Gets the ECEF (Earth-Centered, Earth-Fixed) Ephemeris for the collector. This will also
     *  get any "ECEF (Earth-Centered, Earth-Fixed) Ephemeris Adjunct" information from a
     *  VITA-49.0b packet (where VITA-49.0b is used).
     *  @return The ephemeris information (null if not specified). <i>Note that changes to the
     *          returned object do not alter the packet.</i>
     */
    public: virtual Ephemeris        getEphemerisECEF ()            const { return currentContextState.getEphemerisECEF(); }

    /** Gets the Relative Ephemeris for the collector. Unlike {@link #getEphemerisECEF()}
     *  which uses the ECEF coordinate system, this ephemeris is relative to a user-defined system
     *  specified by {@link #getEphemerisReference()}. This will also get any "Relative Ephemeris
     *  Adjunct" information from a VITA-49.0b packet (where VITA-49.0b is used).
     *  @return The ephemeris information (null if not specified). <i>Note that changes to the
     *          returned object do not alter the packet.</i>
     */
    public: virtual Ephemeris        getEphemerisRelative ()        const { return currentContextState.getEphemerisRelative(); }

    /** Gets the GPS ASCII "sentences". These are ASCII "sentences" from a GPS (Global
     *  Positioning System) receiver, such as those conforming to NMEA-0183. <i>Due to the great
     *  variety in GPS "sentences", this class makes little effort to do anything useful with them.
     *  Users are encouraged to use {@link #getGeolocationGPS()} which essentially provides the same
     *  information, but in a consistent form.</i>
     *  @return The geolocation information (null if not specified). <i>Note that changes to the
     *          returned object do not alter the packet.</i>
     */
    public: virtual GeoSentences      getGeoSentences ()             const { return currentContextState.getGeoSentences(); }

    /** Gets the Context Association Lists. These lists indicate the other context/data
     *  streams associated with this one.
     *  @return The context association lists (null if not specified). <i>Note that changes to the
     *          returned object do not alter the packet.</i>
     */
    public: virtual ContextAssocLists getContextAssocLists ()        const { return currentContextState.getContextAssocLists(); }

    /** Gets the Device Identifier specifying the manufacturer and model of the device
     *  producing this context packet stream.
     *  @return The device identifier as a string (null if not specified).
     */
    public: virtual string            getDeviceID ()                 const { return currentContextState.getDeviceID(); }

    /** Gets the Bandwidth of the signal in Hz.
     *  @return The bandwidth (null if not specified).
     */
    public: virtual double            getBandwidth ()                const { return currentContextState.getBandwidth(); }

    /** Gets the IF Reference Frequency of the signal in Hz.
     *  @return The frequency (null if not specified).
     */
    public: virtual double            getFrequencyIF ()              const { return currentContextState.getFrequencyIF(); }

    /** Gets the RF Reference Frequency of the signal in Hz.
     *  @return The frequency (null if not specified).
     */
    public: virtual double            getFrequencyRF ()              const { return currentContextState.getFrequencyRF(); }

    /** Gets the RF Reference Frequency Offset of the signal in Hz.
     *  @return The frequency offset (null if not specified).
     */
    public: virtual double            getFrequencyOffsetRF ()        const { return currentContextState.getFrequencyOffsetRF(); }

    /** Gets the IF Band Offset of the signal in Hz.
     *  @return The band offset (null if not specified).
     */
    public: virtual double            getBandOffsetIF ()             const { return currentContextState.getBandOffsetIF(); }

    /** Gets the Reference Level of the signal in dBm.
     *  @return The reference level (null if not specified).
     */
    public: virtual float             getReferenceLevel ()           const { return currentContextState.getReferenceLevel(); }

    /** Gets the Stage 1 Gain of the device in dB. This is the front-end gain of the
     *  system. In cases where a separate Stage 1 and Stage 2 gain is not necessary, this holds
     *  the total gain of the system and Gain 2 is set to zero.
     *  @return The gain (null if not specified).
     */
    public: virtual float             getGain1 ()                    const { return currentContextState.getGain1(); }

    /** Gets the Stage 2 Gain of the device in dB. This is the back-end gain of the
     *  system. In cases where a separate Stage 1 and Stage 2 gain is not necessary, the Stage 1
     *  gain holds the total gain of the system and this is set to zero.
     *  @return The gain (null if not specified).
     */
    public: virtual float             getGain2 ()                    const { return currentContextState.getGain2(); }

    /** Gets the Sample Rate in Hz.
     *  @return The sample rate (null if not specified).
     */
    public: virtual double            getSampleRate ()               const { return currentContextState.getSampleRate(); }

    /** Gets the Temperature in degrees Celsius. This is used to convey the temperature of
     *  any component that may affect the described signal.
     *  @return The temperature (null if not specified).
     */
    public: virtual float             getTemperature ()              const { return currentContextState.getTemperature(); }

    /** Gets the User-Defined Bits from the State and Event Indicator Bits. <i>The
     *  definition of these bits should be included in the documentation for the relevent packet
     *  class.</i> Unlike other bits of the State and Event Indicator Field, there is no concept
     *  of an undefined-value for user bits if the State and Event Indicator Field is present to
     *  support some other bits. As such, zero will be returned if the State and Event Indicator
     *  Field is absent.
     *  @return true if the flag is set, false if it is not set, zero if the State and Event
     *  Indicator Field is absent.
     */
    public: virtual int32_t           getUserDefinedBits ()          const { return currentContextState.getUserDefinedBits(); }

    /** Gets the Data Packet Payload Format. This specifies the format of the data in the
     *  paired data packet stream.
     *  @return The payload format (null if not specified). <i>Note that changes to the returned
     *          object do not alter the packet.</i>
     */
    public: virtual PayloadFormat     getDataPayloadFormat ()        const { return currentContextState.getDataPayloadFormat(); }

    /** Gets the Total Gain of the device in dB. The total gain is the sum of
     *  {@link #getGain1()} and {@link #getGain2()}.
     *  @return The gain (null if not specified).
     */
    public: virtual float getGain () const { return currentContextState.getGain(); }

    /** Gets the Over-Range Count. This is the count of the number of over-range data
     *  samples in the <b>single</b> paired data packet.
     *  @return The count (null if not specified).
     */
    public: virtual int64_t getOverRangeCount () const { throw VRTException("State does not maintain non-persistent fields."); }

    /** Gets the Device Identifier specifying the manufacturer and model of the device
     *  producing this context packet stream.
     *  @return The device identifier (null if not specified).
     */
    public: virtual int64_t getDeviceIdentifier () const { return currentContextState.getDeviceIdentifier(); }

    /** Gets the header type, used with getOffset(..) and OFFSET_TABLE. */
    public: virtual int32_t getContextIndicatorField () const { return currentContextState.getContextIndicatorField(); }

    /** Gets the Last update time for the state. */
    public: virtual TimeStamp getLastUpdated () const { return lastUpdated; }

    /** Gets the Last changed time for the state. */
    public: virtual TimeStamp getLastChanged () const { return lastChanged; }

    /** Gets the total number of samples that were over-range since state was maintained. */
    public: virtual int64_t getTotalOverRangeSamples () const { return totalOverRangeSamples; }

    /** Gets the total number of packets that were over-range since state was maintained. */
    public: virtual int64_t getTotalOverRangePackets () const { return totalOverRangePackets; }

    /** Gets the total number of packets containing discontinuous packets since state was maintained. */
    public: virtual int64_t getTotalDiscontinuousPackets () const { return totalDiscontinuousPackets; }

    public: virtual Value* getField (int32_t id) const { return currentContextState.getField(id); }
    public: virtual ValueType getFieldType (int32_t id) const { return currentContextState.getFieldType(id); }
    public: virtual string    getFieldName  (int32_t id) const { return currentContextState.getFieldName(id); };
    public: virtual int32_t    getFieldCount  () const { return currentContextState.getFieldCount(); };
    public: virtual void       setField  (int32_t id, const Value* val) { throw VRTException("Cannot set field in state."); };

    /** Is this object equal to null. */
    public: inline virtual bool isNullValue () const {
      return (!initialized);
    }

  };

}

#endif /* _BasicVRTState_h */

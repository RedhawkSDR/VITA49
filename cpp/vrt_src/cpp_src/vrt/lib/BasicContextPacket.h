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

#ifndef _BasicContextPacket_h
#define _BasicContextPacket_h

#include "BasicVRTPacket.h"
#include "VRTConfig.h"
#include <map>

namespace vrt {

  using namespace std;
  using namespace VRTMath;
  class AbstractGeolocation;  // declared below
  class Geolocation;          // declared below
  class Ephemeris;            // declared below
  class EphemerisAdjunct;     // declared below
  class ContextAssocLists;    // declared below
  class GeoSentences;         // declared below

  /** <b>Internal use only:</b> Only used within BasicContextPacket.h and BasicContextPacket.cc */
  namespace private_BasicContextPacket {
    // CONTEXT FIELDS
    static const int32_t CONTEXT_IND    = 0; // special case
    static const int32_t CHANGE_IND     = 0x80000000;
    static const int32_t REF_POINT      = 0x40000000;
    static const int32_t BANDWIDTH      = 0x20000000;
    static const int32_t IF_FREQ        = 0x10000000;
    static const int32_t RF_FREQ        = 0x08000000;
    static const int32_t RF_OFFSET      = 0x04000000;
    static const int32_t IF_OFFSET      = 0x02000000;
    static const int32_t REF_LEVEL      = 0x01000000;
    static const int32_t GAIN           = 0x00800000;
    static const int32_t OVER_RANGE     = 0x00400000;
    static const int32_t SAMPLE_RATE    = 0x00200000;
    static const int32_t TIME_ADJUST    = 0x00100000;
    static const int32_t TIME_CALIB     = 0x00080000;
    static const int32_t TEMPERATURE    = 0x00040000;
    static const int32_t DEVICE_ID      = 0x00020000;
    static const int32_t STATE_EVENT    = 0x00010000;
    static const int32_t DATA_FORMAT    = 0x00008000;
    static const int32_t GPS_EPHEM      = 0x00004000;
    static const int32_t INS_EPHEM      = 0x00002000;
    static const int32_t ECEF_EPHEM     = 0x00001000;
    static const int32_t REL_EPHEM      = 0x00000800;
    static const int32_t EPHEM_REF      = 0x00000400;
    static const int32_t GPS_ASCII      = 0x00000200;
    static const int32_t CONTEXT_ASOC   = 0x00000100;
    static const int32_t ECEF_EPHEM_ADJ = 0x00000080;
    static const int32_t REL_EPHEM_ADJ  = 0x00000040;
    
    // Field lengths in the IFContext packet. Note that the entries in CTX_36_OCTETS
    // all come *after* the variable-length fields, and all others come before.
    static const int32_t CTX_4_OCTETS  = REF_POINT     | REF_LEVEL     | GAIN          | OVER_RANGE
                                       | TIME_CALIB    | TEMPERATURE   | STATE_EVENT   | EPHEM_REF;
    static const int32_t CTX_8_OCTETS  = BANDWIDTH     | IF_FREQ       | RF_FREQ       | RF_OFFSET
                                       | IF_OFFSET     | SAMPLE_RATE   | TIME_ADJUST   | DEVICE_ID
                                       | DATA_FORMAT;
    static const int32_t CTX_44_OCTETS = GPS_EPHEM     | INS_EPHEM;
    static const int32_t CTX_36_OCTETS = REL_EPHEM_ADJ | ECEF_EPHEM_ADJ;
    static const int32_t CTX_52_OCTETS = ECEF_EPHEM    | REL_EPHEM;
  };
  using namespace private_BasicContextPacket;


  /** Represents a GPS or INS geolocation fix. <i>The fact that {@link GeoSentences}
   *  does not extend this class is not accidental.</i>
   *  @see            Geolocation
   *  @see            Ephemeris
   *  @author         
   */
  class AbstractGeolocation : public Record {
    /** Creates a new instance with the given size.
     *  @param len The record length in bytes.
     */
    public: AbstractGeolocation (size_t len);

    /** Basic copy constructor. */
    public: AbstractGeolocation (const AbstractGeolocation &r);

    public: virtual string toString () const;

    /** Gets the manufacturer of the GPS/INS device.
     *  @return The GPS/INS manufacturer OUI.
     */
    public: inline int32_t getManufacturerIdentifier () const {
      return unpackInt(0) & 0x00FFFFFF;
    }

    /** Sets the manufacturer of the GPS/INS device.
     *  @param oui The GPS/INS manufacturer OUI.
     *  @throws VRTException If the OUI is invalid.
     */
    public: inline void setManufacturerIdentifier (int32_t oui) {
      if ((oui & 0xFF000000) != 0) throw VRTException("Invalid OUI (%d).", oui);
      int32_t val = (unpackByte(0) << 24) | oui;
      packInt(0, val);
    }

    /** Gets the manufacturer of the GPS/INS device.
     *  @return The GPS/INS manufacturer OUI.
     */
    public: inline string getManufacturerID () const {
      return Utilities::toStringOUI(getManufacturerIdentifier());
    }

    /** Sets the manufacturer of the GPS/INS device.
     *  @param oui The GPS/INS manufacturer OUI.
     *  @throws VRTException If the OUI is invalid.
     */
    public: inline void setManufacturerID (const string &oui) {
      if (oui.length() == 0) throw VRTException("OUI can not be null");
      setManufacturerIdentifier(Utilities::fromStringOUI(oui));
    }

    /** Gets the time stamp for the geolocation fix.
     *  @return The time stamp for the geolocation fix or a null time if such is not supported.
     */
    public: TimeStamp getTimeStamp () const;

    /** Sets the time stamp for the geolocation fix.
     *  @param ts The time stamp for the geolocation fix.
     */
    protected: virtual void setTimeStamp (const TimeStamp &ts);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Implement HasFields
    //////////////////////////////////////////////////////////////////////////////////////////////////
    public: virtual int32_t   getFieldCount () const;
    public: virtual string    getFieldName  (int32_t id) const;
    public: virtual ValueType getFieldType  (int32_t id) const;
    public: virtual Value*    getField      (int32_t id) const;
    public: virtual void      setField      (int32_t id, const Value* val);
  };

  /** Represents a GPS or INS geolocation fix.
   *  @see            Ephemeris
   *  @see            GeoSentences
   *  @author         
   */
  class Geolocation : public AbstractGeolocation {
    /** Creates a new instance. */
    public: Geolocation ();

    /** Basic copy constructor. */
    public: Geolocation (const Geolocation &r);

    public: virtual string toString () const;

    /** Sets the time stamp for the geolocation fix.
     *  @param ts The time stamp for the geolocation fix.
     */
    public: inline void setTimeStamp (const TimeStamp &ts) {
      AbstractGeolocation::setTimeStamp(ts);
    }

    /** Gets the latitude of the fix.
     *  @return The latitude in degrees.
     */
    public: inline double getLatitude () const {
      int32_t bits = unpackInt(16);
      return (bits == 0x7FFFFFFF)? DOUBLE_NAN : toDouble32(22, bits);
    }

    /** Gets the longitude of the fix.
     *  @return The longitude in degrees.
     */
    public: inline double getLongitude () const {
      int32_t bits = unpackInt(20);
      return (bits == 0x7FFFFFFF)? DOUBLE_NAN : toDouble32(22, bits);
    }

    /** Gets the altitude of the fix.
     *  @return The altitude in meters (whether this is Mean Sea Level or WGS-84 is defined elsewhere).
     */
    public: inline double getAltitude () const {
      int32_t bits = unpackInt(24);
      return (bits == 0x7FFFFFFF)? DOUBLE_NAN : toDouble32(5, bits);
    }

    /** Gets the speed over the ground.
     *  @return The speed over the ground in meters/second.
     */
    public: inline double getSpeedOverGround () const {
      int32_t bits = unpackInt(28);
      return (bits == 0x7FFFFFFF)? DOUBLE_NAN : toDouble32(16, bits);
    }

    /** Gets the heading of the vehicle. The vehicle heading specifies the vehicle orientation.
     *  @return The vehicle heading in degrees.
     */
    public: inline double getHeadingAngle () const {
      int32_t bits = unpackInt(32);
      return (bits == 0x7FFFFFFF)? DOUBLE_NAN : toDouble32(22, bits);
    }

    /** Gets the heading of the vehicle. The track angle specifies the direction of travel.
     *  @return The track angle in degrees.
     */
    public: inline double getTrackAngle () const {
      int32_t bits = unpackInt(36);
      return (bits == 0x7FFFFFFF)? DOUBLE_NAN : toDouble32(22, bits);
    }

    /** Gets the magnetic variation from true north.
     *  @return The magnetic variation in degrees.
     */
    public: inline double getMagneticVariation () const {
      int32_t bits = unpackInt(40);
      return (bits == 0x7FFFFFFF)? DOUBLE_NAN : toDouble32(22, bits);
    }

    /** Gets the latitude of the fix.
     *  @param val val The latitude in degrees.
     */
    public: inline void setLatitude (double val) {
      int32_t bits = (isNull(val))? 0x7FFFFFFF : fromDouble32(22, val);
      packInt(16, bits);
    }

    /** Gets the longitude of the fix.
     *  @param val The longitude in degrees.
     */
    public: inline void setLongitude (double val) {
      int32_t bits = (isNull(val))? 0x7FFFFFFF : fromDouble32(22, val);
      packInt(20, bits);
    }

    /** Gets the altitude of the fix.
     *  @param val The altitude in meters (whether this is Mean Sea Level or WGS-84 is defined elsewhere).
     */
    public: inline void setAltitude (double val) {
      int32_t bits = (isNull(val))? 0x7FFFFFFF : fromDouble32(5, val);
      packInt(24, bits);
    }

    /** Gets the speed over the ground.
     *  @param val The speed over the ground in meters/second.
     */
    public: inline void setSpeedOverGround (double val) {
      int32_t bits = (isNull(val))? 0x7FFFFFFF : fromDouble32(16, val);
      packInt(28, bits);
    }

    /** Gets the heading of the vehicle. The vehicle heading specifies the vehicle orientation.
     *  @param val The vehicle heading in degrees.
     */
    public: inline void setHeadingAngle (double val) {
      int32_t bits = (isNull(val))? 0x7FFFFFFF : fromDouble32(22, val);
      packInt(32, bits);
    }

    /** Gets the heading of the vehicle. The track angle specifies the direction of travel.
     *  @param val The track angle in degrees.
     */
    public: inline void setTrackAngle (double val) {
      int32_t bits = (isNull(val))? 0x7FFFFFFF : fromDouble32(22, val);
      packInt(36, bits);
    }

    /** Gets the magnetic variation from true north.
     *  @param val The magnetic variation in degrees.
     */
    public: inline void setMagneticVariation (double val) {
      int32_t bits = (isNull(val))? 0x7FFFFFFF : fromDouble32(22, val);
      packInt(40, bits);
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Implement HasFields
    //////////////////////////////////////////////////////////////////////////////////////////////////
    public: virtual int32_t   getFieldCount () const;
    public: virtual string    getFieldName  (int32_t id) const;
    public: virtual ValueType getFieldType  (int32_t id) const;
    public: virtual Value*    getField      (int32_t id) const;
    public: virtual void      setField      (int32_t id, const Value* val);
  };

  /** Represents an Ephemeris Adjunct as used with VITA-49.0b. This class should not be used on
   *  its own, but rather via the {@link Ephemeris} class.
   *  @see            Ephemeris
   *  @author         
   */
  class EphemerisAdjunct : public Record {
    /** <b>Internal use only:</b> Creates a new instance. */
    public: EphemerisAdjunct ();

    /** Copy constructor. */
    public: EphemerisAdjunct (const EphemerisAdjunct& r);

    /** Is anything set in this record? */
    public: virtual bool isAnythingSet () const;

    public: virtual string toString () const;

    /** Gets the rotational velocity about the Z-axis in degrees per second.
     *  @return The value of null if not specified.
     */
    public: inline double getRotationalVelocityAlpha () const {
      int32_t bits = unpackInt(0);
      return (bits == 0x7FFFFFFF)? DOUBLE_NAN : toDouble32(16, bits);
    }

    /** Gets the rotational velocity about the Y-axis in degrees per second.
     *  @return The value of DOUBLE_NAN if not specified.
     */
    public: inline double getRotationalVelocityBeta () const {
      int32_t bits = unpackInt(4);
      return (bits == 0x7FFFFFFF)? DOUBLE_NAN : toDouble32(16, bits);
    }

    /** Gets the rotational velocity about the X-axis in degrees per second.
     *  @return The value of DOUBLE_NAN if not specified.
     */
    public: inline double getRotationalVelocityPhi () const {
      int32_t bits = unpackInt(8);
      return (bits == 0x7FFFFFFF)? DOUBLE_NAN : toDouble32(16, bits);
    }

    /** Sets the rotational velocity about the Z-axis in degrees per second.
     *  @param val The value or DOUBLE_NAN if not specified.
     */
    public: inline void setRotationalVelocityAlpha (double val) {
      int32_t bits = (isNull(val))? 0x7FFFFFFF : fromDouble32(16, val);
      packInt(0, bits);
    }

    /** Sets the rotational velocity about the Y-axis in degrees per second.
     *  @param val The value or DOUBLE_NAN if not specified.
     */
    public: inline void setRotationalVelocityBeta (double val) {
      int32_t bits = (isNull(val))? 0x7FFFFFFF : fromDouble32(16, val);
      packInt(4, bits);
    }

    /** Sets the rotational velocity about the X-axis in degrees per second.
     *  @param val The value or DOUBLE_NAN if not specified.
     */
    public: inline void setRotationalVelocityPhi (double val) {
      int32_t bits = (isNull(val))? 0x7FFFFFFF : fromDouble32(16, val);
      packInt(8, bits);
    }

    /** Gets the acceleration along the X-axis in meters per second<sup>2</sup>.
     *  @return The value of DOUBLE_NAN if not specified.
     */
    public: inline double getAccelerationX () const {
      int32_t bits = unpackInt(12);
      return (bits == 0x7FFFFFFF)? DOUBLE_NAN : toDouble32(24, bits);
    }

    /** Gets the acceleration along the Y-axis in meters per second<sup>2</sup>.
     *  @return The value of DOUBLE_NAN if not specified.
     */
    public: inline double getAccelerationY () const {
      int32_t bits = unpackInt(16);
      return (bits == 0x7FFFFFFF)? DOUBLE_NAN : toDouble32(24, bits);
    }

    /** Gets the acceleration along the Z-axis in meters per second<sup>2</sup>.
     *  @return The value of DOUBLE_NAN if not specified.
     */
    public: inline double getAccelerationZ () const {
      int32_t bits = unpackInt(20);
      return (bits == 0x7FFFFFFF)? DOUBLE_NAN : toDouble32(24, bits);
    }

    /** Sets the acceleration along the X-axis in meters per second<sup>2</sup>.
     *  @param val The value or DOUBLE_NAN if not specified.
     */
    public: inline void setAccelerationX (double val) {
      int32_t bits = (isNull(val))? 0x7FFFFFFF : fromDouble32(24, val);
      packInt(12, bits);
    }

    /** Sets the acceleration along the Y-axis in meters per second<sup>2</sup>.
     *  @param val The value or DOUBLE_NAN if not specified.
     */
    public: inline void setAccelerationY (double val) {
      int32_t bits = (isNull(val))? 0x7FFFFFFF : fromDouble32(24, val);
      packInt(16, bits);
    }

    /** Sets the acceleration along the Z-axis in meters per second<sup>2</sup>.
     *  @param val The value or DOUBLE_NAN if not specified.
     */
    public: inline void setAccelerationZ (double val) {
      int32_t bits = (isNull(val))? 0x7FFFFFFF : fromDouble32(24, val);
      packInt(20, bits);
    }

    /** Gets the rotational acceleration about the Z-axis in degrees per second<sup>2</sup>.
     *  @return The value of DOUBLE_NAN if not specified.
     */
    public: inline double getRotationalAccelerationAlpha () const {
      int32_t bits = unpackInt(24);
      return (bits == 0x7FFFFFFF)? DOUBLE_NAN : toDouble32(23, bits);
    }

    /** Gets the rotational acceleration about the Y-axis in degrees per second<sup>2</sup>.
     *  @return The value of DOUBLE_NAN if not specified.
     */
    public: inline double getRotationalAccelerationBeta () const {
      int32_t bits = unpackInt(28);
      return (bits == 0x7FFFFFFF)? DOUBLE_NAN : toDouble32(23, bits);
    }

    /** Gets the rotational acceleration about the X-axis in degrees per second<sup>2</sup>.
     *  @return The value of DOUBLE_NAN if not specified.
     */
    public: inline double getRotationalAccelerationPhi () const {
      int32_t bits = unpackInt(32);
      return (bits == 0x7FFFFFFF)? DOUBLE_NAN : toDouble32(23, bits);
    }

    /** Sets the rotational acceleration about the Z-axis in degrees per second<sup>2</sup>.
     *  @param val The value or DOUBLE_NAN if not specified.
     */
    public: inline void setRotationalAccelerationAlpha (double val) {
      int32_t bits = (isNull(val))? 0x7FFFFFFF : fromDouble32(23, val);
      packInt(24, bits);
    }

    /** Sets the rotational acceleration about the Y-axis in degrees per second<sup>2</sup>.
     *  @param val The value or DOUBLE_NAN if not specified.
     */
    public: inline void setRotationalAccelerationBeta (double val) {
      int32_t bits = (isNull(val))? 0x7FFFFFFF : fromDouble32(23, val);
      packInt(28, bits);
    }

    /** Sets the rotational acceleration about the X-axis in degrees per second<sup>2</sup>.
     *  @param val The value or DOUBLE_NAN if not specified.
     */
    public: inline void setRotationalAccelerationPhi (double val) {
      int32_t bits = (isNull(val))? 0x7FFFFFFF : fromDouble32(23, val);
      packInt(32, bits);
    }
  };

  /** Represents an ECEF ephemeris fix. This instance will always be associated with an
   *  {@link EphemerisAdjunct}, but the adjunct will only be initialized with non-null values
   *  if VITA-49.0b is used *and* the adjunct is actually present. It will be written out only
   *  if VITA-49.0b is used and one or more values are non-null. This allows code to be written
   *  that presumes the presence of the adjunct (i.e. assumes VITA-49.0b) but still functions
   *  in situations where it can not be inserted into the packet (i.e. strict VITA-49.0).
   *  @see            Geolocation
   *  @see            EphemerisAdjunct
   *  @author         
   */
  class Ephemeris : public AbstractGeolocation {
    /** The adjunct record, or null if not present. */
    private: EphemerisAdjunct adjunct;

    /** Creates a new instance. */
    public: Ephemeris ();

    /** Basic copy constructor. */
    public: Ephemeris (const Ephemeris &r);

    public: virtual string toString () const;

    /** Gets the ephemeris adjunct. */
    public: inline EphemerisAdjunct getAdjunct () const {
      return adjunct;
    }

    /** Gets the reference to the ephemeris adjunct. */
    public: inline EphemerisAdjunct& getAdjunctRef () {
      return adjunct;
    }

    /** Sets the time stamp for the geolocation fix.
     *  @param ts The time stamp for the geolocation fix.
     */
    public: inline void setTimeStamp (const TimeStamp &ts) {
      AbstractGeolocation::setTimeStamp(ts);
    }

    /** Gets the position along the X-axis in meters.
     *  @return The value of null if not specified.
     */
    public: inline double getPositionX () const {
      int32_t bits = unpackInt(16);
      return (bits == 0x7FFFFFFF)? DOUBLE_NAN : toDouble32(5, bits);
    }

    /** Gets the position along the Y-axis in meters.
     *  @return The value of null if not specified.
     */
    public: inline double getPositionY () const {
      int32_t bits = unpackInt(20);
      return (bits == 0x7FFFFFFF)? DOUBLE_NAN : toDouble32(5, bits);
    }

    /** Gets the position along the Z-axis in meters.
     *  @return The value of null if not specified.
     */
    public: inline double getPositionZ () const {
      int32_t bits = unpackInt(24);
      return (bits == 0x7FFFFFFF)? DOUBLE_NAN : toDouble32(5, bits);
    }

    /** Sets the position along the X-axis in meters.
     *  @param val The value or null if not specified.
     */
    public: inline void setPositionX (double val) {
      int32_t bits = (isNull(val))? 0x7FFFFFFF : fromDouble32(5, val);
      packInt(16, bits);
    }

    /** Sets the position along the Y-axis in meters.
     *  @param val The value or null if not specified.
     */
    public: inline void setPositionY (double val) {
      int32_t bits = (isNull(val))? 0x7FFFFFFF : fromDouble32(5, val);
      packInt(20, bits);
    }

    /** Sets the position along the Z-axis in meters.
     *  @param val The value or null if not specified.
     */
    public: inline void setPositionZ (double val) {
      int32_t bits = (isNull(val))? 0x7FFFFFFF : fromDouble32(5, val);
      packInt(24, bits);
    }

    /** Gets the attitude about the Z-axis in degrees.
     *  @return The value of null if not specified.
     */
    public: inline double getAttitudeAlpha () const {
      int32_t bits = unpackInt(28);
      return (bits == 0x7FFFFFFF)? DOUBLE_NAN : toDouble32(22, bits);
    }

    /** Gets the attitude about the Y-axis in degrees.
     *  @return The value of null if not specified.
     */
    public: inline double getAttitudeBeta () const {
      int32_t bits = unpackInt(32);
      return (bits == 0x7FFFFFFF)? DOUBLE_NAN : toDouble32(22, bits);
    }

    /** Gets the attitude about the X-axis in degrees.
     *  @return The value of null if not specified.
     */
    public: inline double getAttitudePhi () const {
      int32_t bits = unpackInt(36);
      return (bits == 0x7FFFFFFF)? DOUBLE_NAN : toDouble32(22, bits);
    }

    /** Sets the attitude about the Z-axis in degrees.
     *  @param val The value or null if not specified.
     */
    public: inline void setAttitudeAlpha (double val) {
      int32_t bits = (isNull(val))? 0x7FFFFFFF : fromDouble32(22, val);
      packInt(28, bits);
    }

    /** Sets the attitude about the Y-axis in degrees.
     *  @param val The value or null if not specified.
     */
    public: inline void setAttitudeBeta (double val) {
      int32_t bits = (isNull(val))? 0x7FFFFFFF : fromDouble32(22, val);
      packInt(32, bits);
    }

    /** Sets the attitude about the X-axis in degrees.
     *  @param val The value or null if not specified.
     */
    public: inline void setAttitudePhi (double val) {
      int32_t bits = (isNull(val))? 0x7FFFFFFF : fromDouble32(22, val);
      packInt(36, bits);
    }

    /** Gets the velocity along the X-axis in meters per second.
     *  @return The value of null if not specified.
     */
    public: inline double getVelocityX () const {
      int32_t bits = unpackInt(40);
      return (bits == 0x7FFFFFFF)? DOUBLE_NAN : toDouble32(16, bits);
    }

    /** Gets the velocity along the Y-axis in meters per second.
     *  @return The value of null if not specified.
     */
    public: inline double getVelocityY () const {
      int32_t bits = unpackInt(44);
      return (bits == 0x7FFFFFFF)? DOUBLE_NAN : toDouble32(16, bits);
    }

    /** Gets the velocity along the Z-axis in meters per second.
     *  @return The value of null if not specified.
     */
    public: inline double getVelocityZ () const {
      int32_t bits = unpackInt(48);
      return (bits == 0x7FFFFFFF)? DOUBLE_NAN : toDouble32(16, bits);
    }

    /** Sets the velocity along the X-axis in meters per second.
     *  @param val The value or null if not specified.
     */
    public: inline void setVelocityX (double val) {
      int32_t bits = (isNull(val))? 0x7FFFFFFF : fromDouble32(16, val);
      packInt(40, bits);
    }

    /** Sets the velocity along the Y-axis in meters per second.
     *  @param val The value or null if not specified.
     */
    public: inline void setVelocityY (double val) {
      int32_t bits = (isNull(val))? 0x7FFFFFFF : fromDouble32(16, val);
      packInt(44, bits);
    }

    /** Sets the velocity along the Z-axis in meters per second.
     *  @param val The value or null if not specified.
     */
    public: inline void setVelocityZ (double val) {
      int32_t bits = (isNull(val))? 0x7FFFFFFF : fromDouble32(16, val);
      packInt(48, bits);
    }


    /** Gets the rotational velocity about the Z-axis in degrees per second.
     *  @return The value of null if not specified.
     */
    public: inline double getRotationalVelocityAlpha () const {
      return adjunct.getRotationalVelocityAlpha();
    }

    /** Gets the rotational velocity about the Y-axis in degrees per second.
     *  @return The value of null if not specified.
     */
    public: inline double getRotationalVelocityBeta () const {
      return adjunct.getRotationalVelocityBeta();
    }

    /** Gets the rotational velocity about the X-axis in degrees per second.
     *  @return The value of null if not specified.
     */
    public: inline double getRotationalVelocityPhi () const {
      return adjunct.getRotationalVelocityPhi();
    }

    /** Sets the rotational velocity about the Z-axis in degrees per second.
     *  @param val The value or null if not specified.
     */
    public: inline void setRotationalVelocityAlpha (double val) {
      adjunct.setRotationalVelocityAlpha(val);
    }

    /** Sets the rotational velocity about the Y-axis in degrees per second.
     *  @param val The value or null if not specified.
     */
    public: inline void setRotationalVelocityBeta (double val) {
      adjunct.setRotationalVelocityBeta(val);
    }

    /** Sets the rotational velocity about the X-axis in degrees per second.
     *  @param val The value or null if not specified.
     */
    public: inline void setRotationalVelocityPhi (double val) {
      adjunct.setRotationalVelocityPhi(val);
    }



    /** Gets the acceleration along the X-axis in meters per second<sup>2</sup>.
     *  @return The value of null if not specified.
     */
    public: inline double getAccelerationX () const {
      return adjunct.getAccelerationX();
    }

    /** Gets the acceleration along the Y-axis in meters per second<sup>2</sup>.
     *  @return The value of null if not specified.
     */
    public: inline double getAccelerationY () const {
      return adjunct.getAccelerationY();
    }

    /** Gets the acceleration along the Z-axis in meters per second<sup>2</sup>.
     *  @return The value of null if not specified.
     */
    public: inline double getAccelerationZ () const {
      return adjunct.getAccelerationZ();
    }

    /** Sets the acceleration along the X-axis in meters per second<sup>2</sup>.
     *  @param val The value or null if not specified.
     */
    public: inline void setAccelerationX (double val) {
      adjunct.setAccelerationX(val);
    }

    /** Sets the acceleration along the Y-axis in meters per second<sup>2</sup>.
     *  @param val The value or null if not specified.
     */
    public: inline void setAccelerationY (double val) {
      adjunct.setAccelerationY(val);
    }

    /** Sets the acceleration along the Z-axis in meters per second<sup>2</sup>.
     *  @param val The value or null if not specified.
     */
    public: inline void setAccelerationZ (double val) {
      adjunct.setAccelerationZ(val);
    }



    /** Gets the rotational acceleration about the Z-axis in degrees per second<sup>2</sup>.
     *  @return The value of null if not specified.
     */
    public: inline double getRotationalAccelerationAlpha () const {
      return adjunct.getRotationalAccelerationAlpha();
    }

    /** Gets the rotational acceleration about the Y-axis in degrees per second<sup>2</sup>.
     *  @return The value of null if not specified.
     */
    public: inline double getRotationalAccelerationBeta () const {
      return adjunct.getRotationalAccelerationBeta();
    }

    /** Gets the rotational acceleration about the X-axis in degrees per second<sup>2</sup>.
     *  @return The value of null if not specified.
     */
    public: inline double getRotationalAccelerationPhi () const {
      return adjunct.getRotationalAccelerationPhi();
    }

    /** Sets the rotational acceleration about the Z-axis in degrees per second<sup>2</sup>.
     *  @param val The value or null if not specified.
     */
    public: inline void setRotationalAccelerationAlpha (double val) {
      adjunct.setRotationalAccelerationAlpha(val);
    }

    /** Sets the rotational acceleration about the Y-axis in degrees per second<sup>2</sup>.
     *  @param val The value or null if not specified.
     */
    public: inline void setRotationalAccelerationBeta (double val) {
      adjunct.setRotationalAccelerationBeta(val);
    }

    /** Sets the rotational acceleration about the X-axis in degrees per second<sup>2</sup>.
     *  @param val The value or null if not specified.
     */
    public: inline void setRotationalAccelerationPhi (double val) {
      adjunct.setRotationalAccelerationPhi(val);
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Implement HasFields
    //////////////////////////////////////////////////////////////////////////////////////////////////
    public: virtual int32_t   getFieldCount () const;
    public: virtual string    getFieldName  (int32_t id) const;
    public: virtual ValueType getFieldType  (int32_t id) const;
    public: virtual Value*    getField      (int32_t id) const;
    public: virtual void      setField      (int32_t id, const Value* val);
  };

  /** Represents GPS "sentences" describing a GPS geolocation fix. <br>
   *  <br>
   *  <i>Users are strongly encouraged to consider using the binary form when specifying a
   *  GPS geolocation fix (see {@link Geolocation}). There is no intention of adding any
   *  significant capabilities to this class.</i>
   *  @see            Geolocation
   *  @author         
   */
  class GeoSentences : public AbstractGeolocation {
    /** Creates a new instance. */
    public: GeoSentences ();

    /** Basic copy constructor. */
    public: GeoSentences (const GeoSentences &r);

    public: virtual string toString () const;

    public: inline virtual void writeBytes (const void *buffer) {
      // Write Counts
      setByteLength(8);
      AbstractGeolocation::writeBytes(buffer);

      // Size as appropriate
      int32_t numberOfWords = unpackInt(4);
      setByteLength(numberOfWords*4 + 8);

      // Write Data
      AbstractGeolocation::writeBytes(buffer);
    }

    /** Gets the time stamp for the geolocation fix.
     *  @return Since no standard time stamp exists for the sentences, this always returns a null value.
     */
    public: inline TimeStamp getTimeStamp () const {
      return TimeStamp(); // no time stamp for this class
    }

    /** Gets the GPS "sentences".
     *  @return The GPS "sentences".
     */
    public: inline string getSentences () const {
      return unpackAscii(8, getByteLength()-8);
    }

    /** Sets the GPS "sentences".
     *  @param s The GPS "sentences" (should be ASCII).
     */
    public: inline void setSentences (string s) {
      int32_t numberOfWords = (s.length() + 3) / 4; // round up to nearest word
      setByteLength(numberOfWords*4 + 8);
      packInt(4, numberOfWords);
      packAscii(8, s, numberOfWords*4);
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Implement HasFields
    //////////////////////////////////////////////////////////////////////////////////////////////////
    public: virtual int32_t   getFieldCount () const;
    public: virtual string    getFieldName  (int32_t id) const;
    public: virtual ValueType getFieldType  (int32_t id) const;
    public: virtual Value*    getField      (int32_t id) const;
    public: virtual void      setField      (int32_t id, const Value* val);
  };

  /** Specifies a Context Association List. */
  class ContextAssocLists : public Record {
    /** Creates a new instance. */
    public: ContextAssocLists ();

    /** Basic copy constructor. */
    public: ContextAssocLists (const ContextAssocLists &r);

    public: virtual string toString () const;

    public: inline virtual void writeBytes (const void *buffer) {
      // Write Counts
      setByteLength(8);
      Record::writeBytes(buffer);

      // Size as appropriate
      updateByteLength(-1);

      // Write Data
      Record::writeBytes(buffer);
    }

    /** Updates the byte length. */
    private: void updateByteLength (int32_t off);

    /** Gets the number of <b>source</b> context associations. */
    public: inline int32_t getSourceContextCount () const { return unpackShort(0) & 0x01FF; }
    /** Gets the number of <b>system</b> context associations. */
    public: inline int32_t getSystemContextCount () const { return unpackShort(2) & 0x01FF; }
    /** Gets the number of <b>vector-component</b> context associations. */
    public: inline int32_t getVectorComponentCount () const { return unpackShort(4) & 0xFFFF; }
    /** Gets the number of <b>Asynchronous-Channel</b> context associations. */
    public: inline int32_t getAsynchronousChannelCount () const { return unpackShort(6) & 0x7FFF; }
    /** Are the <b>Asynchronous-Channel</b> context associations accompanied by <b>Asynchronous-Channel Tags</b>? */
    public: inline bool getAsynchronousChannelTagsPresent () const { return (unpackShort(6) & 0x8000) != 0; }

    private: inline int32_t getSourceContextOffset ()           const { return 8; }
    private: inline int32_t getSystemContextOffset ()           const { return getSourceContextOffset()       + 4*getSourceContextCount(); }
    private: inline int32_t getVectorComponentOffset ()         const { return getSystemContextOffset()       + 4*getSystemContextCount(); }
    private: inline int32_t getAsynchronousChannelOffset ()     const { return getVectorComponentOffset()     + 4*getVectorComponentCount(); }
    private: inline int32_t getAsynchronousChannelTagsOffset () const { return getAsynchronousChannelOffset() + 4*getAsynchronousChannelCount(); }

    /** Does the resizing of a list. */
    private: void resize (int32_t pos, int32_t max, int32_t off, int32_t old, int32_t val);

    /** Sets the number of <b>source</b> context associations. */
    public: inline void setSourceContextCount (int32_t val) {
      resize(0, 0x1FF, getSourceContextOffset(), getSourceContextCount(), val);
    }

    /** Sets the number of <b>system</b> context associations. */
    public: inline void setSystemContextCount (int32_t val) {
      resize(2, 0x1FF, getSystemContextOffset(), getSystemContextCount(), val);
    }

    /** Sets the number of <b>vector-component</b> context associations. */
    public: inline void setVectorComponentCount (int32_t val) {
      resize(4, 0xFFFF, getVectorComponentOffset(), getVectorComponentCount(), val);
    }

    /** Sets the number of <b>Asynchronous-Channel</b> context associations. */
    public: inline void setAsynchronousChannelCount (int32_t val) {
      resize(6, 0x7FFF, getAsynchronousChannelOffset(), getAsynchronousChannelCount(), val);
      if (getAsynchronousChannelTagsPresent()) {
        resize(6, 0x7FFF, getAsynchronousChannelTagsOffset(), getAsynchronousChannelCount(), val);
      }
    }

    /** Are the <b>Asynchronous-Channel</b> context associations accompanied by <b>Asynchronous-Channel Tags</b>? */
    public: void setAsynchronousChannelTagsPresent (bool val);

    /** Gets a copy of the Source Context Association List.
     *  @return A copy of the association list (returns an arry of length 0 if there are no entries).
     */
    public: vector<int32_t> getSourceContext () const;

    /** Gets a copy of the System Context Association List.
     *  @return A copy of the association list (returns an arry of length 0 if there are no entries).
     */
    public: vector<int32_t> getSystemContext () const;

    /** Gets a copy of the Vector-Component Association List.
     *  @return A copy of the association list (returns an arry of length 0 if there are no entries).
     */
    public: vector<int32_t> getVectorComponent () const;

    /** Gets a copy of the Asynchronous-Channel Association List.
     *  @return A copy of the association list (returns an arry of length 0 if there are no entries).
     *  @see #getAsynchronousChannelTag()
     */
    public: vector<int32_t> getAsynchronousChannel () const;

    /** Gets a copy of the Asynchronous-Channel Tag List.
     *  @return A copy of the tag list (returns an arry of length 0 if there are no entries).
     *          The inclusion of the tag list is optional; if it is omitted, this will return null.
     *  @see #getAsynchronousChannel()
     *  @see #setAsynchronousChannel(int[],int[])
     */
    public: vector<int32_t> getAsynchronousChannelTag () const;

    /** Sets the Source Context Association List.
     *  @param val The association list (an arry of length 0 if there are no entries).
     */
    public: void setSourceContext (const vector<int32_t> &val);

    /** Sets the System Context Association List.
     *  @param val The association list (an arry of length 0 if there are no entries).
     */
    public: void setSystemContext (const vector<int32_t> &val);

    /** Sets the Vector-Component Association List.
     *  @param val The association list (an arry of length 0 if there are no entries).
     */
    public: void setVectorComponent (const vector<int32_t> &val);

    /** Sets the Asynchronous-Channel Association List. This is identical to
     *  <tt>setAsynchronousChannel(list,null)</tt>.
     *  @param val The association list (an arry of length 0 if there are no entries).
     */
    public: void setAsynchronousChannel (const vector<int32_t> &val);

    /** Sets the Asynchronous-Channel Association List.
     *  @param val  The association list (an arry of length 0 if there are no entries).
     *  @param tags The tag list (an arry of length 0 if there are no entries or null if not
     *              specified). The inclusion of the tag list is optional; if it is included, it
     *              must have the same length as <tt>list</tt>.
     *  @throws VRTException if <tt>tags</tt> is given but has an invalid length.
     *  @see #getAsynchronousChannelTag()
     */
    public: void setAsynchronousChannel (const vector<int32_t> &val, const vector<int32_t> &tags);
    
    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Implement HasFields
    //////////////////////////////////////////////////////////////////////////////////////////////////
    public: virtual int32_t   getFieldCount () const;
    public: virtual string    getFieldName  (int32_t id) const;
    public: virtual ValueType getFieldType  (int32_t id) const;
    public: virtual Value*    getField      (int32_t id) const;
    public: virtual void      setField      (int32_t id, const Value* val);
  };

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
   *    ----+--------------------------+------------------
   *    (N = number of fields in BasicVRTPacket)
   *  </pre>
   *
   *  @author 
   */
  class BasicContextPacket : public BasicVRTPacket {
    friend class BasicVRTState;

    /** Basic destructor. */
    public: ~BasicContextPacket () { }

    /** Basic copy constructor. */
    public: BasicContextPacket (const BasicVRTPacket &p);

    /** Internal constructor, for use by subclasses.
        @param p       The packet (already initialized).
        @param classID The expected classID.
        @throws VRTException If packet type or classID of the packet are invalid.
     */
    public: BasicContextPacket (const BasicVRTPacket &p, int64_t classID);

    /** Creates a new instance accessing the given data buffer.
     *  @param buf      The pointer to the packet buffer.
     *  @param len      The length  of the packet buffer.
     *  @param readOnly Should users of this instance be able to modify the underlying data buffer?
     */
    public: BasicContextPacket (const void *buf, size_t len, bool readOnly=false);

    /** Creates a new instance that can be written to. Initially this will just be a simple context
     *  packet with no fields set (other than the required packet length of 8 and a default stream ID
     *  of 0).
     */
    public: BasicContextPacket ();
    public: BasicContextPacket (const int32_t pktsize);

    public: BasicContextPacket (const vector<char> &buf, ssize_t start, ssize_t end, bool readOnly = false);

    /** Creates a new instance accessing the given data buffer. Note that when the buffer lengths
     *  are given, only the most minimal of error checking is done. Users should call
     *  {@link #isPacketValid()} to verify that the packet is valid. Invalid packets can result
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
        @param p                The packet (already initialized).
        @param type             The expected packet type.
        @param classID          The expected classID.
        @param minPayloadLength The minimum payload length.
        @param maxPayloadLength The maximum payload length (&lt;0 if not pre-defined).
        @throws VRTException If packet type or classID of the packet are invalid, or if
                             the payload length is invalid.
     */
    protected: BasicContextPacket (const BasicVRTPacket &p, PacketType type, int64_t classID,
                                   int32_t minPayloadLength, int32_t maxPayloadLength);

    /** Gets the specified bit from the trailer.
     *  @param enable    Bit position of the enable flag.
     *  @param indicator Bit position of the indicator flag.
     *  @return null if there is no trailer, null if the enable bit is not set, true if the enable
     *          bit is set and the indicator bit is set, false if the enable bit is set but the
     *          indicator bit is not set.
     */
    protected: boolNull getStateEventBit (int32_t enable, int32_t indicator) const;

    /** <i>Optional functionality:</i> Sets the specified bit from the trailer.
     *  @param enable    Bit position of the enable flag.
     *  @param indicator Bit position of the indicator flag.
     *  @param value     The value of the bits: null if the enable bit should not be set, true if
     *                   the enable bit and the indicator bit should be set, false if the enable bit
     *                   should be set but the indicator bit should not be set.
     */
    protected: void setStateEventBit (int32_t enable, int32_t indicator, boolNull value);

    /** Gets the offset for the given header field. */
    protected: int32_t getOffset (int32_t field) const;

    /** Gets the length of the given field when present (-1 if variable, -2 if not found). */
    protected: int32_t getFieldLen (int32_t field) const;

    public: virtual string getPacketValid (bool strict, int32_t length) const;

    public: virtual inline void setPacketType (PacketType t) {
      if (t != PacketType_Context) {
        throw VRTException("Can not change packet type to non-Context type when using BasicContextPacket");
      }
      BasicVRTPacket::setPacketType(t);
    }

    protected: virtual void toStringStream (ostringstream &str) const;

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

    /** Gets the adjusted time stamp of the packet. This is implemented as follows:
     *  <pre>
     *  public: TimeStamp getAdjustedTimeStamp () const {
     *    int64_t adj = getTimeStampAdjustment();
     *    return (adj == INT64_NULL)? getTimeStamp() : getTimeStamp().addPicoSeconds(adj);
     *  }
     *  </pre>
     *  @return The adjusted time stamp of the packet (never null).
     */
    public: TimeStamp getAdjustedTimeStamp () const {
      int64_t adj = getTimeStampAdjustment();
      return (adj == INT64_NULL)? getTimeStamp() : getTimeStamp().addPicoSeconds(adj);
    }

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
    public: inline boolNull isCalibratedTimeStamp () const  { return getStateEventBit(31, 19); }

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
    public: inline boolNull isDataValid () const            { return getStateEventBit(30, 18); }

    /** Gets the reference lock indicator flag.
     *  <pre>
     *    true  = All of the phase-locked loops affecting the data are locked and stable
     *    false = At least one of the phase-locked loops affecting the data is not locked and stable
     *    null  = <i>This information is not available</i>
     *  </pre>
     *  @return true if the flag is set, false if it is not set, null if this optional flag isn't
     *          specified (null is returned for all context packets).
     */
    public: inline boolNull isReferenceLocked () const      { return getStateEventBit(29, 17); }

    /** Gets the AGC/MGC indicator flag.
     *  <pre>
     *    true  = AGC (automatic gain control) is being used
     *    false = MGC (manual gain control) is being used
     *    null  = <i>This information is not available</i>
     *  </pre>
     *  @return true if the flag is set, false if it is not set, null if this optional flag isn't
     *          specified (null is returned for all context packets).
     */
    public: inline boolNull isAutomaticGainControl () const { return getStateEventBit(28, 16); }

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
    public: inline boolNull isSignalDetected () const       { return getStateEventBit(27, 15); }

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
    public: inline boolNull isInvertedSpectrum () const     { return getStateEventBit(26, 14); }

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
    public: inline boolNull isOverRange () const            { return getStateEventBit(25, 13); }

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
    public: inline boolNull isDiscontinuous () const       { return getStateEventBit(24, 12); }
    
    /** <b>Deprecated:</b> Please change to using <tt>isDiscontinuous</tt>. */
    public: inline __attribute__((deprecated)) __intelattr__((deprecated)) boolNull isDiscontinuious () const { return isDiscontinuous(); }

    /** <i>Optional functionality:</i> Sets the calibrated time indicator flag.
     *  @param v true if the flag is set, false if not set, null if this optional flag isn't specified.
     *  @throws VRTException If this method is not supported or if called on a context packet.
     */
    public: inline void setCalibratedTimeStamp (boolNull v)  { setStateEventBit(31, 19, v); }

    /** <i>Optional functionality:</i> Gets the valid data indicator flag.
     *  @param v true if the flag is set, false if not set, null if this optional flag isn't specified.
     *  @throws VRTException If this method is not supported or if called on a context packet.
     */
    public: inline void setDataValid (boolNull v)            { setStateEventBit(30, 18, v); }

    /** <i>Optional functionality:</i> Gets the reference lock indicator flag.
     *  @param v true if the flag is set, false if not set, null if this optional flag isn't specified.
     *  @throws VRTException If this method is not supported or if called on a context packet.
     */
    public: inline void setReferenceLocked (boolNull v)      { setStateEventBit(29, 17, v); }

    /** <i>Optional functionality:</i> Gets the AGC/MGC indicator flag.
     *  @param v true if the flag is set, false if not set, null if this optional flag isn't specified.
     *  @throws VRTException If this method is not supported or if called on a context packet.
     */
    public: inline void setAutomaticGainControl (boolNull v) { setStateEventBit(28, 16, v); }

    /** <i>Optional functionality:</i> Gets the signal detected indicator flag.
     *  @param v true if the flag is set, false if not set, null if this optional flag isn't specified.
     *  @throws VRTException If this method is not supported or if called on a context packet.
     */
    public: inline void setSignalDetected (boolNull v)       { setStateEventBit(27, 15, v); }

    /** <i>Optional functionality:</i> Gets the spectral inversion indicator flag.
     *  @param v true if the flag is set, false if not set, null if this optional flag isn't specified.
     *  @throws VRTException If this method is not supported or if called on a context packet.
     */
    public: inline void setInvertedSpectrum (boolNull v)     { setStateEventBit(26, 14, v); }

    /** <i>Optional functionality:</i> Gets the over-range indicator flag.
     *  @param v true if the flag is set, false if not set, null if this optional flag isn't specified.
     *  @throws VRTException If this method is not supported or if called on a context packet.
     */
    public: inline void setOverRange (boolNull v)            { setStateEventBit(25, 13, v); }

    /** <i>Optional functionality:</i> Gets the sample loss indicator flag.
     *  @param v true if the flag is set, false if not set, null if this optional flag isn't specified.
     *  @throws VRTException If this method is not supported or if called on a context packet.
     */
    public: inline void setDiscontinuous (boolNull v)       { setStateEventBit(24, 12, v); }
    
    /** <b>Deprecated:</b> Please change to using <tt>setDiscontinuous</tt>. */
    public: inline __attribute__((deprecated)) __intelattr__((deprecated)) void setDiscontinuious (boolNull v) { setDiscontinuous(v); }

    /** Gets the Context Field Change Indicator.
     *  @return true if <b>anything</b> in the packet has changed since the last context packet,
     *          false if <b>nothing</b> has changed since the last context packet.
     */
    public: inline bool              isChangePacket ()              const { return ((getContextIndicatorField() & CHANGE_IND) != 0); }

    /** Gets the Reference Point Identifier.
     *  @return The point in the system where this context applies (null if not specified).
     */
    public: inline int32_t           getReferencePointIdentifier () const { return getL(REF_POINT); }

    /** Gets the Timestamp Adjustment in picoseconds. This is the required time adjustment
     *  between the time the signal was digitized (i.e. the timestamp) and the time at the reference
     *  point.
     *  @return The timestamp adjustment (null if not specified).
     */
    public: inline int64_t           getTimeStampAdjustment ()      const { return getX(TIME_ADJUST); }

    /** Gets the Timestamp Calibration Time in seconds. This is the most recent date and
     *  time when the timestamp in the Data and Context packets was known to be correct.
     *  @return The calibration time (null if not specified).
     */
    public: inline int32_t           getTimeStampCalibration ()     const { return getL(TIME_CALIB); }

    /** Gets the Ephemeris Reference Identifier. This is used specifies the process
     *  whose origin applies to the ephemeris returned by {@link #getEphemerisRelative()}.
     *  @return The ephemeris information (null if not specified). <i>Note that changes to the
     *          returned object do not alter the packet.</i>
     */
    public: inline int32_t           getEphemerisReference ()       const { return getL(EPHEM_REF); }

    private: inline Geolocation       getGeolocation (int32_t offset) const {
      int32_t     off = getOffset(offset);
      Geolocation val;
      if (off >= 0) unpackPayloadRecord(off,val);
      return val;
    }
    
    /** Gets the Formatted GPS (Global Positioning System) Geolocation for the collector.
     *  @return The geolocation information (null if not specified). <i>Note that changes to the
     *          returned object do not alter the packet.</i>
     */
    public: inline Geolocation       getGeolocationGPS ()           const { return getGeolocation(GPS_EPHEM); }

    /** Gets the Formatted INS (Inertial Navigation System) Geolocation for the collector.
     *  @return The geolocation information (null if not specified). <i>Note that changes to the
     *          returned object do not alter the packet.</i>
     */
    public: inline Geolocation       getGeolocationINS ()           const { return getGeolocation(INS_EPHEM); }

    /** Gets the ECEF (Earth-Centered, Earth-Fixed) Ephemeris for the collector. This will also
     *  get any "ECEF (Earth-Centered, Earth-Fixed) Ephemeris Adjunct" information from a
     *  VITA-49.0b packet (where VITA-49.0b is used).
     *  @return The ephemeris information (null if not specified). <i>Note that changes to the
     *          returned object do not alter the packet.</i>
     */
    public: virtual Ephemeris        getEphemerisECEF ()            const;

    /** Gets the Relative Ephemeris for the collector. Unlike {@link #getEphemerisECEF()}
     *  which uses the ECEF coordinate system, this ephemeris is relative to a user-defined system
     *  specified by {@link #getEphemerisReference()}. This will also get any "Relative Ephemeris
     *  Adjunct" information from a VITA-49.0b packet (where VITA-49.0b is used).
     *  @return The ephemeris information (null if not specified). <i>Note that changes to the
     *          returned object do not alter the packet.</i>
     */
    public: virtual Ephemeris        getEphemerisRelative ()        const;

    /** Gets the GPS ASCII "sentences". These are ASCII "sentences" from a GPS (Global
     *  Positioning System) receiver, such as those conforming to NMEA-0183. <i>Due to the great
     *  variety in GPS "sentences", this class makes little effort to do anything useful with them.
     *  Users are encouraged to use {@link #getGeolocationGPS()} which essentially provides the same
     *  information, but in a consistent form.</i>
     *  @return The geolocation information (null if not specified). <i>Note that changes to the
     *          returned object do not alter the packet.</i>
     */
    public: inline GeoSentences getGeoSentences () const {
      int32_t      off = getOffset(GPS_ASCII);
      GeoSentences val;
      if (off >= 0) unpackPayloadRecord(off,val);
      return val;
    }

    /** Gets the Context Association Lists. These lists indicate the other context/data
     *  streams associated with this one.
     *  @return The context association lists (null if not specified). <i>Note that changes to the
     *          returned object do not alter the packet.</i>
     */
    public: inline ContextAssocLists getContextAssocLists () const {
      int32_t           off = getOffset(CONTEXT_ASOC);
      ContextAssocLists val;
      if (off >= 0) unpackPayloadRecord(off,val);
      return val;
    }

    /** Gets the Device Identifier specifying the manufacturer and model of the device
     *  producing this context packet stream.
     *  @return The device identifier as a string (null if not specified).
     */
    public: inline string            getDeviceID ()                 const { return Utilities::toStringDeviceID(getDeviceIdentifier()); }

    /** Gets the Bandwidth of the signal in Hz.
     *  @return The bandwidth (null if not specified).
     */
    public: inline double            getBandwidth ()                const { int64_t bits = getX(BANDWIDTH    ); return (isNull(bits))? DOUBLE_NAN : VRTMath::toDouble64(20,bits); }

    /** Gets the IF Reference Frequency of the signal in Hz.
     *  @return The frequency (null if not specified).
     */
    public: inline double            getFrequencyIF ()              const { int64_t bits = getX(IF_FREQ      ); return (isNull(bits))? DOUBLE_NAN : VRTMath::toDouble64(20,bits); }

    /** Gets the RF Reference Frequency of the signal in Hz.
     *  @return The frequency (null if not specified).
     */
    public: inline double            getFrequencyRF ()              const { int64_t bits = getX(RF_FREQ      ); return (isNull(bits))? DOUBLE_NAN : VRTMath::toDouble64(20,bits); }

    /** Gets the RF Reference Frequency Offset of the signal in Hz.
     *  @return The frequency offset (null if not specified).
     */
    public: inline double            getFrequencyOffsetRF ()        const { int64_t bits = getX(RF_OFFSET    ); return (isNull(bits))? DOUBLE_NAN : VRTMath::toDouble64(20,bits); }

    /** Gets the IF Band Offset of the signal in Hz.
     *  @return The band offset (null if not specified).
     */
    public: inline double            getBandOffsetIF ()             const { int64_t bits = getX(IF_OFFSET    ); return (isNull(bits))? DOUBLE_NAN : VRTMath::toDouble64(20,bits); }

    /** Gets the Reference Level of the signal in dBm.
     *  @return The reference level (null if not specified).
     */
    public: inline float             getReferenceLevel ()           const { int16_t bits = getI(REF_LEVEL  ,2); return (isNull(bits))? FLOAT_NAN  : VRTMath::toFloat16 ( 7,bits); }

    /** Gets the Stage 1 Gain of the device in dB. This is the front-end gain of the
     *  system. In cases where a separate Stage 1 and Stage 2 gain is not necessary, this holds
     *  the total gain of the system and Gain 2 is set to zero.
     *  @return The gain (null if not specified).
     */
    public: inline float             getGain1 ()                    const { int16_t bits = getI(GAIN       ,0); return (isNull(bits))? FLOAT_NAN  : VRTMath::toFloat16 ( 7,bits); }

    /** Gets the Stage 2 Gain of the device in dB. This is the back-end gain of the
     *  system. In cases where a separate Stage 1 and Stage 2 gain is not necessary, the Stage 1
     *  gain holds the total gain of the system and this is set to zero.
     *  @return The gain (null if not specified).
     */
    public: inline float             getGain2 ()                    const { int16_t bits = getI(GAIN       ,2); return (isNull(bits))? FLOAT_NAN  : VRTMath::toFloat16 ( 7,bits); }

    /** Gets the Sample Rate in Hz.
     *  @return The sample rate (null if not specified).
     */
    public: inline double            getSampleRate ()               const { int64_t bits = getX(SAMPLE_RATE  ); return (isNull(bits))? DOUBLE_NAN : VRTMath::toDouble64(20,bits); }

    /** Gets the Sample Period (inverse of Sample Rate) in sec.
     *  @return The sample period (null if not specified).
     */
    public: inline double getSamplePeriod () const {
      double sr = getSampleRate();
      return (isNull(sr))? DOUBLE_NAN : 1.0/sr;
    }

    /** Gets the Temperature in degrees Celsius. This is used to convey the temperature of
     *  any component that may affect the described signal.
     *  @return The temperature (null if not specified).
     */
    public: inline float             getTemperature ()              const { int16_t bits = getI(TEMPERATURE,2); return (isNull(bits))? FLOAT_NAN  : VRTMath::toFloat16 ( 6,bits); }

    /** Gets the User-Defined Bits from the State and Event Indicator Bits. <i>The
     *  definition of these bits should be included in the documentation for the relevent packet
     *  class.</i> Unlike other bits of the State and Event Indicator Field, there is no concept
     *  of an undefined-value for user bits if the State and Event Indicator Field is present to
     *  support some other bits. As such, zero will be returned if the State and Event Indicator
     *  Field is absent.
     *  @return true if the flag is set, false if it is not set, zero if the State and Event
     *  Indicator Field is absent.
     */
    public: inline int32_t           getUserDefinedBits ()          const { int8_t  bits = getB(STATE_EVENT,3); return (isNull(bits))? INT32_NULL : ((int32_t)bits)&0x000000FF; }

    /** Gets the Data Packet Payload Format. This specifies the format of the data in the
     *  paired data packet stream.
     *  @return The payload format (null if not specified). <i>Note that changes to the returned
     *          object do not alter the packet.</i>
     */
    public: inline PayloadFormat getDataPayloadFormat (int32_t offset) const {
      // INT64_NULL (0x8000000000000000) is actually a valid PayloadFormat bit arrangement, so we
      // can't use the normal isNull(bits) check. This isn't an issue in Java where null is
      // not equal to a legal 64-bit integer value.
      return (getOffset(offset) < 0)? PayloadFormat((int64_t)-1) : PayloadFormat(getX(offset));
    }
    public: inline PayloadFormat     getDataPayloadFormat ()        const { return getDataPayloadFormat(DATA_FORMAT); }

    /** Sets the Context Field Change Indicator.
     *  @param change true if <b>anything</b> in the packet has changed since the last context packet,
     *                false if <b>nothing</b> has changed since the last context packet.
     */
    public: inline void setChangePacket (bool change)                       { setContextIndicatorFieldBit(CHANGE_IND, change); }

    /** Sets the Reference Point Identifier.
     *  @param val The point in the system where this context applies (null if not specified).
     */
    public: inline void setReferencePointIdentifier (int32_t val)           { setL(REF_POINT, val); }

    /** Sets the User-Defined Bits from the State and Event Indicator Bits. <i>The
     *  definition of these bits should be included in the documentation for the relevent packet
     *  class.</i> Unlike other bits of the State and Event Indicator Field, there is no concept
     *  of an undefined-value for user bits if the State and Event Indicator Field is present to
     *  support some other bits. As such, no "null value" is supported for the user-defined buts.
     *  @param val true if the flag is set, false if it is not set.
     */
    public: inline void setUserDefinedBits (int32_t val)                    { if ((getOffset(STATE_EVENT) > 0) || !isNull(val)) setB(STATE_EVENT,3,val); }

    /** Sets the Timestamp Adjustment in picoseconds. This is the required time adjustment
     *  between the time the signal was digitized (i.e. the timestamp) and the time at the reference
     *  point.
     *  @param val The timestamp adjustment (null if not specified).
     */
    public: inline void setTimeStampAdjustment (int64_t val)                { setX(TIME_ADJUST,val); }

    /** Sets the Timestamp Calibration Time in seconds. This is the most recent date and
     *  time when the timestamp in the Data and Context packets was known to be correct.
     *  @param val The calibration time (null if not specified).
     */
    public: inline void setTimeStampCalibration (int32_t val)               { setL(TIME_CALIB,val); }

    /** Sets the Ephemeris Reference Identifier. This is used specifies the process
     *  whose origin applies to the ephemeris returned by {@link #getEphemerisRelative()}.
     *  @param val The ephemeris information (null if not specified). <i>Note that changes to the
     *             returned object do not alter the packet.</i>
     */
    public: inline void setEphemerisReference (int32_t val)                { setL(EPHEM_REF,val); }

    /** Sets the Formatted GPS (Global Positioning System) Geolocation for the collector.
     *  @param val The geolocation information (null if not specified). <i>Note that changes to the
     *             returned object do not alter the packet.</i>
     */
    public: inline void setGeolocationGPS (const Geolocation &val)          { setRecord(GPS_EPHEM,val); }

    /** Sets the Formatted INS (Inertial Navigation System) Geolocation for the collector.
     *  @param val The geolocation information (null if not specified). <i>Note that changes to the
     *             returned object do not alter the packet.</i>
     */
    public: inline void setGeolocationINS (const Geolocation &val)          { setRecord(INS_EPHEM,val); }

    /** Sets the ECEF (Earth-Centered, Earth-Fixed) Ephemeris for the collector. This will also
     *  set any applicable "ECEF (Earth-Centered, Earth-Fixed) Ephemeris Adjunct" information in a
     *  VITA-49.0b packet (where VITA-49.0b is used). If all of the adjunct information is null it is
     *  omitted (i.e. nothing to report); if strict VITA-49.0 is used the adjunct information is also
     *  omitted (even if non-null) since strict VITA-49.0 does not permit it to be reported.
     *  @param val The ephemeris information (null if not specified). <i>Note that changes to the
     *             returned object do not alter the packet.</i>
     */
    public: virtual void setEphemerisECEF (const Ephemeris &val);

    /** Sets the Relative Ephemeris for the collector. Unlike {@link #getEphemerisECEF()}
     *  which uses the ECEF coordinate system, this ephemeris is relative to a user-defined system
     *  specified by {@link #getEphemerisReference()}. This will also set any applicable "Relative
     *  Ephemeris Adjunct" information in a VITA-49.0b packet (where VITA-49.0b is used). If all of
     *  the adjunct information is null it is omitted (i.e. nothing to report); if strict VITA-49.0
     *  is used the adjunct information is also omitted (even if non-null) since strict VITA-49.0
     *  does not permit it to be reported.
     *  @param val The ephemeris information (null if not specified). <i>Note that changes to the
     *             returned object do not alter the packet.</i>
     */
    public: virtual void setEphemerisRelative (const Ephemeris &val);

    /** Sets the GPS ASCII "sentences". These are ASCII "sentences" from a GPS (Global
     *  Positioning System) receiver, such as those conforming to NMEA-0183. <i>Due to the great
     *  variety in GPS "sentences", this class makes little effort to do anything useful with them.
     *  Users are encouraged to use {@link #getGeolocationGPS()} which essentially provides the same
     *  information, but in a consistent form.</i>
     *  @param val The geolocation information (null if not specified). <i>Note that changes to the
     *             returned object do not alter the packet.</i>
     */
    public: inline void setGeoSentences (const GeoSentences &val)           { setRecord(GPS_ASCII, val, getGeoSentences()); }

    /** Sets the Context Association Lists. These lists indicate the other context/data
     *  streams associated with this one.
     *  @param val The context association lists (null if not specified). <i>Note that changes to the
     *          returned object do not alter the packet.</i>
     */
    public: inline void setContextAssocLists (const ContextAssocLists &val) { setRecord(CONTEXT_ASOC, val, getContextAssocLists()); }

    /** Sets the Device Identifier specifying the manufacturer and model of the device
     *  producing this context packet stream.
     *  @param val The device identifier as a string (null if not specified).
     */
    public: inline void setDeviceID (const string &val)                     { setDeviceIdentifier(Utilities::fromStringDeviceID(val)); }

    /** Sets the Bandwidth of the signal in Hz.
     *  @param val The bandwidth (null if not specified).
     */
    public: inline void setBandwidth (double val)                           { int64_t bits = (isNull(val))? INT64_NULL : VRTMath::fromDouble64(20,val); setX(BANDWIDTH    ,bits); }

    /** Sets the IF Reference Frequency of the signal in Hz.
     *  @param val The frequency (null if not specified).
     */
    public: inline void setFrequencyIF (double val)                         { int64_t bits = (isNull(val))? INT64_NULL : VRTMath::fromDouble64(20,val); setX(IF_FREQ      ,bits); }

    /** Sets the RF Reference Frequency of the signal in Hz.
     *  @param val The frequency (null if not specified).
     */
    public: inline void setFrequencyRF (double val)                         { int64_t bits = (isNull(val))? INT64_NULL : VRTMath::fromDouble64(20,val); setX(RF_FREQ      ,bits); }

    /** Sets the RF Reference Frequency Offset of the signal in Hz.
     *  @param val The frequency offset (null if not specified).
     */
    public: inline void setFrequencyOffsetRF (double val)                   { int64_t bits = (isNull(val))? INT64_NULL : VRTMath::fromDouble64(20,val); setX(RF_OFFSET    ,bits); }

    /** Sets the IF Band Offset of the signal in Hz.
     *  @param val The band offset (null if not specified).
     */
    public: inline void setBandOffsetIF (double val)                        { int64_t bits = (isNull(val))? INT64_NULL : VRTMath::fromDouble64(20,val); setX(IF_OFFSET    ,bits); }

    /** Sets the Reference Level of the signal in dBm.
     *  @param val The reference level (null if not specified).
     */
    public: inline void setReferenceLevel (float val)                       { int16_t bits = (isNull(val))? INT16_NULL : VRTMath::fromFloat16 ( 7,val); setI(REF_LEVEL  ,2,bits); }

    /** Sets the Stage 1 Gain of the device in dB. This is the front-end gain of the
     *  system. In cases where a separate Stage 1 and Stage 2 gain is not necessary, this holds
     *  the total gain of the system and Gain 2 is set to zero. <i>If no stage 2 gain has been
     *  specified, it will be set to 0 following the call to this method.</i>
     *  @param val The gain (null if not specified).
     */
    public: inline void setGain1 (float val)                                { int16_t bits =                             VRTMath::fromFloat16 ( 7,val); setI(GAIN       ,0,bits); }

    /** Sets the Stage 2 Gain of the device in dB. This is the back-end gain of the
     *  system. In cases where a separate Stage 1 and Stage 2 gain is not necessary, the Stage 1
     *  gain holds the total gain of the system and this is set to zero. <i>If no stage 1 gain
     *  has been specified, it will be set to 0 following the call to this method.</i>
     *  @param val The gain (null if not specified).
     */
    public: inline void setGain2 (float val)                                { int16_t bits =                             VRTMath::fromFloat16 ( 7,val); setI(GAIN       ,2,bits); }

    /** Sets the Sample Rate in Hz.
     *  @param val The sample rate (null if not specified).
     */
    public: inline void setSampleRate (double val)                          { int64_t bits = (isNull(val))? INT64_NULL : VRTMath::fromDouble64(20,val); setX(SAMPLE_RATE  ,bits); }

    /** Sets the Sample Period (inverse of Sample Rate) in sec.
     *  @param val The sample period (null if not specified).
     */
    public: inline void setSamplePeriod (double val)  {
      if (isNull(val)) setSampleRate(val);
      else             setSampleRate(1.0 / val);
    }

    /** Sets the Temperature in degrees Celsius. This is used to convey the temperature of
     *  any component that may affect the described signal.
     *  @param val The temperature (null if not specified).
     */
    public: inline void setTemperature (float val)                          { int16_t bits = (isNull(val))? INT16_NULL : VRTMath::fromFloat16 ( 6,val); setI(TEMPERATURE,2,bits); }

    /** Sets the Data Packet Payload Format. This specifies the format of the data in the
     *  paired data packet stream.
     *  @param val The payload format (null if not specified). <i>Note that changes to the returned
     *             object do not alter the packet.</i>
     */
    public: inline void setDataPayloadFormat (const PayloadFormat &val)     { int64_t bits = (isNull(val))? INT64_NULL : val.getBits();                 setX(DATA_FORMAT  ,bits); }

    /** Gets the Total Gain of the device in dB. The total gain is the sum of
     *  {@link #getGain1()} and {@link #getGain2()}.
     *  @return The gain (null if not specified).
     */
    public: inline float getGain () const {
      float gain1 = getGain1();
      float gain2 = getGain2();
      return (isNull(gain1))? FLOAT_NAN : gain1+gain2;
    }

    /** Sets the Total Gain of the device in dB. This is effectively the same as
     *  <tt>setGain(val,0)</tt> except that <tt>setGain(null)</tt> will mark both gain values
     *  as unspecified. <i>(Note that it is not possible to specify one of the gain values and
     *  leave the other as unspecified.)</i>
     *  @param val The total gain (null if not specified).
     */
    public: inline void setGain (float val) {
      if (isNull(val)) setI(GAIN,0,INT32_NULL); // clears gain field(s)
      else             setGain(val,0.0f);       // sets gain field(s)
    }

    /** Sets the Total Gain of the device in dB.
     *  @param gain1 The stage 1 gain.
     *  @param gain2 The stage 2 gain.
     */
    public: inline void setGain (float gain1, float gain2) {
      setGain1(gain1);
      setGain2(gain2);
    }

    /** Gets the Over-Range Count. This is the count of the number of over-range data
     *  samples in the <b>single</b> paired data packet.
     *  @return The count (null if not specified).
     */
    public: inline int64_t getOverRangeCount () const {
      int32_t bits = getL(OVER_RANGE);
      return (isNull(bits))? INT64_NULL : (bits & __INT64_C(0xFFFFFFFF));
    }

    /** Sets the Over-Range Count. This is the count of the number of over-range data
     *  samples in the <b>single</b> paired data packet.
     *  @param val The count (null if not specified).
     */
    public: inline void setOverRangeCount (int64_t val) {
      if (isNull(val)) setL(OVER_RANGE, INT32_NULL);
      else             setL(OVER_RANGE, (int32_t)val);
    }

    /** Gets the Device Identifier specifying the manufacturer and model of the device
     *  producing this context packet stream.
     *  @return The device identifier (null if not specified).
     */
    public: inline int64_t getDeviceIdentifier () const {
      int64_t bits = getX(DEVICE_ID);
      return (isNull(bits))? INT64_NULL : (bits & __INT64_C(0x00FFFFFF0000FFFF));
    }

    /** Sets the Device Identifier specifying the manufacturer and model of the device
     *  producing this context packet stream.
     *  @param val The device identifier (null if not specified).
     */
    public: inline void setDeviceIdentifier (int64_t val) {
      if (!isNull(val) && ((val & ~__INT64_C(0x00FFFFFF0000FFFF)) != 0)) {
        throw VRTException("Invalid device identifier");
      }
      setX(DEVICE_ID,val);
    }

    /** Unpacks a 16-bit integer from the payload at the indicated position. */
    protected: virtual int8_t getB (int32_t bit, int32_t xoff) const;

    /** Packs a 16-bit integer from the payload at the indicated position. */
    private: void setB (int32_t bit, int32_t xoff, int8_t val);

    /** Unpacks a 16-bit integer from the payload at the indicated position. */
    protected: virtual int16_t getI (int32_t bit, int32_t xoff) const;

    /** Packs a 16-bit integer from the payload at the indicated position. */
    private: void setI (int32_t bit, int32_t xoff, int16_t val);

    /** Unpacks a 32-bit integer from the payload at the indicated position. */
    protected: virtual int32_t getL (int32_t bit) const;

    /** Unpacks a 32-bit integer from the payload at the indicated position. */
    private: virtual int32_t getL24 (int32_t bit, int32_t offset) const;

    /** Packs a 32-bit integer from the payload at the indicated position. */
    private: void setL (int32_t bit, int32_t val);

    /** Unpacks a 64-bit integer from the payload at the indicated position. */
    protected: virtual int64_t getX (int32_t bit) const;

    /** Packs a 64-bit integer from the payload at the indicated position. */
    private: void setX (int32_t bit, int64_t val);

    /** Gets a block of data. */
    private: inline void setRecord (int32_t bit, Record val, Record old) {
      if (isNull(old)) setRecord(bit, val, -1);
      else             setRecord(bit, val, old.getByteLength());
    }

    /** Gets a block of data. */
    private: inline void setRecord (int32_t bit, Record val) {
      setRecord(bit, val, getFieldLen(bit));
    }
  
    /** Gets a block of data. */
    private: void setRecord (int32_t bit, Record val, int32_t oldLen);

    /** Gets the header type, used with getOffset(..) and OFFSET_TABLE. */
    private: int32_t getContextIndicatorField () const {
      // Replaced the "trivial" version (immediately below) with a hand-in-lined
      // version of the getL(..) call since we know that within getL(..) the
      // getOffset(CONTEXT_IND) call will always return 0.
      //   return (getL(CONTEXT_IND) & 0xFFFFFF00);
      int32_t val = VRTMath::unpackInt(bbuf, getHeaderLength());
      return (val & 0xFFFFFFC0);
    }

    /** Sets the header type, used with getOffset(..) and OFFSET_TABLE. */
    private: void setContextIndicatorFieldBit (int32_t bit, bool set);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Implement HasFields
    //////////////////////////////////////////////////////////////////////////////////////////////////
    public: virtual int32_t   getFieldCount () const;
    public: virtual string    getFieldName  (int32_t id) const;
    public: virtual ValueType getFieldType  (int32_t id) const;
    public: virtual Value*    getField      (int32_t id) const;
    public: virtual void      setField      (int32_t id, const Value* val);

  };
};
#endif /* _BasicContextPacket_h */

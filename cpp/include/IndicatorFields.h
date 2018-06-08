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
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 * ============================================================
 */

#ifndef _IndicatorFields_h
#define _IndicatorFields_h

#include "VRTObject.h"
#include "VRTConfig.h"
#include "VRTMath.h"
#include "PayloadFormat.h"
#include <map>
#include <iomanip>
#if NOT_USING_JNI
# include "MetadataBlock.h"
# include "Utilities.h"
# include "TimeStamp.h"
# include "Record.h"
# include "InetAddress.h"
# include "UUID.h"
# include "HasFields.h"
# include "Record.h"
#endif /* NOT_USING_JNI */

using namespace std;

namespace vrt {
  /** Context/Control Indicator Fields and functions to retrieve CIF number and
   * field bitmask from the enumerated value, as well as the reverse.
   */
  namespace IndicatorFields {
    /** Enumeration of all Context/Control Indicator Fields. The enumerated
     *  value can be programatically converted to the CIF# and bitmask. The most
     *  significant 3 bits (left-most) are the CIF#, and the least significant
     *  5 bits are the number of left shifts to apply to 0x1 to get the one-hot
     *  bitmask for the field. As such, a right-shift of the enumerated value
     *  (which is mathematically equivalent to a divide by 32) provides the
     *  CIF#, and a bitwise AND of the enumerated value with 0x1F (which is
     *  mathematically equivalent to a modulo 32) provides the number of bit
     *  shifts needed to produce the bitmask. The comments for each enumeration
     *  present this information as well in the following form: <br>
     *  <b> \<Field Key\> \<Bit #\> \<Description of field\> (\<CIF#\>,\<Bitmask\>) \<==\> \<CIF# bits\> \<Shift# bits\> = \<Enumerated Value\> </b><br>
     *  Note that there are gaps in the enumrated values that are reserved for
     *  potential future CIFs 4, 5, and 6.
     */
    enum IndicatorFieldEnum {
      CIF_NULL = -1,         ///< NULL
      // CIF0 - Legacy Fields and CIF Enables - Starts at 0*32 = 0
      // CIF Key                  Bit #   Description                           (CIF,Bitmask)  <==> CIF|Shift = Enum Value
      CIF0_RESERVED_0 = 0,   ///< Bit 0   Reserved                              (0,0x00000001) <==> 000 00000 =   0
      CIF1_ENABLE,           ///< Bit 1   CIF1 Enable                           (0,0x00000002) <==> 000 00001 =   1
      CIF2_ENABLE,           ///< Bit 2   CIF2 Enable                           (0,0x00000004) <==> 000 00010 =   2
      CIF3_ENABLE,           ///< Bit 3   CIF3 Enable                           (0,0x00000008) <==> 000 00011 =   3
      CIF0_RESERVED_4,       ///< Bit 4   Reserved                              (0,0x00000010) <==> 000 00100 =   4
      CIF0_RESERVED_5,       ///< Bit 5   Reserved                              (0,0x00000020) <==> 000 00101 =   5
      CIF0_RESERVED_6,       ///< Bit 6   Reserved                              (0,0x00000040) <==> 000 00110 =   6
      CIF7_ENABLE,           ///< Bit 7   Field Attributes Enable               (0,0x00000080) <==> 000 00111 =   7
      CONTEXT_ASOC,          ///< Bit 8   Context Association Lists             (0,0x00000100) <==> 000 01000 =   8
      GPS_ASCII,             ///< Bit 9   GPS ASCII                             (0,0x00000200) <==> 000 01001 =   9
      EPHEM_REF,             ///< Bit 10  Ephemeris Ref ID                      (0,0x00000400) <==> 000 01010 =  10
      REL_EPHEM,             ///< Bit 11  Relative Ephemeris                    (0,0x00000800) <==> 000 01011 =  11
      ECEF_EPHEM,            ///< Bit 12  ECEF Ephemeris                        (0,0x00001000) <==> 000 01100 =  12
      INS_EPHEM,             ///< Bit 13  Formatted INS                         (0,0x00002000) <==> 000 01101 =  13
      GPS_EPHEM,             ///< Bit 14  Formatted GPS                         (0,0x00004000) <==> 000 01110 =  14
      DATA_FORMAT,           ///< Bit 15  Signal Data Packet Payload Format     (0,0x00008000) <==> 000 01111 =  15
      STATE_EVENT,           ///< Bit 16  State/Event Indicators                (0,0x00010000) <==> 000 10000 =  16
      DEVICE_ID,             ///< Bit 17  Device Identifier                     (0,0x00020000) <==> 000 10001 =  17
      TEMPERATURE,           ///< Bit 18  Temperature                           (0,0x00040000) <==> 000 10010 =  18
      TIME_CALIB,            ///< Bit 19  Timestamp Calibration Time            (0,0x00080000) <==> 000 10011 =  19
      TIME_ADJUST,           ///< Bit 20  Timestamp Adjustment                  (0,0x00100000) <==> 000 10100 =  20
      SAMPLE_RATE,           ///< Bit 21  Sample Rate                           (0,0x00200000) <==> 000 10101 =  21
      OVER_RANGE,            ///< Bit 22  Over-range Count                      (0,0x00400000) <==> 000 10110 =  22
      GAIN,                  ///< Bit 23  Gain                                  (0,0x00800000) <==> 000 10111 =  23
      REF_LEVEL,             ///< Bit 24  Reference Level                       (0,0x01000000) <==> 000 11000 =  24
      IF_OFFSET,             ///< Bit 25  IF Band Offset                        (0,0x02000000) <==> 000 11001 =  25
      RF_OFFSET,             ///< Bit 26  RF Reference Frequency Offset         (0,0x04000000) <==> 000 11010 =  26
      RF_FREQ,               ///< Bit 27  RF Reference Frequency                (0,0x08000000) <==> 000 11011 =  27
      IF_FREQ,               ///< Bit 28  IF Reference Frequency                (0,0x10000000) <==> 000 11100 =  28
      BANDWIDTH,             ///< Bit 29  Bandwidth                             (0,0x20000000) <==> 000 11101 =  29
      REF_POINT,             ///< Bit 30  Reference Point Indentifier           (0,0x40000000) <==> 000 11110 =  30
      CHANGE_IND,            ///< Bit 31  Context Field Change Indicator        (0,0x80000000) <==> 000 11111 =  31

      // CIF1 - Spatial, Signal, Spectral, I/O, Control - Starts at 1*32 = 32
      CIF1_RESERVED_0 = 32,  ///< Bit 0   Reserved                              (1,0x00000001) <==> 001 00000 =  32
      BUFFER_SZ,             ///< Bit 1   Buffer Size                           (1,0x00000002) <==> 001 00001 =  33
      VER_BLD_CODE,          ///< Bit 2   Version and Build Code                (1,0x00000004) <==> ...
      V49_COMPL,             ///< Bit 3   V49 Spec Compliance                   (1,0x00000008)
      HEALTH_STATUS,         ///< Bit 4   Health Status                         (1,0x00000010)
      DISCRETE_IO64,         ///< Bit 5   Distrete I/O (64-bit)                 (1,0x00000020)
      DISCRETE_IO32,         ///< Bit 6   Discrete I/O (32-bit)                 (1,0x00000040)
      INDEX_LIST,            ///< Bit 7   Index List                            (1,0x00000080)
      CIF1_RESERVED_8,       ///< Bit 8   Reserved                              (1,0x00000100)
      SECTOR_SCN_STP,        ///< Bit 9   Sector Scan/Step                      (1,0x00000200)
      SPECTRUM,              ///< Bit 10  Spectrum                              (1,0x00000400)
      CIFS_ARRAY,            ///< Bit 11  Array of CIFS                         (1,0x00000800)
      CIF1_RESERVED_12,      ///< Bit 12  Reserved                              (1,0x00001000)
      AUX_BANDWIDTH,         ///< Bit 13  Aux Bandwidth                         (1,0x00002000)
      AUX_GAIN,              ///< Bit 14  Aux Gain                              (1,0x00004000)
      AUX_FREQUENCY,         ///< Bit 15  Aux Frequency                         (1,0x00008000)
      SNR_NOISE,             ///< Bit 16  SNR/Noise Figure                      (1,0x00010000)
      ICPT_PTS_2_3,          ///< Bit 17  2nd and 3rd-Order Intercept Points    (1,0x00020000)
      COMPRESS_PT,           ///< Bit 18  Compression Point                     (1,0x00040000)
      THRESHOLD,             ///< Bit 19  Threshold                             (1,0x00080000)
      EB_NO_BER,             ///< Bit 20  Eb/No BER                             (1,0x00100000)
      CIF1_RESERVED_21,      ///< Bit 21  Reserved                              (1,0x00200000)
      CIF1_RESERVED_22,      ///< Bit 22  Reserved                              (1,0x00400000)
      CIF1_RESERVED_23,      ///< Bit 23  Reserved                              (1,0x00800000)
      RANGE,                 ///< Bit 24  Range (Distance)                      (1,0x01000000)
      BEAMWIDTH,             ///< Bit 25  Beamwidth                             (1,0x02000000)
      SPATIAL_REF_TYPE,      ///< Bit 26  Spatial Reference Type                (1,0x04000000)
      SPATIAL_SCAN_TYPE,     ///< Bit 27  Spatial Scan Type                     (1,0x08000000)
      PNT_VECT_3D_ST,        ///< Bit 28  2-D Pointing Angle (structured)       (1,0x10000000)
      PNT_VECT_3D_SI,        ///< Bit 29  2-D Pointing Angle (simple)           (1,0x20000000)
      POLARIZATION,          ///< Bit 30  Polarization                          (1,0x40000000)
      PHASE,                 ///< Bit 31  Phase                                 (1,0x80000000)
      
      // CIF2 - Identifiers (tags) - Starts at 2*32 = 64
      CIF2_RESERVED_0 = 64,  ///< Bit 0   Reserved                              (2,0x00000001) <==> 010 00000 =  64
      CIF2_RESERVED_1,       ///< Bit 1   Reserved                              (2,0x00000002)
      CIF2_RESERVED_2,       ///< Bit 2   Reserved                              (2,0x00000004)
      RF_FOOTPRINT_RANGE,    ///< Bit 3   RF Footprint Range                    (2,0x00000008)
      RF_FOOTPRINT,          ///< Bit 4   RF Footprint                          (2,0x00000010)
      COMM_PRIORITY_ID,      ///< Bit 5   Communication Priority ID             (2,0x00000020)
      FUNCT_PRIORITY_ID,     ///< Bit 6   Function Priority ID                  (2,0x00000040)
      EVENT_ID,              ///< Bit 7   Event ID                              (2,0x00000080)
      MODE_ID,               ///< Bit 8   Mode ID                               (2,0x00000100) <==> 010 01000 = 72
      FUNCTION_ID,           ///< Bit 9   Function ID                           (2,0x00000200)
      MODULATION_TYPE,       ///< Bit 10  Modulation Type                       (2,0x00000400)
      MODULATION_CLASS,      ///< Bit 11  Modulation Class                      (2,0x00000800)
      EMS_DEVICE_INSTANCE,   ///< Bit 12  EMS Device Instance                   (2,0x00001000)
      EMS_DEVICE_TYPE,       ///< Bit 13  EMS Device Type                       (2,0x00002000)
      EMS_DEVICE_CLASS,      ///< Bit 14  EMS Device Class                      (2,0x00004000)
      PLATFORM_DISPLAY,      ///< Bit 15  Platform Display                      (2,0x00008000)
      PLATFORM_INSTANCE,     ///< Bit 16  Platform Instance                     (2,0x00010000)
      PLATFORM_CLASS,        ///< Bit 17  Platform Class                        (2,0x00020000)
      OPERATOR,              ///< Bit 18  Operator                              (2,0x00040000)
      COUNTRY_CODE,          ///< Bit 19  Country Code                          (2,0x00080000)
      TRACK_ID,              ///< Bit 20  Track ID                              (2,0x00100000)
      INFORMATION_SOURCE,    ///< Bit 21  Information Source                    (2,0x00200000)
      CONTROLLER_UUID,       ///< Bit 22  Controller UUID                       (2,0x00400000)
      CONTROLLER_ID,         ///< Bit 23  Controller ID                         (2,0x00800000)
      CONTROLLEE_UUID,       ///< Bit 24  Controllee UUID                       (2,0x01000000)
      CONTROLLEE_ID,         ///< Bit 25  Controllee ID                         (2,0x02000000)
      CITED_MESSAGE_ID,      ///< Bit 26  Cited Message ID                      (2,0x04000000)
      CHILDREN_SID,          ///< Bit 27  Child(ren) SID                        (2,0x08000000)
      PARENTS_SID,           ///< Bit 28  Parent(s) SID                         (2,0x10000000)
      SIBLINGS_SID,          ///< Bit 29  Sibling(s) SID                        (2,0x20000000)
      CITED_SID,             ///< Bit 30  Cited SID                             (2,0x40000000)
      BIND,                  ///< Bit 31  Bind                                  (2,0x80000000)
      
      // CIF3 - Temporal, Environmental - Starts at 3*32 = 96
      CIF3_RESERVED_0 = 96,  ///< Bit 0   Reserved                              (3,0x00000001) <==> 011 00000 =  96
      NETWORK_ID,            ///< Bit 1   Network ID                            (3,0x00000002)
      TROPOSPHERIC_STATE,    ///< Bit 2   Tropospheric State                    (3,0x00000004)
      SEA_AND_SWELL_STATE,   ///< Bit 3   Sea and Swell State                   (3,0x00000008)
      BAROMETRIC_PRESSURE,   ///< Bit 4   Barometric Pressure                   (3,0x00000010)
      HUMIDITY,              ///< Bit 5   Humidity                              (3,0x00000020)
      SEA_GROUND_TEMP,       ///< Bit 6   Sea/Ground Temperature                (3,0x00000040)
      AIR_TEMP,              ///< Bit 7   Air Temperature                       (3,0x00000080)
      CIF3_RESERVED_8,       ///< Bit 8   Reserved                              (3,0x00000100)
      CIF3_RESERVED_9,       ///< Bit 9   Reserved                              (3,0x00000200)
      CIF3_RESERVED_10,      ///< Bit 10  Reserved                              (3,0x00000400)
      CIF3_RESERVED_11,      ///< Bit 11  Reserved                              (3,0x00000800)
      CIF3_RESERVED_12,      ///< Bit 12  Reserved                              (3,0x00001000)
      CIF3_RESERVED_13,      ///< Bit 13  Reserved                              (3,0x00002000)
      CIF3_RESERVED_14,      ///< Bit 14  Reserved                              (3,0x00004000)
      CIF3_RESERVED_15,      ///< Bit 15  Reserved                              (3,0x00008000)
      SHELF_LIFE,            ///< Bit 16  Shelf Life                            (3,0x00010000)
      AGE,                   ///< Bit 17  Age                                   (3,0x00020000)
      CIF3_RESERVED_18,      ///< Bit 18  Reserved                              (3,0x00040000)
      CIF3_RESERVED_19,      ///< Bit 19  Reserved                              (3,0x00080000)
      JITTER,                ///< Bit 20  Jitter                                (3,0x00100000)
      DWELL,                 ///< Bit 21  Dwell                                 (3,0x00200000)
      DURATION,              ///< Bit 22  Duration                              (3,0x00400000)
      PERIOD,                ///< Bit 23  Period                                (3,0x00800000)
      PULSE_WIDTH,           ///< Bit 24  Pulse Width                           (3,0x01000000)
      OFFSET_TIME,           ///< Bit 25  Offset Time                           (3,0x02000000)
      FALL_TIME,             ///< Bit 26  Fall Time                             (3,0x04000000)
      RISE_TIME,             ///< Bit 27  Rise Time                             (3,0x08000000)
      CIF3_RESERVED_28,      ///< Bit 28  Reserved                              (3,0x10000000)
      CIF3_RESERVED_29,      ///< Bit 29  Reserved                              (3,0x20000000)
      TIMESTAMP_SKEW,        ///< Bit 30  Timestamp Skew                        (3,0x40000000)
      TIMESTAMP_DETAILS,     ///< Bit 31  Timestamp Details                     (3,0x80000000) <==> 011 11111 = 127
      
      // CIF4 - Reserved - Starts at 4*32 = 128
      CIF4_RESERVED_0 = 128, ///< Bit 0   Reserved                              (4,0x00000001) <==> 100 00000 = 128
      /* Bits 1-31 reserved/undefined */
      
      // CIF5 - Reserved - Starts at 5*32 = 160
      CIF5_RESERVED_0 = 160, ///< Bit 0   Reserved                              (5,0x00000001) <==> 101 00000 = 160
      /* Bits 1-31 reserved/undefined */

      // CIF6 - Reserved - Starts at 6*32 = 192
      CIF6_RESERVED_0 = 192, ///< Bit 0   Reserved                              (6,0x00000001) <==> 110 00000 = 192
      /* Bits 1-31 reserved/undefined */

      // CIF7 - Attributes - Starts at 7*32 = 224
      CIF7_RESERVED_0 = 224, ///< Bit 0   Reserved                              (7,0x00000001) <==> 111 00000 = 224
      CIF7_RESERVED_1,       ///< Bit 1   Reserved                              (7,0x00000002)
      CIF7_RESERVED_2,       ///< Bit 2   Reserved                              (7,0x00000004)
      CIF7_RESERVED_3,       ///< Bit 3   Reserved                              (7,0x00000008)
      CIF7_RESERVED_4,       ///< Bit 4   Reserved                              (7,0x00000010)
      CIF7_RESERVED_5,       ///< Bit 5   Reserved                              (7,0x00000020)
      CIF7_RESERVED_6,       ///< Bit 6   Reserved                              (7,0x00000040)
      CIF7_RESERVED_7,       ///< Bit 7   Reserved                              (7,0x00000080)
      CIF7_RESERVED_8,       ///< Bit 8   Reserved                              (7,0x00000100)
      CIF7_RESERVED_9,       ///< Bit 9   Reserved                              (7,0x00000200)
      CIF7_RESERVED_10,      ///< Bit 10  Reserved                              (7,0x00000400)
      CIF7_RESERVED_11,      ///< Bit 11  Reserved                              (7,0x00000800)
      CIF7_RESERVED_12,      ///< Bit 12  Reserved                              (7,0x00001000)
      CIF7_RESERVED_13,      ///< Bit 13  Reserved                              (7,0x00002000)
      CIF7_RESERVED_14,      ///< Bit 14  Reserved                              (7,0x00004000)
      CIF7_RESERVED_15,      ///< Bit 15  Reserved                              (7,0x00008000)
      CIF7_RESERVED_16,      ///< Bit 16  Reserved                              (7,0x00010000)
      CIF7_RESERVED_17,      ///< Bit 17  Reserved                              (7,0x00020000)
      CIF7_RESERVED_18,      ///< Bit 18  Reserved                              (7,0x00040000)
      BELIEF,                ///< Bit 19  Belief                                (7,0x00080000)
      PROBABILITY,           ///< Bit 20  Probability                           (7,0x00100000)
      THIRD_DERIVATIVE,      ///< Bit 21  3rd Derivative                        (7,0x00200000)
      SECOND_DERIVATIVE,     ///< Bit 22  2nd Derivative (Accelleration)        (7,0x00400000)
      FIRST_DERIVATIVE,      ///< Bit 23  1st Derivative (Velocity)             (7,0x00800000)
      ACCURACY,              ///< Bit 24  Accuracy                              (7,0x01000000)
      PRECISION,             ///< Bit 25  Precision                             (7,0x02000000)
      MIN_VALUE,             ///< Bit 26  Min Value                             (7,0x04000000)
      MAX_VALUE,             ///< Bit 27  Max Value                             (7,0x08000000)
      STANDARD_DEVIATION,    ///< Bit 28  Standard Deviation                    (7,0x10000000)
      MEDIAN_VALUE,          ///< Bit 29  Median Value                          (7,0x20000000)
      AVERAGE_VALUE,         ///< Bit 30  Average Value                         (7,0x40000000)
      CURRENT_VALUE          ///< Bit 31  Current Value                         (7,0x80000000) <==> 111 11111 = 255

    }; // IndicatorFieldEnum_t

    /** Typedef of enumeration of all Context/Control Indicator Fields. */
    typedef IndicatorFieldEnum IndicatorFieldEnum_t;

    /** Get the CIF number from the IndicatorFieldEnum_t
     * The top three (most-significant) bits of each 8-bit IndicatorFieldEnum_t are the CIF number
     * The bottom five (least-significant) bits of each 8-bit IndicatorFieldEnum_t are the bit number
     * CIF#: 0b11100000 = 0xE0 | Right-shift by 5, i.e. divide by 32
     * Bit#: 0b00011111 = 0x1F | Bitwise AND this bit-mask, i.e. modulo 32
     * @param f Field of interest
     * @return CIF number
     */
    inline int8_t getCIFNumber (IndicatorFieldEnum_t f) {
      return (int8_t) ((f >> 5) & 0xFF); // divide by 32
    }

    /** Get the CIF bit number from the IndicatorFieldEnum_t
     * The top three (most-significant) bits of each 8-bit IndicatorFieldEnum_t are the CIF number
     * The bottom five (least-significant) bits of each 8-bit IndicatorFieldEnum_t are the bit number
     * CIF#: 0b11100000 = 0xE0 | Right-shift by 5, i.e. divide by 32
     * Bit#: 0b00011111 = 0x1F | Bitwise AND this bit-mask, i.e. modulo 32
     * @param f Field of interest
     * @return CIF bit number
     */
    inline int8_t getCIFBitNumber (IndicatorFieldEnum_t f) {
      return (int8_t) (f & 0x1F); // modulo 32
    }

    /** Get the 1-hot bit mask from IndicatorFieldEnum_t
     * The top three (most-significant) bits of each 8-bit IndicatorFieldEnum_t are the CIF number
     * The bottom five (least-significant) bits of each 8-bit IndicatorFieldEnum_t are the bit number
     * CIF#: 0b11100000 = 0xE0 | Right-shift by 5, i.e. divide by 32
     * Bit#: 0b00011111 = 0x1F | Bitwise AND this bit-mask, i.e. modulo 32
     * @param f Field of interest
     * @return 1-hot CIF bit-mask
     */
    inline int32_t getCIFBitMask (IndicatorFieldEnum_t f) {
      return (int32_t) (0x1 << getCIFBitNumber(f)); // 1-hot bit mask for indicator field
    }
    
    /** Get the IndicatorFieldEnum from the CIF number and CIF bit number
     * The top three (most-significant) bits of each 8-bit IndicatorFieldEnum_t are the CIF number
     * The bottom five (least-significant) bits of each 8-bit IndicatorFieldEnum_t are the bit number
     * CIF#: 0b11100000 = 0xE0 | Left-shift by 5, i.e. multiply by 32
     * Bit#: 0b00011111 = 0x1F | Bitwise AND this bit-mask, i.e. modulo 32
     * Add the shifted CIF number to the bit number for the IndicatorFieldEnum value.
     * @param cif CIF number
     * @param bit Bit number
     * @return IndicatorFieldEnum value corresponding with cif and bit.
     */
    inline IndicatorFieldEnum_t getCIFEnum(int8_t cif, int8_t bit) {
      return (IndicatorFieldEnum_t) (((cif & 0x7) << 5) + (bit & 0x1F));
    }
  
    /** <b>Internal Use Only:</b> Bitmasks for CIF0 fields (Legacy Fields and
     *  CIF Enables).
     */
    namespace protected_CIF0 {

      /** CIF0 Bitmasks
       *  Bitmask:                                                     Field Size (# of 32-bit words)
       */
      static const int32_t CIF0_RESERVED_0_mask       = 0x00000001; // N/A (0)
      static const int32_t CIF1_ENABLE_mask           = 0x00000002; // 1
      static const int32_t CIF2_ENABLE_mask           = 0x00000004; // 1
      static const int32_t CIF3_ENABLE_mask           = 0x00000008; // 1
      static const int32_t CIF0_RESERVED_4_mask       = 0x00000010; // N/A (0)
      static const int32_t CIF0_RESERVED_5_mask       = 0x00000020; // N/A (0)
      static const int32_t CIF0_RESERVED_6_mask       = 0x00000040; // N/A (0)
      static const int32_t CIF7_ENABLE_mask           = 0x00000080; // 1
      static const int32_t CONTEXT_ASOC_mask          = 0x00000100; // variable (See V49.2 spec Section 9.13.2)
      static const int32_t GPS_ASCII_mask             = 0x00000200; // variable (See V49.2 spec Section 9.4.7)
      static const int32_t EPHEM_REF_mask             = 0x00000400; // 1
      static const int32_t REL_EPHEM_mask             = 0x00000800; // 11 (See V49.2 spec Section 9.4.10)
      static const int32_t ECEF_EPHEM_mask            = 0x00001000; // 13
      static const int32_t INS_EPHEM_mask             = 0x00002000; // 11
      static const int32_t GPS_EPHEM_mask             = 0x00004000; // 11
      static const int32_t DATA_FORMAT_mask           = 0x00008000; // 2 (See V49.2 spec Section 9.13.3)
      static const int32_t STATE_EVENT_mask           = 0x00010000; // 1 (See V49.2 spec Section 9.10 incorrectly listed as CIF1/16)
      static const int32_t DEVICE_ID_mask             = 0x00020000; // 2
      static const int32_t TEMPERATURE_mask           = 0x00040000; // 1 (See V49.2 spec Section 9.10 incorrectly listed as CIF1/18)
      static const int32_t TIME_CALIB_mask            = 0x00080000; // 1 (See V49.2 spec Section 9.7.3.3)
      static const int32_t TIME_ADJUST_mask           = 0x00100000; // 2 (See V49.2 spec Section 9.7.3.1)
      static const int32_t SAMPLE_RATE_mask           = 0x00200000; // 2
      static const int32_t OVER_RANGE_mask            = 0x00400000; // 1
      static const int32_t GAIN_mask                  = 0x00800000; // 1
      static const int32_t REF_LEVEL_mask             = 0x01000000; // 1
      static const int32_t IF_OFFSET_mask             = 0x02000000; // 2
      static const int32_t RF_OFFSET_mask             = 0x04000000; // 2
      static const int32_t RF_FREQ_mask               = 0x08000000; // 2
      static const int32_t IF_FREQ_mask               = 0x10000000; // 2
      static const int32_t BANDWIDTH_mask             = 0x20000000; // 2
      static const int32_t REF_POINT_mask             = 0x40000000; // 1 (See V49.2 spec Section 9.2)
      static const int32_t CHANGE_IND_mask            = 0x80000000; // N/A (0)

      /** CIF0 Field lengths
       *  Note: Enable bits for CIF1/2/3/7 are included as 4-byte lengths, and the
       *        methods that use these values must have the logic to know that the
       *        additional CIFs appear prior to the rest of CIF0's fields.
       *  Note: GPS_ASCII_mask and CONTEXT_ASSOC_mask are not present because
       *        they have variable length that must be determined for each case.
       */
      /** 4-byte fields */
      static const int32_t CTX_4_OCTETS  = REF_POINT_mask     | REF_LEVEL_mask     | GAIN_mask          | OVER_RANGE_mask
                                         | TIME_CALIB_mask    | TEMPERATURE_mask   | STATE_EVENT_mask   | EPHEM_REF_mask
                                         | CIF1_ENABLE_mask   | CIF2_ENABLE_mask   | CIF3_ENABLE_mask   | CIF7_ENABLE_mask;
      /** 8-byte fields */
      static const int32_t CTX_8_OCTETS  = BANDWIDTH_mask     | IF_FREQ_mask       | RF_FREQ_mask       | RF_OFFSET_mask
                                         | IF_OFFSET_mask     | SAMPLE_RATE_mask   | TIME_ADJUST_mask   | DEVICE_ID_mask
                                         | DATA_FORMAT_mask;
      /** 44-byte fields */
      static const int32_t CTX_44_OCTETS = GPS_EPHEM_mask     | INS_EPHEM_mask     | REL_EPHEM_mask;
      /** 52-byte fields */
      static const int32_t CTX_52_OCTETS = ECEF_EPHEM_mask;
      // others (variable): GPS_ASCII_mask CONTEXT_ASOC_mask

    } END_NAMESPACE // protected_CIF0 namespace

    /** <b>Internal Use Only:</b> Bitmasks for CIF1 fields (Spatial, Signal,
     * Spectral, I/O, and Control fields)
     */
    namespace protected_CIF1 {

      /** Bitmasks                                                     Field Size (# of 32-bit words) */
      static const int32_t CIF1_RESERVED_0_mask       = 0x00000001; // N/A (0)
      static const int32_t BUFFER_SZ_mask             = 0x00000002; // 2
      static const int32_t VER_BLD_CODE_mask          = 0x00000004; // 1
      static const int32_t V49_COMPL_mask             = 0x00000008; // 1
      static const int32_t HEALTH_STATUS_mask         = 0x00000010; // 1 (See V49.2 spec Section 9.10)
      static const int32_t DISCRETE_IO64_mask         = 0x00000020; // 2
      static const int32_t DISCRETE_IO32_mask         = 0x00000040; // 1
      static const int32_t INDEX_LIST_mask            = 0x00000080; // variable (See V49.2 spec Section 9.3.2)
      static const int32_t CIF1_RESERVED_8_mask       = 0x00000100; // N/A (0)
      static const int32_t SECTOR_SCN_STP_mask        = 0x00000200; // variable (See V49.2 spec Section 9.6.2)
      static const int32_t SPECTRUM_mask              = 0x00000400; // 14
      static const int32_t CIFS_ARRAY_mask            = 0x00000800; // variable (See V49.2 spec Section 9.13.1)
      static const int32_t CIF1_RESERVED_12_mask      = 0x00001000; // N/A (0)
      static const int32_t AUX_BANDWIDTH_mask         = 0x00002000; // 2
      static const int32_t AUX_GAIN_mask              = 0x00004000; // 1
      static const int32_t AUX_FREQUENCY_mask         = 0x00008000; // 2
      static const int32_t SNR_NOISE_mask             = 0x00010000; // 1
      static const int32_t ICPT_PTS_2_3_mask          = 0x00020000; // 1
      static const int32_t COMPRESS_PT_mask           = 0x00040000; // 1
      static const int32_t THRESHOLD_mask             = 0x00080000; // 1
      static const int32_t EB_NO_BER_mask             = 0x00100000; // 1
      static const int32_t CIF1_RESERVED_21_mask      = 0x00200000; // N/A (0)
      static const int32_t CIF1_RESERVED_22_mask      = 0x00400000; // N/A (0)
      static const int32_t CIF1_RESERVED_23_mask      = 0x00800000; // N/A (0)
      static const int32_t RANGE_mask                 = 0x01000000; // 1
      static const int32_t BEAMWIDTH_mask             = 0x02000000; // 1
      static const int32_t SPATIAL_REF_TYPE_mask      = 0x04000000; // 1 (See V49.2 spec Section 9.8.11 for the next 2)
      static const int32_t SPATIAL_SCAN_TYPE_mask     = 0x08000000; // 1
      static const int32_t PNT_VECT_3D_ST_mask        = 0x10000000; // variable (See V49.2 spec Section 9.4.1)
      static const int32_t PNT_VECT_3D_SI_mask        = 0x20000000; // 1
      static const int32_t POLARIZATION_mask          = 0x40000000; // 1
      static const int32_t PHASE_mask                 = 0x80000000; // 1

      /** define field lengths */
      static const int32_t CTX_4_OCTETS = PHASE_mask | POLARIZATION_mask | PNT_VECT_3D_SI_mask | BEAMWIDTH_mask | RANGE_mask
                                        | EB_NO_BER_mask | THRESHOLD_mask | COMPRESS_PT_mask | ICPT_PTS_2_3_mask
                                        | SNR_NOISE_mask | AUX_GAIN_mask | DISCRETE_IO32_mask | HEALTH_STATUS_mask
                                        | V49_COMPL_mask | VER_BLD_CODE_mask | SPATIAL_SCAN_TYPE_mask | SPATIAL_REF_TYPE_mask;
      static const int32_t CTX_8_OCTETS = AUX_FREQUENCY_mask | AUX_BANDWIDTH_mask | DISCRETE_IO64_mask | BUFFER_SZ_mask;
      static const int32_t CTX_52_OCTETS = SPECTRUM_mask;
      static const int32_t CTX_ARR_OF_RECS = PNT_VECT_3D_ST_mask | CIFS_ARRAY_mask | SECTOR_SCN_STP_mask | INDEX_LIST_mask;

    } END_NAMESPACE // protected_CIF1 namespace

    /** <b>Internal Use Only:</b> Bitmasks for CIF2 fields (Identifiers (tags)).
     */
    namespace protected_CIF2 {
      
      /** Bitmasks                                                     Field Size (# of 32-bit words) */
      static const int32_t CIF2_RESERVED_0_mask       = 0x00000001; // N/A (0)
      static const int32_t CIF2_RESERVED_1_mask       = 0x00000002; // N/A (0)
      static const int32_t CIF2_RESERVED_2_mask       = 0x00000004; // N/A (0)
      static const int32_t RF_FOOTPRINT_RANGE_mask    = 0x00000008; // 1 (See V49.2 spec Section 9.8.12 for the next 2)
      static const int32_t RF_FOOTPRINT_mask          = 0x00000010; // 1
      static const int32_t COMM_PRIORITY_ID_mask      = 0x00000020; // 1 (See V49.2 spec Section 9.8.10 for the next 5)
      static const int32_t FUNCT_PRIORITY_ID_mask     = 0x00000040; // 1
      static const int32_t EVENT_ID_mask              = 0x00000080; // 1
      static const int32_t MODE_ID_mask               = 0x00000100; // 1
      static const int32_t FUNCTION_ID_mask           = 0x00000200; // 1
      static const int32_t MODULATION_TYPE_mask       = 0x00000400; // 1 (See V49.2 spec Section 9.8.9 for the next 5)
      static const int32_t MODULATION_CLASS_mask      = 0x00000800; // 1
      static const int32_t EMS_DEVICE_INSTANCE_mask   = 0x00001000; // 1
      static const int32_t EMS_DEVICE_TYPE_mask       = 0x00002000; // 1
      static const int32_t EMS_DEVICE_CLASS_mask      = 0x00004000; // 1
      static const int32_t PLATFORM_DISPLAY_mask      = 0x00008000; // 1 (See V49.2 spec Section 9.8.8 for the next 3)
      static const int32_t PLATFORM_INSTANCE_mask     = 0x00010000; // 1
      static const int32_t PLATFORM_CLASS_mask        = 0x00020000; // 1
      static const int32_t OPERATOR_mask              = 0x00040000; // 1 (See V49.2 spec Section 9.8.7 for the next 2)
      static const int32_t COUNTRY_CODE_mask          = 0x00080000; // 1
      static const int32_t TRACK_ID_mask              = 0x00100000; // 1 (See V49.2 spec Section 9.8.6)
      static const int32_t INFORMATION_SOURCE_mask    = 0x00200000; // 1 (See V49.2 spec Section 9.8.5)
      static const int32_t CONTROLLER_UUID_mask       = 0x00400000; // 4 (See V49.2 spec Section 9.8.3 for the next 5)
      static const int32_t CONTROLLER_ID_mask         = 0x00800000; // 1
      static const int32_t CONTROLLEE_UUID_mask       = 0x01000000; // 4
      static const int32_t CONTROLLEE_ID_mask         = 0x02000000; // 1
      static const int32_t CITED_MESSAGE_ID_mask      = 0x04000000; // 1 (more info in See V49.2 spec Section 9.8.4 too)
      static const int32_t CHILDREN_SID_mask          = 0x08000000; // 1 (See V49.2 spec Section 9.8.2 for the next 4)
      static const int32_t PARENTS_SID_mask           = 0x10000000; // 1
      static const int32_t SIBLINGS_SID_mask          = 0x20000000; // 1
      static const int32_t CITED_SID_mask             = 0x40000000; // 1
      static const int32_t BIND_mask                  = 0x80000000; // 1 (See V49.2 spec Section 9.8.1)
      
      /** define field lengths */
      static const int32_t CTX_4_OCTETS  = BIND_mask | CITED_SID_mask | SIBLINGS_SID_mask | PARENTS_SID_mask | CHILDREN_SID_mask
                                         | CITED_MESSAGE_ID_mask | CONTROLLEE_ID_mask | CONTROLLER_ID_mask
                                         | INFORMATION_SOURCE_mask | TRACK_ID_mask | COUNTRY_CODE_mask | OPERATOR_mask
                                         | PLATFORM_CLASS_mask | PLATFORM_INSTANCE_mask | PLATFORM_DISPLAY_mask
                                         | EMS_DEVICE_CLASS_mask | EMS_DEVICE_TYPE_mask | EMS_DEVICE_INSTANCE_mask
                                         | MODULATION_CLASS_mask | MODULATION_TYPE_mask | FUNCTION_ID_mask | MODE_ID_mask
                                         | EVENT_ID_mask | FUNCT_PRIORITY_ID_mask | COMM_PRIORITY_ID_mask | RF_FOOTPRINT_mask
                                         | RF_FOOTPRINT_RANGE_mask;
      static const int32_t CTX_16_OCTETS = CONTROLLEE_UUID_mask | CONTROLLER_UUID_mask;

    } END_NAMESPACE // protected_CIF2 namespace

    /** <b>Internal Use Only:</b> Bitmasks for CIF3 fields (Temporal and
     * Environmental fields).
     */
    namespace protected_CIF3 {

      /** Bitmasks                                                     Field Size (# of 32-bit words) */
      static const int32_t CIF3_RESERVED_0_mask       = 0x00000001; // N/A (0)
      static const int32_t NETWORK_ID_mask            = 0x00000002; // 1 (See V49.2 spec Section 9.8.13)
      static const int32_t TROPOSPHERIC_STATE_mask    = 0x00000004; // 1
      static const int32_t SEA_AND_SWELL_STATE_mask   = 0x00000008; // 1
      static const int32_t BAROMETRIC_PRESSURE_mask   = 0x00000010; // 1
      static const int32_t HUMIDITY_mask              = 0x00000020; // 1
      static const int32_t SEA_GROUND_TEMP_mask       = 0x00000040; // 1
      static const int32_t AIR_TEMP_mask              = 0x00000080; // 1
      static const int32_t CIF3_RESERVED_8_mask       = 0x00000100; // N/A (0)
      static const int32_t CIF3_RESERVED_9_mask       = 0x00000200; // N/A (0)
      static const int32_t CIF3_RESERVED_10_mask      = 0x00000400; // N/A (0)
      static const int32_t CIF3_RESERVED_11_mask      = 0x00000800; // N/A (0)
      static const int32_t CIF3_RESERVED_12_mask      = 0x00001000; // N/A (0)
      static const int32_t CIF3_RESERVED_13_mask      = 0x00002000; // N/A (0)
      static const int32_t CIF3_RESERVED_14_mask      = 0x00004000; // N/A (0)
      static const int32_t CIF3_RESERVED_15_mask      = 0x00008000; // N/A (0)
                                                                    // For the next two, See V49.2 spec Section 9.7.2
      static const int32_t SHELF_LIFE_mask            = 0x00010000; // 1, 2, or 3 (same as packet tstamp based on TSI and TSF)
      static const int32_t AGE_mask                   = 0x00020000; // 1, 2, or 3 (same as packet tstamp based on TSI and TSF)
      static const int32_t CIF3_RESERVED_18_mask      = 0x00040000; // N/A (0)
      static const int32_t CIF3_RESERVED_19_mask      = 0x00080000; // N/A (0)
      static const int32_t JITTER_mask                = 0x00100000; // 2 (See V49.2 spec Section 9.7.1 for the next 8)
      static const int32_t DWELL_mask                 = 0x00200000; // 2
      static const int32_t DURATION_mask              = 0x00400000; // 2
      static const int32_t PERIOD_mask                = 0x00800000; // 2
      static const int32_t PULSE_WIDTH_mask           = 0x01000000; // 2
      static const int32_t OFFSET_TIME_mask           = 0x02000000; // 2
      static const int32_t FALL_TIME_mask             = 0x04000000; // 2
      static const int32_t RISE_TIME_mask             = 0x08000000; // 2
      static const int32_t CIF3_RESERVED_28_mask      = 0x10000000; // N/A (0)
      static const int32_t CIF3_RESERVED_29_mask      = 0x20000000; // N/A (0)
      static const int32_t TIMESTAMP_SKEW_mask        = 0x40000000; // 2 (See V49.2 spec Section 9.7.3.2)
      static const int32_t TIMESTAMP_DETAILS_mask     = 0x80000000; // 2 (See V49.2 spec Section 9.7.3.4)

      /** define field lengths */
      static const int32_t CTX_4_OCTETS = AIR_TEMP_mask | SEA_GROUND_TEMP_mask | HUMIDITY_mask | BAROMETRIC_PRESSURE_mask
                                        | SEA_AND_SWELL_STATE_mask | TROPOSPHERIC_STATE_mask | NETWORK_ID_mask;
      static const int32_t CTX_8_OCTETS = TIMESTAMP_DETAILS_mask | TIMESTAMP_SKEW_mask | RISE_TIME_mask | FALL_TIME_mask
                                        | OFFSET_TIME_mask | PULSE_WIDTH_mask | PERIOD_mask | DURATION_mask
                                        | DWELL_mask | JITTER_mask;
      /** This indicates the field is the same size as the timestamps, as defined by TSI and TSF bits in header */
      static const int32_t CTX_TSTAMP_OCTETS = AGE_mask | SHELF_LIFE_mask;

    } END_NAMESPACE // protected_CIF3 namespace

    // CIF4 -- reserved/undefined
    // CIF5 -- reserved/undefined
    // CIF6 -- reserved/undefined

    /** <b>Internal Use Only:</b> Bitmasks for CIF7 fields (Attributes).
     */
    namespace protected_CIF7 {

      /** Bitmasks                                                     Attribute Size (in 32-bit words OR relation to field) */
      static const int32_t CIF7_RESERVED_0_mask       = 0x00000001; // N/A (0)
      static const int32_t CIF7_RESERVED_1_mask       = 0x00000002; // N/A (0)
      static const int32_t CIF7_RESERVED_2_mask       = 0x00000004; // N/A (0)
      static const int32_t CIF7_RESERVED_3_mask       = 0x00000008; // N/A (0)
      static const int32_t CIF7_RESERVED_4_mask       = 0x00000010; // N/A (0)
      static const int32_t CIF7_RESERVED_5_mask       = 0x00000020; // N/A (0)
      static const int32_t CIF7_RESERVED_6_mask       = 0x00000040; // N/A (0)
      static const int32_t CIF7_RESERVED_7_mask       = 0x00000080; // N/A (0)
      static const int32_t CIF7_RESERVED_8_mask       = 0x00000100; // N/A (0)
      static const int32_t CIF7_RESERVED_9_mask       = 0x00000200; // N/A (0)
      static const int32_t CIF7_RESERVED_10_mask      = 0x00000400; // N/A (0)
      static const int32_t CIF7_RESERVED_11_mask      = 0x00000800; // N/A (0)
      static const int32_t CIF7_RESERVED_12_mask      = 0x00001000; // N/A (0)
      static const int32_t CIF7_RESERVED_13_mask      = 0x00002000; // N/A (0)
      static const int32_t CIF7_RESERVED_14_mask      = 0x00004000; // N/A (0)
      static const int32_t CIF7_RESERVED_15_mask      = 0x00008000; // N/A (0)
      static const int32_t CIF7_RESERVED_16_mask      = 0x00010000; // N/A (0)
      static const int32_t CIF7_RESERVED_17_mask      = 0x00020000; // N/A (0)
      static const int32_t CIF7_RESERVED_18_mask      = 0x00040000; // N/A (0)
      static const int32_t BELIEF_mask                = 0x00080000; // 1 (See V49.2 spec Section 9.12 rules)
      static const int32_t PROBABILITY_mask           = 0x00100000; // 1 (See V49.2 spec Section 9.12 rules)
      static const int32_t THIRD_DERIVATIVE_mask      = 0x00200000; // same as field it describes
      static const int32_t SECOND_DERIVATIVE_mask     = 0x00400000; // same as field it describes
      static const int32_t FIRST_DERIVATIVE_mask      = 0x00800000; // same as field it describes
      static const int32_t ACCURACY_mask              = 0x01000000; // same as field it describes
      static const int32_t PRECISION_mask             = 0x02000000; // same as field it describes
      static const int32_t MIN_VALUE_mask             = 0x04000000; // same as field it describes
      static const int32_t MAX_VALUE_mask             = 0x08000000; // same as field it describes
      static const int32_t STANDARD_DEVIATION_mask    = 0x10000000; // same as field it describes
      static const int32_t MEDIAN_VALUE_mask          = 0x20000000; // same as field it describes
      static const int32_t AVERAGE_VALUE_mask         = 0x40000000; // same as field it describes
      static const int32_t CURRENT_VALUE_mask         = 0x80000000; // same as field it describes

      /** Attribute sizes (See V49.2 spec Section 9.12) */
      static const int32_t CTX_4_OCTETS    = PROBABILITY_mask | BELIEF_mask; // These are 4 bytes regardless of field
      /** These essentially apply a multiplier to the size of the original field for each attr included */
      static const int32_t CTX_SAME_OCTETS = CURRENT_VALUE_mask | AVERAGE_VALUE_mask | MEDIAN_VALUE_mask 
                                           | STANDARD_DEVIATION_mask | MAX_VALUE_mask | MIN_VALUE_mask | PRECISION_mask 
                                           | ACCURACY_mask | FIRST_DERIVATIVE_mask | SECOND_DERIVATIVE_mask
                                           | THIRD_DERIVATIVE_mask;

    } END_NAMESPACE // protected_CIF7 namespace

  } END_NAMESPACE // namespace IndicatorFields
  using namespace IndicatorFields;

  // Classes originally from BasicContextPacket.h

  using namespace VRTMath;

  class AbstractGeolocation;  // declared below
  class Geolocation;          // declared below
  class Ephemeris;            // declared below
  class ContextAssocLists;    // declared below
  class GeoSentences;         // declared below
  class IndexFieldList;       // declared below
  class ArrayOfRecords;       // declared below
  class SpectrumField;        // declared below

  /** Represents a GPS or INS geolocation fix. <i>The fact that {@link GeoSentences}
   *  does not extend this class is not accidental.</i>
   *  @see Geolocation
   *  @see Ephemeris
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
    public: virtual Value*    getField      (int32_t id) const __attribute__((warn_unused_result));
    public: virtual void      setField      (int32_t id, const Value* val);
  };

  /** Represents a GPS or INS geolocation fix.
   *  @see Ephemeris
   *  @see GeoSentences
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
     *  @return The altitude in meters (whether this is MSL or WGS-84 is defined elsewhere).
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
     *  @param val The altitude in meters (whether this is MSL or WGS-84 is defined elsewhere).
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
    public: virtual Value*    getField      (int32_t id) const __attribute__((warn_unused_result));
    public: virtual void      setField      (int32_t id, const Value* val);
  };

  /** Represents an ECEF ephemeris fix.
   *  @see Geolocation
   */
  class Ephemeris : public AbstractGeolocation {
    /** Creates a new instance. */
    public: Ephemeris ();

    /** Basic copy constructor. */
    public: Ephemeris (const Ephemeris &r);

    public: virtual string toString () const;

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

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Implement HasFields
    //////////////////////////////////////////////////////////////////////////////////////////////////
    public: virtual int32_t   getFieldCount () const;
    public: virtual string    getFieldName  (int32_t id) const;
    public: virtual ValueType getFieldType  (int32_t id) const;
    public: virtual Value*    getField      (int32_t id) const __attribute__((warn_unused_result));
    public: virtual void      setField      (int32_t id, const Value* val);
  };

  /** Represents GPS "sentences" describing a GPS geolocation fix. <br>
   *  <br>
   *  <i>Users are strongly encouraged to consider using the binary form when specifying a
   *  GPS geolocation fix (see {@link Geolocation}). There is no intention of adding any
   *  significant capabilities to this class.</i>
   *  @see Geolocation
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
    public: virtual Value*    getField      (int32_t id) const __attribute__((warn_unused_result));
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
    public: virtual Value*    getField      (int32_t id) const __attribute__((warn_unused_result));
    public: virtual void      setField      (int32_t id, const Value* val);
  };

  /** Specifies a Index Field List. */
  class IndexFieldList : public Record {
    /** Creates a new instance. */
    public: IndexFieldList ();

    /** Basic copy constructor. */
    public: IndexFieldList (const IndexFieldList &r);

    public: virtual string toString () const;

    public: inline virtual void writeBytes (const void *buffer) {
      // Write Counts
      setByteLength(8);
      Record::writeBytes(buffer);

      // Size as appropriate
      updateByteLength();

      // Write Data
      Record::writeBytes(buffer);
    }

    /** Updates the byte length. */
    private: void updateByteLength ();

    private: inline int32_t getIndexEntriesByteOffset () const { return 8; }

    /** Gets the total size of Index List Field in number of 32-bit words. */
    public: inline int32_t getTotalFieldSize () const { return unpackInt(0); }

    /** Gets the number of Index Entries. */
    public: inline int32_t getIndexEntriesCount () const { return unpackInt(4) & 0xFFFFF; }

    /** Gets the size of each Index Entry in number of Bytes. */
    public: inline int8_t getIndexEntrySize () const { return (unpackByte(4)>>4)&0x7; }

    /** Sets the number of Index Entries. */
    public: void setIndexEntriesCount (int32_t val);

    /** Sets the size of each Index Entry in number of Bytes. */
    public: void setIndexEntrySize (int8_t val);

    /** Gets a copy of the Index Field List.
     *  @return A copy of the Index Field list (returns an arry of length 0 if there are no entries).
     */
    public: vector<int32_t> getIndexEntries () const;

    /** Sets the Index Field List.
     *  @param val The Index Field List (an arry of length 0 if there are no entries).
     *  @param bpe The number of bytes to use per element; 0 to use current setting.
     */
    public: void setIndexEntries (const vector<int32_t> &val, int8_t bpe);
    public: void setIndexEntries (const vector<int32_t> &val);
    public: void setIndexEntries (const vector<int16_t> &val);
    public: void setIndexEntries (const vector<int8_t> &val);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Implement HasFields
    //////////////////////////////////////////////////////////////////////////////////////////////////
    public: virtual int32_t   getFieldCount () const;
    public: virtual string    getFieldName  (int32_t id) const;
    public: virtual ValueType getFieldType  (int32_t id) const;
    public: virtual Value*    getField      (int32_t id) const __attribute__((warn_unused_result));
    public: virtual void      setField      (int32_t id, const Value* val);
  };

  /** Specifies an Array of Records. */
  class ArrayOfRecords : public Record {
    /** Creates a new instance. */
    public: ArrayOfRecords ();

    /** Basic copy constructor. */
    public: ArrayOfRecords (const ArrayOfRecords &r);

    public: virtual string toString () const;

    public: inline virtual void writeBytes (const void *buffer) {
      // Write Counts
      setByteLength(12);
      Record::writeBytes(buffer);

      // Size as appropriate
      updateByteLength();

      // Write Data
      Record::writeBytes(buffer);
    }

    /** Updates the byte length. */
    private: void updateByteLength (int32_t off=-1);

    /** Gets the total size of Array Of Records in number of 32-bit words. */
    public: inline int32_t getTotalFieldSize () const { return unpackInt(0); }

    /** Gets the size of the optional Application-Specific Header in number of 32-bit words. */
    public: inline int32_t getHeaderSize () const { return ((int32_t) unpackByte(4)) & 0xFF; }

    /** Gets the size of each Record in number of 32-bit words. */
    public: inline int32_t getRecordSize () const { return (unpackInt(4)>>12) & 0xFFF; }

    /** Sets the size of each Record in number of 32-bit words. */
    public: void setRecordSize (int32_t val);

    /** Gets the number of Records. */
    public: inline int32_t getRecordCount () const { return unpackInt(4) & 0xFFF; }

    /** Sets the number of Records. */
    public: void setRecordCount (int32_t val, int32_t off=-1);

    /** Gets the Bit-mapped indicator for optional record subfields. */
    public: inline int32_t getBitMappedIndicator () const { return unpackInt(8); }

    /** Sets the Bit-mapped indicator for optional record subfields. */
    public: inline void setBitMappedIndicator (int32_t val) { packInt(8, val); }

    
    /** Gets a copy of the Application-Specific Header as a byte buffer.
     *  @return A copy of the Application-Specific Header (returns an arry of length 0 if none).
     */
    public: inline vector<char> getApplicationSpecificHeader () const { return unpackBytes(12, getHeaderSize()*4); }

    /** Sets the Application-Specific Header
     *  @param val The Application-Specific Header (an arry of length 0 if none).
     *  @throws VRTException if size of Application-Specific header exceeds 255 32-bit words
     */
    public: void setApplicationSpecificHeader (const vector<char> &val);

    /** Sets the Application-Specific Header
     *  If val pointer is null, Application-Specific Header is only resized to
     *  the number of bytes specified.
     *  @param val The Application-Specific Header (an arry of length 0 if none).
     *  @param bytes The size of Application-Specific Header in bytes.
     *  @throws VRTException if size of Application-Specific header exceeds 255 32-bit words
     *  @throws VRTException if size of Application-Specific header is negative
     */
    public: void setApplicationSpecificHeader (const char* val, size_t bytes);

    /** Gets the byte offset of the first record.
     *  Note: If 0 records, value will be equal to getTotalFieldSize().
     *  @return Byte offset of first record.
     */
    private: inline int32_t getRecordEntriesByteOffset () const { return 12+4*getHeaderSize(); }

    /** Gets a copy of the Array of Records.
     *  @param idx Index of first record to get. Invalid indicies result in
     *  empty vector returned.
     *  @param num Number of records to get. Invalid number (i.e. negative or
     *  too high) result in max available from index specified.
     *  @return A copy of the Array of Records (returns an arry of length 0 if there are no entries).
     */
    public: vector<Record> getRecords (int32_t idx=0, int32_t num=-1) const;

    /** Gets a copy of a single Record.
     *  @param idx Index of record to get.
     *  @param rec Record to be populated with results. Will be resized if
     *  necessary. Set to Null Record if there are no entries or index is
     *  invalid.
     */
    public: void getRecord (int32_t idx, Record &rec) const;

    /** Sets the Array of Records.
     *  @param val The Array of Records (an arry of length 0 if there are no entries).
     */
    public: void setRecords (const vector<Record> &val);

    /** Adds Record(s) to the Array of Records.
     *  All Records must be the same size. Function returns upon encountering
     *  a Record of differing size.
     *  @param val The Array of Records to add.
     *  @param idx The index to insert the new Records. Records are added to the
     *  end for invalid indices.
     *  @return The number of Records inserted. Number may be less than total
     *  attempted if a Record of different size is encountered.
     */
    public: int32_t addRecords (const vector<Record> &val, int32_t idx=-1);

    /** Adds Record(s) to the Array of Records.
     *  @param val The Array of Records to add.
     *  @param idx The index to insert the new Records.
     *  @return True if successfully inserted Record; False otherwise.
     */
    public: bool addRecord (const Record &val, int32_t idx=-1);


    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Implement HasFields
    //////////////////////////////////////////////////////////////////////////////////////////////////
    public: virtual int32_t   getFieldCount () const;
    public: virtual string    getFieldName  (int32_t id) const;
    public: virtual ValueType getFieldType  (int32_t id) const;
    public: virtual Value*    getField      (int32_t id) const __attribute__((warn_unused_result));
    public: virtual void      setField      (int32_t id, const Value* val);
  };

  /** Specifies a Spectrum Field. */
  class SpectrumField : public Record {
    /** Creates a new instance. */
    public: SpectrumField ();

    /** Basic copy constructor. */
    public: SpectrumField (const SpectrumField &r);

    public: virtual string toString () const;

    /** SpectrumType: word 0, bits 7..0 **/
    public: virtual int8_t getSpectrumType () const { return unpackByte(3); }
    public: virtual void setSpectrumType (int8_t val) { packByte(3, val); }

    /** AveragingType: word 0, bits 15..8 **/
    public: virtual int8_t getAveragingType () const { return unpackByte(2); }
    public: virtual void setAveragingType (int8_t val) { packByte(2, val); }

    /** WindowTime: word 0, bits 19..16 **/
    public: virtual int8_t getWindowTime () const { return (unpackByte(1) & 0xF); }
    public: virtual void setWindowTime (int8_t val) { packByte(1, val & 0xF); }

    //reserved: word 0, bits 31..20

    /** WindowType: word 1, bits 7..0 **/
    public: virtual int8_t getWindowType () const { return unpackByte(7); }
    public: virtual void setWindowType (int8_t val) { packByte(7, val); }

    // reserved: word 1, bits 31..8

    // NumTransformPoints: word 2, bits 31..0
    public: virtual int32_t getNumTransformPoints () const { return unpackInt(8); }
    public: virtual void setNumTransformPoints (int32_t val) { packInt(8, val); }

    /** NumWindowPoints: word 3 **/
    public: virtual int32_t getNumWindowPoints () const { return unpackInt(12); }
    public: virtual void setNumWindowPoints (int32_t val) { packInt(12, val); }

    /** Resolution: words 4-5
     *  64-bit floating-point w/ radix point to the right of bit 20
     */
    public: virtual double getResolution () const {
      int64_t bits = unpackLong(16);
      return (isNull(bits))? DOUBLE_NAN : VRTMath::toDouble64(20,bits);
    }
    public: virtual void setResolution (double val) {
      int64_t bits = (isNull(val))? INT64_NULL : VRTMath::fromDouble64(20,val);
      packLong(16, bits);
    }

    /** Span: words 6-7
     *  64-bit floating-point w/ radix point to the right of bit 20
     */
    public: virtual double getSpan () const {
      int64_t bits = unpackLong(24);
      return (isNull(bits))? DOUBLE_NAN : VRTMath::toDouble64(20,bits);
    }
    public: virtual void setSpan (double val) {
      int64_t bits = (isNull(val))? INT64_NULL : VRTMath::fromDouble64(20,val);
      packLong(24, bits);
    }

    /** NumAverages: word 8 **/
    public: virtual int32_t getNumAverages () const { return unpackInt(32); }
    public: virtual void setNumAverages (int32_t val) { packInt(32, val); }

    /** WeightingFactor: word 9 **/
    public: virtual int32_t getWeightingFactor () const { return unpackInt(36); }
    public: virtual void setWeightingFactor (int32_t val) { packInt(36, val); }

    /** SpectrumF1Index: word 10 **/
    public: virtual int32_t getSpectrumF1Index () const { return unpackInt(40); }
    public: virtual void setSpectrumF1Index (int32_t val) { packInt(40, val); }

    /** SpectrumF2Index: word 11 **/
    public: virtual int32_t getSpectrumF2Index () const { return unpackInt(44); }
    public: virtual void setSpectrumF2Index (int32_t val) { packInt(44, val); }

    /** WindowTimeDelta(int): word 12
     *  32-bit integer
     */
    public: virtual int32_t getWindowTimeDeltaInt () const { return unpackInt(48); }
    public: virtual void setWindowTimeDeltaInt (int32_t val) { packInt(48, val); }

    /** WindowTimeDelta(float): word 12
     *  32-bit floating-point w/ radix point to the right of bit 12
     */
    public: virtual double getWindowTimeDeltaFloat () const {
      int32_t bits = unpackInt(48);
      return (bits == 0x7FFFFFFF)? DOUBLE_NAN : VRTMath::toDouble32(12, bits);
    }
    public: virtual void setWindowTimeDeltaFloat (double val) {
      int32_t bits = (isNull(val))? 0x7FFFFFFF : VRTMath::fromDouble32(12, val);
      packInt(48, bits);
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

  /** XXX Interface for working with Context/Command Indicator Fields (CIFs) XXX
   *
   */
  class IndicatorFieldProvider {
    
    public: virtual ~IndicatorFieldProvider () { }

    /**********************************************************/
    /* Functions from BasicContextPacket that are CIF related */
    /**********************************************************/

    /** Gets the offset for the given context indicator field relative to the location of the first
     *  occurrence of CIF0. 
     *  For access to the second occurrence of CIFs (i.e. Error fields in Ack packets), add 8
     *  to the CIF number, effectively setting the 4th bit (mod8 gives field, div8 gives occurrence).
     *  @param cifNum number of CIF that field belongs to.
     *  @param field bitmask associated with field of interest.
     *  @return offset in bytes from beginning of CIF payload, or CIF7 as described above.
     *  @throws VRTException If the CIF number is invalid.
     */
    protected: virtual int32_t getOffset (int8_t cifNum, int32_t field) const = 0;
    protected: virtual int32_t getOffset (IndicatorFieldEnum_t field) const {
      return getOffset(getCIFNumber(field), getCIFBitMask(field));
    }
    // Used for getting attribute of a field, based on CIF7 attributes
    // A negative return value means either the field or the attribute is not present, or both.
    // A NULL return value means the entire CIF is not present.
    // @return offset to attribute of specified field
    // @throws VRTException when field has invalid length (size)
    // @throws VRTException when cif7bit is specified (non-zero) and CIF7 is not enabled
    protected: virtual int32_t getOffset (int8_t cifNum, int32_t field, int32_t cif7bit) const {
      int32_t fieldOffset = getOffset(cifNum, field);
      if (cif7bit == 0 || isNull(fieldOffset) || fieldOffset<0) return fieldOffset;
      //if (fieldOffset < 0) throw VRTException("Cannot get attribute of a field that is not present.");
      int32_t fieldLen = getFieldLen(cifNum, field);
      if (fieldLen <= 0) throw VRTException("Cannot get attribute of a field with invalid length.");
      int32_t cif7Offset = getCIF7Offset(cif7bit, fieldLen, cifNum&0x08);
      int32_t cif7length = getFieldLen(7,cif7bit, fieldLen);
      if (cif7Offset < 0) {
        if (fieldOffset > 0) fieldOffset = -fieldOffset;
        cif7Offset += cif7length; // adjust to be offset to start of attribute, not end
      } else {
        cif7Offset -= cif7length; // adjust to be offset to start of attribute, not end
        if (fieldOffset < 0) cif7Offset = -cif7Offset;
      }
      return fieldOffset + cif7Offset;
    }
    // XXX this may cause ambiguity since both args are ints, same as first getOffset defined above
    protected: virtual int32_t getOffset (IndicatorFieldEnum_t field, IndicatorFieldEnum_t cif7field) const {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      return getOffset(getCIFNumber(field), getCIFBitMask(field), cif7bit);
    }
    // Used for getting second occurrence of CIFs (i.e. Ack packets have second occurrence for errors (first is for warnings)
    protected: virtual int32_t getOffset (IndicatorFieldEnum_t field, bool occurrence) const {
      return getOffset(getCIFNumber(field) | (((int8_t)occurrence)<<3), getCIFBitMask(field));
    }
    // Combo of cif7 attributes and second occurrence
    // XXX - if this is only ever used with occurrence=true, replace with new function name that does it without occurrence arg
    protected: virtual int32_t getOffset (IndicatorFieldEnum_t field, IndicatorFieldEnum_t cif7field, bool occurrence) const {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      return getOffset(getCIFNumber(field) | (((int8_t)occurrence)<<3), getCIFBitMask(field), cif7bit);
    }

    /** Gets the offset of the CIF7 attribute, INCLUDING the attribute
     *  This offset needs to be calculated differently because a value of 0 is
     *  permitted, and we can't return -0 to indicate not present. Instead,
     *  offset is from the start of the parent non-CIF7 field and includes
     *  the entire CIF7 attribute indicated. This is essentially an offset to
     *  the end of the field instead of the beginning. It can be easily
     *  adjusted by the calling function by reducing the magnitude by the
     *  length of the CIF7 attribute (i.e. getFieldLen(7,attr,len) ).
     *  @param attr CIF7 attribute bitmask
     *  @param len Length of the parent field
     *  @param  occurrence False (0) if first occurrence (default; almost everything),
     *                     True (1) if second occurrence (only Errors for BasicAcknowledgePacket)
     *  @return Offset from start of the parent field to the end of CIF7 attribute
     *  @throws VRTException for classes that do not support CIF7.
     */
    protected: virtual int32_t getCIF7Offset (int32_t attr, int32_t len, bool occurrence=0) const {
      UNUSED_VARIABLE(attr);
      UNUSED_VARIABLE(len);
      UNUSED_VARIABLE(occurrence);
      // Default impl throws exception for classes that don't support CIF7
      throw VRTException("Class does not support CIF7.");
    }

    /** Gets the total size of the field, including all CIF7 attributes
     *  @param  fieldLen Length of the field w/o including CIF7 attributes
     *  @param  occurrence False (0) if first occurrence (default; almost everything),
     *                     True (1) if second occurrence (only Errors for BasicAcknowledgePacket)
     *  @return Length of the field including CIF7 attributes
     */
    protected: virtual int32_t getTotalFieldSize (int32_t fieldLen, bool occurrence=0) const = 0;

    /** Gets the length of the given field when present
     *  @param  cifNum CIF number
     *  @param  field CIF field bitmask
     *  @param  parent Length of parent CIF field (only applicable for CIF7 fields)
     *  @return length of the field. -1 if not found
     */
    protected: virtual int32_t getFieldLen (int8_t cifNum, int32_t field, int32_t parent=0) const = 0;
    protected: virtual int32_t getFieldLen (IndicatorFieldEnum_t field, int32_t parent=0) const {
      return getFieldLen(getCIFNumber(field), getCIFBitMask(field), parent);
    }

    /** Unpacks a 16-bit integer from the payload at the indicated position. */
    protected: virtual int8_t getB (int8_t cifNum, int32_t bit, int32_t xoff, int32_t cif7bit) const = 0;
    protected: virtual int8_t getB (IndicatorFieldEnum_t field, int32_t xoff, IndicatorFieldEnum_t cif7field) const {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      return getB(getCIFNumber(field), getCIFBitMask(field), xoff, cif7bit);
    }

    /** Packs a 16-bit integer from the payload at the indicated position. */
    protected: virtual void setB (int8_t cifNum, int32_t bit, int32_t xoff, int8_t val, int32_t cif7bit) = 0;
    protected: virtual void setB (IndicatorFieldEnum_t field, int32_t xoff, int8_t val, IndicatorFieldEnum_t cif7field) {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      setB(getCIFNumber(field), getCIFBitMask(field), xoff, val, cif7bit);
    }

    /** Unpacks a 16-bit integer from the payload at the indicated position. */
    protected: virtual int16_t getI (int8_t cifNum, int32_t bit, int32_t xoff, int32_t cif7bit) const = 0;
    protected: virtual int16_t getI (IndicatorFieldEnum_t field, int32_t xoff, IndicatorFieldEnum_t cif7field) const {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      return getI(getCIFNumber(field), getCIFBitMask(field), xoff, cif7bit);
    }

    /** Packs a 16-bit integer from the payload at the indicated position. */
    protected: virtual void setI (int8_t cifNum, int32_t bit, int32_t xoff, int16_t val, int32_t cif7bit) = 0;
    protected: virtual void setI (IndicatorFieldEnum_t field, int32_t xoff, int16_t val, IndicatorFieldEnum_t cif7field) {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      setI(getCIFNumber(field), getCIFBitMask(field), xoff, val, cif7bit);
    }

    /** Unpacks a 32-bit integer from the payload at the indicated position. */
    protected: virtual int32_t getL (int8_t cifNum, int32_t bit, int32_t cif7bit) const = 0;
    protected: virtual int32_t getL (IndicatorFieldEnum_t field, IndicatorFieldEnum_t cif7field) const {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      return getL(getCIFNumber(field), getCIFBitMask(field), cif7bit);
    }

    /** Unpacks a 32-bit integer from the payload at the indicated position. */
    protected: virtual int32_t getL24 (int8_t cifNum, int32_t bit, int32_t offset, int32_t cif7bit) const = 0;
    protected: virtual int32_t getL24 (IndicatorFieldEnum_t field, int32_t offset, IndicatorFieldEnum_t cif7field) const {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      return getL24(getCIFNumber(field), getCIFBitMask(field), offset, cif7bit);
    }

    /** Packs a 32-bit integer from the payload at the indicated position. */
    protected: virtual void setL (int8_t cifNum, int32_t bit, int32_t val, int32_t cif7bit) = 0;
    protected: virtual void setL (IndicatorFieldEnum_t field, int32_t val, IndicatorFieldEnum_t cif7field) {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      setL(getCIFNumber(field), getCIFBitMask(field), val, cif7bit);
    }

    /** Unpacks a 64-bit integer from the payload at the indicated position. */
    protected: virtual int64_t getX (int8_t cifNum, int32_t bit, int32_t cif7bit) const = 0;
    protected: virtual int64_t getX (IndicatorFieldEnum_t field, IndicatorFieldEnum_t cif7field) const {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      return getX(getCIFNumber(field), getCIFBitMask(field), cif7bit);
    }

    /** Packs a 64-bit integer from the payload at the indicated position. */
    protected: virtual void setX (int8_t cifNum, int32_t bit, int64_t val, int32_t cif7bit) = 0;
    protected: virtual void setX (IndicatorFieldEnum_t field, int64_t val, IndicatorFieldEnum_t cif7field) {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      setX(getCIFNumber(field), getCIFBitMask(field), val, cif7bit);
    }

    /** Unpacks a 128-bit UUID from the payload at the indicated position. */
    protected: virtual UUID getUUID (int8_t cifNum, int32_t bit, int32_t cif7bit) const = 0;
    protected: virtual UUID getUUID (IndicatorFieldEnum_t field, IndicatorFieldEnum_t cif7field) const {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      return getUUID(getCIFNumber(field), getCIFBitMask(field), cif7bit);
    }

    /** Packs a 128-bit UUID from the payload at the indicated position. */
    protected: virtual void setUUID (int8_t cifNum, int32_t bit, const UUID &val, int32_t cif7bit) = 0;
    protected: virtual void setUUID (IndicatorFieldEnum_t field, const UUID &val, IndicatorFieldEnum_t cif7field) {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      setUUID(getCIFNumber(field), getCIFBitMask(field), val, cif7bit);
    }

    /** Unpacks a TimeStamp from the payload at the indicated position. */
    protected: virtual TimeStamp getTimeStampField (int8_t cifNum, int32_t bit, int32_t cif7bit) const = 0;
    protected: virtual TimeStamp getTimeStampField (IndicatorFieldEnum_t field, IndicatorFieldEnum_t cif7field) const {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      return getTimeStampField(getCIFNumber(field), getCIFBitMask(field), cif7bit);
    }

    /** Packs a TimeStamp from the payload at the indicated position. */
    protected: virtual void setTimeStampField (int8_t cifNum, int32_t bit, const TimeStamp &val, int32_t cif7bit) = 0;
    protected: virtual void setTimeStampField (IndicatorFieldEnum_t field, const TimeStamp &val, IndicatorFieldEnum_t cif7field) {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      setTimeStampField(getCIFNumber(field), getCIFBitMask(field), val, cif7bit);
    }

    /** Sets a block of data. */
    protected: inline void setRecord (int8_t cifNum, int32_t bit, const Record &val, const Record &old, int32_t cif7bit) {
      if (isNull(old)) setRecord(cifNum, bit, val, -1, cif7bit);
      else             setRecord(cifNum, bit, val, old.getByteLength(), cif7bit);
    }
    protected: inline void setRecord (IndicatorFieldEnum_t field, const Record &val, const Record &old, IndicatorFieldEnum_t cif7field) {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      setRecord(getCIFNumber(field), getCIFBitMask(field), val, old, cif7bit);
    }

    /** Sets a block of data. */
    protected: inline void setRecord (int8_t cifNum, int32_t bit, const Record &val, int32_t cif7bit) {
      setRecord(cifNum, bit, val, getFieldLen(cifNum, bit), cif7bit);
    }
    protected: inline void setRecord (IndicatorFieldEnum_t field, const Record &val, IndicatorFieldEnum_t cif7field) {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      setRecord(getCIFNumber(field), getCIFBitMask(field), val, cif7bit);
    }

    /** Sets a block of data. */
    protected: inline void setRecord (int8_t cifNum, int32_t bit, const Record *val, int32_t cif7bit) {
      setRecord(cifNum, bit, val, getFieldLen(cifNum, bit), cif7bit);
    }
    protected: inline void setRecord (IndicatorFieldEnum_t field, const Record *val, IndicatorFieldEnum_t cif7field) {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      setRecord(getCIFNumber(field), getCIFBitMask(field), val, cif7bit);
    }

    /** Sets a block of data. */
    protected: inline void setRecord (int8_t cifNum, int32_t bit, const Record &val, int32_t oldLen, int32_t cif7bit) {
      if (isNull(val)) setRecord(cifNum, bit, NULL, oldLen, cif7bit);
      else             setRecord(cifNum, bit, &val, oldLen, cif7bit);
    }
    protected: inline void setRecord (IndicatorFieldEnum_t field, const Record &val, int32_t oldLen, IndicatorFieldEnum_t cif7field) {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      setRecord(getCIFNumber(field), getCIFBitMask(field), val, oldLen, cif7bit);
    }

    /** Gets a block of data. */
    protected: virtual void setRecord (int8_t cifNum, int32_t bit, const Record *val, int32_t oldLen, int32_t cif7bit) = 0;
    
    protected: void setRecord (IndicatorFieldEnum_t field, const Record *val, int32_t oldLen, IndicatorFieldEnum_t cif7field) {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      setRecord(getCIFNumber(field), getCIFBitMask(field), val, oldLen, cif7bit);
    }


    /** Gets the CIF for the CIF number specified. May return NULL
     *  To access second occurrence of CIF, add 8 to cifNum.
     *  @param cifNum Integer number of CIF
     *  @return Value of CIF specified if present; NULL otherwise.
     *  @throws VRTException If the CIF number is invalid.
     */
    protected: virtual int32_t getContextIndicatorField (int8_t cifNum) const {
      switch(cifNum&0xF7) { // Ignore bit 3 (occurrence)
        case 0: return getContextIndicatorField0(cifNum&0x08);
        case 1: return getContextIndicatorField1(cifNum&0x08);
        case 2: return getContextIndicatorField2(cifNum&0x08);
        case 3: return getContextIndicatorField3(cifNum&0x08);
        case 7: return getContextIndicatorField7(cifNum&0x08);
        default:
          throw VRTException("Invalid Context Indicator Field number.");
      }
    }

    // May return NULL
    protected: virtual int32_t getContextIndicatorField (IndicatorFieldEnum_t field, bool occurrence=0) const {
      return getContextIndicatorField(getCIFNumber(field) | (((int8_t)occurrence)<<3));
    }

    // These may return NULL
    protected: virtual int32_t getContextIndicatorField0 (bool occurrence=0) const = 0;
    protected: virtual int32_t getContextIndicatorField1 (bool occurrence=0) const = 0;
    protected: virtual int32_t getContextIndicatorField2 (bool occurrence=0) const = 0;
    protected: virtual int32_t getContextIndicatorField3 (bool occurrence=0) const = 0;
    protected: virtual int32_t getContextIndicatorField7 (bool occurrence=0) const = 0;

    /** Gets a CIF bit value.
     *  @param field Indicator field enumeration value for field of interest
     *  @param occurrence False if first occurrence (almost everything), true if second occurrence (only Errors)
     *  @return true if bit is set, false otherwise.
     */
    // XXX Generic CIF bit getter XXX
    protected: virtual boolNull getContextIndicatorFieldBit (IndicatorFieldEnum_t field, bool occurrence=0) const {
      return getContextIndicatorFieldBit(getCIFNumber(field) | (((int8_t)occurrence)<<3), getCIFBitMask(field));
    }
    protected: virtual boolNull getContextIndicatorFieldBit (int8_t cifNum, int32_t bit) const {
      if (!isCIFEnable(cifNum)) return _NULL;
      int32_t cif = getContextIndicatorField(cifNum);
      return ((cif & bit) != 0) ? _TRUE : _FALSE;
    }

    /** Sets the header type, used with getOffset(..) and OFFSET_TABLE.
     *  @throws VRTException If the CIF is not enabled.
     *  @throws VRTException If the CIF number is invalid.
     */
    protected: virtual void setContextIndicatorFieldBit (IndicatorFieldEnum_t field, bool set, bool occurrence=0) {
      setContextIndicatorFieldBit(getCIFNumber(field) | (((int8_t)occurrence)<<3), getCIFBitMask(field), set);
    }
    protected: virtual void setContextIndicatorFieldBit (int8_t cifNum, int32_t bit, bool set) {
      switch(cifNum&0xF7) { // Ignore bit 3 (occurrence)
        case 0: return setContextIndicatorField0Bit(bit, set, cifNum&0x08);
        case 1: return setContextIndicatorField1Bit(bit, set, cifNum&0x08);
        case 2: return setContextIndicatorField2Bit(bit, set, cifNum&0x08);
        case 3: return setContextIndicatorField3Bit(bit, set, cifNum&0x08);
        case 7: return setContextIndicatorField7Bit(bit, set, cifNum&0x08);
        default:
          throw VRTException("Invalid Context Indicator Field number.");
      }
    }
    protected: virtual void setContextIndicatorField0Bit (int32_t bit, bool set, bool occurrence=0) = 0;
    protected: virtual void setContextIndicatorField1Bit (int32_t bit, bool set, bool occurrence=0) = 0;
    protected: virtual void setContextIndicatorField2Bit (int32_t bit, bool set, bool occurrence=0) = 0;
    protected: virtual void setContextIndicatorField3Bit (int32_t bit, bool set, bool occurrence=0) = 0;
    protected: virtual void setContextIndicatorField7Bit (int32_t bit, bool set, bool occurrence=0) = 0;

    /* XXX START OF CIF0 FUNCTIONS XXX */

    /** Gets the CIF1 Enable Indicator.
     *  @return true if bit is set, false otherwise.
     */
    public: inline bool isCIF1Enable (bool occurrence=0) const {
      return ((getContextIndicatorField0(occurrence) & protected_CIF0::CIF1_ENABLE_mask) != 0);
    }

    /** Gets the CIF2 Enable Indicator.
     *  @return true if bit is set, false otherwise.
     */
    public: inline bool isCIF2Enable (bool occurrence=0) const {
      return ((getContextIndicatorField0(occurrence) & protected_CIF0::CIF2_ENABLE_mask) != 0);
    }

    /** Gets the CIF3 Enable Indicator.
     *  @return true if bit is set, false otherwise.
     */
    public: inline bool isCIF3Enable (bool occurrence=0) const {
      return ((getContextIndicatorField0(occurrence) & protected_CIF0::CIF3_ENABLE_mask) != 0);
    }

    /** Gets the CIF7 Enable Indicator.
     *  @return true if bit is set, false otherwise.
     */
    public: inline bool isCIF7Enable (bool occurrence=0) const {
      return ((getContextIndicatorField0(occurrence) & protected_CIF0::CIF7_ENABLE_mask) != 0);
    }

    protected: inline bool isCIFEnable(int8_t cifNum) const {
      switch(cifNum&0xF7) { // Ignore bit 3 (occurrence)
        case 0: return true;
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
     *  @param occurrence False (0) if first occurrence (default; almost everything),
     *                    True (1) if second occurrence (only Errors for BasicAcknowledgePacket)
     */
    public: virtual void addCIF1 (bool add=true, bool occurrence=false) = 0;
    public: virtual void addCIF2 (bool add=true, bool occurrence=false) = 0;
    public: virtual void addCIF3 (bool add=true, bool occurrence=false) = 0;
    public: virtual void addCIF7 (bool add=true, bool occurrence=false) = 0; // TODO CIF7 - if removing, need to adjust packet massively (perhaps true for other CIFs as well)

    /** Gets the Context Field Change Indicator.
     *  @return true if <b>anything</b> in the packet has changed since the last context packet,
     *          false if <b>nothing</b> has changed since the last context packet.
     */
    public: inline bool isChangePacket () const {
      return ((getContextIndicatorField0() & protected_CIF0::CHANGE_IND_mask) != 0);
    }

    /** Gets the Reference Point Identifier.
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The point in the system where this context applies (null if not specified).
     */
    public: inline int32_t getReferencePointIdentifier (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      return getL(REF_POINT, cif7field);
    }

    /** Gets the Timestamp Adjustment in picoseconds. This is the required time adjustment
     *  between the time the signal was digitized (i.e. the timestamp) and the time at the reference
     *  point.
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The timestamp adjustment (null if not specified).
     */
    public: inline int64_t getTimeStampAdjustment (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      return getX(TIME_ADJUST, cif7field);
    }

    /** Gets the Timestamp Calibration Time in seconds. This is the most recent date and
     *  time when the timestamp in the Data and Context packets was known to be correct.
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The calibration time (null if not specified).
     */
    public: inline int32_t getTimeStampCalibration (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      return getL(TIME_CALIB, cif7field);
    }

    /** Gets the Ephemeris Reference Identifier. This is used specifies the process
     *  whose origin applies to the ephemeris returned by {@link #getEphemerisRelative()}.
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The ephemeris information (null if not specified). <i>Note that changes to the
     *          returned object do not alter the packet.</i>
     */
    public: inline int32_t getEphemerisReference (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      return getL(EPHEM_REF, cif7field);
    }

    /** Used for unpacking geolocation records. */
    //protected: virtual Geolocation getGeolocation (IndicatorFieldEnum_t field) const = 0;
    protected: virtual Geolocation getGeolocation (int8_t cifNum, int32_t field, int32_t cif7bit) const = 0;

    /** Used for unpacking ephemeris records. */
    //private: virtual Ephemeris getEphemeris (IndicatorFieldEnum_t field) const = 0;
    protected: virtual Ephemeris getEphemeris (int8_t cifNum, int32_t field, int32_t cif7bit) const = 0;

    /** Gets the Formatted GPS (Global Positioning System) Geolocation for the collector.
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The geolocation information (null if not specified). <i>Note that changes to the
     *          returned object do not alter the packet.</i>
     */
    public: inline Geolocation getGeolocationGPS (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      //return getGeolocation(GPS_EPHEM);
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      return getGeolocation(0, protected_CIF0::GPS_EPHEM_mask, cif7bit);
    }

    /** Gets the Formatted INS (Inertial Navigation System) Geolocation for the collector.
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The geolocation information (null if not specified). <i>Note that changes to the
     *          returned object do not alter the packet.</i>
     */
    public: inline Geolocation getGeolocationINS (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      //return getGeolocation(INS_EPHEM);
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      return getGeolocation(0, protected_CIF0::INS_EPHEM_mask, cif7bit);
    }

    /** Gets the ECEF (Earth-Centered, Earth-Fixed) Ephemeris for the collector.
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The ephemeris information (null if not specified). <i>Note that changes to the
     *          returned object do not alter the packet.</i>
     */
    public: inline Ephemeris getEphemerisECEF (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      //return getEphemeris(ECEF_EPHEM);
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      return getEphemeris(0, protected_CIF0::ECEF_EPHEM_mask, cif7bit);
    }

    /** Gets the Relative Ephemeris for the collector. Unlike {@link #getEphemerisECEF()}
     *  which uses the ECEF coordinate system, this ephemeris is relative to a user-defined system
     *  specified by {@link #getEphemerisReference()}.
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The ephemeris information (null if not specified). <i>Note that changes to the
     *          returned object do not alter the packet.</i>
     */
    public: inline Ephemeris getEphemerisRelative (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      //return getEphemeris(REL_EPHEM);
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      return getEphemeris(0, protected_CIF0::REL_EPHEM_mask, cif7bit);
    }

    /** Gets the GPS ASCII "sentences". These are ASCII "sentences" from a GPS (Global
     *  Positioning System) receiver, such as those conforming to NMEA-0183. <i>Due to the great
     *  variety in GPS "sentences", this class makes little effort to do anything useful with them.
     *  Users are encouraged to use {@link #getGeolocationGPS()} which essentially provides the same
     *  information, but in a consistent form.</i>
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The geolocation information (null if not specified). <i>Note that changes to the
     *          returned object do not alter the packet.</i>
     */
    public: virtual GeoSentences getGeoSentences (IndicatorFieldEnum_t cif7field=CIF_NULL) const = 0;

    /** Gets the Context Association Lists. These lists indicate the other context/data
     *  streams associated with this one.
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The context association lists (null if not specified). <i>Note that changes to the
     *          returned object do not alter the packet.</i>
     */
    public: virtual ContextAssocLists getContextAssocLists (IndicatorFieldEnum_t cif7field=CIF_NULL) const = 0;

    /** Gets the Device Identifier specifying the manufacturer and model of the device
     *  producing this context packet stream.
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The device identifier as a string (null if not specified).
     */
    public: inline string getDeviceID (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      return Utilities::toStringDeviceID(getDeviceIdentifier(cif7field));
    }

    /** Gets the Bandwidth of the signal in Hz.
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The bandwidth (null if not specified).
     */
    public: inline double getBandwidth (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int64_t bits = getX(BANDWIDTH, cif7field);
      return (isNull(bits))? DOUBLE_NAN : VRTMath::toDouble64(20,bits);
    }

    /** Gets the IF Reference Frequency of the signal in Hz.
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The frequency (null if not specified).
     */
    public: inline double getFrequencyIF (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int64_t bits = getX(IF_FREQ, cif7field);
      return (isNull(bits))? DOUBLE_NAN : VRTMath::toDouble64(20,bits);
    }

    /** Gets the RF Reference Frequency of the signal in Hz.
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The frequency (null if not specified).
     */
    public: inline double getFrequencyRF (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int64_t bits = getX(RF_FREQ, cif7field);
      return (isNull(bits))? DOUBLE_NAN : VRTMath::toDouble64(20,bits);
    }

    /** Gets the RF Reference Frequency Offset of the signal in Hz.
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The frequency offset (null if not specified).
     */
    public: inline double getFrequencyOffsetRF (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int64_t bits = getX(RF_OFFSET, cif7field);
      return (isNull(bits))? DOUBLE_NAN : VRTMath::toDouble64(20,bits);
    }

    /** Gets the IF Band Offset of the signal in Hz.
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The band offset (null if not specified).
     */
    public: inline double getBandOffsetIF (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int64_t bits = getX(IF_OFFSET, cif7field);
      return (isNull(bits))? DOUBLE_NAN : VRTMath::toDouble64(20,bits);
    }

    /** Gets the Reference Level of the signal in dBm.
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The reference level (null if not specified).
     */
    public: inline float getReferenceLevel (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int16_t bits = getI(REF_LEVEL,2, cif7field);
      return (isNull(bits))? FLOAT_NAN : VRTMath::toFloat16(7,bits);
    }

    /** Gets the Stage 1 Gain of the device in dB. This is the front-end gain of the
     *  system. In cases where a separate Stage 1 and Stage 2 gain is not necessary, this holds
     *  the total gain of the system and Gain 2 is set to zero.
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The gain (null if not specified).
     */
    public: inline float getGain1 (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int16_t bits = getI(GAIN,2, cif7field);
      return (isNull(bits))? FLOAT_NAN : VRTMath::toFloat16(7,bits);
    }

    /** Gets the Stage 2 Gain of the device in dB. This is the back-end gain of the
     *  system. In cases where a separate Stage 1 and Stage 2 gain is not necessary, the Stage 1
     *  gain holds the total gain of the system and this is set to zero.
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The gain (null if not specified).
     */
    public: inline float getGain2 (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int16_t bits = getI(GAIN,0, cif7field);
      return (isNull(bits))? FLOAT_NAN : VRTMath::toFloat16(7,bits);
    }

    /** Gets the Sample Rate in Hz.
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The sample rate (null if not specified).
     */
    public: inline double getSampleRate (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int64_t bits = getX(SAMPLE_RATE, cif7field);
      return (isNull(bits))? DOUBLE_NAN : VRTMath::toDouble64(20,bits);
    }

    /** Gets the Sample Period (inverse of Sample Rate) in sec.
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The sample period (null if not specified).
     */
    public: inline double getSamplePeriod (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      double sr = getSampleRate(cif7field);
      return (isNull(sr))? DOUBLE_NAN : 1.0/sr;
    }

    /** Gets the Temperature in degrees Celsius. This is used to convey the temperature of
     *  any component that may affect the described signal.
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The temperature (null if not specified).
     */
    public: inline float getTemperature (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int16_t bits = getI(TEMPERATURE,2, cif7field);
      return (isNull(bits))? FLOAT_NAN : VRTMath::toFloat16( 6,bits);
    }

    /** Gets the specified bit from the State and Event Indicator field.
     *  @param enable    Bit position of the enable flag.
     *  @param indicator Bit position of the indicator flag.
     *  @param cif7bit Bit position of the CIF7 attribute indicator flag.
     *  @return null if field is not present, null if the enable bit is not set, true if the enable
     *          bit is set and the indicator bit is set, false if the enable bit is set but the
     *          indicator bit is not set.
     */
    protected: virtual boolNull getStateEventBit (int32_t enable, int32_t indicator, int32_t cif7bit) const = 0;

    /** Gets the calibrated time indicator flag.
     *  <pre>
     *    true  = The time stamp in this packet is calibrated to an external reference (e.g. atomic clock)
     *    false = The time stamp in this packet is *not* calibrated to an external reference
     *            and may be inaccurate
     *    null  = <i>This information is not available</i>
     *  </pre>
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return true if the flag is set, false if it is not set, null if this optional flag isn't
     *          specified (null is returned for all context packets).
     */
    public: inline boolNull isCalibratedTimeStamp (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      return getStateEventBit(31, 19, cif7bit);
    }

    /** Gets the valid data indicator flag.
     *  <pre>
     *    true  = The data in the packet is valid
     *    false = A condition exists that *may* invalidate some or all of the data in the packet
     *    null  = <i>This information is not available</i>
     *  </pre>
     *  <i>Note that the definition of "valid" and "invalid" data is application specific, so it is
     *  important to conslut the documentation for the relevent packet class before using this field.</i>
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return true if the flag is set, false if it is not set, null if this optional flag isn't
     *          specified (null is returned for all context packets).
     */
    public: inline boolNull isDataValid (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      return getStateEventBit(30, 18, cif7bit);
    }

    /** Gets the reference lock indicator flag.
     *  <pre>
     *    true  = All of the phase-locked loops affecting the data are locked and stable
     *    false = At least one of the phase-locked loops affecting the data is not locked and stable
     *    null  = <i>This information is not available</i>
     *  </pre>
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return true if the flag is set, false if it is not set, null if this optional flag isn't
     *          specified (null is returned for all context packets).
     */
    public: inline boolNull isReferenceLocked (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      return getStateEventBit(29, 17, cif7bit);
    }

    /** Gets the AGC/MGC indicator flag.
     *  <pre>
     *    true  = AGC (automatic gain control) is being used
     *    false = MGC (manual gain control) is being used
     *    null  = <i>This information is not available</i>
     *  </pre>
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return true if the flag is set, false if it is not set, null if this optional flag isn't
     *          specified (null is returned for all context packets).
     */
    public: inline boolNull isAutomaticGainControl (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      return getStateEventBit(28, 16, cif7bit);
    }

    /** Gets the signal detected indicator flag.
     *  <pre>
     *    true  = Some signal detected in this packet
     *    false = No signal detected in this packet
     *    null  = <i>This information is not available</i>
     *  </pre>
     *  <i>Note that the definition of what constitutes a detected signal application specific, so it is
     *  important to conslut the documentation for the relevent packet class before using this field.</i>
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return true if the flag is set, false if it is not set, null if this optional flag isn't
     *          specified (null is returned for all context packets).
     */
    public: inline boolNull isSignalDetected (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      return getStateEventBit(27, 15, cif7bit);
    }

    /** Gets the spectral inversion indicator flag.
     *  <pre>
     *    true  = Signal conveyed in the payload has an inverted spectrum w.r.t. an up-stream
     *            reference point
     *    false = Spectrum is not inverted
     *    null  = <i>This information is not available</i>
     *  </pre>
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return true if the flag is set, false if it is not set, null if this optional flag isn't
     *          specified (null is returned for all context packets).
     */
    public: inline boolNull isInvertedSpectrum (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      return getStateEventBit(26, 14, cif7bit);
    }

    /** Gets the over-range indicator flag.
     *  <pre>
     *    true  = The data in the payload is invalid since one or more data samples exceeded the
     *            range of the data item
     *    false = No samples over range
     *    null  = <i>This information is not available</i>
     *  </pre>
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return true if the flag is set, false if it is not set, null if this optional flag isn't
     *          specified (null is returned for all context packets).
     */
    public: inline boolNull isOverRange (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      return getStateEventBit(25, 13, cif7bit);
    }

    /** Gets the sample loss indicator flag.
     *  <pre>
     *    true  = One or more sample discontinuities present in the data due to processing errors
     *            and/or data overflow
     *    false = No discontinuities present
     *    null  = <i>This information is not available</i>
     *  </pre>
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return true if the flag is set, false if it is not set, null if this optional flag isn't
     *          specified (null is returned for all context packets).
     */
    public: inline boolNull isDiscontinuous (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      return getStateEventBit(24, 12, cif7bit);
    }

    /** Gets the User-Defined Bits from the State and Event Indicator Bits. <i>The
     *  definition of these bits should be included in the documentation for the relevent packet
     *  class.</i> Unlike other bits of the State and Event Indicator Field, there is no concept
     *  of an undefined-value for user bits if the State and Event Indicator Field is present to
     *  support some other bits. As such, zero will be returned if the State and Event Indicator
     *  Field is absent.
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return true if the flag is set, false if it is not set, zero if the State and Event
     *  Indicator Field is absent.
     */
    public: inline int32_t getUserDefinedBits (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int8_t bits = getB(STATE_EVENT,3, cif7field);
      return (isNull(bits))? INT32_NULL : ((int32_t)bits)&0x000000FF;
    }

    /** Sets the Context Field Change Indicator.
     *  @param change true if <b>anything</b> in the packet has changed since the last context packet,
     *                false if <b>nothing</b> has changed since the last context packet.
     */
    public: inline void setChangePacket (bool change) {
      setContextIndicatorFieldBit(CHANGE_IND, change);
    }

    /** Sets the Reference Point Identifier.
     *  @param val The point in the system where this context applies (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setReferencePointIdentifier (int32_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setL(REF_POINT, val, cif7field);
    }

    /** Sets the specified bit of the State and Event Indicator field.
     *  @param enable    Bit position of the enable flag.
     *  @param indicator Bit position of the indicator flag.
     *  @param value     The value of the bits: null if the enable bit should not be set, true if
     *                   the enable bit and the indicator bit should be set, false if the enable bit
     *                   should be set but the indicator bit should not be set.
     *  @param cif7bit   Bit position of the CIF7 attribute indicator flag.
     */
    protected: virtual void setStateEventBit (int32_t enable, int32_t indicator, boolNull value, int32_t cif7bit) = 0;

    /** <i>Optional functionality:</i> Sets the calibrated time indicator flag.
     *  @param v true if the flag is set, false if not set, null if this optional flag isn't specified.
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @throws VRTException If this method is not supported or if called on a context packet.
     */
    public: inline void setCalibratedTimeStamp (boolNull v, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      setStateEventBit(31, 19, v, cif7bit);
    }

    /** <i>Optional functionality:</i> Gets the valid data indicator flag.
     *  @param v true if the flag is set, false if not set, null if this optional flag isn't specified.
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @throws VRTException If this method is not supported or if called on a context packet.
     */
    public: inline void setDataValid (boolNull v, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      setStateEventBit(30, 18, v, cif7bit);
    }

    /** <i>Optional functionality:</i> Gets the reference lock indicator flag.
     *  @param v true if the flag is set, false if not set, null if this optional flag isn't specified.
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @throws VRTException If this method is not supported or if called on a context packet.
     */
    public: inline void setReferenceLocked (boolNull v, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      setStateEventBit(29, 17, v, cif7bit);
    }

    /** <i>Optional functionality:</i> Gets the AGC/MGC indicator flag.
     *  @param v true if the flag is set, false if not set, null if this optional flag isn't specified.
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @throws VRTException If this method is not supported or if called on a context packet.
     */
    public: inline void setAutomaticGainControl (boolNull v, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      setStateEventBit(28, 16, v, cif7bit);
    }

    /** <i>Optional functionality:</i> Gets the signal detected indicator flag.
     *  @param v true if the flag is set, false if not set, null if this optional flag isn't specified.
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @throws VRTException If this method is not supported or if called on a context packet.
     */
    public: inline void setSignalDetected (boolNull v, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      setStateEventBit(27, 15, v, cif7bit);
    }

    /** <i>Optional functionality:</i> Gets the spectral inversion indicator flag.
     *  @param v true if the flag is set, false if not set, null if this optional flag isn't specified.
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @throws VRTException If this method is not supported or if called on a context packet.
     */
    public: inline void setInvertedSpectrum (boolNull v, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      setStateEventBit(26, 14, v, cif7bit);
    }

    /** <i>Optional functionality:</i> Gets the over-range indicator flag.
     *  @param v true if the flag is set, false if not set, null if this optional flag isn't specified.
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @throws VRTException If this method is not supported or if called on a context packet.
     */
    public: inline void setOverRange (boolNull v, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      setStateEventBit(25, 13, v, cif7bit);
    }

    /** <i>Optional functionality:</i> Gets the sample loss indicator flag.
     *  @param v true if the flag is set, false if not set, null if this optional flag isn't specified.
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @throws VRTException If this method is not supported or if called on a context packet.
     */
    public: inline void setDiscontinuous (boolNull v, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      int32_t cif7bit = (getCIFNumber(cif7field) != 7) ? 0 : getCIFBitMask(cif7field);
      setStateEventBit(24, 12, v, cif7bit);
    }

    /** Sets the User-Defined Bits from the State and Event Indicator Bits. <i>The
     *  definition of these bits should be included in the documentation for the relevent packet
     *  class.</i> Unlike other bits of the State and Event Indicator Field, there is no concept
     *  of an undefined-value for user bits if the State and Event Indicator Field is present to
     *  support some other bits. As such, no "null value" is supported for the user-defined buts.
     *  @param val true if the flag is set, false if it is not set.
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setUserDefinedBits (int32_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      if ((getOffset(STATE_EVENT) > 0) || !isNull(val)) {
        setB(STATE_EVENT,3,(char)val, cif7field);
      }
    }

    /** Sets the Timestamp Adjustment in picoseconds. This is the required time adjustment
     *  between the time the signal was digitized (i.e. the timestamp) and the time at the reference
     *  point.
     *  @param val The timestamp adjustment (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setTimeStampAdjustment (int64_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setX(TIME_ADJUST,val, cif7field);
    }

    /** Sets the Timestamp Calibration Time in seconds. This is the most recent date and
     *  time when the timestamp in the Data and Context packets was known to be correct.
     *  @param val The calibration time (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setTimeStampCalibration (int32_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setL(TIME_CALIB,val, cif7field);
    }

    /** Sets the Ephemeris Reference Identifier. This is used specifies the process
     *  whose origin applies to the ephemeris returned by {@link #getEphemerisRelative()}.
     *  @param val The ephemeris information (null if not specified). <i>Note that changes to the
     *             returned object do not alter the packet.</i>
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setEphemerisReference (int32_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setL(EPHEM_REF,val, cif7field);
    }

    /** Sets the Formatted GPS (Global Positioning System) Geolocation for the collector.
     *  @param val The geolocation information (null if not specified). <i>Note that changes to the
     *             returned object do not alter the packet.</i>
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setGeolocationGPS (const Geolocation &val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setRecord(GPS_EPHEM,val, cif7field);
    }

    /** Sets the Formatted INS (Inertial Navigation System) Geolocation for the collector.
     *  @param val The geolocation information (null if not specified). <i>Note that changes to the
     *             returned object do not alter the packet.</i>
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setGeolocationINS (const Geolocation &val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setRecord(INS_EPHEM,val, cif7field);
    }

    /** Sets the ECEF (Earth-Centered, Earth-Fixed) Ephemeris for the collector.
     *  @param val The ephemeris information (null if not specified). <i>Note that changes to the
     *             returned object do not alter the packet.</i>
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setEphemerisECEF (const Ephemeris &val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setRecord(ECEF_EPHEM,val, cif7field);
    }

    /** Sets the Relative Ephemeris for the collector.
     *  @param val The ephemeris information (null if not specified). <i>Note that changes to the
     *             returned object do not alter the packet.</i>
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setEphemerisRelative (const Ephemeris &val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setRecord(REL_EPHEM,val, cif7field);
    }

    /** Sets the GPS ASCII "sentences". These are ASCII "sentences" from a GPS (Global
     *  Positioning System) receiver, such as those conforming to NMEA-0183. <i>Due to the great
     *  variety in GPS "sentences", this class makes little effort to do anything useful with them.
     *  Users are encouraged to use {@link #getGeolocationGPS()} which essentially provides the same
     *  information, but in a consistent form.</i>
     *  @param val The geolocation information (null if not specified). <i>Note that changes to the
     *             returned object do not alter the packet.</i>
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setGeoSentences (const GeoSentences &val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setRecord(GPS_ASCII, val, getGeoSentences(), cif7field);
    }

    /** Sets the Context Association Lists. These lists indicate the other context/data
     *  streams associated with this one.
     *  @param val The context association lists (null if not specified). <i>Note that changes to the
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *          returned object do not alter the packet.</i>
     */
    public: inline void setContextAssocLists (const ContextAssocLists &val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setRecord(CONTEXT_ASOC, val, getContextAssocLists(), cif7field);
    }

    /** Sets the Device Identifier specifying the manufacturer and model of the device
     *  producing this context packet stream.
     *  @param val The device identifier as a string (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setDeviceID (const string &val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setDeviceIdentifier(Utilities::fromStringDeviceID(val), cif7field);
    }

    /** Sets the Bandwidth of the signal in Hz.
     *  @param val The bandwidth (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setBandwidth (double val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      int64_t bits = (isNull(val))? INT64_NULL : VRTMath::fromDouble64(20,val);
      setX(BANDWIDTH,bits, cif7field);
    }

    /** Sets the IF Reference Frequency of the signal in Hz.
     *  @param val The frequency (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setFrequencyIF (double val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      int64_t bits = (isNull(val))? INT64_NULL : VRTMath::fromDouble64(20,val);
      setX(IF_FREQ,bits, cif7field);
    }

    /** Sets the RF Reference Frequency of the signal in Hz.
     *  @param val The frequency (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setFrequencyRF (double val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      int64_t bits = (isNull(val))? INT64_NULL : VRTMath::fromDouble64(20,val);
      setX(RF_FREQ,bits, cif7field);
    }

    /** Sets the RF Reference Frequency Offset of the signal in Hz.
     *  @param val The frequency offset (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setFrequencyOffsetRF (double val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      int64_t bits = (isNull(val))? INT64_NULL : VRTMath::fromDouble64(20,val);
      setX(RF_OFFSET,bits, cif7field);
    }

    /** Sets the IF Band Offset of the signal in Hz.
     *  @param val The band offset (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setBandOffsetIF (double val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      int64_t bits = (isNull(val))? INT64_NULL : VRTMath::fromDouble64(20,val);
      setX(IF_OFFSET,bits, cif7field);
    }

    /** Sets the Reference Level of the signal in dBm.
     *  @param val The reference level (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setReferenceLevel (float val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      int16_t bits = (isNull(val))? INT16_NULL : VRTMath::fromFloat16(7,val);
      setI(REF_LEVEL,2,bits, cif7field);
    }

    /** Sets the Stage 1 Gain of the device in dB. This is the front-end gain of the
     *  system. In cases where a separate Stage 1 and Stage 2 gain is not necessary, this holds
     *  the total gain of the system and Gain 2 is set to zero. <i>If no stage 2 gain has been
     *  specified, it will be set to 0 following the call to this method.</i>
     *  @param val The gain (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setGain1 (float val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      if (isNull(val)) {
        setI(GAIN,0,INT16_NULL, cif7field); // clears gain field(s)
      } else {
        int16_t bits = VRTMath::fromFloat16(7,val);
        setI(GAIN,2,bits, cif7field);
      }
    }

    /** Sets the Stage 2 Gain of the device in dB. This is the back-end gain of the
     *  system. In cases where a separate Stage 1 and Stage 2 gain is not necessary, the Stage 1
     *  gain holds the total gain of the system and this is set to zero. <i>If no stage 1 gain
     *  has been specified, it will be set to 0 following the call to this method.</i>
     *  @param val The gain (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setGain2 (float val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      if (isNull(val)) {
        setI(GAIN,0,INT16_NULL, cif7field); // clears gain field(s)
      } else {
        int16_t bits = VRTMath::fromFloat16(7,val);
        setI(GAIN,0,bits, cif7field);
      }
    }

    /** Sets the Sample Rate in Hz.
     *  @param val The sample rate (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setSampleRate (double val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      int64_t bits = (isNull(val))? INT64_NULL : VRTMath::fromDouble64(20,val);
      setX(SAMPLE_RATE,bits, cif7field);
    }

   /** Sets the Sample Period (inverse of Sample Rate) in sec.
     *  @param val The sample period (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setSamplePeriod (double val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      if (isNull(val)) setSampleRate(val, cif7field);
      else             setSampleRate(1.0 / val, cif7field);
    }

    /** Sets the Temperature in degrees Celsius. This is used to convey the temperature of
     *  any component that may affect the described signal.
     *  @param val The temperature (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setTemperature (float val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      int16_t bits = (isNull(val))? INT16_NULL : VRTMath::fromFloat16(6,val);
      setI(TEMPERATURE,2,bits, cif7field);
    }

    /** Gets the Total Gain of the device in dB. The total gain is the sum of
     *  {@link #getGain1()} and {@link #getGain2()}.
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The gain (null if not specified).
     */
    public: inline float getGain (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      float gain1 = getGain1(cif7field);
      float gain2 = getGain2(cif7field);
      return (isNull(gain1))? FLOAT_NAN : gain1+gain2;
    }

    /** Sets the Total Gain of the device in dB. This is effectively the same as
     *  <tt>setGain(val,0)</tt> except that <tt>setGain(null)</tt> will mark both gain values
     *  as unspecified. <i>(Note that it is not possible to specify one of the gain values and
     *  leave the other as unspecified.)</i>
     *  @param val The total gain (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setGain (float val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      if (isNull(val)) setI(GAIN,0,INT16_NULL, cif7field); // clears gain field(s)
      else             setGain(val,0.0f, cif7field);       // sets gain field(s)
    }

    /** Sets the Total Gain of the device in dB.
     *  @param gain1 The stage 1 gain.
     *  @param gain2 The stage 2 gain.
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setGain (float gain1, float gain2, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setGain1(gain1, cif7field);
      setGain2(gain2, cif7field);
    }

    /** Gets the Over-Range Count. This is the count of the number of over-range data
     *  samples in the <b>single</b> paired data packet.
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The count (null if not specified).
     */
    public: inline int64_t getOverRangeCount (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int32_t bits = getL(OVER_RANGE, cif7field);
      return (isNull(bits))? INT64_NULL : (bits & __INT64_C(0xFFFFFFFF));
    }

    /** Sets the Over-Range Count. This is the count of the number of over-range data
     *  samples in the <b>single</b> paired data packet.
     *  @param val The count (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setOverRangeCount (int64_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      if (isNull(val)) setL(OVER_RANGE, INT32_NULL, cif7field);
      else             setL(OVER_RANGE, (int32_t)val, cif7field);
    }

    /** Gets the Device Identifier specifying the manufacturer and model of the device
     *  producing this context packet stream.
     *  @return The device identifier (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline int64_t getDeviceIdentifier (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int64_t bits = getX(DEVICE_ID, cif7field);
      return (isNull(bits))? INT64_NULL : (bits & __INT64_C(0x00FFFFFF0000FFFF));
    }

    /** Sets the Device Identifier specifying the manufacturer and model of the device
     *  producing this context packet stream.
     *  @param val The device identifier (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setDeviceIdentifier (int64_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      if (!isNull(val) && ((val & ~__INT64_C(0x00FFFFFF0000FFFF)) != 0)) {
        throw VRTException("Invalid device identifier");
      }
      setX(DEVICE_ID,val, cif7field);
    }

    /** Gets the Data Packet Payload Format. This specifies the format of the data in the
     *  paired data packet stream.
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The payload format (null if not specified). <i>Note that changes to the returned
     *          object do not alter the packet.</i>
     */
    public: virtual PayloadFormat getDataPayloadFormat (IndicatorFieldEnum_t cif7field=CIF_NULL) const = 0; // can't implement here due to special NULL handling

    /** Sets the Data Packet Payload Format. This specifies the format of the data in the
     *  paired data packet stream.
     *  @param val The payload format (null if not specified). 
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: virtual void setDataPayloadFormat (const PayloadFormat &val, IndicatorFieldEnum_t cif7field=CIF_NULL) = 0; // can't implement here due to special NULL handling

    /* XXX END OF CIF0 FUNCTIONS XXX */

    /* XXX START OF CIF1 FUNCTIONS XXX */

    /** Gets Buffer Size, Level, and Status (See V49.2 spec Section 9.10.7)
     *  Bits 63-32 | Buffer Sz | Size in bytes (assume 32-bit integer)
     *  Bits 31-16 | Reserved  | Fixed value of 0x0
     *  Bits 15-8  | Level     | Indicates buffer utilization: empty <=> 0x0; full <=> (either 0x80 if one-hot, or 0xFF; user-defined)
     *  Bits 7-0   | Status    | Used to indicate buffer underrun/overrun/etc.; user-defined
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Buffer Status (null if not specified)
     */
    public: inline int64_t getBufferStatus (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int64_t bits = getX(BUFFER_SZ, cif7field);
      return (isNull(bits))? INT64_NULL : (bits & __INT64_C(0xFFFFFFFF0000FFFF));
    }

    /** Sets Buffer Size, Level, and Status (See V49.2 spec Section 9.10.7)
     *  Bits 63-32 | Buffer Sz | Size in bytes (assume 32-bit integer)
     *  Bits 31-16 | Reserved  | Fixed value of 0x0
     *  Bits 15-8  | Level     | Indicates buffer utilization: empty <=> 0x0; full <=> (either 0x80 if one-hot, or 0xFF; user-defined)
     *  Bits 7-0   | Status    | Used to indicate buffer underrun/overrun/etc.; user-defined
     *  @param val The Buffer Status (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setBufferStatus (int64_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      if (!isNull(val) && ((val & ~__INT64_C(0xFFFFFFFF0000FFFF)) != 0)) {
        throw VRTException("Invalid Buffer Status field");
      }
      setX(BUFFER_SZ,val, cif7field);
    }

    /** Gets Build Version Information (See V49.2 spec Section 9.10.4)
     *  bits 31-25 | Year     | Year compiled; Values map linearly such that 0 => year 2000, up to 127 => year 2127
     *  bits 24-16 | Day      | Day of year compiled; Valid values are 1..366, where 1 => Jan 1 (? 366 is for leap year/seconds?)
     *  bits 15-10 | Revision | Used to distinguish between versions made on the same day, allowing up to 64 revisions per day
     *  bits 9-0   | User Def | User defined info associated with the build
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Build Version code (null if not specified)
     */
    public: inline int32_t getBuildVersion (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      return getL(VER_BLD_CODE, cif7field);
    }

    /** Sets Build Version Information (See V49.2 spec Section 9.10.4)
     *  bits 31-25 | Year     | Year compiled; Values map linearly such that 0 => year 2000, up to 127 => year 2127
     *  bits 24-16 | Day      | Day of year compiled; Valid values are 1..366, where 1 => Jan 1 (? 366 is for leap year/seconds?)
     *  bits 15-10 | Revision | Used to distinguish between versions made on the same day, allowing up to 64 revisions per day
     *  bits 9-0   | User Def | User defined info associated with the build
     *  @param  val       The Build Version (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Build Version code (null if not specified)
     */
    public: inline void setBuildVersion (int32_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setL(VER_BLD_CODE,val, cif7field);
    }

    /** Gets V49 Standard and Spec Compliance Number (See V49.2 spec Section 9.10.3)
     *  0x00000001 => V49.0
     *  0x00000002 => V49.1
     *  0x00000003 => V49A (which implies V49.0 and V49.1)
     *  0x00000004 => V49.2
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The V49 spec version (null if not specified)
     */
    public: inline int32_t getV49SpecVersion (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      return getL(V49_COMPL, cif7field);
    }

    /** Sets V49 Standard and Spec Compliance Number (See V49.2 spec Section 9.10.3)
     *  0x00000001 => V49.0
     *  0x00000002 => V49.1
     *  0x00000003 => V49A (which implies V49.0 and V49.1)
     *  0x00000004 => V49.2
     *  @param val The V49 spec version (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setV49SpecVersion (int32_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setL(V49_COMPL,val, cif7field);
    }

    /** Gets Health Status (See V49.2 spec Section 9.10.2)
     *  uses lower 16 bits of 32 bit word
     *  values are user defined and mapped to a defined health state
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Health Status value (null if not specified)
     */
    public: inline int16_t getHealthStatus (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int16_t bits = getI(HEALTH_STATUS, 2, cif7field);
      return (isNull(bits))? INT16_NULL : bits;
    }

    /** Sets Health Status (See V49.2 spec Section 9.10.2)
     *  uses lower 16 bits of 32 bit word
     *  values are user defined and mapped to a defined health state
     *  @param val The Health Status value (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setHealthStatus (int16_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setI(HEALTH_STATUS, 2, val, cif7field);
    }

    /** Gets Discrete IO 64-bit Field (See V49.2 spec Section 9.11)
     *  All bits are user-defined
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Discrete IO 64-bit field (null if not specified)
     */
    public: inline int64_t getDiscreteIO64 (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int64_t bits = getX(DISCRETE_IO64, cif7field);
      return (isNull(bits))? INT64_NULL : bits;
    }

    /** Sets Discrete IO 64-bit Field (See V49.2 spec Section 9.11)
     *  All bits are user-defined
     *  @param val The Discrete IO 64-bit field (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setDiscreteIO64 (int64_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setX(DISCRETE_IO64,val, cif7field);
    }

    /** Gets Discrete IO 32-bit Field (See V49.2 spec Section 9.11)
     *  All bits are user-defined
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Discrete IO 32-bit field (null if not specified)
     */
    public: inline int32_t getDiscreteIO32 (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      return getL(DISCRETE_IO32, cif7field);
    }

    /** Sets Discrete IO 32-bit Field (See V49.2 spec Section 9.11)
     *  All bits are user-defined
     *  @param val The Discrete IO 32-bit field (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setDiscreteIO32 (int32_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setL(DISCRETE_IO32,val, cif7field);
    }

    /** Gets the Index Field List. 
     *  IndexFieldList type
     *  (See V49.2 spec Section 9.3.2)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Index Field List (null if not specified). <i>Note that changes to the
     *          returned object do not alter the packet.</i>
     */
    public: virtual IndexFieldList getIndexList (IndicatorFieldEnum_t cif7field=CIF_NULL) const = 0;

    /** Sets the Index Field List.
     *  IndexFieldList type
     *  (See V49.2 spec Section 9.3.2)
     *  @param val The Index Field List (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setIndexList (const IndexFieldList &val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setRecord(INDEX_LIST, val, getIndexList(), cif7field);
    }

    /** Gets the Sector Scan/Step Field. 
     *  Array-of-Records type
     *  (See V49.2 spec Section 9.6.2)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Sector Scan/Step Array-of-Records (null if not specified).
     *  <i>Note that changes to the returned object do not alter the packet.</i>
     */
    public: virtual ArrayOfRecords getSectorScanStep (IndicatorFieldEnum_t cif7field=CIF_NULL) const = 0;

    /** Sets the Sector Scan/Step Field.
     *  Array-of-Records type
     *  (See V49.2 spec Section 9.6.2)
     *  @param val The Sector Scan/Step Array-of-Records (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setSectorScanStep (const ArrayOfRecords &val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setRecord(SECTOR_SCN_STP, val, getSectorScanStep(), cif7field);
    }

    /** Gets the CIFs Array 
     *  Array-of-Records type
     *  (See V49.2 spec Section 9.13.1)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The CIFs Array as an Array-of-Records (null if not specified).
     *  <i>Note that changes to the returned object do not alter the packet.</i>
     */
    public: virtual ArrayOfRecords getCIFsArray (IndicatorFieldEnum_t cif7field=CIF_NULL) const = 0;

    /** Sets the CIFs Array
     *  Array-of-Records type
     *  (See V49.2 spec Section 9.13.1)
     *  @param val The CIFs Array as an Array-of-Records (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setCIFsArray (const ArrayOfRecords &val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setRecord(CIFS_ARRAY, val, getCIFsArray(), cif7field);
    }

    /** Gets the Spectrum Field 
     *  SpectrumField type
     *  (See V49.2 spec Section 9.6.1)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Spectrum Field (null if not specified).
     *  <i>Note that changes to the returned object do not alter the packet.</i>
     */
    public: virtual SpectrumField getSpectrumField (IndicatorFieldEnum_t cif7field=CIF_NULL) const = 0;

    /** Sets the Spectrum Field
     *  SpectrumField type
     *  (See V49.2 spec Section 9.6.1)
     *  @param val The Spectrum Field (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setSpectrumField (const SpectrumField &val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setRecord(SPECTRUM, val, getSpectrumField(), cif7field);
    }


    /** Gets the Auxiliary Bandwidth of the signal in Hz.
     *  (See V49.2 spec Section 9.5.16)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The aux bandwidth (null if not specified).
     */
    public: inline double getAuxBandwidth (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int64_t bits = getX(AUX_BANDWIDTH, cif7field);
      return (isNull(bits))? DOUBLE_NAN : VRTMath::toDouble64(20,bits);
    }

    /** Sets the Auxiliary Bandwidth of the signal in Hz.
     *  (See V49.2 spec Section 9.5.16)
     *  @param val The aux bandwidth (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setAuxBandwidth (double val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      int64_t bits = (isNull(val))? INT64_NULL : VRTMath::fromDouble64(20,val);
      setX(AUX_BANDWIDTH,bits, cif7field);
    }

    /** Gets the Stage 1 Auxiliary Gain of the device in dB.
     *  This is a second front-end gain control/metadata field for control and
     *  reporting on anciliary hardware or processing in a single chain
     *  described by a single Stream ID.
     *  In cases where a separate Stage 1 and Stage 2 gain is not necessary,
     *  this holds the total aux gain of the system and Stage 2 Auxiliary Gain
     *  is set to zero.
     *  (See V49.2 spec Section 9.5.15)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The stage 1 aux gain (null if not specified).
     */
    public: inline float getAuxGain1 (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int16_t bits = getI(AUX_GAIN,2, cif7field);
      return (isNull(bits))? FLOAT_NAN : VRTMath::toFloat16(7,bits);
    }

    /** Gets the Stage 2 Auxiliary Gain of the device in dB.
     *  This is a second back-end gain control/metadata field for control and
     *  reporting on anciliary hardware or processing in a single chain
     *  described by a single Stream ID.
     *  In cases where a separate Stage 1 and Stage 2 gain is not necessary,
     *  Stage 1 Auxiliary Gain holds the total aux gain of the system and this
     *  is set to zero.
     *  (See V49.2 spec Section 9.5.15)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The stage 2 aux gain (null if not specified).
     */
    public: inline float getAuxGain2 (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int16_t bits = getI(AUX_GAIN,0, cif7field);
      return (isNull(bits))? FLOAT_NAN : VRTMath::toFloat16(7,bits);
    }

    /** Gets the Total Auxiliary Gain of the device in dB. The total aux gain is
     *  the sum of {@link #getAuxGain1()} and {@link #getAuxGain2()}.
     *  (See V49.2 spec Section 9.5.15)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The total aux gain (null if not specified).
     */
    public: inline float getAuxGain (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      float gain1 = getAuxGain1(cif7field);
      float gain2 = getAuxGain2(cif7field);
      return (isNull(gain1))? FLOAT_NAN : gain1+gain2;
    }

    /** Sets the Stage 1 Auxiliary Gain of the device in dB.
     *  This is a second front-end gain control/metadata field for control and
     *  reporting on anciliary hardware or processing in a single chain
     *  described by a single Stream ID.
     *  In cases where a separate Stage 1 and Stage 2 gain is not necessary,
     *  this holds the total aux gain of the system and Stage 2 Auxiliary Gain
     *  is set to zero. <i>If no Stage 2 Auxiliary Gain has been specified, it
     *  will be set to 0 following the call to this method.</i>
     *  (See V49.2 spec Section 9.5.15)
     *  @param val The stage 1 aux gain (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setAuxGain1 (float val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      if (isNull(val)) {
        setI(AUX_GAIN,0,INT16_NULL, cif7field); // clears aux gain field(s)
      } else {
        int16_t bits = VRTMath::fromFloat16(7,val);
        setI(AUX_GAIN,2,bits, cif7field);
      }
    }

    /** Sets the Stage 2 Auxiliary Gain of the device in dB.
     *  This is a second back-end gain control/metadata field for control and
     *  reporting on anciliary hardware or processing in a single chain
     *  described by a single Stream ID.
     *  In cases where a separate Stage 1 and Stage 2 gain is not necessary,
     *  Stage 1 Auxiliary Gain holds the total aux gain of the system and this
     *  is set to zero. <i>If no Stage 1 Auxiliary Gain has been specified, it
     *  will be set to 0 following the call to this method.</i>
     *  (See V49.2 spec Section 9.5.15)
     *  @param val The stage 2 aux gain (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setAuxGain2 (float val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      if (isNull(val)) {
        setI(AUX_GAIN,0,INT16_NULL, cif7field); // clears aux gain field(s)
      } else {
        int16_t bits = VRTMath::fromFloat16(7,val);
        setI(AUX_GAIN,0,bits, cif7field);
      }
    }

    /** Sets the Total Auxiliary Gain of the device in dB. This is effectively the same as
     *  <tt>setAuxGain(val,0)</tt> except that <tt>setAuxGain(null)</tt> will mark both aux gain values
     *  as unspecified. <i>(Note that it is not possible to specify one of the aux gain values and
     *  leave the other as unspecified.)</i>
     *  (See V49.2 spec Section 9.5.15)
     *  @param val The total aux gain (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setAuxGain (float val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      if (isNull(val)) setI(AUX_GAIN,0,INT16_NULL, cif7field); // clears gain field(s)
      else             setAuxGain(val,0.0f, cif7field);       // sets gain field(s)
    }

    /** Sets the Total Auxiliary Gain of the device in dB.
     *  (See V49.2 spec Section 9.5.15)
     *  @param gain1 The stage 1 aux gain.
     *  @param gain2 The stage 2 aux gain.
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setAuxGain (float gain1, float gain2, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setAuxGain1(gain1, cif7field);
      setAuxGain2(gain2, cif7field);
    }

    /** Gets the Auxiliary Frequency of the signal in Hz.
     *  (See V49.2 spec Section 9.5.14)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The aux frequency (null if not specified).
     */
    public: inline double getAuxFrequency (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int64_t bits = getX(AUX_FREQUENCY, cif7field);
      return (isNull(bits))? DOUBLE_NAN : VRTMath::toDouble64(20,bits);
    }

    /** Sets the Auxiliary Frequency of the signal in Hz.
     *  (See V49.2 spec Section 9.5.14)
     *  @param val The aux frequency (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setAuxFrequency (double val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      int64_t bits = (isNull(val))? INT64_NULL : VRTMath::fromDouble64(20,val);
      setX(AUX_FREQUENCY,bits, cif7field);
    }

    // XXX SNR and Noise Figure share a 32-bit word. Setting either to null will set both to null.
    //     The same is true for gain1 and gain2 (as well as aux gain1,2)
    //     and 2nd,3rd order intercept points

    /** Gets the Signal-to-Noise Ratio, expressed in decibels.
     *  (See V49.2 spec Section 9.5.7)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The SNR (null if not specified).
     */
    public: inline float getSNR (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int16_t bits = getI(SNR_NOISE,0, cif7field);
      return (isNull(bits))? FLOAT_NAN : VRTMath::toFloat16(7,bits);
    }

    /** Gets the Noise Figure of the receiver, expressed in decibels.
     *  The Noise Figure of a receiver is a measure of the reduction in the SNR
     *  between the input of the receiver (the antenna) to the output of the RF
     *  processing chain.
     *  (See V49.2 spec Section 9.5.7)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Noise Figure (null if not specified).
     */
    public: inline float getNoiseFigure (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int16_t bits = getI(SNR_NOISE,2, cif7field);
      return (isNull(bits))? FLOAT_NAN : VRTMath::toFloat16(7,bits);
    }

    /** Sets the Signal-to-Noise Ratio, expressed in decibels.
     *  <i>If no Noise Figure has been specified, it will be set to 0 following
     *  the call to this method.</i>
     *  <i>If set to null, both SNR and Noise Figure will be set to null.</i>
     *  (See V49.2 spec Section 9.5.7)
     *  @param val The SNR (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setSNR (float val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      if (isNull(val)) {
        setL(SNR_NOISE, INT32_NULL, cif7field);
      } else {
        int16_t bits = VRTMath::fromFloat16(7,val);
        setI(SNR_NOISE,0,bits, cif7field);
      }
    }

    /** Sets the Noise Figure of the receiver, expressed in decibels.
     *  The Noise Figure of a receiver is a measure of the reduction in the SNR
     *  between the input of the receiver (the antenna) to the output of the RF
     *  processing chain.
     *  <i>If no SNR has been specified, it will be set to 0 following the call
     *  to this method.</i>
     *  <i>If set to null, both SNR and Noise Figure will be set to null.</i>
     *  (See V49.2 spec Section 9.5.7)
     *  @param val The Noise Figure (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setNoiseFigure (float val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      if (isNull(val)) {
        setL(SNR_NOISE, INT32_NULL, cif7field);
      } else {
        int16_t bits = VRTMath::fromFloat16(7,val);
        setI(SNR_NOISE,2,bits, cif7field);
      }
    }

    /** Gets the Second-order Input Intercept Point (IIP2).
     *  (See V49.2 spec Section 9.5.6)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The IIP2 (null if not specified).
     */
    public: inline float getSecondOrderInputInterceptPoint (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int16_t bits = getI(ICPT_PTS_2_3,0, cif7field);
      return (isNull(bits))? FLOAT_NAN : VRTMath::toFloat16(7,bits);
    }

    /** Gets the Third-order Input Intercept Point (IIP3).
     *  (See V49.2 spec Section 9.5.6)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The IIP3 (null if not specified).
     */
    public: inline float getThirdOrderInputInterceptPoint (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int16_t bits = getI(ICPT_PTS_2_3,2, cif7field);
      return (isNull(bits))? FLOAT_NAN : VRTMath::toFloat16(7,bits);
    }

    /** Sets the Second-order Input Intercept Point (IIP2).
     *  <i>If no IIP3 has been specified, it will be set to 0 following the call
     *  to this method.</i>
     *  <i>If set to null, both IIP2 and IIP3 will be set to null.</i>
     *  (See V49.2 spec Section 9.5.6)
     *  @param val The IIP2 (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setSecondOrderInputInterceptPoint (float val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      if (isNull(val)) {
        setL(ICPT_PTS_2_3, INT32_NULL, cif7field);
      } else {
        int16_t bits = VRTMath::fromFloat16(7,val);
        setI(ICPT_PTS_2_3,0,bits, cif7field);
      }
    }

    /** Sets the Third-order Input Intercept Point (IIP3).
     *  <i>If no IIP2 has been specified, it will be set to 0 following the call
     *  to this method.</i>
     *  <i>If set to null, both IIP2 and IIP3 will be set to null.</i>
     *  (See V49.2 spec Section 9.5.6)
     *  @param val The IIP3 (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setThirdOrderInputInterceptPoint (float val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      if (isNull(val)) {
        setL(ICPT_PTS_2_3, INT32_NULL, cif7field);
      } else {
        int16_t bits = VRTMath::fromFloat16(7,val);
        setI(ICPT_PTS_2_3,2,bits, cif7field);
      }
    }

    /** Gets 1-dB Compression Point.
     *  (See V49.2 spec Section 9.5.2)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The 1-dB Compression Point (null if not specified).
     */
    public: inline float getOneDecibelCompressionPoint (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int16_t bits = getI(COMPRESS_PT,2, cif7field);
      return (isNull(bits))? FLOAT_NAN : VRTMath::toFloat16(7,bits);
    }

    /** Sets the 1-dB Compression Point.
     *  (See V49.2 spec Section 9.5.2)
     *  @param val The 1-dB Compression Point (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setOneDecibelCompressionPoint  (float val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      if (isNull(val)) {
        setL(COMPRESS_PT, INT32_NULL, cif7field);
      } else {
        int16_t bits = VRTMath::fromFloat16(7,val);
        setI(COMPRESS_PT,2,bits, cif7field);
      }
    }

    /** Gets the Stage 1 Threshold.
     *  In cases where Stage 1 and Stage 2 thresholds are not necessary, this
     *  holds the single threshold value and Stage 2 Threshold is set to zero.
     *  (See V49.2 spec Section 9.5.13)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The stage 1 threshold (null if not specified).
     */
    public: inline float getThreshold1 (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int16_t bits = getI(THRESHOLD,2, cif7field);
      return (isNull(bits))? FLOAT_NAN : VRTMath::toFloat16(7,bits);
    }

    /** Gets the Stage 2 Threshold.
     *  In cases where Stage 1 and Stage 2 thresholds are not necessary, this
     *  shall be set to zero and Stage 1 Threshold holds the single threshold
     *  value.
     *  (See V49.2 spec Section 9.5.13)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The stage 2 threshold (null if not specified).
     */
    public: inline float getThreshold2 (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int16_t bits = getI(THRESHOLD,0, cif7field);
      return (isNull(bits))? FLOAT_NAN : VRTMath::toFloat16(7,bits);
    }

    /** Sets the Stage 1 Threshold.
     *  In cases where Stage 1 and Stage 2 thresholds are not necessary, this
     *  holds the single threshold value and Stage 2 Threshold is set to zero.
     *  <i>If no Stage 2 Threshold has been specified, it will be set to zero
     *  following the call to this method.</i>
     *  (See V49.2 spec Section 9.5.13)
     *  @param val The stage 1 threshold (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setThreshold1 (float val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      int16_t bits = VRTMath::fromFloat16(7,val);
      setI(THRESHOLD,2,bits, cif7field);
    }

    /** Sets the Stage 2 Threshold.
     *  In cases where Stage 1 and Stage 2 thresholds are not necessary, this
     *  shall be set to zero and Stage 1 Threshold holds the single threshold
     *  value.
     *  <i>If no Stage 1 Threshold has been specified, it will be set to zero
     *  following the call to this method.</i>
     *  (See V49.2 spec Section 9.5.13)
     *  @param val The stage 2 threshold (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setThreshold2 (float val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      int16_t bits = VRTMath::fromFloat16(7,val);
      setI(THRESHOLD,0,bits, cif7field);
    }

    /** Sets the Stage 1 Threshold. This is effectively the same as
     *  <tt>setThreshold(val,0)</tt> except that <tt>setThreshold(null)</tt>
     *  will mark both threshold values as unspecified.
     *  <i>(Note that it is not possible to specify one of the threshold values and
     *  leave the other as unspecified.)</i>
     *  (See V49.2 spec Section 9.5.13)
     *  @param val The total threshold (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setThreshold (float val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      if (isNull(val)) setI(THRESHOLD,0,INT16_NULL, cif7field); // clears threshold field(s)
      else             setThreshold(val,0.0f, cif7field);       // sets threshold field(s)
    }

    /** Sets the Stage 1 and Stage 2 Threshold subfields.
     *  (See V49.2 spec Section 9.5.13)
     *  @param threshold1 The stage 1 threshold.
     *  @param threshold2 The stage 2 threshold.
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setThreshold (float threshold1, float threshold2, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setThreshold1(threshold1, cif7field);
      setThreshold2(threshold2, cif7field);
    }

    /** Gets the Energy per Bit to Noise Density ratio (Eb/N0).
     *  Valid range is from -256dB to +255.984375dB (= 0x7FFE), with the maximum
     *  positive value (0x7FFF) designating the "Eb/N0 not used" case.
     *  (See V49.2 spec Section 9.5.17)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Eb/N0 (null if not specified).
     */
    public: inline float getEbN0 (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int16_t bits = getI(EB_NO_BER,0, cif7field);
      return (isNull(bits))? FLOAT_NAN : VRTMath::toFloat16(7,bits);
    }

    /** Gets the Bit Error Rate (BER).
     *  Valid range is from -256dB to +255.984375dB (= 0x7FFE), with the maximum
     *  positive value (0x7FFF) designating the "BER not used" case.
     *  (See V49.2 spec Section 9.5.17)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The BER (null if not specified).
     */
    public: inline float getBitErrorRate (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int16_t bits = getI(EB_NO_BER,2, cif7field);
      return (isNull(bits))? FLOAT_NAN : VRTMath::toFloat16(7,bits);
    }

    /** Sets the Energy per Bit to Noise Density ratio (Eb/N0).
     *  Valid range is from -256dB to +255.984375dB (= 0x7FFE), with the maximum
     *  positive value (0x7FFF) designating the "Eb/N0 not used" case.
     *  <i>If no BER has been specified, it will be set to zero following the
     *  call to this method.</i>
     *  <i>If set to null, both Eb/N0 and BER will be set to null.</i>
     *  (See V49.2 spec Section 9.5.17)
     *  @param val The Eb/N0 (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setEbN0 (float val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      if (isNull(val)) {
        setL(EB_NO_BER, INT32_NULL, cif7field);
      } else {
        int16_t bits = VRTMath::fromFloat16(7,val);
        setI(EB_NO_BER,0,bits, cif7field);
      }
    }

    /** Sets the Bit Error Rate (BER).
     *  Valid range is from -256dB to +255.984375dB (= 0x7FFE), with the maximum
     *  positive value (0x7FFF) designating the "BER not used" case.
     *  <i>If no Eb/N0 has been specified, it will be set to zero following the
     *  call to this method.</i>
     *  <i>If set to null, both Eb/N0 and BER will be set to null.</i>
     *  (See V49.2 spec Section 9.5.17)
     *  @param val The BER (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setBitErrorRate (float val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      if (isNull(val)) {
        setL(EB_NO_BER, INT32_NULL, cif7field);
      } else {
        int16_t bits = VRTMath::fromFloat16(7,val);
        setI(EB_NO_BER,2,bits, cif7field);
      }
    }

    /** Gets Range.
     *  (See V49.2 spec Section 9.4.10)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The range (null if not specified)
     */
    public: inline double getRange (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int32_t bits = getL(RANGE, cif7field);
      // XXX if bits==INT32_NULL, could be valid value or could be indicating Range is not set
      if (isNull(bits) && getOffset(RANGE, cif7field) < 0) return DOUBLE_NAN;
      return (bits == 0x7FFFFFFF)? DOUBLE_NAN : toDouble32(6, bits);
    }

    /** Sets Range.
     *  (See V49.2 spec Section 9.4.10)
     *  @param val The range (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setRange (double val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      int32_t bits = (isNull(val))? 0x7FFFFFFF : fromDouble32(6, val);
      setL(RANGE,bits, cif7field);
    }

    /** Sets the Horizontal and Vertical Beamwidth in degrees.
     *  Valid range is from 0 to 360 degrees.
     *  Radix point to the right of bit 7 for each 16-bit number
     *  fractional resolution: 0.0078125 (1/128) degrees
     *  Bits 31..16: Horizontal Degrees
     *  Bits 15..0:  Vertical Degrees
     *  (See V49.2 spec Section 9.4.2)
     *  @param hval The Horizontal Beamwidth (null will override vval).
     *  @param vval The Vertical Beamwidth (null will override hval).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setBeamwidth (float hval, float vval, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      if (isNull(hval) || isNull(vval)) {
        setL(BEAMWIDTH, INT32_NULL, cif7field);
      } else {
        int32_t hbits = VRTMath::fromFloat32(7,hval);
        int32_t vbits = VRTMath::fromFloat32(7,vval);
        int32_t bits = (hbits<<16) | (vbits&0xFFFF);
        setL(BEAMWIDTH, bits, cif7field);
      }
    }

    /** Gets the Horizontal Beamwidth in degrees. Valid range is from 0 to 360 degrees.
     *  (See V49.2 spec Section 9.4.2)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Horizontal Beamwidth (null if not specified).
     */
    public: inline float getHorizontalBeamwidth (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int32_t bits = getL(BEAMWIDTH, cif7field);
      return (isNull(bits))? FLOAT_NAN : VRTMath::toFloat32(7,(bits>>16)&0xFFFF);
    }

    /** Sets the Horizontal Beamwidth in degrees. Valid range is from 0 to 360 degrees.
     *  (See V49.2 spec Section 9.4.2)
     *  @param val The Horizontal Beamwidth (null if not specified, which will override
     *             Vertical Beamwidth as well).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setHorizontalBeamwidth (float val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      if (isNull(val)) {
        setL(BEAMWIDTH, INT32_NULL, cif7field);
      } else {
        int16_t bits = (int16_t)(VRTMath::fromFloat32(7,val)&0xFFFF);
        setI(BEAMWIDTH,0,bits, cif7field);
      }
    }

    /** Gets the Vertical Beamwidth in degrees. Valid range is from 0 to 360 degrees.
     *  (See V49.2 spec Section 9.4.2)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Vertical Beamwidth (null if not specified).
     */
    public: inline float getVerticalBeamwidth (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int32_t bits = getL(BEAMWIDTH, cif7field);
      return (isNull(bits))? FLOAT_NAN : VRTMath::toFloat32(7,bits&0xFFFF);
    }

    /** Sets the Vertical Beamwidth in degrees. Valid range is from 0 to 360 degrees.
     *  (See V49.2 spec Section 9.4.2)
     *  @param val The Vertical Beamwidth (null if not specified, which will override
     *             Horizontal Beamwidth as well).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setVerticalBeamwidth (float val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      if (isNull(val)) {
        setL(BEAMWIDTH, INT32_NULL, cif7field);
      } else {
        int16_t bits = (int16_t)(VRTMath::fromFloat32(7,val)&0xFFFF);
        setI(BEAMWIDTH,2,bits, cif7field);
      }
    }

    /** Gets Spatial Reference Type.
     *  Spatial Reference Type uses the Generic32 bit Identifier field
     *  See V49.2 spec Section 9.8.11 for Spatial Reference Type
     *  See V49.2 spec Section 9.8 for Generic32 bit Identifier field
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Spatial Reference Type (null if not specified)
     */
    public: inline int32_t getSpatialReferenceType (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      return getL(SPATIAL_REF_TYPE, cif7field);
    }

    /** Sets Spatial Reference Type.
     *  Spatial Reference Type uses the Generic32 bit Identifier field
     *  See V49.2 spec Section 9.8.11 for Spatial Reference Type
     *  See V49.2 spec Section 9.8 for Generic32 bit Identifier field
     *  @param val The Spatial Reference Type (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setSpatialReferenceType (int32_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setL(SPATIAL_REF_TYPE, val, cif7field);
    }

    /** Gets the 3D Pointing Vector (Structured)
     *  See V49.2 spec Section 9.4.1.
     *  Array-of-Records format
     *  HeaderSize is either 0 or 1
     *    - Note: docs say 3 or 4, but that's inconsistent with definition of ArrayOfRecords, so ignoring that guidance
     *            3 or 4 must include the required 3 words of every ArrayOfRecords, which is incorrect.
     *    - depends on whether optional App-Specific Header is included, which is a 32-bit word if so
     *    - optional App-Specific Header is a *Global* Index/Ref/Beam subfield
     *  NumWords/Record is either 1 or 2
     *    - depends on whether optional Index/Ref/Beam subfield is included in each record
     *  Bitmapped CIF subfield only uses bits 31 and 30
     *    - Bit 31: Set when each record includes the optional Index/Ref/Beam subfield
     *    - Bit 30: Always set; Indicates presence of 3D Pointing Vector subfield in each record, which is required
     *  App-Specific header is optional 32-bit word
     *    - as mentioned above, this is a *Global* Index/Ref/Beam subfield
     *  Each Record is at most 2 words
     *    - First 32-bit word is optional, and is the Index/Ref/Beam subfield
     *    - Second 32-bit word is required, and is the 3D Pointing Vector subfield
     *  Index/Ref/Beam subfield
     *    - Bits 31..16: Record Index (optional; 0 when global or unused in records)
     *    - Bits 15..4: Reserved
     *    - Bits 3..2: Reference (00 not specified; 01 ECEF; 10 Platform centered; 11 Array centered)
     *    - Bits 1..0: Beam (00 not specified; 01 Beam or signal direction; 10 Null; 11 reserved)
     *  3D Pointing Vector subfield
     *    - Bits 31..16: Elevation angle in degrees; radix point to the right of bit 23; range [-90,90]
     *    - Bits 15..0: Azimuthal angle in degrees; radix point to the right of bit 7; range [0,512)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return 3D Pointing Vector as an Array-of-Records (null if not specified).
     *  <i>Note that changes to the returned object do not alter the packet.</i>
     */
    public: virtual ArrayOfRecords get3DPointingVectorStructured (IndicatorFieldEnum_t cif7field=CIF_NULL) const = 0;

    /** Sets the 3D Pointing Vector (Structured)
     *  See V49.2 spec Section 9.4.1.
     *  Array-of-Records format
     *  HeaderSize is either 0 or 1
     *    - Note: docs say 3 or 4, but that's inconsistent with definition of ArrayOfRecords, so ignoring that guidance
     *            3 or 4 must include the required 3 words of every ArrayOfRecords, which is incorrect.
     *    - depends on whether optional App-Specific Header is included, which is a 32-bit word if so
     *    - optional App-Specific Header is a *Global* Index/Ref/Beam subfield
     *  NumWords/Record is either 1 or 2
     *    - depends on whether optional Index/Ref/Beam subfield is included in each record
     *  Bitmapped CIF subfield only uses bits 31 and 30
     *    - Bit 31: Set when each record includes the optional Index/Ref/Beam subfield
     *    - Bit 30: Always set; Indicates presence of 3D Pointing Vector subfield in each record, which is required
     *  App-Specific header is optional 32-bit word
     *    - as mentioned above, this is a *Global* Index/Ref/Beam subfield
     *  Each Record is at most 2 words
     *    - First 32-bit word is optional, and is the Index/Ref/Beam subfield
     *    - Second 32-bit word is required, and is the 3D Pointing Vector subfield
     *  Index/Ref/Beam subfield
     *    - Bits 31..16: Record Index (optional; 0 when global or unused in records)
     *    - Bits 15..4: Reserved
     *    - Bits 3..2: Reference (00 not specified; 01 ECEF; 10 Platform centered; 11 Array centered)
     *    - Bits 1..0: Beam (00 not specified; 01 Beam or signal direction; 10 Null; 11 reserved)
     *  3D Pointing Vector subfield
     *    - Bits 31..16: Elevation angle in degrees; radix point to the right of bit 23; range [-90,90]
     *    - Bits 15..0: Azimuthal angle in degrees; radix point to the right of bit 7; range [0,512)
     *  @param val The 3D Pointing Vector as an Array-of-Records (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void set3DPointingVectorStructured (const ArrayOfRecords &val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setRecord(PNT_VECT_3D_ST, val, get3DPointingVectorStructured(), cif7field);
    }

    /** Gets the Elevation Angle subfield of the Single-word 3D Pointing Vector.
     *  (See V49.2 spec Section 9.4.1)
     *    - Bits 31..16: Elevation angle in degrees; radix point to the right of bit 23; range [-90,90]
     *    - Bits 15..0: Azimuthal angle in degrees; radix point to the right of bit 7; range [0,512)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Elevation Angle (null if not specified).
     */
    public: inline float get3DPointingVectorElevation (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int16_t bits = getI(PNT_VECT_3D_SI,0, cif7field);
      return (isNull(bits))? FLOAT_NAN : VRTMath::toFloat16(7,bits);
    }

    /** Gets the Azimuthal Angle subfield of the Single-word 3D Pointing Vector.
     *  (See V49.2 spec Section 9.4.1)
     *    - Bits 31..16: Elevation angle in degrees; radix point to the right of bit 23; range [-90,90]
     *    - Bits 15..0: Azimuthal angle in degrees; radix point to the right of bit 7; range [0,512)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Azimuthal Angle (null if not specified).
     */
    public: inline float get3DPointingVectorAzimuth (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int16_t bits = getI(PNT_VECT_3D_SI,2, cif7field);
      if (isNull(bits)) return FLOAT_NAN;
      float val = VRTMath::toFloat16(7,bits);
      return (val < 0)? 512.0+val : val;
    }

    /** Sets the Elevation Angle subfield of the Single-word 3D Pointing Vector.
     *  <i>If no Azimuthal Angle has been specified, it will be set to zero
     *  following the call to this method.</i>
     *  <i>If set to null, both Elevation and Azimuthal Angles will be set to 
     *  null.</i>
     *  (See V49.2 spec Section 9.4.1)
     *    - Bits 31..16: Elevation angle in degrees; radix point to the right of bit 23; range [-90,90]
     *    - Bits 15..0: Azimuthal angle in degrees; radix point to the right of bit 7; range [0,512)
     *  @param val The Elevation Angle (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void set3DPointingVectorElevation (float val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      if (isNull(val)) {
        setL(PNT_VECT_3D_SI, INT32_NULL, cif7field);
      } else {
        int16_t bits = VRTMath::fromFloat16(7,val);
        setI(PNT_VECT_3D_SI,0,bits, cif7field);
      }
    }

    /** Sets the Azimuthal Angle subfield of the Single-word 3D Pointing Vector.
     *  <i>If no Elevation Angle has been specified, it will be set to zero 
     *  following the call to this method.</i>
     *  <i>If set to null, both Elevation and Azimuthal Angles will be set to 
     *  null.</i>
     *  (See V49.2 spec Section 9.4.1)
     *    - Bits 31..16: Elevation angle in degrees; radix point to the right of bit 23; range [-90,90]
     *    - Bits 15..0: Azimuthal angle in degrees; radix point to the right of bit 7; range [0,512)
     *  @param val The Azimuthal Angle (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void set3DPointingVectorAzimuth (float val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      if (isNull(val)) {
        setL(PNT_VECT_3D_SI, INT32_NULL, cif7field);
      } else {
        if (val > 255.984375) val = val-512;
        int16_t bits = VRTMath::fromFloat16(7,val);
        setI(PNT_VECT_3D_SI,2,bits, cif7field);
      }
    }

    /** Gets the Polarization Tilt Angle in units of Radians.
     *  (See V49.2 spec Section 9.4.8)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Tilt Angle (null if not specified).
     */
    public: inline float getPolarizationTiltAngle (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int16_t bits = getI(POLARIZATION,0, cif7field);
      return (isNull(bits))? FLOAT_NAN : VRTMath::toFloat16(13,bits);
    }

    /** Gets the Polarization Ellipticity Angle in units of Radians.
     *  (See V49.2 spec Section 9.4.8)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Ellipticity Angle (null if not specified).
     */
    public: inline float getPolarizationEllipticityAngle (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int16_t bits = getI(POLARIZATION,2, cif7field);
      return (isNull(bits))? FLOAT_NAN : VRTMath::toFloat16(13,bits);
    }

    /** Sets the Polarization Tilt Angle in units of Radians.
     *  <i>If no Ellipticity Angle has been specified, it will be set to zero
     *  following the call to this method.</i>
     *  <i>If set to null, both Tilt and Ellipticity Angles will be set to 
     *  null.</i>
     *  (See V49.2 spec Section 9.4.8)
     *  @param val The Tilt Angle (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setPolarizationTiltAngle (float val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      if (isNull(val)) {
        setL(POLARIZATION, INT32_NULL, cif7field);
      } else {
        int16_t bits = VRTMath::fromFloat16(13,val);
        setI(POLARIZATION,0,bits, cif7field);
      }
    }

    /** Sets the Polarization Ellipticity Angle in units of Radians.
     *  <i>If no Tilt Angle has been specified, it will be set to zero 
     *  following the call to this method.</i>
     *  <i>If set to null, both Tilt and Ellipticity Angles will be set to 
     *  null.</i>
     *  (See V49.2 spec Section 9.4.8)
     *  @param val The Ellipticity Angle (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setPolarizationEllipticityAngle (float val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      if (isNull(val)) {
        setL(POLARIZATION, INT32_NULL, cif7field);
      } else {
        int16_t bits = VRTMath::fromFloat16(13,val);
        setI(POLARIZATION,2,bits, cif7field);
      }
    }

    /** Gets the Phase Offset.
     *  Phase Offset is measured from the Reference Point to the Described
     *  Signal. The unit of measure for the reference level is the angle
     *  measure in units of radians. This is sometimes called Phase Difference.
     *  (See V49.2 spec Section 9.5.8)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Phase Offset (null if not specified).
     */
    public: inline float getPhaseOffset (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int16_t bits = getI(PHASE,2, cif7field);
      return (isNull(bits))? FLOAT_NAN : VRTMath::toFloat16(7,bits);
    }

    /** Sets the Phase Offset.
     *  Phase Offset is measured from the Reference Point to the Described
     *  Signal. The unit of measure for the reference level is the angle
     *  measure in units of radians. This is sometimes called Phase Difference.
     *  (See V49.2 spec Section 9.5.8)
     *  @param val The Phase Offset (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setPhaseOffset (float val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      if (isNull(val)) {
        setL(PHASE, INT32_NULL, cif7field);
      } else {
        int16_t bits = VRTMath::fromFloat16(7,val);
        setI(PHASE,2,bits, cif7field);
      }
    }


    /* XXX END OF CIF1 FUNCTIONS XXX */

    /* XXX START OF CIF2 FUNCTIONS XXX */
    
    /** Gets Spatial Scan Type.
     *  Spatial Scan Type uses the Generic16 bit Identifier field
     *  See V49.2 spec Section 9.8.11 for Spatial Scan Type
     *  See V49.2 spec Section 9.8 for Generic16 bit Identifier field
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Spatial Scan Type (null if not specified)
     */
    public: inline int16_t getSpatialScanType (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int16_t bits = getI(SPATIAL_SCAN_TYPE, 2, cif7field);
      return (isNull(bits))? INT16_NULL : bits;
    }

    /** Sets Spatial Scan Type.
     *  Spatial Scan Type uses the Generic16 bit Identifier field
     *  See V49.2 spec Section 9.8.11 for Spatial Scan Type
     *  See V49.2 spec Section 9.8 for Generic16 bit Identifier field
     *  @param val The Spatial Scan Type (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setSpatialScanType (int16_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setI(SPATIAL_SCAN_TYPE, 2, val, cif7field);
    }

    /** Gets RF Footprint Range.
     *  RF Footprint Range is used to portray antenna geographical footprint on
     *  Earth via a Generic32 bit Identifier field linked to a KML in a Data
     *  Extension Packet.
     *  See V49.2 spec Section 9.8.12 for RF Footprint Range
     *  See V49.2 spec Section 9.8 for Generic32 bit Identifier field
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The RF Footprint Range (null if not specified)
     */
    public: inline int32_t getRFFootprintRange (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      return getL(RF_FOOTPRINT_RANGE, cif7field);
    }

    /** Sets RF Footprint Range.
     *  RF Footprint Range is used to portray antenna geographical footprint on
     *  Earth via a Generic32 bit Identifier field linked to a KML in a Data
     *  Extension Packet.
     *  See V49.2 spec Section 9.8.12 for RF Footprint Range
     *  See V49.2 spec Section 9.8 for Generic32 bit Identifier field
     *  @param val The RF Footprint Range (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setRFFootprintRange (int32_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setL(RF_FOOTPRINT_RANGE, val, cif7field);
    }

    /** Gets RF Footprint.
     *  RF Footprint is used to portray antenna geographical footprint on
     *  Earth via a Generic32 bit Identifier field linked to a KML Class Code
     *  in a Data Extension Packet.
     *  See V49.2 spec Section 9.8.12 for RF Footprint
     *  See V49.2 spec Section 9.8 for Generic32 bit Identifier field
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The RF Footprint (null if not specified)
     */
    public: inline int32_t getRFFootprint(IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      return getL(RF_FOOTPRINT, cif7field);
    }

    /** Sets RF Footprint.
     *  RF Footprint is used to portray antenna geographical footprint on
     *  Earth via a Generic32 bit Identifier field linked to a KML Class Code
     *  in a Data Extension Packet.
     *  See V49.2 spec Section 9.8.12 for RF Footprint
     *  See V49.2 spec Section 9.8 for Generic32 bit Identifier field
     *  @param val The RF Footprint (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setRFFootprint(int32_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setL(RF_FOOTPRINT, val, cif7field);
    }

    /** Gets Communication Priority ID
     *  Communication Priority ID is used to set a priority level to the
     *  information to be conveyed over a communication channel.
     *  The V49.2 spec does not specify, but it is assumed that the Generic32
     *  bit Identifier field is used for this field. XXX
     *  See V49.2 spec Section 9.8.10.5 for Communication Priority ID
     *  See V49.2 spec Section 9.8 for Generic32 bit Identifier field
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Communication Priority ID (null if not specified)
     */
    public: inline int32_t getCommunicationPriorityID (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      return getL(COMM_PRIORITY_ID, cif7field);
    }

    /** Sets Communication Priority ID
     *  Communication Priority ID is used to set a priority level to the
     *  information to be conveyed over a communication channel.
     *  The V49.2 spec does not specify, but it is assumed that the Generic32
     *  bit Identifier field is used for this field. XXX
     *  See V49.2 spec Section 9.8.10.5 for Communication Priority ID
     *  See V49.2 spec Section 9.8 for Generic32 bit Identifier field
     *  @param val The Communication Priority ID (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setCommunicationPriorityID (int32_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setL(COMM_PRIORITY_ID, val, cif7field);
    }

    /** Gets Function Priority ID
     *  Function Priority ID enables a prioritization schema to be expressed
     *  for using V49.2 Command Packets, such as in receiver, exciter, DSP and
     *  other device.
     *  Function Priority ID uses the Generic16 bit Identifier field
     *  See V49.2 spec Section 9.8.10.4 for Function Priority ID
     *  See V49.2 spec Section 9.8 for Generic16 bit Identifier field
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Function Priority ID (null if not specified)
     */
    public: inline int16_t getFunctionPriorityID (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int16_t bits = getI(FUNCT_PRIORITY_ID, 2, cif7field);
      return (isNull(bits))? INT16_NULL : bits;
    }

    /** Sets Function Priority ID
     *  Function Priority ID enables a prioritization schema to be expressed
     *  for using V49.2 Command Packets, such as in receiver, exciter, DSP and
     *  other device.
     *  Function Priority ID uses the Generic16 bit Identifier field
     *  See V49.2 spec Section 9.8.10.4 for Function Priority ID
     *  See V49.2 spec Section 9.8 for Generic16 bit Identifier field
     *  @param val The Function Priority ID (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setFunctionPriorityID (int16_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setI(FUNCT_PRIORITY_ID, 2, val, cif7field);
    }

    /** Gets Event ID
     *  Used to enumerate the different events that impact the interpretation of
     *  information or operations. Enumerated events can represent simple or
     *  complex relationships such as signal crossing threshold or signal
     *  crossing threshold while aperture pointing at a specific location.
     *  Event ID uses the Generic16 bit Identifier field
     *  See V49.2 spec Section 9.8.10.3 for Event ID
     *  See V49.2 spec Section 9.8 for Generic16 bit Identifier field
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Event ID (null if not specified)
     */
    public: inline int16_t getEventID (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int16_t bits = getI(EVENT_ID, 2, cif7field);
      return (isNull(bits))? INT16_NULL : bits;
    }

    /** Sets Event ID
     *  Used to enumerate the different events that impact the interpretation of
     *  information or operations. Enumerated events can represent simple or
     *  complex relationships such as signal crossing threshold or signal
     *  crossing threshold while aperture pointing at a specific location.
     *  Event ID uses the Generic16 bit Identifier field
     *  See V49.2 spec Section 9.8.10.3 for Event ID
     *  See V49.2 spec Section 9.8 for Generic16 bit Identifier field
     *  @param val The Event ID (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setEventID (int16_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setI(EVENT_ID, 2, val, cif7field);
    }

    /** Gets Mode ID
     *  Used to enumerate the different modes or states a node associated of the
     *  SID it is bonded to. For example, an enumeration of the field may convey
     *  Start, Stop, Reset, Pause, Restart, Load, Wait for Event, Zeroize
     *  information.
     *  Mode ID uses the Generic16 bit Identifier field
     *  See V49.2 spec Section 9.8.10.2 for Mode ID
     *  See V49.2 spec Section 9.8 for Generic16 bit Identifier field
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Mode ID (null if not specified)
     */
    public: inline int16_t getModeID (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int16_t bits = getI(MODE_ID, 2, cif7field);
      return (isNull(bits))? INT16_NULL : bits;
    }

    /** Sets Mode ID
     *  Used to enumerate the different modes or states a node associated of the
     *  SID it is bonded to. For example, an enumeration of the field may convey
     *  Start, Stop, Reset, Pause, Restart, Load, Wait for Event, Zeroize
     *  information.
     *  Mode ID uses the Generic16 bit Identifier field
     *  See V49.2 spec Section 9.8.10.2 for Mode ID
     *  See V49.2 spec Section 9.8 for Generic16 bit Identifier field
     *  @param val The Mode ID (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setModeID (int16_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setI(MODE_ID, 2, val, cif7field);
    }

    /** Gets Function ID
     *  Used to enumerate different analog, digital and other processes
     *  performed by SID it is associated with. For example, an enumeration of
     *  the field values can convey if it is an antenna, RF receiver, ADC,
     *  channelizer, DDC, DUC, DAC, RF upconverter or other attribute.
     *  Function ID uses the Generic16 bit Identifier field
     *  See V49.2 spec Section 9.8.10.1 for Function ID
     *  See V49.2 spec Section 9.8 for Generic16 bit Identifier field
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Function ID (null if not specified)
     */
    public: inline int16_t getFunctionID (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int16_t bits = getI(FUNCTION_ID, 2, cif7field);
      return (isNull(bits))? INT16_NULL : bits;
    }

    /** Sets Function ID
     *  Used to enumerate different analog, digital and other processes
     *  performed by SID it is associated with. For example, an enumeration of
     *  the field values can convey if it is an antenna, RF receiver, ADC,
     *  channelizer, DDC, DUC, DAC, RF upconverter or other attribute.
     *  Function ID uses the Generic16 bit Identifier field
     *  See V49.2 spec Section 9.8.10.1 for Function ID
     *  See V49.2 spec Section 9.8 for Generic16 bit Identifier field
     *  @param val The Function ID (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setFunctionID (int16_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setI(FUNCTION_ID, 2, val, cif7field);
    }

    /** Gets Modulation Type
     *  Conveys more detailed modulation attributes that can be used to uniquely
     *  identify a waveform association to an EMS device and/or object.
     *  Modulation Type uses the Generic16 bit Identifier field
     *  See V49.2 spec Section 9.8.9 for Modulation Type
     *  See V49.2 spec Section 9.8 for Generic16 bit Identifier field
     *  @return The Modulation Type (null if not specified)
     */
    public: inline int16_t getModulationType (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int16_t bits = getI(MODULATION_TYPE, 2, cif7field);
      return (isNull(bits))? INT16_NULL : bits;
    }

    /** Sets Modulation Type
     *  Conveys more detailed modulation attributes that can be used to uniquely
     *  identify a waveform association to an EMS device and/or object.
     *  Modulation Type uses the Generic16 bit Identifier field
     *  See V49.2 spec Section 9.8.9 for Modulation Type
     *  See V49.2 spec Section 9.8 for Generic16 bit Identifier field
     *  @param val The Modulation Type (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setModulationType (int16_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setI(MODULATION_TYPE, 2, val, cif7field);
    }

    /** Gets Modulation Class
     *  Conveys the general type of waveform used to generate a signal such as
     *  pulsed radar, pulsed linear FM radar, CW FM radar, n-ary QAM, n-ary PSK,
     *  LTE.
     *  Modulation Class uses the Generic16 bit Identifier field
     *  See V49.2 spec Section 9.8.9 for Modulation Class
     *  See V49.2 spec Section 9.8 for Generic16 bit Identifier field
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Modulation Class (null if not specified)
     */
    public: inline int16_t getModulationClass (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int16_t bits = getI(MODULATION_CLASS, 2, cif7field);
      return (isNull(bits))? INT16_NULL : bits;
    }

    /** Sets Modulation Class
     *  Conveys the general type of waveform used to generate a signal such as
     *  pulsed radar, pulsed linear FM radar, CW FM radar, n-ary QAM, n-ary PSK,
     *  LTE.
     *  Modulation Class uses the Generic16 bit Identifier field
     *  See V49.2 spec Section 9.8.9 for Modulation Class
     *  See V49.2 spec Section 9.8 for Generic16 bit Identifier field
     *  @param val The Modulation Class (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setModulationClass (int16_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setI(MODULATION_CLASS, 2, val, cif7field);
    }

    /** Gets EMS Device Instance
     *  Unique identifier for the specific device emitting the RF signal
     *  The V49.2 spec does not specify, but it is assumed that the Generic32
     *  bit Identifier field is used for this field. XXX
     *  See V49.2 spec Section 9.8.9 for EMS Device Instance
     *  See V49.2 spec Section 9.8 for Generic32 bit Identifier field
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The EMS Device Instance (null if not specified)
     */
    public: inline int32_t getEmsDeviceInstance (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      return getL(EMS_DEVICE_INSTANCE, cif7field);
    }

    /** Sets EMS Device Instance
     *  Unique identifier for the specific device emitting the RF signal
     *  The V49.2 spec does not specify, but it is assumed that the Generic32
     *  bit Identifier field is used for this field. XXX
     *  See V49.2 spec Section 9.8.9 for EMS Device Instance
     *  See V49.2 spec Section 9.8 for Generic32 bit Identifier field
     *  @param val The EMS Device Instance (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setEmsDeviceInstance (int32_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setL(EMS_DEVICE_INSTANCE, val, cif7field);
    }

    /** Gets EMS Device Type
     *  Identifies the specific type of EMS device such as a WSR-88D (weather
     *  radar), EASAT Primary Surveillanec radar (air traffic control), Motorola
     *  MOTOTRBO radio (business radio) or Verizon LTE.
     *  The V49.2 spec does not specify, but it is assumed that the Generic32
     *  bit Identifier field is used for this field. XXX
     *  See V49.2 spec Section 9.8.9 for EMS Device Type
     *  See V49.2 spec Section 9.8 for Generic32 bit Identifier field
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The EMS Device Type (null if not specified)
     */
    public: inline int32_t getEmsDeviceType (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      return getL(EMS_DEVICE_TYPE, cif7field);
    }

    /** Sets EMS Device Type
     *  Identifies the specific type of EMS device such as a WSR-88D (weather
     *  radar), EASAT Primary Surveillanec radar (air traffic control), Motorola
     *  MOTOTRBO radio (business radio) or Verizon LTE.
     *  The V49.2 spec does not specify, but it is assumed that the Generic32
     *  bit Identifier field is used for this field. XXX
     *  See V49.2 spec Section 9.8.9 for EMS Device Type
     *  See V49.2 spec Section 9.8 for Generic32 bit Identifier field
     *  @param val The EMS Device Type (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setEmsDeviceType (int32_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setL(EMS_DEVICE_TYPE, val, cif7field);
    }

    // TODO - could provide additional helper functions for EMS Device Class

    /** Gets EMS Device Class
     *  Conveys the general type of sensor used to generate the signal such as
     *  radar, communications, beacon. The enumeration may also represent sub-
     *  classes such as search radar, navigation radar, broadcast commercial FM
     *  station, SATCOM, cellular, public safety radio or military radio.
     *  EMS Device Class uses the Generic16 bit Identifier field
     *  Bits 15..14: General organization relationship 
     *    {00: Coalition, 01: Known, 10: Unknown, 11: Reserved}
     *  Bit 13: Set if device is exciter
     *  Bit 12: Set if device is receiver
     *  Bits 11..0: enumeration of general type of EMS device
     *  See V49.2 spec Section 9.8.9 for EMS Device Class
     *  See V49.2 spec Section 9.8 for Generic16 bit Identifier field
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The EMS Device Class (null if not specified)
     */
    public: inline int16_t getEmsDeviceClass (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int16_t bits = getI(EMS_DEVICE_CLASS, 2, cif7field);
      return (isNull(bits))? INT16_NULL : bits;
    }

    /** Sets EMS Device Class
     *  Conveys the general type of sensor used to generate the signal such as
     *  radar, communications, beacon. The enumeration may also represent sub-
     *  classes such as search radar, navigation radar, broadcast commercial FM
     *  station, SATCOM, cellular, public safety radio or military radio.
     *  EMS Device Class uses the Generic16 bit Identifier field
     *  See V49.2 spec Section 9.8.9 for EMS Device Class
     *  See V49.2 spec Section 9.8 for Generic16 bit Identifier field
     *  @param val The EMS Device Class (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setEmsDeviceClass (int16_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setI(EMS_DEVICE_CLASS, 2, val, cif7field);
    }

    /** Gets Platform Display
     *  Used to link to a graphical display item
     *  The V49.2 spec does not specify, but it is assumed that the Generic32
     *  bit Identifier field is used for this field. XXX
     *  See V49.2 spec Section 9.8.8 for Platform Display
     *  See V49.2 spec Section 9.8 for Generic32 bit Identifier field
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Platform Display (null if not specified)
     */
    public: inline int32_t getPlatformDisplay (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      return getL(PLATFORM_DISPLAY, cif7field);
    }

    /** Sets Platform Display
     *  Used to link to a graphical display item
     *  The V49.2 spec does not specify, but it is assumed that the Generic32
     *  bit Identifier field is used for this field. XXX
     *  See V49.2 spec Section 9.8.8 for Platform Display
     *  See V49.2 spec Section 9.8 for Generic32 bit Identifier field
     *  @param val The Platform Display (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setPlatformDisplay (int32_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setL(PLATFORM_DISPLAY, val, cif7field);
    }


    /** Gets Platform Instance
     *  Used to link to specific objects such as the United Airlines Flight
     *  7301, Mount Shasta, or Empire State Building
     *  The V49.2 spec does not specify, but it is assumed that the Generic32
     *  bit Identifier field is used for this field. XXX
     *  See V49.2 spec Section 9.8.8 for Platform Instance
     *  See V49.2 spec Section 9.8 for Generic32 bit Identifier field
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Platform Instance (null if not specified)
     */
    public: inline int32_t getPlatformInstance (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      return getL(PLATFORM_INSTANCE, cif7field);
    }

    /** Sets Platform Instance
     *  Used to link to specific objects such as the United Airlines Flight
     *  7301, Mount Shasta, or Empire State Building
     *  The V49.2 spec does not specify, but it is assumed that the Generic32
     *  bit Identifier field is used for this field. XXX
     *  See V49.2 spec Section 9.8.8 for Platform Instance
     *  See V49.2 spec Section 9.8 for Generic32 bit Identifier field
     *  @param val The Platform Instance (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setPlatformInstance (int32_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setL(PLATFORM_INSTANCE, val, cif7field);
    }

    /** Gets Platform Class
     *  Platform ID is used to link to a generic class of the object such as
     *  ship, commercial airliner, mountain or building
     *  The V49.2 spec does not specify, but it is assumed that the Generic32
     *  bit Identifier field is used for this field. XXX
     *  See V49.2 spec Section 9.8.8 for Platform Class
     *  See V49.2 spec Section 9.8 for Generic32 bit Identifier field
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Platform Class (null if not specified)
     */
    public: inline int32_t getPlatformClass (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      return getL(PLATFORM_CLASS, cif7field);
    }

    /** Sets Platform Class
     *  Platform ID is used to link to a generic class of the object such as
     *  ship, commercial airliner, mountain or building
     *  The V49.2 spec does not specify, but it is assumed that the Generic32
     *  bit Identifier field is used for this field. XXX
     *  See V49.2 spec Section 9.8.8 for Platform Class
     *  See V49.2 spec Section 9.8 for Generic32 bit Identifier field
     *  @param val The Platform Class (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setPlatformClass (int32_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setL(PLATFORM_CLASS, val, cif7field);
    }

    /** Gets Operator ID
     *  Provides linkage to operator enumerations
     *  Operator ID uses the Generic16 bit Identifier field
     *  See V49.2 spec Section 9.8.7 for Operator ID
     *  See V49.2 spec Section 9.8 for Generic16 bit Identifier field
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Operator ID (null if not specified)
     */
    public: inline int16_t getOperator (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int16_t bits = getI(OPERATOR, 2, cif7field);
      return (isNull(bits))? INT16_NULL : bits;
    }

    /** Sets Operator ID
     *  Provides linkage to operator enumerations
     *  Operator ID uses the Generic16 bit Identifier field
     *  See V49.2 spec Section 9.8.7 for Operator ID
     *  See V49.2 spec Section 9.8 for Generic16 bit Identifier field
     *  @param val The Operator ID (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setOperator (int16_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setI(OPERATOR, 2, val, cif7field);
    }

    // TODO - could provide additional helper functions for Country Code

    /** Gets Country Code
     *  The ISO 3166 Numeric code or user-defined code for a country.
     *  Bit 15: Set if user-defined country/entity code
     *  Bits 14..12: Reserved
     *  Bits 11..0: ISO 3166 or user-defined country/entity code
     *  Country Code uses the Generic16 bit Identifier field
     *  See V49.2 spec Section 9.8.7 for Country Code
     *  See V49.2 spec Section 9.8 for Generic16 bit Identifier field
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Country Code (null if not specified)
     */
    public: inline int16_t getCountryCode (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int16_t bits = getI(COUNTRY_CODE, 2, cif7field);
      return (isNull(bits))? INT16_NULL : bits;
    }

    /** Sets Country Code
     *  The ISO 3166 Numeric code or user-defined code for a country.
     *  Bit 15: Set if user-defined country/entity code
     *  Bits 14..12: Reserved
     *  Bits 11..0: ISO 3166 or user-defined country/entity code
     *  Country Code uses the Generic16 bit Identifier field
     *  See V49.2 spec Section 9.8.7 for Country Code
     *  See V49.2 spec Section 9.8 for Generic16 bit Identifier field
     *  @param val The Country Code (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setCountryCode (int16_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setI(COUNTRY_CODE, 2, val, cif7field);
    }

    /** Gets Track ID
     *  A dynamic number that is generated during operations used to indicate
     *  that information from a specrum observation or a set of spectrum
     *  observations over time is associated.
     *  Track ID uses the Generic32 bit Identifier field
     *  See V49.2 spec Section 9.8.6 for Track ID
     *  See V49.2 spec Section 9.8 for Generic32 bit Identifier field
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Track ID (null if not specified)
     */
    public: inline int32_t getTrackID (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      return getL(TRACK_ID, cif7field);
    }

    /** Sets Track ID
     *  A dynamic number that is generated during operations used to indicate
     *  that information from a specrum observation or a set of spectrum
     *  observations over time is associated.
     *  Track ID uses the Generic32 bit Identifier field
     *  See V49.2 spec Section 9.8.6 for Track ID
     *  See V49.2 spec Section 9.8 for Generic32 bit Identifier field
     *  @param val The Track ID (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setTrackID (int32_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setL(TRACK_ID, val, cif7field);
    }

    /** Gets Information Source
     *  A tag to represent different types of information sources.
     *  The V49.2 spec does not specify, but it is assumed that the Generic32
     *  bit Identifier field is used for this field. XXX
     *  See V49.2 spec Section 9.8.5 for Information Source
     *  See V49.2 spec Section 9.8 for Generic32 bit Identifier field
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Information Source (null if not specified)
     */
    public: inline int32_t getInformationSource (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      return getL(INFORMATION_SOURCE, cif7field);
    }

    /** Sets Information Source
     *  A tag to represent different types of information sources.
     *  The V49.2 spec does not specify, but it is assumed that the Generic32
     *  bit Identifier field is used for this field. XXX
     *  See V49.2 spec Section 9.8.5 for Information Source
     *  See V49.2 spec Section 9.8 for Generic32 bit Identifier field
     *  @param val The Information Source (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setInformationSource (int32_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setL(INFORMATION_SOURCE, val, cif7field);
    }

    /** Gets Controller UUID Indicator Field
     *  See V49.2 spec Section 9.8.3 for Controller UUID
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Controller UUID (null if not specified)
     */
    public: inline UUID getControllerUUIDField (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      return getUUID(CONTROLLER_UUID, cif7field);
    }

    /** Sets Controller UUID Indicator Field
     *  See V49.2 spec Section 9.8.3 for Controller UUID
     *  @param val The Controller UUID (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setControllerUUIDField (const UUID &val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setUUID(CONTROLLER_UUID, val, cif7field);
    }

    /** Gets Controller ID Indicator Field
     *  See V49.2 spec Section 9.8.3 for Controller ID
     *  @return The Controller ID (null if not specified)
     */
    public: inline int32_t getControllerIDField (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      return getL(CONTROLLER_ID, cif7field);
    }

    /** Sets Controller ID Indicator Field
     *  See V49.2 spec Section 9.8.3 for Controller ID
     *  @param val The Controller ID (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setControllerIDField (int32_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setL(CONTROLLER_ID, val, cif7field);
    }

    /** Gets Controllee UUID Indicator Field
     *  See V49.2 spec Section 9.8.3 for Controllee UUID
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Controllee UUID (null if not specified)
     */
    public: inline UUID getControlleeUUIDField (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      return getUUID(CONTROLLEE_UUID, cif7field);
    }

    /** Sets Controllee UUID Indicator Field
     *  See V49.2 spec Section 9.8.3 for Controllee UUID
     *  @param val The Controllee UUID (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setControlleeUUIDField (const UUID &val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setUUID(CONTROLLEE_UUID, val, cif7field);
    }

    /** Gets Controllee ID Indicator Field
     *  See V49.2 spec Section 9.8.3 for Controllee ID
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Controllee ID (null if not specified)
     */
    public: inline int32_t getControlleeIDField (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      return getL(CONTROLLEE_ID, cif7field);
    }

    /** Sets Controllee ID Indicator Field
     *  See V49.2 spec Section 9.8.3 for Controllee ID
     *  @param val The Controllee ID (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setControlleeIDField (int32_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setL(CONTROLLEE_ID, val, cif7field);
    }

    /** Gets Cited Message ID
     *  Used to relay, repeat or recall the information that was conveyed in a
     *  packet with the same Message ID for a Cited SID.
     *  The V49.2 spec does not specify, but it is assumed that the Generic32
     *  bit Identifier field is used for this field. XXX
     *  See V49.2 spec Section 9.8.4 for Cited Message ID
     *  See V49.2 spec Section 9.8 for Generic32 bit Identifier field
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Cited Message ID (null if not specified)
     */
    public: inline int32_t getCitedMessageID (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      return getL(CITED_MESSAGE_ID, cif7field);
    }

    /** Sets Cited Message ID
     *  Used to relay, repeat or recall the information that was conveyed in a
     *  packet with the same Message ID for a Cited SID.
     *  The V49.2 spec does not specify, but it is assumed that the Generic32
     *  bit Identifier field is used for this field. XXX
     *  See V49.2 spec Section 9.8.4 for Cited Message ID
     *  See V49.2 spec Section 9.8 for Generic32 bit Identifier field
     *  @param val The Cited Message ID (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setCitedMessageID (int32_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setL(CITED_MESSAGE_ID, val, cif7field);
    }

    /** Gets Children SID
     *  See V49.2 spec Section 9.8.2 for Children SID
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Children SID (null if not specified)
     */
    public: inline int32_t getChildrenSID (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      return getL(CHILDREN_SID, cif7field);
    }
    /** Gets Children SID as a string
     *  See V49.2 spec Section 9.8.2 for Children SID
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Children SID (null if not specified)
     */
    public: inline string getChildrenSIDString (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int32_t streamID = getL(CHILDREN_SID, cif7field);
      return (isNull(streamID))? string("") : Utilities::format("%d", streamID);
    }

    /** Sets Children SID
     *  See V49.2 spec Section 9.8.2 for Children SID
     *  @param val The Children SID (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setChildrenSID (int32_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setL(CHILDREN_SID, val, cif7field);
    }
    /** Sets Children SID from a string
     *  See V49.2 spec Section 9.8.2 for Children SID
     *  @param val The Children SID (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setChildrenSIDString (string val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      if (val.size() == 0)
        setChildrenSID(INT32_NULL, cif7field);
      else
        setChildrenSID(atoi(val.c_str()), cif7field);
    }

    /** Gets Parents SID
     *  See V49.2 spec Section 9.8.2 for Parents SID
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Parents SID (null if not specified)
     */
    public: inline int32_t getParentsSID (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      return getL(PARENTS_SID, cif7field);
    }
    /** Gets Parents SID as a string
     *  See V49.2 spec Section 9.8.2 for Parents SID
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Parents SID (null if not specified)
     */
    public: inline string getParentsSIDString (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int32_t streamID = getL(PARENTS_SID, cif7field);
      return (isNull(streamID))? string("") : Utilities::format("%d", streamID);
    }

    /** Sets Parents SID
     *  See V49.2 spec Section 9.8.2 for Parents SID
     *  @param val The Parents SID (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setParentsSID (int32_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setL(PARENTS_SID, val, cif7field);
    }
    /** Sets Parents SID from a string
     *  See V49.2 spec Section 9.8.2 for Parents SID
     *  @param val The Parents SID (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setParentsSIDString (string val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      if (val.size() == 0)
        setParentsSID(INT32_NULL, cif7field);
      else
        setParentsSID(atoi(val.c_str()), cif7field);
    }

    /** Gets Siblings SID
     *  See V49.2 spec Section 9.8.2 for Siblings SID
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Siblings SID (null if not specified)
     */
    public: inline int32_t getSiblingsSID (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      return getL(SIBLINGS_SID, cif7field);
    }
    /** Gets Siblings SID as a string
     *  See V49.2 spec Section 9.8.2 for Siblings SID
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Siblings SID (null if not specified)
     */
    public: inline string getSiblingsSIDString (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int32_t streamID = getL(SIBLINGS_SID, cif7field);
      return (isNull(streamID))? string("") : Utilities::format("%d", streamID);
    }

    /** Sets Siblings SID
     *  See V49.2 spec Section 9.8.2 for Siblings SID
     *  @param val The Siblings SID (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setSiblingsSID (int32_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setL(SIBLINGS_SID, val, cif7field);
    }
    /** Sets Siblings SID from a string
     *  See V49.2 spec Section 9.8.2 for Siblings SID
     *  @param val The Siblings SID (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setSiblingsSIDString (string val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      if (val.size() == 0)
        setSiblingsSID(INT32_NULL, cif7field);
      else
        setSiblingsSID(atoi(val.c_str()), cif7field);
    }

    /** Gets Cited SID
     *  See V49.2 spec Section 9.8.2 for Cited SID
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Cited SID (null if not specified)
     */
    public: inline int32_t getCitedSID (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      return getL(CITED_SID, cif7field);
    }
    /** Gets Cited SID as a string
     *  See V49.2 spec Section 9.8.2 for Cited SID
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Cited SID (null if not specified)
     */
    public: inline string getCitedSIDString (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int32_t streamID = getL(CITED_SID, cif7field);
      return (isNull(streamID))? string("") : Utilities::format("%d", streamID);
    }

    /** Sets Cited SID
     *  See V49.2 spec Section 9.8.2 for Cited SID
     *  @param val The Cited SID (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setCitedSID (int32_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setL(CITED_SID, val, cif7field);
    }
    /** Sets Cited SID from a string
     *  See V49.2 spec Section 9.8.2 for Cited SID
     *  @param val The Cited SID (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setCitedSIDString (string val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      if (val.size() == 0)
        setCitedSID(INT32_NULL, cif7field);
      else
        setCitedSID(atoi(val.c_str()), cif7field);
    }

    // TODO - could provide additional helper functions for Country Code

    /** Gets Bind Indicator Field
     *  See V49.2 spec Section 9.8.1 for Bind Indicator Field
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Bind field (null if not specified)
     */
    public: inline int32_t getBindField (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      return getL(BIND, cif7field);
    }

    /** Sets Bind Indicator Field
     *  See V49.2 spec Section 9.8.1 for Bind Indicator Field
     *  @param val The Bind field (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setBindField (int32_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setL(BIND, val, cif7field);
    }
    /* XXX END OF CIF2 FUNCTIONS XXX */

    /* XXX START OF CIF3 FUNCTIONS XXX */
    /** Gets Network ID
     *  Used to associate information to be on the same network.
     *  Network ID uses the Generic32 bit Identifier field
     *  See V49.2 spec Section 9.8.13 for Network ID
     *  See V49.2 spec Section 9.8 for Generic32 bit Identifier field
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Network ID (null if not specified)
     */
    public: inline int32_t getNetworkID (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      return getL(NETWORK_ID, cif7field);
    }

    /** Sets Network ID
     *  Used to associate information to be on the same network.
     *  Network ID uses the Generic32 bit Identifier field
     *  See V49.2 spec Section 9.8.13 for Network ID
     *  See V49.2 spec Section 9.8 for Generic32 bit Identifier field
     *  @param val The Network ID (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setNetworkID (int32_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setL(NETWORK_ID, val, cif7field);
    }

    /** Gets Tropospheric State
     *  Used to enumerate the state of the troposphere.
     *  Tropospheric State uses the Generic16 bit Identifier field
     *  See V49.2 spec Section 9.1.1 for Tropospheric State
     *  See V49.2 spec Section 9.8 for Generic16 bit Identifier field
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Tropospheric State (null if not specified)
     */
    public: inline int16_t getTroposphericState (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int16_t bits = getI(TROPOSPHERIC_STATE, 2, cif7field);
      return (isNull(bits))? INT16_NULL : bits;
    }

    /** Sets Tropospheric State
     *  Used to enumerate the state of the troposphere.
     *  Tropospheric State uses the Generic16 bit Identifier field
     *  See V49.2 spec Section 9.9.1 for Tropospheric State
     *  See V49.2 spec Section 9.8 for Generic16 bit Identifier field
     *  @param val The Tropospheric State (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setTroposphericState (int16_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setI(TROPOSPHERIC_STATE, 2, val, cif7field);
    }

    /** Gets Sea and Swell State
     *  Sea and Swell States use the Douglas sea and swell scale as officially
     *  defined by the United Kingdom MetOffice in "Fact Sheet No. 6". Sea and
     *  Swell States are each a number between 0 and 9.
     *  Bits 31..16: Reserved (property of all Generic16 bit Identifier fields)
     *  Bits 15..10: User Defined
     *  Bits 9..5: Swell State
     *  Bits 4..0: Sea State
     *  Sea and Swell State uses the Generic16 bit Identifier field
     *  See V49.2 spec Section 9.9.1 for Sea and Swell State
     *  See V49.2 spec Section 9.8 for Generic16 bit Identifier field
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Sea and Swell State (null if not specified)
     */
    public: inline int16_t getSeaAndSwellState (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int16_t bits = getI(SEA_AND_SWELL_STATE, 2, cif7field);
      return (isNull(bits))? INT16_NULL : bits;
    }

    /** Sets Sea and Swell State
     *  Sea and Swell States use the Douglas sea and swell scale as officially
     *  defined by the United Kingdom MetOffice in "Fact Sheet No. 6". Sea and
     *  Swell States are each a number between 0 and 9.
     *  Bits 31..16: Reserved (property of all Generic16 bit Identifier fields)
     *  Bits 15..10: User Defined
     *  Bits 9..5: Swell State
     *  Bits 4..0: Sea State
     *  Sea and Swell State uses the Generic16 bit Identifier field
     *  See V49.2 spec Section 9.9.1 for Sea and Swell State
     *  See V49.2 spec Section 9.8 for Generic16 bit Identifier field
     *  @param val The Sea and Swell State (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @throws VRTException if parameter contains invalid sea or swell values
     */
    public: inline void setSeaAndSwellState (int16_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      if (!isNull(val)) {
        int8_t sea = val&0x1F;
        int8_t swell = (val>>5)&0x1F;
        if (sea < 0 || swell < 0 || sea > 9 || swell > 9)
          throw VRTException("Sea and Swell must both be between 0 and 9.");
      }
      setI(SEA_AND_SWELL_STATE, 2, val, cif7field);
    }

    // XXX could add set/getSeaState, set/getSwellState, set/getSeaAndSwellStateUserDefinedBits

    /** Sets Sea and Swell State
     *  Sea and Swell States use the Douglas sea and swell scale as officially
     *  defined by the United Kingdom MetOffice in "Fact Sheet No. 6". Sea and
     *  Swell States are each a number between 0 and 9.
     *  Bits 31..16: Reserved (property of all Generic16 bit Identifier fields)
     *  Bits 15..10: User Defined
     *  Bits 9..5: Swell State
     *  Bits 4..0: Sea State
     *  Sea and Swell State uses the Generic16 bit Identifier field
     *  See V49.2 spec Section 9.9.1 for Sea and Swell State
     *  See V49.2 spec Section 9.8 for Generic16 bit Identifier field
     *  @param sea The Sea State [0 to 9], not null
     *  @param swell The Swell State [0  to 9], not null
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @throws VRTException if either parameter is invalid, including null
     */
    public: inline void setSeaAndSwellState (int8_t sea, int8_t swell, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      if (isNull(sea) || isNull(swell))
        throw VRTException("Sea and Swell must both not be null to use this method.");
      if (sea < 0 || swell < 0 || sea > 9 || swell > 9)
        throw VRTException("Sea and Swell must both be between 0 and 9.");
      int16_t val = (swell<<5) | sea;
      setI(SEA_AND_SWELL_STATE, 2, val, cif7field);
    }

    /** Gets Barometric Pressure
     *  Atmospheric pressure measured in the units of Pascal.
     *  Bits 31..17: Reserved; 0x0
     *  Bits 16..0: 0x1FFFF=131071 Pascals; 0x00001=1/131071 Pascals
     *  See V49.2 spec Section 9.9.2 for Barometric Pressure
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Barometric Pressure (null if not specified)
     */
    public: inline int32_t getBarometricPressure (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      return getL(BAROMETRIC_PRESSURE, cif7field);
    }

    /** Sets Barometric Pressure
     *  Atmospheric pressure measured in the units of Pascal.
     *  Bits 31..17: Reserved; 0x0
     *  Bits 16..0: 0x1FFFF=131071 Pascals; 0x00001=1/131071 Pascals
     *  See V49.2 spec Section 9.9.2 for Barometric Pressure
     *  @param val The Barometric Pressure (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setBarometricPressure (int32_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      if (!isNull(val))
        val = val&0x1FFFF;
      setL(BAROMETRIC_PRESSURE, val, cif7field);
    }

    /** Gets Humidity
     *  Percent humidity.
     *  Bits 31..16: Reserved; 0x0
     *  Bits 15..0: Percent humidity. 0xFFFF=100%; 0x0001=(1/65535)*100%
     *  See V49.2 spec Section 9.9.2 for Humidity
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Humidity (null if not specified)
     */
    public: inline int32_t getHumidity (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      return getL(HUMIDITY, cif7field);
    }

    /** Sets Humidity
     *  Percent humidity.
     *  Bits 31..16: Reserved; 0x0
     *  Bits 15..0: Percent humidity. 0xFFFF=100%; 0x0001=(1/65535)*100%
     *  See V49.2 spec Section 9.9.2 for Humidity
     *  @param val The Humidity (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setHumidity (int32_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      if (!isNull(val))
        val = val&0xFFFF;
      setL(HUMIDITY, val, cif7field);
    }

    /** Gets the Sea/Ground Temperature.
     *  Sea or ground temperature in degrees Celcius. The valid range of values
     *  is from -273.15 to +511.984375 degrees Celcius.
     *  Bits 31..16: Reserved; 0x0
     *  Bits 15..0: 16-bit floating point value with radix point to the right of
     *  bit 6.
     *  See V49.2 spec Section 9.9.2 for Sea/Ground Temperature
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Sea/Ground Temperature (null if not specified).
     */
    public: inline float getSeaGroundTemperature (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int16_t bits = getI(SEA_GROUND_TEMP,2, cif7field);
      return (isNull(bits))? FLOAT_NAN : VRTMath::toFloat16(6,bits);
    }

    /** Sets the Sea/Ground Temperature.
     *  Sea or ground temperature in degrees Celcius. The valid range of values
     *  is from -273.15 to +511.984375 degrees Celcius.
     *  Bits 31..16: Reserved; 0x0
     *  Bits 15..0: 16-bit floating point value with radix point to the right of
     *  bit 6.
     *  See V49.2 spec Section 9.9.2 for Sea/Ground Temperature
     *  @param val The Sea/Ground Temperature (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setSeaGroundTemperature (float val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      int16_t bits = (isNull(val))? INT16_NULL : VRTMath::fromFloat16(6,val);
      setI(SEA_GROUND_TEMP,2,bits, cif7field);
    }

    /** Gets the Air Temperature.
     *  Air temperature in degrees Celcius. The valid range of values is from
     *  -273.15 to +511.984375 degrees Celcius.
     *  Bits 31..16: Reserved; 0x0
     *  Bits 15..0: 16-bit floating point value with radix point to the right of
     *  bit 6.
     *  See V49.2 spec Section 9.9.2 for Air Temperature
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Air Temperature (null if not specified).
     */
    public: inline float getAirTemperature (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int16_t bits = getI(AIR_TEMP,2, cif7field);
      return (isNull(bits))? FLOAT_NAN : VRTMath::toFloat16(6,bits);
    }

    /** Sets the Air Temperature.
     *  Air temperature in degrees Celcius. The valid range of values is from
     *  -273.15 to +511.984375 degrees Celcius.
     *  Bits 31..16: Reserved; 0x0
     *  Bits 15..0: 16-bit floating point value with radix point to the right of
     *  bit 6.
     *  See V49.2 spec Section 9.9.2 for Air Temperature
     *  @param val The Air Temperature (null if not specified).
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setAirTemperature (float val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      int16_t bits = (isNull(val))? INT16_NULL : VRTMath::fromFloat16(6,val);
      setI(AIR_TEMP,2,bits, cif7field);
    }

    /** Gets Shelf Life
     *  The Shelf Life value and format depends on the packet timestamp format
     *  indicated by TSI and TSF fields of the Packet Prologue.
     *  See V49.2 spec Section 9.7.2 for Shelf Life
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Shelf Life (null if not specified)
     */
    public: inline TimeStamp getShelfLife (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      return getTimeStampField(SHELF_LIFE, cif7field);
    }

    /** Sets Shelf Life
     *  The Shelf Life value and format depends on the packet timestamp format
     *  indicated by TSI and TSF fields of the Packet Prologue.
     *  See V49.2 spec Section 9.7.2 for Shelf Life
     *  @param val The Shelf Life (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setShelfLife (const TimeStamp &val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setTimeStampField(SHELF_LIFE, val, cif7field);
    }

    /** Gets Age
     *  The Age value and format depends on the packet timestamp format
     *  indicated by TSI and TSF fields of the Packet Prologue.
     *  See V49.2 spec Section 9.7.2 for Age
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Age (null if not specified)
     */
    public: inline TimeStamp getAge (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      return getTimeStampField(AGE, cif7field);
    }

    /** Sets Age
     *  The Age value and format depends on the packet timestamp format
     *  indicated by TSI and TSF fields of the Packet Prologue.
     *  See V49.2 spec Section 9.7.2 for Age
     *  @param val The Age (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setAge (const TimeStamp &val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setTimeStampField(AGE, val, cif7field);
    }

    /** Gets Jitter
     *  Jitter expresses the uncertainty of the timing attribute. The Jitter
     *  range is +/- the value expressed in the Jitter field.
     *  Jitter uses the 64-bit Fractional Time format where the least
     *  significant bit represents 1 femtosecond (10e-15 seconds).
     *  See V49.2 spec Section 9.7.1 for Jitter
     *  See V49.2 spec Section 9.7 for Fractional Time format
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Jitter (null if not specified)
     */
    public: inline int64_t getJitter (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int64_t bits = getX(JITTER, cif7field);
      return (isNull(bits))? INT64_NULL : bits;
    }

    /** Sets Jitter
     *  Jitter expresses the uncertainty of the timing attribute. The Jitter
     *  range is +/- the value expressed in the Jitter field.
     *  Jitter uses the 64-bit Fractional Time format where the least
     *  significant bit represents 1 femtosecond (10e-15 seconds).
     *  See V49.2 spec Section 9.7.1 for Jitter
     *  See V49.2 spec Section 9.7 for Fractional Time format
     *  @param val The Jitter (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setJitter (int64_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setX(JITTER,val, cif7field);
    }

    /** Gets Dwell
     *  Dwell expresses the length of time that a signal existed (Context
     *  Packets) or should exist (Command Packets).
     *  Dwell uses the 64-bit Fractional Time format where the least
     *  significant bit represents 1 femtosecond (10e-15 seconds).
     *  See V49.2 spec Section 9.7.1 for Dwell
     *  See V49.2 spec Section 9.7 for Fractional Time format
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Dwell (null if not specified)
     */
    public: inline int64_t getDwell (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int64_t bits = getX(DWELL, cif7field);
      return (isNull(bits))? INT64_NULL : bits;
    }

    /** Sets Dwell
     *  Dwell expresses the length of time that a signal existed (Context
     *  Packets) or should exist (Command Packets).
     *  Dwell uses the 64-bit Fractional Time format where the least
     *  significant bit represents 1 femtosecond (10e-15 seconds).
     *  See V49.2 spec Section 9.7.1 for Dwell
     *  See V49.2 spec Section 9.7 for Fractional Time format
     *  @param val The Dwell (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setDwell (int64_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setX(DWELL,val, cif7field);
    }

    /** Gets Duration
     *  Duration expresses the length of time that a pattern in a signal existed
     *  (Context Packets) or should exist (Command Packets).
     *  Duration uses the 64-bit Fractional Time format where the least
     *  significant bit represents 1 femtosecond (10e-15 seconds).
     *  See V49.2 spec Section 9.7.1 for Duration
     *  See V49.2 spec Section 9.7 for Fractional Time format
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Duration (null if not specified)
     */
    public: inline int64_t getDuration (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int64_t bits = getX(DURATION, cif7field);
      return (isNull(bits))? INT64_NULL : bits;
    }

    /** Sets Duration
     *  Duration expresses the length of time that a pattern in a signal existed
     *  (Context Packets) or should exist (Command Packets).
     *  Duration uses the 64-bit Fractional Time format where the least
     *  significant bit represents 1 femtosecond (10e-15 seconds).
     *  See V49.2 spec Section 9.7.1 for Duration
     *  See V49.2 spec Section 9.7 for Fractional Time format
     *  @param val The Duration (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setDuration (int64_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setX(DURATION,val, cif7field);
    }

    /** Gets Period
     *  Period expresses the time between cyclical phenomena of a signal.
     *  Period uses the 64-bit Fractional Time format where the least
     *  significant bit represents 1 femtosecond (10e-15 seconds).
     *  See V49.2 spec Section 9.7.1 for Period
     *  See V49.2 spec Section 9.7 for Fractional Time format
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Period (null if not specified)
     */
    public: inline int64_t getPeriod (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int64_t bits = getX(PERIOD, cif7field);
      return (isNull(bits))? INT64_NULL : bits;
    }

    /** Sets Period
     *  Period expresses the time between cyclical phenomena of a signal.
     *  Period uses the 64-bit Fractional Time format where the least
     *  significant bit represents 1 femtosecond (10e-15 seconds).
     *  See V49.2 spec Section 9.7.1 for Period
     *  See V49.2 spec Section 9.7 for Fractional Time format
     *  @param val The Period (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setPeriod (int64_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setX(PERIOD,val, cif7field);
    }

    /** Gets Pulse Width
     *  Pulse Width measures the delta time between two timing edges of a
     *  signal. It measures the time between the rising edge and falling edge of
     *  a signal or vice versa.
     *  Pulse Width uses the 64-bit Fractional Time format where the least
     *  significant bit represents 1 femtosecond (10e-15 seconds).
     *  See V49.2 spec Section 9.7.1 for Pulse Width
     *  See V49.2 spec Section 9.7 for Fractional Time format
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Pulse Width (null if not specified)
     */
    public: inline int64_t getPulseWidth (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int64_t bits = getX(PULSE_WIDTH, cif7field);
      return (isNull(bits))? INT64_NULL : bits;
    }

    /** Sets Pulse Width
     *  Pulse Width measures the delta time between two timing edges of a
     *  signal. It measures the time between the rising edge and falling edge of
     *  a signal or vice versa.
     *  Pulse Width uses the 64-bit Fractional Time format where the least
     *  significant bit represents 1 femtosecond (10e-15 seconds).
     *  See V49.2 spec Section 9.7.1 for Pulse Width
     *  See V49.2 spec Section 9.7 for Fractional Time format
     *  @param val The Pulse Width (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setPulseWidth (int64_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setX(PULSE_WIDTH,val, cif7field);
    }

    /** Gets Offset Time
     *  Offset Time is used to measure latency between the timestamp in the V49
     *  Packet Prologue and the timing edge when the information in the Context
     *  Packet or Command Packet became valid.
     *  Offset Time uses the 64-bit Fractional Time format where the least
     *  significant bit represents 1 femtosecond (10e-15 seconds).
     *  See V49.2 spec Section 9.7.1 for Offset Time
     *  See V49.2 spec Section 9.7 for Fractional Time format
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Offset Time (null if not specified)
     */
    public: inline int64_t getOffsetTime (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int64_t bits = getX(OFFSET_TIME, cif7field);
      return (isNull(bits))? INT64_NULL : bits;
    }

    /** Sets Offset Time
     *  Offset Time is used to measure latency between the timestamp in the V49
     *  Packet Prologue and the timing edge when the information in the Context
     *  Packet or Command Packet became valid.
     *  Offset Time uses the 64-bit Fractional Time format where the least
     *  significant bit represents 1 femtosecond (10e-15 seconds).
     *  See V49.2 spec Section 9.7.1 for Offset Time
     *  See V49.2 spec Section 9.7 for Fractional Time format
     *  @param val The Offset Time (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setOffsetTime (int64_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setX(OFFSET_TIME,val, cif7field);
    }

    /** Gets Fall Time
     *  Fall Time is the delta time from 90% of maximum deviation to 10% of
     *  maximum deviation. Deviation is referenced to the noise floor of the
     *  signal.
     *  Fall Time uses the 64-bit Fractional Time format where the least
     *  significant bit represents 1 femtosecond (10e-15 seconds).
     *  See V49.2 spec Section 9.7.1 for Fall Time
     *  See V49.2 spec Section 9.7 for Fractional Time format
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Fall Time (null if not specified)
     */
    public: inline int64_t getFallTime (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int64_t bits = getX(FALL_TIME, cif7field);
      return (isNull(bits))? INT64_NULL : bits;
    }

    /** Sets Fall Time
     *  Fall Time is the delta time from 90% of maximum deviation to 10% of
     *  maximum deviation. Deviation is referenced to the noise floor of the
     *  signal.
     *  Fall Time uses the 64-bit Fractional Time format where the least
     *  significant bit represents 1 femtosecond (10e-15 seconds).
     *  See V49.2 spec Section 9.7.1 for Fall Time
     *  See V49.2 spec Section 9.7 for Fractional Time format
     *  @param val The Fall Time (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setFallTime (int64_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setX(FALL_TIME,val, cif7field);
    }

    /** Gets Rise Time
     *  Rise Time is the delta time from 10% of maximum deviation to 90% of
     *  maximum deviation. Deviation is referenced to the noise floor of the
     *  signal.
     *  Rise Time uses the 64-bit Fractional Time format where the least
     *  significant bit represents 1 femtosecond (10e-15 seconds).
     *  See V49.2 spec Section 9.7.1 for Rise Time
     *  See V49.2 spec Section 9.7 for Fractional Time format
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Rise Time (null if not specified)
     */
    public: inline int64_t getRiseTime (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int64_t bits = getX(RISE_TIME, cif7field);
      return (isNull(bits))? INT64_NULL : bits;
    }

    /** Sets Rise Time
     *  Rise Time is the delta time from 10% of maximum deviation to 90% of
     *  maximum deviation. Deviation is referenced to the noise floor of the
     *  signal.
     *  Rise Time uses the 64-bit Fractional Time format where the least
     *  significant bit represents 1 femtosecond (10e-15 seconds).
     *  See V49.2 spec Section 9.7.1 for Rise Time
     *  See V49.2 spec Section 9.7 for Fractional Time format
     *  @param val The Rise Time (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setRiseTime (int64_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setX(RISE_TIME,val, cif7field);
    }

    /** Gets Timestamp Skew
     *  Timestamp Skew conveys a time offset between channels in a system or
     *  between systems in a distributed system-of-systems architecture.
     *  Timestamp Skew uses the 64-bit Fractional Time format where the least
     *  significant bit represents 1 femtosecond (10e-15 seconds).
     *  See V49.2 spec Section 9.7.3.2 for Timestamp Skew
     *  See V49.2 spec Section 9.7 for Fractional Time format
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Timestamp Skew (null if not specified)
     */
    public: inline int64_t getTimestampSkew (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int64_t bits = getX(TIMESTAMP_SKEW, cif7field);
      return (isNull(bits))? INT64_NULL : bits;
    }

    /** Sets Timestamp Skew
     *  Timestamp Skew conveys a time offset between channels in a system or
     *  between systems in a distributed system-of-systems architecture.
     *  Timestamp Skew uses the 64-bit Fractional Time format where the least
     *  significant bit represents 1 femtosecond (10e-15 seconds).
     *  See V49.2 spec Section 9.7.3.2 for Timestamp Skew
     *  See V49.2 spec Section 9.7 for Fractional Time format
     *  @param val The Timestamp Skew (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setTimestampSkew (int64_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      setX(TIMESTAMP_SKEW,val, cif7field);
    }

    // TODO - Could provide additional helpers for Timestamp Details field

    /** Gets Timestamp Details
     *  Timestamp Details describes the timestamps used in the Context Stream
     *  and the paired Data Stream (if present).
     *  First Word:
     *    Bits 31..24: User Defined
     *    Bits 23..19: Reserved; 0x0
     *    Bit      18: (G) Globally applicable?
     *    Bits 17..16: (TSE Code) Type of Timestamp Epoch provided
     *    Bits 15..14: (LSH Code) Indicates the means by which leap seconds are
     *                 handled in the packet timestamps
     *    Bits 13..12: (LSP Code) Indicates the number of seconds in the current
     *                 day denoted by the packet timestamps
     *    Bits  11..9: (Time Source) Indicates time reference source being used
     *    Bit       8: (E) If 0, Current POSIX Time Offset is undefined. If 1,
     *                 Current POSIX Time Offset conveys the difference in
     *                 seconds between UTC time and POSIX time, as a signed
     *                 twos-complement number, and it represents the total leap
     *                 seconds count.
     *    Bits   7..0: (Current POSIX Time Offset) See Bit 8 (E)
     *  Second Word:
     *    Bits  31..0: (Timestamp Epoch) Unsigned integer value specifying the
     *                 number of whole seconds, in the epoch denoted by the TSE
     *                 Code, describing the start of the epoch used for
     *                 timestamps in all packets to which the Timestamp Details
     *                 field applies. When the TSE Code is set to Unspecified,
     *                 the value in the Timestamp Epoch field has no meaning.
     *  See V49.2 spec Section 9.7.3.4 for Timestamp Details
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     *  @return The Timestamp Details (null if not specified)
     */
    public: inline int64_t getTimestampDetails (IndicatorFieldEnum_t cif7field=CIF_NULL) const {
      int64_t bits = getX(TIMESTAMP_DETAILS, cif7field);
      return (isNull(bits))? INT64_NULL : bits;
    }

    /** Sets Timestamp Details
     *  Timestamp Details describes the timestamps used in the Context Stream
     *  and the paired Data Stream (if present).
     *  First Word:
     *    Bits 31..24: User Defined
     *    Bits 23..19: Reserved; 0x0
     *    Bit      18: (G) Globally applicable?
     *    Bits 17..16: (TSE Code) Type of Timestamp Epoch provided
     *    Bits 15..14: (LSH Code) Indicates the means by which leap seconds are
     *                 handled in the packet timestamps
     *    Bits 13..12: (LSP Code) Indicates the number of seconds in the current
     *                 day denoted by the packet timestamps
     *    Bits  11..9: (Time Source) Indicates time reference source being used
     *    Bit       8: (E) If 0, Current POSIX Time Offset is undefined. If 1,
     *                 Current POSIX Time Offset conveys the difference in
     *                 seconds between UTC time and POSIX time, as a signed
     *                 twos-complement number, and it represents the total leap
     *                 seconds count.
     *    Bits   7..0: (Current POSIX Time Offset) See Bit 8 (E)
     *  Second Word:
     *    Bits  31..0: (Timestamp Epoch) Unsigned integer value specifying the
     *                 number of whole seconds, in the epoch denoted by the TSE
     *                 Code, describing the start of the epoch used for
     *                 timestamps in all packets to which the Timestamp Details
     *                 field applies. When the TSE Code is set to Unspecified,
     *                 the value in the Timestamp Epoch field has no meaning.
     *  See V49.2 spec Section 9.7.3.4 for Timestamp Details
     *  @param val The Timestamp Details (null if not specified)
     *  @param  cif7field (Optional) Indicator field for the CIF7 attribute.
     */
    public: inline void setTimestampDetails (int64_t val, IndicatorFieldEnum_t cif7field=CIF_NULL) {
      if (!isNull(val))
        val = val&0xFF07FFFFFFFFFFFF; // force reserved bits to 0
      setX(TIMESTAMP_DETAILS,val, cif7field);
    }

    /* XXX END OF CIF3 FUNCTIONS XXX */

    /* XXX START OF CIF7 FUNCTIONS XXX */
    public: inline void setCIF7Attribute (IndicatorFieldEnum_t cif7field, bool set, bool occurrence=false) {
      if (getCIFNumber(cif7field) != 7) throw VRTException("setCIF7Attribute: Invalid CIF7 field");
      int32_t cif7bit = getCIFBitMask(cif7field);
      setCIF7Bit(cif7bit, set, occurrence);
    }
    protected: virtual void setCIF7Bit (int32_t cif7bit, bool set, bool occurrence=false) = 0;

    // null if no cif7, true if cif7 and bit set, false if cif7 and bit not set
    public: inline boolNull getCIF7Attribute (IndicatorFieldEnum_t cif7field, bool occurrence=false) const {
      if (getCIFNumber(cif7field) != 7) throw VRTException("getCIF7Attribute: Invalid CIF7 field");
      int32_t cif7bit = getCIFBitMask(cif7field);
      return getCIF7Bit(cif7bit, occurrence);
    }
    protected: inline boolNull getCIF7Bit (int32_t cif7bit, bool occurrence=false) const {
      return getContextIndicatorFieldBit(7 | (((int8_t)occurrence)<<3), cif7bit);
    }

    /** Set CIF7 BELIEF Attribute of a field
     *  Conveys a factor conveying the degree of confidence that the probability
     *  is accurate.
     *  Bits 31..8: Reserved (0x0)
     *  Bits 7..0:  Belief %
     *    0xFF = 100%
     *    0x00 = 0%
     *    0x01 = 1/255 of 100%
     *  @param field IndicatorFieldEnum of target field
     *  @param val BELIEF value
     */
    public: inline void setCIF7Belief(IndicatorFieldEnum_t field, int32_t val) {
      if (!isNull(val)) val&=0x000000FF;
      setL(field, val, BELIEF);
    }
    /** Get CIF7 BELIEF Attribute of a field
     *  Conveys a factor conveying the degree of confidence that the probability
     *  is accurate.
     *  Bits 31..8: Reserved (0x0)
     *  Bits 7..0:  Belief %
     *    0xFF = 100%
     *    0x00 = 0%
     *    0x01 = 1/255 of 100%
     *  @param  field IndicatorFieldEnum of target field
     *  @return val BELIEF value
     */
    public: inline int32_t getCIF7Belief(IndicatorFieldEnum_t field) const {
      return getL(field, BELIEF);
    }
    /** Set CIF7 PROBABILITY Attribute of a field
     *  Conveys the probability of the selected field of being accurate or true.
     *  Bits 31..16: Reserved (0x0)
     *  Bits 15..8:  Probability Function
     *    0x00 = Uniformed Distribution
     *    0x01 = Normal Distribution
     *    0x02 to 0xFF = User Defined
     *  Bits 7..0:   Probability %
     *    0xFF = 100%
     *    0x00 = 0%
     *    0x01 = 1/255 of 100%
     *  @param field IndicatorFieldEnum of target field
     *  @param val PROBABILITY value
     */
    public: inline void setCIF7Probability(IndicatorFieldEnum_t field, int32_t val) {
      if (!isNull(val)) val&=0x0000FFFF;
      setL(field, val, PROBABILITY);
    }
    /** Get CIF7 PROBABILITY Attribute of a field
     *  Conveys the probability of the selected field of being accurate or true.
     *  Bits 31..16: Reserved (0x0)
     *  Bits 15..8:  Probability Function
     *    0x00 = Uniformed Distribution
     *    0x01 = Normal Distribution
     *    0x02 to 0xFF = User Defined
     *  Bits 7..0:   Probability %
     *    0xFF = 100%
     *    0x00 = 0%
     *    0x01 = 1/255 of 100%
     *  @param  field IndicatorFieldEnum of target field
     *  @return val PROBABILITY value
     */
    public: inline int32_t getCIF7Probability(IndicatorFieldEnum_t field) const {
      return getL(field, PROBABILITY);
    }

    // BELIEF_mask                = 0x00080000; // 1 (See V49.2 spec Section 9.12 rules)
    // PROBABILITY_mask           = 0x00100000; // 1 (See V49.2 spec Section 9.12 rules)
    // THIRD_DERIVATIVE_mask      = 0x00200000; // same as field it describes
    // SECOND_DERIVATIVE_mask     = 0x00400000; // same as field it describes
    // FIRST_DERIVATIVE_mask      = 0x00800000; // same as field it describes
    // ACCURACY_mask              = 0x01000000; // same as field it describes
    // PRECISION_mask             = 0x02000000; // same as field it describes
    // MIN_VALUE_mask             = 0x04000000; // same as field it describes
    // MAX_VALUE_mask             = 0x08000000; // same as field it describes
    // STANDARD_DEVIATION_mask    = 0x10000000; // same as field it describes
    // MEDIAN_VALUE_mask          = 0x20000000; // same as field it describes
    // AVERAGE_VALUE_mask         = 0x40000000; // same as field it describes
    // CURRENT_VALUE_mask         = 0x80000000; // same as field it describes
    /* XXX END OF CIF7 FUNCTIONS XXX */


  }; /* class IndicatorFieldProvider */

} END_NAMESPACE /* vrt namespace */

#endif /* _IndicatorFields_h */

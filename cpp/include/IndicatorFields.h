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
#include <map>
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
      CIF1_RESERVED_26,      ///< Bit 26  Reserved                              (1,0x04000000)
      CIF1_RESERVED_27,      ///< Bit 27  Reserved                              (1,0x08000000)
      PNT_ANGL_2D_ST,        ///< Bit 28  2-D Pointing Angle (structured)       (1,0x10000000)
      PNT_ANGL_2D_SI,        ///< Bit 29  2-D Pointing Angle (simple)           (1,0x20000000)
      POLARIZATION,          ///< Bit 30  Polarization                          (1,0x40000000)
      PHASE,                 ///< Bit 31  Phase                                 (1,0x80000000)
      
      // CIF2 - Identifiers (tags) - Starts at 2*32 = 64
      CIF2_RESERVED_0 = 64,  ///< Bit 0   Reserved                              (2,0x00000001) <==> 010 00000 =  64
      SPATIAL_REF_TYPE,      ///< Bit 1   Spatial Reference Type                (2,0x00000002)
      SPATIAL_SCAN_TYPE,     ///< Bit 2   Spatial Scan Type                     (2,0x00000004)
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
      static const int32_t CONTEXT_ASOC_mask          = 0x00000100; // variable (Section 9.13.2)
      static const int32_t GPS_ASCII_mask             = 0x00000200; // variable (Section 9.4.7)
      static const int32_t EPHEM_REF_mask             = 0x00000400; // 1
      static const int32_t REL_EPHEM_mask             = 0x00000800; // 11 (Section 9.4.10)
      static const int32_t ECEF_EPHEM_mask            = 0x00001000; // 13
      static const int32_t INS_EPHEM_mask             = 0x00002000; // 11
      static const int32_t GPS_EPHEM_mask             = 0x00004000; // 11
      static const int32_t DATA_FORMAT_mask           = 0x00008000; // 2 (Section 9.13.3)
      static const int32_t STATE_EVENT_mask           = 0x00010000; // 1 (Section 9.10 incorrectly listed as CIF1/16)
      static const int32_t DEVICE_ID_mask             = 0x00020000; // 2
      static const int32_t TEMPERATURE_mask           = 0x00040000; // 1 (Section 9.10 incorrectly listed as CIF1/18)
      static const int32_t TIME_CALIB_mask            = 0x00080000; // 1 (Section 9.7.3.3)
      static const int32_t TIME_ADJUST_mask           = 0x00100000; // 2 (Section 9.7.3.1)
      static const int32_t SAMPLE_RATE_mask           = 0x00200000; // 2
      static const int32_t OVER_RANGE_mask            = 0x00400000; // 1
      static const int32_t GAIN_mask                  = 0x00800000; // 1
      static const int32_t REF_LEVEL_mask             = 0x01000000; // 1
      static const int32_t IF_OFFSET_mask             = 0x02000000; // 2
      static const int32_t RF_OFFSET_mask             = 0x04000000; // 2
      static const int32_t RF_FREQ_mask               = 0x08000000; // 2
      static const int32_t IF_FREQ_mask               = 0x10000000; // 2
      static const int32_t BANDWIDTH_mask             = 0x20000000; // 2
      static const int32_t REF_POINT_mask             = 0x40000000; // 1 (Section 9.2)
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
      static const int32_t BUFFER_SZ_mask             = 0x00000002; // 1
      static const int32_t VER_BLD_CODE_mask          = 0x00000004; // 1
      static const int32_t V49_COMPL_mask             = 0x00000008; // 1
      static const int32_t HEALTH_STATUS_mask         = 0x00000010; // 1 (Section 9.10)
      static const int32_t DISCRETE_IO64_mask         = 0x00000020; // 2
      static const int32_t DISCRETE_IO32_mask         = 0x00000040; // 1
      static const int32_t INDEX_LIST_mask            = 0x00000080; // variable (Section 9.3.2)
      static const int32_t CIF1_RESERVED_8_mask       = 0x00000100; // N/A (0)
      static const int32_t SECTOR_SCN_STP_mask        = 0x00000200; // variable (Section 9.6.2)
      static const int32_t SPECTRUM_mask              = 0x00000400; // 14
      static const int32_t CIFS_ARRAY_mask            = 0x00000800; // variable (Section 9.13.1)
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
      static const int32_t CIF1_RESERVED_26_mask      = 0x04000000; // N/A (0)
      static const int32_t CIF1_RESERVED_27_mask      = 0x08000000; // N/A (0)
      static const int32_t PNT_ANGL_2D_ST_mask        = 0x10000000; // variable (Section 9.4.1)
      static const int32_t PNT_ANGL_2D_SI_mask        = 0x20000000; // 1
      static const int32_t POLARIZATION_mask          = 0x40000000; // 1
      static const int32_t PHASE_mask                 = 0x80000000; // 1

      /** define field lengths */
      static const int32_t CTX_4_OCTETS = PHASE_mask | POLARIZATION_mask | PNT_ANGL_2D_SI_mask | BEAMWIDTH_mask | RANGE_mask
                                        | EB_NO_BER_mask | THRESHOLD_mask | COMPRESS_PT_mask | ICPT_PTS_2_3_mask
                                        | SNR_NOISE_mask | AUX_GAIN_mask | DISCRETE_IO32_mask | HEALTH_STATUS_mask
                                        | V49_COMPL_mask | VER_BLD_CODE_mask | BUFFER_SZ_mask;
      static const int32_t CTX_8_OCTETS = AUX_FREQUENCY_mask | AUX_BANDWIDTH_mask | DISCRETE_IO64_mask;
      static const int32_t CTX_56_OCTETS = SPECTRUM_mask;
      // others (variable) :
      //                     PNT_ANGL_2D_ST_mask TODO
      //                     CIFS_ARRAY_mask TODO
      //                     SECTOR_SCN_STP_mask TODO
      //                     INDEX_LIST_mask TODO
      //         Note TODO : These also matter for others that come after

    } END_NAMESPACE // protected_CIF1 namespace

    /** <b>Internal Use Only:</b> Bitmasks for CIF2 fields (Identifiers (tags)).
     */
    namespace protected_CIF2 {
      
      /** Bitmasks                                                     Field Size (# of 32-bit words) */
      static const int32_t CIF2_RESERVED_0_mask       = 0x00000001; // N/A (0)
      static const int32_t SPATIAL_REF_TYPE_mask      = 0x00000002; // 1 (Section 9.8.11 for the next 2)
      static const int32_t SPATIAL_SCAN_TYPE_mask     = 0x00000004; // 1
      static const int32_t RF_FOOTPRINT_RANGE_mask    = 0x00000008; // 1 (Section 9.8.12 for the next 2)
      static const int32_t RF_FOOTPRINT_mask          = 0x00000010; // 1
      static const int32_t COMM_PRIORITY_ID_mask      = 0x00000020; // 1 (Section 9.8.10 for the next 5)
      static const int32_t FUNCT_PRIORITY_ID_mask     = 0x00000040; // 1
      static const int32_t EVENT_ID_mask              = 0x00000080; // 1
      static const int32_t MODE_ID_mask               = 0x00000100; // 1
      static const int32_t FUNCTION_ID_mask           = 0x00000200; // 1
      static const int32_t MODULATION_TYPE_mask       = 0x00000400; // 1 (Section 9.8.9 for the next 5)
      static const int32_t MODULATION_CLASS_mask      = 0x00000800; // 1
      static const int32_t EMS_DEVICE_INSTANCE_mask   = 0x00001000; // 1
      static const int32_t EMS_DEVICE_TYPE_mask       = 0x00002000; // 1
      static const int32_t EMS_DEVICE_CLASS_mask      = 0x00004000; // 1
      static const int32_t PLATFORM_DISPLAY_mask      = 0x00008000; // 1 (Section 9.8.8 for the next 3)
      static const int32_t PLATFORM_INSTANCE_mask     = 0x00010000; // 1
      static const int32_t PLATFORM_CLASS_mask        = 0x00020000; // 1
      static const int32_t OPERATOR_mask              = 0x00040000; // 1 (Section 9.8.7 for the next 2)
      static const int32_t COUNTRY_CODE_mask          = 0x00080000; // 1
      static const int32_t TRACK_ID_mask              = 0x00100000; // 1 (Section 9.8.6)
      static const int32_t INFORMATION_SOURCE_mask    = 0x00200000; // 1 (Section 9.8.5)
      static const int32_t CONTROLLER_UUID_mask       = 0x00400000; // 4 (Section 9.8.3 for the next 5)
      static const int32_t CONTROLLER_ID_mask         = 0x00800000; // 1
      static const int32_t CONTROLLEE_UUID_mask       = 0x01000000; // 4
      static const int32_t CONTROLLEE_ID_mask         = 0x02000000; // 1
      static const int32_t CITED_MESSAGE_ID_mask      = 0x04000000; // 1 (more info in Section 9.8.4 too)
      static const int32_t CHILDREN_SID_mask          = 0x08000000; // 1 (Section 9.8.2 for the next 4)
      static const int32_t PARENTS_SID_mask           = 0x10000000; // 1
      static const int32_t SIBLINGS_SID_mask          = 0x20000000; // 1
      static const int32_t CITED_SID_mask             = 0x40000000; // 1
      static const int32_t BIND_mask                  = 0x80000000; // 1 (Section 9.8.1)
      
      /** define field lengths */
      static const int32_t CTX_4_OCTETS  = BIND_mask | CITED_SID_mask | SIBLINGS_SID_mask | PARENTS_SID_mask | CHILDREN_SID_mask
                                         | CITED_MESSAGE_ID_mask | CONTROLLEE_ID_mask | CONTROLLER_ID_mask
                                         | INFORMATION_SOURCE_mask | TRACK_ID_mask | COUNTRY_CODE_mask | OPERATOR_mask
                                         | PLATFORM_CLASS_mask | PLATFORM_INSTANCE_mask | PLATFORM_DISPLAY_mask
                                         | EMS_DEVICE_CLASS_mask | EMS_DEVICE_TYPE_mask | EMS_DEVICE_INSTANCE_mask
                                         | MODULATION_CLASS_mask | MODULATION_TYPE_mask | FUNCTION_ID_mask | MODE_ID_mask
                                         | EVENT_ID_mask | FUNCT_PRIORITY_ID_mask | COMM_PRIORITY_ID_mask | RF_FOOTPRINT_mask
                                         | RF_FOOTPRINT_RANGE_mask | SPATIAL_SCAN_TYPE_mask | SPATIAL_REF_TYPE_mask;
      static const int32_t CTX_16_OCTETS = CONTROLLEE_UUID_mask | CONTROLLER_UUID_mask;

    } END_NAMESPACE // protected_CIF2 namespace

    /** <b>Internal Use Only:</b> Bitmasks for CIF3 fields (Temporal and
     * Environmental fields).
     */
    namespace protected_CIF3 {

      /** Bitmasks                                                     Field Size (# of 32-bit words) */
      static const int32_t CIF3_RESERVED_0_mask       = 0x00000001; // N/A (0)
      static const int32_t NETWORK_ID_mask            = 0x00000002; // 1 (Section 9.8.13)
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
                                                                    // For the next two, see Section 9.7.2
      static const int32_t SHELF_LIFE_mask            = 0x00010000; // 1, 2, or 3 (same as packet tstamp based on TSI and TSF)
      static const int32_t AGE_mask                   = 0x00020000; // 1, 2, or 3 (same as packet tstamp based on TSI and TSF)
      static const int32_t CIF3_RESERVED_18_mask      = 0x00040000; // N/A (0)
      static const int32_t CIF3_RESERVED_19_mask      = 0x00080000; // N/A (0)
      static const int32_t JITTER_mask                = 0x00100000; // 2 (Section 9.7.1 for the next 8)
      static const int32_t DWELL_mask                 = 0x00200000; // 2
      static const int32_t DURATION_mask              = 0x00400000; // 2
      static const int32_t PERIOD_mask                = 0x00800000; // 2
      static const int32_t PULSE_WIDTH_mask           = 0x01000000; // 2
      static const int32_t OFFSET_TIME_mask           = 0x02000000; // 2
      static const int32_t FALL_TIME_mask             = 0x04000000; // 2
      static const int32_t RISE_TIME_mask             = 0x08000000; // 2
      static const int32_t CIF3_RESERVED_28_mask      = 0x10000000; // N/A (0)
      static const int32_t CIF3_RESERVED_29_mask      = 0x20000000; // N/A (0)
      static const int32_t TIMESTAMP_SKEW_mask        = 0x40000000; // 2 (Section 9.7.3.2)
      static const int32_t TIMESTAMP_DETAILS_mask     = 0x80000000; // 2 (Section 9.7.3.4)

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
      static const int32_t BELIEF_mask                = 0x00080000; // 1 (see Section 9.12 rules)
      static const int32_t PROBABILITY_mask           = 0x00100000; // 1 (see Section 9.12 rules)
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

      /** Attribute sizes (Section 9.12) */
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
    // A negative return value means attribute not present.
    // A NULL return value means the entire CIF is not present.
    // @return offset to attribute of specified field
    // @throws VRTException when field has invalid length (size)
    // @throws VRTException when field is not present (TODO - or return NULL?)
    protected: virtual int32_t getOffset (int8_t cifNum, int32_t field, int32_t cif7) const {
      int32_t fieldOffset = getOffset(cifNum, field);
      if (fieldOffset < 0) throw VRTException("Cannot get attribute of a field that is not present.");
      int32_t fieldLen = getFieldLen(cifNum, field);
      if (fieldLen <= 0) throw VRTException("Cannot get attribute of a field with invalid length.");
      int32_t cif7Offset = getCif7Offset(cif7, fieldLen);
      if (cif7Offset < 0) fieldOffset = -fieldOffset;
      return fieldOffset + cif7Offset;
    }
    // XXX this may cause ambiguity since both args are ints, same as first getOffset defined above
    protected: virtual int32_t getOffset (IndicatorFieldEnum_t field, IndicatorFieldEnum_t cif7) const {
      return getOffset(getCIFNumber(field), getCIFBitMask(field), getCIFBitMask(cif7));
    }
    // Used for getting second occurrence of CIFs (i.e. Ack packets have second occurrence for errors (first is for warnings)
    protected: virtual int32_t getOffset (IndicatorFieldEnum_t field, bool occurrence) const {
      return getOffset(getCIFNumber(field) | (((int8_t)occurrence)<<3), getCIFBitMask(field));
    }
    // Combo of cif7 attributes and second occurrence
    // XXX - if this is only ever used with occurrence=true, replace with new function name that does it without occurrence arg
    protected: virtual int32_t getOffset (IndicatorFieldEnum_t field, IndicatorFieldEnum_t cif7, bool occurrence) const {
      return getOffset(getCIFNumber(field) | (((int8_t)occurrence)<<3), getCIFBitMask(field), getCIFBitMask(cif7));
    }

    /** Gets the offset of the CIF7 attribute
     *  Offset is from the start of a field of size specified.
     *  @param attr CIF7 attribute bitmask
     *  @param len Length of the field
     *  @return Offset from start of field
     *  @throws VRTException for classes that do not support CIF7.
     */
    protected: virtual int32_t getCif7Offset (int32_t attr, int32_t len) const {
      UNUSED_VARIABLE(attr);
      UNUSED_VARIABLE(len);
      // Default impl throws exception for classes that don't support CIF7
      throw VRTException("Class does not support CIF7.");
    }

    /** Gets the length of the given field when present (-1 if variable, -2 if not found). */
    // TODO - CIF7 implications on below?
    protected: virtual int32_t getFieldLen (int8_t cifNum, int32_t field) const = 0;
    protected: virtual int32_t getFieldLen (IndicatorFieldEnum_t field) const {
      return getFieldLen(getCIFNumber(field), getCIFBitMask(field));
    }

    /** Unpacks a 16-bit integer from the payload at the indicated position. */
    protected: virtual int8_t getB (int8_t cifNum, int32_t bit, int32_t xoff) const = 0;
    protected: virtual int8_t getB (IndicatorFieldEnum_t field, int32_t xoff) const {
      return getB(getCIFNumber(field), getCIFBitMask(field), xoff);
    }

    /** Packs a 16-bit integer from the payload at the indicated position. */
    protected: virtual void setB (int8_t cifNum, int32_t bit, int32_t xoff, int8_t val) = 0;
    protected: virtual void setB (IndicatorFieldEnum_t field, int32_t xoff, int8_t val) {
      setB(getCIFNumber(field), getCIFBitMask(field), xoff, val);
    }

    /** Unpacks a 16-bit integer from the payload at the indicated position. */
    protected: virtual int16_t getI (int8_t cifNum, int32_t bit, int32_t xoff) const = 0;
    protected: virtual int16_t getI (IndicatorFieldEnum_t field, int32_t xoff) const {
      return getI(getCIFNumber(field), getCIFBitMask(field), xoff);
    }

    /** Packs a 16-bit integer from the payload at the indicated position. */
    protected: virtual void setI (int8_t cifNum, int32_t bit, int32_t xoff, int16_t val) = 0;
    protected: virtual void setI (IndicatorFieldEnum_t field, int32_t xoff, int16_t val) {
      setI(getCIFNumber(field), getCIFBitMask(field), xoff, val);
    }

    /** Unpacks a 32-bit integer from the payload at the indicated position. */
    protected: virtual int32_t getL (int8_t cifNum, int32_t bit) const = 0;
    protected: virtual int32_t getL (IndicatorFieldEnum_t field) const {
      return getL(getCIFNumber(field), getCIFBitMask(field));
    }

    /** Unpacks a 32-bit integer from the payload at the indicated position. */
    protected: virtual int32_t getL24 (int8_t cifNum, int32_t bit, int32_t offset) const = 0;
    protected: virtual int32_t getL24 (IndicatorFieldEnum_t field, int32_t offset) const {
      return getL24(getCIFNumber(field), getCIFBitMask(field), offset);
    }

    /** Packs a 32-bit integer from the payload at the indicated position. */
    protected: virtual void setL (int8_t cifNum, int32_t bit, int32_t val) = 0;
    protected: virtual void setL (IndicatorFieldEnum_t field, int32_t val) {
      setL(getCIFNumber(field), getCIFBitMask(field), val);
    }

    /** Unpacks a 64-bit integer from the payload at the indicated position. */
    protected: virtual int64_t getX (int8_t cifNum, int32_t bit) const = 0;
    protected: virtual int64_t getX (IndicatorFieldEnum_t field) const {
      return getX(getCIFNumber(field), getCIFBitMask(field));
    }

    /** Packs a 64-bit integer from the payload at the indicated position. */
    protected: virtual void setX (int8_t cifNum, int32_t bit, int64_t val) = 0;
    protected: virtual void setX (IndicatorFieldEnum_t field, int64_t val) {
      setX(getCIFNumber(field), getCIFBitMask(field), val);
    }

    /** Sets a block of data. */
    protected: inline void setRecord (int8_t cifNum, int32_t bit, const Record &val, const Record &old) {
      if (isNull(old)) setRecord(cifNum, bit, val, -1);
      else             setRecord(cifNum, bit, val, old.getByteLength());
    }
    protected: inline void setRecord (IndicatorFieldEnum_t field, const Record &val, const Record &old) {
      setRecord(getCIFNumber(field), getCIFBitMask(field), val, old);
    }

    /** Sets a block of data. */
    protected: inline void setRecord (int8_t cifNum, int32_t bit, const Record &val) {
      setRecord(cifNum, bit, val, getFieldLen(cifNum, bit));
    }
    protected: inline void setRecord (IndicatorFieldEnum_t field, const Record &val) {
      setRecord(getCIFNumber(field), getCIFBitMask(field), val);
    }

    /** Sets a block of data. */
    protected: inline void setRecord (int8_t cifNum, int32_t bit, const Record *val) {
      setRecord(cifNum, bit, val, getFieldLen(cifNum, bit));
    }
    protected: inline void setRecord (IndicatorFieldEnum_t field, const Record *val) {
      setRecord(getCIFNumber(field), getCIFBitMask(field), val);
    }

    /** Sets a block of data. */
    protected: inline void setRecord (int8_t cifNum, int32_t bit, const Record &val, int32_t oldLen) {
      if (isNull(val)) setRecord(cifNum, bit, NULL, oldLen);
      else             setRecord(cifNum, bit, &val, oldLen);
    }
    protected: inline void setRecord (IndicatorFieldEnum_t field, const Record &val, int32_t oldLen) {
      setRecord(getCIFNumber(field), getCIFBitMask(field), val, oldLen);
    }

    /** Gets a block of data. */
    protected: virtual void setRecord (int8_t cifNum, int32_t bit, const Record *val, int32_t oldLen) = 0;
    
    protected: void setRecord (IndicatorFieldEnum_t field, const Record *val, int32_t oldLen) {
      setRecord(getCIFNumber(field), getCIFBitMask(field), val, oldLen);
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
      int32_t cif = getContextIndicatorField(cifNum);
      if (isNull(cif)) return _NULL;
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

    /** Adds or removes a CIF
     *  Sets the Enable Indicator of CIF0 and adds or removes 4-bytes for the CIF.
     *  @param add True = add CIF (default), False = remove CIF
     *  @param occurrence False (0) if first occurrence (default; almost everything),
     *                    True (1) if second occurrence (only Errors for BasicAcknowledgePacket)
     */
    public: virtual void addCIF1 (bool add=true, bool occurrence=false) = 0;
    public: virtual void addCIF2 (bool add=true, bool occurrence=false) = 0;
    public: virtual void addCIF3 (bool add=true, bool occurrence=false) = 0;
    public: virtual void addCIF7 (bool add=true, bool occurrence=false) = 0;

    /** Gets the Context Field Change Indicator.
     *  @return true if <b>anything</b> in the packet has changed since the last context packet,
     *          false if <b>nothing</b> has changed since the last context packet.
     */
    public: inline bool isChangePacket () const {
      return ((getContextIndicatorField0() & protected_CIF0::CHANGE_IND_mask) != 0);
    }

    /** Gets the Reference Point Identifier.
     *  @return The point in the system where this context applies (null if not specified).
     */
    public: inline int32_t getReferencePointIdentifier () const {
      return getL(REF_POINT);
    }

    /** Gets the Timestamp Adjustment in picoseconds. This is the required time adjustment
     *  between the time the signal was digitized (i.e. the timestamp) and the time at the reference
     *  point.
     *  @return The timestamp adjustment (null if not specified).
     */
    public: inline int64_t getTimeStampAdjustment () const {
      return getX(TIME_ADJUST);
    }

    /** Gets the Timestamp Calibration Time in seconds. This is the most recent date and
     *  time when the timestamp in the Data and Context packets was known to be correct.
     *  @return The calibration time (null if not specified).
     */
    public: inline int32_t getTimeStampCalibration () const {
      return getL(TIME_CALIB);
    }

    /** Gets the Ephemeris Reference Identifier. This is used specifies the process
     *  whose origin applies to the ephemeris returned by {@link #getEphemerisRelative()}.
     *  @return The ephemeris information (null if not specified). <i>Note that changes to the
     *          returned object do not alter the packet.</i>
     */
    public: inline int32_t getEphemerisReference () const { return getL(EPHEM_REF); }

    /** Used for unpacking geolocation records. */
    //protected: virtual Geolocation getGeolocation (IndicatorFieldEnum_t field) const = 0;
    protected: virtual Geolocation getGeolocation (int8_t cifNum, int32_t field) const = 0;

    /** Used for unpacking ephemeris records. */
    //private: virtual Ephemeris getEphemeris (IndicatorFieldEnum_t field) const = 0;
    protected: virtual Ephemeris getEphemeris (int8_t cifNum, int32_t field) const = 0;

    /** Gets the Formatted GPS (Global Positioning System) Geolocation for the collector.
     *  @return The geolocation information (null if not specified). <i>Note that changes to the
     *          returned object do not alter the packet.</i>
     */
    public: inline Geolocation getGeolocationGPS () const {
      //return getGeolocation(GPS_EPHEM);
      return getGeolocation(0, protected_CIF0::GPS_EPHEM_mask);
    }

    /** Gets the Formatted INS (Inertial Navigation System) Geolocation for the collector.
     *  @return The geolocation information (null if not specified). <i>Note that changes to the
     *          returned object do not alter the packet.</i>
     */
    public: inline Geolocation getGeolocationINS () const {
      //return getGeolocation(INS_EPHEM);
      return getGeolocation(0, protected_CIF0::INS_EPHEM_mask);
    }

    /** Gets the ECEF (Earth-Centered, Earth-Fixed) Ephemeris for the collector.
     *  @return The ephemeris information (null if not specified). <i>Note that changes to the
     *          returned object do not alter the packet.</i>
     */
    public: inline Ephemeris getEphemerisECEF () const {
      //return getEphemeris(ECEF_EPHEM);
      return getEphemeris(0, protected_CIF0::ECEF_EPHEM_mask);
    }

    /** Gets the Relative Ephemeris for the collector. Unlike {@link #getEphemerisECEF()}
     *  which uses the ECEF coordinate system, this ephemeris is relative to a user-defined system
     *  specified by {@link #getEphemerisReference()}.
     *  @return The ephemeris information (null if not specified). <i>Note that changes to the
     *          returned object do not alter the packet.</i>
     */
    public: inline Ephemeris getEphemerisRelative () const {
      //return getEphemeris(REL_EPHEM);
      return getEphemeris(0, protected_CIF0::REL_EPHEM_mask);
    }

    /** Gets the GPS ASCII "sentences". These are ASCII "sentences" from a GPS (Global
     *  Positioning System) receiver, such as those conforming to NMEA-0183. <i>Due to the great
     *  variety in GPS "sentences", this class makes little effort to do anything useful with them.
     *  Users are encouraged to use {@link #getGeolocationGPS()} which essentially provides the same
     *  information, but in a consistent form.</i>
     *  @return The geolocation information (null if not specified). <i>Note that changes to the
     *          returned object do not alter the packet.</i>
     */
    public: virtual GeoSentences getGeoSentences () const = 0;

    /** Gets the Context Association Lists. These lists indicate the other context/data
     *  streams associated with this one.
     *  @return The context association lists (null if not specified). <i>Note that changes to the
     *          returned object do not alter the packet.</i>
     */
    public: virtual ContextAssocLists getContextAssocLists () const = 0;

    /** Gets the Device Identifier specifying the manufacturer and model of the device
     *  producing this context packet stream.
     *  @return The device identifier as a string (null if not specified).
     */
    public: inline string getDeviceID () const { return Utilities::toStringDeviceID(getDeviceIdentifier()); }

    /** Gets the Bandwidth of the signal in Hz.
     *  @return The bandwidth (null if not specified).
     */
    public: inline double getBandwidth () const {
      int64_t bits = getX(BANDWIDTH);
      return (isNull(bits))? DOUBLE_NAN : VRTMath::toDouble64(20,bits);
    }

    /** Gets the IF Reference Frequency of the signal in Hz.
     *  @return The frequency (null if not specified).
     */
    public: inline double getFrequencyIF () const {
      int64_t bits = getX(IF_FREQ);
      return (isNull(bits))? DOUBLE_NAN : VRTMath::toDouble64(20,bits);
    }

    /** Gets the RF Reference Frequency of the signal in Hz.
     *  @return The frequency (null if not specified).
     */
    public: inline double getFrequencyRF () const {
      int64_t bits = getX(RF_FREQ);
      return (isNull(bits))? DOUBLE_NAN : VRTMath::toDouble64(20,bits);
    }

    /** Gets the RF Reference Frequency Offset of the signal in Hz.
     *  @return The frequency offset (null if not specified).
     */
    public: inline double getFrequencyOffsetRF () const {
      int64_t bits = getX(RF_OFFSET);
      return (isNull(bits))? DOUBLE_NAN : VRTMath::toDouble64(20,bits);
    }

    /** Gets the IF Band Offset of the signal in Hz.
     *  @return The band offset (null if not specified).
     */
    public: inline double getBandOffsetIF () const {
      int64_t bits = getX(IF_OFFSET);
      return (isNull(bits))? DOUBLE_NAN : VRTMath::toDouble64(20,bits);
    }

    /** Gets the Reference Level of the signal in dBm.
     *  @return The reference level (null if not specified).
     */
    public: inline float getReferenceLevel () const {
      int16_t bits = getI(REF_LEVEL,2);
      return (isNull(bits))? FLOAT_NAN : VRTMath::toFloat16(7,bits);
    }

    /** Gets the Stage 1 Gain of the device in dB. This is the front-end gain of the
     *  system. In cases where a separate Stage 1 and Stage 2 gain is not necessary, this holds
     *  the total gain of the system and Gain 2 is set to zero.
     *  @return The gain (null if not specified).
     */
    public: inline float getGain1 () const {
      int16_t bits = getI(GAIN,2);
      return (isNull(bits))? FLOAT_NAN : VRTMath::toFloat16(7,bits);
    }

    /** Gets the Stage 2 Gain of the device in dB. This is the back-end gain of the
     *  system. In cases where a separate Stage 1 and Stage 2 gain is not necessary, the Stage 1
     *  gain holds the total gain of the system and this is set to zero.
     *  @return The gain (null if not specified).
     */
    public: inline float getGain2 () const {
      int16_t bits = getI(GAIN,0);
      return (isNull(bits))? FLOAT_NAN : VRTMath::toFloat16(7,bits);
    }

    /** Gets the Sample Rate in Hz.
     *  @return The sample rate (null if not specified).
     */
    public: inline double getSampleRate () const {
      int64_t bits = getX(SAMPLE_RATE);
      return (isNull(bits))? DOUBLE_NAN : VRTMath::toDouble64(20,bits);
    }

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
    public: inline float getTemperature () const {
      int16_t bits = getI(TEMPERATURE,2);
      return (isNull(bits))? FLOAT_NAN : VRTMath::toFloat16( 6,bits);
    }

    /** Gets the specified bit from the State and Event Indicator field.
     *  @param enable    Bit position of the enable flag.
     *  @param indicator Bit position of the indicator flag.
     *  @return null if field is not present, null if the enable bit is not set, true if the enable
     *          bit is set and the indicator bit is set, false if the enable bit is set but the
     *          indicator bit is not set.
     */
    protected: virtual boolNull getStateEventBit (int32_t enable, int32_t indicator) const = 0;

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
    public: inline boolNull isCalibratedTimeStamp () const {
      return getStateEventBit(31, 19);
    }

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
    public: inline boolNull isDataValid () const {
      return getStateEventBit(30, 18);
    }

    /** Gets the reference lock indicator flag.
     *  <pre>
     *    true  = All of the phase-locked loops affecting the data are locked and stable
     *    false = At least one of the phase-locked loops affecting the data is not locked and stable
     *    null  = <i>This information is not available</i>
     *  </pre>
     *  @return true if the flag is set, false if it is not set, null if this optional flag isn't
     *          specified (null is returned for all context packets).
     */
    public: inline boolNull isReferenceLocked () const {
      return getStateEventBit(29, 17);
    }

    /** Gets the AGC/MGC indicator flag.
     *  <pre>
     *    true  = AGC (automatic gain control) is being used
     *    false = MGC (manual gain control) is being used
     *    null  = <i>This information is not available</i>
     *  </pre>
     *  @return true if the flag is set, false if it is not set, null if this optional flag isn't
     *          specified (null is returned for all context packets).
     */
    public: inline boolNull isAutomaticGainControl () const {
      return getStateEventBit(28, 16);
    }

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
    public: inline boolNull isSignalDetected () const {
      return getStateEventBit(27, 15);
    }

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
    public: inline boolNull isInvertedSpectrum () const {
      return getStateEventBit(26, 14);
    }

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
    public: inline boolNull isOverRange () const {
      return getStateEventBit(25, 13);
    }

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
    public: inline boolNull isDiscontinuous () const {
      return getStateEventBit(24, 12);
    }

    /** Gets the User-Defined Bits from the State and Event Indicator Bits. <i>The
     *  definition of these bits should be included in the documentation for the relevent packet
     *  class.</i> Unlike other bits of the State and Event Indicator Field, there is no concept
     *  of an undefined-value for user bits if the State and Event Indicator Field is present to
     *  support some other bits. As such, zero will be returned if the State and Event Indicator
     *  Field is absent.
     *  @return true if the flag is set, false if it is not set, zero if the State and Event
     *  Indicator Field is absent.
     */
    public: inline int32_t getUserDefinedBits () const {
      int8_t bits = getB(STATE_EVENT,3);
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
     */
    public: inline void setReferencePointIdentifier (int32_t val) {
      setL(REF_POINT, val);
    }

    /** Sets the specified bit of the State and Event Indicator field.
     *  @param enable    Bit position of the enable flag.
     *  @param indicator Bit position of the indicator flag.
     *  @param value     The value of the bits: null if the enable bit should not be set, true if
     *                   the enable bit and the indicator bit should be set, false if the enable bit
     *                   should be set but the indicator bit should not be set.
     */
    protected: virtual void setStateEventBit (int32_t enable, int32_t indicator, boolNull value) = 0;

    /** <i>Optional functionality:</i> Sets the calibrated time indicator flag.
     *  @param v true if the flag is set, false if not set, null if this optional flag isn't specified.
     *  @throws VRTException If this method is not supported or if called on a context packet.
     */
    public: inline void setCalibratedTimeStamp (boolNull v) {
      setStateEventBit(31, 19, v);
    }

    /** <i>Optional functionality:</i> Gets the valid data indicator flag.
     *  @param v true if the flag is set, false if not set, null if this optional flag isn't specified.
     *  @throws VRTException If this method is not supported or if called on a context packet.
     */
    public: inline void setDataValid (boolNull v) {
      setStateEventBit(30, 18, v);
    }

    /** <i>Optional functionality:</i> Gets the reference lock indicator flag.
     *  @param v true if the flag is set, false if not set, null if this optional flag isn't specified.
     *  @throws VRTException If this method is not supported or if called on a context packet.
     */
    public: inline void setReferenceLocked (boolNull v) {
      setStateEventBit(29, 17, v);
    }

    /** <i>Optional functionality:</i> Gets the AGC/MGC indicator flag.
     *  @param v true if the flag is set, false if not set, null if this optional flag isn't specified.
     *  @throws VRTException If this method is not supported or if called on a context packet.
     */
    public: inline void setAutomaticGainControl (boolNull v) {
      setStateEventBit(28, 16, v);
    }

    /** <i>Optional functionality:</i> Gets the signal detected indicator flag.
     *  @param v true if the flag is set, false if not set, null if this optional flag isn't specified.
     *  @throws VRTException If this method is not supported or if called on a context packet.
     */
    public: inline void setSignalDetected (boolNull v) {
      setStateEventBit(27, 15, v);
    }

    /** <i>Optional functionality:</i> Gets the spectral inversion indicator flag.
     *  @param v true if the flag is set, false if not set, null if this optional flag isn't specified.
     *  @throws VRTException If this method is not supported or if called on a context packet.
     */
    public: inline void setInvertedSpectrum (boolNull v) {
      setStateEventBit(26, 14, v);
    }

    /** <i>Optional functionality:</i> Gets the over-range indicator flag.
     *  @param v true if the flag is set, false if not set, null if this optional flag isn't specified.
     *  @throws VRTException If this method is not supported or if called on a context packet.
     */
    public: inline void setOverRange (boolNull v) {
      setStateEventBit(25, 13, v);
    }

    /** <i>Optional functionality:</i> Gets the sample loss indicator flag.
     *  @param v true if the flag is set, false if not set, null if this optional flag isn't specified.
     *  @throws VRTException If this method is not supported or if called on a context packet.
     */
    public: inline void setDiscontinuous (boolNull v) {
      setStateEventBit(24, 12, v);
    }

    /** Sets the User-Defined Bits from the State and Event Indicator Bits. <i>The
     *  definition of these bits should be included in the documentation for the relevent packet
     *  class.</i> Unlike other bits of the State and Event Indicator Field, there is no concept
     *  of an undefined-value for user bits if the State and Event Indicator Field is present to
     *  support some other bits. As such, no "null value" is supported for the user-defined buts.
     *  @param val true if the flag is set, false if it is not set.
     */
    public: inline void setUserDefinedBits (int32_t val) {
      if ((getOffset(STATE_EVENT) > 0) || !isNull(val)) {
        setB(STATE_EVENT,3,(char)val);
      }
    }

    /** Sets the Timestamp Adjustment in picoseconds. This is the required time adjustment
     *  between the time the signal was digitized (i.e. the timestamp) and the time at the reference
     *  point.
     *  @param val The timestamp adjustment (null if not specified).
     */
    public: inline void setTimeStampAdjustment (int64_t val) {
      setX(TIME_ADJUST,val);
    }

    /** Sets the Timestamp Calibration Time in seconds. This is the most recent date and
     *  time when the timestamp in the Data and Context packets was known to be correct.
     *  @param val The calibration time (null if not specified).
     */
    public: inline void setTimeStampCalibration (int32_t val) {
      setL(TIME_CALIB,val);
    }

    /** Sets the Ephemeris Reference Identifier. This is used specifies the process
     *  whose origin applies to the ephemeris returned by {@link #getEphemerisRelative()}.
     *  @param val The ephemeris information (null if not specified). <i>Note that changes to the
     *             returned object do not alter the packet.</i>
     */
    public: inline void setEphemerisReference (int32_t val) {
      setL(EPHEM_REF,val);
    }

    /** Sets the Formatted GPS (Global Positioning System) Geolocation for the collector.
     *  @param val The geolocation information (null if not specified). <i>Note that changes to the
     *             returned object do not alter the packet.</i>
     */
    public: inline void setGeolocationGPS (const Geolocation &val) {
      setRecord(GPS_EPHEM,val);
    }

    /** Sets the Formatted INS (Inertial Navigation System) Geolocation for the collector.
     *  @param val The geolocation information (null if not specified). <i>Note that changes to the
     *             returned object do not alter the packet.</i>
     */
    public: inline void setGeolocationINS (const Geolocation &val) {
      setRecord(INS_EPHEM,val);
    }

    /** Sets the ECEF (Earth-Centered, Earth-Fixed) Ephemeris for the collector.
     *  @param val The ephemeris information (null if not specified). <i>Note that changes to the
     *             returned object do not alter the packet.</i>
     */
    public: inline void setEphemerisECEF (const Ephemeris &val) {
      setRecord(ECEF_EPHEM,val);
    }

    /** Sets the Relative Ephemeris for the collector.
     *  @param val The ephemeris information (null if not specified). <i>Note that changes to the
     *             returned object do not alter the packet.</i>
     */
    public: inline void setEphemerisRelative (const Ephemeris &val) {
      setRecord(REL_EPHEM,val);
    }

    /** Sets the GPS ASCII "sentences". These are ASCII "sentences" from a GPS (Global
     *  Positioning System) receiver, such as those conforming to NMEA-0183. <i>Due to the great
     *  variety in GPS "sentences", this class makes little effort to do anything useful with them.
     *  Users are encouraged to use {@link #getGeolocationGPS()} which essentially provides the same
     *  information, but in a consistent form.</i>
     *  @param val The geolocation information (null if not specified). <i>Note that changes to the
     *             returned object do not alter the packet.</i>
     */
    public: inline void setGeoSentences (const GeoSentences &val) {
      setRecord(GPS_ASCII, val, getGeoSentences());
    }

    /** Sets the Context Association Lists. These lists indicate the other context/data
     *  streams associated with this one.
     *  @param val The context association lists (null if not specified). <i>Note that changes to the
     *          returned object do not alter the packet.</i>
     */
    public: inline void setContextAssocLists (const ContextAssocLists &val) {
      setRecord(CONTEXT_ASOC, val, getContextAssocLists());
    }

    /** Sets the Device Identifier specifying the manufacturer and model of the device
     *  producing this context packet stream.
     *  @param val The device identifier as a string (null if not specified).
     */
    public: inline void setDeviceID (const string &val) {
      setDeviceIdentifier(Utilities::fromStringDeviceID(val));
    }

    /** Sets the Bandwidth of the signal in Hz.
     *  @param val The bandwidth (null if not specified).
     */
    public: inline void setBandwidth (double val) {
      int64_t bits = (isNull(val))? INT64_NULL : VRTMath::fromDouble64(20,val);
      setX(BANDWIDTH,bits);
    }

    /** Sets the IF Reference Frequency of the signal in Hz.
     *  @param val The frequency (null if not specified).
     */
    public: inline void setFrequencyIF (double val) {
      int64_t bits = (isNull(val))? INT64_NULL : VRTMath::fromDouble64(20,val);
      setX(IF_FREQ,bits);
    }

    /** Sets the RF Reference Frequency of the signal in Hz.
     *  @param val The frequency (null if not specified).
     */
    public: inline void setFrequencyRF (double val) {
      int64_t bits = (isNull(val))? INT64_NULL : VRTMath::fromDouble64(20,val);
      setX(RF_FREQ,bits);
    }

    /** Sets the RF Reference Frequency Offset of the signal in Hz.
     *  @param val The frequency offset (null if not specified).
     */
    public: inline void setFrequencyOffsetRF (double val) {
      int64_t bits = (isNull(val))? INT64_NULL : VRTMath::fromDouble64(20,val);
      setX(RF_OFFSET,bits);
    }

    /** Sets the IF Band Offset of the signal in Hz.
     *  @param val The band offset (null if not specified).
     */
    public: inline void setBandOffsetIF (double val) {
      int64_t bits = (isNull(val))? INT64_NULL : VRTMath::fromDouble64(20,val);
      setX(IF_OFFSET,bits);
    }

    /** Sets the Reference Level of the signal in dBm.
     *  @param val The reference level (null if not specified).
     */
    public: inline void setReferenceLevel (float val) {
      int16_t bits = (isNull(val))? INT16_NULL : VRTMath::fromFloat16(7,val);
      setI(REF_LEVEL,2,bits);
    }

    /** Sets the Stage 1 Gain of the device in dB. This is the front-end gain of the
     *  system. In cases where a separate Stage 1 and Stage 2 gain is not necessary, this holds
     *  the total gain of the system and Gain 2 is set to zero. <i>If no stage 2 gain has been
     *  specified, it will be set to 0 following the call to this method.</i>
     *  @param val The gain (null if not specified).
     */
    public: inline void setGain1 (float val) {
      int16_t bits = VRTMath::fromFloat16(7,val);
      setI(GAIN,2,bits);
    }

    /** Sets the Stage 2 Gain of the device in dB. This is the back-end gain of the
     *  system. In cases where a separate Stage 1 and Stage 2 gain is not necessary, the Stage 1
     *  gain holds the total gain of the system and this is set to zero. <i>If no stage 1 gain
     *  has been specified, it will be set to 0 following the call to this method.</i>
     *  @param val The gain (null if not specified).
     */
    public: inline void setGain2 (float val) {
      int16_t bits = VRTMath::fromFloat16(7,val);
      setI(GAIN,0,bits);
    }

    /** Sets the Sample Rate in Hz.
     *  @param val The sample rate (null if not specified).
     */
    public: inline void setSampleRate (double val) {
      int64_t bits = (isNull(val))? INT64_NULL : VRTMath::fromDouble64(20,val);
      setX(SAMPLE_RATE,bits);
    }

   /** Sets the Sample Period (inverse of Sample Rate) in sec.
     *  @param val The sample period (null if not specified).
     */
    public: inline void setSamplePeriod (double val) {
      if (isNull(val)) setSampleRate(val);
      else             setSampleRate(1.0 / val);
    }

    /** Sets the Temperature in degrees Celsius. This is used to convey the temperature of
     *  any component that may affect the described signal.
     *  @param val The temperature (null if not specified).
     */
    public: inline void setTemperature (float val) {
      int16_t bits = (isNull(val))? INT16_NULL : VRTMath::fromFloat16(6,val);
      setI(TEMPERATURE,2,bits);
    }

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
      if (isNull(val)) setI(GAIN,0,INT16_NULL); // clears gain field(s)
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

    /* XXX END OF CIF0 FUNCTIONS XXX */

    // TODO - remember to check if CIF is enabled for all other CIFs

    /* XXX START OF CIF1 FUNCTIONS XXX */
    // TODO
    /* XXX START OF CIF1 FUNCTIONS XXX */

    /* XXX START OF CIF2 FUNCTIONS XXX */
    // TODO

    /** Gets Mode ID
     *  Mode ID uses the Generic16 bit Identifier field
     *  See Section 9.8.10.2 for Mode ID
     *  See Section 9.8 for Generic16 bit Identifier field
     *  @return The Mode ID (null if not specified)
     */
    public: inline int16_t getModeID () const {
      int16_t bits = getI(MODE_ID, 2);
      return (isNull(bits))? INT16_NULL : bits;
    }

    /** Sets Mode ID
     *  Mode ID uses the Generic16 bit Identifier field
     *  See Section 9.8.10.2 for Mode ID
     *  See Section 9.8 for Generic16 bit Identifier field
     *  @param val The Mode ID (null if not specified)
     */
    public: inline void setModeID (int16_t val) {
      setI(MODE_ID, 2, val);
    }

    /* XXX START OF CIF2 FUNCTIONS XXX */

    /* XXX START OF CIF3 FUNCTIONS XXX */
    // TODO
    /* XXX START OF CIF3 FUNCTIONS XXX */

    /* XXX START OF CIF7 FUNCTIONS XXX */
    // TODO
    /* XXX START OF CIF7 FUNCTIONS XXX */


  }; /* class IndicatorFieldProvider */

} END_NAMESPACE /* vrt namespace */

#endif /* _IndicatorFields_h */

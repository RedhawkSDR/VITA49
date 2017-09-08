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

#ifndef _StandardDataPacket_h
#define _StandardDataPacket_h
#include "BasicDataPacket.h"

namespace vrt {
  /** Represents a standard data packet. <br>
   *  <br>
   *  <h2>Overview</h2>
   *  <br>
   *  The "standard data packet" defines a specific set of Class Identifiers that can be
   *  used with IF Data and Extension Data packets. Rather than enumerate all possible Class
   *  Identifiers that can be used, an algorithmic approach is taken to assign them (this
   *  algorithm should be apparent from the diagrams shown below). All of the Class Identifiers
   *  will be in the range of <tt>FF-FF-FA:0000.0000</tt> to <tt>FF-FF-FA:00FF.FFFF</tt>.<br>
   *  <br>
   *  Note that although much of the information in the algorithm derives from information
   *  contained in the paired IF Context packet (especially the Data Packet Payload Field), this
   *  is not intended to be a substitute for the proper use of the fields in the paired IF Context
   *  packet.<br>
   *  <br>
   *  The <b>StandardDataPacket</b> shall only be used with data packets that meet all of
   *  the following criteria <i>(see VRT section 7.1.5.18)</i>:
   *  <ul>
   *    <li>Contain "Real" or "Complex, Cartesian" values;</li>
   *    <li>Contain 4-/8-/16-/32-/64-bit Signed Fixed Point,
   *        1-/4-/8-/16-/32-/64-bit Unsigned Fixed Point, 32-bit
   *        IEEE-754 Single-Precision Floating-Point, or 64-bit
   *        IEEE-754 Double-Precision Floating-Point values;</li>
   *    <li>Have an Event-Tag Size of zero;</li>
   *    <li>Have an Channel-Tag Size of zero;</li>
   *    <li>Have an Item Packing Field Size equal to the Data Item Size; and</li>
   *    <li>Have a Repeat Count of 1.</li>
   *  </ul><br>
   *  <br>
   *  <pre>
   *  +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
   *  |31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|15|14|13|12|11|10| 9| 8| 7| 6| 5| 4| 3| 2| 1| 0|
   *  +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
   *  |        Reserved       |                         FF-FF-FA                                      |
   *  +-----------------------+-----+-----+-----------+-----------------------------------------------+
   *  | 0  0  0  0  0  0  0  0| Res | R/C | Data Type |               Vector Size                     |
   *  +-----------------------+-----+-----+-----------+-----------------------------------------------+
   *                   <b>Organization of the StandardDataPacket Class Identifier</b>
   *  </pre><br>
   *  <br>
   *  <pre>
   *              +--------+-------------------------------------------+-----------+
   *              |  Bit   |               Field                       |Field Width|
   *              |Position|            Description                    |  (bits)   |
   *              +--------+-------------------------------------------+-----------+
   *              | 24..31 | Fixed Value (binary 0000 0000)            |     8     |
   *              +--------+-------------------------------------------+-----------+
   *              | 22..23 | Reserved    (binary 00)                   |     2     |
   *              +--------+-------------------------------------------+-----------+
   *              | 20..21 | Real/Complex Indicator:                   |     1     |
   *              |        |     00 = Real                             |           |
   *              |        |     01 = Complex, Cartesian               |           |
   *              |        |     10 = <i>Reserved</i>                         |           |
   *              |        |     11 = <i>Reserved</i>                         |           |
   *              |        |   <i>(This mirrors VRT Rule 7.1.5.18-3.)</i>     |           |
   *              +--------+-------------------------------------------+-----------+
   *              | 16..19 | Data Type:                                |     4     |
   *              |        |   0000 = <i>Reserved</i>                         |           |
   *              |        |   0001 =  4-bit Signed Fixed Point        |           |
   *              |        |   0010 =  8-bit Signed Fixed Point        |           |
   *              |        |   0011 = 16-bit Signed Fixed Point        |           |
   *              |        |   0100 = 32-bit Signed Fixed Point        |           |
   *              |        |   0101 = 64-bit Signed Fixed Point        |           |
   *              |        |   0110 = 32-bit IEEE-754 Single-Precision |           |
   *              |        |   0111 = 64-bit IEEE-754 Double-Precision |           |
   *              |        |   1000 =  1-bit Unsigned Fixed Point      |           |
   *              |        |   1001 =  4-bit Unsigned Fixed Point      |           |
   *              |        |   1010 =  8-bit Unsigned Fixed Point      |           |
   *              |        |   1011 = 16-bit Unsigned Fixed Point      |           |
   *              |        |   1100 = 32-bit Unsigned Fixed Point      |           |
   *              |        |   1101 = 64-bit Unsigned Fixed Point      |           |
   *              |        |   1110 = <i>Reserved</i>                         |           |
   *              |        |   1111 = <i>Reserved</i>                         |           |
   *              +--------+-------------------------------------------+-----------+
   *              |  0..15 | Vector Size:                              |    16     |
   *              |        |    An unsigned number that is one less    |           |
   *              |        |    than the actual Vector size. <i>(This     |           |
   *              |        |    mirrors VRT Rule 7.1.5.18-11.)</i>         |           |
   *              +--------+-------------------------------------------+-----------+
   *              <b>Location of Parameter Fields in the Second Class Identifier Word</b>
   *  </pre>
   *  <br>
   *  <h2>Permission</h2>
   *  <br>
   *  The authors of this library give permission for the above text and diagrams to be incorporated
   *  into other standards and documentation, provided that no substantive changes are made thereto
   *  and the authors are clearly identified. <br>
   *  <br>
   *  <h2>History</h2>
   *  <br>
   *  The idea of the "standard data packet" originated with this library in early 2011. At the time
   *  the OUI of "FF-FF-00" the was used (this was arbitrarily chosen based on suggestions from the
   *  VITA-49.0b suggestions) and the exact layout of the bits differed slightly from what is used
   *  today. There was also a notion for using the user-defined bits in the trailer to indicate the
   *  number of pad bits (this concept later worked its way into the VITA-49.0b suggestions). <br>
   *  <br>
   *  After some "beta testing" and discussions with a number of users it was decided to make some
   *  changes, with the agreement that other users would then adopt the "StandardDataPacket" as a
   *  (pseudo-)standard. The change from "FF-FF-00" to "FF-FF-FA" was done to avoid conflict with
   *  other users. Some users have given special meaning to the "A" in at the end of the "FF-FF-FA"
   *  OUI; the authors of this library is aware of this and views it as "convenient" but neither
   *  endorses nor denounces this use. <br>
   *  <br>
   *  <h2>Future</h2>
   *  <br>
   *  Although there are no plans to make changes at this time, there are two bits ("Res" in the
   *  top diagram) that could be used and a number of additional data types and real/complex
   *  indicators available. However, the "Fixed Value" bits are permanently fixed at zero as
   *  some users have already been using other parts of the "FF-FF-FA" for other uses.
   */
  class StandardDataPacket : public BasicDataPacket {
    /** Mask and check value used for determining if a given class ID matches one used for a
     *  {@link StandardDataPacket}. This is intended to be used as follows:
     *  <pre>
     *    if ((classID & CLASS_ID_MASK) == CLASS_ID_CHECK) {
     *      // Found a StandardDataPacket
     *      ...
     *    }
     *  </pre>
     *  Note that this is not a guarantee that the class ID is valid, but it says that the class
     *  ID matches that expected for a {@link StandardDataPacket}.
     */
    public: static const int64_t CLASS_ID_MASK  = __INT64_C(0xFFFFFFFF000000);
    /** <i>See {@link #CLASS_ID_MASK}</i> */
    public: static const int64_t CLASS_ID_CHECK = __INT64_C(0xFFFFFA00000000);

    /** Basic destructor for the class. */
    public: ~StandardDataPacket () { }

    // TODO FIXME - add back constructor from old VRT to work with SinkVITA49
    // TODO FIXME - can SinkVITA49 be updated to use a different constructor?
    public: StandardDataPacket( const int32_t pktsize) : BasicDataPacket(pktsize) {}
    // End TODO FIXME

    /** Basic copy constructor for the class. */
    public: StandardDataPacket (const BasicDataPacket &p) : BasicDataPacket(p) { }

    /** Creates a new instance accessing the given data buffer. Note that when the buffer lengths
     *  are given, only the most minimal of error checking is done. Users should call
     *  <tt>isPacketValid()</tt> to verify that the packet is valid. Invalid packets can result
     *  unpredictable behavior, but this is explicitly allowed (to the extent possible) so that
     *  applications creating packets can use this class even if the packet isn't yet "valid".
     *  @param buf      The data buffer to use.
     *  @param readonly Should users of this instance be able to modify the underlying data buffer?
     */
    public: StandardDataPacket (const vector<char> &buf, bool readonly) : BasicDataPacket(buf,readonly) { }

    /** Creates a new instance that can be written to.
        @param type        The numeric type (one of: Int4, Int8, Int16, Int32, Int64, Float, Double, etc.).
        @param realComplex Real/Complex mode (one of: Real, ComplexCartesian).
        @param frameSize   Frame size, must be a multiple of 256 or 0/1 to indicate that the data is
                           not framed.
        @throws VRTException if an invalid value is passed in.
     */
    public: StandardDataPacket (DataType type=DataType_Int8, RealComplexType realComplex=RealComplexType_Real, int32_t frameSize=0) {
      setPayloadFormat(type, realComplex, frameSize);
    }

    /** Creates a new instance that can be written to.
        @param format The payload format to use.
        @throws VRTException if the payload format is unsupported.
     */
    public: StandardDataPacket (const PayloadFormat &format) {
      setPayloadFormat(format);
    }

    /** Does the ID match that of a StandardDataPacket? */
    public: static bool isStandardDataPacket (int64_t id) {
      if (isNull(id)) return false;
      return ((id & CLASS_ID_MASK) == CLASS_ID_CHECK);
    }

    public: virtual string getPacketValid (bool strict, int32_t length) const;

    protected: virtual void toStringStream (ostringstream &str) const;

    public: virtual inline PayloadFormat getPayloadFormat () const {
      return toPayloadFormat(getClassIdentifier());
    }

    /** Gets the data type used.
        @return The data type.
        @see #setPayloadFormat
     */
    public: inline DataType getDataType () const {
      PayloadFormat pf = getPayloadFormat();
      return (isNull(pf))? (DataType)INT32_NULL : pf.getDataType();
    }

    /** Gets the real/complex type used.
        @return The data type.
        @see #setPayloadFormat
     */
    public: inline RealComplexType getRealComplexType () const {
      PayloadFormat pf = getPayloadFormat();
      return (isNull(pf))? (RealComplexType)INT32_NULL : pf.getRealComplexType();
    }

    /** Gets the frame size used.
        @return The frame size.
        @see #setPayloadFormat
     */
    public: inline int32_t getFrameSize () const {
      PayloadFormat pf = getPayloadFormat();
      return (isNull(pf))? -1 : pf.getVectorSize();
    }

    public: virtual void setPayloadFormat (const PayloadFormat &pf);

    /** Sets the payload format and class ID.
        @param type        The numeric type (one of: Int4, Int8, Int16, Int32, Int64, Float, Double, etc.).
        @param realComplex Real/Complex mode (one of: Real, ComplexCartesian).
        @param frameSize   Frame size, must be a multiple of 256 or 0/1 to indicate that the data is
                           not framed.
        @throws VRTException if an invalid value is passed in.
     */
    public: virtual void setPayloadFormat (DataType type, RealComplexType realComplex, int32_t frameSize);

    /** Convert class ID to payload format, return null on error. */
    private: static PayloadFormat toPayloadFormat (int64_t classID);

    /** Convert payload format to class ID, return -1 on error. */
    private: static int64_t toClassID (DataType type, RealComplexType realComplex, int32_t frameSize) {
      return toClassID(_toPayloadFormat(type, realComplex, frameSize));
    }

    /** Convert payload format to class ID, return -1 on error. */
    private: static int64_t toClassID (const PayloadFormat &pf);
    
    /** Converts to PayloadFormat. */
    private: static inline PayloadFormat _toPayloadFormat (DataType type, RealComplexType realComplex, int32_t frameSize) {
      PayloadFormat pf(type, realComplex);
      if (frameSize > 1) pf.setVectorSize(frameSize);
      return pf;
    }
  };
} END_NAMESPACE
#endif /* _StandardDataPacket_h */

/**
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

package nxm.vrt.lib;

import nxm.vrt.lib.VRTPacket.PayloadFormat;


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
 *
 *  @author         
 */
public class StandardDataPacket extends BasicDataPacket {
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
  public static final long CLASS_ID_MASK  = 0xFFFFFFFF000000L,
                           CLASS_ID_CHECK = 0xFFFFFA00000000L;

  private static final long ERROR = -1;

  private static final long[] ID_TO_FORMAT = {
    ERROR,                  // reserved
    0x000000C300000000L,    //  4-bit Signed Fixed Point
    0x000001C700000000L,    //  8-bit Signed Fixed Point
    0x000003CF00000000L,    // 16-bit Signed Fixed Point
    0x000007DF00000000L,    // 32-bit Signed Fixed Point
    0x00000FFF00000000L,    // 64-bit Signed Fixed Point
    0x0E0007DF00000000L,    // 32-bit IEEE-754 Single-Precision
    0x0F000FFF00000000L,    // 64-bit IEEE-754 Double-Precision
    0x1000000000000000L,    //  1-bit Unsigned Fixed Point
    0x100000C300000000L,    //  4-bit Unsigned Fixed Point
    0x100001C700000000L,    //  8-bit Unsigned Fixed Point
    0x100003CF00000000L,    // 16-bit Unsigned Fixed Point
    0x100007DF00000000L,    // 32-bit Unsigned Fixed Point
    0x10000FFF00000000L,    // 64-bit Unsigned Fixed Point
    ERROR,                  // reserved
    ERROR,                  // reserved
  };

  private static final long[] SIZE_TO_TYPE = {
    0x00FFFFFA00000000L,                 ERROR,                ERROR,  0x00FFFFFA00010000L,  //  1-bit to  4-bit
                  ERROR,                 ERROR,                ERROR,  0x00FFFFFA00020000L,  //  5-bit to  8-bit
                  ERROR,                 ERROR,                ERROR,                ERROR,  //  9-bit to 12-bit
                  ERROR,                 ERROR,                ERROR,  0x00FFFFFA00030000L,  // 13-bit to 16-bit
                  ERROR,                 ERROR,                ERROR,                ERROR,  // 17-bit to 20-bit
                  ERROR,                 ERROR,                ERROR,                ERROR,  // 21-bit to 24-bit
                  ERROR,                 ERROR,                ERROR,                ERROR,  // 25-bit to 28-bit
                  ERROR,                 ERROR,                ERROR,  0x00FFFFFA00040000L,  // 29-bit to 32-bit
                  ERROR,                 ERROR,                ERROR,                ERROR,  // 33-bit to 36-bit
                  ERROR,                 ERROR,                ERROR,                ERROR,  // 37-bit to 40-bit
                  ERROR,                 ERROR,                ERROR,                ERROR,  // 41-bit to 44-bit
                  ERROR,                 ERROR,                ERROR,                ERROR,  // 45-bit to 48-bit
                  ERROR,                 ERROR,                ERROR,                ERROR,  // 49-bit to 52-bit
                  ERROR,                 ERROR,                ERROR,                ERROR,  // 53-bit to 56-bit
                  ERROR,                 ERROR,                ERROR,                ERROR,  // 57-bit to 60-bit
                  ERROR,                 ERROR,                ERROR,  0x00FFFFFA00050000L,  // 61-bit to 64-bit
  };

  /** Internal constructor, see docs below. */
  public StandardDataPacket (VRTPacket p, boolean copy) {
    super(p, copy);
  }
  
  /** Creates a new instance that can be written to. This is identical to:
   *  <tt>StandardDataPacket(DataType.Int8,RealComplexType.Real,0)</tt>
   */
  public StandardDataPacket () {
    this(DataType.Int8,RealComplexType.Real,0);
  }

  /** Creates a new instance that can be written to. This is identical to:
   *  <tt>StandardDataPacket(type,RealComplexType.Real,0)</tt>
   *  @param type        The numeric type (one of: Int4, Int8, Int16, Int32, Int64, Float, Double, PackedBits).
   *  @throws IllegalArgumentException if an invalid value is passed in.
   */
  public StandardDataPacket (DataType type) {
    this(type,RealComplexType.Real,0);
  }

  /** Creates a new instance that can be written to. This is identical to:
   *  <tt>StandardDataPacket(type,realComplex,0)</tt>
   *  @param type        The numeric type (one of: Int4, Int8, Int16, Int32, Int64, Float, Double, PackedBits).
   *  @param realComplex Real/Complex mode (one of: Real, ComplexCartesian).
   *  @throws IllegalArgumentException if an invalid value is passed in.
   */
  public StandardDataPacket (DataType type, RealComplexType realComplex) {
    this(type,realComplex,0);
  }

  /** Creates a new instance that can be written to.
   *  @param type        The numeric type (one of: Int4, Int8, Int16, Int32, Int64, Float, Double, PackedBits).
   *  @param realComplex Real/Complex mode (one of: Real, ComplexCartesian).
   *  @param frameSize   Frame size, must be a multiple of 256 or 0/1 to indicate that the data is
   *                     not framed.
   *  @throws IllegalArgumentException if an invalid value is passed in.
   */
  @SuppressWarnings("OverridableMethodCallInConstructor")
  public StandardDataPacket (DataType type, RealComplexType realComplex, int frameSize) {
    super();
    setPayloadFormat(type, realComplex, frameSize);
  }
  
  /** Creates a new instance that can be written to.
   *  @param format The payload format to use.
   *  @throws IllegalArgumentException if the payload format is unsupported.
   */
  @SuppressWarnings("OverridableMethodCallInConstructor")
  public StandardDataPacket (PayloadFormat format) {
    super();
    setPayloadFormat(format);
  }
  
  /** Creates a new instance accessing the given data buffer.
   *  @param bbuf     The data buffer to use.
   *  @param boff     The byte offset into the bbuf to start reading/writing at.
   *  @param blen     The length of bbuf in bytes (-1=use size in header of packet).
   *  @param readOnly Should users of this instance be able to modify the underlying data buffer?
   *  @param direct   Should the methods in this class directly interact with the buffer?
   *  @throws NullPointerException If the buffer is null.
   *  @throws IllegalArgumentException If the offset is negative or greater than the
   *          length of the buffer given. Also thrown if the buffer length is -1 and a copy
   *          can not be made because the sizes in the header of the packet are invalid.
   */
  public StandardDataPacket (byte[] bbuf, int boff, int blen, boolean readOnly, boolean direct) {
    super(bbuf, boff, blen, readOnly, direct);
  }
  
  /** Creates a new instance accessing the given data buffers.
   *  @param hbuf     The data buffer to use for the header.
   *  @param hoff     The byte offset into hbuf to start reading/writing at.
   *  @param hlen     The length of hbuf in bytes (-1=use size in header of packet).
   *  @param pbuf     The data buffer to use for the payload.
   *  @param poff     The byte offset into pbuf to start reading/writing at.
   *  @param plen     The length of pbuf in bytes (-1=use size in header of packet).
   *  @param tbuf     The data buffer to use for the trailer.
   *  @param toff     The byte offset into tbuf to start reading/writing at.
   *  @param tlen     The length of tbuf in bytes (-1=use size in header of packet).
   *  @param readOnly Should users of this instance be able to modify the underlying data buffer?
   *  @param direct   Should the methods in this class directly interact with the buffer?
   *  @throws NullPointerException If a buffers is null.
   *  @throws IllegalArgumentException If an offset is negative or greater than the
   *          length of the buffer given. Also thrown if the a buffer length is -1 and a copy
   *          can not be made because the sizes in the header of the packet are invalid.
   */
  public StandardDataPacket (byte[] hbuf, int hoff, int hlen,
                             byte[] pbuf, int poff, int plen,
                             byte[] tbuf, int toff, int tlen, boolean readOnly, boolean direct) {
    super(hbuf, hoff, hlen, pbuf, poff, plen, tbuf, toff, tlen, readOnly, direct);
  }

  /** Does the ID match that of a StandardDataPacket? */
  public static boolean isStandardDataPacket (Long id) {
    if (id == null) return false;
    return ((id & CLASS_ID_MASK) == CLASS_ID_CHECK);
  }

  @Override
  public String getPacketValid (boolean strict, int length) {
    String        err = super.getPacketValid(strict, length);
    PayloadFormat fmt = toPayloadFormat(getClassIdentifier());

    if ((err == null) && (fmt == null)) {
      return "Invalid use of "+getClass().getName()+" with ClassID="+getClassID()+".";
    }
    return err;
  }
  
  @Override
  public StandardDataPacket copy () {
    return new StandardDataPacket(this, true);
  }

  @Override
  protected StringBuilder toStringBuilder () {
    StringBuilder str = super.toStringBuilder();
    str.append(" DataType="       ).append(getDataType());
    str.append(" RealComplexType=").append(getRealComplexType());
    str.append(" FrameSize="      ).append(getFrameSize());
    str.append(" DataLength="     ).append(getDataLength()).append(" samples");
    return str;
  }

  @Override
  public void setClassIdentifier (Long classID) {
    PayloadFormat pf = toPayloadFormat(classID);
    if (pf == null) {
      throw new IllegalArgumentException("Invalid class identifier ("+classID+") for use with StandardDataPacket");
    }
    super.setClassIdentifier(classID);
    super.setPayloadFormat(pf);
  }

  @Override
  public PayloadFormat getPayloadFormat () {
    return toPayloadFormat(getClassIdentifier());
  }

  /** Gets the data type used.
   *  @return The data type.
   *  @see #setPayloadFormat
   */
  public final DataType getDataType () {
    PayloadFormat pf = getPayloadFormat();
    return (pf == null)? null : pf.getDataType();
  }

  /** Gets the real/complex type used.
   *  @return The data type.
   *  @see #setPayloadFormat
   */
  public final RealComplexType getRealComplexType () {
    PayloadFormat pf = getPayloadFormat();
    return (pf == null)? null : pf.getRealComplexType();
  }

  /** Gets the frame size used.
   *  @return The frame size.
   *  @see #setPayloadFormat
   */
  public final int getFrameSize () {
    PayloadFormat pf = getPayloadFormat();
    return (pf == null)? -1 : pf.getVectorSize();
  }

  @Override
  public void setPayloadFormat (PayloadFormat pf) {
    long classID = toClassID(pf);
    if (classID == ERROR) {
      throw new IllegalArgumentException("Invalid payload format ("+pf+") for use with StandardDataPacket");
    }
    super.setClassIdentifier(classID);
    super.setPayloadFormat(pf);
  }

  /** Sets the payload format and class ID.
   *  @param type        The numeric type (one of: Int4, Int8, Int16, Int32, Int64, Float, Double, PackedBits).
   *  @param realComplex Real/Complex mode (one of: Real, ComplexCartesian).
   *  @param frameSize   Frame size, must be a multiple of 256 or 0/1 to indicate that the data is
   *                     not framed.
   *  @throws IllegalArgumentException if an invalid value is passed in.
   */
  public void setPayloadFormat (DataType type, RealComplexType realComplex, int frameSize) {
    setPayloadFormat(_toPayloadFormat(type, realComplex, frameSize));
  }

  /** Convert class ID to payload format, return null on error. */
  private static PayloadFormat toPayloadFormat (Long classID) {
    if (!isStandardDataPacket(classID)) return null; // ERROR

    long id   = classID.longValue();
    long bits = ID_TO_FORMAT[(int)((id & 0x000F0000) >> 16)]  // Item Format + Packing Size + Item Size
              | ((id & 0x00300000L) << 41)                    // Real/Complex
              | ((id & 0x0000FFFFL)      );                   // Vector Size

    return (bits == ERROR)? null : new PayloadFormat(bits);
  }

  /** Convert payload format to class ID, return ERROR on error. */
  private static long toClassID (PayloadFormat pf) {
    if (pf == null) return ERROR;
    
    long bits  = pf.getBits();
    long check = bits & 0x0FFFF000FFFF0000L;
    long itmSz = bits & 0x0000001F00000000L;
    long fmtSz = bits & 0x00000FE000000000L;

    if ((itmSz << 6) != fmtSz) {
      return ERROR;
    }
    else if (check == 0x0000000000000000L) {
      return SIZE_TO_TYPE[(int)(itmSz>>32)]        // Type size
           | ((bits >> 41) & 0x00380000L)          // Real/Complex + Signed/Unsigned
           | ((bits      ) & 0x0000FFFFL);         // Vector Size
    }
    else if (check == 0x0E00000000000000L) {
      return 0x00FFFFFA00060000L                   // 32-bit IEEE-754 Double-Precision
           | ((bits >> 41) & 0x00300000L)          // Real/Complex
           | ((bits      ) & 0x0000FFFFL);         // Vector Size
    }
    else if (check == 0x0F00000000000000L) {
      return 0x00FFFFFA00070000L                   // 64-bit IEEE-754 Double-Precision
           | ((bits >> 41) & 0x00300000L)          // Real/Complex
           | ((bits      ) & 0x0000FFFFL);         // Vector Size
    }
    else {
      return ERROR;
    }
  }

  /** Convert payload format to class ID, return 0 on error. */
  private static long toClassID (DataType type, RealComplexType realComplex, int frameSize) {
    return toClassID(_toPayloadFormat(type, realComplex, frameSize));
  }
  
  /** Converts to PayloadFormat. */
  private static PayloadFormat _toPayloadFormat (DataType type, RealComplexType realComplex, int frameSize) {
    PayloadFormat pf = new PayloadFormat(type, realComplex);
    if (frameSize > 1) pf.setVectorSize(frameSize);
    return pf;
  }
}

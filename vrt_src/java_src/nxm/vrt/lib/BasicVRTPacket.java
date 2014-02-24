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

import java.io.EOFException;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetAddress;
import java.util.UUID;
import nxm.vrt.lib.VRTConfig.VITAVersion;
import nxm.vrt.lib.VRTPacket.PacketType;
import nxm.vrt.lib.TimeStamp.FractionalMode;
import nxm.vrt.lib.TimeStamp.IntegerMode;

import static java.lang.Boolean.TRUE;
import static java.lang.Math.ceil;
import static java.lang.Math.min;
import static nxm.vrt.lib.Utilities.append;
import static nxm.vrt.lib.VRTMath.packShort;
import static nxm.vrt.lib.VRTMath.packInt;
import static nxm.vrt.lib.VRTMath.packLong;
import static nxm.vrt.lib.VRTMath.unpackShort;
import static nxm.vrt.lib.VRTMath.unpackInt;
import static nxm.vrt.lib.VRTMath.unpackLong;

import static nxm.vrt.lib.VRTConfig.VRT_VERSION;

/** A basic but full-featured implementation of {@link VRTPacket}. <br>
 *  <br>
 *  The following implementation is used for the {@link HasFields} methods:
 *  <pre>
 *     ID | Name      | Type
 *    ----+-----------+-----------
 *     0  | StreamID  | String
 *     1  | ClassID   | String
 *     2  | TimeStamp | TimeStamp
 *    ----+-----------+-----------
 *  </pre>
 *
 *  @author 
 */
public class BasicVRTPacket implements VRTPacket, HasFields {
  private static final byte[] NULL_TRAILER = new byte[MAX_TRAILER_LENGTH]; // only used by trailerEquals(..)
  
  // Flags for getOffset(..). These values are in the form:
  //   AABBaabb         AA - Bit mask to apply to bbuf[0] indicating flag is present
  //                    BB - Bit mask to apply to bbuf[1] indicating flag is present
  //                    aa - Bit mask to apply to bbuf[0] highlighting all preceding
  //                    bb - Bit mask to apply to bbuf[1] highlighting all preceding
  // In doing this, we use AA=0x20 to indicate the presence of the payload (and always
  // apply it to our read of bbuf[0] since the payload is mandatory). 0x20 was chosen
  // since that bit is always 0 in the header.
  private static final int STREAM_ID = 0x50000000; // getOffset(..) flag for STREAM_ID
  private static final int CLASS_ID  = 0x08005000; // getOffset(..) flag for field: CLASS_ID
  private static final int TSI       = 0x00C05800; // getOffset(..) flag for field: TSI
  private static final int TSF       = 0x003058C0; // getOffset(..) flag for field: TSF
  private static final int PAYLOAD   = 0x020058F0; // getOffset(..) flag for field: PAYLOAD
  
  /** An empty byte buffer of length 0. */
  public static final byte[] EMPTY = new byte[0];
  
  static final byte CLASS_ID_BIT = 0x8;  // Class ID present bit in buf[boff+0]
  static final byte TRAILER_BIT  = 0x4;  // Trailer present bit in buf[boff+0]
  static final byte TSM_BIT      = 0x1;  // TimeStamp Mode bit in buf[boff+0]
  
  final byte[]  bbuf;      // Packet  buffer (always includes the header)
        byte[]  pbuf;      // Payload buffer (null if not separate)
  final byte[]  tbuf;      // Trailer buffer (null if not separate)
  final int     boff;      // Offset into bbuf
  final int     poff;      // Offset into pbuf
  final int     toff;      // Offset into tbuf
  final boolean readOnly;  // Is this instance read-only?
  final boolean direct;    // Is this instance directly writing to external buffers?

  /** Creates a new instance from an existing VRTPacket.
   *  @param p    The existing packet.
   *  @param copy Must the data be copied? If true, the resulting packet must copy the data from
   *              <tt>p</tt> in such a way that no changes to the returned packet will alter
   *              <tt>p</tt>. If false, a "fast" initialization is used, meaning that the resulting
   *              packet may or may not be a copy of the given data, the implementation will use the
   *              fastest creation method available.
   *  @throws NullPointerException If <tt>p</tt> is null.
   */
  public BasicVRTPacket (VRTPacket p, boolean copy) {
    if (!(p instanceof BasicVRTPacket)) {
      int blen = p.getHeaderLength();
      int plen = p.getPayloadLength();
      int tlen = p.getTrailerLength();

      this.bbuf     = new byte[MAX_HEADER_LENGTH];
      this.pbuf     = p.getPacket(); // includes header and trailer... we will just ignore them
      this.tbuf     = new byte[MAX_TRAILER_LENGTH];
      this.boff     = 0;
      this.poff     = blen;          // skip the header we are ignoring
      this.toff     = 0;
      this.readOnly = false;
      this.direct   = false;

      System.arraycopy(pbuf,     0,     bbuf, 0, blen); // copy header to bbuf
      System.arraycopy(pbuf, blen+plen, tbuf, 0, tlen); // copy trailer to tbuf
    }
    else if (copy) {
      int blen = p.getHeaderLength();
      int plen = p.getPayloadLength();
      int tlen = p.getTrailerLength();

      BasicVRTPacket bp = (BasicVRTPacket)p;
      this.bbuf     = new byte[MAX_HEADER_LENGTH];
      this.pbuf     = new byte[plen];
      this.tbuf     = new byte[MAX_TRAILER_LENGTH];
      this.boff     = 0;
      this.poff     = 0;
      this.toff     = 0;
      this.readOnly = false;
      this.direct   = false;

      System.arraycopy(bp.bbuf,      bp.boff,      bbuf, boff, blen);
      System.arraycopy(bp.getPBuf(), bp.getPOff(), pbuf, poff, plen);
      System.arraycopy(bp.getTBuf(), bp.getTOff(), tbuf, toff, tlen);
    }
    else {
      BasicVRTPacket bp = (BasicVRTPacket)p;
      this.bbuf     = bp.bbuf;
      this.pbuf     = bp.pbuf;
      this.tbuf     = bp.tbuf;
      this.boff     = bp.boff;
      this.poff     = bp.poff;
      this.toff     = bp.toff;
      this.readOnly = bp.readOnly;
      this.direct   = bp.direct;
    }
  }
  
  /** Internal constructor, for use by subclasses.
      @param p                The packet (already initialized).
      @param type             The expected packet type.
      @param classID          The expected classID.
      @param minPayloadLength The minimum payload length.
      @param maxPayloadLength The maximum payload length (&lt;0 if not pre-defined).
      @throws IllegalArgumentException If packet type or classID of the packet are invalid, or if
                                       the payload length is invalid.
   */
  protected BasicVRTPacket (VRTPacket p, boolean copy, PacketType type, long classID,
                            int minPayloadLength, int maxPayloadLength) {
    this(p, copy);
    if (getPacketType() != type) {
      throw new IllegalArgumentException("Can not initialize "+getClass()+" when given packet type of "+getPacketType());
    }
    if (getClassIdentifier() != classID) {
      throw new IllegalArgumentException("Can not initialize "+getClass()+" when given packet class ID of "+getClassID());
    }
    
    int len = getPayloadLength();
    if (len < minPayloadLength) {
      throw new IllegalArgumentException("Can not initialize "+getClass()+", payload length is invalid.");
    }
    if ((maxPayloadLength >= 0) && (len > maxPayloadLength)) {
      throw new IllegalArgumentException("Can not initialize "+getClass()+", payload length is invalid.");
    }
  }
  
  /** Internal constructor, used by the public constructors below. */
  BasicVRTPacket (byte[] bbuf, int boff, int blen,
                  byte[] pbuf, int poff, int plen,
                  byte[] tbuf, int toff, int tlen,
                  boolean readOnly, boolean direct, boolean separate) {
    if (direct) {
      if (separate) {
        if (bbuf == null) throw new NullPointerException("Given null buffer.");
        if (pbuf == null) throw new NullPointerException("Given null buffer.");
        if (tbuf == null) throw new NullPointerException("Given null buffer.");
      }
      else {
        if (bbuf == null) throw new NullPointerException("Given null buffer.");
      }
      this.bbuf     = bbuf;
      this.pbuf     = pbuf;
      this.tbuf     = tbuf;
      this.boff     = boff;
      this.poff     = poff;
      this.toff     = toff;
      this.readOnly = readOnly;
      this.direct   = true;
    }
    else if (!separate && readOnly && (blen >= 0)) {
      // Use "fast" copy of single buffer.
      this.bbuf     = new byte[blen];
      this.pbuf     = null;
      this.tbuf     = null;
      this.boff     = 0;
      this.poff     = 0;
      this.toff     = 0;
      this.readOnly = readOnly;
      this.direct   = false;

      System.arraycopy(bbuf, boff, this.bbuf, 0, blen);
    }
    else if (separate && (blen >= 0) && (plen >= 0) && (tlen >= 0)) {
      // Use "fast" copy three separate buffers.
      this.bbuf     = new byte[MAX_HEADER_LENGTH];
      this.pbuf     = new byte[plen];
      this.tbuf     = new byte[MAX_TRAILER_LENGTH];
      this.boff     = 0;
      this.poff     = 0;
      this.toff     = 0;
      this.readOnly = readOnly;
      this.direct   = false;

      System.arraycopy(bbuf, boff, this.bbuf, 0, blen);
      System.arraycopy(pbuf, poff, this.pbuf, 0, plen);
      System.arraycopy(tbuf, toff, this.tbuf, 0, tlen);
    }
    else {
      // We need to inspect the packet in order to work with it... ugh!
      BasicVRTPacket p = new BasicVRTPacket(bbuf, boff, -1, pbuf, poff, -1, tbuf, toff, -1, false, true, separate);
      
      this.bbuf     = new byte[MAX_HEADER_LENGTH];
      this.pbuf     = new byte[p.getPayloadLength()];
      this.tbuf     = new byte[MAX_TRAILER_LENGTH];
      this.boff     = 0;
      this.poff     = 0;
      this.toff     = 0;
      this.readOnly = readOnly;
      this.direct   = false;

      p.readPacket(this.bbuf, 0, 0,                     p.getHeaderLength());
      p.readPacket(this.pbuf, 0, p.getHeaderLength(),   p.getPayloadLength());
      p.readPacket(this.tbuf, 0, p.getPacketLength()-4, p.getTrailerLength());
    }
    
    // Sanity checks -- if these fail, there is an error in the constructor's code
    if ((pbuf == null) != (tbuf == null)) {
      throw new AssertionError("Can not have pbuf="+pbuf+" and tbuf="+tbuf+".");
    }
  }
  
  /** Creates a new instance that can be written to. Initially this will just be a simple data
   *  packet with no fields set (other than the required packet length of 4).
   */
  @SuppressWarnings("OverridableMethodCallInConstructor")
  public BasicVRTPacket () {
    this.bbuf     = createDefaultHeader();
    this.pbuf     = createDefaultPayload();
    this.tbuf     = createDefaultTrailer();
    this.boff     = 0;
    this.poff     = 0;
    this.toff     = 0;
    this.readOnly = false;
    this.direct   = false;
  }

  /** Creates a new instance that can be written to. This packet will be in "standard form" with
   *  the type, stream ID and classID given. The time will be set to 00-00-1970T00:00:00.000 UTC.
   */
  @SuppressWarnings("OverridableMethodCallInConstructor")
  public BasicVRTPacket (PacketType type, int streamID, long classID) {
    this();
    setPacketType(type);
    setStreamIdentifier(streamID);
    setClassIdentifier(classID);
  }
  
  /** Creates a new instance that can be written to. This packet will be in "standard form" with
   *  the type, stream ID and classID given. The time will be set to 00-00-1970T00:00:00.000 UTC.
   */
  @SuppressWarnings("OverridableMethodCallInConstructor")
  public BasicVRTPacket (PacketType type, int streamID, int classOUI, short classICC, short classPCC) {
    this();
    setPacketType(type);
    setStreamIdentifier(streamID);
    setClassIdentifier(classOUI, classICC, classPCC);
  }
  
  /** Creates a new instance that can be written to. This packet will be in "standard form" with
   *  the type, stream ID and classID given. The time will be set to 00-00-1970T00:00:00.000 UTC.
   */
  @SuppressWarnings("OverridableMethodCallInConstructor")
  public BasicVRTPacket (PacketType type, int streamID, int classOUI, short classICC, short classPCC,
                         byte[] pbuf, int poff) {
    this.bbuf     = createDefaultHeader();
    this.pbuf     = pbuf;
    this.tbuf     = createDefaultTrailer();
    this.boff     = 0;
    this.poff     = poff;
    this.toff     = 0;
    this.readOnly = false;
    this.direct   = true;
    setPacketType(type);
    setStreamIdentifier(streamID);
    setClassIdentifier(classOUI, classICC, classPCC);
  }
  
  /** Creates a new instance accessing the given data buffer. Note that when the buffer lengths
   *  are given, only the most minimal of error checking is done. Users should call
   *  {@link #isPacketValid()} to verify that the packet is valid. Invalid packets can result
   *  unpredictable behavior, but this is explicitly allowed (to the extent possible) so that
   *  applications creating packets can use this class even if the packet isn't yet "valid".<br>
   *  <br>
   *  <b>Warning: If <tt>direct=true</tt> and <tt>readOnly=false</tt> the instance will have
   *  direct access to the underlying buffers. The lengths specified here are considered to be
   *  the *initial* length, not the max length. As updates are made to the packet, the space used
   *  is permitted to grow up to the maximum lengths for a VRT packet (see {@link #MAX_HEADER_LENGTH},
   *  {@link #MAX_PACKET_LENGTH}, {@link #MAX_TRAILER_LENGTH}). This may result in data corruption
   *  and/or a {@link ArrayIndexOutOfBoundsException} if such space is not available.</b>
   *  @param bbuf     The data buffer to use.
   *  @param boff     The byte offset into the bbuf to start reading/writing at.
   *  @param blen     The length of bbuf in bytes (-1=use size in header of packet).
   *  @param readOnly Should users of this instance be able to modify the underlying data buffer?
   *  @param direct   Should the methods in this class directly interact with the buffer or should
   *                  a copy of the data be made so that changes to the buffer don't affect this
   *                  instance?
   *  @throws NullPointerException If the buffer is null.
   *  @throws IllegalArgumentException If the offset is negative or greater than the
   *          length of the buffer given. Also thrown if the buffer length is -1 and a copy
   *          can not be made because the sizes in the header of the packet are invalid.
   */
  public BasicVRTPacket (byte[] bbuf, int boff, int blen, boolean readOnly, boolean direct) {
    this(bbuf, boff, blen, null, -1, -1, null, -1, -1, readOnly, direct, false);
  }
  
  /** Creates a new instance accessing the given data buffers. Note that when the buffer lengths
   *  are given, only the most minimal of error checking is done. Users should call
   *  {@link #isPacketValid()} to verify that the packet is valid. Invalid packets can result
   *  unpredictable behavior, but this is explicitly allowed (to the extent possible) so that
   *  applications creating packets can use this class even if the packet isn't yet "valid".<br>
   *  <br>
   *  Note that none of the buffers can be null; however, it is legal to have <tt>pbuf</tt>
   *  and/or <tt>tbuf</tt> be of length 0 in the case where there is no payload (rare) and/or
   *  no trailer.<br>
   *  <br>
   *  <b>Warning: If <tt>direct=true</tt> and <tt>readOnly=false</tt> the instance will have
   *  direct access to the underlying buffers. The lengths specified here are considered to be
   *  the *initial* length, not the max length. As updates are made to the packet, the space used
   *  is permitted to grow up to the maximum lengths for a VRT packet (see {@link #MAX_HEADER_LENGTH},
   *  {@link #MAX_PACKET_LENGTH}, {@link #MAX_TRAILER_LENGTH}). This may result in data corruption
   *  and/or a {@link ArrayIndexOutOfBoundsException} if such space is not available.</b>
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
   *  @param direct   Should the methods in this class directly interact with the buffer or should
   *                  a copy of the data be made so that changes to the buffer don't affect this
   *                  instance?
   *  @throws NullPointerException If a buffers is null.
   *  @throws IllegalArgumentException If an offset is negative or greater than the
   *          length of the buffer given. Also thrown if the a buffer length is -1 and a copy
   *          can not be made because the sizes in the header of the packet are invalid.
   */
  public BasicVRTPacket (byte[] hbuf, int hoff, int hlen,
                         byte[] pbuf, int poff, int plen,
                         byte[] tbuf, int toff, int tlen, boolean readOnly, boolean direct) {
    this(hbuf, hoff, hlen, pbuf, poff, plen, tbuf, toff, tlen, readOnly, direct, true);
  }

  @Override
  public long getStreamCode () {
    int type = bbuf[boff] & 0xF0;

    if ((type == 0x00) || (type == 0x20)) {
      return ((long)type) << 56; // no stream identifier
    }
    else {
      return unpackLong(bbuf, boff) & 0xF0000000FFFFFFFFL;
    }
  }


  @Override
  public VRTPacket copy () {
    return VRTConfig.getPacket(this, true);
  }

  /** Converts a {@link VRTPacket} of unknown type to a {@link BasicVRTPacket}. If the input is
   *  already a {@link BasicVRTPacket}, it is simply returned as-is, otherwise a copy of the packet
   *  is made that is an instance of {@link BasicVRTPacket}.
   *  @param p The input packet.
   *  @return A VRT packet version of <tt>p</tt>.
   */
  public static BasicVRTPacket toBasicVRTPacket (VRTPacket p) {
    if (p instanceof BasicVRTPacket) return (BasicVRTPacket)p;
    byte[] pkt = p.getPacket();
    return new BasicVRTPacket(pkt, 0, pkt.length, false, false);
  }
  
  /** A combination of {@link #createDefaultHeader()}, {@link #createDefaultPayload()} and
   *  {@link #createDefaultTrailer()}. <i>This method is rarely used within the Java version, but
   *  is the primary method used within C++.</i>
   *  @return The initial value for a default packet.
   */
  protected final byte[] createDefaultPacket () {
    byte[] header  = createDefaultHeader();
    byte[] payload = createDefaultPayload();
    byte[] trailer = createDefaultTrailer();

    if ((payload.length == 0) && (trailer.length == 0)) {
      // This is very common
      return header;
    }
    byte[] packet  = new byte[header.length + payload.length + trailer.length];
    
    int off = 0;
    System.arraycopy(header,  0, packet, off, header.length ); off += header.length;
    System.arraycopy(payload, 0, packet, off, payload.length); off += payload.length;
    System.arraycopy(trailer, 0, packet, off, trailer.length);
    return packet;
  }

  /** Gets the initial header value for a default packet. */
  protected byte[] createDefaultHeader () {
    byte[] buf = new byte[MAX_HEADER_LENGTH];
    buf[0] = 0x18;
    buf[1] = 0x60;
    buf[2] = 0x00;
    buf[3] = 0x07;
    return buf;
  }
  
  /** Gets the initial payload value for a default packet. */
  protected byte[] createDefaultPayload () {
    return EMPTY;
  }
  
  /** Gets the initial trailer value for a default packet. */
  protected byte[] createDefaultTrailer () {
    return EMPTY;
  }
  
  @Override
  public final String toString () {
    String err = getPacketValid(false);
    return (err != null)? getClass().getName()+": <"+err+">"
                        : toStringBuilder().toString();
  }
  
  protected StringBuilder toStringBuilder () {
    StringBuilder str = new StringBuilder();
    str.append(getClass().getName()).append(":");
    
    str.append(" PacketType=").append(getPacketType());
    str.append(" PacketCount=").append(getPacketCount());
    str.append(" PacketLength=").append(getPacketLength())
                                .append(" (Header=").append(getHeaderLength())
                                .append(",Payload=").append(getPayloadLength())
                                .append(",Trailer=").append(getTrailerLength()).append(')');
    str.append(" PadBitCount=").append(getPadBitCount());
    
    append(str, " TimeStamp=",          getTimeStamp().toString());
    append(str, " StreamID=",           getStreamID());
    append(str, " ClassID=",            getClassID());
    append(str, " TimeStampMode=",      isTimeStampMode());
    return str;
  }

  /** Compares two buffer segments for equality. */
  private static boolean equals (byte[] buf1, int off1, byte[] buf2, int off2, int len) {
    if ((buf1 == buf2) && (off1 == off2)) return true; // same memory locations
    while (len > 0) {
      if (buf1[off1++] != buf2[off2++]) return false;
      len--;
    }
    return true;
  }
  
  @Override
  public final boolean headerEquals (VRTPacket p2) {
    if (p2 == this) return true;
    if (p2 == null) return false;
    
    // check some obvious things first
    int hLen = getHeaderLength();
    if (hLen != p2.getHeaderLength()) return false;

    if (p2 instanceof BasicVRTPacket) {
      BasicVRTPacket p = (BasicVRTPacket)p2;
      return equals(bbuf, boff, p.bbuf, p.boff, hLen);
    }

    byte[] p_bbuf = new byte[hLen];
    int n = p2.readPacket(p_bbuf, 0, 0, hLen);
    return (n == hLen) && equals(bbuf, boff, p_bbuf, 0, hLen);
  }
  
  @Override
  public final boolean payloadEquals (VRTPacket p2) {
    if (p2 == this) return true;
    if (p2 == null) return false;

    int pLen = getPayloadLength();
    if (pLen != p2.getPayloadLength()) return false;
    
    byte[] buf1 = getPBuf();
    int    off1 = getPOff();
    byte[] buf2;
    int    off2;
    
    if (p2 instanceof BasicVRTPacket) {
      BasicVRTPacket p = (BasicVRTPacket)p2;
      buf2 = p.getPBuf();
      off2 = p.getPOff();
    }
    else {
      buf2 = new byte[pLen];
      off2 = 0;
      int n = p2.readPacket(buf2, 0, 0, pLen);
      if (n != pLen) return false;
    }
    return equals(buf1, off1, buf2, off2, pLen);
  }


  @Override
  public final boolean trailerEquals (VRTPacket p2) {
    return trailerEquals(p2,true);
  }

  @Override
  public final boolean trailerEquals (VRTPacket p2, boolean exact) {
    if (p2 == this) return true;
    if (p2 == null) return false;
    
    boolean t1 = hasTrailer();
    boolean t2 = (p2.getTrailerLength() > 0);
    
    if (!t1 && !t2) return true; // no trailer in either one
    if (exact && (t1 != t2)) return false;

    byte[] buf1 = NULL_TRAILER;
    int    off1 = 0;
    byte[] buf2 = NULL_TRAILER;
    int    off2 = 0;

    if (t1) {
      buf1 = getTBuf();
      off1 = getTOff();
    }

    if (t2) {
      if (p2 instanceof BasicVRTPacket) {
        BasicVRTPacket p = (BasicVRTPacket)p2;
        buf2 = p.getTBuf();
        off2 = p.getTOff();
      }
      else {
        buf2 = new byte[MAX_TRAILER_LENGTH];
        off2 = 0;
        int n = p2.readPacket(buf2, 0, getPacketLength()-MAX_TRAILER_LENGTH, MAX_TRAILER_LENGTH);
        if (n != MAX_TRAILER_LENGTH) return false;
      }
    }

    if (exact) {
      return equals(buf1, off1, buf2, off2, MAX_TRAILER_LENGTH);
    }
    else {
      if ((buf1[off1+3] < 0) || (buf2[off2+3] < 0)) {
        if (buf1[off1+3] != buf2[off2+3]) return false; // assoc. context packet counts don't match
      }
      for (int e=20,i=8; e <= 31; e++,i++) {
        Boolean bit1 = getStateEventBit(buf1, off1, e, i);
        Boolean bit2 = getStateEventBit(buf2, off2, e, i);
        if (bit1 != bit2) return false; // state/event bits don't match
      }
      return true;
    }
  }
  
  @Override
  public final boolean equals (Object o) {
    if (this == o) return true;
    if (!(o instanceof VRTPacket)) return false;
    
    VRTPacket p1 = this;
    VRTPacket p2 = (VRTPacket)o;
    
    // check some obvious things first
    int hLen = p1.getHeaderLength();
    int pLen = p1.getPacketLength();
    
    if (hLen != p2.getHeaderLength()) return false;
    if (pLen != p2.getPacketLength()) return false;
    
    byte[] buf1 = new byte[4096];
    byte[] buf2 = new byte[4096];
    int    off  = 0;
    
    while (pLen > 0) {
      int a = p1.readPacket(buf1, 0, off, buf1.length);
      int b = p2.readPacket(buf2, 0, off, buf1.length);
      
      if (a != b) return false;
      for (int i = 0; i < a; i++) {
        if (buf1[i] != buf2[i]) return false;
      }
      off  += a;
      pLen -= a;
    }
    return true;
  }
  
  // hashCode() copied directly from the interface
  @Override
  public final int hashCode () {
    int    hash = 0;
    int    len  = Math.min(getPacketLength(), 32);
    byte[] buf  = new byte[len];
    
    readPacket(buf, 0, 0, len);
    for (int i = 0; i < len; i++) {
      hash ^= (buf[i] << i);
    }
    return hash;
  }
  
  /** Gets the offset for the given header field. */
  private int getOffset (int field) {
    int a0  = bbuf[ boff ] | 0x02;
    int b0  = bbuf[boff+1];
    int a   = a0 & (field >> 8);
    int b   = b0 & (field     );
    int sgn = ~(-((a0 & (field >> 24))    // If any bits in mask -> "N>0", otherwise "N=0",
                 |(b0 & (field >> 16)))); // then doing ~(-N) changes this to "N>0" and "N<0"
    int mul = (sgn >> 31) | 0x00000001;   // This changes it to "N=+1" or "N=-1"
    
    int off = 4
            + (((a >> 2) | (a >> 4)) & 0x04)   // xxx1 xxxx OR x1xx xxxx -> has stream identifier (4)
            + (( a                 ) & 0x08)   // xxxx 1xxx              -> has class identifier  (8)
            + (((b >> 5) | (b >> 4)) & 0x04)   // 1xxx xxxx OR x1xx xxxx -> has TSI               (4)
            + (((b >> 2) | (b >> 1)) & 0x08);  // xx1x xxxx OR xxx1 xxxx -> has TSF               (8)
    return mul * off;
  }
  
  /** Shifts the header by the given number of bytes for insertion/removal of the specified field. */
  private void shiftHeader (int field, int bytes, boolean present) {
    int pos = getOffset(field);
    if ((pos >= 0) == present) return; // no change
    
    if (pos < 0) { // ADD
      int off = boff - pos;
      int len = (pbuf == null)? getPacketLength() : getHeaderLength();
      System.arraycopy(bbuf, off, bbuf, off+bytes, len+pos);
      memset(bbuf, off, 0, bytes); // init any new bits to 0
      setPacketLength(getPacketLength()+bytes);
    }
    else { // REMOVE
      int off = boff + pos;
      int len = (pbuf == null)? getPacketLength() : getHeaderLength();
      System.arraycopy(bbuf, off+bytes, bbuf, off, len-pos-bytes);
      setPacketLength(getPacketLength()-bytes);
    }
  }
  
  /** Shifts the payload by the given number of bytes for insertion/removal of the specified field. */
  final int shiftPayload (int off, int bytes, boolean add) {
    int offset = (off < 0)? -off : off;
    if ((off >= 0) == add) return offset; // no change
    
    byte[] xbuf;
    int    xoff;
    int    xlen;
    int    plen = getPayloadLength();
    
    if (add) setPayloadLength(plen+bytes); // <-- may reallocate bbuf or pbuf if required
    if (pbuf != null) {
      xbuf = pbuf;
      xoff = poff + offset;
      xlen = plen;
    }
    else {
      int hlen = getHeaderLength();
      xbuf = bbuf;
      xoff = boff + hlen + offset;
      xlen = getPacketLength();
    }
    
    if (add) {
      System.arraycopy(xbuf, xoff, xbuf, xoff+bytes, xlen-xoff);
      memset(xbuf, xoff, 0, bytes); // init any new bits to 0
    }
    else {
      System.arraycopy(xbuf, xoff+bytes, xbuf, xoff, xlen-xoff-bytes);
      setPayloadLength(plen-bytes); // <-- may reallocate bbuf or pbuf if required
    }
    return offset;
  }
  
  /** Inserts/removes the trailer. */
  final void shiftTrailer (int bytes) {
    setPacketLength(getPacketLength()+bytes);
    if (bytes > 0) {
      // init all bits to 0
      if (tbuf == null) memset(bbuf, boff+getPacketLength()-4, 0, bytes);
      else              memset(tbuf, toff, 0, bytes);
    }
  }
  
  /** Sets memory in the array to the given value. */
  private void memset (byte[] buf, int boff, int val, int len) {
    byte b = (byte)val;
    for (int i=0,j=boff; i < len; i++,j++) {
      buf[j] = b;
    }
  }
  
  /** Gets the specified bit from the trailer. By default all of the "is" methods that access the
   *  trailer call this method with the appropriate bit values (no other combinations of inputs
   *  is valid):
   *  <pre>
   *    Name                Enable Indicator
   *    ====================================
   *    CalibratedTimeStamp   31      19
   *    DataValid             30      18
   *    ReferenceLocked       29      17
   *    AutomaticGainControl  28      16
   *    SignalDetected        27      15
   *    InvertedSpectrum      26      14
   *    OverRange             25      13
   *    Discontinuious        24      12
   *    Bit11                 23      11 (Data and ExtData Only)
   *    Bit10                 22      10 (Data and ExtData Only)
   *    Bit9                  21       9 (Data and ExtData Only)
   *    Bit8                  20       8 (Data and ExtData Only)
   *  </pre>
   *  @param enable    Bit position of the enable flag.
   *  @param indicator Bit position of the indicator flag.
   *  @return null if there is no trailer, null if the enable bit is not set, true if the enable
   *          bit is set and the indicator bit is set, false if the enable bit is set but the
   *          indicator bit is not set.
   */
  static Boolean getStateEventBit (byte[] buf, int off, int enable, int indicator) {
    int eByte = 3 - (enable    / 8);
    int iByte = 3 - (indicator / 8);
    int eBit  = 0x1 << (enable    % 8);
    int iBit  = 0x1 << (indicator % 8);
    
    return ((buf[off+eByte] & eBit) == 0)? null : ((buf[off+iByte] & iBit) != 0);
  }
  
  /** <i>Optional functionality:</i> Sets the specified bit from the trailer. By default all of
   *  the "set" methods that access the trailer call this method with the appropriate bit values
   *  (see {@link #getTrailerBit(int,int)} for a list).
   *  @param enable    Bit position of the enable flag.
   *  @param indicator Bit position of the indicator flag.
   *  @param value     The value of the bits: null if the enable bit should not be set, true if
   *                   the enable bit and the indicator bit should be set, false if the enable bit
   *                   should be set but the indicator bit should not be set.
   */
  static void setStateEventBit (byte[] buf, int off, int enable, int indicator, Boolean value) {
    int eByte = 3 - (enable    / 8);
    int iByte = 3 - (indicator / 8);
    int eBit  = 0x1 << (enable    % 8);
    int iBit  = 0x1 << (indicator % 8);
    
    if (value == null) {
      buf[off+eByte] &= ~eBit;
      buf[off+iByte] &= ~iBit;
    }
    else if (value == TRUE) {
      buf[off+eByte] |= eBit;
      buf[off+iByte] |= iBit;
    }
    else {
      buf[off+eByte] |=  eBit;
      buf[off+iByte] &= ~iBit;
    }
  }
  
  // The following "constants" are only used in isPacketValid() and indicate the bits in the first
  // octet of the ClassID that are reserved.
  private static final int CONTEXT_CLASSID_RESERVED = 0xFF;
  private static final int DATA_CLASSID_RESERVED    = (VRT_VERSION == VITAVersion.V49b)? 0x07 : 0xFF;
  
  @Override
  public String getPacketValid (boolean strict, int length) {
    // This method is called a LOT so it has been re-written to eliminate any branches outside of
    // the error case.
    boolean data = isData();
    int     cid  = getOffset(CLASS_ID);
    boolean resOK = !strict || (( data && ((bbuf[boff] & 0x3) == 0))               // Fixed Reserved Bits (data)
                            ||  (!data && ((bbuf[boff] & 0x6) == 0)));             // Fixed Reserved Bits (context)
    boolean cidOK = !strict || (cid < 0)
                            || ( data && ((bbuf[boff+cid] & DATA_CLASSID_RESERVED   ) == 0))  // ClassID Reserved Bits (data)
                            || (!data && ((bbuf[boff+cid] & CONTEXT_CLASSID_RESERVED) == 0)); // ClassID Reserved Bits (context)
    // Check lengths
    int     pLen     = getPacketLength();
    int     hLen     = getHeaderLength();
    int     tLen     = getTrailerLength();
    boolean sizeMin  = (pLen >= hLen+tLen);
    boolean sizeOk1  = (pbuf == null) && (bbuf.length - boff >=      pLen     );
    boolean sizeOk2  = (pbuf != null) && (bbuf.length - boff >=      hLen     )
                                      && (pbuf.length - poff >= pLen-hLen-tLen)
                                      && (tbuf.length - toff >=      tLen     );
    boolean lenOK    = (length == -1) || (length == pLen);
    boolean allOK    = resOK && cidOK && sizeMin && lenOK && (sizeOk1 || sizeOk2);

    if (allOK) {
      return null;
    }
    else {
      if (!resOK  ) return "Invalid VRTPacket: Reserved bit(s) set to 1 but expected 0.";
      if (!cidOK  ) return "Invalid VRTPacket: Reserved ClassID bit(s) set to 1 but "
                                            + "expected 0 (using VRT_VERSION="+VRT_VERSION+").";
      if (!sizeMin) return "Invalid VRTPacket: Invalid packet length, packet reports "+pLen+" "
                                             + "octets total, but has "+hLen+"+"+tLen+" octets "
                                             + "in header+trailer.";
      if (!lenOK  ) return "Invalid VRTPacket: Invalid packet length, packet reports "+pLen+" "
                                            + "octets, but working with "+length+" octets.";
      if (!sizeOk1) return "Invalid VRTPacket: Allocated buffer shorter than packet length.";
      if (!sizeOk2) return "Invalid VRTPacket: Allocated buffers shorter than packet length.";
                    return "Invalid VRTPacket: Unknown issue with packet.";
    }
  }

  @Override public final boolean isPacketValid ()                { return getPacketValid(true,    -1) == null; }
  @Override public final boolean isPacketValid (int len)         { return getPacketValid(true,   len) == null; }
  @Override public final String  getPacketValid (boolean strict) { return getPacketValid(strict,  -1); }
  @Override public       boolean resetForResend (TimeStamp t)    { setTimeStamp(t); return false; }
  @Override public       boolean isChangePacket ()               { return true;  }
  @Override public final Boolean isTimeStampMode ()              { return (isData())? null : ((bbuf[boff]&TSM_BIT)!=0); }
  @Override public final int     getPacketCount ()               { return bbuf[boff+1] & 0xF; }
  @Override public final int     getHeaderLength ()              { return getOffset(PAYLOAD); }
  @Override public final int     getPayloadLength ()             { return getPacketLength() - getHeaderLength() - getTrailerLength(); }

  @Override
  public final int getPacketLength () {
    // This function is used a LOT so make it fast.
    // handles endianness, unsignedness and word-to-byte convert
    return ((0xFF & bbuf[boff+2]) << 10) | ((0xFF & bbuf[boff+3]) << 2);
  }

  private void setPacketLength (int v) {
    if ((v & 0x3) != 0) {
      throw new IllegalArgumentException("Given packet length ("+v+") is not a multiple of 4.");
    }
    packShort(bbuf, boff+2, (short)(v>>>2)); // divide by 4 to convert bytes to words
  }

  @Override
  public final int getPadBitCount () {
    // This function optimizes out the "isData()" and "VRT_VERSION == V49b" checks since all other
    // uses require the bits to be set to zero (which matches the required return value for this
    // function in those situations). The isPacketValid() check will allert the user if the bits
    // were incorrectly set.
    int cid = getOffset(CLASS_ID);

    return (cid > 0)? ((bbuf[boff+cid] >> 3) & 0x1F) : 0;
  }

  @Override
  public final void setPadBitCount (int bits, int bitsPerSample) {
    if (readOnly) throw new UnsupportedOperationException("Can not write to read-only VRTPacket.");
    int cid = getOffset(CLASS_ID);

    // bits==0 is most common case so check that first
    if (bits == 0) {
      // If a ClassID is present, make sure the bits are set to 0 (this works with both
      // VITA-49.0 and VITA-49.0b since the bits should be all zero in VITA-49.0b), this
      // also works with context packets (the bits should be all zero here too). If there
      // is no ClassID, don't worry about anything (no ClassID -> no pad bits)
      if (cid > 0) {
        bbuf[boff+cid] = 0;
      }
    }
    else if (!isData()) {
      // Context packets expect PadBitCount=0
      throw new IllegalArgumentException("Can not set PadBitCount to "+bits+" for a context packet.");
    }
    else if ((bits < 0) || (bits > 31)) {
      throw new IllegalArgumentException("Given PadBitCount of "+bits+" is outside of allowable limits.");
    }
    else if (bits < bitsPerSample) {
      // Value is implicit, setting is optional (so clear it out to match V49)
      if (cid > 0) {
        bbuf[boff+cid] = 0;
      }
    }
    else if (cid < 0) {
      // Value is not implicit, but can not set
      throw new IllegalArgumentException("Can not set PadBitCount to "+bits+" without a ClassID set.");
    }
    else if (VRT_VERSION != VITAVersion.V49b) {
      // Value is not implicit, but can not set
      throw new IllegalArgumentException("Can only set PadBitCount to "+bits+" when using VRT_VERSION=V49b.");
    }
    else {
      // Have a ClassID and using VITA-49.0b so set the bits
      bbuf[boff+cid] = (byte)(bits << 3);
    }
  }

  private boolean isData () {
    // This used to be implemented as
    //    return getPacketType().isData();
    // but changed to make it faster since getTrailerLength() is called frequently
    // and relies on this method
    return ((bbuf[boff] & 0xC0) == 0); // i.e. accept types 0x0o, 0x1o, 0x2o, 0x3o
  }
  
  protected final boolean hasTrailer () {
    // This used to be implemented as
    //    return isData() && ((bbuf[boff]&TRAILER_BIT) != 0);
    // but changed to make it faster since getTrailerLength() is called frequently
    return ((bbuf[boff] & 0xC4) == 0x4); // i.e. accept types 0x0o, 0x1o, 0x2o, 0x3o with 0x04 set
  }
  
  @Override
  public final int getTrailerLength () {
    // This used to be implemented as
    //    return (hasTrailer())? MAX_TRAILER_LENGTH : 0;
    // but has been changed to eliminate the branch
    int n = bbuf[boff];
    int e = ((n>>4) | (n>>5)) & 0x4; // set e=0x4 if not a data packet
    return n & 0x4 & ~e; // if data, and if trailer present this will be 0x4 which matches the trailer length of 4
  }
  
  @Override
  public final PacketType getPacketType () {
    // This used to be implemented as
    //    return PacketType.values()[(bbuf[boff] >>> 4) & 0xF];
    // but for some reason the PacketType.values() was a performance hot spot.
    return PacketType.VALUES[(bbuf[boff] >>> 4) & 0xF];
  }
  
  @Override
  public void setPacketType (PacketType t) {
    if (readOnly) throw new UnsupportedOperationException("Can not write to read-only VRTPacket.");
    
    shiftHeader(STREAM_ID, 4, t.hasStreamIdentifier());
    bbuf[boff] = (byte)((0x0F & bbuf[boff]) | (t.ordinal() << 4));
    
    if (t.isData()) bbuf[boff] &= ~TSM_BIT;     // clear TSM bit
    else            bbuf[boff] &= ~TRAILER_BIT; // clear trailer bit
  }
  
  @Override
  public void setTimeStampMode (boolean v) {
    if (readOnly) throw new UnsupportedOperationException("Can not write to read-only VRTPacket.");
    
    if (getPacketType().isData()) {
      throw new UnsupportedOperationException("Can not set time-stamp mode on a data packet.");
    }
    if (v) bbuf[boff] |=  TSM_BIT;
    else   bbuf[boff] &= ~TSM_BIT;
  }
  
  @Override
  public final TimeStamp getTimeStamp () {
    IntegerMode    tsiMode = IntegerMode.VALUES[(bbuf[boff+1] >>> 6) & 0x3];
    FractionalMode tsfMode = FractionalMode.VALUES[(bbuf[boff+1] >>> 4) & 0x3];
    int            tsiOff  = getOffset(TSI);
    int            tsfOff  = getOffset(TSF);
    Long           tsi     = (tsiOff < 0)? null : unpackInt(bbuf, boff+tsiOff) & 0xFFFFFFFFL;
    Long           tsf     = (tsfOff < 0)? null : unpackLong(bbuf, boff+tsfOff);
    
    return new TimeStamp(tsiMode, tsfMode, tsi, tsf);
  }
  
  @Override
  public void setTimeStamp (TimeStamp ts) {
    if (readOnly) throw new UnsupportedOperationException("Can not write to read-only VRTPacket.");
    if (ts == null) ts = TimeStamp.NO_TIME_STAMP;
    
    IntegerMode    im = ts.getIntegerMode();
    FractionalMode fm = ts.getFractionalMode();
    
    // The next 4 lines must follow in the order shown!
    shiftHeader(TSI, 4, (im != IntegerMode.None));
    bbuf[boff+1] = (byte)((0x3F & bbuf[boff+1]) | (im.ordinal() << 6));
    shiftHeader(TSF, 8, (fm != FractionalMode.None));
    bbuf[boff+1] = (byte)((0xCF & bbuf[boff+1]) | (fm.ordinal() << 4));
    
    if (im != IntegerMode.None) {
      packInt(bbuf, boff+getOffset(TSI), ts.getTimeStampInteger().intValue());
    }
    if (fm != FractionalMode.None) {
      packLong(bbuf, boff+getOffset(TSF), ts.getTimeStampFractional());
    }
  }
  
  @Override
  public void setPacketCount (int v) {
    if (readOnly) throw new UnsupportedOperationException("Can not write to read-only VRTPacket.");
    if ((v < 0) || (v > 15)) throw new IllegalArgumentException("Invalid packet count "+v);
    bbuf[boff+1] = (byte)((bbuf[boff+1] & 0xF0) | v);
  }
  
  @Override
  public void setPayloadLength (int v) {
    if (readOnly) throw new UnsupportedOperationException("Can not write to read-only VRTPacket.");
    if (v == getPayloadLength()) return; // no change
    
    if ((v < 0) || (v > MAX_PAYLOAD_LENGTH)) {
      throw new IllegalArgumentException("Given length of "+v+" is outside of allowable limits.");
    }
    if ((v & 0x3) != 0) {
      throw new IllegalArgumentException("Given length of "+v+" is not a multiple of 4-bytes.");
    }
    
    if (direct) {
      if (hasTrailer() && (tbuf == null)) {
        int off1 = boff + getPacketLength() - 4; // old trailer start
        int off2 = boff + getHeaderLength() + v; // new trailer start
        System.arraycopy(bbuf, off1, bbuf, off2, 4);
      }
    }
    else if (v > pbuf.length) {
      int    len = (int)ceil(v / 4096.0) * 4096; // shift an even interval of 4k
      byte[] buf = new byte[len];
      System.arraycopy(pbuf, 0, buf, 0, pbuf.length);
      pbuf = buf;
    }
    else {
      // length is adequate + no trailer to move -> nothing to do
    }
    setPacketLength(getHeaderLength() + v + getTrailerLength());
  }
  
  @Override
  public final Integer getStreamIdentifier () {
    int off = getOffset(STREAM_ID);
    return (off < 0)? null : unpackInt(bbuf, boff+off);
  }
  
  @Override
  public void setStreamIdentifier (Integer v) {
    if (readOnly) throw new UnsupportedOperationException("Can not write to read-only VRTPacket.");
    
    if ((v == null) == getPacketType().hasStreamIdentifier()) {
      throw new IllegalArgumentException("Can not set stream identifier to "+v+" for a "+getPacketType()+" packet.");
    }
    if (v != null) {
      packInt(bbuf, boff+getOffset(STREAM_ID), v);
    }
  }
  
  @Override
  public final Long getClassIdentifier () {
    int off = getOffset(CLASS_ID);
    return (off < 0)? null : (0x00FFFFFFFFFFFFFFL & unpackLong(bbuf, boff+off));
  }
  
  @Override
  public final String getStreamID () {
    Integer streamID = getStreamIdentifier();
    return (streamID == null)? null : streamID.toString();
  }
  
  @Override
  public final void setStreamID (CharSequence v) {
    if (v == null) { setStreamIdentifier(null); return; }
    
    try {
      setStreamIdentifier(Integer.parseInt(v.toString()));
    }
    catch (NumberFormatException e) {
      throw new IllegalArgumentException("Invalid stream ID ("+v+").");
    }
  }
  
  @Override
  public final String getClassID () {
    Long id = getClassIdentifier();
    if (id == null) return null;
    return Utilities.toStringClassID(id);
  }
  
  @Override
  public final void setClassID (CharSequence v) {
    setClassIdentifier(Utilities.fromStringClassID(v));
  }
  
  @Override
  public void setClassIdentifier (Long v) {
    if (readOnly) throw new UnsupportedOperationException("Can not write to read-only VRTPacket.");
    
    if ((v != null) && ((v & 0xFF00000000000000L) != 0)) {
      throw new IllegalArgumentException("Invalid OUI in ClassIdentifier 0x"+Long.toHexString(v));
    }
    
    int  oldCID   = getOffset(CLASS_ID);
    byte saveBits = (oldCID < 0)? 0 : bbuf[boff+oldCID];
    
    if ((v == null) && (saveBits != 0)) {
      throw new IllegalArgumentException("Can not set ClassIdentifier to null when PadBitCount="+getPadBitCount());
    }
    
    shiftHeader(CLASS_ID, 8, (v != null));
    if (v != null) {
      bbuf[boff] |= CLASS_ID_BIT;
      
      int newCID = getOffset(CLASS_ID);
      
      packLong(bbuf, boff+newCID, v);
      bbuf[boff+newCID] = saveBits;
    }
    else {
      bbuf[boff] &= ~CLASS_ID_BIT;
    }
  }
  
  @Override
  public void setClassIdentifier (int oui, short icc, short pcc) {
    long _oui = (oui & 0xFFFFFFFFL) << 32;
    long _icc = (icc & 0x0000FFFFL) << 16;
    long _pcc = (pcc & 0x0000FFFFL);
    setClassIdentifier(_oui|_icc|_pcc);
  }
  
  @Override
  public final Integer getClassIdentifierOUI () {
    int off = getOffset(CLASS_ID);
    return (off < 0)? null : unpackInt(bbuf, boff+off) & 0x00FFFFFF;
  }
  
  @Override
  public final Short getClassIdentifierICC () {
    int off = getOffset(CLASS_ID);
    return (off < 0)? null : unpackShort(bbuf, boff+off+4);
  }
  
  @Override
  public final Short getClassIdentifierPCC () {
    int off = getOffset(CLASS_ID);
    return (off < 0)? null : unpackShort(bbuf, boff+off+6);
  }
  
  @Override
  public void writePacketTo (OutputStream out) throws IOException {
    if (pbuf == null) {
      out.write(bbuf, boff, getPacketLength());
    }
    else {
      out.write(bbuf, boff, getHeaderLength());
      out.write(pbuf, poff, getPayloadLength());
      out.write(tbuf, toff, getTrailerLength());
    }
  }
  
  @Override
  public int readPacketFrom (InputStream in) throws IOException {
    return readPacketFrom(in, null, 0);
  }

  /** <b>Internal use only:</b> This allows the first four bytes (and it must be exactly 4) to be
   *  passed in pre-read without creating an additional PushbackInputStream. This is usually done
   *  within code that supports reading of VRT packets or VRL frames (see
   *  {@link VRLFrame#readFrameFrom(InputStream)}).
   */
  public int readPacketFrom (InputStream in, byte[] hdr, int hoff) throws IOException {
    if (readOnly) throw new UnsupportedOperationException("Can not write to read-only VRTPacket.");
    
    // Read the initial header in
    if (hdr != null) {
      System.arraycopy(hdr, hoff, bbuf, boff, 4);
    }
    else {
      _read(in, bbuf, boff, 4);
    }
    
    if (pbuf == null) {
      // Check sizing
      int len = getPacketLength();

      if (bbuf.length < boff+len) {
        if (direct) {
          in.skip(len-4);
          throw new ArrayIndexOutOfBoundsException("Packet contents exceeds buffer length");
        }
      }
      _read(in, bbuf, boff+4, getPacketLength()-4);
    }
    else {
      int len = getPayloadLength();
      if (pbuf.length < poff+len) {
        if (direct) {
          throw new ArrayIndexOutOfBoundsException("Packet contents exceeds buffer length");
        }
        else {
          pbuf = new byte[len+poff];
        }
      }
      else if (!direct && (pbuf.length > (len+poff+4096))) {
        pbuf = new byte[len+poff];
      }

      _read(in, bbuf, boff+4, getHeaderLength()-4);
      _read(in, pbuf,  poff,  getPayloadLength());
      _read(in, tbuf,  toff,  getTrailerLength());
    }
    return getPacketLength();
  }

  /** Calls InputStream.read(..) until the entire length requested is read in. If the EOF comes
   *  before it should, an EOFException is thrown.
   */
  static byte[] _read (InputStream in, byte[] buf, int off, int len) throws IOException {
    if (buf == null) buf = new byte[off+len];

    int numRead = 0;
    while (numRead < len) {
      int count = in.read(buf, off, len);

      if (count < 0) {
        throw new EOFException("Premature end of VRT packet when reading from "+in);
      }
      numRead += count;
      off     += count;
    }
    return buf;
  }
  
  @Override
  public final byte[] getPacket () {
    byte[] buffer = new byte[getPacketLength()];
    readPacket(buffer, 0, 0, buffer.length);
    return buffer;
  }
  
  @Override
  public final int readPacket (byte[] buffer) {
    return readPacket(buffer, 0, 0, buffer.length);
  }
  
  @Override
  public final byte[] getPayload () {
    byte[] buffer = new byte[getPayloadLength()];
    readPayload(buffer, 0, 0, buffer.length);
    return buffer;
  }
  
  @Override
  public final int readPayload (byte[] buffer) {
    if (buffer == null) throw new NullPointerException("Buffer is null.");
    return readPayload(buffer, 0, 0, buffer.length);
  }
  
  @Override
  public int readPacket (byte[] buffer, int bufferOffset, int packetOffset, int len) {
    int packetLength = getPacketLength();
    
    if (buffer == null) {
      throw new NullPointerException("Buffer is null.");
    }
    if (buffer.length-bufferOffset < len) {
      throw new ArrayIndexOutOfBoundsException("Buffer length ("+buffer.length+") is shorter than "
              + "that required by buffer offset ("+bufferOffset+") and length ("+len+").");
    }
    
    len = min(len, packetLength-packetOffset);
    if (len <= 0) {
      return -1;
    }
    
    if (pbuf == null) {
      System.arraycopy(bbuf, boff+packetOffset, buffer, bufferOffset, len);
    }
    else {
      int avail         = len;
      int headerLength  = getHeaderLength();
      int trailerLength = getTrailerLength();
      int payloadLength = getPacketLength() - headerLength - trailerLength;
      
      if ((packetOffset < headerLength) && (avail > 0)) {
        int n = min(avail, headerLength);
        System.arraycopy(bbuf, boff+packetOffset, buffer, bufferOffset, n);
        avail -= n;
        packetOffset += n-headerLength;
        bufferOffset += n;
      }
      
      if ((packetOffset < payloadLength) && (avail > 0)) {
        int n = min(avail, payloadLength);
        System.arraycopy(pbuf, poff+packetOffset, buffer, bufferOffset, n);
        avail -= n;
        packetOffset += n-payloadLength;
        bufferOffset += n;
      }
      
      if ((packetOffset < trailerLength) && (avail > 0)) {
        int n = min(avail, trailerLength);
        System.arraycopy(tbuf, toff+packetOffset, buffer, bufferOffset, n);
        avail -= n;
        packetOffset += n;
        bufferOffset += n;
      }
    }
    return len;
  }
  
  @Override
  public int readPayload (byte[] buffer, int bufferOffset, int payloadOffset, int len) {
    if (buffer == null) {
      throw new NullPointerException("Buffer is null.");
    }
    if (buffer.length-bufferOffset < len) {
      throw new ArrayIndexOutOfBoundsException("Buffer length ("+buffer.length+") is shorter than "
              + "that required by buffer offset ("+bufferOffset+") and length ("+len+").");
    }
    
    int payloadLength = getPayloadLength();
    if (payloadOffset > payloadLength) return -1;
    
    len = min(len, payloadLength-payloadOffset);
    
    System.arraycopy(getPBuf(), getPOff()+payloadOffset, buffer, bufferOffset, len);
    return len;
  }
  
  @Override
  public final void setPayload (byte[] buffer) {
    if (buffer == null) throw new NullPointerException("Buffer is null.");
    setPayload(buffer, 0, buffer.length);
  }
  
  @Override
  public final void setPayload (byte[] buffer, int offset, int length) {
    if (buffer == null) throw new NullPointerException("Buffer is null.");
    if ((offset < 0) || (offset > buffer.length)) {
      throw new IllegalArgumentException("Illegal buffer offset ("+offset+").");
    }
    if (buffer.length < length+offset) {
      throw new ArrayIndexOutOfBoundsException("Illegal length ("+length+").");
    }
    setPayloadLength(length);
    writePayload(buffer, offset, 0, length);
  }
  
  @Override
  public void writePayload (byte[] buffer, int bufferOffset, int payloadOffset, int len) {
    if (readOnly) throw new UnsupportedOperationException("Can not write to read-only VRTPacket.");
    
    if (buffer == null) {
      throw new NullPointerException("Buffer is null.");
    }
    if (buffer.length-bufferOffset < len) {
      throw new ArrayIndexOutOfBoundsException("Buffer length ("+buffer.length+") is shorter than "
              + "that required by buffer offset ("+bufferOffset+") and length ("+len+").");
    }
    if (len+payloadOffset > getPayloadLength()) {
      throw new ArrayIndexOutOfBoundsException("Bytes to be written to payload ("+len+") with given "
              + "start offset ("+payloadOffset+") exceeds payload length ("+getPayloadLength()+").");
    }
    
    System.arraycopy(buffer, bufferOffset, getPBuf(), getPOff()+payloadOffset, len);
  }

  /** Gets the payload buffer. */
  protected final byte[] getPBuf () {
    return (pbuf != null)? pbuf : bbuf; // <-- Java should inline this
  }

  /** Gets the payload buffer offset. */
  protected final int getPOff () {
    return (pbuf != null)? poff : boff+getHeaderLength(); // <-- Java should inline this
  }
  
  /** Gets the trailer buffer. */
  protected final byte[] getTBuf () {
    return (tbuf != null)? tbuf : bbuf; // <-- Java should inline this
  }
  
  /** Gets the trailer buffer offset. */
  protected final int getTOff () {
    return (tbuf != null)? toff : boff+getPacketLength()-getTrailerLength(); // <-- Java should inline this
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // PAYLOAD PACK/UNPACK METHODS
  //////////////////////////////////////////////////////////////////////////////////////////////////
  protected final void packPayloadByte      (int off, byte    val) { VRTMath.packByte(    getPBuf(), getPOff()+off, val); }
  protected final void packPayloadShort     (int off, short   val) { VRTMath.packShort(   getPBuf(), getPOff()+off, val); }
  protected final void packPayloadInt       (int off, int     val) { VRTMath.packInt(     getPBuf(), getPOff()+off, val); }
  protected final void packPayloadLong      (int off, long    val) { VRTMath.packLong(    getPBuf(), getPOff()+off, val); }
  protected final void packPayloadFloat     (int off, float   val) { VRTMath.packFloat(   getPBuf(), getPOff()+off, val); }
  protected final void packPayloadDouble    (int off, double  val) { VRTMath.packDouble(  getPBuf(), getPOff()+off, val); }
  protected final void packPayloadBoolNull  (int off, Boolean val) { VRTMath.packBoolNull(getPBuf(), getPOff()+off, val); }
  protected final void packPayloadBoolean   (int off, boolean val) { VRTMath.packBoolean( getPBuf(), getPOff()+off, val); }
  protected final void packPayloadRecord    (int off, Record  val) { VRTMath.packRecord(  getPBuf(), getPOff()+off, val); }
  protected final void packPayloadMetadata  (int off, MetadataBlock val,           int len          ) { VRTMath.packMetadata( getPBuf(), getPOff()+off, val, len); }
  protected final void packPayloadBytes     (int off, byte[]        val,           int len          ) { VRTMath.packBytes(    getPBuf(), getPOff()+off, val, len); }
  protected final void packPayloadBytes     (int off, byte[]        val, int boff, int len          ) { VRTMath.packBytes(    getPBuf(), getPOff()+off, val, boff, len); }
  protected final void packPayloadTimeStamp (int off, TimeStamp     val, TimeStamp.IntegerMode epoch) { VRTMath.packTimeStamp(getPBuf(), getPOff()+off, val, epoch); }
  protected final void packPayloadInetAddr  (int off, InetAddress   val                             ) { VRTMath.packInetAddr( getPBuf(), getPOff()+off, val); }
  protected final void packPayloadUUID      (int off, UUID          val                             ) { VRTMath.packUUID(     getPBuf(), getPOff()+off, val); }

  protected final void packPayloadAscii (int off, CharSequence val, int len) {
    if (len < 0) { // compute max length
      int max = -len;
      setPayloadLength(off+max);
      int act = VRTMath.packAscii(getPBuf(), getPOff()+off, val, max);
      if (act < max) setPayloadLength(off+act);
    }
    else {
      VRTMath.packAscii(getPBuf(), getPOff()+off, val, len);
    }
  }

  protected final void packPayloadUTF8 (int off, CharSequence val, int len) {
    if (len < 0) { // compute max length
      int max = -len;
      setPayloadLength(off+max);
      int act = VRTMath.packUTF8(getPBuf(), getPOff()+off, val, max);
      if (act < max) setPayloadLength(off+act);
    }
    else {
      VRTMath.packAscii(getPBuf(), getPOff()+off, val, len);
    }
  }

  protected final byte                 unpackPayloadByte      (int off) { return VRTMath.unpackByte(    getPBuf(), getPOff()+off); }
  protected final short                unpackPayloadShort     (int off) { return VRTMath.unpackShort(   getPBuf(), getPOff()+off); }
  protected final int                  unpackPayloadInt       (int off) { return VRTMath.unpackInt(     getPBuf(), getPOff()+off); }
  protected final long                 unpackPayloadLong      (int off) { return VRTMath.unpackLong(    getPBuf(), getPOff()+off); }
  protected final float                unpackPayloadFloat     (int off) { return VRTMath.unpackFloat(   getPBuf(), getPOff()+off); }
  protected final double               unpackPayloadDouble    (int off) { return VRTMath.unpackDouble(  getPBuf(), getPOff()+off); }
  protected final Boolean              unpackPayloadBoolNull  (int off) { return VRTMath.unpackBoolNull(getPBuf(), getPOff()+off); }
  protected final Boolean              unpackPayloadBoolean   (int off) { return VRTMath.unpackBoolean( getPBuf(), getPOff()+off); }
  protected final <T extends Record> T unpackPayloadRecord    (int off, T rec                        ) { return VRTMath.unpackRecord(   getPBuf(), getPOff()+off, rec); }
  protected final AsciiCharSequence    unpackPayloadAscii     (int off,                       int len) { return VRTMath.unpackAscii(    getPBuf(), getPOff()+off, len); }
  protected final CharSequence         unpackPayloadUTF8      (int off,                       int len) { return VRTMath.unpackUTF8(     getPBuf(), getPOff()+off, len); }
  protected final MetadataBlock        unpackPayloadMetadata  (int off,                       int len) { return VRTMath.unpackMetadata( getPBuf(), getPOff()+off, len); }
  protected final byte[]               unpackPayloadBytes     (int off,                       int len) { return VRTMath.unpackBytes(    getPBuf(), getPOff()+off, len); }
  protected final byte[]               unpackPayloadBytes     (int off, byte[] val, int boff, int len) { return VRTMath.unpackBytes(    getPBuf(), getPOff()+off, val, boff, len); }
  protected final TimeStamp            unpackPayloadTimeStamp (int off, TimeStamp.IntegerMode epoch  ) { return VRTMath.unpackTimeStamp(getPBuf(), getPOff()+off, epoch); }
  protected final InetAddress          unpackPayloadInetAddr  (int off                               ) { return VRTMath.unpackInetAddr( getPBuf(), getPOff()+off); }
  protected final UUID                 unpackPayloadUUID      (int off                               ) { return VRTMath.unpackUUID(     getPBuf(), getPOff()+off); }
  
  //////////////////////////////////////////////////////////////////////////////////////////////////
  // HASFIELDS METHODS
  //////////////////////////////////////////////////////////////////////////////////////////////////
  @Override
  public int getFieldCount () {
    return 3;
  }

  @Override
  public String getFieldName (int id) {
    switch (id) {
      case 0: return "StreamID";
      case 1: return "ClassID";
      case 2: return "TimeStamp";
      default: throw new IllegalArgumentException("Invalid field #"+id+" in "+getClass());
    }
  }

  @Override
  public Class<?> getFieldType (int id) {
    switch (id) {
      case 0: return String.class;
      case 1: return String.class;
      case 2: return TimeStamp.class;
      default: throw new IllegalArgumentException("Invalid field #"+id+" in "+getClass());
    }
  }

  @Override
  public Object getField (int id) {
    switch (id) {
      case 0:  return getStreamID();
      case 1:  return getClassID();
      case 2:  return getTimeStamp();
      default: throw new IllegalArgumentException("Invalid field #"+id+" in "+getClass());
    }
  }

  @Override
  public void setField (int id, Object val) {
    switch (id) {
      case 0:  setStreamID( Utilities.castValue(String.class,    val)); break;
      case 1:  setClassID(  Utilities.castValue(String.class,    val)); break;
      case 2:  setTimeStamp(Utilities.castValue(TimeStamp.class, val)); break;
      default: throw new IllegalArgumentException("Invalid field #"+id+" in "+getClass());
    }
  }

  @Override
  public final int getFieldID (String name) {
    return Utilities.getFieldID(this, name);
  }
  
  @Override
  public final Object getFieldByName (String name) {
    return Utilities.getFieldByName(this, name);
  }

  @Override
  public final void setFieldByName (String name, Object val) {
    Utilities.setFieldByName(this, name, val);
  }
}

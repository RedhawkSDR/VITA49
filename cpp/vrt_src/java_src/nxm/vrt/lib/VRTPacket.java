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

import java.nio.ByteOrder;
import java.util.Comparator;
import java.io.EOFException;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/** Defines a VRT packet type. The most frequently used implementations of interface this is
 *  {@link BasicVRTPacket} and its subclasses {@link BasicDataPacket} and {@link BasicContextPacket}. <br>
 *  <br>
 *  Note that the numeric representation (byte order) used by all VRT packets is
 *  {@link ByteOrder#BIG_ENDIAN}.
 *
 *  @author 
 */
public interface VRTPacket {
  /** The maximum length of the packet in bytes (header + payload + trailer). Note that this
   *  is the absolute limit imposed by the VRT specification. The underlying transport protocols
   *  used (e.g. IP, UDP, TCP, etc.) have limits lower than this.
   *  <pre>
   *    MAX_PACKET_LENGTH = {@value}
   *  </pre>
   *  @see #getHeaderLength()
   */
  static final int MAX_PACKET_LENGTH = 65536*4;
  
  /** The maximum length of the header in bytes (all optional fields included). Note that
   *  in cases where optional fields are not included, the header will be shorter.
   *  <pre>
   *    MAX_HEADER_LENGTH = {@value}
   *  </pre>
   *  @see #getHeaderLength()
   */
  static final int MAX_HEADER_LENGTH = 7*4;
  
  /** The maximum length of the trailer in bytes.
   *  <pre>
   *    MAX_TRAILER_LENGTH = {@value}
   *  </pre>
   *  @see #getTrailerLength()
   */
  static final int MAX_TRAILER_LENGTH = 1*4;
  
  /** The <b>de-facto</b> maximum length of the payload in bytes. Note that the de-jure limit
   *  can be up to 28 bytes more than this when optional fields in the header and trailer are
   *  dropped. In practice this difference is nearly meaningless since (as noted with
   *  {@link #MAX_PACKET_LENGTH}) this value exceeds the limits the underlying transport protocols.
   *  <pre>
   *    MAX_PAYLOAD_LENGTH = MAX_PACKET_LENGTH - MAX_HEADER_LENGTH - MAX_TRAILER_LENGTH = {@value}
   *  </pre>
   *  @see #getPayloadLength()
   */
  static final int MAX_PAYLOAD_LENGTH = MAX_PACKET_LENGTH - MAX_HEADER_LENGTH - MAX_TRAILER_LENGTH;
  
  /** An Invalid VRT packet. This is used as default return value for methods that must return
   *  a {@link VRTPacket} but can not normally do so because the data they have is invalid.
   */
  static final VRTPacket INVALID_PACKET = new BasicVRTPacket(new byte[4], 0, 4, true, true);

  /** A comparator that will compare two VRTPackets based on their time stamp. This is check
   *  implemented as:
   *  <pre>
   *    p1.getTimeStamp().compareTo(p2.getTimeStamp())
   *  </pre>
   */
  static final Comparator<VRTPacket> TIME_STAMP_COMPARATOR = new Comparator<VRTPacket>() {
    @Override
    public int compare(VRTPacket p1, VRTPacket p2) {
      return p1.getTimeStamp().compareTo(p2.getTimeStamp());
    }
  };

  /** Standard payload format: Real, signed 4-bit integer. */
  public static final PayloadFormat INT4        = new PayloadFormat(DataType.Int4);
  /** Standard payload format: Real, signed 8-bit integer. */
  public static final PayloadFormat INT8        = new PayloadFormat(DataType.Int8);
  /** Standard payload format: Real, signed 16-bit integer. */
  public static final PayloadFormat INT16       = new PayloadFormat(DataType.Int16);
  /** Standard payload format: Real, signed 32-bit integer. */
  public static final PayloadFormat INT32       = new PayloadFormat(DataType.Int32);
  /** Standard payload format: Real, signed 64-bit integer. */
  public static final PayloadFormat INT64       = new PayloadFormat(DataType.Int64);
  /** Standard payload format: Real, 32-bit float. */
  public static final PayloadFormat FLOAT32     = new PayloadFormat(DataType.Float);
  /** Standard payload format: Real, 64-bit double. */
  public static final PayloadFormat DOUBLE64    = new PayloadFormat(DataType.Double);
  /** Standard payload format: Real, unsigned 1-bit packed bits. */
  public static final PayloadFormat UINT1       = new PayloadFormat(DataType.UInt1);
  /** Standard payload format: Real, unsigned 4-bit integer. */
  public static final PayloadFormat UINT4       = new PayloadFormat(DataType.UInt4);
  /** Standard payload format: Real, unsigned 8-bit integer. */
  public static final PayloadFormat UINT8       = new PayloadFormat(DataType.UInt8);
  /** Standard payload format: Real, unsigned 16-bit integer. */
  public static final PayloadFormat UINT16      = new PayloadFormat(DataType.UInt16);
  /** Standard payload format: Real, unsigned 32-bit integer. */
  public static final PayloadFormat UINT32      = new PayloadFormat(DataType.UInt32);
  /** Standard payload format: Real, unsigned 64-bit integer. */
  public static final PayloadFormat UINT64      = new PayloadFormat(DataType.UInt64);

  /** Gets a free-form description of the packet. Note that the content and structure of this
   *  string is implementation dependant and may change at any time.
   *  @return A free-form string describing the packet.
   */
  @Override
  String toString ();

  /** Checks for header equality with another VRTPacket. The headers of two VRT packets are
   *  considered equal if the content of the payloads is bit-for-bit identical. The contents of the
   *  payload and trailer need not be equal for this method to return true. However, any two packets
   *  for which {@link #equals(Object)} returns true, must have this method return true as well.
   *  @param p The other packet (can be null).
   *  @return true if they are identical, false otherwise. (This will always returns false if the
   *          packet is null.)
   */
  boolean headerEquals (VRTPacket p);
  
  /** Checks for payload equality with another VRTPacket. The payloads of two VRT packets are
   *  considered equal if the content of the payloads is bit-for-bit identical. The contents of the
   *  header and trailer need not be equal for this method to return true. However, any two packets
   *  for which {@link #equals(Object)} returns true, must have this method return true as well. This
   *  will always return false if the two packets have different payload lengths.
   *  @param p The other packet (can be null).
   *  @return true if they are identical, false otherwise. (This will always returns false if the
   *          packet is null.)
   */
  boolean payloadEquals (VRTPacket p);
  
  /** Checks for trailer equality with another VRTPacket. This method is identical to
   *  <tt>trailerEquals(p,true)</tt>. <b>In most cases, use of <tt>trailerEquals(p,false)</tt>
   *  is more appropriate for comparing trailers.</b>
   *  @param p The other packet (can be null).
   *  @return true if they are identical, false otherwise. (This will always returns false if the
   *          packet is null.)
   */
  boolean trailerEquals (VRTPacket p);

  /** Checks for trailer equality with another VRTPacket. This method allows two different
   *  interpretations of trailer equality:
   *  <ul>
   *    <li><b>Exact:</b> With "exact" equality, the trailers of two VRT packets are considered
   *      equal if the content of the trailers is either bit-for-bit identical, or if <u>both</u>
   *      packets omit the trailers.
   *    </li>
   *    <li><b>Equal:</b> With "equal" equality, the trailers of two VRT packets are considered
   *      equal if the content of the trailers conveys the same meaning. Conveying the same meaning
   *      means that the enable bits in the trailer are checked first. Any values not enabled, are
   *      ignored when comparing. In this form, a packet with no trailer is considered equal to a
   *      packet with a trailer that has nothing enabled.
   *    </li>
   *  </ul>
   *  Regardless of the comparison used, any two packets for which {@link #equals(Object)} returns
   *  true, must have this method return true as well. This method will always return true if
   *  comparing two context packets since they do not contain trailers.
   *  @param p     The other packet (can be null).
   *  @param exact true to use the <b>Exact</b> comparison, false to use the <b>Equal</b> comparison.
   *  @return true if they are identical, false otherwise. (This will always returns false if the
   *          packet is null.)
   */
  boolean trailerEquals (VRTPacket p, boolean exact);
  
  /** Checks for equality with an unknown object. Two VRT packets are considered equal if the
   *  content of the packets is bit-for-bit identical. The actual implementation classes are not
   *  considered relevant for checking equality.
   *  @param o The unknown object (can be null).
   *  @return true if they are identical, false otherwise. (This will always returns false if the
   *          object is null.)
   */
  @Override
  boolean equals (Object o);
  
  /** Computes the hash code for a VRT packet. For performance reasons, the hash code is based
   *  on up to 32 bytes at the start of the packet; since this typically includes timecode, there is
   *  usually a lot of variation within these 32 bytes (which is good). Specifically, the following
   *  algorithm is mandatory:
   *  <pre>
   *    public final int hashCode () {
   *      int    hash = 0;
   *      int    len  = Math.min(getPacketLength(), 32);
   *      byte[] buf  = new byte[len];
   *      
   *      readPacket(buf, 0, 0, len);
   *      for (int i = 0; i &lt; len; i++) {
   *        hash ^= (buf[i] &lt;&lt; i);
   *      }
   *      return hash;
   *    }
   *  </pre>
   *  @return The hash code for the object.
   */
  @Override
  int hashCode ();
  
  /** Checks to see if the packet is valid. This checks the overall structure of the packet (i.e.
   *  length is correct, reserved bits set to 0, etc.). It does not check the content of the packet
   *  or the validity of the data in the packet (the state and event indicators [see
   *  {@link HasStateEventBits}] are provided for that purpose).
   *  @return true if it is valid, false otherwise.
   */
  boolean isPacketValid ();
  
  /** Checks to see if the packet is valid. This method adds an extra length check onto the basic
   *  checks performed by {@link #isPacketValid()}. The most common use for this method is to
   *  verify that a packet read from a socket matches the length expected.
   *  @param length Verifies that the length of the packet matches this value.
   *  @return true if it is valid, false otherwise.
   */
  boolean isPacketValid (int length);

  /** Checks to see if the packet is valid. This is identical to {@link #isPacketValid()} except
   *  that a description of the error is returned.
   *  @param strict Should strict checking be done, or just enough to verify that things are OK for
   *                reading. No packet failing strict checking should ever be sent out, but it may
   *                be useful to disable strict checking when reading in a packet.
   *  @return null if it is valid, a description of the error otherwise.
   */
  String getPacketValid (boolean strict);

  /** Checks to see if the packet is valid. This is identical to {@link #isPacketValid(int)}
   *  except that a description of the error is returned.
   *  @param strict Should strict checking be done, or just enough to verify that things are OK for
   *                reading. No packet failing strict checking should ever be sent out, but it may
   *                be useful to disable strict checking when reading in a packet.
   *  @param length Verifies that the length of the packet matches this value.
   *  @return null if it is valid, a description of the error otherwise.
   */
  String getPacketValid (boolean strict, int length);

  /** Creates a copy of this packet that is independent of the original. The copy will not be
   *  read only and will not be "direct" (i.e. will not share data with the original). This
   *  method's behavior is undefined if the packet is invalid and may throw an exception in that
   *  event.
   *  @return A copy of this packet.
   */
  VRTPacket copy ();

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Context Change
  //////////////////////////////////////////////////////////////////////////////////////////////////
  /** Prepares a packet for resend resetting any non-persistent values and clearing the context
   *  change flags. At a minimum this will update the time stamp. This is implemented as follows:
   *  <pre>
   *    public void resetForResend (TimeStamp t) {
   *      setTimeStamp(t);
   *      return false;
   *    }
   *  </pre>
   *  @param t The new time stamp.
   *  @return true if non-persistent values and the change flags have been reset, false if only the
   *               time stamp has been updated.
   *  @see ContextPacket#resetForResend(TimeStamp)
   */
  boolean resetForResend (TimeStamp t);

  /** Indicates if any fields have changes since the last packet in this stream. Packet streams
   *  that do not support this concept, should always return true indicating that the recipient must
   *  read the packet to determine if there was a change. An IF Data stream will always return true.
   *  @return true if <u>anything</u> in the packet may have changed since the last context
   *          packet, false if <u>nothing</u> has changed since the last context packet.
   */
  boolean isChangePacket ();
  
  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Get
  //////////////////////////////////////////////////////////////////////////////////////////////////
  /** <b>Internal use only:</b> Gets the packet stream code. The stream code is a 64-bit value
   *  equal to:
   *  <tt>
   *    0xt0000000ssssssss   where t = is the numeric value corresponding to the packet type
   *                               s = the stream identifier or 0x00000000 if not specified
   *  </tt>
   *   This is used as the hashtable key when doing packet sequence accounting since the counters
   *  are unique to the stream and type.
   *  @return The packet stream code.
   */
  long getStreamCode ();

  /** Gets the packet type. */
  PacketType getPacketType ();
  
  /** Context packets only: Gets the Time Stamp Mode (TSM). This indicates if a context packet
   *  is being used to convey timing of context events related to the signal with fine or coarse
   *  resolution:
   *  <pre>
   *    true  = Time stamp fields indicate *precise* timing of context changes (to the
   *            resolution of the system)
   *    false = Time stamp fields indicate *general* timing of the context changes (i.e.
   *            the change occurred within the time interval covered by this packet)
   *    null  = <i>Error: not a context packet</i>
   *  </pre>
   *  @return true or false as indicated above or null if this is a data packet.
   */
  Boolean isTimeStampMode ();

  /** Gets the time stamp of the packet.
   *  @return The time stamp of the packet (never null).
   */
  TimeStamp getTimeStamp ();
  
  /** Gets the packet count. This is the modulo-16 count of all packets on this stream.
   *  @return The packet count (0..15).
   */
  int getPacketCount ();
  
  /** Gets the packet length in bytes. The length of the header will be a value <b>up to</b>
   *  {@link #MAX_PACKET_LENGTH} (see {@link #MAX_PACKET_LENGTH} for explanation as to why
   *  actual packet lengths used are usually a lot less than this number). The packet
   *  length will always be a multiple of 4-bytes.
   *  @return The total length of the packet in bytes.
   */
  int getPacketLength ();
  
  /** Gets the payload length in bytes. The payload length is a derived value computed as:
   *  <pre>
   *    payloadLength = {@link #getPacketLength()} - {@link #getHeaderLength()} - {@link #getTrailerLength()}
   *  </pre>
   *  The payload length will always be a multiple of 4-bytes.
   *  @return The length of the payload in bytes.
   */
  int getPayloadLength ();
  
  /** Gets the number of pad bits inserted following the data in the payload such that the
   *  payload section is always a multiple of 4-bytes in length. The number of pad bits is stored
   *  in five of the (formerly-reserved) bits in the ClassID block in the header as part of the
   *  suggested VITA-49.0 modifications (i.e. VITA-49.0b), but could be specified elsewhere for
   *  some user-defined class types. (The default implementation exclusively follows VITA-49.0b.)
   *  <br>
   *  Note that reporting the number of pad bits is optional in situations where the number of pad
   *  bits is less than the length of a single sample (this may occur when using the link-efficient
   *  packing mode with a data format that does not evenly divide into a 32-/64-bit length). In this
   *  situation, the number of pad bits is implicit and can be determined after consideration of
   *  the data payload format and payload length (see {@link DataPacket#getDataLength()}). This
   *  usage matches the VITA-49.0 specification.
   *  @return The number of pad bits *reported* for a data packet. This will return 0 for context
   *          packets and data packets that do not report the number of pad bits.
   *  @see #getPayloadLength()
   *  @see DataPacket#getDataLength()
   */
  int getPadBitCount ();
  
  /** Gets the header length in bytes (does not include trailer). The length of the header will be
   *  a value <b>up to</b> {@link #MAX_HEADER_LENGTH}. The header length will always be a multiple
   *  of 4-bytes.
   *  @return The length of the header in bytes.
   */
  int getHeaderLength ();
  
  /** Gets the trailer length in bytes. Note that the presence of the trailer is an all-or-nothing,
   *  so this method return either {@link #MAX_TRAILER_LENGTH} or <tt>0</tt>.
   *  @return The length of the trailer in bytes.
   */
  int getTrailerLength ();
  
  /** Gets the 32-bit packet stream identifier as a string.
   *  @return The stream identifier or null if not specified.
   */
  String getStreamID ();
  
  /** Gets the 32-bit packet stream identifier.
   *  @return The stream identifier or null if not specified.
   */
  Integer getStreamIdentifier ();
  
  /** Gets the class identifier as a string. This will return the class identifier in the form
   *  <tt>CC-CC-CC:IIII.PPPP</tt> where <tt>CC-CC-CC</tt> is the standard hexadecimal representation
   *  of the OUI (see IEEE standard 802-2001), IIII is the hexadecimal representation of the ICC
   *  and PPPP is the hexadecimal representation of the PCC.
   *  @return The packet class identifier as a string or null if not specified.
   */
  String getClassID ();
  
  /** Gets the packet class identifier. Note that the the packet class identifier is the union
   *  of the three distinct parts of the class identifier: OUI, ICC, and PCC. This method allows for
   *  easy comparison of the class identifier as a whole. <i>Note that if this is null, the OUI,
   *  ICC, and PCC will all be null; if this is non-null, the OUI, ICC, and PCC will all be
   *  non-null.</i>
   *  @return The packet class identifier or null if not specified.
   *  @see #getClassIdentifierOUI()
   *  @see #getClassIdentifierICC()
   *  @see #getClassIdentifierPCC()
   */
  Long getClassIdentifier ();
  
  /** Gets the Organizational Unique Identifier (OUI) from the packet class identifier. The OUI
   *  is a 24-bit IEEE-assigned identifier for the organization that defined the packet class.
   *  @return The OUI or null if not specified.
   */
  Integer getClassIdentifierOUI ();
  
  /** Gets the Information Class Code (ICC) from the packet class identifier. This is a
   *  user-defined value specified by the OUI (see {@link #getClassIdentifierOUI()}) that defined
   *  this packet class.
   *  @return The ICC or null if not specified.
   */
  Short getClassIdentifierICC ();
  
  /** Gets the Packet Class Code (PCC) from the packet class identifier. This is a user-defined
   *  value specified by the OUI (see {@link #getClassIdentifierOUI()}) that defined this packet
   *  class.
   *  @return The ICC or null if not specified.
   */
  Short getClassIdentifierPCC ();
  
  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Set
  //////////////////////////////////////////////////////////////////////////////////////////////////
  /** <i>Optional functionality:</i> Sets the packet type.
   *  @param t The packet type.
   *  @throws UnsupportedOperationException If this method is not supported.
   */
  void setPacketType (PacketType t);
  
  /** <i>Optional functionality:</i> Context packets only: Sets the Time Stamp Mode (TSM).
   *  @param v The new value, see {@link #isTimeStampMode()} for details.
   *  @throws UnsupportedOperationException If this method is not supported or if the packet type
   *                                        doesn't support this setting.
   */
  void setTimeStampMode (boolean v);
  
  /** <i>Optional functionality:</i> Sets the time stamp for the packet.
   *  @param v The new time stamp.
   *  @throws UnsupportedOperationException If this method is not supported.
   */
  void setTimeStamp (TimeStamp v);
  
  /** <i>Optional functionality:</i> Gets the packet count. This is the modulo-16 count of all
   *  packets on this stream.
   *  @param v The packet count (0..15).
   *  @throws UnsupportedOperationException If this method is not supported.
   *  @throws IllegalArgumentException If the value passed in is invalid.
   */
  void setPacketCount (int v);
  
  /** <i>Optional functionality:</i> Sets the payload length in bytes. The packet length is
   *  recomputed and trailer is shifted as required. The payload length must be a multiple
   *  of 4-bytes.
   *  @param v The length of the payload in bytes.
   *  @throws UnsupportedOperationException If this method is not supported.
   *  @throws IllegalArgumentException If the value passed in is invalid.
   */
  void setPayloadLength (int v);
  
  /** <i>Optional functionality:</i> Sets the number of pad bits inserted following the data
   *  in the payload such that the payload section is always a multiple of 4-bytes in length (see
   *  {@link #getPadBitCount()} for a detailed discussion about the reporting of pad bits). <br>
   *  <br>
   *  This function will *not* alter the packet if the number of pad bits is implicit (or zero) when
   *  in strict VITA-49.0 (rather than VITA-49.0b) mode. <br>
   *  <br>
   *  Note: <b>It is not possible to report the number of pad bits if the ClassID is not set.</b>
   *  If this function is called with a non-zero value, the number of pad bits is not implicit and
   *  the ClassID is not set, an exception will be thrown. <br>
   *  <br>
   *  Assuming this method is supported, it is safe to call this method on a non-data packet with
   *  a value of zero. <br>
   *  <br>
   *  <b>In most cases, this method should not be called directly (except to clear it by setting a
   *  value of zero); rather this should be called by {@link DataPacket#setDataLength(int)}.</b>
   *  @param bits          The number of pad bits to report.
   *  @param bitsPerSample Number of bits per sample. This is used to determine if the number of pad
   *                       bits is implicit. Note the value of <tt>bitsPerSample</tt> is irrelevant
   *                       when <tt>bits</tt> is zero, therefore users can just call
   *                       <tt>setPadBitCount(0,0)</tt> to set the field to zero (where applicable).
   *  @see #setPayloadLength(int)
   *  @see DataPacket#setDataLength(int)
   *  @throws UnsupportedOperationException If this method is not supported.
   *  @throws IllegalArgumentException      If this method is called with a non-zero value for a
   *                                        non-data packet or if the number of bits is not implicit
   *                                        and the ClassID is not set.
   */
  void setPadBitCount (int bits, int bitsPerSample);

  /** <i>Optional functionality:</i> Sets the 32-bit packet stream identifier.
   *  @param v The stream identifier or null if not specified.
   *  @throws UnsupportedOperationException If this method is not supported.
   */
  void setStreamID (CharSequence v);
  
  /** <i>Optional functionality:</i> Sets the 32-bit packet stream identifier.
   *  @param v The stream identifier or null if not specified.
   *  @throws UnsupportedOperationException If this method is not supported.
   */
  void setStreamIdentifier (Integer v);
  
  /** <i>Optional functionality:</i> Sets the packet class identifier.
   *  @param v The packet class identifier or null if not specified.
   *  @throws UnsupportedOperationException If this method is not supported.
   *  @throws IllegalArgumentException If the value passed in is invalid.
   */
  void setClassID (CharSequence v);
  
  /** <i>Optional functionality:</i> Sets the packet class identifier. See
   *  {@link #getClassIdentifier()} for details.
   *  @param v The packet class identifier or null if not specified.
   *  @throws UnsupportedOperationException If this method is not supported.
   *  @throws IllegalArgumentException If the value passed in is invalid.
   */
  void setClassIdentifier (Long v);
  
  /** <i>Optional functionality:</i> Sets the packet class identifier. See
   *  {@link #getClassIdentifierOUI()}, {@link #getClassIdentifierICC()}, and
   *  {@link #getClassIdentifierPCC()} for details.
   *  @param oui The new OUI value.
   *  @param icc The new ICC value.
   *  @param pcc The new PCC value.
   *  @throws UnsupportedOperationException If this method is not supported.
   *  @throws IllegalArgumentException If the value passed in is invalid.
   */
  void setClassIdentifier (int oui, short icc, short pcc);
  
  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Read
  //////////////////////////////////////////////////////////////////////////////////////////////////
  
  /** <i>Optional functionality:</i> Reads the entire packet from an input stream. The state of the
   *  packet and the InputStream are indeterminate if an exception is thrown.
   *  @param in The input stream.
   *  @return The number of octets read in.
   *  @throws IOException                     On an I/O exception.
   *  @throws EOFException                    If an EOF is reached before the entire packet is read.
   *  @throws ArrayIndexOutOfBoundsException  If the length of the packet in the InputStream exceeds
   *                                          the local buffer capacity.
   *  @throws UnsupportedOperationException   If this method is not supported.
   */
  int readPacketFrom (InputStream in) throws IOException;

  /** Writes the entire packet to an output stream. The state of the OutputStream is indeterminate
   *  if an exception is thrown. The most basic implementation of this function is:
   *  <pre>
   *    public void writePacketTo (OutputStream out) throws IOException {
   *      out.write(getPacket());
   *    }
   *  </pre>
   *  Note that in most cases this will be implemented with a more efficient algorithm that does
   *  not require an extra buffer creation and data copy.
   *  @param out The output stream.
   *  @throws IOException On an I/O exception.
   */
  void writePacketTo (OutputStream out) throws IOException;

  /** Gets the content of the packet.
   *  @see #readPacket(byte[])
   */
  byte[] getPacket ();
  
  /** Reads the packet into the provided buffer.
   *  @param buffer The buffer to read into.
   *  @return The number of bytes read.
   *  @throws NullPointerException if the buffer is null.
   *  @throws ArrayIndexOutOfBoundsException If the length of the buffer is less than
   *          {@link #getPacketLength()}.
   */
  int readPacket (byte[] buffer);
  
  /** Reads the packet into the provided buffer.
   *  @param buffer The buffer to read into.
   *  @param boff   The byte offset into the buffer to start reading into.
   *  @param poff   The packet offset to start reading from.
   *  @param len    The maximum length to read in.
   *  @return The number of bytes read or -1 if the end of the payload has been reached.
   *  @throws NullPointerException if the buffer is null.
   *  @throws ArrayIndexOutOfBoundsException If the length of the buffer is less than len+boff.
   */
  int readPacket (byte[] buffer, int boff, int poff, int len);
  
  /** Gets the content of the packet payload.
   *  @see #readPayload(byte[])
   */
  byte[] getPayload ();
  
  /** Reads the packet payload into the provided buffer up to the length of the buffer given.
   *  @param buffer The buffer to read into.
   *  @return The number of bytes read.
   *  @throws NullPointerException if the buffer is null.
   */
  int readPayload (byte[] buffer);
  
  /** Reads the packet payload into the provided buffer.
   *  @param buffer The buffer to read into.
   *  @param boff   The byte offset into the buffer to start reading into.
   *  @param poff   The payload offset to start reading from.
   *  @param len    The maximum length to read in.
   *  @return The number of bytes read or -1 if the end of the payload has been reached.
   *  @throws NullPointerException if the buffer is null.
   *  @throws ArrayIndexOutOfBoundsException If the length of the buffer is less than len+boff.
   */
  int readPayload (byte[] buffer, int boff, int poff, int len);
  
  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Write
  //////////////////////////////////////////////////////////////////////////////////////////////////
  
  /** <i>Optional functionality:</i> Sets the content of the packet payload and updates the length
   *  of the payload.
   *  @param buffer The new payload.
   *  @throws NullPointerException if the buffer is null.
   *  @throws UnsupportedOperationException If this method is not supported
   *  @throws IllegalArgumentException If the new payload length is invalid.
   *  @see #setPayload(byte[],int,int)
   */
  void setPayload (byte[] buffer);
  
  /** <i>Optional functionality:</i> Sets the content of the packet payload and updates the length
   *  of the payload.
   *  @param buffer The new payload.
   *  @param offset The offset into buffer to start reading from.
   *  @param length The number of bytes to copy from the buffer into the payload.
   *  @throws NullPointerException If the buffer is null.
   *  @throws ArrayIndexOutOfBoundsException If the length of the buffer is less than length+offset.
   *  @throws UnsupportedOperationException If this method is not supported
   *  @throws IllegalArgumentException If the new payload length is invalid or if the offset is
   *                                   invalid.
   */
  void setPayload (byte[] buffer, int offset, int length);
  
  /** <i>Optional functionality:</i> Writes to the packet payload, but does NOT update the length
   *  of the payload.
   *  @param buffer The buffer to with the new data.
   *  @param boff   The byte offset into the buffer to from.
   *  @param poff   The payload offset to start writing to.
   *  @param len    The length to write.
   *  @throws NullPointerException If the buffer is null.
   *  @throws ArrayIndexOutOfBoundsException If the length of the buffer is less than len+boff or
   *                               if there is insufficient room to write to.
   *  @throws UnsupportedOperationException If this method is not supported.
   */
  void writePayload (byte[] buffer, int boff, int poff, int len);

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Enums
  //////////////////////////////////////////////////////////////////////////////////////////////////
  /** Identifies a VRT Packet type. */
  public static enum PacketType {
    /** A standard data packet with no stream identifier.     */    UnidentifiedData(0),
    /** A standard data packet (with stream identifier).      */    Data(1),
    /** An extension data packet with no stream identifier.   */    UnidentifiedExtData(2),
    /** An extension data packet (with stream identifier).    */    ExtData(3),
    /** A standard context packet (with stream identifier).   */    Context(4),
    /** An extension context packet (with stream identifier). */    ExtContext(5),
    /** <b>Do Not Use:</b> Reserved for future use.           */    reserved6(-6),
    /** <b>Do Not Use:</b> Reserved for future use.           */    reserved7(-7),
    /** <b>Do Not Use:</b> Reserved for future use.           */    reserved8(-8),
    /** <b>Do Not Use:</b> Reserved for future use.           */    reserved9(-9),
    /** <b>Do Not Use:</b> Reserved for future use.           */    reserved10(-10),
    /** <b>Do Not Use:</b> Reserved for future use.           */    reserved11(-11),
    /** <b>Do Not Use:</b> Reserved for future use.           */    reserved12(-12),
    /** <b>Do Not Use:</b> Reserved for future use.           */    reserved13(-13),
    /** <b>Do Not Use:</b> Reserved for future use.           */    reserved14(-14),
    /** <b>Do Not Use:</b> Reserved for future use.           */    reserved15(-15);

    /** Single static instance to eliminate array creation/initialization on every use of {@link #values()}. */
    public static final PacketType[] VALUES = values();

    private final boolean supported;
    
    private PacketType (int code) {
      this.supported = (code >= 0);
      if (code < 0) code = -code;
      if (code != ordinal()) throw new AssertionError("Enum for "+getClass()+" is out of alignment");
    }
    /** Does this packet type have a stream identifier associated with it? */
    public boolean hasStreamIdentifier () {
      return (this != UnidentifiedData) && (this != UnidentifiedExtData);
    }
    /** Is this a data packet? */
    public boolean isData () { return (ordinal() <= 3); }
    /** Is this type supported? */
    public boolean isSupported () { return supported; }
  }
  
  /** Available the real/complex types for data items. */
  public static enum RealComplexType {
    /** Real samples.                                                   */    Real(0),
    /** Complex cartesian samples (in-phase and quadrature components). */    ComplexCartesian(1),
    /** Complex polar samples (amplitude and phase components).         */    ComplexPolar(2),
    /** <b>Do Not Use:</b> Reserved for future use.                     */    reserved3(-3);

    /** Single static instance to eliminate array creation/initialization on every use of {@link #values()}. */
    public static final RealComplexType[] VALUES = values();

    private final boolean supported;
    
    private RealComplexType (int code) {
      this.supported = (code >= 0);
      if (code < 0) code = -code;
      if (code != ordinal()) throw new AssertionError("Enum for "+getClass()+" is out of alignment");
    }
    /** Is this type supported? */
    public boolean isSupported () { return supported; }
  }
  
  /** Available numeric formats for data items.*/
  public static enum DataItemFormat {
    /** Signed fixed-point.                         */    SignedInt(0),
    /** Signed VRT, 1-bit exponent.                 */    SignedVRT1(1,1),
    /** Signed VRT, 2-bit exponent.                 */    SignedVRT2(2,2),
    /** Signed VRT, 3-bit exponent.                 */    SignedVRT3(3,3),
    /** Signed VRT, 4-bit exponent.                 */    SignedVRT4(4,4),
    /** Signed VRT, 5-bit exponent.                 */    SignedVRT5(5,5),
    /** Signed VRT, 6-bit exponent.                 */    SignedVRT6(6,6),
    /** <b>Do Not Use:</b> Reserved for future use. */    reserved7(-7),
    /** <b>Do Not Use:</b> Reserved for future use. */    reserved8(-8),
    /** <b>Do Not Use:</b> Reserved for future use. */    reserved9(-9),
    /** <b>Do Not Use:</b> Reserved for future use. */    reserved10(-10),
    /** <b>Do Not Use:</b> Reserved for future use. */    reserved11(-11),
    /** <b>Do Not Use:</b> Reserved for future use. */    reserved12(-12),
    /** <b>Do Not Use:</b> Reserved for future use. */    reserved13(-13),
    /** 32-bit IEEE-754 floating-point.             */    Float(14),
    /** 64-bit IEEE-754 floating-point.             */    Double(15),
    /** Unsigned fixed-point.                       */    UnsignedInt(16),
    /** Unsigned VRT, 1-bit exponent.               */    UnsignedVRT1(17,1),
    /** Unsigned VRT, 2-bit exponent.               */    UnsignedVRT2(18,2),
    /** Unsigned VRT, 3-bit exponent.               */    UnsignedVRT3(19,3),
    /** Unsigned VRT, 4-bit exponent.               */    UnsignedVRT4(20,4),
    /** Unsigned VRT, 5-bit exponent.               */    UnsignedVRT5(21,5),
    /** Unsigned VRT, 6-bit exponent.               */    UnsignedVRT6(22,6),
    /** <b>Do Not Use:</b> Reserved for future use. */    reserved23(-23),
    /** <b>Do Not Use:</b> Reserved for future use. */    reserved24(-24),
    /** <b>Do Not Use:</b> Reserved for future use. */    reserved25(-25),
    /** <b>Do Not Use:</b> Reserved for future use. */    reserved26(-26),
    /** <b>Do Not Use:</b> Reserved for future use. */    reserved27(-27),
    /** <b>Do Not Use:</b> Reserved for future use. */    reserved28(-28),
    /** <b>Do Not Use:</b> Reserved for future use. */    reserved29(-29),
    /** <b>Do Not Use:</b> Reserved for future use. */    reserved30(-30),
    /** <b>Do Not Use:</b> Reserved for future use. */    reserved31(-31);

    /** Single static instance to eliminate array creation/initialization on every use of {@link #values()}. */
    public static final DataItemFormat[] VALUES = values();

    private final boolean supported;
    private final int     exponentBits;
    
    private DataItemFormat (int code) {
      this(code,-1);
    }
    private DataItemFormat (int code, int exponentBits) {
      this.supported    = (code >= 0);
      this.exponentBits = exponentBits;
      if (code < 0) code = -code;
      if (code != ordinal()) throw new AssertionError("Enum for "+getClass()+" is out of alignment");
    }
    /** Is this type supported? */
    public boolean isSupported () { return supported; }
    /** Is this a signed number? */
    public boolean isSigned () { return ordinal() <= 15; }
    /** Gets the number of exponent bits for a VRT number.
     *  @return The number of bits or -1 if not a VRT number.
     */
    public int getExponentBits () { return exponentBits; }
  }

  /** Data types supported by a {@link StandardDataPacket}. */
  public static enum DataType {
    /** <b>Do Not Use:</b> Reserved for future use. */  reserved0(-1),
    /**  4-bit signed integer.                      */  Int4(  0x00FFFFFA00010000L),
    /**  8-bit signed integer.                      */  Int8(  0x00FFFFFA00020000L),
    /** 16-bit signed integer.                      */  Int16( 0x00FFFFFA00030000L),
    /** 32-bit signed integer.                      */  Int32( 0x00FFFFFA00040000L),
    /** 64-bit signed integer.                      */  Int64( 0x00FFFFFA00050000L),
    /** 32-bit single-precision floating-point.     */  Float( 0x00FFFFFA00060000L),
    /** 64-bit double-precision floating-point.     */  Double(0x00FFFFFA00070000L),
    /**  1-bit unsigned integer ("packed bits").    */  UInt1( 0x00FFFFFA00080000L),
    /**  4-bit unsigned integer.                    */  UInt4( 0x00FFFFFA00090000L),
    /**  8-bit unsigned integer.                    */  UInt8( 0x00FFFFFA000A0000L),
    /** 16-bit unsigned integer.                    */  UInt16(0x00FFFFFA000B0000L),
    /** 32-bit unsigned integer.                    */  UInt32(0x00FFFFFA000C0000L),
    /** 64-bit unsigned integer.                    */  UInt64(0x00FFFFFA000D0000L),
    /** <b>Do Not Use:</b> Reserved for future use. */  reservedE(-1),
    /** <b>Do Not Use:</b> Reserved for future use. */  reservedF(-1);

    /** Single static instance to eliminate array creation/initialization on every use of {@link #values()}. */
    public static final DataType[] VALUES = values();

    private final long bits;
    private DataType (long bits) {
      this.bits = bits;
    }

    /** <i>Utility method:</i> Indicates if the data type is signed.
        @return <tt>true</tt> if the data format is signed, <tt>false</tt> if it is unsigned.
     */
    public boolean isSigned () { return (bits <= 0x00FFFFFA00070000L); }

    /** Is this type supported? */
    public boolean isSupported () { return (bits != -1); }

    /** <b>Internal use only:</b> Gets the associated bits. */
    public long getBits () { return bits; }
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // NESTED CLASSES
  //////////////////////////////////////////////////////////////////////////////////////////////////
  
  /** Specifies the payload format. <br>
   *  <br>
   *  The following implementation is used for the {@link HasFields} methods:
   *  <pre>
   *     ID | Name                 | Type
   *    ----+----------------------+---------------
   *     0  | ProcessingEfficient  | boolean
   *     1  | RealComplexType      | RealComplexType
   *     2  | DataItemFormat       | DataItemFormat
   *     3  | Repeating            | boolean
   *     4  | EventTagSize         | int
   *     5  | ChannelTagSize       | int
   *     6  | ItemPackingFieldSize | int
   *     7  | DataItemSize         | int
   *     8  | RepeatCount          | int
   *     9  | VectorSize           | int
   *     10 | DataType             | DataType
   *    ----+----------------------+---------------
   *  </pre>
   */
  public static class PayloadFormat implements HasFields {
    private int hi; // high-order bits
    private int lo; // low-order bits
    
    /** Creates new instance. */
    public PayloadFormat () { }

    /** Creates new instance. This is identical to
     *  <tt>new PayloadFormat(true,RealComplexType.Real,format,false,0,0,size,size,1,1)</tt>.
     *  @param format        The data format used.
     *  @param size          The data item size.
     */
    public PayloadFormat (DataItemFormat format, int size) {
      this(true,RealComplexType.Real,format,false,0,0,size,size,1,1);
    }

    /** Creates new instance. This is identical to
     *  <tt>new PayloadFormat(true,type,format,false,0,0,size,size,1,1)</tt>.
     *  @param type          The real/complex type used.
     *  @param format        The data format used.
     *  @param size          The data item size.
     */
    public PayloadFormat (RealComplexType type, DataItemFormat format, int size) {
      this(true,type,format,false,0,0,size,size,1,1);
    }

    /** Creates new instance. This is identical to
     *  <tt>new PayloadFormat(dataType,RealComplexType.Real)</tt>.
     *  @param dataType     The data format used.
     */
    public PayloadFormat (DataType dataType) {
      this(dataType,RealComplexType.Real);
    }

    /** Creates new instance.
     *  @param dataType     The data format used.
     *  @param realComplex  The real/complex type used.
     */
    @SuppressWarnings("OverridableMethodCallInConstructor")
    public PayloadFormat (DataType dataType, RealComplexType realComplex) {
      setRealComplexType(realComplex);
      setDataType(dataType);
    }

    /** Creates new instance.
     *  @param procEfficient true if Processing Efficient or false if Link Efficient.
     *  @param type          The real/complex type used.
     *  @param format        The data format used.
     *  @param repeating     Indicates if the sample-component repeat is used.
     *  @param eventTagSize  the event tag size.
     *  @param chanTagSize   the channel tag size.
     *  @param fieldSize     The item packing field size.
     *  @param itemSize      The data item size.
     *  @param repeatCount   The repeat count.
     *  @param vectorSize    The vector size.
     */
    @SuppressWarnings("OverridableMethodCallInConstructor")
    public PayloadFormat (boolean procEfficient, RealComplexType type, DataItemFormat format,
                          boolean repeating, int eventTagSize, int chanTagSize, int fieldSize,
                          int itemSize, int repeatCount, int vectorSize) {
      setProcessingEfficient(procEfficient);
      setRealComplexType(type);
      setDataItemFormat(format);
      setRepeating(repeating);
      setEventTagSize(eventTagSize);
      setChannelTagSize(chanTagSize);
      setItemPackingFieldSize(fieldSize);
      setDataItemSize(itemSize);
      setRepeatCount(repeatCount);
      setVectorSize(vectorSize);
      
      // Validate entries
      String valid = getValid();
      if (valid != null) {
        throw new IllegalArgumentException(valid+" in "+toString());
      }
    }
    
    /** <i>Internal Use Only:</i> Creates new instance. */
    @SuppressWarnings("OverridableMethodCallInConstructor")
    PayloadFormat (long bits) {
      setBits(bits);
    }

    @Override
    public String toString () {
      return "ProcessingEfficient="+isProcessingEfficient()
           +" RealComplexType="+getRealComplexType()
           +" DataItemFormat="+getDataItemFormat()
           +" Repeating="+isRepeating()
           +" EventTagSize="+getEventTagSize()
           +" ChannelTagSize="+getChannelTagSize()
           +" ItemPackingFieldSize="+getItemPackingFieldSize()
           +" DataItemSize="+getDataItemSize()
           +" RepeatCount="+getRepeatCount()
           +" VectorSize="+getVectorSize()
           +" DataType="+getDataType();
    }
    
    @Override
    public boolean equals (Object o) {
      if (o == this) return true;
      if (!(o instanceof PayloadFormat)) return false;
      PayloadFormat pf = (PayloadFormat)o;
      return (pf.hi == hi) && (pf.lo == lo);
    }
    
    /** Gets the hash code for the object. The hash code is specifically defined
     *  to be equal to:
     *  <pre>
     *    public final int hashCode () {
     *      return Long.valueOf(getBits()).hashCode();
     *    }
     *  </pre>
     */
    @Override
    public final int hashCode () {
      // This is the same algorithm used internal to Long.hashCode()
      return (hi ^ lo);
    }
    
    /** Checks to see if the payload format is valid.
     *  @return true if it is valid, false otherwise.
     */
    public boolean isValid () {
      return (getValid() == null);
    }

    /** Checks to see if the payload format is valid. This is identical to
     *  {@link #isValid()} except that a description of the error is returned.
     *  @return null if it is valid, a description of the error otherwise.
     */
    public String getValid () {
      int            fSize  = getItemPackingFieldSize();
      int            dSize  = getDataItemSize();
      int            eSize  = getEventTagSize();
      int            cSize  = getChannelTagSize();
      DataItemFormat format = getDataItemFormat();
      
      if (fSize < (dSize+eSize+cSize)) {
        return "Given ItemPackingFieldSize is incompatible with DataItemSize, "
             + "EventTagSize, and ChannelTagSize.";
      }
      
      switch (format) {
        case SignedVRT1:   if (dSize <   2) return "Invalid use of DataItemFormat=SignedVRT1 with DataItemSize<2"; break;
        case SignedVRT2:   if (dSize <   3) return "Invalid use of DataItemFormat=SignedVRT2 with DataItemSize<3"; break;
        case SignedVRT3:   if (dSize <   4) return "Invalid use of DataItemFormat=SignedVRT3 with DataItemSize<4"; break;
        case SignedVRT4:   if (dSize <   5) return "Invalid use of DataItemFormat=SignedVRT4 with DataItemSize<5"; break;
        case SignedVRT5:   if (dSize <   6) return "Invalid use of DataItemFormat=SignedVRT5 with DataItemSize<6"; break;
        case SignedVRT6:   if (dSize <   7) return "Invalid use of DataItemFormat=SignedVRT6 with DataItemSize<7"; break;
        case UnsignedVRT1: if (dSize <   2) return "Invalid use of DataItemFormat=UnsignedVRT1 with DataItemSize<2"; break;
        case UnsignedVRT2: if (dSize <   3) return "Invalid use of DataItemFormat=UnsignedVRT2 with DataItemSize<3"; break;
        case UnsignedVRT3: if (dSize <   4) return "Invalid use of DataItemFormat=UnsignedVRT3 with DataItemSize<4"; break;
        case UnsignedVRT4: if (dSize <   5) return "Invalid use of DataItemFormat=UnsignedVRT4 with DataItemSize<5"; break;
        case UnsignedVRT5: if (dSize <   6) return "Invalid use of DataItemFormat=UnsignedVRT5 with DataItemSize<6"; break;
        case UnsignedVRT6: if (dSize <   7) return "Invalid use of DataItemFormat=UnsignedVRT6 with DataItemSize<7"; break;
        case Float:        if (dSize != 32) return "Invalid use of DataItemFormat=Float with DataItemSize!=32"; break;
        case Double:       if (dSize != 64) return "Invalid use of DataItemFormat=Double with DataItemSize!=64"; break;
        default: break;
      }
      
      return null;
    }
    
    /** <i>Internal Use Only:</i> Gets the bits. */
    public long getBits () {
      return (((long)hi) << 32) | (lo & 0xFFFFFFFFL);
    }
    
    /** <i>Internal Use Only:</i> Sets the bits. */
    public void setBits (long bits) {
      this.hi = ((int)(bits >> 32)) & 0xFFFF0FFF;
      this.lo = (int)bits;
    }
    
    /** Indicates if the processing efficient packing method used.
     *  @return true if Processing Efficient (byte-aligned) or false if Link Efficient (fewest bytes used).
     */
    public boolean isProcessingEfficient () {
      int mask = 0x80000000;
      return ((hi & mask) == 0);
    }
    
    /** Indicates if the processing efficient packing method used.
     *  @param val true if Processing Efficient (byte-aligned) or false if Link Efficient (fewest bytes used).
     */
    public void setProcessingEfficient (boolean val) {
      int mask = 0x80000000;
      int set  = (val)? 0 : mask;
      hi = set | (hi & ~mask);
    }
    
    /** Gets the real/complex type used.
     *  @return The type used.
     */
    public RealComplexType getRealComplexType () {
      int mask = 0x60000000;
      int val  = (hi & mask) >>> 29;
      return RealComplexType.VALUES[val];
    }
    
    /** Sets the real/complex type used.
     *  @param val The type used.
     *  @throws IllegalArgumentException If the specified value is unsupported.
     */
    public void setRealComplexType (RealComplexType val) {
      if (!val.isSupported()) {
        throw new IllegalArgumentException("Type "+val+" is not supported in "+val.getClass());
      }
      int mask = 0x60000000;
      int set  = val.ordinal() << 29;
      hi = set | (hi & ~mask);
    }

    /** <i>Utility method:</i> If this payload format matches one of the pre-defined data types,
     *  and is stored without any "extra bits", return that type. Although technically "unsigned"
     *  this method will recognize any 1-bit integer data as {@link DataType#UInt1}.
     *  @return The data type or null if not a pre-defined type.
     */
    public DataType getDataType () {
      int size = getDataItemSize();
      if (getItemPackingFieldSize() != size) return null; // ERROR
      
      switch (getDataItemFormat()) {
        case SignedInt:
          switch (size) {
            case  1:  return DataType.UInt1; // allow packed bits as signed or unsigned
            case  4:  return DataType.Int4;
            case  8:  return DataType.Int8;
            case 16:  return DataType.Int16;
            case 32:  return DataType.Int32;
            case 64:  return DataType.Int64;
            default:  return null; // ERROR
          }
        case UnsignedInt:
          switch (size) {
            case  1:  return DataType.UInt1; // allow packed bits as signed or unsigned
            case  4:  return DataType.UInt4;
            case  8:  return DataType.UInt8;
            case 16:  return DataType.UInt16;
            case 32:  return DataType.UInt32;
            case 64:  return DataType.UInt64;
            default:  return null; // ERROR
          }
        case Float:   return DataType.Float;
        case Double:  return DataType.Double;
        default:      return null; // ERROR
      }
    }

    /** <i>Utility method:</i> Sets the payload format to match one of the pre-defined data types.
     *  @param type The data type.
     *  @throws NullPointerException if type is null.
     */
    public void setDataType (DataType type) {
      if (type == null) throw new NullPointerException("Type is null");

      int size;
      switch (type) {
        case Int4:       size =  4; setDataItemFormat(DataItemFormat.SignedInt  ); break; // nibble
        case Int8:       size =  8; setDataItemFormat(DataItemFormat.SignedInt  ); break; // byte
        case Int16:      size = 16; setDataItemFormat(DataItemFormat.SignedInt  ); break; // short
        case Int32:      size = 32; setDataItemFormat(DataItemFormat.SignedInt  ); break; // int
        case Int64:      size = 64; setDataItemFormat(DataItemFormat.SignedInt  ); break; // long
        case UInt1:      size =  1; setDataItemFormat(DataItemFormat.UnsignedInt); break; // bit
        case UInt4:      size =  4; setDataItemFormat(DataItemFormat.UnsignedInt); break; // nibble
        case UInt8:      size =  8; setDataItemFormat(DataItemFormat.UnsignedInt); break; // byte
        case UInt16:     size = 16; setDataItemFormat(DataItemFormat.UnsignedInt); break; // short
        case UInt32:     size = 32; setDataItemFormat(DataItemFormat.UnsignedInt); break; // int
        case UInt64:     size = 64; setDataItemFormat(DataItemFormat.UnsignedInt); break; // long
        case Float:      size = 32; setDataItemFormat(DataItemFormat.Float      ); break; // int
        case Double:     size = 64; setDataItemFormat(DataItemFormat.Double     ); break; // long
        default:         throw new IllegalArgumentException("Unsupported data type "+type);
      }
      setItemPackingFieldSize(size);
      setDataItemSize(size);
    }

    /** <i>Utility method:</i> Indicates if the payload format is complex.
     *  @return <tt>true</tt> if the data format is complex, <tt>false</tt> if it is real.
     */
    public boolean isComplex () {
      return (getRealComplexType() != RealComplexType.Real);
    }

    /** <i>Utility method:</i> Indicates if the payload format is signed.
     *  @return <tt>true</tt> if the data format is signed, <tt>false</tt> if it is unsigned.
     */
    public boolean isSigned () {
      return (getDataItemFormat().ordinal() < 16);
    }

    /** Gets the data format used.
     *  @return The format used.
     */
    public DataItemFormat getDataItemFormat () {
      int mask = 0x1F000000;
      int val  = (hi & mask) >>> 24;
      return DataItemFormat.VALUES[val];
    }
    
    /** Sets the data format used.
     *  @param val The format used.
     *  @throws IllegalArgumentException If the specified value is unsupported.
     */
    public void setDataItemFormat (DataItemFormat val) {
      if (!val.isSupported()) {
        throw new IllegalArgumentException("Type "+val+" is not supported in "+val.getClass());
      }
      int mask = 0x1F000000;
      int set  = val.ordinal() << 24;
      hi = set | (hi & ~mask);
    }
    
    /** Indicates if the sample-component repeat is used.
     *  @return true if it is being used, false if not.
     */
    public boolean isRepeating () {
      int mask = 0x00800000;
      return ((hi & mask) != 0);
    }
    
    /** Indicates if the sample-component repeat is used.
     *  @param val true if it is being used, false if not.
     */
    public void setRepeating (boolean val) {
      int mask = 0x00800000;
      int set  = (val)? mask : 0;
      hi = set | (hi & ~mask);
    }
    
    /** Gets the event tag size.
     *  @return The event tag size (0..7).
     */
    public int getEventTagSize () {
      int mask = 0x00700000;
      int  val  = (hi & mask) >>> 20;
      return val;
    }
    
    /** Sets the event tag size.
     *  @param val The event tag size (0..7).
     *  @throws IllegalArgumentException If the specified value is not in the allowable range.
     */
    public void setEventTagSize (int val) {
      if ((val < 0) || (val > 7)) {
        throw new IllegalArgumentException("Value ("+val+") not in range of 0..7");
      }
      int mask = 0x00700000;
      int set  = val << 20;
      hi = set | (hi & ~mask);
    }
    
    /** Gets the channel tag size.
     *  @return The channel tag size (0..15).
     */
    public int getChannelTagSize () {
      int mask = 0x000F0000;
      int val  = (hi & mask) >>> 16;
      return val;
    }
    
    /** Sets the channel tag size.
     *  @param val The channel tag size (0..15).
     *  @throws IllegalArgumentException If the specified value is not in the allowable range.
     */
    public void setChannelTagSize (int val) {
      if ((val < 0) || (val > 15)) {
        throw new IllegalArgumentException("Value ("+val+") not in range of 0..15");
      }
      int mask = 0x000F0000;
      int set  = val << 16;
      hi = set | (hi & ~mask);
    }
    
    /** Gets the item packing field size.
     *  @return The field size (1..64).
     */
    public int getItemPackingFieldSize () {
      int mask = 0x00000FC0;
      int val  = (hi & mask) >>> 6;
      return val+1;
    }
    
    /** Sets the item packing field size.
     *  @param val The field size (1..64).
     *  @throws IllegalArgumentException If the specified value is not in the allowable range.
     */
    public void setItemPackingFieldSize (int val) {
      if ((val < 1) || (val > 64)) {
        throw new IllegalArgumentException("Value ("+val+") not in range of 1..64");
      }
      int mask = 0x00000FC0;
      int set  = (val-1) << 6;
      hi = set | (hi & ~mask);
    }
    
    /** Gets the data item size.
     *  @return The data item size (1..64).
     */
    public int getDataItemSize () {
      int mask = 0x0000003F;
      int val  = (hi & mask);
      return val+1;
    }
    
    /** Sets the data item size.
     *  @param val The data item size (1..64).
     *  @throws IllegalArgumentException If the specified value is not in the allowable range.
     */
    public void setDataItemSize (int val) {
      if ((val < 1) || (val > 64)) {
        throw new IllegalArgumentException("Value ("+val+") not in range of 1..64");
      }
      int mask = 0x0000003F;
      int set  = (val-1);
      hi = set | (hi & ~mask);
    }
    
    /** Gets the repeat count.
     *  @return The repeat count (1..65536).
     */
    public int getRepeatCount () {
      return (lo >>> 16)+1;
    }
    
    /** Sets the repeat count.
     *  @param val The repeat count (1..65536).
     *  @throws IllegalArgumentException If the specified value is not in the allowable range.
     */
    public void setRepeatCount (int val) {
      if ((val < 1) || (val > 65536)) {
        throw new IllegalArgumentException("Value ("+val+") not in range of 1..65536");
      }
      lo = ((val-1) << 16) | (lo & 0x0000FFFF);
    }
    
    /** Gets the vector size.
     *  @return The vector size (1..65536).
     */
    public int getVectorSize () {
      return (lo & 0x0000FFFF)+1;
    }
    
    /** Sets the vector size.
     *  @param val The vector size (1..65536).
     *  @throws IllegalArgumentException If the specified value is not in the allowable range.
     */
    public void setVectorSize (int val) {
      if ((val < 1) || (val > 65536)) {
        throw new IllegalArgumentException("Value ("+val+") not in range of 1..65536");
      }
      lo = (val-1) | (lo & 0xFFFF0000);
    }
    
    ////////////////////////////////////////////////////////////////////////////
    // Implement HasFields
    ////////////////////////////////////////////////////////////////////////////
    @Override
    public int getFieldCount () {
      return 11;
    }
    
    @Override
    public String getFieldName (int id) {
      switch (id) {
        case  0: return "ProcessingEfficient";
        case  1: return "RealComplexType";
        case  2: return "DataItemFormat";
        case  3: return "Repeating";
        case  4: return "EventTagSize";
        case  5: return "ChannelTagSize";
        case  6: return "ItemPackingFieldSize";
        case  7: return "DataItemSize";
        case  8: return "RepeatCount";
        case  9: return "VectorSize";
        case 10: return "DataType";
        default: throw new IllegalArgumentException("Invalid field #"+id+" in "+getClass());
      }
    }

    @Override
    public Class<?> getFieldType (int id) {
      switch (id) {
        case  0: return Boolean.TYPE;
        case  1: return RealComplexType.class;
        case  2: return DataItemFormat.class;
        case  3: return Boolean.TYPE;
        case  4: return Integer.TYPE;
        case  5: return Integer.TYPE;
        case  6: return Integer.TYPE;
        case  7: return Integer.TYPE;
        case  8: return Integer.TYPE;
        case  9: return Integer.TYPE;
        case 10: return DataType.class;
        default: throw new IllegalArgumentException("Invalid field #"+id+" in "+getClass());
      }
    }

    @Override
    public Object getField (int id) {
      switch (id) {
        case  0: return isProcessingEfficient();
        case  1: return getRealComplexType();
        case  2: return getDataItemFormat();
        case  3: return isRepeating();
        case  4: return getEventTagSize();
        case  5: return getChannelTagSize();
        case  6: return getItemPackingFieldSize();
        case  7: return getDataItemSize();
        case  8: return getRepeatCount();
        case  9: return getVectorSize();
        case 10: return getDataType();
        default: throw new IllegalArgumentException("Invalid field #"+id+" in "+getClass());
      }
    }

    @Override
    public void setField (int id, Object val) {
      switch (id) {
        case  0: setProcessingEfficient( Utilities.castValue(Boolean.TYPE,          val)); break;
        case  1: setRealComplexType(     Utilities.castValue(RealComplexType.class, val)); break;
        case  2: setDataItemFormat(      Utilities.castValue(DataItemFormat.class,  val)); break;
        case  3: setRepeating(           Utilities.castValue(Boolean.TYPE,          val)); break;
        case  4: setEventTagSize(        Utilities.castValue(Integer.class,         val)); break;
        case  5: setChannelTagSize(      Utilities.castValue(Integer.class,         val)); break;
        case  6: setItemPackingFieldSize(Utilities.castValue(Integer.class,         val)); break;
        case  7: setDataItemSize(        Utilities.castValue(Integer.class,         val)); break;
        case  8: setRepeatCount(         Utilities.castValue(Integer.class,         val)); break;
        case  9: setVectorSize(          Utilities.castValue(Integer.class,         val)); break;
        case 10: setDataType(            Utilities.castValue(DataType.class,        val)); break;
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
}

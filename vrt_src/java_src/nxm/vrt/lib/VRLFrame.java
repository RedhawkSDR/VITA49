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
import java.util.Iterator;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/** Defines a VRL frame type. The most frequently used implementations of interface this is
 *  {@link BasicVRLFrame}. <br>
 *  <br>
 *  Note that the numeric representation (byte order) used by all VRL frames is
 *  {@link ByteOrder#BIG_ENDIAN}.
 *
 *  @author         
 */
public interface VRLFrame extends Iterable<VRTPacket> {
  
  /** The length of the header in bytes.
   *  <pre>
   *    MAX_HEADER_LENGTH = {@value}
   *  </pre>
   */
  static final int HEADER_LENGTH = 8;
  
  /** The length of the trailer in bytes.
   *  <pre>
   *    MAX_TRAILER_LENGTH = {@value}
   *  </pre>
   */
  static final int TRAILER_LENGTH = 4;

  /** The maximum length of the frame in bytes (header + payload + trailer). Note that this
   *  is the absolute limit imposed by the VRL specification. The underlying transport protocols
   *  used (e.g. IP, UDP, TCP, etc.) have limits lower than this.
   *  <pre>
   *    MAX_FRAME_LENGTH = {@value}
   *  </pre>
   *  @see #getFrameLength()
   */
  static final int MAX_FRAME_LENGTH = 0x000FFFFF*4; // ~4MiB
  
  /** The minimum length of the frame in bytes (header + no payload + trailer). Note that this
   *  is the absolute limit imposed by the VRL specification and results in a near-useless frame
   *  with no payload packets.
   *  <pre>
   *    MIN_FRAME_LENGTH = HEADER_LENGTH + TRAILER_LENGTH = {@value}
   *  </pre>
   *  @see #getFrameLength()
   */
  static final int MIN_FRAME_LENGTH = HEADER_LENGTH + TRAILER_LENGTH;
  
  /** The maximum length of the payload in bytes. Note that the underlying transport protocols
   *  used (e.g. IP, UDP, TCP, etc.) impose limits lower than this.
   *  <pre>
   *    MAX_PAYLOAD_LENGTH = MAX_FRAME_LENGTH - HEADER_LENGTH - TRAILER_LENGTH = {@value}
   *  </pre>
   */
  static final int MAX_PAYLOAD_LENGTH = MAX_FRAME_LENGTH - HEADER_LENGTH - TRAILER_LENGTH;

  /** This is the 32-bit FWA (frame alignment word) marking the start of a VRLP frame.
   *  <pre>
   *    VRL_FAW = 0x56524C50 = { 'V', 'R', 'L', 'P' }
   *  </pre>
   */
  static final int VRL_FAW = 0x56524C50;

  /** This is the special code that can be used in place of the CRC32 in cases where
   *  a CRC is not considered unnecessary, and not included.
   *  <pre>
   *    NO_CRC = 0x56454E44 = { 'V', 'E', 'N', 'D' }
   *  </pre>
   */
  static final int NO_CRC = 0x56454E44;

  /** This is the 1st transmitted byte of the 32-bit FWA ({@link #VRL_FAW}). */
  static final byte VRL_FAW_0 = 0x56;
  /** This is the 2nd transmitted byte of the 32-bit FWA ({@link #VRL_FAW}). */
  static final byte VRL_FAW_1 = 0x52;
  /** This is the 3rd transmitted byte of the 32-bit FWA ({@link #VRL_FAW}). */
  static final byte VRL_FAW_2 = 0x4C;
  /** This is the 4th transmitted byte of the 32-bit FWA ({@link #VRL_FAW}). */
  static final byte VRL_FAW_3 = 0x50;

  /** This is the 1st transmitted byte of the 32-bit {@link #NO_CRC} code. */
  static final byte NO_CRC_0 = 0x56;
  /** This is the 2nd transmitted byte of the 32-bit {@link #NO_CRC} code. */
  static final byte NO_CRC_1 = 0x45;
  /** This is the 3rd transmitted byte of the 32-bit {@link #NO_CRC} code. */
  static final byte NO_CRC_2 = 0x4E;
  /** This is the 4th transmitted byte of the 32-bit {@link #NO_CRC} code. */
  static final byte NO_CRC_3 = 0x44;


  /** Gets a free-form description of the packet. Note that the content and structure of this
   *  string is implementation dependant and may change at any time.
   *  @return A free-form string describing the packet.
   */
  @Override
  String toString ();
  
  /** Checks for equality with an unknown object. Two VRL frames are considered equal if the
   *  content of the frames is bit-for-bit identical. The actual implementation classes are not
   *  considered relevent for checking equality.
   *  @param o The unknown object (can be null).
   *  @return true if they are identical, false otherwise. (This will always returns false if the
   *          object is null.)
   */
  @Override
  boolean equals (Object o);
  
  /** Computes the hash code for a VRT packet. For performance reasons, the hash code is based
   *  on up to 32 bytes at the start of the frame. Specifically, the following algorithm is mandatory:
   *  <pre>
   *    public final int hashCode () {
   *      int    hash = 0;
   *      int    len  = Math.min(getFrameLength(), 32);
   *      byte[] buf  = new byte[len];
   *      
   *      readFrame(buf, 0, 0, len);
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
  
  /** Checks to see if the frame is valid. This checks the overall structure of the frame and
   *  verifies the checksum (if specified). It also checks to see that the length of the frame as
   *  reported in the frame header is equal to the length of the header and trailer plus the length
   *  of each packet in the trailer. For the purposes of the packet length check, only the length
   *  of a packet is considered, no consideration is made to the validity of the packet.
   *  @return true if it is valid, false otherwise.
   */
  boolean isFrameValid ();
  
  /** Checks to see if the frame is valid. This method adds an extra length check onto the basic
   *  checks performed by {@link #isFrameValid()}. The most common use for this method is to
   *  verify that a packet read from a socket matches the length expected.
   *  @param length Verifies that the length of the packet matches this value.
   *  @return true if it is valid, false otherwise.
   */
  boolean isFrameValid (int length);

  /** Checks to see if the CRC for the frame is valid. This will always return true when the CRC
   *  is set to the special {@link #NO_CRC} value.
   *  @return true if it is valid, false otherwise.
   */
  boolean isCRCValid ();

  /** This will compute the CRC value for the packet and set it in the CRC field. All successful
   *  calls to one of the set methods on this packet will result in the CRC field being updated,
   *  but that usually means that the CRC was merely set to the special {@link #NO_CRC} value. This
   *  method must be called on the packet immediately before transmission (i.e. after the frame
   *  counter and frame length have been updated) if CRC protection is desired. <br>
   *  <br>
   *  Most transmission options (UDP, UDP Multicast, TCP, etc.) provide their own CRC protection,
   *  which renders the CRC field in the VRL frame redundant and just wastes extra computational
   *  time on both the sender and receiver. As such, this method should only be called when
   *  absolutely necessary and such a call should usually be the responsibility of the frame
   *  sending routines.
   */
  void updateCRC ();
  
  /** Creates a copy of this frame that is independent of the original. The copy will not be
   *  read only and will not be "direct" (i.e. will not share data with the original). This
   *  method's behavior is undefined if the packet is invalid and may throw an exception in that
   *  event.
   *  @return A copy of this packet.
   */
  VRLFrame copy ();

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Get / Set
  //////////////////////////////////////////////////////////////////////////////////////////////////

  /** Gets the number of packets contained in this frame. This will be equal to
   *  <tt>getVRTPackets().length</tt> though will typically be implemented in a much
   *  more efficient fashion.
   */
  int getPacketCount ();

  /** Gets an iterator over all of the VRT packets contained in the VRL frame. Unlike the
   *  {@link #getVRTPackets()} method the packets returned will still map into the underlying
   *  VRL frame, as such the packets are all read-only. This iterator will not support the
   *  <tt>remove()</tt> function and will have unpredictable behavior if the underlying frame
   *  is modified during iteration.
   *  @return An iterator that will iterate over all the packets in the frame.
   */
  @Override
  Iterator<VRTPacket> iterator ();

  /** Gets all of the VRT packets contained in the VRL frame. (Prior to Jan 2012 this did not
   *  attempt to resolve the specific type of VRT packet and just returned a basic VRT packet,
   *  since that time the classes in the output array match the classes returned from
   *  {@link #iterator()} excepting that the ones returned here are writable copies.)
   *  @return The packets.
   *  @throws UnsupportedOperationException If this method is not supported
   */
  VRTPacket[] getVRTPackets ();

  /** <i>Optional functionality:</i> Sets all of the VRT packets contained in the VRL frame.
   *  Strictly based on the maximum size of a VRT packet and the maximum size of a VRL frame, a
   *  minimum of 15 packets can be included. In normal use, only one or two tend to be included
   *  at a time.
   *  @param packets The packets.
   *  @throws UnsupportedOperationException If this method is not supported
   *  @throws IllegalArgumentException If the total length of all packets exceeds the maximum that
   *                                   can be included in a single frame or if any of the packets
   *                                   are invalid.
   */
  void setVRTPackets (VRTPacket ... packets);

  /** <i>Optional functionality:</i> Sets all of the VRT packets contained in the VRL frame. Of
   *  the packets given, this method will try to put as many as possible into the frame that will
   *  fit given the specified maximum frame length. If fewer fit, it will put fewer in.
   *  @param maxFrameLength The maximum frame length in bytes (inclusive of header and trailer).
   *  @param packets        The packets.
   *  @return The number of packets inserted into the frame (could be 0).
   *  @throws UnsupportedOperationException If this method is not supported
   *  @throws IllegalArgumentException If <tt>maxFrameLength</tt> is invalid or if any of the packets
   *                                   are invalid.
   */
  int setVRTPackets (int maxFrameLength, VRTPacket ... packets);

  /** Gets the value of the 12-bit frame counter.
   *  @return The value of the frame counter.
   */
  int getFrameCount ();

  /** <i>Optional functionality:</i> Sets the value of the 12-bit frame counter.
   *  @param count The frame count.
   *  @throws UnsupportedOperationException If this method is not supported
   *  @throws IllegalArgumentException If the count is less than 0 or greater than 0x00000FFF.
   */
  void setFrameCount (int count);
  
  /** Gets the total length of the frame in bytes including the header and trailer.
   *  @return The total length of the frame in bytes.
   */
  int getFrameLength ();

  /** <i>Optional functionality:</i> Sets the total length of the frame in bytes including the header and trailer.
   *  @param length The total length of the frame in bytes.
   *  @throws UnsupportedOperationException If this method is not supported
   *  @throws IllegalArgumentException If the length is less than 24 bytes (the minimum length for
   *                                   a VRL frame) and/or not a multiple of 4-bytes.
   */
  void setFrameLength (int length);

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Read / Write
  //////////////////////////////////////////////////////////////////////////////////////////////////
  
  /** <i>Optional functionality:</i> Reads the entire frame from an input stream. The state of the
   *  packet and the InputStream are indeterminate if an exception is thrown. <br>
   *  <br>
   *  This function supports the reading of an input stream that contains <u>either</u> a VRL frame
   *  or an un-framed VRT packet. If a VRT packet is found, a "virtual" VRL frame will be created
   *  for it that contains only that packet and with the frame count set to zero. The determination
   *  of VRL frame vs VRT packet will be done by looking at the first four octets. If the first four
   *  octets match the {@link #VRL_FAW} a VRL frame is assumed, otherwise a VRT packet is assumed.
   *  The return value can be checked to differentiate the two situations:
   *  <pre>
   *    VRTFrame f;
   *    InputStream in;
   *    ...
   *    int numRead = f.readFrameFrom(in);
   *    if (numRead == f.getFrameLength()) {
   *      // read in a VRL frame (f.getFrameCount() will return the
   *      // actual count, which could be 0)
   *    }
   *    else {
   *      // read in a VRT packet (f.getFrameCount() will return 0
   *      // since a "virtual" VRL frame is being used)
   *    }
   *  </pre>
   *  @param in The input stream.
   *  @return The number of octets read in.
   *  @throws IOException                     On an I/O exception.
   *  @throws EOFException                    If an EOF is reached before the entire frame is read.
   *  @throws ArrayIndexOutOfBoundsException  If the length of the frame in the InputStream exceeds
   *                                          the local buffer capacity.
   *  @throws UnsupportedOperationException   If this method is not supported.
   */
  int readFrameFrom (InputStream in) throws IOException;

  /** Writes the entire frame to an output stream. The state of the OutputStream is indeterminate
   *  if an exception is thrown. The most basic implementation of this function is:
   *  <pre>
   *    public void writeFrameTo (OutputStream out) throws IOException {
   *      out.write(getFrame());
   *    }
   *  </pre>
   *  Note that in most cases this will be implemented with a more efficient algorithm that does
   *  not require an extra buffer creation and data copy.
   *  @param out The output stream.
   *  @throws IOException On an I/O exception.
   */
  void writeFrameTo (OutputStream out) throws IOException;

  /** Gets the content of the frame.
   *  @see #readFrame(byte[])
   */
  byte[] getFrame ();
  
  /** Reads the frame into the provided buffer.
   *  @param buffer The buffer to read into.
   *  @return The number of bytes read.
   *  @throws NullPointerException if the buffer is null.
   *  @throws ArrayIndexOutOfBoundsException If the length of the buffer is less than
   *          {@link #getFrameLength()}.
   */
  int readFrame (byte[] buffer);
  
  /** Reads the frame into the provided buffer.
   *  @param buffer The buffer to read into.
   *  @param boff   The byte offset into the buffer to start reading into.
   *  @param poff   The packet offset to start reading from.
   *  @param len    The maximum length to read in.
   *  @return The number of bytes read or -1 if the end of the payload has been reached.
   *  @throws NullPointerException if the buffer is null.
   *  @throws ArrayIndexOutOfBoundsException If the length of the buffer is less than len+boff.
   */
  int readFrame (byte[] buffer, int boff, int poff, int len);
  
  /** <i>Optional functionality:</i> Sets the content of the frame and updates the length
   *  of the frame.
   *  @param buffer The new payload.
   *  @throws NullPointerException if the buffer is null.
   *  @throws UnsupportedOperationException If this method is not supported
   *  @throws IllegalArgumentException If the new payload length is invalid.
   *  @see #setFrame(byte[],int,int)
   */
  void setFrame (byte[] buffer);
  
  /** <i>Optional functionality:</i> Sets the content of the frame and updates the length
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
  void setFrame (byte[] buffer, int offset, int length);
}

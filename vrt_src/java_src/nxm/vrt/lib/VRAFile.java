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

import java.io.Closeable;
import java.io.Flushable;
import java.net.URI;
import java.nio.ByteOrder;
import java.util.Iterator;

/** Defines a VRA file type. The most frequently used implementations of interface this is
 *  {@link BasicVRAFile}. <br>
 *  <br>
 *  Note that the numeric representation (byte order) used by all VRA files is
 *  {@link ByteOrder#BIG_ENDIAN}.
 *
 *  @author 
 */
public interface VRAFile extends Iterable<VRTPacket>, Flushable, Closeable {

  /** The minimum file version supported by this version of the library.
   *  <pre>
   *    MIN_VERSION_SUPPORTED = {@value}
   *  </pre>
   */
  static final byte MIN_VERSION_SUPPORTED = 1;

  /** The maximum file version supported by this version of the library.
   *  <pre>
   *    MAX_VERSION_SUPPORTED = {@value}
   *  </pre>
   */
  static final byte MAX_VERSION_SUPPORTED = 1;

  /** The default file version used for a new output file.
   *  <pre>
   *    DEFAULT_VERSION = {@value}
   *  </pre>
   */
  static final byte DEFAULT_VERSION = 1;

  /** The file name extension for a VRA file.
   *  <pre>
   *    FILE_NAME_EXT = {@value}
   *  </pre>
   */
  static final String FILE_NAME_EXT = ".vra";

  /** The MIME type for a VRA file.
   *  <pre>
   *    MIME_TYPE = {@value}
   *  </pre>
   */
  static final String MIME_TYPE = "application/x-vita-radio-archive";
  
  /** The length of the header in bytes.
   *  <pre>
   *    HEADER_LENGTH = {@value}
   *  </pre>
   */
  static final int HEADER_LENGTH = 20;
  
  /** The length of the trailer in bytes.
   *  <pre>
   *    TRAILER_LENGTH = {@value}
   *  </pre>
   */
  static final int TRAILER_LENGTH = 0;

  /** The maximum length of the file in bytes (header + payload + trailer). Note that this
   *  is NOT an absolute limit imposed by the VRA specification, but the limit imposed by the
   *  specification in situations where the FileSize field is used and/or the underlying file
   *  system doesn't provide the required file size details. In most cases, this is far in excess
   *  of what any file system can handle.
   *  <pre>
   *    MAX_FILE_LENGTH = {@value}
   *  </pre>
   *  @see #getFileLength()
   */
  static final long MAX_FILE_LENGTH = 0x7FFFFFFFFFFFFFFFL;
  
  /** The minimum length of the file in bytes (header + no payload). Note that this is the
   *  absolute limit imposed by the VRA specification and results in a near-useless frame
   *  with no payload packets.
   *  <pre>
   *    MIN_FILE_LENGTH = HEADER_LENGTH + TRAILER_LENGTH = {@value}
   *  </pre>
   *  @see #getFileLength()
   */
  static final long MIN_FILE_LENGTH = HEADER_LENGTH + TRAILER_LENGTH;
  
  /** The maximum length of the payload in bytes.
   *  <pre>
   *    MAX_PAYLOAD_LENGTH = MAX_FILE_LENGTH - HEADER_LENGTH - TRAILER_LENGTH = {@value}
   *  </pre>
   */
  static final long MAX_PAYLOAD_LENGTH = MAX_FILE_LENGTH - HEADER_LENGTH - TRAILER_LENGTH;

  /** This is the 32-bit FWA (frame alignment word) marking the start of a VRAF file. This is also
   *  commonly known as the "magic number" for the file.
   *  <pre>
   *    VRA_FAW = 0x56524146 = { 'V', 'R', 'A', 'F' }
   *  </pre>
   */
  static final int VRA_FAW = 0x56524146;

  /** This is the 1st recorded byte of the 32-bit FWA ({@link #VRA_FAW}). */
  static final byte VRA_FAW_0 = 0x56;
  /** This is the 2nd recorded byte of the 32-bit FWA ({@link #VRA_FAW}). */
  static final byte VRA_FAW_1 = 0x52;
  /** This is the 3rd recorded byte of the 32-bit FWA ({@link #VRA_FAW}). */
  static final byte VRA_FAW_2 = 0x41;
  /** This is the 4th recorded byte of the 32-bit FWA ({@link #VRA_FAW}). */
  static final byte VRA_FAW_3 = 0x46;

  /** Gets a free-form description of the file. Note that the content and structure of this
   *  string is implementation dependant and may change at any time.
   *  @return A free-form string describing the file.
   */
  @Override
  String toString ();

  /** Gets the URI for the file.
   *  @return The URI for the file or null if not applicable.
   */
  URI getURI ();
  
  /** Checks for equality with an unknown object. Two VRA files are considered equal if they
   *  have the same values for {@link #getVersion()} and  {@link #getFileLength()} and the
   *  packets contained in their payloads are identical. The actual implementation classes
   *  are not considered relevant for checking equality.<br>
   *  <br>
   *  Note that this specifically permits different values in the FileLength field (and
   *  consequently CRC header field) since setting it to 0 (in the header) and using the
   *  file length as reported by the O/S is not considered a significant difference in
   *  the file content.
   *  @param o The unknown object (can be null).
   *  @return true if they are identical, false otherwise. (This will always returns false
   *          if the object is null.)
   */
  @Override
  boolean equals (Object o);
  
  /** Computes the hash code for a VRA file. For performance reasons, the hash code is based
   *  only on the size of the file. Specifically, the following algorithm is mandatory:
   *  <pre>
   *    public final int hashCode () {
   *      long len = getFileLength();
   *      return (int)((len >> 32) ^ len);
   *    }
   *  </pre>
   *  @return The hash code for the object.
   */
  @Override
  int hashCode ();
  
  /** Checks to see if the file is valid. This checks the overall structure of the file and
   *  verifies the checksum (if specified). It also checks to see that the length of the file as
   *  reported in the frame header is equal to the length of the header and trailer plus the length
   *  of each packet in the trailer. For the purposes of the packet length check, only the length
   *  of a packet is considered, no consideration is made to the validity of the packet.
   *  @return true if it is valid, false otherwise.
   */
  boolean isFileValid ();
  
  /** Checks to see if the file is valid. This method adds an extra length check onto the basic
   *  checks performed by {@link #isFileValid()}. The most common use for this method is to
   *  verify that a packet read from disk matches the length expected.
   *  @param length Verifies that the length of the packet matches this value.
   *  @return true if it is valid, false otherwise.
   */
  boolean isFileValid (long length);

  /** Checks to see if the CRC for the frame is valid. This will always return true when the CRC
   *  is set to the special {@link VRLFrame#NO_CRC} value.
   *  @return true if it is valid, false otherwise.
   */
  boolean isCRCValid ();

  /** This will compute the CRC value for the file and set it in the CRC field. All successful
   *  calls to one of the set methods on this file will result in the CRC field being updated,
   *  but that usually means that the CRC was merely set to the special {@link VRLFrame#NO_CRC}
   *  value.
   */
  void updateCRC ();

  /** This will compute the length of the file and set it in the FileLength field. All successful
   *  calls to one of the write methods on this file will result in the FileLength field being
   *  updated, but that usually means that the FileLength field was merely set to zero indicating
   *  that the file length should be obtained from the O/S.
   */
  void updateFileLength ();

  /** Gets the file version. */
  int getVersion ();

  /** <i>Optional functionality:</i> Sets the file version.
   *  @param version The file version. Use {@link #DEFAULT_VERSION} to indicate the default version
   *                 should be used.
   *  @throws UnsupportedOperationException If this method is not supported
   *  @throws IllegalArgumentException If the version number provided is invalid.
   */
  void setVersion (int version);

  /** Gets the total length of the file in bytes including the header and trailer.
   *  @return The total length of the file in bytes or -1 if unknown.
   */
  long getFileLength ();

  /** <b>Internal Use Only: Gets a copy of the VRA header. This is intended for
   *  use in implementing {@link #equals} and has little value otherwise. <b>The
   *  value returned must be treated as read-only, and may or may not change
   *  to reflect future changes!</b>
   *  @return The header, this will be a byte buffer of length {@link #HEADER_LENGTH}.
   */
  byte[] getHeader ();

  /** Gets an iterator over all of the VRT packets contained in the VRL frame. This iterator will
   *  not support the <tt>remove()</tt> function and will have unpredictable behavior if the
   *  underlying file is modified during iteration.
   *  @return An iterator that will iterate over all the packets in the frame.
   */
  @Override
  Iterator<VRTPacket> iterator ();

  /** <i>Optional functionality:</i> Appends a VRTPacket to the end of the file.
   *  @param p The packet to append.
   *  @throws UnsupportedOperationException If this method is not supported
   */
  void append (VRTPacket p);

  /** Flushes this file by writing any buffered output to the underlying stream. If no "flushing"
   *  is required (e.g. for a read-only file), invoking this method has no effect.
   */
  @Override
  void flush ();

  /** Closes this file and releases any system resources associated with it. If the file is already
   *  closed then invoking this method has no effect.
   */
  @Override
  void close ();
}

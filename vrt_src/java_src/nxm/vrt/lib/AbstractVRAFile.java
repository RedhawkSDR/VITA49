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

import java.io.IOException;
import java.net.URI;
import java.util.Arrays;
import java.util.Iterator;
import java.util.NoSuchElementException;

import static nxm.vrt.lib.VRLFrame.NO_CRC;
import static nxm.vrt.lib.VRLFrame.NO_CRC_0;
import static nxm.vrt.lib.VRLFrame.NO_CRC_1;
import static nxm.vrt.lib.VRLFrame.NO_CRC_2;
import static nxm.vrt.lib.VRLFrame.NO_CRC_3;

/** Abstract class that makes it easier to implement a VRA file type. Most implementations
 *  that extend this class will only need to override the following methods:
 *  <pre>
 *    {@link #close()}
 *    {@link #flush(boolean)}
 *    {@link #getFileLengthOS()}
 *    {@link #getFileLengthRW()}
 *    {@link #open()}
 *    {@link #read(long, byte[], int, int)}
 *    {@link #write(long, byte[], int, int, boolean)}
 *  </pre>
 *
 *  @author 
 */
public abstract class AbstractVRAFile implements VRAFile, PacketIterator.PacketContainer {
  private static final byte[] DEFAULT_HEADER = { VRA_FAW_0, VRA_FAW_1, VRA_FAW_2, VRA_FAW_3,
                                                 DEFAULT_VERSION, 0, 0, 0,
                                                 0, 0, 0, 0,
                                                 0, 0, 0, 0,
                                                 NO_CRC_0, NO_CRC_1, NO_CRC_2, NO_CRC_3 };

  /** Indicator used with <tt>write(..)</tt> calls indicating the write should
   *  be made to the end-of-file.
   */
  public static final long EOF = -1;
  
  
  private   final byte[]   header;        // A copy of the header
  private         int      hdrVersion;    // Local copy of file version (from header)
  private         long     hdrFileLength; // Local copy of file size    (from header)
  private         int      hdrCRC;        // Local copy of CRC          (from header)
  protected final URI      uri;           // The URI for the file (null if n/a).
  protected final boolean  isRead;        // Is the file being opened for reading?
  protected final boolean  isWrite;       // Is the file being opened for writing?
  protected final boolean  isSetSize;     // Should the size be set on write?
  protected final boolean  isSetCRC;      // Should the CRC  be set on write?
  protected final boolean  isStrict;      // Should strict packet checks be used on write?

  /** <b>Internal Use Only:</b> Creates a new instance, but does not open it yet.
   *  Subclasses must call <tt>open()</tt> as part of the constructor.
   *  @param uri        The URI for the file (null if n/a).
   *  @param isRead     Is the file being opened for reading?
   *  @param isWrite    Is the file being opened for writing?
   *  @param isSetSize  Should the size be set on write? (ignored if read-only)
   *  @param isSetCRC   Should the CRC  be set on write? (ignored if read-only)
   *  @param isStrict   Should strict packet checks be used on write?
   */
  protected AbstractVRAFile (URI uri, boolean isRead,    boolean isWrite,
                                      boolean isSetSize, boolean isSetCRC,
                                      boolean isStrict) {
    this.header        = DEFAULT_HEADER.clone();
    this.hdrVersion    = DEFAULT_VERSION;  // <-- matches DEFAULT_HEADER
    this.hdrFileLength = 0;                // <-- matches DEFAULT_HEADER
    this.hdrCRC        = NO_CRC;           // <-- matches DEFAULT_HEADER
    this.uri           = uri;
    this.isRead        = isRead;
    this.isWrite       = isWrite;
    this.isSetSize     = isSetSize;
    this.isSetCRC      = isSetCRC;
    this.isStrict      = isStrict;
  }
  
  @Override public final void    flush ()      { flush(true); }
  @Override public final byte[]  getHeader ()  { return header; }
  @Override public final int     getVersion () { return hdrVersion; }
  @Override public final URI     getURI ()     { return uri; }
  @Override public final boolean isCRCValid () { return (hdrCRC==NO_CRC) || (hdrCRC==computeCRC()); }
  
  /** Opens the file. Overriding classes should call <tt>super.open()</tt> once
   *  the file has been opened in order to ensure the header fields have been
   *  read in (if applicable) and initialized.
   */
  protected void open () {
    if (isRead) {
      readHeader();
    }
    if (isWrite) {
      // If we don't need to set FileLength or CRC, clear them now so we don't
      // need to fuss with them in the future.
      if (!isSetSize) _setFileLengthHeader(0L);
      if (!isSetCRC ) _setCRC(NO_CRC);
      writeHeader();
    }
  }

  /** Reads the header in. */
  protected void readHeader () {
    read(0, header, 0, HEADER_LENGTH);
    hdrVersion    = 0xFF & header[4];
    hdrFileLength = VRTMath.unpackLong(header, 8);
    hdrCRC        = VRTMath.unpackInt(header, 16);
  }
  
  @Override
  public String toString () {
    // Do not include the file length or version since it may not be readable if
    // the file isn't open and this may result in errors with exception handling,
    // etc.
    return getClass().getName() + ": URI=" + getURI();
  }

  @Override
  public boolean equals (Object o) {
    if (o == this) return true;
    if (!(o instanceof VRAFile)) return false;
    
    VRAFile that = (VRAFile)o;
    boolean ok   = (this.getVersion()    == that.getVersion())
                && (this.getFileLength() == that.getFileLength());
    
    if (!ok) return false;
    
    if (that instanceof AbstractVRAFile) {
      AbstractVRAFile _that = (AbstractVRAFile)that;
      
      // If both have a matching FileLength in the header, and both have their
      // CRC set, then the CRC values must be equal. If not, we can assume the
      // files are not equal.
      if ((this.hdrFileLength == _that.hdrFileLength) &&
          (this.hdrCRC  != NO_CRC) &&
          (_that.hdrCRC != NO_CRC) &&
          (this.hdrCRC  != _that.hdrCRC)) {
        return false;
      }
    }
    
    if (!Arrays.equals(this.header, that.getHeader())) return false;

    Iterator<VRTPacket> this_packets = this.iterator();
    Iterator<VRTPacket> that_packets = that.iterator();

    while (this_packets.hasNext()) {
      if (!that_packets.hasNext()) return false;

      VRTPacket this_pkt = this_packets.next();
      VRTPacket that_pkt = that_packets.next();

      if (!this_pkt.equals(that_pkt)) {
        return false;
      }
    }
    return !that_packets.hasNext();
  }

  @Override
  public final int hashCode () {
    long len = getFileLength();
    return (int)((len >> 32) ^ len);
  }

  /** The very basic frame validity checks. */
  private boolean isFileValid0 () {
    return (getFileLength() >= HEADER_LENGTH)
        && (header[0] == VRA_FAW_0)
        && (header[1] == VRA_FAW_1)
        && (header[2] == VRA_FAW_2)
        && (header[3] == VRA_FAW_3)
        && isCRCValid();
  }

  @Override
  public boolean isFileValid () {
    if (!isFileValid0()) return false;
    
    long lenFromHeader = getFileLengthHeader();
    long lenFromOS     = getFileLengthOS();

    if (lenFromOS > 0) {
      if ((lenFromHeader != 0) && (lenFromOS < lenFromHeader)) {
        return false; // file on disk shorter than header reports
      }
      if ((lenFromHeader == 0) && ((lenFromOS & 0x03) != 0)) {
        return false; // file not a multiple of 32-bits in length
      }
    }

    try {
      PacketIterator pi = iterator();
      while (pi.hasMoreElements()) {
        pi.skipNext(); // <-- throws exception if invalid
      }
    }
    catch (Exception e) {
      return false;
    }
    return true;
  }

  @Override
  public boolean isFileValid (long length) {
    return isFileValid() && (getFileLength() == length);
  }

  @Override
  public final void updateFileLength () {
    long len = (isSetSize)? getFileLengthRW() : 0L;
    setFileLengthHeader(len);
  }

  @Override
  public final void updateCRC () {
    int crc = (isSetCRC)? computeCRC() : NO_CRC;
    setCRC(crc);
  }

  /** Computes the CRC for the frame, but does not insert it into the frame. */
  private int computeCRC () {
    // References:
    //   [1] VITA 49.1
    //   [2] Warren, Henry S. Jr. "Hacker's Delight." Addison-Wesley, 2002. (ISBN 0-201-91465-4)

    final int COEFICIENTS = 0xEDB88320; // Corresponds to CRC32 polynomial
    int crc = 0;

    // This is based on the code given in Appendix A of [1], but it has
    // been extensively optimized. Note that to make the (COEFICIENTS * (0 or 1)) trick work
    // we need to compute the CRC in bit-reversed order and then flip the bits at the end.
    
    // compute over the header
    for (int off = 0; off < HEADER_LENGTH; off++) {
      if ((off >= 16) && (off < 20)) continue; // skip CRC field
      int val = header[off];
      crc = (crc >>> 1) ^ (COEFICIENTS * ((crc ^ (val >> 7)) & 1));
      crc = (crc >>> 1) ^ (COEFICIENTS * ((crc ^ (val >> 6)) & 1));
      crc = (crc >>> 1) ^ (COEFICIENTS * ((crc ^ (val >> 5)) & 1));
      crc = (crc >>> 1) ^ (COEFICIENTS * ((crc ^ (val >> 4)) & 1));
      crc = (crc >>> 1) ^ (COEFICIENTS * ((crc ^ (val >> 3)) & 1));
      crc = (crc >>> 1) ^ (COEFICIENTS * ((crc ^ (val >> 2)) & 1));
      crc = (crc >>> 1) ^ (COEFICIENTS * ((crc ^ (val >> 1)) & 1));
      crc = (crc >>> 1) ^ (COEFICIENTS * ((crc ^ (val     )) & 1));
    }
    
    // compute over the payload
    long   offset = HEADER_LENGTH;
    long   end    = getFileLength();
    byte[] buffer = new byte[4096];
    while (offset < end) {
      int numRead = read(offset, buffer, 0, buffer.length);
      
      for (int off = 0; off < numRead; off++) {
        int val = buffer[off];
        crc = (crc >>> 1) ^ (COEFICIENTS * ((crc ^ (val >> 7)) & 1));
        crc = (crc >>> 1) ^ (COEFICIENTS * ((crc ^ (val >> 6)) & 1));
        crc = (crc >>> 1) ^ (COEFICIENTS * ((crc ^ (val >> 5)) & 1));
        crc = (crc >>> 1) ^ (COEFICIENTS * ((crc ^ (val >> 4)) & 1));
        crc = (crc >>> 1) ^ (COEFICIENTS * ((crc ^ (val >> 3)) & 1));
        crc = (crc >>> 1) ^ (COEFICIENTS * ((crc ^ (val >> 2)) & 1));
        crc = (crc >>> 1) ^ (COEFICIENTS * ((crc ^ (val >> 1)) & 1));
        crc = (crc >>> 1) ^ (COEFICIENTS * ((crc ^ (val     )) & 1));
      }
      offset += numRead;
    }

    // Flip CRC bits as noted above, we are using the bit-shifting algorithm from Section 7-1 in
    // [2] rather than a loop-based solution in Appendix A of [1] which is considerably slower.
    crc = ((crc & 0x55555555) <<  1) | ((crc & 0xAAAAAAAA) >>>  1);  // abcdefgh -> badcfehg (per byte)
    crc = ((crc & 0x33333333) <<  2) | ((crc & 0xCCCCCCCC) >>>  2);  // badcfehg -> dcbahgfe (per byte)
    crc = ((crc & 0x0F0F0F0F) <<  4) | ((crc & 0xF0F0F0F0) >>>  4);  // dcbahgfe -> hgfedcba (per byte)
    crc = ((crc & 0x00FF00FF) <<  8) | ((crc & 0xFF00FF00) >>>  8);  // 0123 -> 1032
    crc = ((crc & 0x0000FFFF) << 16) | ((crc & 0xFFFF0000) >>> 16);  // 1032 -> 3210

    return crc;
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // PacketContainer Functions
  //////////////////////////////////////////////////////////////////////////////////////////////////
  @Override
  public PacketIterator iterator () {
    return new PacketIterator(this, HEADER_LENGTH, false, false, true);
  }
  
  @Override
  public boolean hasNextPacket (PacketIterator pi) {
    return (pi.offset >= HEADER_LENGTH) && (pi.offset < getFileLength());
  }
  
  @Override
  public VRTPacket getNextPacket (PacketIterator pi, boolean skip) {
    if (!hasNextPacket(pi)) {
      throw new NoSuchElementException("No such element in "+pi);
    }
    
    // ==== READ PACKET HEADER =================================================
    int numRead = read(pi.offset, pi.buf, 0, 4);
    
    if (numRead != 4) {
      // Invalid File
      throw new RuntimeException("Error reading from "+this+" at "+pi.offset);
    }
    int len = ((0xFF & pi.buf[2]) << 10) | ((0xFF & pi.buf[3]) << 2);

    if ((pi.offset+len > getFileLength()) || (len < 4)) {
      // Invalid Packet
      throw new RuntimeException("Error reading from "+this+" at "+pi.offset);
    }
    
    // ==== READ PACKET ========================================================
    VRTPacket packet = null;
    
    if (!skip) {
      if (pi.buf.length < len) {
        pi.buf = Arrays.copyOf(pi.buf, len);
      }

      int toRead = len - 4;

      do {
        int n = read(pi.offset+numRead, pi.buf, numRead, toRead);
        if (n < 0) {
          // Invalid Packet
          throw new RuntimeException("Error reading from "+this+" at "+pi.offset);
        }
        numRead += n;
        toRead  -= n;
      } while (numRead < toRead);

      packet = new BasicVRTPacket(pi.buf, 0, -1, pi.readOnly, pi.direct);
      packet = (pi.resolve)? VRTConfig.getPacket(packet, false) : packet;
    }
    
    // ==== UPDATE OFFSET COUNTER ==============================================
    pi.offset += len;
    return packet;
  }
  
  @Override
  public void removePrevPacket (PacketIterator pi) {
    throw new UnsupportedOperationException("remove() not supported");
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Get / Set
  //////////////////////////////////////////////////////////////////////////////////////////////////
  @Override
  public void append (VRTPacket p) {
    String err = p.getPacketValid(isStrict);
    if (err != null) throw new IllegalArgumentException(err);

    byte[] buf = p.getPacket();
    write(EOF, buf, 0, buf.length);
  }

  @Override
  public final void setVersion (int ver) {
    if (_setVersion(ver)) {
      writeHeader();
    }
  }

  /** Sets the version, but does not write header to disk.
   *  @param ver The new version.
   *  @return true if header needs to be written to disk; false if no changes made.
   */
  private boolean _setVersion (int ver) {
    if (hdrVersion == ver) return false;
    if (!isWrite) throw new UnsupportedOperationException("File is read-only");
    
    if ((ver < MIN_VERSION_SUPPORTED) || (ver > MAX_VERSION_SUPPORTED)) {
      throw new IllegalArgumentException("Invalid version specified, expected version in the range "
                                       + MIN_VERSION_SUPPORTED + " <= ver <= "+MAX_VERSION_SUPPORTED
                                       + " but given ver="+ver+".");
    }
    header[4] = (byte)ver;
    return true;
  }

  @Override
  public long getFileLength () {
    long length = getFileLengthHeader();
    return (length != 0)? length : getFileLengthOS();
  }

  /** Gets the file length from the O/S or -1 if unknown. */
  protected abstract long getFileLengthOS ();

  /** Gets the read/write length. This is the index of the last octet that has
   *  been read from or written to the file. This may either be the maximum
   *  value of the read/write pointer *OR* the size of the file if such is known
   *  prior to reading/writing.
   */
  protected abstract long getFileLengthRW ();

  /** Gets the file length from the header (0 if unspecified). */
  protected final long getFileLengthHeader () {
    return hdrFileLength;
  }

  /** Sets the file length in the header (0 if unspecified).
   *  @param len The new length value (in octets).
   */
  protected final void setFileLengthHeader (long len) {
    if (_setFileLengthHeader(len)) {
      writeHeader();
    }
  }

  /** Sets the file length in the header, but does not write header to disk.
   *  @param len The new length value (in octets).
   *  @return true if header needs to be written to disk; false if no changes made.
   */
  private boolean _setFileLengthHeader (long len) {
    if (hdrFileLength == len) return false;
    if (!isWrite) throw new UnsupportedOperationException("File is read-only");
    hdrFileLength = len;
    VRTMath.packLong(header, 8, len);
    return true;
  }
  
  /** Sets the CRC in the header.
   *  @param crc The CRC or {@link VRLFrame#NO_CRC} if no crc is being used.
   */
  protected final void setCRC (int crc) {
    if (_setCRC(crc)) {
      writeHeader();
    }
  }
  
  /** Sets the CRC in the header, but does not write header to disk.
   *  @param crc The CRC or {@link #NO_CRC} if no crc is being used.
   *  @return true if header needs to be written to disk; false if no changes made.
   */
  private boolean _setCRC (int crc) {
    if (hdrCRC == crc) return false;
    if (!isWrite) throw new UnsupportedOperationException("File is read-only");
    hdrCRC = crc;
    VRTMath.packInt(header, 16, crc);
    return true;
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Read / Write
  //////////////////////////////////////////////////////////////////////////////////////////////////
  /** Write the header to disk. */
  protected void writeHeader () {
    write(0, header, 0, HEADER_LENGTH, false);
  }

  /** Flush the content to the disk. Overriding classes should call
   *  <tt>super.flush(true)</tt> if they wish to make sure that the FileLength
   *  and CRC get updated.
   *  @param force Force contents to be flushed, otherwise skip for now.
   */
  protected void flush (boolean force) {
    if (force) {
      // Since this is called frequently, we do some stuff manually so that
      // we can use a single writeHeader() call. Note that setting of the
      // length must come *before* computation of the updated CRC.
      long    len      = (isSetSize)? getFileLengthRW() : 0L;
      boolean writeLen = _setFileLengthHeader(len);
      int     crc      = (isSetCRC)? computeCRC() : NO_CRC;
      boolean writeCRC = _setCRC(crc);
      
      if (writeLen || writeCRC) {
        writeHeader();
      }
    }
  }

  /** Closes the file. Overriding classes should call <tt>super.close()</tt> first
   *  to make sure any last-minute updates to FileLength and CRC have been written
   *  out.
   */
  @Override
  public void close () {
    if (isWrite) {
      flush(true);
    }
  }

  /** Reads from the file.
   *  @param off  File offset at which to begin reading.
   *  @param buf  The buffer to hold the data read in.
   *  @param boff Offset into the buffer.
   *  @param len  The maximum number of octets to read.
   *  @return The number of octets actually read in.
   *  @throws UnsupportedOperationException If the file is write-only.
   */
  protected abstract int read (long off, byte[] buf, int boff, int len);

  /** Writes to the file.
   *  @param off  File offset at which to begin writing (EOF to write at end of file).
   *  @param buf  The buffer containing the data to write.
   *  @param boff Offset into the buffer.
   *  @param len  The number of octets to write.
   *  @throws UnsupportedOperationException If the file is read-only.
   */
  protected final void write (long off, byte[] buf, int boff, int len) {
    write(off, buf, boff, len, true);
  }

  /** Writes to the file. This should call <tt>flush(doFlush)</tt> following the write.
   *  @param off   File offset at which to begin writing.
   *  @param buf   The buffer containing the data to write.
   *  @param boff  Offset into the buffer.
   *  @param len   The number of octets to write.
   *  @param flush Flush to disk? Setting this to false disables header updates
   *               and flushing to disk, even if required.
   *  @throws UnsupportedOperationException If the file is read-only.
   */
  protected abstract void write (long off, byte[] buf, int boff, int len, boolean flush);
  
  //////////////////////////////////////////////////////////////////////////////////////////////////
  // RWException
  //////////////////////////////////////////////////////////////////////////////////////////////////
  /** A Read/Write exception which is a simple wrapper for an {@link IOException} that extends
   *  {@link RuntimeException} permitting it to be thrown from functions that do not declare
   *  an {@link IOException} to be thrown.
   */
  public static class RWException extends RuntimeException {
    static final long serialVersionUID = 0x95f212236516b57dL;
    
    public RWException (IOException e) {
      super(e.getMessage(), e);
    }
  }
}

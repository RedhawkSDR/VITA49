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
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see http://www.gnu.org/licenses/.
 * ============================================================
 */

#ifndef _AbstractVRAFile_h
#define _AbstractVRAFile_h
#include "BasicVRLFrame.h"
#include "BasicVRTPacket.h"
#include "PacketIterator.h"

/** Indicator used with <tt>write(..)</tt> calls indicating the write should
 *  be made to the end-of-file.
 */
#ifndef EOF
# define EOF -1
#else
# if EOF != -1
#  error "Incompatible definition of EOF (EOF != -1)"
# endif
#endif

namespace vrt {
  /** Defines a VRA file type. The most frequently used implementations of interface
   *  this is {@link BasicVRAFile}. Most implementations that extend this class will
   *  only need to override the following methods:
   *  <pre>
   *    {@link #close}
   *    {@link #flush}
   *    {@link #getFileLengthOS}
   *    {@link #getFileLengthRW}
   *    {@link #open}
   *    {@link #read}
   *    {@link #write}
   *  </pre>
   */
  class AbstractVRAFile : public VRTObject, public PacketContainer {
    /** The minimum file version supported by this version of the library.
     *  <pre>
     *    MIN_VERSION_SUPPORTED = 1
     *  </pre>
     */
    public: static const int8_t MIN_VERSION_SUPPORTED = 1;

    /** The maximum file version supported by this version of the library.
     *  <pre>
     *    MAX_VERSION_SUPPORTED = 1
     *  </pre>
     */
    public: static const int8_t MAX_VERSION_SUPPORTED = 1;

    /** The default file version used for a new output file.
     *  <pre>
     *    DEFAULT_VERSION = 1
     *  </pre>
     */
    public: static const int8_t DEFAULT_VERSION = 1;

    /** The file name extension for a VRA file.
     *  <pre>
     *    FILE_NAME_EXT = ".vra"
     *  </pre>
     */
    public: static const string FILE_NAME_EXT;

    /** The MIME type for a VRA file.
     *  <pre>
     *    MIME_TYPE = "application/x-vita-radio-archive"
     *  </pre>
     */
    public: static const string MIME_TYPE;

    /** The length of the header in bytes.
     *  <pre>
     *    HEADER_LENGTH = 20
     *  </pre>
     */
    public: static const int32_t HEADER_LENGTH = 20;

    /** The length of the trailer in bytes.
     *  <pre>
     *    TRAILER_LENGTH = 0
     *  </pre>
     */
    public: static const int32_t TRAILER_LENGTH = 0;

    /** The maximum length of the file in bytes (header + payload + trailer). Note that this
     *  is NOT an absolute limit imposed by the VRA specification, but the limit imposed by the
     *  specification in situations where the FileSize field is used and/or the underlying file
     *  system doesn't provide the required file size details. In most cases, this is far in excess
     *  of what any file system can handle.
     *  <pre>
     *    MAX_FILE_LENGTH = 0x7FFFFFFFFFFFFFFF
     *  </pre>
     *  @see #getFileLength()
     */
    public: static const int64_t MAX_FILE_LENGTH = __INT64_C(0x7FFFFFFFFFFFFFFF);

    /** The minimum length of the file in bytes (header + no payload). Note that this is the
     *  absolute limit imposed by the VRA specification and results in a near-useless frame
     *  with no payload packets.
     *  <pre>
     *    MIN_FILE_LENGTH = HEADER_LENGTH + TRAILER_LENGTH
     *  </pre>
     *  @see #getFileLength()
     */
    public: static const int64_t MIN_FILE_LENGTH = HEADER_LENGTH + TRAILER_LENGTH;

    /** The maximum length of the payload in bytes.
     *  <pre>
     *    MAX_PAYLOAD_LENGTH = MAX_FILE_LENGTH - HEADER_LENGTH - TRAILER_LENGTH
     *  </pre>
     */
    public: static const int64_t MAX_PAYLOAD_LENGTH = MAX_FILE_LENGTH - HEADER_LENGTH - TRAILER_LENGTH;

    /** This is the 32-bit FWA (frame alignment word) marking the start of a VRAF file. This is also
     *  commonly known as the "magic number" for the file.
     *  <pre>
     *    VRA_FAW = 0x56524146 = { 'V', 'R', 'A', 'F' }
     *  </pre>
     */
    public: static const int VRA_FAW = 0x56524146;

    /** This is the 1st recorded byte of the 32-bit FWA ({@link #VRA_FAW}). */
    public: static const char VRA_FAW_0 = 0x56;
    /** This is the 2nd recorded byte of the 32-bit FWA ({@link #VRA_FAW}). */
    public: static const char VRA_FAW_1 = 0x52;
    /** This is the 3rd recorded byte of the 32-bit FWA ({@link #VRA_FAW}). */
    public: static const char VRA_FAW_2 = 0x41;
    /** This is the 4th recorded byte of the 32-bit FWA ({@link #VRA_FAW}). */
    public: static const char VRA_FAW_3 = 0x46;

    private:   char     header[HEADER_LENGTH];        // A copy of the header
    private:   int32_t  hdrVersion;    // Local copy of file version (from header)
    private:   int64_t  hdrFileLength; // Local copy of file size    (from header)
    private:   int32_t  hdrCRC;        // Local copy of CRC          (from header)
    protected: string   uri;           // The URI for the file (null if n/a).
    protected: bool     isRead;        // Is the file being opened for reading?
    protected: bool     isWrite;       // Is the file being opened for writing?
    protected: bool     isSetSize;     // Should the size be set on write?
    protected: bool     isSetCRC;      // Should the CRC  be set on write?
    protected: bool     isStrict;      // Should strict packet checks be used on write?


    /** Basic destructor for the class. */
    public: ~AbstractVRAFile () { }

    /** Basic copy constructor for the class. */
    public: AbstractVRAFile (const AbstractVRAFile &f);

    /** <b>Internal Use Only:</b> Creates a new instance, but does not open it yet.
     *  Subclasses must call <tt>open()</tt> as part of the constructor.
     *  @param _uri        The URI for the file (null if n/a).
     *  @param _isRead     Is the file being opened for reading?
     *  @param _isWrite    Is the file being opened for writing?
     *  @param _isSetSize  Should the size be set on write? (ignored if read-only)
     *  @param _isSetCRC   Should the CRC  be set on write? (ignored if read-only)
     *  @param _isStrict   Should strict packet checks be used on write?
     */
    protected: AbstractVRAFile (string _uri, bool _isRead,    bool _isWrite,
                                             bool _isSetSize, bool _isSetCRC,
                                             bool _isStrict);

    /** Gets a free-form description of the file. Note that the content and structure of this
     *  string is implementation dependant and may change at any time.
     *  @return A free-form string describing the file.
     */
    public: virtual string toString () const;

    /** Gets the URI for the file.
     *  @return The URI for the file or null if not applicable.
     */
    public: inline string getURI () const {
      return uri;
    }

    using VRTObject::equals;
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
    public: inline virtual bool equals (const VRTObject &o) const {
      try {
        return equals(*checked_dynamic_cast<const AbstractVRAFile*>(&o));
      }
      catch (VRTException e) {
        UNUSED_VARIABLE(e);
        return false;
      }
    }

    /** Checks for equality with another file. */
    public: virtual bool equals (const AbstractVRAFile &f) const;

    /** Checks to see if the file is valid. This checks the overall structure of the file and
     *  verifies the checksum (if specified). It also checks to see that the length of the file as
     *  reported in the frame header is equal to the length of the header and trailer plus the length
     *  of each packet in the trailer. For the purposes of the packet length check, only the length
     *  of a packet is considered, no consideration is made to the validity of the packet.
     *  @return true if it is valid, false otherwise.
     */
    public: virtual bool isFileValid () const;

    /** Checks to see if the file is valid. This method adds an extra length check onto the basic
     *  checks performed by {@link #isFileValid()}. The most common use for this method is to
     *  verify that a packet read from disk matches the length expected.
     *  @param length Verifies that the length of the packet matches this value.
     *  @return true if it is valid, false otherwise.
     */
    public: virtual bool isFileValid (int64_t length) const;

    /** Checks to see if the CRC for the frame is valid. This will always return true when the CRC
     *  is set to the special {@link BasicVRLFrame#NO_CRC} value.
     *  @return true if it is valid, false otherwise.
     */
    public: inline bool isCRCValid () const {
      return (hdrCRC==BasicVRLFrame::NO_CRC) || (hdrCRC==computeCRC());
    }

    /** This will compute the CRC value for the file and set it in the CRC field. All successful
     *  calls to one of the set methods on this file will result in the CRC field being updated,
     *  but that usually means that the CRC was merely set to the special {@link BasicVRLFrame#NO_CRC}
     *  value.
     */
    public: virtual void updateCRC ();

    /** This will compute the length of the file and set it in the FileLength field. All successful
     *  calls to one of the write methods on this file will result in the FileLength field being
     *  updated, but that usually means that the FileLength field was merely set to zero indicating
     *  that the file length should be obtained from the O/S.
     */
    public: virtual void updateFileLength ();

    /** Gets the file version. */
    public: inline int32_t getVersion () const {
      return hdrVersion;
    }

    /** <i>Optional functionality:</i> Sets the file version.
     *  @param version The file version. Use {@link #DEFAULT_VERSION} to indicate the default version
     *                 should be used.
     *  @throws UnsupportedOperationException If this method is not supported
     *  @throws IllegalArgumentException If the version number provided is invalid.
     */
    public: inline void setVersion (int32_t version) {
      if (_setVersion(version)) {
        writeHeader();
      }
    }

    /** Gets the total length of the file in bytes including the header and trailer.
     *  @return The total length of the file in bytes or -1 if unknown.
     */
    public: virtual int64_t getFileLength () const;

    /** <b>Internal Use Only:</b> Gets a copy of the VRA header. This is intended for
     *  use in implementing {@link #equals} and has little value otherwise. <b>The
     *  value returned must be treated as read-only, and may or may not change
     *  to reflect future changes!</b>
     *  @return A copy of the header, this will be a pointer to a byte buffer of
     *          length {@link #HEADER_LENGTH}.
     */
    public: inline void* getHeaderPointer () {
      return header;
    }

    public: virtual ConstPacketIterator begin () const;
    public: virtual ConstPacketIterator end () const;
    public: virtual void                gotoNextPacket (ConstPacketIterator &pi) const;
    public: virtual BasicVRTPacket*     getThisPacket (ConstPacketIterator &pi, bool skip)
                                          const __attribute__((warn_unused_result));

    /** <i>Optional functionality:</i> Appends a VRTPacket to the end of the file.
     *  @param p The packet to append.
     *  @throws UnsupportedOperationException If this method is not supported
     */
    public: virtual void append (BasicVRTPacket &p);

    /** Flushes this file by writing any buffered output to the underlying stream. If no "flushing"
     *  is required (e.g. for a read-only file), invoking this method has no effect.
     */
    public: inline void flush () {
      flush(true);
    }

    /** Closes this file and releases any system resources associated with it. If the file is already
     *  closed then invoking this method has no effect.
     */
    public: virtual void close ();

    /** Opens the file. Overriding classes should call <tt>super.open()</tt> once
     *  the file has been opened in order to ensure the header fields have been
     *  read in (if applicable) and initialized.
     */
    protected: virtual void open ();

    /** Reads the header in. */
    protected: virtual void readHeader ();

    /** The very basic frame validity checks. */
    private: bool isFileValid0 () const;

    /** Computes the CRC for the frame, but does not insert it into the frame. */
    private: int32_t computeCRC () const;

    /** Sets the version, but does not write header to disk.
     *  @param ver The new version.
     *  @return true if header needs to be written to disk; false if no changes made.
     */
    private: bool _setVersion (int32_t ver);

    /** Gets the file length from the O/S or -1 if unknown. */
    protected: virtual int64_t getFileLengthOS () const = 0;

    /** Gets the read/write length. This is the index of the last octet that has
     *  been read from or written to the file. This may either be the maximum
     *  value of the read/write pointer *OR* the size of the file if such is known
     *  prior to reading/writing.
     */
    protected: virtual int64_t getFileLengthRW () const = 0;

    /** Gets the file length from the header (0 if unspecified). */
    protected: inline int64_t getFileLengthHeader () const {
      return hdrFileLength;
    }

    /** Sets the file length in the header (0 if unspecified).
     *  @param len The new length value (in octets).
     */
    protected: inline void setFileLengthHeader (int64_t len) {
      if (_setFileLengthHeader(len)) {
        writeHeader();
      }
    }

    /** Sets the file length in the header, but does not write header to disk.
     *  @param len The new length value (in octets).
     *  @return true if header needs to be written to disk; false if no changes made.
     */
    private: bool _setFileLengthHeader (int64_t len);

    /** Sets the CRC in the header.
     *  @param crc The CRC or {@link BasicVRLFrame#NO_CRC} if no crc is being used.
     */
    protected: inline void setCRC (int32_t crc) {
      if (_setCRC(crc)) {
        writeHeader();
      }
    }

    /** Sets the CRC in the header, but does not write header to disk.
     *  @param crc The CRC or {@link #NO_CRC} if no crc is being used.
     *  @return true if header needs to be written to disk; false if no changes made.
     */
    private: bool _setCRC (int32_t crc);

    /** Write the header to disk. */
    protected: virtual void writeHeader ();

    /** Flush the content to the disk. Overriding classes should call
     *  <tt>super.flush(true)</tt> if they wish to make sure that the FileLength
     *  and CRC get updated.
     *  @param force Force contents to be flushed, otherwise skip for now.
     */
    protected: virtual void flush (bool force);

    /** Reads from the file.
     *  @param off  File offset at which to begin reading.
     *  @param ptr  Pointer to the buffer to hold the data read in.
     *  @param len  The maximum number of octets to read.
     *  @return The number of octets actually read in.
     *  @throws VRTException If the file is write-only.
     */
    protected: virtual int32_t read (int64_t off, void *ptr, int32_t len) const = 0;

    /** Writes to the file.
     *  @param off  File offset at which to begin writing (EOF to write at end of file).
     *  @param ptr  Pointer to the buffer containing the data to write.
     *  @param len  The number of octets to write.
     *  @throws VRTException If the file is read-only.
     */
    protected: inline void write (int64_t off, void *ptr, int32_t len) {
      write(off, ptr, len, true);
    }

    /** Writes to the file. This should call <tt>flush(doFlush)</tt> following the write.
     *  @param off   File offset at which to begin writing.
     *  @param ptr   Pointer to the buffer containing the data to write.
     *  @param len   The number of octets to write.
     *  @param flush Flush to disk? Setting this to false disables header updates
     *               and flushing to disk, even if required.
     *  @throws VRTException If the file is read-only.
     */
    protected: virtual void write (int64_t off, void *ptr, int32_t len, bool flush) = 0;
  };
} END_NAMESPACE
#endif /* _AbstractVRAFile_h */

/*
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

#ifndef _BasicVRAFile_h
#define _BasicVRAFile_h
#include "AbstractVRAFile.h"


namespace vrt {
  /** The file access mode. These modes are based on those permitted for a Java
   *  <tt>RandomAccessFile</tt> and <tt>fopen(..)</tt> in C/C++.
   *  <pre>
   *    FileMode           | Java | C++ | Description
   *    -------------------+------+-----+-----------------------------------------
   *    Read               | r    | rb  | Open for reading only
   *    -------------------+------+-----+-----------------------------------------
   *    ReadWrite          | rw   | rb+ | Open for reading and writing
   *                       |      |     | (keep existing data)
   *    -------------------+------+-----+-----------------------------------------
   *    Write              | rw   | wb+ | Open for reading and writing
   *                       | [1]  | [2] | (delete existing data)
   *    -------------------+------+-----+-----------------------------------------
   *    ReadWriteSynchAll  | rws  | rb+ | Same as ReadWrite/Write but with
   *     or WriteSynchAll  | [1]  | wb+ | synchronous writing of content and
   *                       |      |     | metadata [3]
   *    -------------------+------+-----+-----------------------------------------
   *    ReadWriteSynchData | rwd  | rb+ | Same as ReadWrite/Write but with
   *     or WriteSynchData | [1]  | wb+ | synchronous writing of content only
   *                       |      |     | (not synchronous for metadata) [3]
   *    -------------------+------+-----+-----------------------------------------
   *
   *    Notes:
   *    [1] The Java "Write" and "ReadWrite" modes share a mode string, and
   *        implement the the "delete existing data" by truncating the file to
   *        zero length immediately after opening.
   *    [2] The C++ write-only ("wb") mode is not explicitly supported since
   *        it would prevent the (optional) setting of the CRC and FileSize
   *        fields in the header. Nor is there explicit support for the C++
   *        append modes ("ab" or "ab+") since they are identical to opening
   *        in ReadWrite mode and then using the append(..) function.
   *    [3] In C++ synchronous writes are implemented via calls to fflush(..)
   *        and fsync(..) or fdatasync(..) following any writes and may not be
   *        available on all systems. <b>In both Java and C++ this feature is
   *        extremely costly since it prevents caching of writes in the O/S or
   *        VM (use only where required).</b>
   *    [4] The C++ modes include the "b" flag indicating binary (as opposed
   *        to text) output is being used. This flag is included for strict
   *        compatibility with the C++ standards although most POSIX systems
   *        ignore it.
   *  </pre>
   */
  enum FileMode {
    /** Open for reading only. */
    FileMode_Read                      = 0x01,
    /** Open for reading and writing (keep existing data). */
    FileMode_ReadWrite                 = 0x03,
    /** Open for reading and writing (delete existing data). */
    FileMode_Write                     = 0x02,
    /** Open for reading and writing (keep existing data); and require updates
     *  to file's content or medatada to be done synchronously.
     */
    FileMode_ReadWriteSynchAll         = 0x33,
    /** Open for reading and writing (keep existing data); and require updates
     *  to file's data (but not medatada) to be done synchronously.
     */
    FileMode_ReadWriteSynchData        = 0x13,
    /** Open for reading and writing (keep existing data); and require updates
     *  to file's content or medatada to be done synchronously. <i>For the
     *  purposes of this usage "content" includes everything within a VRA file
     *  (including the VRA file header) and "metadata" means only the O/S-level
     *  metadata (e.g. update time).</i>
     */
    FileMode_WriteSynchAll             = 0x32,
    /** Open for reading and writing (keep existing data); and require updates
     *  to file's data (but not medatada) to be done synchronously. <i>For the
     *  purposes of this usage "content" includes everything within a VRA file
     *  (including the VRA file header) and "metadata" means only the O/S-level
     *  metadata (e.g. update time).</i>
     */
    FileMode_WriteSynchData            = 0x12
  };
  
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
   *
   *  @author 
   */
  class BasicVRAFile : public AbstractVRAFile {
    private: string    fname; // The file name (not in URI form)
    private: FILE     *file;  // The underlying file
    private: FileMode  mode;  // The underlying mode
    
    
    
    /** Basic destructor for the class. */
    public: ~BasicVRAFile () { }

    /** Basic copy constructor for the class. */
    public: BasicVRAFile (const BasicVRAFile &f);
    
    /** Creates a new instance for a local file on disk.
     *  @param fname      The file name.
     *  @param fmode      The mode to use when opening the file.
     *  @param isSetSize  Should the size be set on write? (ignored if read-only)
     *  @param isSetCRC   Should the CRC  be set on write? (ignored if read-only)
     *  @param isStrict   Should strict packet checks be used on write?
     */
    public: BasicVRAFile (string fname, FileMode fmode, bool isSetSize=false,
                          bool isSetCRC=false, bool isStrict=false);
    

    protected: virtual void    open ();
    public:    virtual void    close ();
    protected: virtual void    flush (bool force);
    protected: virtual int64_t getFileLengthOS () const;
    protected: virtual int64_t getFileLengthRW () const;
    protected: virtual int32_t read (int64_t off, void *ptr, int32_t len) const;
    protected: virtual void    write (int64_t off, void *ptr, int32_t len, bool flush);
  };
};

/** Supports appending a {@link FileMode} value to a string. */
ostream& operator<< (ostream &s, vrt::FileMode mode);

/** Supports appending a {@link FileMode} value to an output stream. */
string   operator+  (string  &s, vrt::FileMode mode);

#endif /* _BasicVRAFile_h */

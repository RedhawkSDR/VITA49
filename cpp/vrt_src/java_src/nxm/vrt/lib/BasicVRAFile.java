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

import java.io.File;
import java.io.IOException;
import java.io.RandomAccessFile;

/** Defines a VRA file that is readable on the local disk using {@link RandomAccessFile}.
 *
 *  @author 
 */
public final class BasicVRAFile extends AbstractVRAFile {
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
  public static enum FileMode {
    /** Open for reading only. */
    Read("r", false, false, false),
    /** Open for reading and writing (keep existing data). */
    ReadWrite("rw", false, false, false),
    /** Open for reading and writing (delete existing data). */
    Write("rw", true, false, false),
    /** Open for reading and writing (keep existing data); and require updates
     *  to file's content or medatada to be done synchronously.
     */
    ReadWriteSynchAll("rws", false, true, true),
    /** Open for reading and writing (keep existing data); and require updates
     *  to file's data (but not medatada) to be done synchronously.
     */
    ReadWriteSynchData("rwd", false, false, true),
    /** Open for reading and writing (keep existing data); and require updates
     *  to file's content or medatada to be done synchronously. <i>For the
     *  purposes of this usage "content" includes everything within a VRA file
     *  (including the VRA file header) and "metadata" means only the O/S-level
     *  metadata (e.g. update time).</i>
     */
    WriteSynchAll("rws", true, true, true),
    /** Open for reading and writing (keep existing data); and require updates
     *  to file's data (but not medatada) to be done synchronously. <i>For the
     *  purposes of this usage "content" includes everything within a VRA file
     *  (including the VRA file header) and "metadata" means only the O/S-level
     *  metadata (e.g. update time).</i>
     */
    WriteSynchData("rwd", true, false, true);
    
    private final String  rafMode;     // Mode string for RandomAccessFile
    private final boolean isWrite;     // Is a write mode?
    private final boolean isTruncate;  // Is a truncate mode?
    private final boolean isSynchMeta; // Is a synch-meta mode?
    private final boolean isSynchData; // Is a synch-data mode?
    
    private FileMode (String mode, boolean truncate, boolean isSynchMeta, boolean isSynchData) {
      this.rafMode     = mode;
      this.isWrite     = mode.contains("w");
      this.isTruncate  = truncate;
      this.isSynchMeta = isSynchMeta;
      this.isSynchData = isSynchData;
    }
  };
  
  
  private final RandomAccessFile file;     // The underlying file
  private final FileMode         mode;     // The underlying mode
  private       boolean          isClosed; // Has the file been closed?

  /** Creates a new instance using a {@link RandomAccessFile}. This is identical
   *  to <tt>BasicVRAFile(fname,fmode,false,false,false)</tt>.
   *  @param fname The file name.
   *  @param fmode The mode to use when opening the file.
   */
  public BasicVRAFile (CharSequence fname, FileMode fmode) {
    this(fname,fmode,false,false,false);
  }

  /** Creates a new instance using a {@link RandomAccessFile}. This is identical
   *  to <tt>BasicVRAFile(fname,fmode,false,false,false)</tt>.
   *  @param fname The file name.
   *  @param fmode The mode to use when opening the file.
   */
  public BasicVRAFile (File fname, FileMode fmode) {
    this(fname,fmode,false,false,false);
  }

  /** Creates a new instance using a {@link RandomAccessFile}. Note that any
   *  file opened for writing is implicitly opened for reading too.
   *  @param fname      The file name.
   *  @param fmode      The mode to use when opening the file.
   *  @param isSetSize  Should the size be set on write? (ignored if read-only)
   *  @param isSetCRC   Should the CRC  be set on write? (ignored if read-only)
   *  @param isStrict   Should strict packet checks be used on write?
   */
  public BasicVRAFile (CharSequence fname, FileMode fmode, boolean isSetSize,
                                           boolean isSetCRC, boolean isStrict) {
    this(new File(fname.toString()), fmode, isSetSize, isSetCRC, isStrict);
  }

  /** Creates a new instance using a {@link RandomAccessFile}. Note that any
   *  file opened for writing is implicitly opened for reading too.
   *  @param fname      The file name.
   *  @param fmode      The mode to use when opening the file.
   *  @param isSetSize  Should the size be set on write? (ignored if read-only)
   *  @param isSetCRC   Should the CRC  be set on write? (ignored if read-only)
   *  @param isStrict   Should strict packet checks be used on write?
   */
  public BasicVRAFile (File fname, FileMode fmode, boolean isSetSize,
                                   boolean isSetCRC, boolean isStrict) {
    super(fname.toURI(), true, fmode.isWrite, isSetSize, isSetCRC, isStrict);
    
    try {
      this.file     = new RandomAccessFile(fname, fmode.rafMode);
      this.mode     = fmode;
      this.isClosed = false;
      
      if (mode.isTruncate) {
        // Truncate the file on opening
        file.setLength(0L);
      }
      open();
    }
    catch (IOException e) {
      throw new RWException(e);
    }
  }

  @Override
  protected void open () {
    // In Java the file is automatically opened by the RandomAccessFile
    // constructor so there is nothing to do here but call super.open().
    super.open();
  }
  
  @Override
  public void close () {
    if (isClosed) return;
    try {
      super.close();
      file.close();
      isClosed = true;
    }
    catch (IOException e) {
      throw new RWException(e);
    }
  }

  @Override
  protected void flush (boolean force) {
    force = force || mode.isSynchData; // All SynchMeta also SynchData
    super.flush(force);
  }
  
  @Override
  protected long getFileLengthOS () {
    try {
      return file.length();
    }
    catch (IOException e) {
      throw new RWException(e);
    }
  }
  
  @Override
  protected long getFileLengthRW () {
    return getFileLengthOS();
  }
  
  @Override
  protected int read (long off, byte[] buf, int boff, int len) {
    try {
      if (off == EOF) {
        off = file.length();
      }
      file.seek(off);
      return file.read(buf, boff, len);
    }
    catch (IOException e) {
      throw new RWException(e);
    }
  }
  
  @Override
  protected void write (long off, byte[] buf, int boff, int len, boolean flush) {
    if (!isWrite) throw new UnsupportedOperationException("File is read-only");
    try {
      if (off == EOF) {
        off = file.length();
      }
      file.seek(off);
      file.write(buf, boff, len);
      if (flush) flush(false);
    }
    catch (IOException e) {
      throw new RWException(e);
    }
  }
}

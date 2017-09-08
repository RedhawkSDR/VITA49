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

#include "BasicVRAFile.h"
#include <limits.h>   // for realpath(..)
#include <stdio.h>    // for fopen(..) / fclose(..) / et.al.
#include <stdlib.h>   // for realpath(..)
#include <unistd.h>   // for getcwd(..)
#include <errno.h>    // Required when using errno / ERRNO_STR

using namespace vrt;

// FileMode bit fields
#define FileMode_READ       0x01
#define FileMode_WRITE      0x02
#define FileMode_SYNCH_DATA 0x10
#define FileMode_SYNCH_META 0x20

#ifndef PATH_MAX
// Per the realpath(..) man page, not all systems have PATH_MAX defined. If this
// is the case, just issue a warning and use a max of 4096. (The warning may be
// removed in the future if this gets a lot of testing and seems fine.)
#  warning "Using PATH_MAX=4096"
#  define PATH_MAX 4096
#endif

/** Seeks to a position in the file with special handling for EOF. */
static inline void fileSeek (FILE *file, string fname, int64_t off) {
  if (off >= 0) {
    if (fseek(file, off, SEEK_SET) != 0) {
      throw VRTException("Unable to seek to %" PRId64 " in %s: %s", off, fname.c_str(), ERRNO_STR);
    }
  }
  else if (off == EOF) {
    if (fseek(file, __INT64_C(0), SEEK_END) != 0) {
      throw VRTException("Unable to seek to EOF in %s: %s", off, fname.c_str(), ERRNO_STR);
    }
  }
  else {
    throw VRTException("Unable to seek to %" PRId64 " in %s: %s", off, fname.c_str(), "Invalid offset");
  }
}

/** Supports appending a {@link FileMode} value to an output stream. */
static inline string _toString (vrt::FileMode mode) {
  switch (mode) {
    case FileMode_Read:               return "Read";
    case FileMode_ReadWrite:          return "ReadWrite";
    case FileMode_Write:              return "Write";
    case FileMode_ReadWriteSynchAll:  return "ReadWriteSynchAll";
    case FileMode_ReadWriteSynchData: return "ReadWriteSynchData";
    case FileMode_WriteSynchAll:      return "WriteSynchAll";
    case FileMode_WriteSynchData:     return "WriteSynchData";
    default: return Utilities::format("Unknown FileMode (%d)", (int32_t)mode);
  }
}

/** Converts FileMode to applicable fopen(..) flags. */
static inline const char* getOpenFlags (FileMode mode) {
  switch (mode) {
    case FileMode_Read:               return "rb";
    case FileMode_ReadWrite:          return "rb+";
    case FileMode_Write:              return "wb+";
    case FileMode_ReadWriteSynchAll:  return "rb+";
    case FileMode_ReadWriteSynchData: return "rb+";
    case FileMode_WriteSynchAll:      return "wb+";
    case FileMode_WriteSynchData:     return "wb+";
    default: throw VRTException(string("Unknown FileMode ")+_toString(mode));
  }
}

/** Supports appending a {@link FileMode} value to a string. */
ostream& operator<< (ostream &s, vrt::FileMode mode) {
  return s << _toString(mode);
}

/** Supports appending a {@link FileMode} value to an output stream. */
string operator+  (string  &s, vrt::FileMode mode) {
  return s + _toString(mode);
}

/** Converts file name to URI. */
static string toURI (string fname) {
  if (isNull(fname)) {
    throw VRTException("Invalid use of null file name ''");
  }
  
  // Get absolute path
#if defined(_WIN32)
# warning "Windows version of toURI(..) is untested"
  // Get absolute path
  if ((fname[0] != '/') && (fname[0] != '\\') && ((fname.size() < 3) || (fname[1] != ':')) {
    char cwd[PATH_MAX];
    
    if (getcwd(cwd, PATH_MAX) == NULL) {
      throw VRTException("Unable to get CWD: %s", ERRNO_STR);
    }
    fname = (fname[0] != '/')? string(cwd) + '/' + fname
                             : string(cwd) + '\\' + fname;
  }
  return string("file:") + fname;
#else
  // Get absolute path
  if (fname[0] != '/') {
    char cwd[PATH_MAX];
    
    if (getcwd(cwd, PATH_MAX) == NULL) {
      throw VRTException("Unable to get CWD: %s", ERRNO_STR);
    }
    fname = string(cwd) + '/' + fname;
  }
  
  // Get the canonical path (if possible)
# if defined(_BSD_SOURCE) || (_XOPEN_SOURCE >= 500)
  char *resolved = realpath(fname.c_str(), NULL);
  
  if (resolved == NULL) {
    throw VRTException("Unable to get real path for '%s': %s", fname.c_str(), ERRNO_STR);
  }
  fname = string(resolved);
  free(resolved);
# endif

  return string("file://") + fname;
#endif
}


BasicVRAFile::BasicVRAFile (string fname, FileMode fmode, bool isSetSize, bool isSetCRC, bool isStrict) :
  AbstractVRAFile(toURI(fname), ((fmode & FileMode_READ) != 0), ((fmode & FileMode_WRITE) != 0),
                  isSetSize, isSetCRC, isStrict),
  fname(fname),
  file(NULL),
  mode(fmode)
{
  open();
}


void BasicVRAFile::open () {
#if (_POSIX_SYNCHRONIZED_IO > 0)  || (defined(__APPLE__) && defined(__MACH__))
  // Use of the synch flags is OK.
#else
  // No support for the synch flags. This isn't strictly true, the check here
  // just verifies support for fdatasync(..) (per man page), so we could add
  // a limited capability (e.g. using fsync(..) for any synch operation, when
  // available) in the future.
# warning "No support for synchronized file I/O, use of BasicVRAFile restricted \
           to Read/Write/ReadWrite."
  if (((mode & FileMode_SYNCH_DATA) != 0) || ((mode & FileMode_SYNCH_META) != 0)) {
    throw VRTException(string("Use of synchronized file I/O is disabled, use of "
                              "BasicVRAFile restricted to Read/Write/ReadWrite, "
                              "given ")+_toString(mode)+" for use with "+fname);
  }
#endif
  
  file = fopen(fname.c_str(), getOpenFlags(mode));
  if (file == NULL) {
    throw VRTException("Unable to open %s: %s", fname.c_str(), ERRNO_STR);
  }
  AbstractVRAFile::open();
}


void BasicVRAFile::close () {
  if (file != NULL) {
    AbstractVRAFile::close();
    if (fclose(file) != 0) {
      throw VRTException("Unable to close %s: %s", fname.c_str(), ERRNO_STR);
    }
    file = NULL;
  }
}

void BasicVRAFile::flush (bool force) {
  force = force || ((mode & FileMode_SYNCH_DATA) != 0); // All SYNCH_META also SYNCH_DATA
  AbstractVRAFile::flush(force);
  if (force) {
    if (fflush(file) != 0) {
      throw VRTException("Unable to flush %s: %s", fname.c_str(), ERRNO_STR);
    }
    
#if (_POSIX_SYNCHRONIZED_IO > 0)
    if (mode & FileMode_SYNCH_META) {
      int f = fileno(file);
      if ((f == -1) || (fsync(f) != 0)) {
        throw VRTException("Unable to synch data+metadata for %s: %s", fname.c_str(), ERRNO_STR);
      }
    }
    else if (mode & FileMode_SYNCH_DATA) {
      int f = fileno(file);
      if ((f == -1) || (fdatasync(f) != 0)) {
        throw VRTException("Unable to synch data for %s: %s", fname.c_str(), ERRNO_STR);
      }
    }
#elif (defined(__APPLE__) && defined(__MACH__))
    if ((mode & FileMode_SYNCH_META) || (mode & FileMode_SYNCH_DATA)) {
      int f = fileno(file);
      if ((f == -1) || (fsync(f) != 0)) {
        throw VRTException("Unable to synch data+metadata for %s: %s", fname.c_str(), ERRNO_STR);
      }
    }
#endif
  }
}

int64_t BasicVRAFile::getFileLengthOS () const {
  fileSeek(file, fname, EOF);
  int64_t off = ftell(file);
  if (off < 0) {
    throw VRTException(errno);
  }
  return off;
}

int64_t BasicVRAFile::getFileLengthRW () const {
  return getFileLengthOS();
}

int32_t BasicVRAFile::read (int64_t off, void *ptr, int32_t len) const {
  if (ptr == NULL) throw VRTException("Can not read from %s to NULL buffer", fname.c_str());
  if (len <     0) throw VRTException("Can not read %d octets from %s", len, fname.c_str());
  if (len ==    0) return 0;
  
  fileSeek(file, fname, off);
  size_t count = fread(ptr, sizeof(char), len, file); // <-- Returns N<=0 on Error *or* EOF
  if (count > 0) {
    return (int32_t)count;
  }
  else if (feof(file)) {
    clearerr(file);
    return EOF;
  }
  else {
    clearerr(file);
    throw VRTException("Error while reading from %s", fname.c_str());
  }
}

void BasicVRAFile::write (int64_t off, void *ptr, int32_t len, bool flush) {
  if (!isWrite   ) throw VRTException("File is read-only");
  if (ptr == NULL) throw VRTException("Can not read from %s to NULL buffer", fname.c_str());
  if (len <     0) throw VRTException("Can not read %d octets from %s", len, fname.c_str());
  if (len ==    0) return;
  
  fileSeek(file, fname, off);
  size_t count = fwrite(ptr, sizeof(char), len, file); // <-- Returns N<=0 on Error *or* EOF
  if (((int64_t)count) == len) {
    this->flush(flush);
    return;
  }
  else if (feof(file)) {
    // This doesn't make much sense, so use an alternate error message so we can
    // differentiate it if we ever see it.
    clearerr(file);
    throw VRTException("EOF error while writing to %s", fname.c_str());
  }
  else {
    clearerr(file);
    throw VRTException("Error while writing to %s", fname.c_str());
  }
}

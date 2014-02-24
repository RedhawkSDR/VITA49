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

#ifndef _MetadataBlock_h
#define _MetadataBlock_h

#include "VRTObject.h"
#include "VRTMath.h"
#include <vector>
#include "string.h"    // required for memcpy(..) on GCC4.4/libc6 2.11.1

namespace vrt {
  /**  A metadata entry in the metadata block. */
  class MetadataEntry : public VRTObject {
    private: int32_t  level;   // The level
    private: string   key;     // The key
    private: wstring  val;     // The value
    private: bool     keyNull; // Is the key null?
    private: bool     valNull; // Is the val null?

    /**  Basic destructor. */
    public: ~MetadataEntry () { }
    
    /**  Copy constructor. */
    public: MetadataEntry (const MetadataEntry &m);

    /**  Creates a new uninitialized (null) instance. */
    public: MetadataEntry ();

    /**  Creates a new instance with the given entries.
       @param level The level.
       @param key   The key.
       @param val   The value.
     */
    public: MetadataEntry (int32_t level, const string &key, const wstring &val);

    /**  Prints the entry in string form. */
    public: virtual string toString () const;
    
    /**  Is this a null-form packet? */
    public: virtual inline bool isNull () const {
      return (level == -1);
    }

    /**  Gets the level of the entry. */
    public: inline int32_t getLevel () const {
      return level;
    }

    /**  Sets the level of the entry. */
    public: inline void setLevel (int32_t level) {
      if ((level < 0) || (level > 4095)) {
        throw VRTException("Invalid level (%d), value must be in range [0,4095]", level);
      }
      this->level = level;
    }

    /**  Gets the key name. */
    public: inline string getKey () const {
      return key;
    }

    /**  Sets the key name. */
    public: void setKey (const string *key);

    /**  Sets the key name. */
    public: inline void setKey (const string &key) {
      setKey(&key);
    }

    /**  Gets the value. */
    public: inline wstring getValue () const {
      return val;
    }

    /**  Sets the value. */
    public: void setValue (const wstring *val);

    /**  Sets the value. */
    public: inline void setValue (const wstring &val) {
      setValue(&val);
    }

    /**  Writes the entry to a byte buffer.
        @param buf  The buffer.
        @param off  Offset into the buffer.
        @param len  Length in the byte buffer.
        @return The number of bytes required within the byte buffer. If there are insufficient bytes
                to include the entire metadata block this will return a number larger than the length
                passed in.
     */
    public: int32_t writeBytes (vector<char> &buf, int32_t off, int32_t len) const;


    /**  Reads the entry from a byte buffer.
        @param buf  The buffer.
        @param off  Offset into the buffer.
        @param len  Length in the byte buffer.
        @throws IllegalArgumentException If the block is invalid.
     */
    public: int32_t readBytes (const vector<char> &buf, int32_t off, int32_t len);
  };
  
  /**  Class to support the Binary Metadata Language. Each entry has three pieces of information:
      <ul>
        <li>LEVEL - The level applicable to the entry (0 if n/a) (unsigned integer 0..4095).</li>
        <li>KEY   - The key or tag for the entry. (ASCII 0..32767 characters or NULL)</li>
        <li>VALUE - The key or tag for the entry. (UTF-8 0..2147483647 characters or NULL)
            <i>(Most implementations limit the value length to 32767)</i></li>
      </ul>
       Each entry has a 2-/4-/8-octet header describing the entry followed by the octets comprising
      the KEY followed by the octets comprising the VALUE.
      <pre>
         NULL FORM (1-byte)
           +-----------------+
           | 0 0 0 0 0 0 0 0 | (no data, just an ignored byte, typically at the end of a field)
           +-----------------+

         SHORT FORM (2-byte)
           +-----------------+-----------------+
           | 1 0 N N K K K K | L L L L L L L L |
           +-----------------+-----------------+

         STANDARD FORM (4-byte)
           +-----------------+-----------------+-----------------+-----------------+
           | 1 1 0 N N N N N | K K K K K K K K | L L L L L L L L | L L L L L L L L |
           +-----------------+-----------------+-----------------+-----------------+

         LONG FORM (8-byte)
           +-----------------+-----------------+-----------------+-----------------+
           | 1 1 1 0 N N N N | N N N N N N N N | 0 K K K K K K K | K K K K K K K K |
           +-----------------+-----------------+-----------------+-----------------+

           +-----------------+-----------------+-----------------+-----------------+
           | 0 L L L L L L L | L L L L L L L L | L L L L L L L L | L L L L L L L L |
           +-----------------+-----------------+-----------------+-----------------+

         0 - Zero (required)
         1 - One (required)
         N - Level
         K - Key Length
         L - Total Length
      </pre>
       The length of the KEY shall represent the number of octets reserved for holding the
      NUL-terminated KEY. A non-null key may contain mode octets than the minimum required. The null
      value shall be indicated by a KEY length of 0. An empty string shall be encoded by a KEY length
      of at least one where the first octet is the NUL terminator. <br>
      <br>
       The length of the VALUE shall be computed by subtracting the length of the KEY and the
      length of the header from the total length. The VALUE shall be NUL-terminated where a VALUE with
      length 0 shall represent the null value and an empty string shall be encoded with a length
      of at least 1 where the first octet is the NUL terminator.

      @author         
   */
  class MetadataBlock : public VRTObject {
    private: vector<MetadataEntry> entries;

    /**  Basic destructor. */
    public: ~MetadataBlock () { }

    /**  Basic copy constructor. */
    public: MetadataBlock (const MetadataBlock &m);

    /**  Creates a new uninitialized instance. */
    public: MetadataBlock ();

    /**  Creates a new instance with the given entries. */
    public: MetadataBlock (const vector<MetadataEntry> &entries);

    /**  Prints the entry in string form. */
    public: virtual string toString () const;

    /**  Reports that the block is null if it has no entries. */
    public: virtual inline bool isNull () const {
      return (entries.size() == 0);
    }

    /**  Gets the length of the metadata block in bytes. This method is intended for use during
        space allocation, frequent use of this method is discouraged since this method must assess
        the length of each metadata element and that requires scanning through the metadata values
        and assessing the number of bytes required when packing as UTF-8.
     */
    public: int32_t getLengthInBytes () const;

    /**  Gets a copy of the entries in the metadata block. */
    public: inline vector<MetadataEntry> getEntries () const {
      return entries;
    }

    /**  Gets a pointer to the entries in the metadata block. */
    public: inline vector<MetadataEntry> *getEntriesPointer () {
      return &entries;
    }

    /**  Sets the metadata block to match the specified entries. */
    public: inline void setEntries (const vector<MetadataEntry> &entries) {
      this->entries = entries;
    }

    /**  Reads the entries from a byte buffer.
        @param buf      The buffer.
        @param off      Offset into the buffer.
        @param len      Length in the byte buffer.
        @param keepNull Keep null entries? (This is usually false.)
        @throws IllegalArgumentException If the block is invalid.
     */
    public: void readBytes (const vector<char> &buf, int32_t off, int32_t len, bool keepNull=false);

    /**  Writes the entries to a byte buffer.
        @param buf  The buffer.
        @param off  Offset into the buffer.
        @param len  Length in the byte buffer.
        @return The number of bytes required within the byte buffer. If there are insufficient bytes
                to include the entire metadata block this will return a number larger than the length
                passed in.
     */
    public: int32_t writeBytes (vector<char> &buf, int32_t off, int32_t len) const;
  };

  // See VRTMath.c for function code
  namespace VRTMath {
    /**  Pack a metadata block into a buffer.
        @param buf   byte array of data
        @param off   Offset into array
        @param val   The metadata block to pack
        @param len   The length of the block
     */
    int32_t packMetadata (vector<char> &buf, int32_t off, const MetadataBlock &val, int32_t len);

    /**  Unpack a metadata block into a buffer.
        @param buf   byte array of data
        @param off   Offset into array
        @param len   The length of the block
     */
    MetadataBlock unpackMetadata (const vector<char> &buf, int32_t off, int32_t len);
  };
};
#endif /* _MetadataBlock_h */

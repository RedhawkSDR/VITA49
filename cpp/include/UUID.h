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

#ifndef _UUID_h
#define _UUID_h

#include "VRTObject.h"
#include "HasFields.h"
#include <string.h>    // required for memcpy(..) on GCC4.4/libc6 2.11.1

#ifndef INTERNAL_UUID_LIB
# define INTERNAL_UUID_LIB 0  // see comments at top of UUID class
#endif

#if INTERNAL_UUID_LIB == 0
# include <uuid/uuid.h>
# ifndef _UUID_UUID_H
#  error *******************************************************
#  error * Could not find system UUID library, try building with
#  error * INTERNAL_UUID_LIB=1 or INTERNAL_UUID_LIB=2
#  error *******************************************************
# endif
#else
  typedef unsigned char uuid_t[16];
#endif

namespace vrt {
  /** A 128-bit universally unique identifier (UUID). This provides an OO wrapper for the
   *  <tt>uuid_t</tt> value defined in the <tt>uuid/uuid.h</tt> library that allows its inclusion
   *  in VRT packets similar to other values.
   *  <pre>
   *    By default the standard UUID library (-luuid) will be used when working with
   *    UUIDs in the C++ code. Since some platforms do not have this library installed,
   *    there is a mini UUID library internal to the VRT C++ code. (The internal library
   *    supports the minimum set of functions required here and is not intended for use
   *    by external users.) The internal library has two versions: the second version
   *    supports generation of RFC 4122-compliant UUIDs using gcrypt as a source of
   *    secure-random numbers; the first version does not depend on gcrypt but does
   *    not support generation of UUIDs (use the first version only as a last resort).
   *      Standard UUID library:            setenv INTERNAL_UUID_LIB 0
   *      Internal UUID library (no gen):   setenv INTERNAL_UUID_LIB 1
   *      Internal UUID library (random):   setenv INTERNAL_UUID_LIB 2
   *  </pre>
   */
  class UUID : public VRTObject, public HasFields {
    /** The 128-bit UUID. */
    private: uuid_t uu; // uuid_t is a typedef for a char[16]

    /** Destructor. */
    public: ~UUID () { }

    /** Basic no-argument constructor. */
    public: UUID ();

    /** Copy constructor. */
    public: UUID (const UUID &uuid);

    /** Copy constructor. */
    public: UUID (const uuid_t uuid);

#if INTERNAL_UUID_LIB != 1
    /** Generates a random UUID. */
    public: static UUID randomUUID ();
#endif

    /** A UUID is considered null if all bits are set to 0 (this is not a valid UUID). */
    public: virtual bool isNullValue () const;

    using VRTObject::equals;
    public: virtual bool equals (const VRTObject &o) const;

    /** Checks to see if two UUIDs are equal. */
    public: virtual bool equals (const UUID &uuid) const;

    /** Converts the UUID to a string. */
    public: inline virtual string toString () const {
      return getUUID();
    }

    /** Gets a pointer to the UUID value. */
    public: uuid_t &getUUIDPointer() {
      return uu;
    }

    /** Gets the 16-byte UUID value into the given buffer. */
    public: inline void getValue (void *buf) const {
      memcpy(buf, uu, 16);
    }

    /** Sets the 16-byte UUID value from the given buffer. */
    public: inline void setValue (const void *buf) {
      memcpy(uu, buf, 16);
    }

    /** Gets the 16-byte UUID value into the given buffer. */
    public: string getUUID () const;

    /** Sets the 16-byte UUID value from the given buffer. */
    public: void setUUID (const string &uuid);

    /** Sets the 16-byte UUID value from the given buffer. */
    public: void setUUID (const char *uuid);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Implement HasFields
    //////////////////////////////////////////////////////////////////////////////////////////////////
    public: virtual int32_t   getFieldCount () const;
    public: virtual string    getFieldName  (int32_t id) const;
    public: virtual ValueType getFieldType  (int32_t id) const;
    public: virtual Value*    getField      (int32_t id) const __attribute__((warn_unused_result));
    public: virtual void      setField      (int32_t id, const Value* val);
  };

  namespace VRTMath {
    /** Pack a 16-byte UUID into a buffer.
     *  @param ptr   Pointer to packed byte array [OUTPUT]
     *  @param off   Offset into array
     *  @param val   value to pack [INPUT]
     */
    inline void packUUID (void *ptr, int32_t off, const UUID &val) {
      char *buf = (char*)ptr;
      val.getValue(&buf[off]);
    }

    /** Pack a 16-byte UUID into a buffer.
     *  @param buf   byte array of data [OUTPUT]
     *  @param off   Offset into array
     *  @param val   value to pack [INPUT]
     */
    inline void packUUID (vector<char> &buf, int32_t off, const UUID &val) {
      val.getValue(&buf[off]);
    }

    /** Unpack a 16-byte UUID from a buffer.
     *  @param ptr   pointer to byte array of data
     *  @param off   Offset into array
     *  @return The unpacked value
     */
    inline UUID unpackUUID (const void *ptr, int32_t off) {
      const char *buf = (const char*)ptr;
      UUID val;
      val.setValue(&buf[off]);
      return val;
    }

    /** Unpack a 16-byte UUID from a buffer.
     *  @param buf   byte array of data
     *  @param off   Offset into array
     *  @return The unpacked value
     */
    inline UUID unpackUUID (const vector<char> &buf, int32_t off) {
      UUID val;
      val.setValue(&buf[off]);
      return val;
    }
  } END_NAMESPACE
} END_NAMESPACE
#endif /* _UUID_h */

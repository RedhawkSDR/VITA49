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

#ifndef _Record_h
#define _Record_h

#include "VRTObject.h"
#include "VRTMath.h"
#include "MetadataBlock.h"
#include "TimeStamp.h"
#include "InetAddress.h"
#include "UUID.h"
#include "HasFields.h"
#include <vector>
#include <string.h>    // required for memcpy(..) on GCC4.4/libc6 2.11.1

namespace vrt {
  /** Defines a record that could be in a {@link BasicVRTPacket}.
   *
   *  @author         
   */
  class Record : public VRTObject, public HasFields {
    /** Chack for strict equality? */
    private:   bool         strictEquality;
    /** <b>Internal Use Only:</b> The data buffer used. */
    protected: vector<char> buf;
    /** Creates a new instance sharing the buffers of the one specified. */
    public: Record (const Record &r);

    /** Creates a new instance with the given length.
        @param strictEquality Should the <tt>equals(..)</tt> check ensure strict equality by checking to
                              see that both the data and class match? If false, only the data is checked.
        @param len            The initial length of the record.
     */
    public: Record (int32_t len, bool strictEquality=true);

    /** A record is considered null if it has no data. */
    public: inline virtual bool isNullValue () const {
      for (size_t i = 0; i < buf.size(); i++) {
        if (buf[i] != 0) return false;
      }
      return true;
    }

    /** Reads bytes from this record to the specified buffer.
        @param buffer The buffer to write to.
     */
    public: inline virtual void readBytes  (void *buffer) const {
      memmove(buffer, &buf[0], buf.size());
    }

    /** Writes bytes to this record from the specified buffer.
        @param buffer The buffer to read from.
     */
    public: inline virtual void writeBytes (const void *buffer) {
      memmove(&buf[0], buffer, buf.size());
    }

    /** Gets the length in bytes. */
    public: inline virtual int32_t getByteLength () const {
      return buf.size();
    }

    /** Sets the length in bytes with any additional bytes added/removed from the given point (-1=end). */
    protected: virtual void setByteLength (int32_t len, int32_t off=-1);

    /** Gets a basic description of the object, useful for debugging.
     * @return A basic description of the object.
     */
    public: inline virtual string toString () const {
      return "";
    }

    public: virtual bool equals (const VRTObject &o) const;

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // PACK/UNPACK METHODS
    //////////////////////////////////////////////////////////////////////////////////////////////////
    /** Packs an 8-bit integer into the record. */
    protected: inline void packByte      (int32_t off, int8_t              val                   ) { buf[off] = val; }
    /** Packs a 16-bit integer into the record. */
    protected: inline void packShort     (int32_t off, int16_t             val                   ) { VRTMath::packShort(    buf, off, val); }
    /** Packs a 32-bit integer into the record. */
    protected: inline void packInt       (int32_t off, int32_t             val                   ) { VRTMath::packInt(      buf, off, val); }
    /** Packs a 64-bit integer into the record. */
    protected: inline void packLong      (int32_t off, int64_t             val                   ) { VRTMath::packLong(     buf, off, val); }
    /** Packs a 32-bit floating-point value into the record. */
    protected: inline void packFloat     (int32_t off, float               val                   ) { VRTMath::packFloat(    buf, off, val); }
    /** Packs a 64-bit floating-point value into the record. */
    protected: inline void packDouble    (int32_t off, double              val                   ) { VRTMath::packDouble(   buf, off, val); }
    /** Packs a <tt>boolNull</tt> value into the record. */
    protected: inline void packBoolNull  (int32_t off, boolNull            val                   ) { VRTMath::packBoolNull( buf, off, val); }
    /** Packs a boolean value into the record. */
    protected: inline void packBoolean   (int32_t off, bool                val                   ) { VRTMath::packBoolean(  buf, off, val); }
    /** Packs an ASCII string into the record. */
    protected: inline void packAscii     (int32_t off, string              val, int32_t len      ) { VRTMath::packAscii(    buf, off, val, len); }
    /** Packs a UTF-8 string into the record. */
    protected: inline void packUTF8      (int32_t off, string              val, int32_t len      ) { VRTMath::packUTF8(     buf, off, val, len); }
    /** Packs a UTF-8 string into the record. */
    protected: inline void packUTF8      (int32_t off, wstring             val, int32_t len      ) { VRTMath::packUTF8(     buf, off, val, len); }
    /** Packs a metadata block into the record. */
    protected: inline void packMetadata  (int32_t off, MetadataBlock       val, int32_t len      ) { VRTMath::packMetadata( buf, off, val, len); }
    /** Packs a block of bytes into the record. */
    protected: inline void packBytes     (int32_t off, const vector<char> &val, int32_t len      ) { VRTMath::packBytes(    buf, off, val, len); }
    /** Packs a block of bytes into the record. */
    protected: inline void packBytes     (int32_t off, void               *val, int32_t len      ) { VRTMath::packBytes(    buf, off, val, len); }
    /** Packs a {@link TimeStamp} into the record. */
    protected: inline void packTimeStamp (int32_t off, const TimeStamp    &val, IntegerMode epoch) { VRTMath::packTimeStamp(buf, off, val, epoch); }
    /** Packs a {@link InetAddress} into the record. */
    protected: inline void packInetAddr  (int32_t off, const InetAddress  &val                   ) { VRTMath::packInetAddr( buf, off, val); }
    /** Packs a {@link UUID} into the record. */
    protected: inline void packUUID      (int32_t off, const UUID         &val                   ) { VRTMath::packUUID(     buf, off, val); }

    /** Unpacks an 8-bit integer from the record. */
    protected: inline int8_t        unpackByte      (int32_t off                        ) const { return buf[off]; }
    /** Unpacks a 16-bit integer from the record. */
    protected: inline int16_t       unpackShort     (int32_t off                        ) const { return VRTMath::unpackShort(    buf, off); }
    /** Unpacks a 32-bit integer from the record. */
    protected: inline int32_t       unpackInt       (int32_t off                        ) const { return VRTMath::unpackInt(      buf, off); }
    /** Unpacks a 64-bit integer from the record. */
    protected: inline int64_t       unpackLong      (int32_t off                        ) const { return VRTMath::unpackLong(     buf, off); }
    /** Unpacks a 32-bit floating-point value from the record. */
    protected: inline float         unpackFloat     (int32_t off                        ) const { return VRTMath::unpackFloat(    buf, off); }
    /** Unpacks a 64-bit floating-point value from the record. */
    protected: inline double        unpackDouble    (int32_t off                        ) const { return VRTMath::unpackDouble(   buf, off); }
    /** Unpacks a <tt>boolNull</tt> value from the record. */
    protected: inline boolNull      unpackBoolNull  (int32_t off                        ) const { return VRTMath::unpackBoolNull( buf, off); }
    /** Unpacks a boolean value from the record. */
    protected: inline bool          unpackBoolean   (int32_t off                        ) const { return VRTMath::unpackBoolean(  buf, off); }
    /** Unpacks an ASCII string from the record. */
    protected: inline string        unpackAscii     (int32_t off,            int32_t len) const { return VRTMath::unpackAscii(    buf, off, len); }
    /** Unpacks a UTF-8 string from the record. */
    protected: inline wstring       unpackUTF8      (int32_t off,            int32_t len) const { return VRTMath::unpackUTF8(     buf, off, len); }
    /** Unpacks a metadata block from the record. */
    protected: inline MetadataBlock unpackMetadata  (int32_t off,            int32_t len) const { return VRTMath::unpackMetadata( buf, off, len); }
    /** Unpacks a block of bytes from the record. */
    protected: inline vector<char>  unpackBytes     (int32_t off,            int32_t len) const { return VRTMath::unpackBytes(    buf, off, len); }
    /** Unpacks a block of bytes from the record. */
    protected: inline void*         unpackBytes     (int32_t off, void *val, int32_t len) const { return VRTMath::unpackBytes(    buf, off, val, len); }
    /** Unpacks a {@link TimeStamp} from the record. */
    protected: inline TimeStamp     unpackTimeStamp (int32_t off, IntegerMode epoch     ) const { return VRTMath::unpackTimeStamp(buf, off, epoch); }
    /** Unpacks a {@link InetAddress} from the record. */
    protected: inline InetAddress   unpackInetAddr  (int32_t off                        ) const { return VRTMath::unpackInetAddr( buf, off); }
    /** Unpacks a {@link UUID} from the record. */
    protected: inline UUID          unpackUUID      (int32_t off                        ) const { return VRTMath::unpackUUID(     buf, off); }
    /** Unpacks an bit from the record. */
    protected:        bool          unpackBit       (int32_t off,            int32_t bit) const;

    /** Unpacks a state event bit from the record. */
    protected: boolNull getStateEventBit (int32_t off, int32_t enable, int32_t indicator) const;

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Implement HasFields
    //////////////////////////////////////////////////////////////////////////////////////////////////
    public: virtual int32_t   getFieldCount () const;
    public: virtual string    getFieldName  (int32_t id) const;
    public: virtual ValueType getFieldType  (int32_t id) const;
    public: virtual Value*    getField      (int32_t id) const;
    public: virtual void      setField      (int32_t id, const Value* val);
  };

  // See VRTMath.c for function code
  namespace VRTMath {
    /** Pack a record into a buffer.
        @param buf   byte array of data
        @param off   Offset into array
        @param val   The record to pack
     */
    void packRecord (vector<char> &buf, int32_t off, const Record &val);

    /** Pack a record into a buffer.
        @param buf   byte array of data
        @param off   Offset into array
        @param val   The record to pack
     */
    void unpackRecord (const vector<char> &buf, int32_t off, Record &val);
  };
};
#endif /* _Record_h */

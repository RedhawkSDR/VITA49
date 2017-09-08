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

#ifndef _InetAddress_h
#define _InetAddress_h

#include "VRTObject.h"
#include "HasFields.h"
#include <netinet/in.h>

namespace vrt {
  /** An IPv4 or IPv6 Internet address. */
  class InetAddress : public VRTObject, public HasFields {
    private: struct in_addr  ipv4;
    private: struct in6_addr ipv6;

    /** Destructor. */
    public: ~InetAddress () { }

    /** Basic no-argument constructor. */
    public: InetAddress ();

    /** Copy constructor. */
    public: InetAddress (const InetAddress &addr);

    /** Creates a new instance, using an address given as a string. */
    public: InetAddress (const string &addr);

    /** Creates a new instance, using an address given as a string. */
    public: InetAddress (const char *addr);

    /** Creates a new instance, using the specified address from a buffer. If <tt>off</tt> is 0
     *  and the vector size is 4, then a 4-byte IPv4 address is used, otherwise a 16-byte IPv6
     *  address is assumed.
     *  @param addr The given address.
     *  @param off  The offset into the array.
     */
    public: InetAddress (const vector<char> &addr, size_t off=0);

    /** Creates a new instance, using the specified address from a buffer. If <tt>off</tt> is 0
     *  and the vector size is 4, then a 4-byte IPv4 address is used, otherwise a 16-byte IPv6
     *  address is assumed.
     *  @param addr The given address.
     *  @param off  The offset into the array.
     *  @param len  The length of the address (4=IPv4, 16=IPv6).
     */
    public: InetAddress (const void *addr, size_t off, size_t len);

    /** Creates a new instance, using the specified address.
     *  @param addr The given address.
     */
    public: InetAddress (const struct in_addr &addr);

    /** Creates a new instance, using the specified address.
     *  @param addr The given address.
     */
    public: InetAddress (const struct in6_addr &addr);

    /** An address is considered null if it is set to 0.0.0.0 or ::0. */
    public: virtual bool isNullValue () const;

    /** Is this an IPv4 address? */
    public: virtual bool isIPv4 () const;

    /** Indicates if the given address is a multicast address. */
    public: virtual bool isMulticastAddress () const;

    /** Gets this address as an IPv4 address. If the address is only
     *  representable as an IPv6 address, 0.0.0.0 is returned.
     */
    public: inline struct in_addr toIPv4 () const {
      return ipv4;
    }

    /** Gets this address as an IPv6 address. */
    public: inline struct in6_addr toIPv6 () const {
      return ipv6;
    }

    using VRTObject::equals;
    /** Checks to see if two addresses are equal. Note that for the purposes of
     *  this check "0.0.0.0" and "::0" are considered NOT EQUAL even though both
     *  are considered a "null value" (the difference being IPv4 null vs IPv6
     *  null).
     */
    public: virtual bool equals (const VRTObject &o) const;

    /** Checks to see if two addresses are equal. Note that for the purposes of
     *  this check "0.0.0.0" and "::0" are considered NOT EQUAL even though both
     *  are considered a "null value" (the difference being IPv4 null vs IPv6
     *  null).
     */
    public: virtual bool equals (const InetAddress &addr) const;

    /** Converts the address to a string. */
    public: virtual inline string toString () const {
      return getHostAddress();
    }

    /** Converts the address to a string. */
    public: virtual string getHostAddress () const;

    /** Converts a string to an address. */
    public: virtual void setHostAddress (const string &addr);

    /** Converts a string to an address. */
    public: virtual void setHostAddress (const char *addr);

    /** Converts a host name string to an address. */
    private: void setHostAddressByName (const string &addr);

    /** Converts a host name string to an address. */
    private: void setHostAddressByName (const char *addr);

    /** Converts a host name string to an address with preference given to a
     *  single address family (AF_INET or AF_INET6) *or* trying any of them
     *  (AF_UNSPEC).
     */
    private: void setHostAddressByName (const string &addr, int family);

    /** Gets the local host address (not the loop-back address). */
    public: static InetAddress getLocalHost ();

    /** Gets the loop-back address (this will be 127.0.0.1 or ::1). The choice
     *  of IPv4 vs IPv6 is based on the <tt>VRT_PREFER_IPV6_ADDRESSES</tt>
     *  setting (see <tt>VRTConfig</tt>).
     */
    public: static InetAddress getLoopbackAddress ();

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Implement HasFields
    //////////////////////////////////////////////////////////////////////////////////////////////////
    public: virtual int32_t   getFieldCount () const;
    public: virtual string    getFieldName  (int32_t id) const;
    public: virtual ValueType getFieldType  (int32_t id) const;
    public: virtual Value*    getField      (int32_t id) const  __attribute__((warn_unused_result));
    public: virtual void      setField      (int32_t id, const Value* val);
  };


  // See VRTMath.c for function code
  namespace VRTMath {
    /** Pack a 16-byte IPv6 address into a buffer.
     *  @param ptr   Pointer to packed byte array [OUTPUT]
     *  @param off   Offset into array
     *  @param val   value to pack [INPUT]
     */
    inline void packInetAddr (void *ptr, int32_t off, const InetAddress &val) {
      char *buf = (char*)ptr;
      memcpy(&buf[off], &val.toIPv6().s6_addr[0], 16);
    }

    /** Pack a 16-byte IPv6 address into a buffer.
     *  @param buf   byte array of data [OUTPUT]
     *  @param off   Offset into array
     *  @param val   value to pack [INPUT]
     */
    inline void packInetAddr (vector<char> &buf, int32_t off, const InetAddress &val) {
      memcpy(&buf[off], &val.toIPv6().s6_addr[0], 16);
    }

    /** Unpack a 16-byte IPv6 address from a buffer.
     *  @param ptr   pointer to byte array of data
     *  @param off   Offset into array
     *  @return The unpacked value
     */
    inline InetAddress unpackInetAddr (const void *ptr, int32_t off) {
      return InetAddress(ptr, off, 16);
    }

    /** Unpack a 16-byte IPv6 address from a buffer.
     *  @param buf   byte array of data
     *  @param off   Offset into array
     *  @return The unpacked value
     */
    inline InetAddress unpackInetAddr (const vector<char> &buf, int32_t off) {
      return InetAddress(buf, off);
    }
  } END_NAMESPACE
} END_NAMESPACE
#endif /* _InetAddress_h */

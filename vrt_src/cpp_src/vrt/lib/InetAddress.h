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

#ifndef _InetAddress_h
#define _InetAddress_h

#include "VRTObject.h"
#include "HasFields.h"
#include <netinet/in.h>

namespace vrt {
  /** An IPv4 or IPv6 internet address.
   *
   *  @author 
   */
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

    /** Creates a new instance, using the specified address from a buffer. If <tt>off</tt> is 0
     *  and the vector size is 4, then a 4-byte IPv4 address is used, otherwise a 16-byte IPv6
     *  address is assumed.
     *  @param addr The given address.
     *  @param off  The offset into the array.
     */
    public: InetAddress (const vector<char> &addr, size_t off=0);

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
    
    /** Converts a string to an address. <b>This is not yet supported.</b> */
    public: virtual void setHostAddress (const string &addr);
    
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
    /** Pack a 16-byte IPv6 address into a buffer.
        @param buf   byte array of data [OUTPUT]
        @param off   Offset into array
        @param val   value to pack [INPUT]
     */
    void packInetAddr (vector<char> &buf, int32_t off, const InetAddress &val);

    /** Unpack a 16-byte IPv6 address from a buffer.
        @param buf   byte array of data
        @param off   Offset into array
        @return The unpacked value
     */
    InetAddress unpackInetAddr (const vector<char> &buf, int32_t off);
  };
};
#endif /* _InetAddress_h */

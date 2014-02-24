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

#include "InetAddress.h"
#include <vector>
#include <string.h>    // required for memcpy(..) on GCC4.4/libc6 2.11.1
#include <arpa/inet.h> // required for inet_pton(..)
#include <errno.h>

#if (defined(__APPLE__) && defined(__MACH__))
// Some of the "standard" include-file definitions used on Linux are not present
// on OS X (__APPLE__ & __MACH__) under GCC 4.2.1. It is possible these are
// missing under BSD too, but this is untested.
# ifndef s6_addr16
#  define s6_addr16 __u6_addr.__u6_addr16
# endif
# ifndef s6_addr32
#  define s6_addr32 __u6_addr.__u6_addr32
# endif
#endif

using namespace vrt;

InetAddress::InetAddress () {
  ipv4.s_addr       = 0x00000000;
  ipv6.s6_addr32[0] = 0x00000000;
  ipv6.s6_addr32[1] = 0x00000000;
  ipv6.s6_addr32[2] = 0x00000000;
  ipv6.s6_addr32[3] = 0x00000000;
}

InetAddress::InetAddress (const InetAddress &addr) : ipv4(addr.ipv4), ipv6(addr.ipv6) {
  // done
}

InetAddress::InetAddress (const string &addr) {
  setHostAddress(addr);
}

InetAddress::InetAddress (const vector<char> &addr, size_t off) {
  if ((off == 0) && (addr.size() == 4)) {
    ipv6.s6_addr32[2] = 0x0000FFFF;
    memcpy(&ipv6.s6_addr[12], &addr[off], 4);
    ipv4.s_addr = ipv6.s6_addr32[3];
  }
  else {
    memcpy(&ipv6.s6_addr[0], &addr[off], 16);
    if (isIPv4()) {
      ipv4.s_addr = ipv6.s6_addr32[3];
    }
  }
}

InetAddress::InetAddress (const struct in_addr &addr) : ipv4(addr) {
  ipv6.s6_addr16[0] = 0x0000;
  ipv6.s6_addr16[1] = 0x0000;
  ipv6.s6_addr16[2] = 0x0000;
  ipv6.s6_addr16[3] = 0x0000;
  ipv6.s6_addr16[4] = 0x0000;
  ipv6.s6_addr16[5] = 0xFFFF;
  ipv6.s6_addr32[3] = ipv4.s_addr;
}

InetAddress::InetAddress (const struct in6_addr &addr) : ipv6(addr) {
  if (isIPv4()) {
    ipv4.s_addr = ipv6.s6_addr32[3];
  }
}

bool InetAddress::isNullValue () const {
  return  (ipv6.s6_addr16[0] == 0x0000)
      &&  (ipv6.s6_addr16[1] == 0x0000)
      &&  (ipv6.s6_addr16[2] == 0x0000)
      &&  (ipv6.s6_addr16[3] == 0x0000)
      &&  (ipv6.s6_addr16[4] == 0x0000)
      && ((ipv6.s6_addr16[5] == 0x0000) || (ipv6.s6_addr16[5] == 0xFFFF))
      &&  (ipv6.s6_addr16[6] == 0x0000)
      &&  (ipv6.s6_addr16[7] == 0x0000);
}

bool InetAddress::isIPv4 () const {
  return  (ipv6.s6_addr16[0] == 0x0000)
      &&  (ipv6.s6_addr16[1] == 0x0000)
      &&  (ipv6.s6_addr16[2] == 0x0000)
      &&  (ipv6.s6_addr16[3] == 0x0000)
      &&  (ipv6.s6_addr16[4] == 0x0000)
      &&  (ipv6.s6_addr16[5] == 0xFFFF);
}

bool InetAddress::equals (const VRTObject &o) const {
  try {
    return equals(*checked_dynamic_cast<const InetAddress*>(&o));
  }
  catch (bad_cast &e) {
    return false;
  }
}

bool InetAddress::equals (const InetAddress &addr) const {
  return (ipv6.s6_addr32[0] == addr.ipv6.s6_addr32[0])
      && (ipv6.s6_addr32[1] == addr.ipv6.s6_addr32[1])
      && (ipv6.s6_addr32[2] == addr.ipv6.s6_addr32[2])
      && (ipv6.s6_addr32[3] == addr.ipv6.s6_addr32[3]);
}

string InetAddress::getHostAddress () const {
  char str[48];

  if (isIPv4()) {
    snprintf(str, 48, "%d.%d.%d.%d", ipv6.s6_addr[12], ipv6.s6_addr[13], ipv6.s6_addr[14], ipv6.s6_addr[15]);
  }
  else {
    snprintf(str, 48, "%.4x:%.4x:%.4x:%.4x:%.4x:%.4x:%.4x:%.4x",
                      ipv6.s6_addr16[0], ipv6.s6_addr16[1], ipv6.s6_addr16[2], ipv6.s6_addr16[3],
                      ipv6.s6_addr16[4], ipv6.s6_addr16[5], ipv6.s6_addr16[6], ipv6.s6_addr16[7]);

  }
  return str;
}

void InetAddress::setHostAddress (const string &addr) {
  // Can probably use inet_pton(..) from arpa/inet.h for this, but need to check
  // to see it is on all platforms we support. Also need to make sure we support
  // IPv4/IPv6 and IPv4-in-IPv6.
  if ((addr == "") || (addr == "null")) {
    // the setField(..) with an empty string becomes a set with "null", so handle
    // it likewise
    throw VRTException("Invalid HostAddress given empty/null string");
  }
  
  size_t colon  = addr.find(':');
  bool   isIPv6 = (colon != string::npos);
  int    status;
  
  if (isIPv6) {
#ifdef AF_INET6
    status = inet_pton(AF_INET6, addr.c_str(), &ipv6);
#else
    throw VRTException("Setting InetAddress.HostAddress using IPv6 is not "
                       "supported on this platform.");
#endif
  }
  else {
    status = inet_pton(AF_INET, addr.c_str(), &ipv4);
  }
  
  if (status == 1) { // SUCCESS
    if (isIPv6) {
      if (isIPv4()) {
        ipv4.s_addr = ipv6.s6_addr32[3];
      }
      else {
        ipv4.s_addr = ipv6.s6_addr32[3];
      }
    }
    else {
      ipv6.s6_addr16[0] = 0x0000;
      ipv6.s6_addr16[1] = 0x0000;
      ipv6.s6_addr16[2] = 0x0000;
      ipv6.s6_addr16[3] = 0x0000;
      ipv6.s6_addr16[4] = 0x0000;
      ipv6.s6_addr16[5] = 0xFFFF;
      ipv6.s6_addr32[3] = ipv4.s_addr;
    }
  }
  else if (status == 0) { // INVALID
    if (isIPv6) {
      throw VRTException("Invalid IPv6 HostAddress given '%s'", addr.c_str());
    }
    else {
      throw VRTException("Invalid IPv4 HostAddress given '%s'", addr.c_str());
    }
  }
  else if (status == -1) { // ERROR (errno set)
    throw VRTException(errno);
  }
  else { // UNKNOWN ERROR
    throw VRTException("Invalid HostAddress given '%s'", addr.c_str());
  }
}

int32_t InetAddress::getFieldCount () const {
  return 1;
}

string InetAddress::getFieldName (int32_t id) const {
  switch (id) {
    case 0: return "HostAddress";
    default: throw VRTException("Invalid field #%d in %s", id, getClassName().c_str());
  }
}

ValueType InetAddress::getFieldType (int32_t id) const {
  switch (id) {
    case 0: return ValueType_String;
    default: throw VRTException("Invalid field #%d in %s", id, getClassName().c_str());
  }
}

Value* InetAddress::getField (int32_t id) const {
  switch (id) {
    case 0:  return new Value(getHostAddress());
    default: return new Value(); // null value
  }
}

void InetAddress::setField (int32_t id, const Value* val) {
  switch (id) {
    case 0: setHostAddress((string)*val); return;
    default: throw VRTException("Invalid field #%d in %s", id, getClassName().c_str());
  }
}


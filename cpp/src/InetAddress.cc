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

#include "InetAddress.h"
#include "VRTConfig.h"
#include <vector>
#include <string.h>      // required for memcpy(..) on GCC4.4/libc6 2.11.1
#include <arpa/inet.h>   // required for inet_pton(..)
#include <errno.h>
#if (_POSIX_C_SOURCE >= 1) || _XOPEN_SOURCE || _POSIX_SOURCE || (defined(__APPLE__) && defined(__MACH__))
# include <sys/types.h>  // required for getaddrinfo(..)
# include <sys/socket.h> // required for getaddrinfo(..)
# include <netdb.h>      // required for getaddrinfo(..)
# include <unistd.h>     // required for gethostname(..) on OS X
#else
#endif

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

#ifndef HOST_NAME_MAX
# ifdef _SC_HOST_NAME_MAX
#  define HOST_NAME_MAX _SC_HOST_NAME_MAX // Used on OS X (other BSD?)
# else
#  define HOST_NAME_MAX 255 // Common default
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

InetAddress::InetAddress (const InetAddress &addr) :
  VRTObject(addr), // <-- Used to avoid warnings under GCC with -Wextra turned on
  ipv4(addr.ipv4),
  ipv6(addr.ipv6)
{
  // done
}

InetAddress::InetAddress (const string &addr) {
  setHostAddress(addr);
}

InetAddress::InetAddress (const char *addr) {
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

InetAddress::InetAddress (const void *addr, size_t off, size_t len) {
  char *buf = (char*)addr;

  if (len == 4) {
    ipv6.s6_addr32[2] = 0x0000FFFF;
    memcpy(&ipv6.s6_addr[12], &buf[off], 4);
    ipv4.s_addr = ipv6.s6_addr32[3];
  }
  else {
    memcpy(&ipv6.s6_addr[0], &buf[off], 16);
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

bool InetAddress::isMulticastAddress () const {
  if (isIPv4()) {
    return (((uint8_t)ipv6.s6_addr[12]) >= 224) &&
           (((uint8_t)ipv6.s6_addr[12]) <= 239);
  }
  else {
    return (((uint8_t)ipv6.s6_addr[0]) == (uint8_t)0xFF);
  }
}

bool InetAddress::equals (const VRTObject &o) const {
  try {
    return equals(*checked_dynamic_cast<const InetAddress*>(&o));
  }
  catch (bad_cast &e) {
    UNUSED_VARIABLE(e);
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
    snprintf(str, 48, "%.2x%.2x:%.2x%.2x:%.2x%.2x:%.2x%.2x:%.2x%.2x:%.2x%.2x:%.2x%.2x:%.2x%.2x",
                      ipv6.s6_addr[ 0], ipv6.s6_addr[ 1], ipv6.s6_addr[ 2], ipv6.s6_addr[ 3],
                      ipv6.s6_addr[ 4], ipv6.s6_addr[ 5], ipv6.s6_addr[ 6], ipv6.s6_addr[ 7],
                      ipv6.s6_addr[ 8], ipv6.s6_addr[ 9], ipv6.s6_addr[10], ipv6.s6_addr[11],
                      ipv6.s6_addr[12], ipv6.s6_addr[13], ipv6.s6_addr[14], ipv6.s6_addr[15]);
  }
  return str;
}

void InetAddress::setHostAddressByName (const string &addr) {
  int family = (VRTConfig::getPreferIPv6Addresses())? AF_INET6 : AF_INET;
  setHostAddressByName(addr, family);
}

void InetAddress::setHostAddressByName (const char *addr) {
  if (addr == NULL) throw VRTException("Can not set address to null");
  int family = (VRTConfig::getPreferIPv6Addresses())? AF_INET6 : AF_INET;
  setHostAddressByName(string(addr), family);
}

void InetAddress::setHostAddressByName (const string &addr, int family) {
  bool ok = false;
#if (_POSIX_C_SOURCE >= 1) || _XOPEN_SOURCE || _POSIX_SOURCE || (defined(__APPLE__) && defined(__MACH__))
  struct addrinfo *res = NULL;
  struct addrinfo *r   = NULL;
  struct addrinfo  hints;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = family;

  int err = getaddrinfo(addr.c_str(), NULL, &hints, &res);
  if (err != 0) {
    throw VRTException(string("Error looking up '")+addr+"': "+gai_strerror(err));
  }

  for (r = res; r != NULL; r = r->ai_next) {
    if (res->ai_family == AF_INET) {
      ipv4 = ((sockaddr_in*)res->ai_addr)->sin_addr;
      ipv6.s6_addr16[0] = 0x0000;
      ipv6.s6_addr16[1] = 0x0000;
      ipv6.s6_addr16[2] = 0x0000;
      ipv6.s6_addr16[3] = 0x0000;
      ipv6.s6_addr16[4] = 0x0000;
      ipv6.s6_addr16[5] = 0xFFFF;
      ipv6.s6_addr32[3] = ipv4.s_addr;
      ok = true;
      break;
    }
    else if (res->ai_family == AF_INET6) {
      ipv6 = ((sockaddr_in6*)res->ai_addr)->sin6_addr;
      if (isIPv4()) {
        ipv4.s_addr = ipv6.s6_addr32[3];
      }
      ok = true;
      break;
    }
  }
  freeaddrinfo(res);
#else
  // This code is "obsolescent" in POSIX.1-2001 and removed from POSIX.1-2008.
  // However we keep it here in the off-chance the above code doesn't work
  // (though we only include the IPv4 form). [This code was manually tested in
  // Nov 2013 but isn't in the normal unit tests as the test platforms all
  // support the newer code.]
  struct hostent *res = gethostbyname(addr.c_str());
  if (res == NULL) {
    throw VRTException(string("Error looking up '")+addr+"': "+hstrerror(h_errno));
  }

  if (res->h_addrtype == AF_INET) {
    memcpy(&ipv4.s_addr, res->h_addr, res->h_length);
    ipv6.s6_addr16[0] = 0x0000;
    ipv6.s6_addr16[1] = 0x0000;
    ipv6.s6_addr16[2] = 0x0000;
    ipv6.s6_addr16[3] = 0x0000;
    ipv6.s6_addr16[4] = 0x0000;
    ipv6.s6_addr16[5] = 0xFFFF;
    ipv6.s6_addr32[3] = ipv4.s_addr;
    ok = true;
  }
#endif
  if (!ok) {
    if (family != AF_UNSPEC) {
      // Fall-back to trying both IPv4 *and* IPv6
      return setHostAddressByName(addr, AF_UNSPEC);
    }
    throw VRTException(string("Error looking up '")+addr+"': Unsupported address type returned");
  }
}

void InetAddress::setHostAddress (const string &addr) {
  if ((addr == "") || (addr == "null")) {
    // the setField(..) with an empty string becomes a set with "null", so handle
    // it likewise
    throw VRTException("Invalid HostAddress given empty/null string");
  }

  string _addr   = addr;
  size_t colon   = _addr.find(":");
  size_t rcolon  = _addr.rfind(":");
  size_t bracket = _addr.find("[");
  bool   _isIPv6 = (bracket == 0) || ((colon != string::npos) && (colon != rcolon));
  bool   _isIPv4 = !_isIPv6;
  int    status;

  if (bracket == 0) _addr = _addr.substr(1,addr.size()-2);

  for (size_t i = 0; _isIPv4 && (i < _addr.size()); i++) {
    char c = _addr[i];
    _isIPv4 = (c == '.') || ((c >= '0') && (c <= '9'));
  }

  if (_isIPv6) {
#ifdef AF_INET6
    status = inet_pton(AF_INET6, _addr.c_str(), &ipv6);
#else
    throw VRTException("Setting InetAddress.HostAddress using IPv6 is not "
                       "supported on this platform.");
#endif
  }
  else if (colon != string::npos) {
    // Error where user passes in host:port (VRT-43)
    throw VRTException("Expected host name but given %s", addr.c_str());
  }
  else if (_isIPv4) {
    status = inet_pton(AF_INET, _addr.c_str(), &ipv4);
  }
  else {
    setHostAddressByName(_addr);
    return;
  }

  if (status == 1) { // SUCCESS
    if (_isIPv6) {
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
    if (_isIPv6) {
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

void InetAddress::setHostAddress (const char *addr) {
  if (addr == NULL) throw VRTException("Can not set address to null");
  setHostAddress(string(addr));
}

InetAddress InetAddress::getLocalHost () {
#if _BSD_SOURCE || _XOPEN_SOURCE >= 500 || (defined(__APPLE__) && defined(__MACH__))
  char hostname[HOST_NAME_MAX + 1]; // +1 for terminating NUL character
  int status = gethostname(hostname, HOST_NAME_MAX+1);
  if (status < 0) {
    throw VRTException(errno);
  }
  hostname[HOST_NAME_MAX] = '\0'; // since POSIX.1-2001 says \0 can be omitted if name doesn't fit

  return InetAddress(hostname);
#else
# warning "gethostname(..) is unsupported, will use loop-back address as local host address"
  return getLoopbackAddress();
#endif
}

InetAddress InetAddress::getLoopbackAddress () {
  return (VRTConfig::getPreferIPv6Addresses())? InetAddress("::1")
                                              : InetAddress("127.0.0.1");
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
    case 0:  setHostAddress(val->toString()); return;
    default: throw VRTException("Invalid field #%d in %s", id, getClassName().c_str());
  }
}


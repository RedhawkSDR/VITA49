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

#ifndef _NetUtilities_h
#define _NetUtilities_h

#include "VRTObject.h"

using namespace std;

namespace vrt {
  namespace NetUtilities {
    /**  Maximum length of a UDP packet (including the UDP header) in bytes (65,635).
        @see #MAX_IPv4_UDP_LEN
     */
    const int32_t MAX_UDP_LEN = 65635;

    /**  Maximum length of a UDP packet (including the UDP header) in bytes when transmitted
        over IPv4 (65,471). Although the max packet length for UDP is 65,635 bytes, IPv4 imposes a
        limit of 65,515 to 65,471 bytes based on the number of "options" set in the IPv4 header.
        Accordingly this value should be thought of as the maximum for transmission size and
        {@link #MAX_UDP_LEN} should be used as the maximum receive size.
        @see #MAX_UDP_LEN
     */
    const int32_t MAX_IPv4_UDP_LEN = 65471;

    /**  The length of a UPD packet header in bytes (8). */
    const int32_t UDP_HEADER_LENGTH = 8;

    /**  Gets the device name applicable when given a NIC and VLAN.
     *  @param nic  The NIC to use  ("" if n/a).
     *  @param vlan The VLAN to use (-1 if n/a).
     *  @return The applicable device identifier (e.g. "eth1", "eth0.123").
     *  @throws VRTException if a VLAN is specified without a NIC
     */
    string getNetworkDeviceName (const string &nic, int32_t vlan);
  };
};

#endif /* _NetUtilities_h */

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

#include "NetUtilities.h"

using namespace std;
using namespace vrt;

string vrt::NetUtilities::getNetworkDeviceName (const string &nic, int32_t vlan) {
  if (vlan < 0) return nic;

  if (nic.length() == 0) throw VRTException("Can not specify VLAN of %d without specifying a NIC", vlan);

  char dev[32];
  snprintf(dev, 32, "%s.%d", nic.c_str(), vlan);
  return dev;
}

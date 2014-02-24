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

#include "PacketIterator.h"

using namespace std;
using namespace vrt;

ConstPacketIterator::ConstPacketIterator (const PacketContainer *container, int64_t offset, bool resolve) :
  container(container),
  resolve(resolve),
  offset(offset),
  length(__INT64_C(-1))
{
  buf = vector<char>(4);
}

ConstPacketIterator::ConstPacketIterator (const ConstPacketIterator &pi) :
  container(pi.container),
  resolve(pi.resolve),
  offset(pi.offset),
  length(pi.length),
  buf(pi.buf)
{
  // done
}

string ConstPacketIterator::toString () const {
  const VRTObject *c = checked_dynamic_cast<const VRTObject*>(container);
  return getClassName() + " for " + c->toString();
//  return Utilities::format("%s for %s @%" PRId64 "/%d",
//                           getClassName().c_str(), c->toString().c_str(), offset, resolve);
}

bool ConstPacketIterator::equals (const ConstPacketIterator &pi) const {
  const VRTObject *c1 = checked_dynamic_cast<const VRTObject*>(container);
  const VRTObject *c2 = checked_dynamic_cast<const VRTObject*>(pi.container);
  
  return (c1       == c2        )
      && (offset   == pi.offset )
      && (resolve  == pi.resolve);
}

ConstPacketIterator ConstPacketIterator::operator++ (int) { // postfix ++
  ConstPacketIterator pi(*this);
  container->gotoNextPacket(*this);
  return pi;
}

ConstPacketIterator ConstPacketIterator::operator++ () { // prefix ++
  container->gotoNextPacket(*this);
  return *this;
}

BasicVRTPacket* ConstPacketIterator::operator* () {
  return container->getThisPacket(*this,false);
}

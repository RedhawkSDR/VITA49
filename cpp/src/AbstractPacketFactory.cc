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

#include "AbstractPacketFactory.h"
#include "BasicContextPacket.h"
#include "BasicDataPacket.h"
#include "StandardDataPacket.h"
#include "VRTConfig.h"

using namespace vrt;

AbstractPacketFactory::~AbstractPacketFactory () {
  if (parent != NULL) delete parent;
  parent = NULL;
}

AbstractPacketFactory::AbstractPacketFactory (const AbstractPacketFactory *pf) :
  parent(pf)
{
  // done
}

AbstractPacketFactory::AbstractPacketFactory () :
  parent(NULL)
{
  // done
}

BasicVRTPacket *AbstractPacketFactory::getPacket (PacketType type, int64_t id, const BasicVRTPacket *p) const {
  if (parent != NULL) {
    return (p == NULL)? parent->getPacket(type, id) : parent->getPacket(p);
  }

  if (StandardDataPacket::isStandardDataPacket(id)) {
    return (p == NULL)? new StandardDataPacket() : new StandardDataPacket(*p);
  }
  if (p != NULL) {
    switch (type) {
      case PacketType_UnidentifiedData:    return new BasicDataPacket(*p);
      case PacketType_Data:                return new BasicDataPacket(*p);
      case PacketType_UnidentifiedExtData: return new BasicDataPacket(*p);
      case PacketType_ExtData:             return new BasicDataPacket(*p);
      case PacketType_Context:             return new BasicContextPacket(*p);
      case PacketType_ExtContext:          return new BasicVRTPacket(*p);
      default:                             return new BasicVRTPacket(*p);
    }
  }
  else {
    switch (type) {
      case PacketType_UnidentifiedData:    return new BasicDataPacket();
      case PacketType_Data:                return new BasicDataPacket();
      case PacketType_UnidentifiedExtData: return new BasicDataPacket();
      case PacketType_ExtData:             return new BasicDataPacket();
      case PacketType_Context:             return new BasicContextPacket();
      case PacketType_ExtContext:          return new BasicVRTPacket();
      default:                             return new BasicVRTPacket();
    }
  }
}

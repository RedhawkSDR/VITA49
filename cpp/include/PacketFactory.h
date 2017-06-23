/*
 * This file is protected by Copyright. Please refer to the COPYRIGHT file
 * distributed with this source distribution.
 *
 * This file is part of REDHAWK VITA49Libraries.
 *
 * REDHAWK VITA49Libraries is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * REDHAWK VITA49Libraries is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */
//
//  This class was auto-generated, do not modify.
//
#ifndef _PacketFactory_h
#define _PacketFactory_h

#include "AbstractPacketFactory.h"
#include "BasicVRTPacket.h"

using namespace std;
using namespace vrt;

namespace vrt {
  
  /** Packet factory for auto-generated classes.
   *  
   *  @author null
   */
  class PacketFactory: public AbstractPacketFactory {
    
    /** Destructor for the class. */
    public: ~PacketFactory ();
    
    /** Creates a new instance with no parent class. */
    public: PacketFactory ();
    
    /** Creates a new instance with a parent class. */
    public: PacketFactory (AbstractPacketFactory *parent);
    
    protected: virtual BasicVRTPacket *getPacket (PacketType type, int64_t id, const BasicVRTPacket *p) const;
  };
};

#endif /* _PacketFactory_h */


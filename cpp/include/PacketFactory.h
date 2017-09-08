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
  
  /** Packet factory for auto-generated classes. */
  class PacketFactory: public AbstractPacketFactory {
    
    /** Destructor for the class. */
    public: ~PacketFactory ();
    
    /** Creates a new instance with no parent class. */
    public: PacketFactory ();
    
    /** Creates a new instance with a parent class. */
    public: PacketFactory (AbstractPacketFactory *parent);
    
    protected: virtual BasicVRTPacket *getPacket (PacketType type, int64_t id, const BasicVRTPacket *p) const;
  };
} END_NAMESPACE

#endif /* _PacketFactory_h */


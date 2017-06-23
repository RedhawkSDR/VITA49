//
//  This class was auto-generated, do not modify.
//
#include "PacketFactory.h"
#include "BasicVRTPacket.h"
#include "NoDataPacket.h"
#include "StandardContextPacket.h"
#include "TimestampAccuracyPacket.h"
#include "StreamStatePacket.h"
#include "ReferencePointPacket.h"
#include "EphemerisPacket.h"

using namespace std;
using namespace vrt;

PacketFactory::~PacketFactory () {
  // done
}

PacketFactory::PacketFactory () :
  AbstractPacketFactory()
{
  // done
}

PacketFactory::PacketFactory (AbstractPacketFactory *parent) :
  AbstractPacketFactory(parent)
{
  // done
}

BasicVRTPacket *PacketFactory::getPacket (PacketType type, int64_t id, const BasicVRTPacket *p) const {
  if (p == NULL) {
    if ((type == PacketType_UnidentifiedData   ) || (type == PacketType_Data   ) ||
        (type == PacketType_UnidentifiedExtData) || (type == PacketType_ExtData)) {
      if (id == NoDataPacket::CLASS_IDENTIFIER) return new NoDataPacket;
    }
    else {
      if (id == StandardContextPacket::CLASS_IDENTIFIER) return new StandardContextPacket;
      if (id == TimestampAccuracyPacket::CLASS_IDENTIFIER) return new TimestampAccuracyPacket;
      if (id == StreamStatePacket::CLASS_IDENTIFIER) return new StreamStatePacket;
      if (id == ReferencePointPacket::CLASS_IDENTIFIER) return new ReferencePointPacket;
      if (id == EphemerisPacket::CLASS_IDENTIFIER) return new EphemerisPacket;
    }
  }
  else {
    if ((type == PacketType_UnidentifiedData   ) || (type == PacketType_Data   ) ||
        (type == PacketType_UnidentifiedExtData) || (type == PacketType_ExtData)) {
      if (id == NoDataPacket::CLASS_IDENTIFIER) return new NoDataPacket(*p);
    }
    else {
      if (id == StandardContextPacket::CLASS_IDENTIFIER) return new StandardContextPacket(*p);
      if (id == TimestampAccuracyPacket::CLASS_IDENTIFIER) return new TimestampAccuracyPacket(*p);
      if (id == StreamStatePacket::CLASS_IDENTIFIER) return new StreamStatePacket(*p);
      if (id == ReferencePointPacket::CLASS_IDENTIFIER) return new ReferencePointPacket(*p);
      if (id == EphemerisPacket::CLASS_IDENTIFIER) return new EphemerisPacket(*p);
    }
  }
  return AbstractPacketFactory::getPacket(type, id, p);
}

/** Creates a new instance of the factory via a C-linkable method. This method is
 *  used for dynamically creating an instance of the class at run-time; this method
 *  should never be called directly.
 */
extern "C"
AbstractPacketFactory* _new_vrt__PacketFactory_0 () {
  return new PacketFactory;
}

/** Creates a new instance of the factory via a C-linkable method. This method is
 *  used for dynamically creating an instance of the class at run-time; this method
 *  should never be called directly.
 *  @param parent The parent packet factory.
 */
extern "C"
AbstractPacketFactory* _new_vrt__PacketFactory_1 (AbstractPacketFactory *parent) {
  return new PacketFactory(parent);
}


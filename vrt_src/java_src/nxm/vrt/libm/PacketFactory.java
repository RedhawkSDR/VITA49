//
//  This class was auto-generated, do not modify.
//
package nxm.vrt.libm;

/** Packet factory for auto-generated classes.
 *  
 *  @author null
 */
public class PacketFactory extends nxm.vrt.lib.AbstractPacketFactory {
  
  /** Creates a new instance with no parent class. */
  public PacketFactory () {
    super();
  }
  
  /** Creates a new instance with a parent class. */
  public PacketFactory (nxm.vrt.lib.PacketFactory parent) {
    super(parent);
  }
  
  @Override
  public java.lang.Class<? extends nxm.vrt.lib.VRTPacket> getPacketClass (nxm.vrt.lib.VRTPacket.PacketType type, java.lang.Long id) {
    if (id == null) return super.getPacketClass(type, id);
    long classIdentifier = id;
    
    if (type.isData()) {
      if (classIdentifier == NoDataPacket.CLASS_IDENTIFIER) return NoDataPacket.class;
    }
    else {
      if (classIdentifier == StandardContextPacket.CLASS_IDENTIFIER) return StandardContextPacket.class;
      if (classIdentifier == TimestampAccuracyPacket.CLASS_IDENTIFIER) return TimestampAccuracyPacket.class;
      if (classIdentifier == StreamStatePacket.CLASS_IDENTIFIER) return StreamStatePacket.class;
      if (classIdentifier == ReferencePointPacket.CLASS_IDENTIFIER) return ReferencePointPacket.class;
      if (classIdentifier == EphemerisPacket.CLASS_IDENTIFIER) return EphemerisPacket.class;
    }
    return super.getPacketClass(type, id);
  }
  
  @Override
  protected nxm.vrt.lib.VRTPacket getPacket (java.lang.Class<? extends nxm.vrt.lib.VRTPacket> clazz,
                                             nxm.vrt.lib.VRTPacket.PacketType type, java.lang.Long id,
                                             nxm.vrt.lib.VRTPacket p, boolean copy) {
    if (p == null) {
      if (clazz == NoDataPacket.class) return new NoDataPacket();
      if (clazz == StandardContextPacket.class) return new StandardContextPacket();
      if (clazz == TimestampAccuracyPacket.class) return new TimestampAccuracyPacket();
      if (clazz == StreamStatePacket.class) return new StreamStatePacket();
      if (clazz == ReferencePointPacket.class) return new ReferencePointPacket();
      if (clazz == EphemerisPacket.class) return new EphemerisPacket();
    }
    else {
      if (clazz == NoDataPacket.class) return new NoDataPacket(nxm.vrt.lib.BasicVRTPacket.toBasicVRTPacket(p), copy);
      if (clazz == StandardContextPacket.class) return new StandardContextPacket(nxm.vrt.lib.BasicVRTPacket.toBasicVRTPacket(p), copy);
      if (clazz == TimestampAccuracyPacket.class) return new TimestampAccuracyPacket(nxm.vrt.lib.BasicVRTPacket.toBasicVRTPacket(p), copy);
      if (clazz == StreamStatePacket.class) return new StreamStatePacket(nxm.vrt.lib.BasicVRTPacket.toBasicVRTPacket(p), copy);
      if (clazz == ReferencePointPacket.class) return new ReferencePointPacket(nxm.vrt.lib.BasicVRTPacket.toBasicVRTPacket(p), copy);
      if (clazz == EphemerisPacket.class) return new EphemerisPacket(nxm.vrt.lib.BasicVRTPacket.toBasicVRTPacket(p), copy);
    }
    return super.getPacket(clazz, type, id, p, copy);
  }
}

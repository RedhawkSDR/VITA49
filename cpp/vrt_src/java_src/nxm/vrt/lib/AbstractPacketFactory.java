/**
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

package nxm.vrt.lib;

import nxm.vrt.lib.VRTPacket.PacketType;

/** A basic but full-featured implementation of {@link PacketFactory}. Typical implementations
 *  will look like:
 *  <pre>
 *    public class MyPacketFactory extends AbstractPacketFactory {
 *      public MyPacketFactory () {
 *        super();
 *      }
 *
 *      public MyPacketFactory (PacketFactory parent) {
 *        super(parent);
 *      }
 *
 *      public Class&lt;? extends VRTPacket&gt; getPacketClass (PacketType type, Long id) {
 *        ...
 *        return super.getPacketClass(type, id);
 *      }
 *
 *      protected VRTPacket getPacket (Class&lt;? extends VRTPacket&gt; clazz, PacketType type,
 *                                     Long id, VRTPacket p, boolean copy) {
 *        ...
 *        return super.getPacket(clazz, type, id, p, copy);
 *      }
 *    }
 *  </pre>
 *  @author 
 */
public class AbstractPacketFactory implements PacketFactory {
  private final PacketFactory parent;

  /** Creates a new instance with no parent class. This is identical to
   *  <tt>new BasicPacketFactory(null)</tt>.
   */
  public AbstractPacketFactory () {
    this(null);
  }
  
  /** Creates a new instance that chains delegates <tt>getPacket(..)</tt> calls to the parent
   *  class if the <tt>getPacketFor(..)</tt> or <tt>getClassFor(..)</tt> methods return null. If the
   *  a call to <tt>getPacketFor(..)</tt> or <tt>getClassFor(..)</tt> returns null while the parent
   *  class is null, the default class ({@link BasicDataPacket}, {@link BasicContextPacket}, or
   *  {@link BasicVRTPacket}) will be used.
   *  @param parent The applicable parent class.
   */
  public AbstractPacketFactory (PacketFactory parent) {
    this.parent = parent;
  }
  
  @Override
  public Class<? extends VRTPacket> getPacketClass (PacketType type, Long id) {
    if (parent != null) return parent.getPacketClass(type, id);
    
    if (StandardDataPacket.isStandardDataPacket(id)) {
      return StandardDataPacket.class;
    }

    switch (type) {
      case UnidentifiedData:    return BasicDataPacket.class;
      case Data:                return BasicDataPacket.class;
      case UnidentifiedExtData: return BasicDataPacket.class;
      case ExtData:             return BasicDataPacket.class;
      case Context:             return BasicContextPacket.class;
      case ExtContext:          return BasicVRTPacket.class;
      default:                  return BasicVRTPacket.class;
    }
  }

  @Override
  public VRTPacket getPacket (VRTPacket p, boolean copy) {
    PacketType type = p.getPacketType();
    Long       id   = p.getClassIdentifier();
    return getPacket(getPacketClass(type, id), type, id, p, copy);
  }

  @Override
  public VRTPacket getPacket (PacketType type, Long id) {
    return getPacket(getPacketClass(type, id), type, id, null, false);
  }

  /** Initializes a packet. This is a utility method called by both
   *  {@link #getPacket(VRTPacket,boolean)} and {@link #getPacket(VRTPacket.PacketType,Long)}.
   *  @param clazz The applicable class returned from {@link #getPacket(VRTPacket.PacketType,Long)}.
   *  @param type  The packet type.
   *  @param id    The packet class ID.
   *  @param p     The packet to initialize with (null if n/a).
   *  @param copy  Should a copy by made? (false if p is null).
   *  @return This will never return null, the result of an unknown packet
   *          may result in a new {@link BasicVRTPacket}.
   */
  protected VRTPacket getPacket (Class<? extends VRTPacket> clazz, PacketType type, Long id,
                                                                   VRTPacket p, boolean copy) {
    if (parent != null) {
      return (p == null)? parent.getPacket(type, id) : parent.getPacket(p, copy);
    }
    
    if (StandardDataPacket.isStandardDataPacket(id)) {
      return (p == null)? new StandardDataPacket() : new StandardDataPacket(p, copy);
    }
    if (p != null) {
      switch (type) {
        case UnidentifiedData:    return new BasicDataPacket(p, copy);
        case Data:                return new BasicDataPacket(p, copy);
        case UnidentifiedExtData: return new BasicDataPacket(p, copy);
        case ExtData:             return new BasicDataPacket(p, copy);
        case Context:             return new BasicContextPacket(p, copy);
        case ExtContext:          return new BasicVRTPacket(p, copy);
        default:                  return new BasicVRTPacket(p, copy);
      }
    }
    else {
      switch (type) {
        case UnidentifiedData:    return new BasicDataPacket();
        case Data:                return new BasicDataPacket();
        case UnidentifiedExtData: return new BasicDataPacket();
        case ExtData:             return new BasicDataPacket();
        case Context:             return new BasicContextPacket();
        case ExtContext:          return new BasicVRTPacket();
        default:                  return new BasicVRTPacket();
      }
    }
  }
}

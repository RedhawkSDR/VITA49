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

package nxm.vrt.lib;

import nxm.vrt.lib.VRTPacket.PacketType;

/** A factory used to associate a {@link VRTPacket} with its specific implementation instance.
 *
 *  @see VRTConfig#getPacketFactory()
 *  @see VRTConfig#setPacketFactory(PacketFactory)
 *  @author 
 */
public interface PacketFactory {
  /**  Gets the packet implementation class used for a given packet type and class.
   *  @param type  The packet type (VRT allows one Data/ExtData and one Context/ExtContext per class).
   *  @param id    The class ID of the packet.
   *  @return The applicable implementation class, or null if none found.
   */
  Class<? extends VRTPacket> getPacketClass (PacketType type, Long id);

  /** Gets a specific packet from the factory when given a generic packet.
   *  @param p    The existing (generic) packet.
   *  @param copy Must the data be copied? If true, the resulting packet must copy the data from
   *              <tt>p</tt> in such a way that no changes to the returned packet will alter
   *              <tt>p</tt>. If false, a "fast" initialization is used, meaning that the resulting
   *              packet may or may not be a copy of the given data, the implementation will use the
   *              fastest creation method available.
   *  @return The applicable packet. This will return null if initialization of the given
   *          <tt>type</tt> is not supported.
   *  @throws NullPointerException If  <tt>p</tt> is null.
   *  @throws IllegalArgumentException If initialization of the given type is supported,
   *          but the packet given does not match it.
   */
  VRTPacket getPacket (VRTPacket p, boolean copy);

  /** Gets a specific packet from the factory when given a generic packet.
   *  @param type  The packet type (VRT allows one Data/ExtData and one Context/ExtContext per class).
   *  @param id    The class ID of the packet.
   *  @return The applicable packet. This will return null if initialization of the given
   *          <tt>clazz</tt> is not supported.
   *  @throws IllegalArgumentException If initialization of the given type is supported,
   *          but <tt>data</tt> and/or <tt>id</tt> do not match it.
   */
  VRTPacket getPacket (PacketType type, Long id);
}

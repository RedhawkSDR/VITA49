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

package nxm.vrt.net;

import nxm.vrt.lib.*;
import java.util.EventObject;

/**  A simple extension of {@link EventObject}. This class is used rather than just using
 *  {@link EventObject} so that we can add functionality in the future if required.
 *
 *  @author         
 */
public final class VRTEvent extends EventObject {
  private static final long serialVersionUID = 0x2010020400000002L; // 2010-02-04 rev 00000002
  
  private final VRTPacket packet;

  /**  Creates a new instance.
   *  @param source The event source.
   */
  public VRTEvent (Object source) {
    this(source, null);
  }
  
  /**  Creates a new instance.
   *  @param source The event source.
   *  @param packet The VRT packet, if applicable.
   */
  public VRTEvent (Object source, VRTPacket packet) {
    super(source);
    this.packet = packet;
  }
  
  /**  Gets the packet associated with this error, if avaliable. Note that the packet may be
   *  invalid.
   *  @return The packet or null if not available
   */
  public VRTPacket getPacket () {
    return packet;
  }
}

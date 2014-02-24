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

import nxm.vrt.lib.VRTPacket;

import java.util.EventListener;

/**  A listener for VRT events.
 *
 *  @author         
 */
public interface VRTEventListener extends EventListener {
  
  /**  This method is called when a VRT packet has been received.
   *  @param e The event object.
   *  @param p The packet received (may be read-only).
   */
  public void receivedPacket (VRTEvent e, VRTPacket p);
  
  /**  This method is called when a VRT packet has been sent.
   *  @param e The event object.
   *  @param p The packet received (may be read-only).
   */
  public void sentPacket (VRTEvent e, VRTPacket p);
  
  /**  This method is called when an error condition is detected.
   *  @param e   The event object.
   *  @param msg A message describing the error condition.
   *  @param t   The error, if available.
   */
  public void errorOccurred (VRTEvent e, String msg, Throwable t);
  
  /**  This method is called when a warning condition is detected.
   *  @param e   The event object.
   *  @param msg A message describing the error condition.
   *  @param t   The error, if available.
   */
  public void warningOccurred (VRTEvent e, String msg, Throwable t);
  
}

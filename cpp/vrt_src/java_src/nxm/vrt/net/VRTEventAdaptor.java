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

/**  A listener for VRT events that just prints errors and warnings to {@link System#err}. Any
 *  notifications of a received and/or sent packet are ignored unless debug is turned on; if debug
 *  is turned on, packets sent/received will be printed to {@link System#out}. <br>
 *  <br>
 *   Users are encouraged to extend this class and override the {@link #receivedPacket} and
 *  {@link #sentPacket} methods.
 *
 *  @author         
 */
public class VRTEventAdaptor implements VRTEventListener {
  private final boolean debug;

  /**  Creates a new instance (debug turned off). */
  public VRTEventAdaptor () {
    this(false);
  }

  /**  Creates a new instance where debug output can optionally be turned on. */
  public VRTEventAdaptor (boolean debug) {
    this.debug = debug;
  }

  public void receivedPacket (VRTEvent e, VRTPacket p) {
    if (debug) receiveOrSendOccurred("ReceivedPacket", e, p);
  }
  
  public void sentPacket (VRTEvent e, VRTPacket p) {
    if (debug) receiveOrSendOccurred("SentPacket", e, p);
  }
  
  public void errorOccurred (VRTEvent e, String msg, Throwable t)  {
    errorOrWarningOccurred("ErrorOccurred", e, msg, t);
  }

  public void warningOccurred (VRTEvent e, String msg, Throwable t)  {
    errorOrWarningOccurred("WarningOccurred", e, msg, t);
  }

  /**  Displays an error/warning. */
  private void errorOrWarningOccurred (String type, VRTEvent e, String msg, Throwable t)  {
    StringBuilder str = new StringBuilder();

    str.append(type);

    if ((e != null) && (e.getSource() != null)) {
      str.append(" from ").append(e.getSource());
    }

    if ((msg == null) && (t != null)) {
      msg = t.getMessage();
    }
    str.append(": ").append(msg);

    if ((e != null) && (e.getPacket() != null)) {
      String pkt = null;
      try {
        pkt = e.getPacket().toString(); // if the problem was an invalid packet, this could error
      }
      catch (Exception ex) {
        pkt = null;
      }
      if (pkt != null) str.append(" [related to ").append(e.getPacket()).append("]");
    }

    System.err.println(str);
    if (t != null) t.printStackTrace(System.err);
  }

  /**  Displays an error/warning. */
  private void receiveOrSendOccurred (String type, VRTEvent e, VRTPacket p)  {
    StringBuilder str = new StringBuilder();

    str.append(type);

    if ((e != null) && (e.getSource() != null)) {
      str.append(" from ").append(e.getSource());
    }

    str.append(": ").append(p);

    System.out.println(str);
  }
}

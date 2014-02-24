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

import java.io.IOException;

/**  A multicast reader that is designed to translate packets from one form to another.
 *
 *  @author         
 */
public final class TranslatingReader extends MulticastReader {
  private static final boolean DEBUG = true;

  protected final PacketHandler packetTranslator;

  /**  Creates a new instance.
   *  @param host      The host to connect to (null = wildcard address).
   *  @param port      The port number.
   *  @param device    The device to connect to (e.g. "eth0", "eth1.101") (null = use default).
   *  @param warnings  The listener to handle any warnings. This is only used for handling warnings
   *                   during startup, and is discarded immediately following. If this is null, any
   *                   warnings will print to the console.
   *  @param lbuf      The buffer length to use.
   *  @throws IOException If an I/O exection occurrs.
   */
  public TranslatingReader (PacketHandler trans, String host, int port, String device,
                            VRTEventListener warnings, int lbuf) throws IOException {
    super(host, port, device, warnings, lbuf);
    this.packetTranslator = trans;
    start();
  }

  protected void handle (byte[] buffer, int length) throws IOException {
    try {
      packetTranslator.translate(listenerSupport, buffer, length);
    }
    catch (Exception e) {
      if (DEBUG) e.printStackTrace();
      throw new IOException("Error translating packet of type ", e);
    }
    catch (Error t) {
      if (DEBUG) t.printStackTrace();
      throw t;
    }
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // NESTED CLASSES
  //////////////////////////////////////////////////////////////////////////////////////////////////

  /**  Handles the translation of a raw packet stream into a set of {@link nxm.vrt.lib.VRTPacket VRTPackets}.
   *
   *  @author         
   */
  public interface PacketHandler {
    /**  Translates a raw packet to a {@link nxm.vrt.lib.VRTPacket}.
        @param vls    Listener support object, used to pass along errors/warnings.
        @param buffer The raw input data/packet.
        @param length The length of the input data/packet.
        @throws Exception If there is any exception in processing (this is optional since the
                handler can directly pass it to the <tt>vls</tt>.
     */
    public void translate (VRTListenerSupport vls, byte[] buffer, int length) throws Exception;
  }
}

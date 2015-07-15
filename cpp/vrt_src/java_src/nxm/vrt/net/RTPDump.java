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

/**  A basic tool for debugging RTP multicast streams.
 *
 *  @author         
 *  @deprecated     This never properly worked with RTP streams and just did UDP, hence it was
 *                  renamed {@link UDPDump}.
 */
@Deprecated
public class RTPDump {
  private RTPDump () { }

  @Deprecated
  public static void main (String ... args) {
    UDPDump.main(args);
  }
}

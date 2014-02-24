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
import java.net.BindException;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.MulticastSocket;
import java.net.NetworkInterface;
import java.net.UnknownHostException;

/**  Various utilities methods.
 *
 *  @author         
 */
public final class NetUtilities {
  private NetUtilities() { } // prevent instantiation
  
  /**  Maximum length of a UDP packet (including the UDP header) in bytes (65,635).
      @see #MAX_IPv4_UDP_LEN
   */
  public static final int MAX_UDP_LEN = 65635;
  
  /**  Maximum length of a UDP packet (including the UDP header) in bytes when transmitted
      over IPv4 (65,471). Although the max packet length for UDP is 65,635 bytes, IPv4 imposes a
      limit of 65,515 to 65,471 bytes based on the number of "options" set in the IPv4 header.
      Accordingly this value should be thought of as the maximum for transmission size and
      {@link #MAX_UDP_LEN} should be used as the maximum receive size.
      @see #MAX_UDP_LEN
   */
  public static final int MAX_IPv4_UDP_LEN = 65471;
  
  /**  The length of a UPD packet header in bytes (8). */
  public static final int UDP_HEADER_LENGTH = 8;

  /**  Gets the host name from a host:port.
      @param hostPort The host:port in "host:port", "host" or ":port" syntax.
      @return The applicable host name or null if not specified.
      @throws UnknownHostException If the host name is invalid.
   */
  public static InetAddress getHostAddress (String hostPort) throws UnknownHostException {
    String host = getHost(hostPort);
    return (host == null)? null : InetAddress.getByName(host);
  }

  /**  Gets the host name from a host:port.
      @param hostPort The host:port in "host:port", "host" or ":port" syntax.
      @return The applicable host name or null if not specified.
   */
  public static String getHost (String hostPort) {
    if (hostPort.startsWith("[")) {
      // IPv6 address
      int i = hostPort.indexOf("]");
      return hostPort.substring(1,i);
    }
    int i = hostPort.lastIndexOf(':');
    if (i <  0) return hostPort; // no port, just a host name
    if (i == 0) return null;     // no host, just a port number
    return hostPort.substring(0,i);
  }

  /**  Gets the port number from a host:port.
      @param hostPort The host:port in "host:port", "host" or ":port" syntax.
      @return The applicable port number or null if not specified.
   */
  public static Short getPort (String hostPort) {
    if (hostPort.startsWith("[")) {
      // IPv6 address
      int i = hostPort.indexOf("]:");
      if (i < 0) return null; // no port, just a host name
      return Short.parseShort(hostPort.substring(1,i));
    }
    int i = hostPort.lastIndexOf(':');
    if (i <  0) return null; // no port, just a host name
    return Short.parseShort(hostPort.substring(i+1));
  }

  /**  Creates a new datagram socket. The applicable datagram socket is initialized with
   *  the following parameters:
   *  <pre>
   *    SO_RCVBUF    = 8MiB      (ask for 8MiB, but will probably get closer to 128kiB)
   *    SO_SNDBUF    = 8MiB      (ask for 8MiB, but will probably get closer to 128kiB)
   *    SO_REUSEADDR = true
   *    SO_TIMEOUT   = 10000 ms
   *  </pre>
   *  The type of datagram socket is based on the input address. If the input address is a
   *  multicast address, {@link MulticastSocket} is used, otherwise {@link DatagramSocket} is
   *  used (<tt>address.getAddress().isMulticastAddress()</tt>).
   *  @param host      The host to connect to (null = wildcard address).
   *  @param port      The port number.
   *  @param device    The device to connect to (e.g. "eth0", "eth1.101") (null = use default).
   *  @param warnings  The listener to handle any warnings. This is only used for handling warnings
   *                   during startup, and is discarded immediately following. If this is null, any
   *                   warnings will print to the console.
   *  @param bufLength The buffer length in bytes.
   *  @return The socket.
   *  @throws IOException If an I/O error occurrs.
   */
  public static DatagramSocket createSocket (String host, int port, String device,
                                             VRTEventListener warnings, int bufLength) throws IOException {
    InetSocketAddress addr = (host == null)? new InetSocketAddress(port)
                                           : new InetSocketAddress(host,port);
    NetworkInterface  dev  = (device == null)? null : NetworkInterface.getByName(device);
    
    return createSocket(addr, dev, warnings, bufLength);
  }
  public static DatagramSocket createSocket (InetSocketAddress address, NetworkInterface device,
                                             VRTEventListener warnings, int bufLength) throws IOException {
    DatagramSocket  socket;
    MulticastSocket msocket;
    
    boolean multicast = (address.getAddress() != null) && address.getAddress().isMulticastAddress();

    //System.out.println("createSocket:");
    //System.out.println("  address   = "+address);
    //System.out.println("  device    = "+device);
    //System.out.println("  multicast = "+multicast);
    
    
    if (multicast) {
      // Java bugs 6347853 and 6218155 (which are actually references to Linux bugs) can cause a
      // BindException on some systems if -Djava.net.preferIPv4Stack=true is not set due to a bug
      // in the IPv6 protocol stack.
      try {
        if (false) {
          // work-around for Java Bug 4701650 on Windows
          socket = msocket = new MulticastSocket(address.getPort());
        }
        else {
          socket = msocket = new MulticastSocket(address);
        }
      }
      catch (BindException e) {
        if (System.getProperty("java.net.preferIPv4Stack") == null) {
          throw new IOException(""+e.getMessage()+" (some older Linux versions have a bug in their IPv6 "
                  + "protocol stack and require -Djava.net.preferIPv4Stack=true to be set to avoid it)", e);
        }
        else {
          throw e;
        }
      }
      
      if (device != null) {
        msocket.setNetworkInterface(device);
        msocket.setReuseAddress(true);
        msocket.joinGroup(address, device);
      }
      else {
        msocket.setReuseAddress(true);
        msocket.joinGroup(address.getAddress());
      }
    }
    else {
      socket = new DatagramSocket(address);
    }
    
    socket.setReceiveBufferSize(128*1024*1024); // <-- Ask for 128MiB, but will probably get less than this
    socket.setSendBufferSize(128*1024*1024);    // <-- Ask for 128MiB, but will probably get less than this
    socket.setReuseAddress(true);
    socket.setSoTimeout(10000);
    //System.out.println("  getReceiveBufferSize = "+socket.getReceiveBufferSize());
    //System.out.println("  getSendBufferSize = "+socket.getSendBufferSize());
    
    return socket;
  }

  /**  Converts a socket user-friendly definition to a string. */
  public static String toString (DatagramSocket socket) {
    StringBuilder str = new StringBuilder();
    str.append(socket.getLocalAddress());
    str.append(':');
    str.append(socket.getLocalPort());

    if (socket instanceof MulticastSocket) {
      MulticastSocket msocket = (MulticastSocket)socket;

      str.append(" (multicast)");
      try {
        NetworkInterface dev = msocket.getNetworkInterface();
        if (dev == null) str.append(" using default interface");
        else             str.append(" using ").append(dev.getName());
      }
      catch (Exception e) {
        str.append(" using unknown interface");
      }
    }
    else {
      str.append(" (unicast)");
    }
    return str.toString();
  }
}

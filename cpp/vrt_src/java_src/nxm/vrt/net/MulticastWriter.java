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
import java.io.Closeable;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetSocketAddress;
import java.util.Arrays;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.ThreadFactory;


/**  Writes VRT packets.
 *
 *  @author         
 */
public abstract class MulticastWriter<T> implements Closeable {
  public static final int PACKET_GAP = MulticastReader.PACKET_GAP;
  protected static final ThreadFactory THREAD_FACTORY = Utilities.newNamedThreadFactory("MulticastWriter");
  
  protected final ExecutorService    eventExecutor;
  protected final VRTListenerSupport listenerSupport;
  protected final byte[]             buffer;
  protected final DatagramPacket     datagram;
  protected final InetSocketAddress  address;
  protected final DatagramSocket     socket;
  
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
  protected MulticastWriter (String host, int port, String device, VRTEventListener warnings, int lbuf) throws IOException {
    if ((host   != null) && (host.length()   == 0)) host   = null;
    if ((device != null) && (device.length() == 0)) device = null;
    
    this.eventExecutor   = Executors.newSingleThreadExecutor(THREAD_FACTORY);
    this.listenerSupport = new VRTListenerSupport(this);
    this.address         = new InetSocketAddress(host,port);
    this.socket          = NetUtilities.createSocket(host, port, device, warnings, lbuf+PACKET_GAP);
    this.buffer          = new byte[lbuf+PACKET_GAP];
    this.datagram        = new DatagramPacket(buffer, 0, buffer.length);
  }
  
  /** Starts the writer. */
  protected void start () throws IOException {
    // nothing to do, yet!
  }
  
  /**  Provides a description of the instance useful for debugging.
   *  @return A debugging string.
   */
  @Override
  public String toString () {
    return getClass().getSimpleName()+" on "+NetUtilities.toString(socket);
  }
  
  /**  Gets a reference to the socket being used. This is provided so that 
   *  @return The socket being used.
   */
  public DatagramSocket getSocket () {
    return socket;
  }

  /**  Sends a packet. The actual transmitted packet will have the packet count updated as
   *  necessary, however the packet object passed in will not be altered. This method does not
   *  block until the packet has been sent. To see the actual packet sent and verification of
   *  sending, call <tt>addSentPacketListener(..)</tt> to add the appropriate listener.
   *  @param packet The packet to send.
   *  @throws IOException If an I/O error occurrs.
   */
  public abstract void sendPacket (final T packet) throws IOException;
  
  /**  Sends a set of packet in a logical frame or frames (where applicable). The actual
   *  transmitted packet will have the packet count updated as necessary, however the packet
   *  object passed in will not be altered. This method does not block until the packet has
   *  been sent. To see the actual packet sent and verification of sending, call
   *  <tt>addSentPacketListener(..)</tt> to add the appropriate listener.
   *  @param packets The packets to send.
   *  @throws IOException If an I/O error occurrs.
   */
  @SuppressWarnings("unchecked")
  public void sendPackets (final T ... packets) throws IOException {
    for (T p : packets) {
      sendPacket(p);
    }
  }
  
  @SuppressWarnings("unchecked")
  protected void sendDatagram (int len, T ... packets) {
    try {
      if (len < 0) throw new IllegalArgumentException("Illegal length ("+len+").");
      datagram.setAddress(address.getAddress());
      datagram.setPort(address.getPort());
      datagram.setLength(len);
      socket.send(datagram);
    }
    catch (Exception e) {
      listenerSupport.fireErrorOccurred("Unable to send "+Arrays.toString(packets), e);
    }
  }
  
  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Implement Closeable
  //////////////////////////////////////////////////////////////////////////////////////////////////
  
  public void close () {
    eventExecutor.shutdown();
    listenerSupport.shutdown();
    socket.close();
  }
  
  //////////////////////////////////////////////////////////////////////////////////////////////////
  // BinaryMulticastWriter
  //////////////////////////////////////////////////////////////////////////////////////////////////
  
  /**  Creates a new instance that writes a <tt>byte[]</tt> holding the packet data.
   *  @param hostPort  The host:port to connect to.
   *  @param device    The device to connect to (e.g. "eth0", "eth1.101") (null = use default).
   *  @param warnings  The listener to handle any warnings. This is only used for handling warnings
   *                   during startup, and is discarded immediately following. If this is null, any
   *                   warnings will print to the console.
   *  @throws IOException If an I/O exection occurrs.
   */
  public static MulticastWriter<byte[]> getBinaryMulticastWriter (String hostPort, String device,
                                                                  VRTEventListener warnings) throws IOException {
    //return getBinaryMulticastWriter(DataSourceLookupTable.getHost(hostPort), DataSourceLookupTable.getPort(hostPort), device, warnings);
    return getBinaryMulticastWriter(NetUtilities.getHost(hostPort), NetUtilities.getPort(hostPort), device, warnings);
  }

  /**  Creates a new instance that writes a <tt>byte[]</tt> holding the packet data.
   *  @param host      The host to connect to (null = wildcard address).
   *  @param port      The port number.
   *  @param device    The device to connect to (e.g. "eth0", "eth1.101") (null = use default).
   *  @param warnings  The listener to handle any warnings. This is only used for handling warnings
   *                   during startup, and is discarded immediately following. If this is null, any
   *                   warnings will print to the console.
   *  @throws IOException If an I/O exection occurrs.
   */
  public static MulticastWriter<byte[]> getBinaryMulticastWriter (String host, int port, String device,
                                                                  VRTEventListener warnings) throws IOException {
    return new MulticastWriter<byte[]>(host, port, device, warnings, NetUtilities.MAX_UDP_LEN) {
      @Override
      public void sendPacket (byte[] packet) throws IOException {
        System.arraycopy(packet, 0, buffer, 0, packet.length);
        sendDatagram(packet.length, packet);
      }
      @Override
      public String toString () {
        return "BinaryMulticastWriter on "+NetUtilities.toString(socket);
      }
    };
  }
  
  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Event Handling
  //////////////////////////////////////////////////////////////////////////////////////////////////
  
  /**  Adds a listener to listen to the given messages. If <tt>classID</tt> is non-null, only the
   *  packets matching the given class ID will be passed on to the listener. If <tt>classID</tt> is
   *  null, all packets (including those without a class ID) will be passed on to the listener.
   *  @param l       The listener.
   *  @param classID The class ID of the messages to listen to (null=listen to all).
   *  @throws NullPointerException if the listener is null.
   */
  public void addSentPacketListener (VRTEventListener l, Long classID) {
    listenerSupport.addSentPacketListener(l, classID);
  }
  /**  Removes a given listener's binding.
   *  @param l       The listener.
   *  @param classID The class ID of the messages to listen to (null=listen to all).
   *  @throws NullPointerException if the listener is null.
   */
  public void removeSentPacketListener (VRTEventListener l, Long classID) {
    listenerSupport.removeSentPacketListener(l, classID);
  }
  /**  Removes all listeners. */
  public void removeAllSentPacketListeners () {
    listenerSupport.removeAllSentPacketListeners();
  }
  /**  Gets all active listeners matching the given class ID.
   *  @param classID The class ID of the messages to listen to (null=listen to all).
   */
  public List<VRTEventListener> getSentPacketListeners (Long classID) {
    return listenerSupport.getSentPacketListeners(classID);
  }
  /**  Gets all active listeners. */
  public List<VRTEventListener> getAllSentPacketListeners () {
    return listenerSupport.getAllSentPacketListeners();
  }
  
  
  /**  Adds a listener to listen to the given messages. If <tt>classID</tt> is non-null, only the
   *  packets matching the given class ID will be passed on to the listener. If <tt>classID</tt> is
   *  null, all packets (including those without a class ID) will be passed on to the listener.
   *  @param l The listener.
   *  @throws NullPointerException if the listener is null.
   */
  public void addErrorWarningListener (VRTEventListener l) {
    listenerSupport.addErrorWarningListener(l);
  }
  /**  Removes all listeners. */
  public void removeAllErrorWarningListeners () {
    listenerSupport.removeAllErrorWarningListeners();
  }
  /**  Removes a given listener's binding.
   *  @param l The listener.
   *  @throws NullPointerException if the listener is null.
   */
  public void removeErrorWarningListener (VRTEventListener l) {
    listenerSupport.removeErrorWarningListener(l);
  }
  /**  Gets all active listeners. */
  public List<VRTEventListener> getAllErrorWarningListeners () {
    return listenerSupport.getAllErrorWarningListeners();
  }
}

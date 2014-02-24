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
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.PortUnreachableException;
import java.net.SocketTimeoutException;
import java.util.ArrayDeque;
import java.util.List;

import static nxm.vrt.lib.Utilities.sleep;
import static nxm.vrt.net.NetUtilities.MAX_UDP_LEN;

/**  Reads VRT packets.
 *
 *  @author         
 */
public class MulticastReader extends Threadable {
  /** @deprecated Use {@link #QUEUE_LIMIT_PACKETS} or {@link #QUEUE_LIMIT_OCTETS}. */
  @Deprecated
  public static final int QUEUE_LIMIT = 1024;
  
  public  static final int PACKET_GAP          = 32;
  public  static final int QUEUE_LIMIT_PACKETS = 1024;
  public  static final int QUEUE_LIMIT_OCTETS  = 8388608; // 8,388,608 = 8 MiB

  protected final VRTListenerSupport      listenerSupport;
  private   final byte[]                  buffer;
  private   final DatagramPacket          datagram;
  private            DatagramSocket          socket;
  private   final    String                  host;
  private   final    int                     port;
  private   final    String                  device;
  private            VRTEventListener        reconnectWarnings;
  private   volatile boolean                 reconnectNow;
  private   volatile boolean                 reconnectDone;
  private            IOException             reconnectError;
  private   final    ArrayDeque<byte[]>      packetsReadIn;
  private            int                     lengthReadIn;
  private   final    MulticastHandler        multicastHandler;

  /**  Creates a new instance.
   *  @param host      The host to connect to (null = wildcard address).
   *  @param port      The port number.
   *  @param device    The device to connect to (e.g. "eth0", "eth1.101") (null = use default).
   *  @param warnings  The listener to handle any warnings. This is only used for handling warnings
   *                   during startup, and is discarded immediately following. If this is null, any
   *                   warnings will print to the console.
   *  @throws IOException If an I/O execution occurs.
   */
  public MulticastReader (String host, int port, String device, VRTEventListener warnings) throws IOException {
    this(host, port, device, warnings, -1);
  }
  
  /**  Creates a new instance.
   *  @param host      The host to connect to (null = wildcard address).
   *  @param port      The port number.
   *  @param device    The device to connect to (e.g. "eth0", "eth1.101") (null = use default).
   *  @param warnings  The listener to handle any warnings. This is only used for handling warnings
   *                   during startup, and is discarded immediately following. If this is null, any
   *                   warnings will print to the console.
   *  @param lbuf      The buffer length to use.
   *  @throws IOException If an I/O execution occurs.
   */
  protected MulticastReader (String host, int port, String device, VRTEventListener warnings, int lbuf) throws IOException {
    if ((host   != null) && (host.length()   == 0)) host   = null;
    if ((device != null) && (device.length() == 0)) device = null;
    
    if (lbuf < 0) {
      lbuf = MAX_UDP_LEN;
    }
    this.listenerSupport = new VRTListenerSupport(this);
    this.buffer          = new byte[lbuf+PACKET_GAP];
    this.datagram        = new DatagramPacket(buffer, 0, buffer.length);
    this.socket            = NetUtilities.createSocket(host, port, device, warnings, buffer.length);
    this.host              = host;
    this.port              = port;
    this.device            = device;
    this.packetsReadIn     = new ArrayDeque<byte[]>(QUEUE_LIMIT_PACKETS);
    this.lengthReadIn      = 0;
    this.multicastHandler  = new MulticastHandler(this);
  }
  
  /**  Provides a description of the instance useful for debugging.
   *  @return A debugging string.
   */
  @Override
  public String toString () {
    return getClass().getSimpleName()+" on "+NetUtilities.toString(socket);
  }
  
  /** Gets a reference to the socket being used.
   *  @return The socket being used.
   */
  public synchronized DatagramSocket getSocket () {
    return socket;
  }
  
  /** Requests a reconnect and waits for it to be done. */
  public void reconnect (VRTEventListener warnings) throws IOException {
    // Only do the parts we absolutely need within a synchronized block since
    // the actual work is going to be done on another thread.
    synchronized (this) {
      if (reconnectNow) {
        throw new RuntimeException("Already trying to reconnect");
      }
      reconnectWarnings = warnings;
      reconnectError    = null;
      reconnectNow      = true;
      reconnectDone     = false;
    }

    while (!stopNow()) {
      synchronized (this) {
        if (reconnectDone) {
          IOException err = reconnectError;

          reconnectWarnings = null;
          reconnectError    = null;
          reconnectNow      = false;
          reconnectDone     = false;

          if (err != null) throw err;    // done (error)
          else             return;       // done (success)
        }
      }
      sleep(100);
    }
  }

  @Override
  public void start () {
    super.start();
    multicastHandler.start();
  }

  @Override
  public void stop (boolean wait) {
    multicastHandler.stop(wait);
    super.stop(wait);
  }

  @Override
  protected void shutdown () {
    listenerSupport.shutdown();
    socket.close();
    clearQueue(); // <-- release any memory with the packets we were holding
  }
  
  @Override
  public void runThread () {
    int waitForPort = 500;
    boolean reconnectNeeded = false;
    
    while (!stopNow()) {
      try {
        // RECONNECT, IF REQUESTED
        while ((reconnectNow && !reconnectDone) || reconnectNeeded) {
          synchronized (this) {
            try {
              socket.close();
              socket          = NetUtilities.createSocket(host, port, device, reconnectWarnings, buffer.length);
              reconnectDone   = true;
              reconnectNeeded = false;
            }
            catch (IOException e) {
              reconnectError = e;
              reconnectDone  = true;
            }

            if (reconnectNeeded) {
              sleep(waitForPort);
              waitForPort = Math.min(10000, waitForPort*2);
          }
        }
        }

        // RECEIVE THE PACKET AND ADD IT TO THE QUEUE
        pushPacket(receivePacket());
        waitForPort = 500; // reset
      }
      catch (SocketTimeoutException e) {
        // this is normal
      }
      catch (PortUnreachableException e) {
        listenerSupport.fireErrorOccurred("Error while reading socket", e);
        sleep(waitForPort);
          waitForPort = Math.min(10000, waitForPort*2);
        }
      catch (IOException e) {
        listenerSupport.fireErrorOccurred("Error while reading socket", e);
      }
    }
  }
  
  /** Handles the packet read in.
   *  @param buffer The data read in.
   *  @param length The number of bytes read in. <i>[This is here for backwards-compatibility,
   *                it is always called with a value equal to <tt>buffer.length</tt>.]</i>
   */
  protected void handle (byte[] buffer, int length) throws IOException {
    // does nothing
  }

  /**  Reads a single packet from the input stream. This method can not be used when the
      reader is running as a thread.
      @return The packet received.
      @throws IOException If an exception occurs, common exceptions include
                          {@link SocketTimeoutException} and {@link PortUnreachableException}.
   */
  public byte[] receivePacket () throws IOException {
    datagram.setLength(buffer.length); // reset length since receive changes it
    socket.receive(datagram);

    int    len = datagram.getLength();
    byte[] buf = new byte[len];
    System.arraycopy(buffer, 0, buf, 0, len);

    return buf;
  }
  
  /** Adds a packet to the processing queue. */
  private void pushPacket (byte[] p) {
    synchronized (packetsReadIn) {
      if ((packetsReadIn.size() > QUEUE_LIMIT_PACKETS) && (lengthReadIn > QUEUE_LIMIT_OCTETS)) {
        listenerSupport.fireWarningOccurred("Incoming multicast packet queue full",
               new QueueFullException(this, "Incoming multicast packet queue full dropping "
                                          + packetsReadIn.size() + " packets totalling "+lengthReadIn
                                          + " octets", packetsReadIn.size()));
        packetsReadIn.clear();
        lengthReadIn = 0;
      }
      packetsReadIn.add(p);
      lengthReadIn += p.length;
    }
  }

  /** Pops a packet off of the queue. */
  private byte[] popPacket () {
    byte[] p = null;
    synchronized (packetsReadIn) {
      p = packetsReadIn.poll();
      if (p != null) {
        lengthReadIn -= p.length;
      }
    }
    return p;
  }

  /** Pops a packet off of the queue. */
  private void clearQueue () {
    synchronized (packetsReadIn) {
      packetsReadIn.clear();
      lengthReadIn = 0;
    }
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
  public void addReceivedPacketListener (VRTEventListener l, Long classID) {
    listenerSupport.addReceivedPacketListener(l, classID);
  }
  /**  Removes a given listener's binding.
   *  @param l       The listener.
   *  @param classID The class ID of the messages to listen to (null=listen to all).
   *  @throws NullPointerException if the listener is null.
   */
  public void removeReceivedPacketListener (VRTEventListener l, Long classID) {
    listenerSupport.removeReceivedPacketListener(l, classID);
  }
  /**  Removes all listeners. */
  public void removeAllReceivedPacketListeners () {
    listenerSupport.removeAllReceivedPacketListeners();
  }
  /**  Gets all active listeners matching the given class ID.
   *  @param classID The class ID of the messages to listen to (null=listen to all).
   */
  public List<VRTEventListener> getReceivedPacketListeners (Long classID) {
    return listenerSupport.getReceivedPacketListeners(classID);
  }
  /**  Gets all active listeners. */
  public List<VRTEventListener> getAllReceivedPacketListeners () {
    return listenerSupport.getAllReceivedPacketListeners();
  }

  @Deprecated
  public void addReveivedPacketListener (VRTEventListener l, Long classID) {
    addReceivedPacketListener(l, classID);
  }
  @Deprecated
  public void removeReveivedPacketListener (VRTEventListener l, Long classID) {
    removeReceivedPacketListener(l, classID);
  }
  @Deprecated
  public void removeAllReveivedPacketListeners () {
    removeAllReceivedPacketListeners();
  }
  @Deprecated
  public List<VRTEventListener> getReveivedPacketListeners (Long classID) {
    return getReceivedPacketListeners(classID);
  }
  public List<VRTEventListener> getAllReveivedPacketListeners () {
    return getAllReceivedPacketListeners();
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

  /** Handles the handling of packets on the queue. */
  private static class MulticastHandler extends Threadable {
    private final MulticastReader reader;

    public MulticastHandler (MulticastReader reader) {
      this.reader = reader;
    }
    
    public void runThread () {
      while (!stopNow() && !reader.stopNow()) {
        try {
          byte[] p = reader.popPacket();

          if (p != null) {
            reader.handle(p, p.length);
          }
          else {
            sleep(10);
          }
        }
        catch (Exception e) {
          reader.listenerSupport.fireErrorOccurred("Error while processing packet", e);
        }
      }
    }
  }
}

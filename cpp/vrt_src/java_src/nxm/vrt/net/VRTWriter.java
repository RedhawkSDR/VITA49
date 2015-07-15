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
import nxm.vrt.lib.VRTPacket;
import java.io.IOException;
import java.util.Iterator;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicInteger;

import static nxm.vrt.lib.VRLFrame.MAX_FRAME_LENGTH;
import static nxm.vrt.lib.VRLFrame.VRL_FAW_0;
import static nxm.vrt.lib.VRLFrame.VRL_FAW_1;
import static nxm.vrt.lib.VRLFrame.VRL_FAW_2;
import static nxm.vrt.lib.VRLFrame.VRL_FAW_3;
import static nxm.vrt.lib.VRLFrame.NO_CRC_0;
import static nxm.vrt.lib.VRLFrame.NO_CRC_1;
import static nxm.vrt.lib.VRLFrame.NO_CRC_2;
import static nxm.vrt.lib.VRLFrame.NO_CRC_3;
import static nxm.vrt.lib.VRTPacket.MAX_PACKET_LENGTH;
import static nxm.vrt.net.NetUtilities.MAX_IPv4_UDP_LEN;

/**  Writes VRT packets which may optionally be included in VRL frames. The choice between
 *  un-framed VRT packets and using VRL frames must be made at the time the class is instantiated.
 *  If frames are used, there is also an option to use CRC protection for the frames.
 *
 *  @author         
 */
public final class VRTWriter extends MulticastWriter<VRTPacket> {
  /**  Self-imposed point at which a frame is split into two frames. */
  private static final int FRAME_BREAK = MAX_IPv4_UDP_LEN & 0xFFFFF000;
  
  /**  Packet counters. */
  private final ConcurrentHashMap<Long,AtomicInteger> packetCounters = new ConcurrentHashMap<Long,AtomicInteger>();

  /**  Frame counter. */
  private final AtomicInteger frameCounter = new AtomicInteger(-1);
  
  private final boolean   crc;
  private final BasicVRLFrame  frame;
  private final BasicVRTPacket packet;
  
  @Deprecated
  public VRTWriter (String hostPort, String device, VRTEventListener warnings) throws IOException {
    this(hostPort, device, warnings, false, false);
  }

  @Deprecated
  public VRTWriter (String host, int port, String device, VRTEventListener warnings) throws IOException {
    this(host, port, device, warnings, false, false);
  }
  
  /**  Creates a new instance.
   *  @param hostPort  The host:port to connect to.
   *  @param device    The device to connect to (e.g. "eth0", "eth1.101") (null = use default).
   *  @param warnings  The listener to handle any warnings. This is only used for handling warnings
   *                   during startup, and is discarded immediately following. If this is null, any
   *                   warnings will print to the console.
   *  @param useFrames Should packets be sent embedded in VRL frames?
   *  @param useCRC    Should frames provide CRC protection? (This parameter only applies if frames
   *                   are in use.)
   *  @throws IOException If an I/O execution occurs.
   */
  public VRTWriter (String hostPort, String device, VRTEventListener warnings,
                    boolean useFrames, boolean useCRC) throws IOException {
    //this(DataSourceLookupTable.getHost(hostPort), DataSourceLookupTable.getPort(hostPort),
    //     device, warnings, useFrames, useCRC);
    this(NetUtilities.getHost(hostPort), NetUtilities.getPort(hostPort),
         device, warnings, useFrames, useCRC);
  }

  /**  Creates a new instance.
   *  @param host      The host to connect to (null = wildcard address).
   *  @param port      The port number.
   *  @param device    The device to connect to (e.g. "eth0", "eth1.101") (null = use default).
   *  @param warnings  The listener to handle any warnings. This is only used for handling warnings
   *                   during startup, and is discarded immediately following. If this is null, any
   *                   warnings will print to the console.
   *  @param useFrames Should packets be sent embedded in VRL frames?
   *  @param useCRC    Should frames provide CRC protection? (This parameter only applies if frames
   *                   are in use.)
   *  @throws IOException If an I/O execution occurs.
   */
  public VRTWriter (String host, int port, String device, VRTEventListener warnings,
                    boolean useFrames, boolean useCRC) throws IOException {
    super(host, port, device, warnings, (useFrames)? MAX_FRAME_LENGTH : MAX_PACKET_LENGTH);

    if (useFrames) {
      // Need to init the header and trailer of the frame so it is valid
      buffer[ 0] = VRL_FAW_0;  buffer[ 1] = VRL_FAW_1;  buffer[ 2] = VRL_FAW_2;  buffer[ 3] = VRL_FAW_3;
      buffer[ 4] = 0;          buffer[ 5] = 0;          buffer[ 6] = 0;          buffer[ 7] = 3;
      buffer[ 8] = NO_CRC_0;   buffer[ 9] = NO_CRC_1;   buffer[10] = NO_CRC_2;   buffer[11] = NO_CRC_3;

      this.crc    = useCRC;
      this.frame  = new BasicVRLFrame(buffer, 0, buffer.length, false, true);
      this.packet = null;
    }
    else {
      this.crc    = false;
      this.frame  = null;
      this.packet = new BasicVRTPacket(buffer, 0, buffer.length, false, true);
    }
    start();
  }
  
  /**  Updates the counter in the given packet. */
  private void updateCounter (VRTPacket p) {
    long code = p.getStreamCode();

    AtomicInteger counter = packetCounters.get(code);

    if (counter == null) {
      packetCounters.putIfAbsent(code, new AtomicInteger(0)); // first packet sent
      counter = packetCounters.get(code);
    }

    int count = counter.getAndIncrement();
    p.setPacketCount(count % 16);
  }

  @Override
  public void sendPacket (VRTPacket p) throws IOException {
    if (packet == null) {
      sendPackets(p);
      return;
    }

    // SANITY CHECKS
    if (p == null) throw new NullPointerException("Can not send null.");
    String err = p.getPacketValid(true);
    if (err != null) throw new IllegalArgumentException(err);

    // FIT PACKET INTO DATAGRAM
    p.readPacket(buffer);

    // UPDATE COUNTERS (our copy only)
    updateCounter(packet);

    // SEND THE PACKET
    super.sendDatagram(packet.getPacketLength(), packet);

    // REPORT SENT PACKET
    listenerSupport.fireSentPacket(packet);
  }

  @Override
  public void sendPackets (VRTPacket ... packets) throws IOException {
    if (frame == null) {
      super.sendPackets(packets);
      return;
    }

    // FIT PACKETS INTO FRAME/DATAGRAM
    int count = frame.setVRTPackets(FRAME_BREAK, packets);
    if (count < 1) {
      frame.setVRTPackets(MAX_FRAME_LENGTH, packets);
      count = 1;
    }

    // UPDATE COUNTERS (our copy only) AND CRC (if applicable)
    Iterator<VRTPacket> packetsInFrame = frame.iteratorRW();
    while (packetsInFrame.hasNext()) {
      updateCounter(packetsInFrame.next());
    }
    frame.setFrameCount(frameCounter.incrementAndGet() & 0xFFF);
    if (crc) frame.updateCRC();

    // SEND THE FRAME
    super.sendDatagram(frame.getFrameLength(), packets);

    // REPORT SENT PACKETS
    for (VRTPacket p : frame) {
      listenerSupport.fireSentPacket(p);
    }

    // SEND ANY PACKETS THAT DID NOT FIT IN THIS FRAME
    if (count < packets.length) {
      VRTPacket[] next = new VRTPacket[packets.length-count];
      System.arraycopy(packets, count, next, 0, next.length);
      sendPackets(next);
    }
  }
  
  /**  Pipes the output from the specified reader and dumps it out from this writer. Typical
   *  usage is to create a XYZ to VRT translation service:
   *  <pre>
   *    XYZReader r = XYZReader.startReader(..);
   *    VRTWriter w = VRTWriter.startWriter(..);
   * 
   *    r.addErrorWarningListener(this);     // assumes this implements VRTEventListener
   *    w.addErrorWarningListener(this);     // assumes this implements VRTEventListener
   *    
   *    w.pipeFrom(r);
   *    
   *    // wait until done
   *    
   *    r.close();
   *    w.close();
   *  </pre>
   *  @param r The reader to read from.
   */
  public void pipeFrom (MulticastReader r) {
    Translator t = new Translator();
    r.addErrorWarningListener(t);
    r.addReceivedPacketListener(t, null);
  }
  
  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Nested classes
  //////////////////////////////////////////////////////////////////////////////////////////////////
  /**  Provides translation support for VRT packets. */
  private class Translator implements VRTEventListener {
    public void receivedPacket (VRTEvent e, VRTPacket p) {
      try {
        sendPacket(p);
      }
      catch (IOException ex) {
        listenerSupport.fireErrorOccurred("Error sending "+p, ex);
      }
    }

    public void sentPacket (VRTEvent e, VRTPacket p) { }
    public void errorOccurred (VRTEvent e, String msg, Throwable t) { }
    public void warningOccurred (VRTEvent e, String msg, Throwable t) { }
  }
}

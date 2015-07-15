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

package nxm.vita.prim;

import nxm.sys.lib.*;
import nxm.vrt.lib.*;
import nxm.vrt.lib.VRTPacket.PayloadFormat;
import nxm.vrt.net.VRTReader;
import nxm.vrt.net.VRTEvent;
import nxm.vrt.net.VRTEventListener;

import java.io.IOException;
import java.util.Map;
import java.util.LinkedHashMap;

import static java.util.Collections.synchronizedMap;
import static nxm.vita.inc.VRTIO.toTimeObject;
import static nxm.vita.inc.VRTIO.writeVRTPacket;
import static nxm.vita.prim.sinkvrt.parseStreamID;

/**  The <tt>SOURCEVRT</tt> primitive.

    @author         
 */
public class sourcevrt extends Primitive implements VRTEventListener {
  private static final byte[] EMPTY = new byte[4096];

  private          String                    hostPort;
  private          String                    device;
  private          boolean                   fillMode;
  private          boolean                   force1000;
  private          boolean                   packetMode;
  private final    byte[]                    dataBuffer    = new byte[VRTPacket.MAX_PACKET_LENGTH];
  private final    Map<Integer,PacketStream> pipeForStream = synchronizedMap(new LinkedHashMap<Integer,PacketStream>());
  private          PacketStream              pipeForAll;
  private          VRTReader                 packetReader;
  private volatile boolean                   opening;
  private volatile Integer                   openStream;
  private volatile ContextPacket             openPacket;
  private volatile boolean                   running;

  @Override
  public int open () {
    hostPort     = MA.getS("HOSTPORT");
    device       = MA.getS("/DEVICE", null);
    fillMode     = MA.getState("/FILL");
    force1000    = MA.getState("/FORCE1000");
    packetMode   = MA.getState("/PACKET");

    // OPEN THE SOCKET
    try {
      
      packetReader = new VRTReader(hostPort, device, this);
      packetReader.addErrorWarningListener(this);
      packetReader.addReceivedPacketListener(this, null);
      packetReader.start();
    }
    catch (IOException e) {
      throw new MidasException("Unable to open connection to "+hostPort, e);
    }

    if (packetMode && fillMode) {
      throw new MidasException("/PACKET and /FILL are mutually-exclusive switches");
    }

    // OPEN THE PIPES
    for (int i = 2; i <= MA.getNArgs(); i++) {
      String key = MA.getKey(i);
      boolean all = key.equals("ALLSTREAMS");
        
      if (all || key.startsWith("STREAM")) {
        Integer streamID = (all)? null : parseStreamID(key, MA.getU(i));
        
        // Grab a context packet for the stream
        openStream = streamID;
        openPacket = null;
        opening    = true;
        
System.out.println("DEBUG: Checking format for stream "+streamID);
        while (opening) {
          if (getInterruptX()) return ABORT;
          Time.sleep(0.1);
        }
System.out.println("DEBUG: Got packet "+openPacket);
        
        PacketStream ps = new PacketStream(openPacket, MA.getDataFile(i), streamID, force1000, packetMode);
        
        if (all) pipeForAll = ps;
        else     pipeForStream.put(streamID, ps);
      }
      else {
        throw new MidasException("Invalid parameter "+key+"="+MA.getU(i));
      }
    }

    return NORMAL;
  }

  /**  User interrupt detected? This should be replaces with a getInterrupt() call in the
      future, see NeXtMidas DR 526828-3.
   */
  private boolean getInterruptX () {
    return (M.terminal.getInterrupt() != 0) || (isPiped && (M.pipeMode == Midas.POFF));
  }

  @Override
  public int process () {
    running = true;
    // processing handled in other threads
    return SLEEP;
  }

  @Override
  public int close () {
    // CLOSE SOCKET
    running = false;
    packetReader.close();

    // CLOSE PIPES
    if (pipeForAll != null) {
      pipeForAll.close();
      pipeForAll = null;
    }
    for (PacketStream ps : pipeForStream.values()) {
      ps.close();
    }
    pipeForStream.clear();

    return NORMAL;
  }

  /**  Writes data from the packet to the pipe. */
  private void writeToPipe (PacketStream ps, VRTPacket p) {
    if (ps == null) return;

    if (packetMode) {
      writeVRTPacket(ps.pipe, p);
    }
    else if (p instanceof DataPacket) {
      DataPacket    dp = (DataPacket)p;
      TimeStamp     ts = p.getTimeStamp();
      PayloadFormat pf = ps.context.getDataPayloadFormat();
      
      if (ts == null) {
        M.warning("No TimeStamp in "+p);
        return;
      }
      if (fillMode) {
        long timeDeltaSec   = ts.getSecondsUTC()  - ps.nextTimeStamp.getSecondsUTC();
        long timeDeltaPS    = ts.getPicoSeconds() - ps.nextTimeStamp.getPicoSeconds() + (timeDeltaSec * TimeStamp.ONE_SEC);
        int  missingSamples = (int)(timeDeltaPS / (ps.context.getSampleRate() * TimeStamp.ONE_SEC));

        if (missingSamples == 0) {
          // Perfect
        }
        else if (missingSamples > 0) {
          // Missing data (need to fill)
          int missingBytes = missingSamples * pf.getDataItemSize()/8;

          while (missingBytes > EMPTY.length) {
            ps.pipe.write(EMPTY, 0, EMPTY.length);
            missingBytes -= EMPTY.length;
          }
          ps.pipe.write(EMPTY, 0, missingBytes);
        }
        else {
          // Extra data (issue warning and discard packet)
          M.warning("Expected to receive packet with TimeStamp="+ps.nextTimeStamp+" but got TimeStamp="
                   +ts+" dropping packet. This error is usually caused by an improperly-reported "
                   +"SampleRate.");
          return;
        }

        double dt = ps.context.getSampleRate() * dp.getDataLength(pf); // seconds
        ps.nextTimeStamp = ts.addPicoSeconds((long)(dt / TimeStamp.ONE_SEC));
      }
      else {
        ps.pipe.setTimeAt(toTimeObject(ts).getSec()); // <-- Loss of accuracy
      }

      dp.getData(pf, dataBuffer, 0);
      ps.pipe.write(dataBuffer, 0, p.getPayloadLength());
    }
    else if (p instanceof ContextPacket) {
      ContextPacket cp = (ContextPacket)p;

      if (!ps.isCompatibleWith(cp)) {
        ps.reopenVrtToBlue(cp); // trigger a pipe restart and reopen with the new parameters
      }
    }
  }
  

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Implement VRTEventListener
  //////////////////////////////////////////////////////////////////////////////////////////////////
  @Override
  public void receivedPacket (VRTEvent e, VRTPacket p) {
    Integer streamID = p.getStreamIdentifier();
    if (streamID == null) streamID = 0;
    
    if (opening && (p instanceof ContextPacket)
                && ((openStream == null) || (streamID.equals(openStream)))) {
      // Looks for context packet describing the given stream
      openPacket = (ContextPacket)p;
      opening    = false;
    }

    if (running) {
      // Write data to pipes
      writeToPipe(pipeForAll,                  p);
      writeToPipe(pipeForStream.get(streamID), p);
    }
  }
  
  @Override
  public void sentPacket (VRTEvent e, VRTPacket p) {
    // not applicable
  }

  @Override
  public void errorOccurred (VRTEvent e, String msg, Throwable t) {
    if (t != null) M.printStackTrace("SOURCEVRT: "+msg, t);
    else           M.warning("SOURCEVRT: "+msg);
  }

  @Override
  public void warningOccurred (VRTEvent e, String msg, Throwable t) {
    if (t != null) M.printStackTrace("SOURCEVRT: "+msg, t);
    else           M.warning("SOURCEVRT: "+msg);
  }
}

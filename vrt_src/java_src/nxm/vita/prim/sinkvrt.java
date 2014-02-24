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
import nxm.vrt.lib.VRTPacket.PacketType;
import nxm.vrt.lib.VRTPacket.PayloadFormat;
import nxm.vrt.net.VRTWriter;
import nxm.vrt.net.VRTEvent;
import nxm.vrt.net.VRTEventListener;

import java.io.IOException;
import java.util.Iterator;
import java.util.Map;
import java.util.LinkedHashMap;
import java.util.concurrent.atomic.AtomicInteger;

import static nxm.vita.inc.VRTIO.isReady;
import static nxm.vita.inc.VRTIO.readVRTPacket;
import static nxm.vita.inc.VRTIO.toTimeStamp;
import static java.util.Collections.synchronizedMap;

/**  The <tt>SINKVRT</tt> primitive.

    @author         
 */
public class sinkvrt extends Primitive implements VRTEventListener {
  /**  Maximum transfer length in bytes. This is equal to:
      <pre>
        MAX_XFER = round((max packet length) - (UDP header length) - MAX_HEADER_LENGTH - MAX_TRAILER_LENGTH)

        Where round(..) rounds to the next lowest multiple of 4-bytes or 8-bytes
      </pre>
       Although the max packet length for UDP is 65,635 bytes, IPv4 imposes a limit of 65,515 to
      65,471 bytes based on the number of "options" set in the IPv4 header.
   */
  private static final int MAX_XFER = (65471 - 8 - VRTPacket.MAX_HEADER_LENGTH - VRTPacket.MAX_TRAILER_LENGTH) & 0xFFF8;

  /**  Default transfer length in bytes. This is equal to MAX_XFER rounded to the nearest 1024 bytes. */
  private static final int DEF_XFER = MAX_XFER & 0xFC00;

  private final    byte[]                    dataBuffer    = new byte[VRTPacket.MAX_PACKET_LENGTH];
  private final    Map<Integer,PacketStream> pipeForStream = synchronizedMap(new LinkedHashMap<Integer,PacketStream>());
  private final    AtomicInteger             numRead       = new AtomicInteger(0);
  private          VRTWriter                 packetWriter;
  private          boolean                   packetMode;
  private          int                       contextFrequency;

  @Override
  public int open () {
    // OPEN THE SOCKET
    String hostPort   = MA.getS("HOSTPORT");
    String device     = MA.getS("/DEVICE", null);
    boolean useFrames = MA.getState("/VRL", false);
    boolean useCRC    = MA.getState("/CRC", false);
    try {
      packetMode   = MA.getState("/PACKET");
      packetWriter = new VRTWriter(hostPort, device, this, useFrames, useCRC);

      packetWriter.addErrorWarningListener(this);
      packetWriter.addSentPacketListener(this, null);
    }
    catch (IOException e) {
      throw new MidasException("Unable to open connection to "+hostPort, e);
    }

    // OPEN THE PIPES
    boolean force1000 = MA.getState("/FORCE1000");

    xfer             = MA.getL("/TL", DEF_XFER);
    packetMode       = MA.getState("/PACKET");
    contextFrequency = MA.getL("/CONTEXTFREQ", 16);

    if ((xfer < 8) || (xfer > MAX_XFER) || ((xfer&0x7) != 0)) {
      // technically it could be a multiple of 4, if only ever using data sizes <=32-bits
      throw new MidasException("Illegal value /TL="+xfer+" (must be between 8 and "+MAX_XFER+" and a multiple of 8)");
    }
    if (contextFrequency < 1) {
      throw new MidasException("Illegal value /CONTEXTFREQ="+contextFrequency);
    }

    for (int i = 2; i <= MA.getNArgs(); i++) {
      String key = MA.getKey(i);
      boolean all = key.equals("ALLSTREAMS");

      if (all || key.startsWith("STREAM")) {
        Integer streamID = (all)? null : parseStreamID(key, MA.getU(i));
        
        PacketStream ps = new PacketStream(MA.getDataFile(i), streamID, force1000, packetMode, xfer);
        pipeForStream.put(streamID, ps);
      }
      else {
        throw new MidasException("Invalid parameter "+key+"="+MA.getU(i));
      }
    }
    return NORMAL;
  }

  @Override
  public int process () {
    int stat = NOOP;

    Iterator<PacketStream> streams = pipeForStream.values().iterator();
    while (streams.hasNext()) {
      PacketStream ps = streams.next();

      if (packetMode) {
        if (!isReady(ps.pipe)) continue; // not ready for reading yet
        
        VRTPacket p = readVRTPacket(ps.pipe, numRead);
        if (p == null) {
          if (numRead.get() == CoreIO.REOPEN) {
            // reopened (pickup new header on next pass)
            ps.reopenBlueToVrt();
            stat = NORMAL;
            continue;
          }
          else {
            // End of file/pipe reached, close it
            ps.close();
            streams.remove();
            continue;
          }
        }
        
        try {
          packetWriter.sendPacket(p);
        }
        catch (Exception e) {
          M.printStackTrace("Can not send "+p, e);
        }
        stat = NORMAL;
      }
      else {
        if (!ps.pipe.isReady(ps.transferLengthElem)) continue; // not ready for reading yet

        int num = ps.pipe.read(dataBuffer, 0, ps.transferLengthBytes);
        if (num < 0) {
          if (num == CoreIO.REOPEN) {
            // reopened (pickup new header on next pass)
            ps.reopenBlueToVrt();
            stat = NORMAL;
            continue;
          }
          else {
            // End of file/pipe reached, close it
            ps.close();
            streams.remove();
            continue;
          }
        }
        double        t  = ps.pipe.getTimeAt();
        TimeStamp     ts = toTimeStamp(t);
        PayloadFormat pf = ps.context.getDataPayloadFormat();
        DataPacket    p  = (true)? new StandardDataPacket(pf)
                                 : new BasicDataPacket();

        p.setPacketType(PacketType.Data);
        p.setTimeStamp(ts);
        p.setStreamIdentifier(ps.streamID);
        p.setData(pf, dataBuffer, 0, num);

        if (ps.newContext || (ps.counter == 0)) {
          // INCLUDE REGULAR RE-SEND OF CONTEXT PACKET
          if (!ps.newContext) ps.context.resetForResend(ts);
          else                ps.context.setTimeStamp(ts);
          try {
            packetWriter.sendPackets(ps.context, p);
          }
          catch (Exception e) {
            M.printStackTrace("Can not send "+ps.context+" and/or "+p, e);
          }
          ps.newContext = false;
          ps.counter    = 0;
        }
        else {
          // JUST SEND DATA PACKET
          try {
            packetWriter.sendPacket(p);
          }
          catch (Exception e) {
            M.printStackTrace("Can not send "+p, e);
          }
          ps.counter = (ps.counter + 1) % contextFrequency;
        }
      }
      stat = NORMAL;
    }
    return (pipeForStream.isEmpty())? FINISH : stat;
  }

  @Override
  public int close () {
    // CLOSE SOCKET
    packetWriter.close();

    // CLOSE PIPES
    for (PacketStream ps : pipeForStream.values()) {
      ps.close();
    }
    pipeForStream.clear();

    return NORMAL;
  }

  @Override
  public int restart () {
    // Restarts handled individually for each pipe in process()
    return NORMAL;
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Implement VRTEventListener
  //////////////////////////////////////////////////////////////////////////////////////////////////
  @Override
  public void receivedPacket (VRTEvent e, VRTPacket p) {
    // not applicable
  }

  @Override
  public void sentPacket (VRTEvent e, VRTPacket p) {
    // not that important
  }

  @Override
  public void errorOccurred (VRTEvent e, String msg, Throwable t) {
    if (t != null) M.printStackTrace("SINKVRT: "+msg, t);
    else           M.warning("SINKVRT: "+msg);
  }

  @Override
  public void warningOccurred (VRTEvent e, String msg, Throwable t) {
    if (t != null) M.printStackTrace("SINKVRT: "+msg, t);
    else           M.warning("SINKVRT: "+msg);
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Methods Shared with SOURCEVRT
  //////////////////////////////////////////////////////////////////////////////////////////////////

  /**  Converts "STREAMn" to n with applicable error checking. */
  static int parseStreamID (String key, String val) { // shared with SOURCEVRT
    try {
      long streamID = Long.parseLong(key.substring(6)); // use parse long since ID is unsigned
      if ((streamID >= 0) && (streamID <= 0xFFFFFFFFL)) {
        return (int)streamID;
      }
    }
    catch (NumberFormatException e) {
      // handled below
    }
    throw new MidasException("Invalid stream number "+key.substring(6)+" in "+key+"="+val);
  }
}

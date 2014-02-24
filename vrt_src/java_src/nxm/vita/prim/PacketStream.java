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

import nxm.sys.inc.Units;
import nxm.sys.lib.DataFile;
import nxm.sys.lib.MidasException;
import nxm.vrt.lib.*;
import nxm.vrt.lib.VRTPacket.PayloadFormat;

import java.io.Closeable;

import static nxm.vita.inc.VRTIO.contextPacketFor;
import static nxm.vita.inc.VRTIO.toBlueFormat;
import static nxm.vita.inc.VRTIO.toTimeObject;

/**  <b>Internal use only:</b> Used internal to {@link sourcevrt} and {@link sinkvrt}.

    @author         
 */
final class PacketStream implements Closeable {
           DataFile      pipe;
           ContextPacket context;
           double        transferLengthElem;  // Transfer length in elements
           int           transferLengthBytes; // Transfer length in bytes
  final    int           defTransferLength;   // Default transfer length in bytes
  final    Integer       streamID;
  final    boolean       packetMode;
  final    boolean       force1000;
  volatile TimeStamp     nextTimeStamp;
  volatile int           counter;
  volatile boolean       newContext;

  /**  Creates a new BLUE-to-VRT stream and opens the input pipe.
      @param in         The input pipe (not open yet).
      @param streamID   The VRT stream ID.
      @param force1000  Force treatment as a Type 1000 file?
      @param packetMode Use packet mode?
      @param xfer       Default transfer length in bytes.
   */
  public PacketStream (DataFile in, Integer streamID, boolean force1000, boolean packetMode, int xfer) {
    this.pipe                 = in;
    this.context              = new BasicContextPacket();
    this.transferLengthBytes  = -1;
    this.transferLengthElem   = -1;
    this.defTransferLength    = xfer;
    this.streamID             = streamID;
    this.packetMode           = packetMode;
    this.force1000            = force1000;
    this.nextTimeStamp        = null;
    this.counter              = 0;
    this.newContext           = true;
    openBlueToVrt();
  }

  /**  Creates a new VRT-to-BLUE stream and opens the output pipe.
      @param in        The context packet defining the input VRT stream.
      @param out       The output pipe (not open yet).
      @param streamID  The VRT stream ID.
      @param force1000 Force treatment as a Type 1000 file?
      @param packetMode Use packet mode?
   */
  public PacketStream (ContextPacket in, DataFile out, Integer streamID, boolean force1000, boolean packetMode) {
    this.pipe                 = out;
    this.context              = in;
    this.transferLengthBytes  = -1;
    this.transferLengthElem   = -1;
    this.defTransferLength    = -1;
    this.streamID             = streamID;
    this.packetMode           = packetMode;
    this.force1000            = force1000;
    this.nextTimeStamp        = null;
    this.counter              = 0;
    this.newContext           = true;
    openVrtToBlue();
  }

  /**  Re-opens a VRT-to-BLUE stream. */
  public void reopenVrtToBlue (ContextPacket in) {
System.out.println("DEBUG: reopen "+pipe.getName());
    pipe.setFlags(DataFile.REOPEN);

    // ---- This block based on DataFile.reOpen()
    pipe.close();
    Object ref = pipe.cmd;
    if (ref==null) ref = pipe.M;
    DataFile df = new DataFile(ref, pipe.fn);
    df.flags = pipe.flags;
    // ---- end block

    pipe       = df;
    context    = in;
    newContext = true;
    openVrtToBlue();
  }


  /**  Re-opens a BLUE-to-VRT stream. */
  public void reopenBlueToVrt () {
System.out.println("DEBUG: reopen "+pipe.getName());
    //pipe.setFlags(DataFile.REOPEN);

    // ---- This block based on DataFile.reOpen()
    pipe.close();
    Object ref = pipe.cmd;
    if (ref==null) ref = pipe.M;
    DataFile df = new DataFile(ref, pipe.fn);
    df.flags = pipe.flags;
    // ---- end block

    pipe       = df;
    newContext = true;
    openBlueToVrt();
  }

  /**  Opens a VRT-to-BLUE stream. */
  private void openVrtToBlue () {
    Double        sr = context.getSampleRate();
    PayloadFormat pf = context.getDataPayloadFormat();
    TimeStamp     ts = context.getTimeStamp();

    if (sr == null) throw new NullPointerException("Missing SampleRate in "+context);
    if (pf == null) throw new NullPointerException("Missing DataPayloadFormat in "+context);
    if (ts == null) throw new NullPointerException("Missing TimeStamp in "+context);

    int fs = (force1000)? 0 : pf.getVectorSize();
    if (fs == 1) fs = 0;

    String stream = (streamID == null)? "ALL" : Long.toString(streamID & 0xFFFFFFFFL);

    if (packetMode) {
      pipe.setType(1049);
      pipe.setFormat("SB");
      pipe.setDataRep("IEEE");
      pipe.setComment("Raw VRT packets from stream "+stream);
    }
    else {
      pipe.setTime(toTimeObject(ts));
      pipe.setFormat(toBlueFormat(pf));
      pipe.setComment("VRT data packets from stream "+stream);
      pipe.setFrameSize(fs);
      pipe.setSize(Double.POSITIVE_INFINITY);
      pipe.setUnits(Units.TIME_S);
      pipe.setStart(0.0);
      pipe.setDelta(1.0 / sr); // Hz -> sec

      if (fs > 1) {
        // Type 2000 file, set secondary axis
        Double freq = context.getFrequencyIF();
        Double bw   = context.getBandwidth();

        if ((freq != null) && (bw != null)) {
          pipe.setXUnits(Units.FREQUENCY_HZ);
          pipe.setXStart(freq - bw/2);
          pipe.setXDelta(bw / fs);
        }
      }

      char type = (char)pipe.getFormatType();
      if ((type == 'P') || (type == 'N')) {
        // Bit and Nibble data still packed IEEE format within the byte
        pipe.setDataRep("IEEE");
      }
    }
    pipe.open(DataFile.OUTPUT);
System.out.println("DEBUG: context = "+context);
  }

  /**  Opens a BLEU-to-VRT stream. */
  private void openBlueToVrt () {
    pipe.open(DataFile.INPUT);
    if (force1000) pipe.setFrameSize(0);

    context       = contextPacketFor(pipe);
    nextTimeStamp = context.getTimeStamp();

    if (packetMode) {
      transferLengthBytes = 4;
    }
    else if (pipe.getTypeCodeClass() == 2) {
      int frameSize = (int)pipe.getBPE(); // FrameSize in bytes
      transferLengthBytes = (defTransferLength / frameSize) * frameSize;
      if (transferLengthBytes <= 0) {
        throw new MidasException("Transfer length of "+defTransferLength+" bytes is less than one frame in "+pipe.getName());
      }
    }
    else {
      transferLengthBytes = defTransferLength;
    }
    transferLengthElem = transferLengthBytes / pipe.getBPE();
System.out.println("DEBUG: context = "+context);
  }

  @Override
  public void close () {
    pipe.close();
  }

  /**  Printable description of the object for debugging purposes. */
  @Override
  public String toString () {
    String stream = (streamID == null)? "ALL" : Long.toString(streamID & 0xFFFFFFFFL);
    return "PacketStream: pipe="+pipe.getName()
                    +" context="+context
                     +" stream="+stream
                 +" packetMode="+packetMode
              +" nextTimeStamp="+nextTimeStamp;
  }

  /**  Checks to see if the given context packet is compatible with the current stream. */
  public boolean isCompatibleWith (ContextPacket p) {
    if (!context.getDataPayloadFormat().equals(p.getDataPayloadFormat())) return false;
    if (!context.getSampleRate().equals(p.getSampleRate())) return false;

    // other checks???
    return true;
  }

}

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

package nxm.vita.inc;

import java.util.concurrent.atomic.AtomicInteger;
import java.io.InputStream;
import java.io.OutputStream;

import nxm.sys.lib.BaseFile;
import nxm.sys.lib.DataFile;
import nxm.sys.lib.Time;
import nxm.sys.lib.MidasException;

import nxm.vrt.lib.BasicContextPacket;
import nxm.vrt.lib.ContextPacket;
import nxm.vrt.lib.TimeStamp;
import nxm.vrt.lib.BasicVRTPacket;
import nxm.vrt.lib.BasicVRLFrame;
import nxm.vrt.lib.VRTConfig;
import nxm.vrt.lib.VRTPacket;
import nxm.vrt.lib.VRTPacket.DataItemFormat;
import nxm.vrt.lib.VRTPacket.PayloadFormat;
import nxm.vrt.lib.VRTPacket.RealComplexType;
import nxm.vrt.lib.VRLFrame;

/**  Provides a few NeXtMidas-like functions that support I/O of VRT packets similar to that used
 *  in the core NeXtMidas I/O libraries.
 *
 *  @author         
 */
public final class VRTIO {
  /** Delta between J1970 and J1950 epochs. */
  public static final int J1970TOJ1950       = 631152000; // (7305 days) * (86,400 sec/day)

  private VRTIO () { } // prevent instantiation
  
  /**  This function checks to see if a file or pipe is ready for reading a VRTPacket. This is
   *  a small wrapper over the {@link DataFile#isReady(double)} method in NeXtMidas.
   *  @param in The input file.
   *  @return true if ready for a call to {@link #readVRTPacket(DataFile)}, false if not yet ready.
   */
  public static boolean isReady (DataFile in) {
    double numElements = 4 / in.getBPE();
    return in.isReady(numElements);
  }

  /**  This function reads a VRT packet from a file or pipe. This is a small wrapper over the
   *  {@link DataFile#read(byte[],int,int)} method in NeXtMidas. This method should not be mixed
   *  with other file read calls as it takes care to update the internal values as appropriate to
   *  maintain synch between packets. This method will block waiting for a packet to come in, if
   *  this is undesirable, the {@link #isReady(DataFile)} method can be used to see if a read can
   *  be done without waiting; note however that {@link #isReady(DataFile)} only checks to see if
   *  the start of a packet is available and this method may need to wait for the rest of the packet
   *  (packet writes should be nearly atomic, so this should rarely be an issue). <br>
   *  <br>
   *  <b>Users are encouraged to use the <tt>readVRLFrame(..)</tt> function in place of this one
   *  since it supports both VRT packets and VRL frames in the input stream.</b>
   *  @param in  The input file.
   *  @return The packet read in. The packet will be "null" if the end-of-file is reached
   *          (similar to <tt>read(..)</tt> returning -1) before a packet is seen.
   *  @throws RuntimeException If an incomplete or blatantly invalid packet is read in.
   */
  public static VRTPacket readVRTPacket (DataFile in) {
    return readVRTPacket(in, null);
  }

  /**  This function reads a VRT packet from a file or pipe. This is identical to
   *  {@link #readVRTPacket(DataFile)}, except that it reports back the number of bytes read
   *  and/or the EOF code from {@link DataFile}. <br>
   *  <br>
   *  <b>Users are encouraged to use the <tt>readVRLFrame(..)</tt> function in place of this one
   *  since it supports both VRT packets and VRL frames in the input stream.</b>
   *  @param in      The input file.
   *  @param numRead The number of bytes read or the EOF code value. (OUTPUT)
   *  @return The packet read in. The packet will be "null" if the end-of-file is reached
   *          (similar to <tt>read(..)</tt> returning -1) before a packet is seen.
   *  @throws RuntimeException If an incomplete or blatantly invalid packet is read in.
   */
  public static VRTPacket readVRTPacket (DataFile in, AtomicInteger numRead) {
    try {
      byte[] firstFour = new byte[4];
      int num = in.read(firstFour, 0, 4);
    if (num < 0) {
      if (numRead != null) numRead.set(num);
      return null; // EOF
    }

      BasicVRTPacket p = new BasicVRTPacket();
      numRead.set(p.readPacketFrom(BaseFile_getInputStream(in), firstFour, 0));
      return VRTConfig.getPacket(p, false); // <-- resolves the exact packet class
    }
    catch (Exception e) {
      throw new MidasException("Error while reading VRT packet from "+in.getName(), e);
    }
  }

  /** This function reads a VRL frame from a file or pipe. This is a small wrapper over the
   *  {@link DataFile#read(byte[],int,int)} method in NeXtMidas. This method should not be mixed
   *  with other file read calls as it takes care to update the internal values as appropriate to
   *  maintain synch between packets. This method will block waiting for a frame to come in, if
   *  this is undesirable, the {@link #isReady(DataFile)} method can be used to see if a read can
   *  be done without waiting; note however that {@link #isReady(DataFile)} only checks to see if
   *  the start of a frame is available and this method may need to wait for the rest of the frame
   *  (frame writes should be nearly atomic, so this should rarely be an issue).
   *  @param in  The input file.
   *  @return The frame read in. The frame will be "null" if the end-of-file is reached
   *          (similar to <tt>read(..)</tt> returning -1) before a frame is seen.
   *  @throws RuntimeException If an incomplete or blatantly invalid packet is read in.
   */
  public static VRLFrame readVRLFrame (DataFile in) {
    return readVRLFrame(in, null);
  }

  /** This function reads a VRL frame from a file or pipe. This is identical to
   *  {@link #readVRTPacket(DataFile)}, except that it reports back the number of bytes read
   *  and/or the EOF code from {@link DataFile}.
   *  @param in      The input file.
   *  @param numRead The number of bytes read or the EOF code value. (OUTPUT)
   *  @return The frame read in. The frame will be "null" if the end-of-file is reached
   *          (similar to <tt>read(..)</tt> returning -1) before a frame is seen.
   *  @throws RuntimeException If an incomplete or blatantly invalid packet is read in.
   */
  public static VRLFrame readVRLFrame (DataFile in, AtomicInteger numRead) {
    try {
      byte[] firstFour = new byte[4];
      int num = in.read(firstFour, 0, 4);
      if (num < 0) {
        if (numRead != null) numRead.set(num);
        return null; // EOF
      }

      BasicVRLFrame f = new BasicVRLFrame();
      numRead.set(f.readFrameFrom(BaseFile_getInputStream(in), firstFour, 0));
      return f;
    }
    catch (Exception e) {
      throw new MidasException("Error while reading VRT packet from "+in.getName(), e);
    }
  }

  /** This function writes a VRT packet to a file or pipe. This is a small wrapper over the
   *  {@link DataFile#write(byte[],int,int)} method in NeXtMidas. This method should not be mixed
   *  with other file write calls as it takes care to update the internal values as appropriate to
   *  maintain synch between packets.
   *  @param out  The output file.
   *  @param p    The packet to write.
   *  @return The number of bytes written.
   *  @throws RuntimeException If a blatantly invalid packet is passed in.
   */
  public static int writeVRTPacket (DataFile out, VRTPacket p) {
    try {
      // OutputStream os = out.getOutputStream();       // See DR xxxx
      OutputStream os = BaseFile_getOutputStream(out);
      p.writePacketTo(os);
      return p.getPacketLength();
    }
    catch (Exception e) {
      throw new MidasException("Error while writing VRT packet to "+out.getName(), e);
    }
    }

  /** This function writes a VRL frame to a file or pipe. This is a small wrapper over the
   *  {@link DataFile#write(byte[],int,int)} method in NeXtMidas. This method should not be mixed
   *  with other file write calls as it takes care to update the internal values as appropriate to
   *  maintain synch between packets.
   *  @param out  The output file.
   *  @param f    The frame to write.
   *  @return The number of bytes written.
   *  @throws RuntimeException If a blatantly invalid packet is passed in.
   */
  public static int writeVRLFrame (DataFile out, VRLFrame f) {
    try {
      // OutputStream os = out.getOutputStream();       // See DR xxxx
      OutputStream os = BaseFile_getOutputStream(out);
      f.writeFrameTo(os);
      return f.getFrameLength();
    }
    catch (Exception e) {
      throw new MidasException("Error while writing VRL frame to "+out.getName(), e);
    }
  }

  /**  Converts a TimeStamp to a UTC Time object.
      @param ts The time stamp.
      @return The Time object or null if not supported.
   */
  public static Time toTimeObject (TimeStamp ts) {
    if (ts == null) return null;

    if ((ts.getEpoch() == TimeStamp.GPS_EPOCH) || (ts.getEpoch() == TimeStamp.UTC_EPOCH)) {
      if (ts.getFractionalMode() == TimeStamp.FractionalMode.None) {
        double wsec = ts.getUTCSeconds() + J1970TOJ1950;
        double fsec = 0.0;
        return new Time(wsec, fsec);
      }
      if (ts.getFractionalMode() == TimeStamp.FractionalMode.RealTime) {
        double wsec = ts.getUTCSeconds() + J1970TOJ1950;
        double fsec = ts.getPicoSeconds() / ((double)TimeStamp.ONE_SEC);
        return new Time(wsec, fsec);
      }
    }
    return null; // unsupported
  }

  /**  Converts a Midas time object in UTC to a TimeStamp.
      @param t The Midas time in UTC.
      @return The TimeStamp object.
   */
  public static TimeStamp toTimeStamp (Time t) {
    if (t == null) return null;

    long sec = (long)(t.getWSec() - J1970TOJ1950);
    long ps  = (long)(t.getFSec() * TimeStamp.ONE_SEC);

    return new TimeStamp(TimeStamp.IntegerMode.UTC, sec, ps);
  }

  /**  Converts a Midas time in UTC to a TimeStamp.
      @param t The Midas time in UTC.
      @return The TimeStamp object.
   */
  public static TimeStamp toTimeStamp (double t) {
    return toTimeStamp(new Time(t));
  }

  /**  Gets a VRT format specifier matching a given BLUE format specifier.
      @param mode      The Midas mode. Must be scalar ('S' or '1') or complex ('C' or '2'), other
                       modes are not supported.
      @param type      The Midas type. Must be double ('D'), float ('F'), long ('X'), int ('L'),
                       short ('I'), byte ('B'), nibble ('N') or bit ('P'). (Nibble and bit are
                       assumed to be big-endian notation within the byte.)
      @param frameSize The frame size (must be less than 65536, though any size where the total
                       number of octets per frame exceeds 32768 is strongly discouraged).
      @return The VRT format specifier.
      @throws IllegalArgumentException If the format specifier is not convertible to a VRT format.
   */
  public static PayloadFormat toVrtFormat (char mode, char type, int frameSize) {
    PayloadFormat pf = new PayloadFormat();

    switch (mode) {
      case 'S': pf.setRealComplexType(RealComplexType.Real            ); break;
      case 'C': pf.setRealComplexType(RealComplexType.ComplexCartesian); break;
      case '1': pf.setRealComplexType(RealComplexType.Real            ); break; // Assume 'S'
      case '2': pf.setRealComplexType(RealComplexType.ComplexCartesian); break; // Assume 'C'
      default:  throw new IllegalArgumentException("Unsupported format "+mode+type);
    }

    int dSize;
    switch (type) {
      case 'D': pf.setDataItemFormat(DataItemFormat.Double     ); dSize = 64; break;
      case 'F': pf.setDataItemFormat(DataItemFormat.Float      ); dSize = 32; break;
      case 'X': pf.setDataItemFormat(DataItemFormat.SignedInt  ); dSize = 64; break;
      case 'L': pf.setDataItemFormat(DataItemFormat.SignedInt  ); dSize = 32; break;
      case 'I': pf.setDataItemFormat(DataItemFormat.SignedInt  ); dSize = 16; break;
      case 'B': pf.setDataItemFormat(DataItemFormat.SignedInt  ); dSize =  8; break;
      case 'N': pf.setDataItemFormat(DataItemFormat.SignedInt  ); dSize =  4; break;
      case 'P': pf.setDataItemFormat(DataItemFormat.UnsignedInt); dSize =  1; break;
      default:  throw new IllegalArgumentException("Unsupported format "+mode+type);
    }
    pf.setDataItemSize(dSize);
    pf.setItemPackingFieldSize(dSize);

    if (frameSize <= 0) frameSize = 1; // <-- Type 1000 files may use FS=0
    pf.setVectorSize(frameSize);
    return pf;
  }

  /**  Gets a BLUE format specifier matching a given VRT format specifier.
      @param pf The payload format.
      @return The applicable BLUE format specifier giving mode and type.
      @throws IllegalArgumentException If the format specifier is not convertible to a BLUE format.
   */
  public static String toBlueFormat (PayloadFormat pf) {
    char mode;
    char type;

    if (pf.getRepeatCount() != 1) {
      throw new IllegalArgumentException("Unsupported payload format "+pf);
    }

    switch (pf.getRealComplexType()) {
      case Real:             mode = 'S'; break;
      case ComplexCartesian: mode = 'C'; break;
      default: throw new IllegalArgumentException("Unsupported payload format "+pf);
    }

    int dSize = pf.getDataItemSize();
    int pSize = pf.getItemPackingFieldSize();

    if (pSize != dSize) {
      // may choose to support this in the future
      throw new IllegalArgumentException("Unsupported payload format "+pf);
    }

    switch (pf.getDataItemFormat()) {
      case Double:         type = 'D'; break;
      case Float:          type = 'F'; break;
      case SignedInt:
        if (dSize ==  1) { type = 'P'; break; } // Allow 1-bit signed and unsigned as SP
        if (dSize ==  4) { type = 'N'; break; }
        if (dSize ==  8) { type = 'B'; break; }
        if (dSize == 16) { type = 'I'; break; }
        if (dSize == 32) { type = 'L'; break; }
        if (dSize == 64) { type = 'X'; break; }
        throw new IllegalArgumentException("Unsupported payload format "+pf);
      case UnsignedInt:
        if (dSize ==  1) { type = 'P'; break; } // Allow 1-bit signed and unsigned as SP
        throw new IllegalArgumentException("Unsupported payload format "+pf);
      default: throw new IllegalArgumentException("Unsupported payload format "+pf);
    }
    return ""+mode+type;
  }

  /**  Creates a VRT context packet that matches a BLUE file. This only supports the basic Type
      1000 and Type 2000 BLUE files with scalar and complex data types and small frame size.
      @param in The input file.
      @return The corresponding context packet.
      @throws IllegalArgumentException If it if not possible to create a VRT context packet for the
                                       given input file/pipe.
   */
  public static ContextPacket contextPacketFor (DataFile in) {
    int           fs = in.getFrameSize();
    PayloadFormat pf = toVrtFormat((char)in.getFormatMode(), (char)in.getFormatType(), fs);
    TimeStamp     ts = toTimeStamp(in.getTime());
    double        xd = in.getDelta();

    BasicContextPacket context = new BasicContextPacket();
    context.setDataPayloadFormat(pf);
    context.setSampleRate(1.0 / xd); // sec -> Hz
    context.setTimeStamp(ts);
    context.setChangePacket(true);

    if (fs > 1) {
      // Get parameters for secondary axis
      double bw = fs * in.getXDelta();
      double xs = in.getXStart();
      context.setFrequencyIF(xs + bw/2);
      context.setBandwidth(bw);
    }
    return context;
  }


  //////////////////////////////////////////////////////////////////////////////////////////////////
  // FUNCTIONS THAT BELONG ELSEWHERE
  //////////////////////////////////////////////////////////////////////////////////////////////////

  /** Gets an InputStream for this file. This is a wrapper on the basic
      {@link BaseFile#read(byte[],int,int)} function and works no different than calling it. As
      such, users should expect similar performance, such as only reading from the "data section"
      for some files. The {@link IOResource#getInputStream()} is available for situations where
      these restrictions may be unacceptable. <br>
      <br>
      <b>Note that the object returned from this function call is NOT thread-safe.</b> As an
      optimization, multiple calls to this function MAY return a pointer to the same InputStream
      object.
      @return An InputStream for this file.
   */
  private static InputStream BaseFile_getInputStream (BaseFile bf) {
    InputStream inputStream = null;

    if (inputStream == null) {
      final BaseFile self = bf;

      inputStream = new InputStream() {
        final byte[] tempBuf = new byte[1];

        @Override
        public String toString () {
          return "InputStream for "+self;
        }
        
        @Override
        public int available ()  {
          return (int)Math.min(Integer.MAX_VALUE, self.avail());
        }

        @Override
        public int read () {
          int count = read(tempBuf, 0, 1);
          return (count < 0)? count : tempBuf[0];
        }

        @Override
        public int read (byte[] buf, int off, int len) {
          return self.read(buf, off, len);
        }

        @Override
        public void close() {
          self.close();
        }
      };
    }
    return inputStream;
  }

  /** Gets an OutputStream for this file. This is a wrapper on the basic
      {@link BaseFile#write(byte[],int,int)} function and works no different than calling it. As
      such, users should expect similar performance, such as only writing to the "data section"
      for some files. The {@link IOResource#getOutputStream()} is available for situations where
      these restrictions may be unacceptable. <br>
      <br>
      <b>Note that the object returned from this function call is NOT thread-safe.</b> As an
      optimization, multiple calls to this function MAY return a pointer to the same OutputStream
      object.
      @return An OutputStream for this file.
   */
  private static OutputStream BaseFile_getOutputStream (BaseFile bf) {
    OutputStream outputStream = null;

    if (outputStream == null) {
      final BaseFile self = bf;

      outputStream = new OutputStream() {
        final byte[] tempBuf = new byte[1];

        @Override
        public String toString () {
          return "OutputStream for "+self;
        }

        @Override
        public void write (int b) {
          tempBuf[0] = (byte)b;
          self.write(tempBuf, 0, 1);
        }

        @Override
        public void write (byte[] buf, int off, int len) {
          self.write(buf, off, len);
        }

        @Override
        public void close() {
          self.close();
        }
      };
    }
    return outputStream;
  }
}

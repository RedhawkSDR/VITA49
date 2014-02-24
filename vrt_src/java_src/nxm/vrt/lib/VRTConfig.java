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

package nxm.vrt.lib;

import java.io.IOException;
import java.io.InputStream;
import nxm.vrt.lib.VRTPacket.PacketType;

import static nxm.vrt.lib.Utilities.toBooleanValue;
import static nxm.vrt.lib.VRLFrame.VRL_FAW_0;
import static nxm.vrt.lib.VRLFrame.VRL_FAW_1;
import static nxm.vrt.lib.VRLFrame.VRL_FAW_2;
import static nxm.vrt.lib.VRLFrame.VRL_FAW_3;

/** Global settings for the VRT package. Most of these can be controlled from the <tt>java</tt>
 *  command line with the following properties:
 *  <pre>
 *    vrt.dslt_service=[host:port]          - The host:port for the default Data Source Lookup
 *
 *    vrt.dslt_device=[device]              - The device (e.g. "eth0") to use for the default Data
 *                                            Source Lookup Table (DSLT) service. If unspecified the
 *                                            the default (null) device is used.
 *
 *    vrt.ignore_env_var=[true/false]       - Ignore environment variables and only check the
 *                                            properties on the java command line (default is
 *                                            false). <i>(This capability was added Jan 2012,
 *                                            setting this to true matches the previous ver.)</i>
 *
 *    vrt.leap_seconds=[file]               - The name of the leap seconds file to use (must be a
 *                                            path relative to the CLASSPATH). The default is
 *                                            "nxm/vrt/dat/tai-utc.dat". <i>(Prior to Jan 2012
 *                                            this defaulted to "nxm/vrt/dat/tai_utc_data.txt".)</i>
 *
 *    vrt.norad_ls_counted=[true/false]     - Turns on/off the default mode for counting mid-year
 *                                            leap seconds in NORAD time (default is on).
 *
 *    vrt.packet_factory=[factory]          - Sets the packet factory. The [factory] must be the
 *                                            fully-qualified class name and the class must have
 *                                            a no-argument constructor. (This can be altered at
 *                                            run time.)
 *
 *    vrt.packet_factory=[f0]:[f1]:...:[fN] - Sets the packet factory. Each entry must be the
 *                                            fully-qualified name of a packet factory. The zero'th
 *                                            entry the list must have a no-argument constructor.
 *                                            Subsequent entries must have a one-argument
 *                                            constructor that takes in a parent PacketFactory as
 *                                            its argument. (This can be altered at run time.)
 *
 *    vrt.quick_test=[true/false]           - Turns on/off "quick test" mode which skips some of
 *                                            the loop iterations in some of the tests in order
 *                                            to speed up the testing process (default is on).
 *                                            <i>(This is only applicable to the running of the
 *                                            internal unit tests.)</i>
 *
 *    vrt.strict=[true/false]               - Turns on/off strict checking of input packets
 *                                            (default is off).
 *
 *    vrt.version=[V49/V49b]                - Sets the protocol version to use. Valid values are:
 *                                              "V49"  = VITA 49.0
 *                                              "V49b" = VITA 49.0b
 *  </pre>
 *  If <tt>vrt.ignore_env_var</tt> is not set to true on the Java command line, this will look at
 *  the environment variables for any properties not found. This allows compatibility with the C++
 *  library for mixed Java/C++ environments. Note that the environment variables use all-uppercase
 *  with the "VRT_" prefix rather than "vrt." (e.g. "VRT_VERSION" for "vrt.version"). <br>
 *  <br>
 *  Except where explicitly stated, the above values are constants that can not be altered
 *  after the program is started.
 *  @author 
 */
public final class VRTConfig {
  private VRTConfig () { } // prevent instantiation

  /** Looks up a "system" property returning a default if not found. */
  private static String getProperty (String key, String def) {
    // ==== JAVA PROPERTY ======================================================
    String val = System.getProperty(key);
    if (val != null) return val;
    
    // ==== ENVIRONMENT VARIABLE ===============================================
    try {
      boolean ignore = toBooleanValue(System.getProperty("vrt.ignore_env_var","false"));
      if (!ignore) {
        String var = key.toUpperCase().replace('.', '_'); // change "vrt.version" to "VRT_VERSION"
        String env = System.getenv(var);
        if (env != null) return env;
      }
    }
    catch (Exception e) {
      // ignore
    }
    
    // ==== OTHER ==============================================================
    // In the future this may include special handling for applets, etc.
    
    // ==== USE DEFAULT ========================================================
    return def;
  }
  
  //////////////////////////////////////////////////////////////////////////////////////////////////
  //  Version
  //////////////////////////////////////////////////////////////////////////////////////////////////
  /** The version number for the library, always {@value} with this version. */
  public static final String LIBRARY_VERSION = "0.9.0";
  
  /** Gets the protocol version to use (<tt>vrt.version</tt>). */
  public static final VITAVersion VRT_VERSION = VITAVersion.valueOf(getProperty("vrt.version", "V49"));

  /** Supported protocol versions. (<tt>V49</tt> = VITA 49.0, <tt>V49b</tt> = VITA 49.0b) */
  public static enum VITAVersion { V49, V49b };

  //////////////////////////////////////////////////////////////////////////////////////////////////
  //  Miscelaneous
  //////////////////////////////////////////////////////////////////////////////////////////////////
  /** Enable quicker unit tests (<tt>vrt.quick_test</tt>). */
  public static final boolean QUICK_TEST = toBooleanValue(getProperty("vrt.quick_test","true"));
  
  /** Enable strict checks (<tt>vrt.strict</tt>). */
  public static final boolean STRICT = toBooleanValue(getProperty("vrt.strict","false"));
  
  /** Default leap-seconds file (<tt>vrt.leap_seconds</tt>). */
  public static final String LEAP_SECONDS_FILE = getProperty("vrt.leap_seconds","nxm/vrt/dat/tai-utc.dat");

  /** Are leap seconds counted by default in NORAD time? */
  public static final boolean NORAD_LEAP_SEC_COUNTED = toBooleanValue(getProperty("vrt.norad_ls_counted","true"));

  /** Default DSLT service host:port (<tt>vrt.dslt_service</tt>). */
  public static final String DSLT_SERVICE = getProperty("vrt.dslt_service","XXX.XXX.XXX.XXX:XXXX");

  /** Default DSLT service device to use (<tt>vrt.dslt_device</tt>). */
  public static final String DSLT_DEVICE = getProperty("vrt.dslt_device",null);

  //////////////////////////////////////////////////////////////////////////////////////////////////
  //  PacketFactory and VRTPacket creation
  //////////////////////////////////////////////////////////////////////////////////////////////////
  private static PacketFactory packetFactory = null;

  static {
    // Unlike C++ there is no option to just do a "new nxm.vrt.libm.PacketFactory()" since this
    // class is loaded when building PacketGen and that creates a circular dependancy with
    // nxm/vrt/libm/PacketFactory.java being an output from PacketGen just prior to building the
    // library
    String prop = getProperty("vrt.packet_factory","nxm.vrt.libm.PacketFactory");
    try {
      if (prop.indexOf(':') > 0) {
        String[] props = prop.split(":");
        
        packetFactory = (PacketFactory)Class.forName(props[0]).newInstance();
        for (int i = 1; i < props.length; i++) {
          Class<?> c = Class.forName(props[i]);
          @SuppressWarnings("StaticNonFinalUsedInInitialization")
          Object   pf = c.getConstructor(PacketFactory.class).newInstance(packetFactory);
          packetFactory = (PacketFactory)pf;
        }
      }
      else if (prop.length() > 0) {
        packetFactory = (PacketFactory)Class.forName(prop).newInstance();
      }
    }
    catch (Exception e) {
      throw new Error("Invalid property set vrt.packet_factory="+prop, e);
    }
  }

  /** Gets the current packet factory. */
  public static PacketFactory getPacketFactory () {
    return packetFactory;
  }
  
  /** Sets the current packet factory. */
  public static void setPacketFactory (PacketFactory pf) {
    if (pf == null) {
      throw new NullPointerException("Can not set packet factory to null");
    }
    packetFactory = pf;
  }
  
  /** Internal method, gets applicable packet type. */
  public static Class<? extends VRTPacket> getPacketClass (PacketType type, Long id) {
    return packetFactory.getPacketClass(type, id);
  }
  
  /** Gets a specific packet from the factory when given a generic packet.
   *  @param p    The existing (generic) packet.
   *  @param copy Must the data be copied? If true, the resulting packet must copy the data from
   *              <tt>p</tt> in such a way that no changes to the returned packet will alter
   *              <tt>p</tt>. If false, a "fast" initialization is used, meaning that the resulting
   *              packet may or may not be a copy of the given data, the implementation will use the
   *              fastest creation method available.
   *  @return The applicable packet. This will never return null, the result of an unknown packet
   *          may result in a new {@link BasicVRTPacket} or may simply returning <tt>p</tt> (if
   *          <tt>copy</tt> is false).
   *  @throws NullPointerException If <tt>p</tt> is null.
   */
  public static VRTPacket getPacket (VRTPacket p, boolean copy) {
    return packetFactory.getPacket(p, copy);
  }
  
  /** Gets a specific packet from the factory when given a generic packet.
   *  @param type The packet type (VRT allows one Data/ExtData and one Context/ExtContext per class).
   *  @param id   The class ID of the packet (null if not specified).
   *  @return The applicable packet. This will never return null, the result of an unknown packet
   *          may result in a new {@link BasicVRTPacket}.
   */
  public static VRTPacket getPacket (PacketType type, Long id) {
    return packetFactory.getPacket(type, id);
  }

  /** Creates a new instance based on the given data buffer. Note that when the buffer lengths
   *  are given, only the most minimal of error checking is done. Users should call
   *  {@link VRTPacket#isPacketValid()} to verify that the packet is valid. Invalid packets can
   *  result in unpredictable behavior, but this is explicitly allowed (to the extent possible) so
   *  that applications creating packets can use this class even if the packet isn't yet "valid".<br>
   *  <br>
   *  This function will also detect if a VRL frame was given and return null if that happens.
   *  The {@link #getPackets} method can be used in cases where VRL frames are expected. <br>
   *  <bt>
   *  <b>Warning: Use of <tt>direct=true</tt> and <tt>readOnly=false</tt> are dangerous, see
   *  {@link BasicVRTPacket#BasicVRTPacket(byte[],int,int,byte[],int,int,byte[],int,int,boolean,boolean)}
   *  for details.</b>
   *  @param bbuf     The data buffer to use.
   *  @param boff     The byte offset into the bbuf to start reading/writing at.
   *  @param blen     The length of bbuf in bytes (-1=use size in header of packet).
   *  @param readOnly Should users of this instance be able to modify the underlying data buffer?
   *  @param direct   Can the methods in this class directly interact with the buffer or must
   *                  a copy of the data be made so that changes to the buffer don't affect this
   *                  instance?
   *  @return The applicable type of packet.
   *  @throws NullPointerException If the buffer is null.
   *  @throws IllegalArgumentException If the offset is negative or greater than the
   *          length of the buffer given. Also thrown if the buffer length is -1 and a copy
   *          can not be made because the sizes in the header of the packet are invalid.
   */
  public static VRTPacket getPacket (byte[] bbuf, int boff, int blen, boolean readOnly, boolean direct) {
    boolean isVRL = (bbuf[ boff ] == VRL_FAW_0) && (bbuf[boff+1] == VRL_FAW_1)
                 && (bbuf[boff+2] == VRL_FAW_2) && (bbuf[boff+3] == VRL_FAW_3);
    if (isVRL) {
      return null;
    }
    else {
      BasicVRTPacket p = new BasicVRTPacket(bbuf, boff, blen, readOnly, direct);
      return getPacket(p, false);
    }
  }

  /** Creates a new instance based on the given data buffer. Note that when the buffer lengths
   *  are given, only the most minimal of error checking is done. Users should call
   *  {@link VRTPacket#isPacketValid()} to verify that the packet is valid. Invalid packets can
   *  result in unpredictable behavior, but this is explicitly allowed (to the extent possible) so
   *  that applications creating packets can use this class even if the packet isn't yet "valid".<br>
   *  <br>
   *  This function will also detect if a VRL frame was given and return null if that happens.
   *  The {@link #getPackets} method can be used in cases where VRL frames are expected. <br>
   *  <bt>
   *  This function is identical to calling <tt>getPacket(bbuf,boff,blen,false,false)</tt>.
   *  @param bbuf     The data buffer to use.
   *  @param boff     The byte offset into the bbuf to start reading/writing at.
   *  @param blen     The length of bbuf in bytes (-1=use size in header of packet).
   *  @return The applicable type of packet.
   *  @throws NullPointerException If the buffer is null.
   *  @throws IllegalArgumentException If the offset is negative or greater than the
   *          length of the buffer given. Also thrown if the buffer length is -1 and a copy
   *          can not be made because the sizes in the header of the packet are invalid.
   */
  public static VRTPacket getPacket (byte[] bbuf, int boff, int blen) {
    return getPacket(bbuf,boff,blen,false,false);
  }

  /** Creates new instances based on the given data buffer. Users should call
   *  {@link VRTPacket#isPacketValid()} to verify that the packet is valid. Invalid packets can
   *  result in unpredictable behavior, but this is explicitly allowed (to the extent possible) so
   *  that applications creating packets can use this class even if the packet isn't yet "valid".<br>
   *  <br>
   *  This function will also detect if a VRL frame was given, in which case it will return the
   *  packets contained in the frame.
   *  @param bbuf     The data buffer to use.
   *  @param boff     The byte offset into the bbuf to start reading/writing at.
   *  @param blen     The length of bbuf in bytes (-1=use size in header of packet).
   *  @throws NullPointerException If the buffer is null.
   *  @throws IllegalArgumentException If the offset is negative or greater than the
   *          length of the buffer given. Also thrown if the buffer length is -1 and a copy
   *          can not be made because the sizes in the header of the packet are invalid.
   */
  public static VRTPacket[] getPackets (byte[] bbuf, int boff, int blen) {
    boolean isVRL = (bbuf[ boff ] == VRL_FAW_0) && (bbuf[boff+1] == VRL_FAW_1)
                 && (bbuf[boff+2] == VRL_FAW_2) && (bbuf[boff+3] == VRL_FAW_3);
    if (isVRL) {
      BasicVRLFrame f = new BasicVRLFrame(bbuf, boff, blen, true, true);
      return f.getVRTPackets();
    }
    else {
      VRTPacket p = getPacket(bbuf, boff, blen, false, false);
      return new VRTPacket[] { p };
    }
  }

  /** Creates new instances based on the given input stream. The implementation of this function is
   *  such that the behavior is identical to doing (but may include optimizations that are outside
   *  of what is shown here):
   *  <pre>
   *    public static VRTPacket[] getPackets (InputStream in) throws IOException {
   *      VRLFrame f = ...;
   *      f.readFrameFrom(in);
   *      return f.getVRTPackets();
   *    }
   *  </pre>
   *  @param in The input stream.
   *  @throws NullPointerException If the buffer is null.
   *  @throws IOException If there is an error reading the stream. In the event of an I/O error, the
   *                      state of the input stream is indeterminate.
   */
  public static VRTPacket[] getPackets (InputStream in) throws IOException {
    BasicVRLFrame f = new BasicVRLFrame();
    f.readFrameFrom(in);
    return f.getVRTPackets();
  }
}

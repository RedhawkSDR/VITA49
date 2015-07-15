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
import nxm.vrt.lib.ContextPacket.ContextAssocLists;
import java.io.IOException;
import java.util.HashSet;
import java.util.Map;
import java.util.LinkedHashMap;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicInteger;

import static nxm.vrt.lib.VRLFrame.VRL_FAW_0;
import static nxm.vrt.lib.VRLFrame.VRL_FAW_1;
import static nxm.vrt.lib.VRLFrame.VRL_FAW_2;
import static nxm.vrt.lib.VRLFrame.VRL_FAW_3;
import static nxm.vrt.lib.VRTPacket.MAX_PACKET_LENGTH;

import static nxm.vrt.net.VRTContextListener.NO_DATA_STREAM;
import static nxm.vrt.net.VRTContextListener.NO_CONTEXT_STREAM;

/**  Reads VRT packets which may optionally be included in VRL frames. The choice between VRT
 *  packet and VRL frame is made automatically by looking at the first four bytes received and
 *  seeing if they match the {@link VRLFrame#VRL_FAW}, if not a VRT packet is assumed.
 *
 *  @author         
 */
public final class VRTReader extends MulticastReader {
  /** The "timeout" value indicating the default wait of 60.0 sec. */
  public static final int DEFAULT_TIMEOUT = 60;

  /** The "timeout" value indicating an unlimited wait. */
  public static final int UNLIMITED_TIMEOUT = -1;

  /** The "timeout" value indicating that "legacy mode" should be used. */
  public static final int LEGACY_MODE = -2;

  /** The start time indicator that the initial context has already been found. */
  private static final int FOUND_INITIAL = -3;


  private final ConcurrentHashMap<Long,AtomicInteger> packetCounters = new ConcurrentHashMap<Long,AtomicInteger>();
  private final AtomicInteger                         frameCounter   = new AtomicInteger(-1);
  private final long                                  timeoutMS;     // timeout in ms (-1=LegacyMode)
  private       long                                  startTimeMS;   // start time in ms (0=not started)
  private       DataPacket                            initialData;
  private       ContextPacket                         initialCtx;
  private       Map<Integer,VRTPacket>                initialContext;
  private       Set<Integer>                          requiredContext;
  private       Integer                               idContext;

  /** Creates a new instance uses {@link #LEGACY_MODE}.
   *  @param hostPort  The host:port to connect to.
   *  @param device    The device to connect to (e.g. "eth0", "eth1.101") (null = use default).
   *  @param warnings  The listener to handle any warnings. This is only used for handling warnings
   *                   during startup, and is discarded immediately following. If this is null, any
   *                   warnings will print to the console.
   *  @throws IOException If an I/O execution occurs.
   */
  public VRTReader (String hostPort, String device, VRTEventListener warnings) throws IOException {
    //this(DataSourceLookupTable.getHost(hostPort), DataSourceLookupTable.getPort(hostPort), device, warnings);
    this(NetUtilities.getHost(hostPort), NetUtilities.getPort(hostPort), device, warnings, LEGACY_MODE);
  }

  /** Creates a new instance uses {@link #LEGACY_MODE}.
   *  @param host      The host to connect to (null = wildcard address).
   *  @param port      The port number.
   *  @param device    The device to connect to (e.g. "eth0", "eth1.101") (null = use default).
   *  @param warnings  The listener to handle any warnings. This is only used for handling warnings
   *                   during startup, and is discarded immediately following. If this is null, any
   *                   warnings will print to the console.
   *  @throws IOException If an I/O execution occurs.
   */
  public VRTReader (String host, int port, String device, VRTEventListener warnings) throws IOException {
    this(host, port, device, warnings, LEGACY_MODE);
  }

  /** Creates a new instance uses. <br>
   *  <br>
   *  <i>For historical reasons (i.e. to prevent incompatibilities with the Java version in VRT
   *  version 1.0 and earlier), the default timeout value (i.e. {@link #DEFAULT_TIMEOUT}) must
   *  be explicitly provided.</i>
   *  @param hostPort   The host:port to connect to.
   *  @param device     The device to connect to (e.g. "eth0", "eth1.101") (null = use default).
   *  @param listener   The primary listener *and* the listener used to handle any warnings during
   *                    startup (see {@link #VRTReader(String,int,String,VRTEventListener,double)}
   *                    for details).
   *  @param timeout    The approximate timeout (in seconds) applied when checking for the initial
   *                    packets (see {@link #VRTReader(String,int,String,VRTEventListener,double)}
   *                    for details).
   *  @throws IOException If an I/O execution occurs.
   */
  public VRTReader (String hostPort, String device, VRTEventListener listener, double timeout) throws IOException {
    //this(DataSourceLookupTable.getHost(hostPort), DataSourceLookupTable.getPort(hostPort), device, listener, timeout);
    this(NetUtilities.getHost(hostPort), NetUtilities.getPort(hostPort), device, listener, timeout);
  }

  /** Creates a new instance. <br>
   *  <br>
   *  <i>For historical reasons (i.e. to prevent incompatibilities with the Java version in VRT
   *  version 1.0 and earlier), the default timeout value (i.e. {@link #DEFAULT_TIMEOUT}) must
   *  be explicitly provided.</i>
   *  @param host       The host to connect to (null = wildcard address).
   *  @param port       The port number.
   *  @param device     The device to connect to (e.g. "eth0", "eth1.101") (null = use default).
   *  @param listener   The primary listener *and* the listener used to handle any warnings during
   *                    startup (if null, any startup warnings will print to the console). After
   *                    startup, this will be registered the same as calling:
   *                    <pre>
   *                      if (listener != null) {
   *                        addErrorWarningListener(listener);
   *                        addReveivedPacketListener(listener,null);
   *                      }
   *                    </pre>
   *                    Setting this to null puts the reader in "legacy" mode that does not use the new
   *                    {@link VRTContextListener} functions.
   *  @param timeout    The approximate timeout (in seconds) applied when checking for the initial
   *                    packets. If the complete initial context is not received before the first
   *                    packet received after this time has elapsed, it will be reported as an error
   *                    (via <tt>errorMsg</tt> in {@link VRTContextListener#receivedInitialContext}
   *                    if appropriate). Setting this to {@link #UNLIMITED_TIMEOUT} disables the
   *                    timeout and setting this to {@link #LEGACY_MODE} enters into "legacy mode"
   *                    which matches the VRT 1.0 library in that it NEVER uses any of the new
   *                    {@link VRTContextListener} functions. The default timeout value to use
   *                    is {@link #DEFAULT_TIMEOUT}.
   *  @throws IOException If an I/O execution occurs.
   */
  public VRTReader (String host, int port, String device, VRTEventListener listener, double timeout)
                                                                                    throws IOException {
    super(host, port, device, listener, MAX_PACKET_LENGTH);
    if (timeout == LEGACY_MODE) {
      this.timeoutMS       = LEGACY_MODE;
      this.startTimeMS     = LEGACY_MODE;
      this.initialData     = null;
      this.initialCtx      = null;
      this.initialContext  = null;
      this.requiredContext = null;
      this.idContext       = null;
    }
    else {
      this.timeoutMS       = (timeout < 0)? ((long)timeout) : ((long)(timeout * 1000));
      this.startTimeMS     = 0;
      this.initialData     = null;
      this.initialCtx      = null;
      this.initialContext  = new LinkedHashMap<Integer,VRTPacket>();
      this.requiredContext = new HashSet<Integer>();
      this.idContext       = null;
    }

    if (listener != null) {
      addErrorWarningListener(listener);
      addReceivedPacketListener(listener,null);
    }
  }
  
  @Override
  protected void handle (byte[] buffer, int length) throws IOException {
    if (startTimeMS == 0) {
      // start the clock on the first datagram received
      startTimeMS = System.currentTimeMillis();
    }
    
    if ((buffer[0] == VRL_FAW_0) && (buffer[1] == VRL_FAW_1) &&
        (buffer[2] == VRL_FAW_2) && (buffer[3] == VRL_FAW_3)) {
      handle(new BasicVRLFrame(buffer, 0, length, false, true));
    }
    else {
      handle(VRTConfig.getPacket(buffer, 0, length, false, true));
    }
  }
  
  protected void handle (VRLFrame frame) throws IOException {
    if (!frame.isFrameValid()) {
      if (frame.isCRCValid()) listenerSupport.fireErrorOccurred("Received invalid frame", null);
      else                    listenerSupport.fireErrorOccurred("Received corrupt frame", null);
      return;
    }
    int count    = frame.getFrameCount();
    int next     = (count+1) & 0xFFF;
    int expected = frameCounter.getAndSet(next);
    
    if ((expected != count) && (expected >= 0)) {
      listenerSupport.fireErrorOccurred("Missed frames "+expected+" (inclusive) to "+count+" (exclusive).", null);
    }

    for (VRTPacket packet : frame) {
      handle(packet);
    }
  }
  
  protected void handle (VRTPacket packet) throws IOException {
    String err = packet.getPacketValid(false);
    
    if (err != null) {
      listenerSupport.fireErrorOccurred(err, null);
      return;
    }
    long          code    = packet.getStreamCode();
    AtomicInteger counter = packetCounters.get(code);
    int           count   = packet.getPacketCount();
    int           next    = (count+1) & 0xF;

    if (counter == null) {
      packetCounters.putIfAbsent(code, new AtomicInteger(count)); // first packet received
      counter = packetCounters.get(code);
    }

    int expected = counter.getAndSet(next);
    if (count != expected) {
      listenerSupport.fireErrorOccurred("Missed packets "+expected+" (inclusive) to "+count+" (exclusive).", null);
    }

    // SEE IF WE ARE USING LEGACY MODE ========================================
    if (startTimeMS == LEGACY_MODE) {
    listenerSupport.fireReceivedPacket(packet);
      return;
    }

    // SEE IF WE ALREADY FOUND INITIAL CONTEXT ================================
    if (startTimeMS == FOUND_INITIAL) {
      if (packet instanceof DataPacket) {
        listenerSupport.fireReceivedDataPacket((DataPacket)packet);
        return;
      }
      else {
        listenerSupport.fireReceivedContextPacket(packet);
        return;
      }
    }

    // STILL NEED MORE CONTEXT (OR INITIAL DATA PACKET) =======================
    long    now     = System.currentTimeMillis();
    boolean timeout = (timeoutMS > 0) && (startTimeMS+timeoutMS <= now);

    // ---- If this is a DataPacket, handle it as such ------------------------
    if (packet instanceof DataPacket) {
      initialData = (DataPacket)packet;
      idContext   = initialData.getStreamIdentifier();

      if (idContext == null) {
        fireReceivedInitialContext(null); // unidentified stream (i.e. done)
        return;
      }
      else if (timeout) {
        fireReceivedInitialContext(NO_CONTEXT_STREAM);
        return;
      }
      else {
        return;
      }
    }

    // ---- Found a context packet --------------------------------------------
    int id = packet.getStreamIdentifier();
    initialContext.put(id, packet);

    // ---- Is this a non-ContextPacket primary stream (rare)? ----------------
    if ((idContext != null) && (id == idContext) && !(packet instanceof ContextPacket)) {
      if (initialContext.size() == 1) {
        fireReceivedInitialContext(null);
        return;
      }
      else {
        fireReceivedInitialContext("Context packets do not follow stream ID rules (found streams "
                                 + initialContext.keySet() + " but expected only " + id + ").");
        return;
      }
    }

    // ---- For any ContextPackets, check assoc. lists first ------------------
    if (packet instanceof ContextPacket) {
      ContextPacket     ctx   = (ContextPacket)packet;
      ContextAssocLists assoc = ctx.getContextAssocLists();

      if ((idContext != null) && (id == idContext)) {
        // it is the primary, set it and mark as required
        initialCtx = ctx;
        requiredContext.add(id);
      }
      if (assoc != null) {
        for (int val : assoc.getSourceContext()) requiredContext.add(val);
        for (int val : assoc.getSystemContext()) requiredContext.add(val);
      }
    }

    // ---- Check to see if done ----------------------------------------------
    boolean foundCtx = (initialCtx != null);
    boolean sameSize = initialContext.size() == requiredContext.size();
    boolean foundAll = initialContext.keySet().containsAll(requiredContext);

    if (foundCtx && foundAll) {
      // found all required context
      if (sameSize) {
        fireReceivedInitialContext(null);
        return;
      }
      else {
        fireReceivedInitialContext("Context packets do not follow stream ID rules (found streams "
                                 + initialContext.keySet() + " but expected " + requiredContext + ").");
        return;
      }
    }
    
    if (timeout && foundCtx) {
      // timeout before finding all required context
      if (sameSize) {
        fireReceivedInitialContext("Context packets do not follow stream ID rules (found streams "
                                 + initialContext.keySet() + " but expected " + requiredContext + ").");
        return;
      }
      else {
        fireReceivedInitialContext("Timeout before all required context could be found (found streams "
                                  + initialContext.keySet() + " but expected " + requiredContext + ").");
        return;
      }
    }
      
    if (timeout) {
      // timeout before finding primary context
      if (initialData == null) {
        fireReceivedInitialContext(NO_DATA_STREAM);
        return;
      }
      else {
        fireReceivedInitialContext("Could not find IF Context for stream ID "
                                   + initialData.getStreamID() + ".");
        return;
      }
    }
  }


  /** Variant of listenerSupport.fireReceivedInitialContext(..) that resets fields when done. */
  private void fireReceivedInitialContext (String errorMsg) {
    listenerSupport.fireReceivedInitialContext(errorMsg, initialData, initialCtx, initialContext);
    startTimeMS     = FOUND_INITIAL;
    initialData     = null;
    initialCtx      = null;
    idContext       = null;
    initialContext  = null;
    requiredContext = null;
  }
}

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

import java.io.PrintStream;
import java.io.IOException;
import java.util.Map;

import static nxm.vrt.lib.Utilities.sleep;
import static nxm.vrt.lib.Utilities.toHexDump;

/** <b>Command-Line Use Only:</b> A basic tool for debugging UDP multicast streams.
 *
 *  @author 
 */
public class UDPDump extends Main {
  /** Supported translation formats. */
  private static enum Format {
    RAW,
    VITA49,
    VITA49_NO_DATA,
    VITA49_ALL;

    public static final Format[] VALUES = values();

    /** Finds the applicable translation format.
     *  @patam name  The name to search for.
     *  @param error Throw an exception if not found? If false and the name is not found,
     *               this function will return null.
     */
    public static Format forName (String name, boolean error) {
      if ((name == null) || name.isEmpty()) return null;

      String str = name.toUpperCase().replace('-', '_');

      for (Format tf : VALUES) {
        if (str.equals(tf.name())) return tf;
      }

      if (error) {
        throw new IllegalArgumentException("Unsupported TranslationFormat name '"+name+"'");
      }
      return null;
    }

    /** Converts to the translation format to its 'name' format. */
    public String toName () {
      return name().toLowerCase().replace('_', '-');
    }
  };


  private static final String[] OPTIONS = { "-A", "-C", "-T=", "-i=", "-n=", "-p=", "-v", "-w=", "-x" };
  private static final String[] VERSION = { "UDPDump "+VRTConfig.LIBRARY_VERSION };
  private static final String[] USAGE   = {
      "Usage: udpdump [OPTION]... <host:port>",
      " or:   java -classpath <classpath> nxm.vrt.net.UDPDump [OPTION]... <host:port>",
      "",
      "",
      "-A                 Print each packet in ASCII",                            // from tcpdump
      "-C                 Print each packet in hex and ASCII",                    // from hexdump
      "-H                 Prints usage information and exits",
      "-i interface       Use the specified device interface",                    // from tcpdump
      "-n limit           Exit after receiving the specified number of packets",  // from tail, etc.
      "-p port            Port number (allows entry of just host in <host:port>)",// from ssh
      "-T format          Translate packets before output to using one of the following conversions:",
      "                       raw            - Raw packet data (default)",
      "                       vita49         - VITA-49.0 packets or VITA-49.1 frames in 'processing' form",
      "                                        waiting for initial context and then displaying context",
      "                                        and data packets as received (for the purposes of -n, the",
      "                                        initial context, which may be multiple packets, counts as",
      "                                        one packet)",
      "                       vita49-no-data - Same as 'vita49' but omits any data packets other than the",
      "                                        one that is associated with the initial context (for the",
      "                                        purposes of -n, any data packets received are not counted)",
      "                       vita49-all     - VITA-49.0 packets or VITA-49.1 frames in 'all' form",
      "                                        where evey packet is displayed as received without any",
      "                                        waiting for initial context",
      "-V                 Prints version information and exits",
      "-v                 Output verbose output (automatically on if -A, -C, or -x are specified)",
      "-w file            Save a copy of the data to a file",                     // from tcpdump
      "-x                 Print each packet in hex",                              // from tcpdump, hexdump
      "",
      "(--help, --verbose, and --version are aliases for -H, -v, and -V)",
      "",
      "Example:",
      "  udpdump -n 4 -C XXX.XXX.XXX.XXX:XXXX"
  };

  private final    String      device;
  private final    String      host;
  private final    int         port;
  private final    Format      format;
  private final    double      timeout;
  private final    int         limit;
  private volatile int         count;
  private final    boolean     ascii;
  private final    boolean     hex;
  private final    boolean     verbose;
  private final    PrintStream fileOut;
  
  /** <b>Internal Use Only:</b> Creates a new instance.
   *  @param args The command-line arguments.
   */
  public UDPDump (String[] args) throws IOException {
    super(VERSION, USAGE, OPTIONS, 1, 1, args);
    String hostPort = (isHelpVersion())? "127.0.0.1:666" : parameters.get(0);
    int    colon    = hostPort.indexOf(':');
    String _host     = null;
    String _port     = null;
    String _limit    = options.get("-n");
    String _fileName = options.get("-w");
    String _tf       = options.get("-T");
    Format _format   = Format.forName(_tf, false);

    if ((_tf != null) && (_format == null)) {
      throw new Main.InvalidCommandLineException("Unsupported translation format for -T "+_tf);
    }

    if (colon >= 0) {
      _host = hostPort.substring(0,colon);
      _port = hostPort.substring(colon+1);
    }
    else if (options.containsKey("-p")) {
      _host = hostPort;
      _port = options.get("-p");
    }
    else {
      throw new Main.InvalidCommandLineException("Unsupported host:port in "+hostPort);
    }

    if (_host.length() ==    0) _host  = null;
    if (_port.length() ==    0) _port  = "0";
    if (_limit         == null) _limit = "-1";
    
    this.device  = options.get("-i");
    this.host    = _host;
    this.port    = Integer.parseInt(_port);
    this.format  = (_format == null)? Format.RAW : _format;
    this.timeout = 60.0;
    this.limit   = Integer.parseInt(_limit);
    this.ascii   = options.containsKey("-A") || options.containsKey("-C");
    this.hex     = options.containsKey("-x") || options.containsKey("-C");
    this.verbose = ascii || hex || !options.containsKey("-w")
                                || options.containsKey("-v") || options.containsKey("--verbose");
    this.fileOut = (_fileName == null)? null : new PrintStream(_fileName);
  }
  
  /** Main method for the class.
   *  @param args The command line arguments.
   */
  public static void main (String ... args) {
    runMain(UDPDump.class, args);
  }

  @Override
  protected void main () throws Exception {
    MulticastReader  r;
    VRTEventListener l;

    //(String host, int port, String device, VRTEventListener listener, double timeout)
    switch (format) {
      case RAW:            r = new PacketReader(host, port, device); break;
      case VITA49:         r = new VRTReader(host, port, device, new V49Listener(true ), timeout); break;
      case VITA49_NO_DATA: r = new VRTReader(host, port, device, new V49Listener(false), timeout); break;
      case VITA49_ALL:     r = new VRTReader(host, port, device, new RawV49Listener(),   timeout); break;
      default:             throw new AssertionError("Unsupported format "+format);
    }
    r.start();

    while ((limit == -1) || (count < limit)) {
      sleep(100);
    }

    r.close();
    if (fileOut != null) {
      fileOut.close();
    }
  }

  /** The basic multicast reader. */
  private final class PacketReader extends MulticastReader {
    /** Creates a new instance. */
    public PacketReader (String host, int port, String device) throws IOException {
      super(host, port, device, null);
    }

    @Override
    protected void handle (byte[] buffer, int length) throws IOException {
      if (count >= limit) return; // ignore any received during shutdown

      if (verbose) {
        System.out.println("Got a packet of length "+length);
      }
      if (fileOut != null) {
        fileOut.write(buffer, 0, length);
      }
      if (hex || ascii) {
        System.out.println(toHexDump(buffer, 0, length, hex, ascii));
      }
      count++;
    }
  }

  /** Basic VITA-49 listener that does not implement VRTContextListener and prints to screen. */
  private class RawV49Listener implements VRTEventListener {
    private final boolean showData;

    public RawV49Listener () {
      this(true);
    }

    protected RawV49Listener (boolean showData) {
      this.showData = showData;
    }

    /** Prints out a line of text, also prints to output file. */
    protected void println (String str) {
      System.out.println(str);
      if (fileOut != null) {
        fileOut.println(str);
      }
    }

    private void receivedPkt (String type, VRTEvent e, VRTPacket p) {
      if (count >= limit) return; // ignore any received during shutdown
      println("Received "+type+": "+p);
      println("");
      count++;
    }

    private void receivedErr (String type, VRTEvent e, String msg, Throwable t) {
      if (count >= limit) return; // ignore any received during shutdown
      println(type+" Condition Detected: "+msg);
      if (t != null) {
        t.printStackTrace(System.out);
        if (fileOut != null) {
          t.printStackTrace(fileOut);
        }
      }
      println("");
    }

    @Override public synchronized void errorOccurred         (VRTEvent e, String msg, Throwable t) { receivedErr("Error", e, msg, t); }
    @Override public synchronized void warningOccurred       (VRTEvent e, String msg, Throwable t) { receivedErr("Warning", e, msg, t); }
    @Override public synchronized void sentPacket            (VRTEvent e, VRTPacket p) { /* not used */ }
    @Override public synchronized void receivedPacket        (VRTEvent e, VRTPacket p) { receivedPkt("Packet",  e, p); }
              public synchronized void receivedContextPacket (VRTEvent e, VRTPacket p) { receivedPkt("Context", e, p); }
              public synchronized void receivedDataPacket    (VRTEvent e,DataPacket p) { if (showData) receivedPkt("Data", e, p); }

    public synchronized void receivedInitialContext (VRTEvent e, String errorMsg, DataPacket data,
                                                     ContextPacket ctx, Map<Integer,VRTPacket> context) {
      if (count >= limit) return; // ignore any received during shutdown
      
      println("Received Initial Context:");
      println("  Status          : " + ((errorMsg == null)? "OK" : errorMsg));
      println("  Associated Data : " + data);
      println("  Primary Context : " + ctx);

      if (context.isEmpty()) {
        println("  Full Context : <none>");
      }
      else {
        boolean first = true;
        for (Integer key : context.keySet()) {
          if (first) println("  Full Context    : "+key+"\t = "+context.get(key));
          else       println("                    "+key+"\t = "+context.get(key));
          first = false;
        }
      }
      println("");
      count++;
    }
  }

  /** Extension on the basic VITA-49 listener that implements VRTContextListener. */
  private final class V49Listener extends RawV49Listener implements VRTContextListener {
    public V49Listener (boolean showData) {
      super(showData);
    }
  }
}

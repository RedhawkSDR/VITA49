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

import java.io.File;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Enumeration;
import java.util.List;
import java.util.LinkedList;
import java.util.LinkedHashMap;
import java.util.Map;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

import static nxm.vrt.lib.XmlUtilities.evalXPath;
import static nxm.vrt.lib.XmlUtilities.readDocumentXML;

/** <b>Command-Line Use Only:</b> A basic tool for determining which NIC is
 *  active on what network.
 *
 *  @author 
 */
public class NICCheck extends Main {
  private static final String[] OPTIONS = { "--ttl=", "--timeout=" };
  private static final String[] VERSION = { "NICCheck "+VRTConfig.LIBRARY_VERSION };
  private static final String[] USAGE   = {
      "Usage: niccheck [OPTION]... <networks.xml>",
      " or:   java -classpath <classpath> nxm.vrt.net.NICCheck [OPTION]... <networks.xml>",
      "",
      "",
      "--help      or -H    Prints usage information and exits",
      "--timeout [sec]      Override the default timeout (in seconds) for 'is-reachable' checks",
      "--ttl [ttl]          Override the default TTL value for 'is-reachable' checks",
      "--version   or -V    Prints version information and exits",
      "",
      "Example:",
      "  niccheck networks.xml"
  };

  private static final String UNKNOWN  = "--UNKNOWN--";  // Unknown network
  private static final String LOOPBACK = "--LOOPBACK--"; // Loopback network
  private static final String ERROR    = "--ERROR--";    // Error in checking network
  
  private final String inputFile;
  private final int    ttl;
  private final int    timeout;

  /** <b>Internal Use Only:</b> Creates a new instance.
   *  @param args The command-line arguments.
   */
  public NICCheck (String[] args) {
    super(VERSION, USAGE, OPTIONS, 1, 1, args);
    String _ttl     = options.get("--ttl");
    String _timeout = options.get("--timeout");
    
    this.inputFile   = (isHelpVersion())? "--none--" : parameters.get(0);
    this.ttl         = (_ttl     == null)? 0     : Integer.parseInt(_ttl);
    this.timeout     = (_timeout == null)? 10000 : (int)(1000 * Math.ceil(Float.parseFloat(_timeout)));
  }

  /**  Discovers all of the attached network interfaces. */
  private static List<NIC> getNICList () throws SocketException {
    // SEARCH THROUGH THE INTERFACES
    Enumeration<NetworkInterface> interfaces   = NetworkInterface.getNetworkInterfaces();
    List<String>                  networkCards = new ArrayList<String>();
    Map<String,List<String>>      vlans        = new LinkedHashMap<String,List<String>>();

    while (interfaces.hasMoreElements()) {
      NetworkInterface nic = interfaces.nextElement();
      String name  = nic.getName();
      int    index = name.indexOf('.');

      if (index > 0) {
        String       cardName = name.substring(0,index);
        List<String> vlanList = vlans.get(cardName);

        if (vlanList == null) {
          vlanList = new LinkedList<String>();
          vlans.put(cardName, vlanList);
        }
        vlanList.add(nic.getName());
      }
      else {
        networkCards.add(name);
      }
    }

    // SORT THINGS (otherwise everything tends to be all jumbled up and hard to debug)
    Collections.sort(networkCards);
    for (List<String> vlanList : vlans.values()) {
      Collections.sort(vlanList);
    }

    // MAKE THE NIC OBJECTS
    List<NIC> nics = new LinkedList<NIC>();
    for (String name : networkCards) {
      nics.add(new NIC(name, vlans.get(name)));
    }
    
    return nics;
  }
  
  /** Finds the *first* NIC that can see the given VLAN. Note that preference is given to NICs that
      are up and NICs that are not the loopback interface. (Note that if vlan=0, the *first* NIC
      will be returned.)
      @param vlan The VLAN number.
      @return The NIC name (e.g. "eth1") or null if not found.
   */
  public static String findVLAN (int vlan) {
    try {
      List<NIC> nics = getNICList();
      
      for (int stage = 0; stage <= 2; stage++) {
        for (NIC nic : nics) {
          if ((stage < 1) && nic.getNetIF().isUp()) {
            continue; // prefer interfaces that are up
          }
          if ((stage < 2) && nic.getNetIF().isLoopback()) {
            continue; // prefer interfaces other than the loopback
          }
          if (vlan <= 0) {
            return nic.getName();
          }

          String vlanName = nic.getName()+"."+vlan;
          if ((nic.getVLANs() != null) && nic.getVLANs().contains(vlanName)) {
            return nic.getName();
          }
        }
      }
      return null;
    }
    catch (Exception e) {
      return null;
    }
  }

  /**  Main method for the class.
   *  @param args The command line arguments.
   */
  public static void main (String ... args) {
    runMain(NICCheck.class, args);
  }
  
  @Override
  protected void main () throws Exception {
    Document  xmlDocument = readDocumentXML(new File(inputFile));
    List<NIC> nics        = getNICList();

    // CHECK FOR OBVIOUS THINGS
    for (NIC nic : nics) {
      NetworkInterface n = NetworkInterface.getByName(nic.getName());

      if (!n.isUp()) {
        nic.setNetwork(ERROR);
      }
      else if (n.isLoopback()) {
        nic.setNetwork(LOOPBACK);
      }

    }

    // CHECK THE NETWORKS FILE
    NodeList networks = evalXPath("/networks/network", xmlDocument, NodeList.class);
    for (NIC nic : nics) {
      if (!nic.isUnknown()) continue;
      
      for (int i = 0; i < networks.getLength(); i++) {
        Node     node    = networks.item(i);
        String   network = evalXPath("./@name", node, ERROR);
        NodeList hints   = evalXPath("./hints", node, NodeList.class);

        if (hints != null) {
          for (int j = 0; j < hints.getLength(); j++) {
            Node   e = hints.item(j);
            String n = e.getNodeName();

            if (n.equals("is-reachable")) {
              String      url  = evalXPath("./@url", e, ERROR);
              InetAddress inet = InetAddress.getByName(url);

              try {
                if (inet.isReachable(nic.getNetIF(), ttl, timeout)) {
                  nic.setNetwork(network);
                  break;
                }
              }
              catch (Exception ex) {
                // ignore error
              }
            }
            else if (n.equals("has-vlan")) {
              String vlan     = evalXPath("./@vlan", e, ERROR);
              String vlanName = nic.getName()+"."+vlan;

              if ((nic.getVLANs() != null) && nic.getVLANs().contains(vlanName)) {
                nic.setNetwork(network);
                break;
              }
            }
            else if (n.equals("script")) {
              String cmd = evalXPath("./@cmd", e, ERROR).replace("${NIC}", nic.getName())
                                                        .replace("$NIC",   nic.getName());
              try {
                int exit = Runtime.getRuntime().exec(cmd).waitFor();
                if (exit == 0) {
                  nic.setNetwork(network);
                  break;
                }
              }
              catch (Exception ex) {
                // ignore
              }
            }
            else {
              throw new RuntimeException("Unknown hint "+n+" for "+network);
            }
          }

          if (!nic.isUnknown()) break;
        }
      }
    }

    for (NIC nic : nics) {
      System.out.println(nic);
    }
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // NESTED CLASSES
  //////////////////////////////////////////////////////////////////////////////////////////////////
  /**  An internal definition for a network card. */
  private static class NIC {
    private       String       network;
    private final String       name;
    private final List<String> vlans;

    NIC(String name, List<String> vlans) {
      this.network = null;
      this.name    = name;
      this.vlans   = vlans;
    }

    @Override
    public String toString () {
      return getName()+"="+getNetwork();
    }

    /**  Is the network unknown? */
    public boolean isUnknown () {
      return (network == null);
    }

    /**  Sets the name of the network associated with the device. */
    public void setNetwork (String net) {
      network = net;
    }

    /**  Gets the name of the network associated with the device. */
    public String getNetwork () {
      return (network == null)? UNKNOWN : network;
    }

    /**  Gets the name of the device. */
    public String getName () {
      return name;
    }

    /**  Gets the network interface of the device. */
    public NetworkInterface getNetIF () throws SocketException {
      return NetworkInterface.getByName(name);
    }

    /**  Gets the name of the VLANs associated with the device (may return null if there are none). */
    public List<String> getVLANs () {
      return vlans;
    }
  }
}

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

package nxm.vrt.libg;

import java.io.PrintStream;
import java.util.Arrays;
import java.util.LinkedHashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import nxm.vrt.lib.TimeStamp;
import nxm.vrt.lib.VRTConfig;
import nxm.vrt.lib.VRTPacket;
import nxm.vrt.libg.Type.ArrayType;
import nxm.vrt.libg.Type.AsciiType;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

import static nxm.vrt.lib.Utilities.toHexString;
import static nxm.vrt.lib.Utilities.fromStringClassID;
import static nxm.vrt.lib.XmlUtilities.evalXPath;
import static nxm.vrt.libg.PacketGen.ALWAYS_EXCLUDE;
import static nxm.vrt.libg.PacketGen.printHeaderHTML;
import static nxm.vrt.libg.PacketGen.printFooterHTML;
import static nxm.vrt.libg.PacketGen.printIndexHTML;
import static nxm.vrt.libg.PacketGen.toHtml;
import static nxm.vrt.libg.PacketGen.toHtmlDoc;
import static nxm.vrt.libg.PacketGen.toHtmlTitle;
import static nxm.vrt.libg.PacketGen.notPlural;
import static nxm.vrt.libg.PacketGen.use;
import static nxm.vrt.libg.Type.BASIC_TYPES;

/** A packet type definition.
 *
 *  @author 
 */
class PacketType extends Record {
  final Map<String, PacketType>  packetTypes;
  final String                   id;        // Class ID in string form
  final long                     classID;   // Class ID in numeric form
  final String                   srcFileName;
  final VRTPacket.PacketType     type;
  final List<Enumeration>        enums     = new LinkedList<Enumeration>();
  final Map<String, Enumeration> enumMap   = new LinkedHashMap<String, Enumeration>();
  final Map<String, Record>      recordMap = new LinkedHashMap<String, Record>();
  final Map<String, XPath>       xpathMap  = new LinkedHashMap<String, XPath>();
  final Trailer                  trailer;
  
  /** Creates a new packet definition.
   *  @param node        Node from the XML.
   *  @param packetTypes Associated packet types.
   *  @param srcFileName Name of the source file.
   */
  public PacketType (Node node, Map<String, PacketType> packetTypes, String srcFileName) {
    super(node);
    Node     trailerNode  = evalXPath("./TRAILER",                  node, Node.class);
    NodeList fieldList    = evalXPath("./FIELDS/FIELD",             node, NodeList.class);
    NodeList xpathList    = evalXPath("./XPATH/XPATH",              node, NodeList.class);
    NodeList enumList     = evalXPath("./ENUMERATIONS/ENUMERATION", node, NodeList.class);
    NodeList recordList   = evalXPath("./RECORDS/RECORD",           node, NodeList.class);
    String   packet_type  = evalXPath("./PACKET_TYPE",              node, String.class);

    if (enumList != null) {
      for (int i = 0; i < enumList.getLength(); i++) {
        Enumeration e = new Enumeration(this, enumList.item(i));
        enums.add(e);
        enumMap.put("/" + e.name + "/", e);
      }
    }

    if (recordList != null) {
      for (int i = 0; i < recordList.getLength(); i++) {
        Record r = new Record(this, recordList.item(i));
        recordMap.put("/" + r.name + "/", r);
      }
    }

    if (xpathList != null) {
      for (int i = 0; i < xpathList.getLength(); i++) {
        XPath x = new XPath(this, xpathList.item(i));
        xpathMap.put(x.name, x);
      }
    }

    this.packetTypes = packetTypes;
    this.srcFileName = srcFileName;
    this.id          = evalXPath("./CLASS_ID", node, String.class);
    this.classID     = fromStringClassID(id);
    this.type        = VRTPacket.PacketType.VALUES[Integer.parseInt(packet_type)];

    switch (this.type) {
      case UnidentifiedData:
      case Data:
      case UnidentifiedExtData:
      case ExtData:
        this.trailer     = (trailerNode == null) ? null : new Trailer(this, trailerNode);
        this.length      = readFields(Math.abs(getParentLength()), fieldList);
        break;
      case Context:
        this.trailer     = null;
        this.length      = -4;    // 4 = Mandatory Context Indicator Field
        break;
      case ExtContext:
        this.trailer     = null;
        this.length      = readFields(Math.abs(getParentLength()), fieldList);
        break;
      default: throw new AssertionError("Unknown packet type "+this.type);
    }
  }

  @Override
  public String toString () {
    return "PacketType: name='"+name+"'";
  }

  /** String form of the packet type. */
  private static String toTypeString (VRTPacket.PacketType type) {
    switch (type) {
      case UnidentifiedData:    return "IF Data packet without Stream Identifier";
      case Data:                return "IF Data packet with Stream Identifier";
      case UnidentifiedExtData: return "Extension Data packet without Stream Identifier";
      case ExtData:             return "Extension Data packet with Stream Identifier";
      case Context:             return "IF Context Packet";
      case ExtContext:          return "Extension Context Packet";
      default: throw new RuntimeException("Unknown packet type "+type);
    }
  }

  /** Short string form of the packet type. */
  private static String toTypeStringShort (VRTPacket.PacketType type) {
    switch (type) {
      case UnidentifiedData:    return "IF Data";
      case Data:                return "IF Data";
      case UnidentifiedExtData: return "Extension Data";
      case ExtData:             return "Extension Data";
      case Context:             return "IF Context";
      case ExtContext:          return "Extension Context";
      default: throw new RuntimeException("Unknown packet type "+type);
    }
  }

  /** Gets the parent for a current packet type. */
  private Object getParent () {
    if ((parent == null) || parent.isEmpty()) return null;

    PacketType pt = packetTypes.get(parent);
    if (pt != null) return pt;

    Long parentID = fromStringClassID(parent);

    return VRTConfig.getPacketClass(packetType.type, parentID);
  }

  /** Gets the Java class name of the parent for a current packet type. */
  private String getParentClassJava (PacketGen outParams) {
    Object p = getParent();
    if (p instanceof PacketType) {
      return outParams.javaPackage + "." + ((PacketType)p).name;
    }
    if (p instanceof Class<?>) {
      return ((Class<?>)p).getName();
    }
    switch (type) {
      case UnidentifiedData:    return "nxm.vrt.lib.BasicDataPacket";
      case Data:                return "nxm.vrt.lib.BasicDataPacket";
      case UnidentifiedExtData: return "nxm.vrt.lib.BasicDataPacket";
      case ExtData:             return "nxm.vrt.lib.BasicDataPacket";
      case Context:             return "nxm.vrt.lib.BasicContextPacket";
      case ExtContext:          return "nxm.vrt.lib.BasicVRTPacket";
      default: throw new RuntimeException("Unknown packet type "+type);
    }
  }

  /** Gets the C++ class name of the parent for a current packet type. */
  private String getParentClassCPP (PacketGen outParams) {
    Object p = getParent();
    if (p instanceof PacketType) {
      return ((PacketType)p).name;
    }
    if (p instanceof Class<?>) {
      return ((Class<?>)p).getSimpleName();
    }
    switch (type) {
      case UnidentifiedData:    return "BasicDataPacket";
      case Data:                return "BasicDataPacket";
      case UnidentifiedExtData: return "BasicDataPacket";
      case ExtData:             return "BasicDataPacket";
      case Context:             return "BasicContextPacket";
      case ExtContext:          return "BasicVRTPacket";
      default: throw new RuntimeException("Unknown packet type "+type);
    }
  }

  /** Gets the parent length for the current packet type (0 if unknown). */
  @SuppressWarnings("UseOfSystemOutOrSystemErr")
  private int getParentLength () {
    Object p = getParent();
    if (p instanceof PacketType) {
      return ((PacketType)p).length;
    }
    if (p instanceof Class<?>) {
      Class<?> c = (Class<?>)p;
      try {
        return c.getField("PACKET_LENGTH").getInt(null);
      }
      catch (Exception e) {
        System.err.println("Could not access PACKET_LENGTH in "+p+": "+e);
        return 0;
      }
    }
    return 0;
  }

  /** Determines if it is necessary to include the XPath methods. */
  @SuppressWarnings("UseOfSystemOutOrSystemErr")
  private boolean getDoXPath () {
    // If XPath definitions exist, include the XPath method
    if (xpathMap.size() > 0) return true;
    
    // If parent supports XPath, this should too
    Object p = getParent();
    if (p instanceof PacketType) {
      return ((PacketType)p).getDoXPath();
    }
    if (p instanceof Class<?>) {
      Class<?> c = (Class<?>)p;
      try {
        return (c.getMethod("newInstance", Object.class, TimeStamp.class) != null);
      }
      catch (NoSuchMethodException e) {
        return false;
      }
      catch (Exception e) {
        System.err.println("Could not access newInstance(Object,TimeStamp) in "+p+": "+e);
        return false;
      }
    }
    return false;
  }

  @Override
  public void writeHTML (PacketGen outParams, PrintStream html, PrintStream index, String fileName) {
    printIndexHTML(index, sect, author, descr, fileName);
    printHeaderHTML(html, sect);

    boolean streamID = (type != VRTPacket.PacketType.UnidentifiedData)
                    && (type != VRTPacket.PacketType.UnidentifiedExtData);
    String  javaDoc  = "../java_doc/"+outParams.javaPackage.replace('.','/') +"/"+name+".html";
    String  cppDoc   = "../cpp_doc/html/class"+outParams.cppNamespace+"_1_1"+name+".html";
    
    if (Arrays.binarySearch(ALWAYS_EXCLUDE, name) >= 0) {
      javaDoc = "../java_doc/nxm/vrt/lib/"+name+".html";
      cppDoc  = "../cpp_doc/html/classvrt_1_1"+name+".html";
    }
    
    html.println(toHtml(descr));
    html.println("<br>");
    html.println("<br>");
    html.println("<h2>See Also:</h2>");
    html.println("<ul>");
    html.println("  <li><a href='"+javaDoc+"'>"+name+".java</a></li>");
    html.println("  <li><a href='"+cppDoc+"'>"+name+".h</a></li>");
    html.println("</ul>");
    html.println("<center>");
    html.println("  <table border='1'>");
    html.println("    <tr><th colspan='3' class='title'>"+toTypeStringShort(type)+" Packet Class Options Table   </th></tr>");
    html.println("    <tr>");
    html.println("      <th>");
    html.println("        Class Name");
    html.println("      </th>");
    html.println("      <td colspan='2'>");
    html.println("        <tt style='font-size:large'><b>" + name + "</b></tt>");
    html.println("      </td>");
    html.println("    </tr>");
    html.println("    <tr>");
    html.println("      <th>");
    html.println("        Class Code");
    html.println("      </th>");
    html.println("      <td colspan='2'>");
    html.println("        <tt style='font-size:large'><b>" + id + "</b></tt>");
    html.println("      </td>");
    html.println("    </tr>");
    if (parent != null) {
      html.println("    <tr><th>Extends               </th><td colspan='2'> "+parent+" </td></tr>");
    }
    //html.println("    <tr><th>Packet Stream Purpose   </th><td colspan='2'> TBD </td></tr>");
    
    html.println("    <tr><th colspan='3' class='section'>         Packet Header                                      </th></tr>");
    html.println("    <tr><th class='col'>Parameter</th><th class='col'>Allowed Options</th><th class='col'>Comments</th></tr>");
    html.println("    <tr><th>Packet Type                 </th><td>" + toTypeStringShort(type)   + "</td><td>&nbsp;   </td></tr>");
    html.println("    <tr><th>Stream Identifier           </th><td>" + use(streamID)             + "</td><td>&nbsp;   </td></tr>");
    html.println("    <tr><th>Class Identifier            </th><td>" + use(true)                 + "</td><td>&nbsp;   </td></tr>");
    html.println("    <tr><th>Integer-seconds    Timestamp</th><td>" + "UTC (or GPS)"            + "</td><td>&nbsp;   </td></tr>");
    html.println("    <tr><th>Fractional-seconds Timestamp</th><td>" + "Real-time"               + "</td><td>&nbsp;   </td></tr>");
    html.println("    <tr><th>Timestamp Precision         </th><td>" + "As specified in TimestampAccuracyPacket" + "</td><td>&nbsp;</td></tr>");
    
    if (type.isData()) {
      html.println("    <tr><th colspan='3' class='section'>       Packet Payload                                     </th></tr>");
      html.println("    <tr><th class='col'>Parameter</th><th class='col'>Allowed Options</th><th class='col'>Comments</th></tr>");
      html.println("    <tr><th>Packing Method              </th><td rowspan='10'>As specified in the paired "
                                                                             + "IF Context packet.</td><td>&nbsp;   </td></tr>");
      html.println("    <tr><th>Item Packing Field Size     </th>                                        <td>&nbsp;   </td></tr>");
      html.println("    <tr><th>Data Item Size              </th>                                        <td>&nbsp;   </td></tr>");
      html.println("    <tr><th>Event-Tag Sizeer            </th>                                        <td>&nbsp;   </td></tr>");
      html.println("    <tr><th>Channel-Tag Size            </th>                                        <td>&nbsp;   </td></tr>");
      html.println("    <tr><th>Vector Size                 </th>                                        <td>&nbsp;   </td></tr>");
      html.println("    <tr><th>Real/Complex Type           </th>                                        <td>&nbsp;   </td></tr>");
      html.println("    <tr><th>Data Item Format            </th>                                        <td>&nbsp;   </td></tr>");
      html.println("    <tr><th>Sample-Repeating / Channel Repeating</th>                                <td>&nbsp;   </td></tr>");
      html.println("    <tr><th>Repeat Count                </th>                                        <td>&nbsp;   </td></tr>");
      html.println("    <tr><th colspan='3' class='section'>       Packet Trailer                                     </th></tr>");
      if (trailer == null) {
        html.println("    <tr><td colspan='3'><center>             Trailer is Not Used                       </center></td></tr>");
      }
      else if (trailer.usage == null) {
        html.println("    <tr><td colspan='3'><center>             Trailer is Optional                       </center></td></tr>");
        trailer.writeHTML(outParams, html, index, fileName);
      }
      else if (trailer.usage) {
        html.println("    <tr><td colspan='3'><center>             Trailer is Required                       </center></td></tr>");
        trailer.writeHTML(outParams, html, index, fileName);
      }
      else {
        html.println("    <tr><td colspan='3'><center>             Trailer is Not Used                       </center></td></tr>");
      }
    }
    else if (type == VRTPacket.PacketType.Context) {
      html.println("    <tr><th colspan='3' class='section'>       Context Fields                                     </th></tr>");
      html.println("    <tr><th class='col'>Parameter</th><th class='col'>Allowed Options</th><th class='col'>Comments</th></tr>");
      html.println("    <tr><th>Context Field Change Indicator   </th><td>" + use(true)          + "</td><td>&nbsp;   </td></tr>");
      html.println("    <tr><th>Reference Point Identifier       </th><td>" + use(null)          + "</td><td>&nbsp;   </td></tr>");
      html.println("    <tr><th>Bandwidth                        </th><td>" + use(null)          + "</td><td>&nbsp;   </td></tr>");
      html.println("    <tr><th>IF Reference Frequency           </th><td>" + use(null)          + "</td><td>&nbsp;   </td></tr>");
      html.println("    <tr><th>RF Reference Frequency           </th><td>" + use(null)          + "</td><td>&nbsp;   </td></tr>");
      html.println("    <tr><th>RF Frequency Offset              </th><td>" + use(null)          + "</td><td>&nbsp;   </td></tr>");
      html.println("    <tr><th>IF Band Offset                   </th><td>" + use(null)          + "</td><td>&nbsp;   </td></tr>");
      html.println("    <tr><th>Reference Level                  </th><td>" + use(null)          + "</td><td>&nbsp;   </td></tr>");
      html.println("    <tr><th>Gain                             </th><td>" + use(null)          + "</td><td>&nbsp;   </td></tr>");
      html.println("    <tr><th>Over-Range Count                 </th><td>" + use(null)          + "</td><td>&nbsp;   </td></tr>");
      html.println("    <tr><th>Sample Rate                      </th><td>" + use(null)          + "</td><td>&nbsp;   </td></tr>");
      html.println("    <tr><th>Timestamp Adjustment             </th><td>" + use(null)          + "</td><td>&nbsp;   </td></tr>");
      html.println("    <tr><th>Timestamp Calibration Time       </th><td>" + use(null)          + "</td><td>&nbsp;   </td></tr>");
      html.println("    <tr><th>Temperature                      </th><td>" + use(null)          + "</td><td>&nbsp;   </td></tr>");
      html.println("    <tr><th>Device Identifier                </th><td>" + use(null)          + "</td><td>&nbsp;   </td></tr>");
      html.println("    <tr><th>State and Event Indicators       </th><td>" + use(null)          + "</td><td>&nbsp;   </td></tr>");
      html.println("    <tr><th>--Calibrated Time  Indicator     </th><td>" + use(null)          + "</td><td>&nbsp;   </td></tr>");
      html.println("    <tr><th>--Valid Data       Indicator     </th><td>" + use(null)          + "</td><td>&nbsp;   </td></tr>");
      html.println("    <tr><th>--Reference Lock   Indicator     </th><td>" + use(null)          + "</td><td>&nbsp;   </td></tr>");
      html.println("    <tr><th>--AGC/MGC          Indicator     </th><td>" + use(null)          + "</td><td>&nbsp;   </td></tr>");
      html.println("    <tr><th>--Detected Signal  Indicator     </th><td>" + use(null)          + "</td><td>&nbsp;   </td></tr>");
      html.println("    <tr><th>--Over-Range       Indicator     </th><td>" + use(null)          + "</td><td>&nbsp;   </td></tr>");
      html.println("    <tr><th>--Sample Loss      Indicator     </th><td>" + use(null)          + "</td><td>&nbsp;   </td></tr>");
      html.println("    <tr><th>--User Defined Bits              </th><td>" + use(null)          + "</td><td>Reserved for future use, set to zero.</td></tr>");
      html.println("    <tr><th>Data Packet Payload Format       </th><td>" + use(null)          + "</td><td>&nbsp;   </td></tr>");
      html.println("    <tr><th>Formatted GPS Geolocation        </th><td>" + use(null)          + "</td><td>&nbsp;   </td></tr>");
      html.println("    <tr><th>ECEF Ephemeris                   </th><td>" + use(null)          + "</td><td>&nbsp;   </td></tr>");
      html.println("    <tr><th>Relative Ephemeris               </th><td>" + use(null)          + "</td><td>&nbsp;   </td></tr>");
      html.println("    <tr><th>Ephemeris Reference Identifier   </th><td>" + use(null)          + "</td><td>&nbsp;   </td></tr>");
      html.println("    <tr><th>GPS ASCII                        </th><td>" + use(null)          + "</td><td>&nbsp;   </td></tr>");
      html.println("    <tr><th>Context Association List         </th><td>" + use(null)          + "</td><td>&nbsp;   </td></tr>");
      html.println("    <tr><th>ECEF Ephemeris Adjunct           </th><td>" + use(null)          + "</td><td>&nbsp;   </td></tr>");
      html.println("    <tr><th>Relative Ephemeris Adjunct       </th><td>" + use(null)          + "</td><td>&nbsp;   </td></tr>");
    }
    else if (type == VRTPacket.PacketType.ExtContext) {
      String len = (length >= 0)? ""+length : ""+(-length)+"+";

      html.println("    <tr><th colspan='3'  class='section'>          Packet Payload                                 </th></tr>");
      html.println("    <tr><td colspan='3'><center>        Total Payload Length = "+len+" bytes             </center></td></tr>");
      html.println("    <tr>");
      html.println("      <td colspan='3'>");
      html.println("        <table class='nested'>");
      html.println("          <tr>");
      html.println("            <th class='col'>Offset</th>");
      html.println("            <th class='col'>Type</th>");
      html.println("            <th class='col'>Name</th>");
      html.println("            <th class='col'>Units</th>");
      html.println("            <th class='col'>Range</th>");
      html.println("            <th class='col'>Description</th>");
      html.println("          </tr>");
      for (Field f : fields) {
        f.writeHTML(outParams, html, index, fileName);
      }
      html.println("        </table>");
      html.println("      </td>");
      html.println("    </tr>");
    }
    html.println("  </table>");
    html.println("</center>");

    if (type == VRTPacket.PacketType.ExtContext) {
      if (enums.size() > 0) {
        html.println("<br>");
        html.println("<h2>Enumeration Definitions</h2>");
        for (Enumeration e : enums) {
          e.writeHTML(outParams, html, index, fileName);
        }
      }
      if (recordMap.size() > 0) {
        html.println("<br>");
        html.println("<h2>Record Definitions</h2>");
        for (Record r : recordMap.values()) {
          r.writeHTML(outParams, html, index, fileName);
        }
      }
    }
    printFooterHTML(html);
  }

  /** Generates the default VRT packet header applicable to this packet type. */
  private byte[] createDefaultHeader () {
    int tot = getDefaultPacketLength()/4;
    byte[] buf = new byte[VRTPacket.MAX_HEADER_LENGTH];
    buf[ 0] = (trailer == null)? (byte)((type.ordinal() << 4) | 8) : (byte)((type.ordinal() << 4) | 12);
    buf[ 1] = (byte)160;
    buf[ 2] = (byte)((tot >> 8) & 255);
    buf[ 3] = (byte)((tot) & 255);
    buf[ 8] = (byte)(classID >> 56);
    buf[ 9] = (byte)(classID >> 48);
    buf[10] = (byte)(classID >> 40);
    buf[11] = (byte)(classID >> 32);
    buf[12] = (byte)(classID >> 24);
    buf[13] = (byte)(classID >> 16);
    buf[14] = (byte)(classID >>  8);
    buf[15] = (byte)(classID      );
    return buf;
  }
  
  private int getDefaultPayloadLength () {
    return Math.abs(length);
  }
  
  private int getDefaultPacketLength () {
    int len = getDefaultPayloadLength();
    return (trailer == null)? (VRTPacket.MAX_HEADER_LENGTH + len + 0)
                            : (VRTPacket.MAX_HEADER_LENGTH + len + VRTPacket.MAX_TRAILER_LENGTH);
  }

  /** Gets the class description in HTML usable in Java or C++. */
  private CharSequence getClassDescrHTML (boolean isJava) {
    String        indent = (isJava)? "" : "  ";
    StringBuilder str    = new StringBuilder();
    
    str.append(indent).append("/** ").append(toHtmlTitle(sect,indent,isJava)).append("\n");
    str.append(indent).append(" *  ").append(toHtmlDoc(descr,indent+" *  ")).append("\n");
    if (!author.trim().isEmpty()) {
      str.append(indent).append(" *  \n");
      str.append(indent).append(" *  @author ").append(author).append("\n");
    }
    str.append(indent).append(" */");
    return str;
  }

  @Override
  public void writeCPP (PacketGen outParams, PrintStream hdr, PrintStream cpp) {
    String superClass = getParentClassCPP(outParams);

    hdr.println("//");
    hdr.println("//  This class was auto-generated, do not modify.");
    hdr.println("//");
    hdr.println("#ifndef _"+name+"_h");
    hdr.println("#define _"+name+"_h");
    hdr.println("");
    hdr.println("#include \"BasicVRTPacket.h\"");
    hdr.println("#include \"BasicDataPacket.h\"");
    hdr.println("#include \"BasicContextPacket.h\"");
    hdr.println("#include <cstring>");
    hdr.println("#include <vector>");
    hdr.println("");
    hdr.println("using namespace std;");
    hdr.println("using namespace vrt;");
    hdr.println("");
    hdr.println("namespace " + outParams.cppNamespace + " {");
    
    cpp.println("//");
    cpp.println("//  This class was auto-generated, do not modify.");
    cpp.println("//");
    cpp.println("#include \"" + name + ".h\"");
    cpp.println("");
    cpp.println("using namespace std;");
    cpp.println("using namespace vrt;");
    if (!outParams.cppNamespace.equals("vrt")) {
      cpp.println("using namespace "+outParams.cppNamespace+";");
    }
    cpp.println("");

    if (enums.size() > 0) {
      hdr.println("  //======================================================================");
      hdr.println("  // ENUMERATIONS");
      hdr.println("  //======================================================================");
      cpp.println("//======================================================================");
      cpp.println("// ENUMERATIONS");
      cpp.println("//======================================================================");
      for (Enumeration e : enums) {
        e.writeCPP(outParams, hdr, cpp);
      }
    }
    if (recordMap.size() > 0) {
      hdr.println("  //======================================================================");
      hdr.println("  // RECORDS");
      hdr.println("  //======================================================================");
      cpp.println("//======================================================================");
      cpp.println("// RECORDS");
      cpp.println("//======================================================================");
      for (Record r : recordMap.values()) {
        r.writeCPP(outParams, hdr, cpp);
      }
    }
    hdr.println("  ");
    hdr.println("  //======================================================================");
    hdr.println("  // PACKET TYPE");
    hdr.println("  //======================================================================");
    hdr.println(getClassDescrHTML(false));
    hdr.println("  class " + name + " : public " + superClass + " {");
    hdr.println("    /** The packet type (<tt>PacketType_"+type+"</tt>). */");
    hdr.println("    public: static const PacketType PACKET_TYPE;");
    hdr.println("    ");
    if (length == 0) {
      hdr.println("    /** The packet length (<tt>"+length+"</tt>) excluding the header and trailer (if applicable).");
      hdr.println("     *  A value of "+length+" indicates that the length is unknown.");
      hdr.println("     */");
      hdr.println("    public: static const int32_t PACKET_LENGTH;");
    }
    else if (length > 0) {
      hdr.println("    /** The packet length (<tt>"+length+"</tt>) excluding the header and trailer (if applicable).");
      hdr.println("     *  A value of "+length+" indicates that the length is fixed at "+length+" octets.");
      hdr.println("     */");
      hdr.println("    public: static const int32_t PACKET_LENGTH;");
    }
    else {
      hdr.println("    /** The packet length (<tt>"+length+"</tt>) excluding the header and trailer (if applicable).");
      hdr.println("        A value of "+length+" indicates that the length is variable with a minimum length of "+(-length)+" octets.");
      hdr.println("     */");
      hdr.println("    public: static const int32_t PACKET_LENGTH;");
    }
    hdr.println("    ");
    hdr.println("    /** The class identifier in numeric form (<tt>0x"+toHexString(classID,7)+"</tt>). */");
    hdr.println("    public: static const int64_t CLASS_IDENTIFIER;");
    hdr.println("    ");
    hdr.println("    /** The class identifier in string form (<tt>"+id+"</tt>). */");
    hdr.println("    public: static const string CLASS_ID;");
    hdr.println("    ");
    hdr.println("    //======================================================================");
    hdr.println("    // CONSTRUCTORS");
    hdr.println("    //======================================================================");
    hdr.println("    /** Creates a new instance ready for the fields to be set. The stream ID");
    hdr.println("     *  and timecode values will be initialized to 0.");
    hdr.println("     */");
    hdr.println("    public: " + name + " ();");
    hdr.println("    ");
    hdr.println("    /** Creates a new instance from the given packet.");
    hdr.println("     *  @param p    The packet.");
    hdr.println("     *  @throws VRTException If the packet given does not match this type.");
    hdr.println("     */");
    hdr.println("    public: " + name + " (const BasicVRTPacket &p);");
    hdr.println("    ");
    hdr.println("    /** Internal constructor, for use by subclasses.");
    hdr.println("     *  @param p                The packet (already initialized).");
    hdr.println("     *  @param type             The expected packet type.");
    hdr.println("     *  @param classID          The expected classID.");
    hdr.println("     *  @param minPayloadLength The minimum payload length.");
    hdr.println("     *  @param maxPayloadLength The maximum payload length (&lt;0 if not pre-defined).");
    hdr.println("     *  @throws IllegalArgumentException If packet type or classID of the packet are invalid, or if");
    hdr.println("     *                                   the payload length is invalid.");
    hdr.println("     */");
    hdr.println("    protected: " + name + " (const BasicVRTPacket &p, PacketType type,");
    hdr.println("                            int64_t classID, int32_t minPayloadLength, int32_t maxPayloadLength);");
    hdr.println("    ");
    hdr.println("    /** Creates a new instance accessing the given data buffer.");
    hdr.println("     *  @param bbuf     The data buffer to use.");
    hdr.println("     *  @param readOnly Should users of this instance be able to modify the underlying data buffer?");
    hdr.println("     */");
    hdr.println("    public: " + name + " (const vector<char> &bbuf, bool readOnly);");
    hdr.println("    ");
    hdr.println("    //======================================================================");
    hdr.println("    // UTILITY METHODS");
    hdr.println("    //======================================================================");
    hdr.println("    ");
    hdr.println("    public:    virtual string    getPacketValid (bool strict, int32_t length) const;");
    hdr.println("    protected: virtual void      toStringStream (ostringstream &str) const;");
    hdr.println("    public:    virtual int32_t   getFieldCount () const;");
    hdr.println("    public:    virtual string    getFieldName (int32_t id) const;");
    hdr.println("    public:    virtual ValueType getFieldType (int32_t id) const;");
    hdr.println("    public:    virtual Value*    getField (int32_t id) const;");
    hdr.println("    public:    virtual void      setField (int32_t id, const Value* val);");
    
    cpp.println("");
    cpp.println("//======================================================================");
    cpp.println("// NON-CLASS METHODS");
    cpp.println("//======================================================================");
    cpp.println("/** Creates the default header for the packet. This is defined separate");
    cpp.println(" *  from the class to account for the systems/compilers that disallow");
    cpp.println(" *  calling of subclass methods from the constructor.");
    cpp.println(" */");
    cpp.println("static inline vector<char> "+name+"_createDefaultPacket () {");
    cpp.println("  vector<char> buf("+getDefaultPacketLength()+");");
    byte[] buf = createDefaultHeader();
    for (int i = 0; i < buf.length; i++) {
      cpp.println("  buf[" + i + "] = (char)" + buf[i] + ";");
    }
    cpp.println("  return buf;");
    cpp.println("}");
    cpp.println("");
    cpp.println("//======================================================================");
    cpp.println("// PACKET TYPE STATIC CONSTANTS");
    cpp.println("//======================================================================");
    cpp.println("const PacketType "+name+"::PACKET_TYPE = PacketType_"+type+";");
    cpp.println("const int32_t    "+name+"::PACKET_LENGTH = "+length+";");
    cpp.println("const int64_t    "+name+"::CLASS_IDENTIFIER = __INT64_C(0x"+toHexString(classID,7)+");");
    cpp.println("const string     "+name+"::CLASS_ID(\""+id+"\");");
    cpp.println("");
    cpp.println("//======================================================================");
    cpp.println("// PACKET TYPE CONSTRUCTORS");
    cpp.println("//======================================================================");
    cpp.println(name + "::" + name + " () :");
    cpp.println("  "+superClass+"("+name+"_createDefaultPacket(), false)");
    cpp.println("{");
    cpp.println(" // done");
    cpp.println("}");
    cpp.println("");
    cpp.println(name + "::" + name + " (const vector<char> &bbuf, bool readOnly) :");
    cpp.println("  "+superClass+"(bbuf, readOnly)");
    cpp.println("{");
    cpp.println(" // done");
    cpp.println("}");
    cpp.println("");
    cpp.println(name + "::" + name + " (const BasicVRTPacket &p) :");
    cpp.println("  "+superClass+"(p, PACKET_TYPE, CLASS_IDENTIFIER, " + getDefaultPayloadLength() + ", PACKET_LENGTH)");
    cpp.println("{");
    cpp.println(" // done");
    cpp.println("}");
    cpp.println("");
    cpp.println(name + "::" + name + " (const BasicVRTPacket &p, PacketType type,");
    cpp.println("                       int64_t classID, int32_t minPayloadLength, int32_t maxPayloadLength) :");
    cpp.println("  "+superClass+"(p, type, classID, minPayloadLength, maxPayloadLength)");
    cpp.println("{");
    cpp.println(" // done");
    cpp.println("}");
    cpp.println("");
    cpp.println("//======================================================================");
    cpp.println("// UTILITY METHODS");
    cpp.println("//======================================================================");
    cpp.println("string "+name+"::getPacketValid (bool strict, int32_t length) const {");
    cpp.println("  string err = "+superClass+"::getPacketValid(strict, length);");
    cpp.println("  if (err != \"\") return err;");
    cpp.println("  ");
    cpp.println("  bool okType = (getPacketType() == PACKET_TYPE);");
    cpp.println("  bool okID   = (getClassIdentifier() == CLASS_IDENTIFIER);");
    cpp.println("  ");
    cpp.println("  if (!okType) {");
    cpp.println("    ostringstream str;");
    cpp.println("    str << \"Invalid use of \" << getClassName() << \" with \" << getPacketType() << \" packet.\";");
    cpp.println("    return str.str();");
    cpp.println("  }");
    cpp.println("  if (!okID) {");
    cpp.println("    ostringstream str;");
    cpp.println("    str << \"Invalid use of \" << getClassName() << \" with ClassID=\" << getClassID() << \".\";");
    cpp.println("    return str.str();");
    cpp.println("  }");
    cpp.println("  return \"\";");
    cpp.println("}");
    cpp.println("");
    cpp.println("void "+name+"::toStringStream (ostringstream &str) const {");
    cpp.println("  "+superClass+"::toStringStream(str);");
    for (Field f : fields) {
      if (f.methodName != null) {
        Type t = f.getType();
        if (t.isArrayType()) {
          String singleName = notPlural(f.methodName);
          Type typ = t.getArrayType();
          cpp.println("  { // Print up to 8 of the array entries");
          cpp.println("    str << \" " + f.name + "={\";");
          cpp.println("    int32_t count = get" + singleName + "Count();");
          cpp.println("    for (int32_t i = 0; (i < count) && (i < 8); i++) {");
          cpp.println("      if (i > 0) str << \", \";");
          if (typ.isStringType()) {
            cpp.println("      str << \"\'\" << get" + singleName + "(i) << \"\'\";");
          }
          else if (typ.isMetadataType()) {
            cpp.println("      str << \"[\" << get" + singleName + "(i) << \"]\";");
          } if (typ instanceof Record) {
            cpp.println("      str << \"[\" << get" + singleName + "(i) << \"]\";");
          }
          else {
            cpp.println("      str << get" + singleName + "(i);");
          }
          cpp.println("    }");
          cpp.println("    if (count > 8) str << \", ...}\";");
          cpp.println("    else           str << \"}\";");
          cpp.println("  }");
        }
        else if (t.isStringType()) {
          cpp.println("  str << \" " + f.name + "=\'\" << get" + f.methodName + "() << \"\'\";");
        }
        else if (t.isMetadataType()) {
          cpp.println("  str << \" " + f.name + "=[\" << get" + f.methodName + "() << \"]\";");
        }
        else if (t instanceof Record) {
          cpp.println("  str << \" " + f.name + "=[\" << get" + f.methodName + "() << \"]\";");
        }
        else {
          cpp.println("  str << \" " + f.name + "=\" << get" + f.methodName + "();");
        }
      }
    }
    cpp.println("}");
    
    int fieldCount = 0;
    for (int i = 0; i < fields.size(); i++) {
      if (fields.get(i).methodName == null) continue;
      fieldCount++;
    }
    cpp.println("");
    cpp.println("int32_t "+name+"::getFieldCount () const {");
    cpp.println("  return "+superClass+"::getFieldCount() + "+fieldCount+";");
    cpp.println("}");
    cpp.println("");
    
    cpp.println("string "+name+"::getFieldName (int32_t id) const {");
    cpp.println("  switch (id - "+superClass+"::getFieldCount()) {");
    for (int i=0,j=0; i < fields.size(); i++) {
      if (fields.get(i).methodName == null) continue;
      cpp.println("    case "+(j++)+": return \""+fields.get(i).name+"\";");
    }
    cpp.println("    default: return "+superClass+"::getFieldName(id);");
    cpp.println("  }");
    cpp.println("}");
    cpp.println("");
    
    cpp.println("ValueType "+name+"::getFieldType (int32_t id) const {");
    cpp.println("  switch (id - "+superClass+"::getFieldCount()) {");
    for (int i=0,j=0; i < fields.size(); i++) {
      if (fields.get(i).methodName == null) continue;
      cpp.println("    case "+(j++)+": return "+fields.get(i).getCPPFieldType()+";");
    }
    cpp.println("    default: return "+superClass+"::getFieldType(id);");
    cpp.println("  }");
    cpp.println("}");
    cpp.println("");
    
    cpp.println("Value* "+name+"::getField (int32_t id) const {");
    cpp.println("  switch (id - "+superClass+"::getFieldCount()) {");
    for (int i=0,j=0; i < fields.size(); i++) {
      if (fields.get(i).methodName == null) continue;
      cpp.println("    case "+(j++)+": return "+fields.get(i).getCPPGetField()+";");
    }
    cpp.println("    default: return "+superClass+"::getField(id);");
    cpp.println("  }");
    cpp.println("}");
    cpp.println("");
    
    cpp.println("void "+name+"::setField (int32_t id, const Value* val) {");
    cpp.println("  switch (id - "+superClass+"::getFieldCount()) {");
    for (int i=0,j=0; i < fields.size(); i++) {
      if (fields.get(i).methodName == null) continue;
      cpp.println("    case "+(j++)+": "+fields.get(i).getCPPSetField()+"; break;");
    }
    cpp.println("    default: "+superClass+"::setField(id,val); break;");
    cpp.println("  }");
    cpp.println("}");
    cpp.println("");
    
    if (getDoXPath()) {
      // TBD
    }
    hdr.println("    //======================================================================");
    hdr.println("    // GET/SET METHODS");
    hdr.println("    //======================================================================");
    cpp.println("//======================================================================");
    cpp.println("// GET/SET METHODS");
    cpp.println("//======================================================================");
    for (Field f : fields) {
      if (f.methodName != null) {
        f.writeCPP(outParams, hdr, cpp);
      }
    }
    if (trailer != null) {
      hdr.println("    //======================================================================");
      hdr.println("    // TRAILER");
      hdr.println("    //======================================================================");
      cpp.println("//======================================================================");
      cpp.println("// TRAILER");
      cpp.println("//======================================================================");
      trailer.writeCPP(outParams, hdr, cpp);
    }
    hdr.println("  };");
    hdr.println("};");
    for (Enumeration e : enums) {
      e.writeCPP2(outParams, hdr, cpp);
    }
    hdr.println("#endif /* _"+name+"_h */");
    hdr.println("");

    cpp.println("");
  }

  @Override
  public void writeJava (PacketGen outParams, PrintStream out) {
    String superClass = getParentClassJava(outParams);
    
    out.println("//");
    out.println("//  This class was auto-generated, do not modify.");
    out.println("//");
    out.println("package " + outParams.javaPackage + ";");
    out.println("");
    out.println(getClassDescrHTML(true));
    out.println("public class " + name + " extends " + superClass + " {");
    out.println("  /** The packet type ({@link nxm.vrt.lib.VRTPacket.PacketType#"+type+"}). */");
    out.println("  public static final nxm.vrt.lib.VRTPacket.PacketType PACKET_TYPE = nxm.vrt.lib.VRTPacket.PacketType."+type+";");
    out.println("    ");
    if (length == 0) {
      out.println("  /** The packet length (<tt>"+length+"</tt>) excluding the header and trailer (if applicable).");
      out.println("   *  A value of "+length+" indicates that the length is unknown.");
      out.println("   */");
      out.println("  public static final int PACKET_LENGTH = "+length+";");
    }
    else if (length > 0) {
      out.println("  /** The packet length (<tt>"+length+"</tt>) excluding the header and trailer (if applicable).");
      out.println("   *  A value of "+length+" indicates that the length is fixed at "+length+" octets.");
      out.println("   */");
      out.println("  public static final int PACKET_LENGTH = "+length+";");
    }
    else {
      out.println("  /** The packet length (<tt>"+length+"</tt>) excluding the header and trailer (if applicable).");
      out.println("   *  A value of "+length+" indicates that the length is variable with a minimum length of "+(-length)+" octets.");
      out.println("   */");
      out.println("  public static final int PACKET_LENGTH = "+length+";");
    }
    out.println("  ");
    out.println("  /** The class identifier in numeric form (<tt>0x"+toHexString(classID,7)+"L</tt>). */");
    out.println("  public static final long CLASS_IDENTIFIER = 0x"+toHexString(classID,7)+"L;");
    out.println("  ");
    out.println("  /** The class identifier in string form (<tt>"+id+"</tt>). */");
    out.println("  public static final java.lang.String CLASS_ID = \""+id+"\";");
    out.println("  ");
    out.println("  ////////////////////////////////////////////////////////////////////////");
    out.println("  // CONSTRUCTORS");
    out.println("  ////////////////////////////////////////////////////////////////////////");
    out.println("  /** Creates a new instance ready for the fields to be set. The stream ID");
    out.println("   *  and timecode values will be initialized to 0.");
    out.println("   */");
    out.println("  public " + name + " () {");
    out.println("    super();");
    out.println("  }");
    out.println("  ");
    out.println("  /** Creates a new instance from the given packet.");
    out.println("   *  @param p                The packet.");
    out.println("   *  @param copy             Must the data be copied? If true, the resulting packet must copy");
    out.println("   *                          the data from <tt>p</tt> in such a way that no changes to the");
    out.println("   *                          returned packet will alter <tt>p</tt>. If false, a \"fast\"");
    out.println("   *                          initialization is used, meaning that the resulting packet may");
    out.println("   *                          or may not be a copy of the given data, the implementation will");
    out.println("   *                          use the fastest creation method available.");
    out.println("   *  @throws IllegalArgumentException If the packet given does not match this type.");
    out.println("   */");
    out.println("  public " + name + " (nxm.vrt.lib.VRTPacket p, boolean copy) {");
    out.println("    this(p, copy, PACKET_TYPE, CLASS_IDENTIFIER, " + getDefaultPayloadLength() + ", PACKET_LENGTH);");
    out.println("  }");
    out.println("  ");
    out.println("  /** Internal constructor, for use by subclasses.");
    out.println("   *  @param p                The packet (already initialized).");
    out.println("   *  @param copy             Must the data be copied? If true, the resulting packet must copy");
    out.println("   *                          the data from <tt>p</tt> in such a way that no changes to the");
    out.println("   *                          returned packet will alter <tt>p</tt>. If false, a \"fast\"");
    out.println("   *                          initialization is used, meaning that the resulting packet may");
    out.println("   *                          or may not be a copy of the given data, the implementation will");
    out.println("   *                          use the fastest creation method available.");
    out.println("   *  @param type             The expected packet type.");
    out.println("   *  @param classID          The expected classID.");
    out.println("   *  @param minPayloadLength The minimum payload length.");
    out.println("   *  @param maxPayloadLength The maximum payload length (&lt;0 if not pre-defined).");
    out.println("   *  @throws IllegalArgumentException If packet type or classID of the packet are invalid, or if");
    out.println("   *                                   the payload length is invalid.");
    out.println("   */");
    out.println("  protected " + name + " (nxm.vrt.lib.VRTPacket p, boolean copy, nxm.vrt.lib.VRTPacket.PacketType type,");
    out.println("                          long classID, int minPayloadLength, int maxPayloadLength) {");
    out.println("    super(p, copy, type, classID, minPayloadLength, maxPayloadLength);");
    out.println("  }");
    out.println("  ");
    out.println("  /** Creates a new instance accessing the given data buffer.");
    out.println("   *  @param bbuf     The data buffer to use.");
    out.println("   *  @param boff     The byte offset into the bbuf to start reading/writing at.");
    out.println("   *  @param blen     The length of bbuf in bytes (-1=use size in header of packet).");
    out.println("   *  @param readOnly Should users of this instance be able to modify the underlying data buffer?");
    out.println("   *  @param direct   Should the methods in this class directly interact with the buffer or should");
    out.println("   *                  a copy of the data be made so that changes to the buffer don\'t affect this");
    out.println("   *                  instance?");
    out.println("   *  @throws NullPointerException If the buffer is null.");
    out.println("   *  @throws IllegalArgumentException If the offset is negative or greater than the");
    out.println("   *          length of the buffer given. Also thrown if the buffer length is -1 and a copy");
    out.println("   *          can not be made because the sizes in the header of the packet are invalid.");
    out.println("   */");
    out.println("  public " + name + " (byte[] bbuf, int boff, int blen, boolean readOnly, boolean direct) {");
    out.println("    super(bbuf, boff, blen, readOnly, direct);");
    out.println("  }");
    out.println("  ");
    out.println("  ////////////////////////////////////////////////////////////////////////");
    out.println("  // UTILITY METHODS");
    out.println("  ////////////////////////////////////////////////////////////////////////");
    out.println("  ");
    out.println("  @Override");
    out.println("  public " + name + " copy () {");
    out.println("    return new " + name + "(this, true);");
    out.println("  }");
    out.println("  ");
    out.println("  @Override");
    out.println("  public String getPacketValid (boolean strict, int length) {");
    out.println("    String err = super.getPacketValid(strict, length);");
    out.println("    ");
    out.println("    if (err == null) {");
    out.println("    Long id = getClassIdentifier();");
    out.println("      boolean okType = (getPacketType() == PACKET_TYPE);");
    out.println("      boolean okID   = (id != null) && (id.longValue() == CLASS_IDENTIFIER);");
    out.println("      ");
    out.println("      if (!okType) return \"Invalid use of \"+getClass().getName()");
    out.println("                         +\" with \"+getPacketType()+\" packet.\";");
    out.println("      if (!okID  ) return \"Invalid use of \"+getClass().getName()");
    out.println("                         +\" with ClassID=\"+getClassID()+\".\";");
    out.println("    }");
    out.println("    return err;");
    out.println("  }");
    out.println("  ");
    out.println("  @Override");
    out.println("  protected byte[] createDefaultHeader () {");
    out.println("    byte[] buf = new byte[MAX_HEADER_LENGTH];");
    byte[] buf = createDefaultHeader();
    for (int i = 0; i < buf.length; i++) {
      out.println("    buf[" + i + "] = (byte)" + buf[i] + ";");
    }
    out.println("    return buf;");
    out.println("  }");
    out.println("  ");
    out.println("  @Override");
    out.println("  protected byte[] createDefaultPayload () {");
    out.println("    return new byte[" + getDefaultPayloadLength() + "];");
    out.println("  }");
    out.println("  ");
    out.println("  @Override");
    out.println("  protected byte[] createDefaultTrailer () {");
    if (trailer == null) {
      out.println("    return EMPTY;");
    }
    else {
      out.println("    return new byte[MAX_TRAILER_LENGTH];");
    }
    out.println("  }");
    out.println("  ");
    out.println("  ");
    out.println("  @Override");
    out.println("  protected java.lang.StringBuilder toStringBuilder () {");
    out.println("    java.lang.StringBuilder str = super.toStringBuilder();");
    for (Field f : fields) {
      if (f.methodName != null) {
        Type t = f.getType();
        if (t.isArrayType()) {
          String singleName = notPlural(f.methodName);
          Type typ = t.getArrayType();
          out.println("    { // Print up to 8 of the array entries");
          out.println("      str.append(\" " + f.name + "={\");");
          out.println("      int count = get" + singleName + "Count();");
          out.println("      for (int i = 0; (i < count) && (i < 8); i++) {");
          out.println("        if (i > 0) str.append(\", \");");
          if (typ.isStringType()) {
            out.println("        str.append(\"\'\").append(get" + singleName + "(i)).append(\"\'\");");
          }
          else if (typ.isMetadataType()) {
            out.println("        str.append(\"[\").append(get" + singleName + "(i)).append(\"]\");");
          }
          else if (typ instanceof Record) {
            out.println("        str.append(\"[\").append(get" + singleName + "(i)).append(\"]\");");
          }
          else {
            out.println("        str.append(get" + singleName + "(i));");
          }
          out.println("      }");
          out.println("      if (count > 8) str.append(\", ...}\");");
          out.println("      else           str.append(\"}\");");
          out.println("    }");
        }
        else if (t.isStringType()) {
          out.println("    str.append(\" " + f.name + "=\'\").append(get" + f.methodName + "()).append(\"\'\");");
        }
        else if (t.isMetadataType()) {
          out.println("    str.append(\" " + f.name + "=[\").append(get" + f.methodName + "()).append(\"]\");");
        }
        else if (t instanceof Record) {
          out.println("    str.append(\" " + f.name + "=[\").append(get" + f.methodName + "()).append(\"]\");");
        }
        else {
          out.println("    str.append(\" " + f.name + "=\").append(get" + f.methodName + "());");
        }
      }
    }
    out.println("    return str;");
    out.println("  }");
    
    int fieldCount = 0;
    for (int i = 0; i < fields.size(); i++) {
      if (fields.get(i).methodName == null) continue;
      fieldCount++;
    }
    
    out.println("");
    out.println("  @Override");
    out.println("  public int getFieldCount () {");
    out.println("    return super.getFieldCount() + "+fieldCount+";");
    out.println("  }");
    out.println("");
    
    out.println("  @Override");
    out.println("  public String getFieldName (int id) {");
    out.println("    switch (id - super.getFieldCount()) {");
    for (int i=0,j=0; i < fields.size(); i++) {
      if (fields.get(i).methodName == null) continue;
      out.println("      case "+(j++)+": return \""+fields.get(i).name+"\";");
    }
    out.println("      default: return super.getFieldName(id);");
    out.println("    }");
    out.println("  }");
    out.println("");
    
    out.println("  @Override");
    out.println("  public Class<?> getFieldType (int id) {");
    out.println("    switch (id - super.getFieldCount()) {");
    for (int i=0,j=0; i < fields.size(); i++) {
      if (fields.get(i).methodName == null) continue;
      out.println("      case "+(j++)+": return "+fields.get(i).getJavaFieldType()+";");
    }
    out.println("      default: return super.getFieldType(id);");
    out.println("    }");
    out.println("  }");
    out.println("");
    
    out.println("  @Override");
    out.println("  public Object getField (int id) {");
    out.println("    switch (id - super.getFieldCount()) {");
    for (int i=0,j=0; i < fields.size(); i++) {
      if (fields.get(i).methodName == null) continue;
      out.println("      case "+(j++)+": return "+fields.get(i).getJavaGetField()+";");
    }
    out.println("      default: return super.getField(id);");
    out.println("    }");
    out.println("  }");
    out.println("");
    
    out.println("  @Override");
    out.println("  public void setField (int id, Object val) {");
    out.println("    switch (id - super.getFieldCount()) {");
    for (int i=0,j=0; i < fields.size(); i++) {
      if (fields.get(i).methodName == null) continue;
      out.println("      case "+(j++)+": "+fields.get(i).getJavaSetField()+"; break;");
    }
    out.println("      default: super.setField(id,val); break;");
    out.println("    }");
    out.println("  }");
    out.println("");
    
    if (getDoXPath()) {
      for (XPath x : xpathMap.values()) {
        x.writeConstant(outParams, out);
      }
      out.println("  ");
      out.println("  /** Initializes this instance using the XPath expressions.");
      out.println("   *  @param xml The XML document.");
      out.println("   *  @throws org.jdom.JDOMException If there is a problem evaluating the xpath expression.");
      out.println("   */");
      if (parent != null) {
        out.println("  @Override");
        out.println("  protected void init (org.jdom.Document xml) throws org.jdom.JDOMException {");
        out.println("    super.init(xml);");
      }
      else {
        out.println("  protected void init (org.jdom.Document xml) throws org.jdom.JDOMException {");
      }
      for (XPath x : xpathMap.values()) {
        x.writeJava(outParams, out);
      }
      out.println("  }");
      out.println("  ");
      out.println("  ");
      out.println("  /** Creates a new instance and <b>paritally</b> initializes it from the");
      out.println("   *  provided XML document using the following pre-defined XPath expressions.");
      out.println("   *  <pre>");
      for (XPath x : xpathMap.values()) {
        x.writeComment(outParams, out);
      }
      out.println("      </pre>");
      if (parent != null) {
        out.println("   *  Note that all of the XPath expressions defined in {@link " + superClass + "#newInstance} also apply.");
      }
      out.println("   *  @param xml The XML document.");
      out.println("   *  @param ts  The packet time stamp.");
      out.println("   *  @throws org.jdom.JDOMException If there is a problem evaluating the xpath expression.");
      out.println("   */");
      out.println("  public static " + name + " newInstance (java.lang.Object xml, nxm.vrt.lib.TimeStamp ts) throws org.jdom.JDOMException {");
      out.println("    " + name + " p = new " + name + "();");
      out.println("    p.setTimeStamp(ts);");
      out.println("    p.init(xml);");
      out.println("    return p;");
      out.println("  }");
      out.println("  ");
    }
    out.println("  ////////////////////////////////////////////////////////////////////////");
    out.println("  // GET/SET METHODS");
    out.println("  ////////////////////////////////////////////////////////////////////////");
    for (Field f : fields) {
      if (f.methodName != null) {
        f.writeJava(outParams, out);
      }
    }
    if (trailer != null) {
      out.println("  ////////////////////////////////////////////////////////////////////////");
      out.println("  // TRAILER");
      out.println("  ////////////////////////////////////////////////////////////////////////");
      trailer.writeJava(outParams, out);
    }
    if (enums.size() > 0) {
      out.println("  ////////////////////////////////////////////////////////////////////////");
      out.println("  // ENUMERATIONS");
      out.println("  ////////////////////////////////////////////////////////////////////////");
      for (Enumeration e : enums) {
        e.writeJava(outParams, out);
      }
    }
    if (recordMap.size() > 0) {
      out.println("  ////////////////////////////////////////////////////////////////////////");
      out.println("  // RECORDS");
      out.println("  ////////////////////////////////////////////////////////////////////////");
      for (Record r : recordMap.values()) {
        r.writeJava(outParams, out);
      }
    }
    out.println("}");
  }

  /** Gets the type corresponding to the given type name. */
  public Type getType (String typeName) {
    Type t;
    int i1 = typeName.indexOf('[');
    if (i1 > 0) {
      int i2 = typeName.indexOf(']');
      String base = typeName.substring(0, i1);
      String mult = typeName.substring(i1 + 1, i2);
      if (base.equals("ascii")) return new AsciiType(mult);
      if (base.equals("utf8" )) return new UTF8Type(mult);
      if (base.equals("meta" )) return new MetadataType(mult);
      return new ArrayType(getType(base), mult);
    }

    t = BASIC_TYPES.get(typeName);    if (t != null) return t;
    t = enumMap.get(typeName);        if (t != null) return t;
    t = recordMap.get(typeName);      if (t != null) return t;

    Object p = getParent();
    if (p instanceof PacketType) {
      return ((PacketType)p).getType(typeName);
    }
    else if (p instanceof Class<?>) {
      return getType(typeName, (Class<?>)p);
    }

    throw new RuntimeException("Unknown type " + typeName);
  }

  /** Gets the type from a pre-compiled parent class. */
  private Type getType (String typeName, Class<?> parent) {
    for (Class<?> c : parent.getClasses()) {
      if (typeName.equals("/"+c.getSimpleName()+"/")) {
        return new Type.PrecompiledType(c);
      }
    }
    Class<?> p = parent.getSuperclass();
    if (p != null) return getType(typeName, p);
    throw new RuntimeException("Unknown type " + typeName);
  }
}

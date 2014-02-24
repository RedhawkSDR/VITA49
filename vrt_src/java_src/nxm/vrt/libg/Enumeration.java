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
import java.util.LinkedList;
import java.util.List;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

import static nxm.vrt.lib.XmlUtilities.evalXPath;
import static nxm.vrt.libg.PacketGen.toHtmlDoc;

/** An enumeration used within a packet definition.
 *
 *  @author 
 */
class Enumeration implements Type {
  final PacketType      packetType;
  final String          name;
  final String          type;
  final String          title;
  final String          descr;
  final List<EnumValue> values;

  /** Creates a new enumeration definition.
   *  @param pt   Packet type.
   *  @param node Node from the XML.
   */
  public Enumeration (PacketType pt, Node node) {
    super();
    NodeList _values;
    packetType = pt;
    name       = evalXPath("./NAME",         node, String.class);
    title      = evalXPath("./TITLE",        node, String.class);
    descr      = evalXPath("./DESCRIPTION",  node, String.class);
    _values    = evalXPath("./VALUES/VALUE", node, NodeList.class);
    type       = "byte";
    values     = new LinkedList<EnumValue>();
    for (int i = 0; i < _values.getLength(); i++) {
      values.add(new EnumValue(this, _values.item(i)));
    }
  }

  @Override public String  toString ()       { return "/"+name+"/"; }
  @Override public boolean isArrayType ()    { return false; }
  @Override public boolean isAsciiType ()    { return false; }
  @Override public boolean isUTF8Type ()     { return false; }
  @Override public boolean isAsciiTypeStr () { return false; }
  @Override public boolean isUTF8TypeStr ()  { return false; }
  @Override public boolean isStringType ()   { return isAsciiType() || isUTF8Type(); }
  @Override public boolean isMetadataType () { return false; }
  @Override public boolean isNumericType ()  { return false; }
  @Override public String  getTypeName ()    { return "enumByte"; }
  @Override public int     getTypeBytes ()   { return 1; }
  @Override public String  getTypeDefault () { return "0"; }
  @Override public String  getTypeCPPGet ()  { return name; }
  @Override public String  getTypeCPPSet ()  { return name; }
  @Override public String  getTypeJavaGet () { return name; }
  @Override public String  getTypeJavaSet () { return name; }
  @Override public int     getArrayLength () { return -2; }
  @Override public Type    getArrayType ()   { return null; }

  public void writeHTML (PacketGen outParams, PrintStream html, PrintStream index, String fileName) {
    html.println("<center>");
    html.println("  <table>");
    html.println("    <tr>");
    html.println("      <th class='title' colspan='3'>" + title + "</th>");
    html.println("    </tr>");
    html.println("    <tr>");
    html.println("      <th>Name</th>");
    html.println("      <td colspan='2'>" + name + "</td>");
    html.println("    </tr>");
    html.println("    <tr>");
    html.println("      <th>Type</th>");
    html.println("      <td colspan='2'>byte</td>");
    html.println("    </tr>");
    html.println("    <tr>");
    html.println("      <th>Description</th>");
    html.println("      <td colspan='2'>" + descr + "</td>");
    html.println("    </tr>");
    html.println("    <tr>");
    html.println("      <th class='section' colspan='3'>Enumeration Values</th>");
    html.println("    </tr>");
    html.println("    <tr>");
    html.println("      <th class='col'>Value</th>");
    html.println("      <th class='col'>Name(s)</th>");
    html.println("      <th class='col'>Description</th>");
    html.println("    </tr>");
    for (EnumValue v : values) {
      StringBuilder names = new StringBuilder();
      names.append(v.name);
      if ((v.alias  != null) && (v.alias .length() > 0)) names.append("<br><i>(").append(v.alias ).append(")</i>");
      if ((v.alias2 != null) && (v.alias2.length() > 0)) names.append("<br><i>(").append(v.alias2).append(")</i>");
      if ((v.alias3 != null) && (v.alias3.length() > 0)) names.append("<br><i>(").append(v.alias3).append(")</i>");

      html.println("    <tr>");
      html.println("      <td style='text-align:right;' ><tt>" + v.value + "</tt></td>");
      html.println("      <td style='text-align:center;'><tt>" + names   + "</tt></td>");
      html.println("      <td style='text-align:left;'  >    " + v.descr + "     </td>");
      html.println("    </tr>");
    }
    html.println("  </table>");
    html.println("</center>");
    html.println("<br>");
  }

  public void writeCPP (PacketGen outParams, PrintStream hdr, PrintStream cpp) {
    EnumValue[] vals = values.toArray(new EnumValue[values.size()]);
    hdr.println("  /** " + toHtmlDoc(title,"   *  ") + " */");
    hdr.println(" enum " + name + " { ");
    boolean first = true;
    for (EnumValue v : vals) {
      if (!first) hdr.println(",");
      first = false;
      if ((v.descr != null) && (v.descr.length() > 0)) {
        hdr.println("    /** " + toHtmlDoc(v.descr,"     *  ") + " */");
      }
      hdr.print("    " + name + "_" + v.name + " = " + v.value);
    }
    hdr.println("");
    hdr.println("  };");

    cpp.println("string operator+(string &s, "+name+" val) {");
    cpp.println("  ostringstream str;");
    cpp.println("  str << s << val;");
    cpp.println("  return str.str();");
    cpp.println("}");
    cpp.println("");
    cpp.println("ostream& operator<<(ostream &s, "+name+" val) {");
    cpp.println("  switch (val) {");
    for (EnumValue v : vals) {
      cpp.println("    case "+v.value+": return s << \""+v.name+"\";");
    }
    cpp.println("    default: return s << \"Unknown "+name+" \" << (int32_t)val;");
    cpp.println("  }");
    cpp.println("}");
  }

  public void writeCPP2 (PacketGen outParams, PrintStream hdr, PrintStream cpp) {
    hdr.println("string   operator+ (string  &s, "+name+" val);");
    hdr.println("ostream& operator<<(ostream &s, "+name+" val);");
  }

  public void writeJava (PacketGen outParams, PrintStream out) {
    EnumValue skip = null;
    EnumValue[] vals = values.toArray(new EnumValue[values.size()]);
    out.println("  /** " + toHtmlDoc(title,"   *  ") + " */");
    out.println("  public static enum " + name + " { ");
    boolean first = true;
    for (EnumValue v : vals) {
      if (v.name.equals("null")) {
        skip = v;
        continue;
      }
      if (!first) out.println(",");
      first = false;
      if ((v.descr != null) && (v.descr.length() > 0)) {
        out.println("    /** " + toHtmlDoc(v.descr,"     *  ") + " */");
      }
      out.print("    " + v.name);
    }
    out.println(";");
    out.println("    ");
    out.println("    /** Gets the value stored in the packet for this enum element. */");
    out.println("    public byte getValue() { return getValue(this); }");
    out.println("    ");
    out.println("    /** Gets the value stored in the packet for this enum element.");
    out.println("     *  @param e The element to get the value of.");
    out.println("     *  @return The value of <tt>e</tt> as stored within the packet.");
    if (skip == null) {
      out.println("     *  @throws java.lang.NullPointerException if <tt>e</tt> is null.");
    }
    out.println("     */");
    out.println("    public static byte getValue (" + name + " e) {");
    if (skip != null) {
      out.println("      if (e == null) return (byte)"+skip.value+";");
    }
    else {
      out.println("      if (e == null) throw new java.lang.NullPointerException(\"null is not supported within "+name+"\");");
    }
    out.println("      switch(e) {");
    for (EnumValue v : vals) {
      if (v.name.equals("null")) continue;
      out.println("        case "+v.name+": return (byte)"+v.value+";");
    }
    out.println("        default: throw new java.lang.AssertionError(\"Error in "+name+".getValue("+name+"): Invalid element found \"+e);");
    out.println("      }");
    out.println("    }");
    out.println("    ");
    out.println("    /** Gets the enum element corresponding to the given packet value. */");
    out.println("    public static " + name + " forValue (byte val) {");
    out.println("      switch (val) {");
    for (EnumValue v : vals) {
      out.println("        case (byte)" + v.value + ": return " + v.name + ";");
    }
    out.println("        default: throw new IllegalArgumentException(\"Value \"+val+\" is not member of \"+" + name + ".class);");
    out.println("      }");
    out.println("    }");
    out.println("    ");
    out.println("    /** Gets the enum element corresponding to the name. Returns null if ");
    out.println("     *  passed null or the empty string.");
    out.println("     */");
    out.println("    public static " + name + " forName (java.lang.CharSequence val) {");
    out.println("      if ((val == null) || (val.length() == 0)) return null;");
    for (EnumValue v : vals) {
      if (v.name.equals("null")) {
        continue;
      }
      out.println("      if (\"" + v.name + "\".contentEquals(val)) return " + v.name + ";");
      if ((v.alias != null) && (v.alias.length() > 0)) {
        out.println("      if (\"" + v.alias + "\".contentEquals(val)) return " + v.name + "; // alias for " + v.name);
      }
      if ((v.alias2 != null) && (v.alias2.length() > 0)) {
        out.println("      if (\"" + v.alias2 + "\".contentEquals(val)) return " + v.name + "; // alias for " + v.name);
      }
      if ((v.alias3 != null) && (v.alias3.length() > 0)) {
        out.println("      if (\"" + v.alias3 + "\".contentEquals(val)) return " + v.name + "; // alias for " + v.name);
      }
    }
    out.println("      throw new IllegalArgumentException(\"Value \"+val+\" is not member of \"+" + name + ".class);");
    out.println("    }");
    out.println("  }");
    out.println("  ");
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////
  //  Nested Classes
  //////////////////////////////////////////////////////////////////////////////////////////////////
  
  /** A enumeration value. */
  static class EnumValue {
    final Enumeration enumeration;
    final String      value;
    final String      alias;
    final String      alias2;
    final String      alias3;
    final String      name;
    final String      descr;

    public EnumValue (Enumeration e, Node node) {
      enumeration = e;
      value       = evalXPath("./@VAL",         node, "");
      name        = evalXPath("./@NAME",        node, "");
      alias       = evalXPath("./@ALIAS",       node, "");
      alias2      = evalXPath("./@ALIAS2",      node, "");
      alias3      = evalXPath("./@ALIAS3",      node, "");
      descr       = evalXPath("./@DESCRIPTION", node, "");
    }
  }
}

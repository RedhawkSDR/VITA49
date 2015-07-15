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
import java.util.LinkedHashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

import static nxm.vrt.lib.XmlUtilities.evalXPath;
import static nxm.vrt.libg.PacketGen.toHtmlDoc;
import static nxm.vrt.libg.PacketGen.toHtmlTitle;

/** A record definition used by a packet type.
 *
 *  @author 
 */
class Record extends Definition implements Type {
  final PacketType         packetType;
  final String             parent;
  final String             name;
  final String             title;
  final String             sect;
  final String             descr;
  final String             author;
        int                length;
  final List<Field>        fields   = new LinkedList<Field>();
  final Map<String, Field> fieldMap = new LinkedHashMap<String, Field>();

  /** Creates a new record definition.
   *  @param pt   Packet type.
   *  @param node Node from the XML.
   */
  public Record (PacketType pt, Node node) {
    super();
    this.parent     = null;
    this.packetType = pt;
    this.name       = evalXPath("./NAME",           node, String.class);
    this.title      = evalXPath("./TITLE",          node, name);
    this.descr      = evalXPath("./DESCRIPTION",    node, "");
    this.author     = evalXPath("./AUTHOR",         node, pt.author);
    this.sect       = title;
    this.length     = readFields(0, evalXPath("./FIELDS/FIELD", node, NodeList.class));
  }

  /** Constructor used by PacketType.
   *  @param node Node from the XML.
   */
  protected Record (Node node) {
    super();
    this.packetType = (PacketType)this;
    this.parent     = evalXPath("./EXTENDS",        node, String.class);
    this.name       = evalXPath("./NAME",           node, String.class);
    this.title      = evalXPath("./TABLE_TITLE",    node, "");
    this.sect       = evalXPath("./SECTION_TITLE",  node, "");
    this.descr      = evalXPath("./DESCRIPTION",    node, "");
    this.author     = evalXPath("./AUTHOR",         node, "");
    this.length     = -999;
  }

  @Override
  public String toString () {
    return "/"+name+"/";
  }

  /** Reads fields from a list of 'FIELD' entries under a 'FIELDS' tag. */
  @SuppressWarnings("UseOfSystemOutOrSystemErr")
  final int readFields (int offset, NodeList nodeList) {
    if (nodeList != null) {
      for (int i = 0; i < nodeList.getLength(); i++) {
        Field f = new Field(this, nodeList.item(i), offset);
        if (offset >= 0) {
          int bytes = f.getType().getTypeBytes();
          if (bytes < 0) {
            offset = -offset;
          }
          else {
            offset = offset + bytes;
            int b = Math.min(8, bytes);
            if ((f.name.length() > 0) && ((offset % b) != 0)) {
              System.err.println(f.name + " in " + name + " is not " + b + "-byte aligned");
            }
          }
        }
        else {
          System.err.println(f.name + " found in " + name + " after [*] entry which extends to the end of the packet");
        }
        fields.add(f);
        fieldMap.put(f.name, f);
      }
    }
    if ((offset > 0) && ((offset % 4) != 0)) {
      System.err.println("Payload length for " + name + " is not a 4-byte multiple");
    }
    return offset;
  }

  @Override public boolean isArrayType ()     { return false; }
  @Override public boolean isAsciiType ()     { return false; }
  @Override public boolean isUTF8Type ()      { return false; }
  @Override public boolean isAsciiTypeStr ()  { return false; }
  @Override public boolean isUTF8TypeStr ()   { return false; }
  @Override public boolean isStringType ()    { return false; }
  @Override public boolean isMetadataType ()  { return false; }
  @Override public boolean isNumericType ()   { return false; }
  @Override public String  getTypeName ()     { return "byte[" + length + "]"; }
  @Override public int     getTypeBytes ()    { return length; }
  @Override public String  getTypeDefault ()  { return "0"; }
  @Override public String  getTypeCPPGet ()   { return name; }
  @Override public String  getTypeCPPSet ()   { return "const "+name+" &"; }
  @Override public String  getTypeJavaGet ()  { return name; }
  @Override public String  getTypeJavaSet ()  { return name; }
  @Override public int     getArrayLength ()  { return -2; }
  @Override public Type    getArrayType ()    { return null; }

  @Override
  public void writeHTML (PacketGen outParams, PrintStream html, PrintStream index, String fileName) {
    String len = (length >= 0)? ""+length : ""+(-length)+"+";

    html.println("<center>");
    html.println("  <table>");
    html.println("    <tr>");
    html.println("      <th class='title' colspan='6'>" + title + "</th>");
    html.println("    </tr>");
    html.println("    <tr>");
    html.println("      <th colspan='2'>Name</th>");
    html.println("      <td colspan='4'>" + name + "</td>");
    html.println("    </tr>");
    html.println("    <tr>");
    html.println("      <th colspan='2'>Description</th>");
    html.println("      <td colspan='4'>" + descr + "</td>");
    html.println("    </tr>");
    html.println("    <tr>");
    html.println("      <th class='section' colspan='6'>Record Fields</th>");
    html.println("    </tr>");
    html.println("    <tr>");
    html.println("      <td class='section' colspan='6'><center>Total Payload Length = "+len+" bytes</center></td>");
    html.println("    </tr>");
    html.println("    <tr>");
    html.println("      <th class='col'>Offset</th>");
    html.println("      <th class='col'>Type</th>");
    html.println("      <th class='col'>Name</th>");
    html.println("      <th class='col'>Units</th>");
    html.println("      <th class='col'>Range</th>");
    html.println("      <th class='col'>Description</th>");
    html.println("    </tr>");
      for (Field f : fields) {
        f.writeHTML(outParams, html, index, fileName);
      }
    html.println("  </table>");
    html.println("</center>");
    html.println("<br>");
  }

  @Override
  public void writeCPP (PacketGen outParams, PrintStream hdr, PrintStream cpp) {
    hdr.println("    ");
    hdr.println("    ");
    hdr.println("  /** " + toHtmlTitle(sect,"  ",false) + " <br>");
    hdr.println("   *  <br>");
    if (!descr.trim().isEmpty()) {
      hdr.println("   *  " + toHtmlDoc(descr,"   *  ") + " <br>");
      hdr.println("   *  <br>");
    }
    hdr.println("   *  <i>Note: Any changes made to this record will not be reflected in the");
    hdr.println("   *  packet. This maintains the CONOP that anything read from the packet can be");
    hdr.println("   *  used independent of the packet and that the packet can only be modified");
    hdr.println("   *  by calling a set method on the packet its self.</i>");
    if (!author.trim().isEmpty()) {
      hdr.println("   *  ");
      hdr.println("   *  @author " + author);
    }
    hdr.println("   */");
    hdr.println("  class " + name + " : public Record {");
    hdr.println("    ");
    hdr.println("    ");
    hdr.println("    /** The length of the record in octets (<tt>"+length+"</tt>). */");
    hdr.println("    public: static const int32_t RECORD_LENGTH;");
    hdr.println("    ");
    hdr.println("    /** Basic copy constructor. */");
    hdr.println("    protected: " + name + " (const Record &r);");
    hdr.println("    ");
    hdr.println("    /** Creates a new instance ready for the fields to be set. All of the bytes");
    hdr.println("     *  that make up the record will be initialized to 0.");
    hdr.println("     */");
    hdr.println("    public: " + name + " ();");
    hdr.println("    ");
    hdr.println("    public: virtual string    toString () const;");
    hdr.println("    public: virtual int32_t   getFieldCount () const;");
    hdr.println("    public: virtual string    getFieldName (int32_t id) const;");
    hdr.println("    public: virtual ValueType getFieldType (int32_t id) const;");
    hdr.println("    public: virtual Value*    getField (int32_t id) const;");
    hdr.println("    public: virtual void      setField (int32_t id, const Value* val);");
    hdr.println("    ");
    for (Field f : fields) {
      if (f.methodName != null) {
        f.writeCPP(outParams, hdr, cpp, "  ");
      }
    }
    hdr.println("  };");
    
    
    cpp.println("");
    cpp.println("const int32_t "+name+"::RECORD_LENGTH = "+length+";");
    cpp.println("");
    cpp.println(name + "::" + name + " () : Record(" + length + ") { }");
    cpp.println(name + "::" + name + " (const Record &r) : Record(r) { }");
    cpp.println("");
    cpp.println("string " + name + "::toString () const {");
    cpp.println("  ostringstream str;");
    cpp.println("  str << Record::toString();");
    boolean first = true;
    for (Field f : fields) {
      if (f.methodName != null) {
        String sp = (first)? "" : " ";
        if (f.getType().isStringType()) {
          cpp.println("  str << \"" + sp + f.name + "=\'\" << get" + f.methodName + "() << \"\'\";");
        }
        else {
          cpp.println("  str << \"" + sp + f.name + "=\" << get" + f.methodName + "();");
        }
        first = false;
      }
    }
    cpp.println("  return str.str();");
    cpp.println("}");
    
    cpp.println("");
    cpp.println("int32_t "+name+"::getFieldCount () const {");
    cpp.println("  return Record::getFieldCount() + "+fields.size()+";");
    cpp.println("}");
    cpp.println("");
    
    cpp.println("string "+name+"::getFieldName (int32_t id) const {");
    cpp.println("  switch (id - Record::getFieldCount()) {");
    for (int i = 0; i < fields.size(); i++) {
      cpp.println("    case "+i+": return \""+fields.get(i).name+"\";");
    }
    cpp.println("    default: return Record::getFieldName(id);");
    cpp.println("  }");
    cpp.println("}");
    cpp.println("");
    
    cpp.println("ValueType "+name+"::getFieldType (int32_t id) const {");
    cpp.println("  switch (id - Record::getFieldCount()) {");
    for (int i = 0; i < fields.size(); i++) {
      cpp.println("    case "+i+": return "+fields.get(i).getCPPFieldType()+";");
    }
    cpp.println("    default: return Record::getFieldType(id);");
    cpp.println("  }");
    cpp.println("}");
    cpp.println("");
    
    cpp.println("Value* "+name+"::getField (int32_t id) const {");
    cpp.println("  switch (id - Record::getFieldCount()) {");
    for (int i = 0; i < fields.size(); i++) {
      cpp.println("    case "+i+": return "+fields.get(i).getCPPGetField()+";");
    }
    cpp.println("    default: return Record::getField(id);");
    cpp.println("  }");
    cpp.println("}");
    cpp.println("");
    
    cpp.println("void "+name+"::setField (int32_t id, const Value* val) {");
    cpp.println("  switch (id - Record::getFieldCount()) {");
    for (int i = 0; i < fields.size(); i++) {
      cpp.println("    case "+i+": "+fields.get(i).getCPPSetField()+"; break;");
    }
    cpp.println("    default: Record::setField(id,val); break;");
    cpp.println("  }");
    cpp.println("}");
    cpp.println("");
  }

  @Override
  public void writeJava (PacketGen outParams, PrintStream out) {
    out.println("  /** " + toHtmlTitle(sect,"  ",true) + " <br>");
    out.println("   *  <br>");
    if (!descr.trim().isEmpty()) {
      out.println("   *  " + toHtmlDoc(descr,"   *  ") + " <br>");
      out.println("   *  <br>");
    }
    out.println("   *  <i>Note: Any changes made to this record will not be reflected in the");
    out.println("   *  packet. This maintains the CONOP that anything read from the packet can be");
    out.println("   *  used independent of the packet and that the packet can only be modified");
    out.println("   *  by calling a set method on the packet its self.</i>");
    if (!author.trim().isEmpty()) {
      out.println("   *  ");
      out.println("   *  @author " + author);
    }
    out.println("   */");
    out.println("  public static final class " + name + " extends nxm.vrt.lib.Record {");
    out.println("    ");
    out.println("    /** The length of the record in octets (<tt>"+length+"</tt>). */");
    out.println("    public static final int RECORD_LENGTH = "+length+";");
    out.println("    ");
    out.println("    /** Creates a new instance sharing the buffers of the one specified. */");
    out.println("    protected " + name + " (nxm.vrt.lib.Record r) { super(r); }");
    out.println("    ");
    out.println("    /** Creates a new instance ready for the fields to be set. All of the bytes");
    out.println("     *  that make up the record will be initialized to 0.");
    out.println("     */");
    out.println("    public " + name + " () { super(" + length + "); }");
    out.println("    ");
    out.println("    @Override");
    out.println("    public " + name + " copy () { return new " + name + "(super.copy()); }");
    out.println("    ");
    out.println("    @Override");
    out.println("    protected java.lang.StringBuilder toStringBuilder () {");
    out.println("      java.lang.StringBuilder str = super.toStringBuilder();");
    boolean first = true;
    for (Field f : fields) {
      if (f.methodName != null) {
        String sp = (first)? "" : " ";
        if (f.getType().isStringType()) {
          out.println("      str.append(\"" + sp + f.name + "=\'\").append(get" + f.methodName + "()).append(\"\'\");");
        }
        else {
          out.println("      str.append(\"" + sp + f.name + "=\").append(get" + f.methodName + "());");
        }
        first = false;
      }
    }
    out.println("      return str;");
    out.println("    }");
    out.println("    ");
    for (Field f : fields) {
      if (f.methodName != null) {
        f.writeJava(outParams, out, "  ");
      }
    }
    
    out.println("");
    out.println("    @Override");
    out.println("    public int getFieldCount () {");
    out.println("      return super.getFieldCount() + "+fields.size()+";");
    out.println("    }");
    out.println("");
    
    out.println("    @Override");
    out.println("    public String getFieldName (int id) {");
    out.println("      switch (id - super.getFieldCount()) {");
    for (int i = 0; i < fields.size(); i++) {
      out.println("        case "+i+": return \""+fields.get(i).name+"\";");
    }
    out.println("        default: return super.getFieldName(id);");
    out.println("      }");
    out.println("    }");
    out.println("");
    
    out.println("    @Override");
    out.println("    public Class<?> getFieldType (int id) {");
    out.println("      switch (id - super.getFieldCount()) {");
    for (int i = 0; i < fields.size(); i++) {
      out.println("        case "+i+": return "+fields.get(i).getJavaFieldType()+";");
    }
    out.println("        default: return super.getFieldType(id);");
    out.println("      }");
    out.println("    }");
    out.println("");
    
    out.println("    @Override");
    out.println("    public Object getField (int id) {");
    out.println("      switch (id - super.getFieldCount()) {");
    for (int i = 0; i < fields.size(); i++) {
      out.println("        case "+i+": return "+fields.get(i).getJavaGetField()+";");
    }
    out.println("        default: return super.getField(id);");
    out.println("      }");
    out.println("    }");
    out.println("");
    
    out.println("    @Override");
    out.println("    public void setField (int id, Object val) {");
    out.println("      switch (id - super.getFieldCount()) {");
    for (int i = 0; i < fields.size(); i++) {
      out.println("        case "+i+": "+fields.get(i).getJavaSetField()+"; break;");
    }
    out.println("        default: super.setField(id,val); break;");
    out.println("      }");
    out.println("    }");
    out.println("  }");
    out.println("");
  }
}

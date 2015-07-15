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
import org.w3c.dom.Node;

import static nxm.vrt.lib.XmlUtilities.evalXPath;
import static nxm.vrt.libg.Type.BYTE;
import static nxm.vrt.libg.Type.SHORT;
import static nxm.vrt.libg.Type.INT;
import static nxm.vrt.libg.Type.LONG;
import static nxm.vrt.libg.Type.FLOAT;
import static nxm.vrt.libg.Type.DOUBLE;
import static nxm.vrt.libg.Type.BOOLEAN;
import static nxm.vrt.libg.Type.BOOLNULL;
import static nxm.vrt.libg.Type.INETADDR;
import static nxm.vrt.libg.Type.UUID;
import static nxm.vrt.libg.Type.UTCTIME;
import static nxm.vrt.libg.Type.GPSTIME;
import static nxm.vrt.libg.PacketGen.toHtmlDoc;
import static nxm.vrt.libg.PacketGen.notPlural;

/** A field in the packet definition.
 *
 *  @author 
 */
class Field extends Definition {
  final Record     record;
  final PacketType packetType;
  final String     type;
  final int        offset;
  final String     name;
  final String     methodName;
  final String     units;
  final String     range;
  final String     descr;

  public Field (Record r, Node node, int off) {
    super();
    record     = r;
    packetType = r.packetType;
    type       = evalXPath("./@TYPE",        node, "");
    name       = evalXPath("./@NAME",        node, "");
    units      = evalXPath("./@UNITS",       node, "");
    range      = evalXPath("./@RANGE",       node, "");
    descr      = evalXPath("./@DESCRIPTION", node, "");
    offset     = off;
    methodName = (name.length() == 0) ? null : name.substring(0,1).toUpperCase() + name.substring(1);
  }

  @Override
  public String toString () {
    return "Field: name='"+name+"'";
  }

  public Type getType () {
    return packetType.getType(type);
  }

  private CodeParams getCodeParams (boolean isJava) {
    // ignore the pad bytes
    Type t = getType();
    int  b = getType().getTypeBytes();
    String lenGet;
    String lenSet;
    String limit;
    String size;
    if (b >= 0) {
      lenGet = Integer.toString(b);
      lenSet = Integer.toString(b);
      limit  = null;
      size   = Integer.toString(b);
    }
    else if (b != Type.STAR_LENGTH) {
      if (t.isStringType()) {
        lenGet = "getPayloadLength()-" + offset;
        lenSet = Integer.toString(b);
        limit  = null;
        size   = "0 to "+(-b-1)+" octets";
      }
      else {
        throw new RuntimeException("The array syntax [0"+b+"] is only applicable to ascii and utf8 fields.");
      }
    }
    else if (t.isAsciiType() || t.isAsciiTypeStr()) {
      lenGet = "getPayloadLength()-" + offset;
      lenSet = "val.length()";
      limit  = "MAX_PAYLOAD_LENGTH-"+offset;
      size   = "variable";
    }
    else if (t.isStringType()) {
      lenGet = "getPayloadLength()-" + offset;
      lenSet = (isJava)? "nxm.vrt.lib.VRTMath.lengthUTF8(val)"
                       : "vrt::VRTMath::lengthUTF8(val)";
      limit  = "MAX_PAYLOAD_LENGTH-"+offset;
      size   = "variable";
    }
    else if (t.isMetadataType()) {
      lenGet = "getPayloadLength()-" + offset;
      lenSet = "val.getLengthInBytes()";
      limit  = null;
      size   = "variable";
    }
    else if (t.isArrayType()) {
      int bytes = t.getArrayType().getTypeBytes();
      if (bytes == 0) {
        lenGet = "getPayloadLength()-" + offset;
        lenSet = "val.length";
        limit  = null;
        size   = "variable";
      }
      else if (bytes > 0) {
        lenGet = "(getPayloadLength()-" + offset + ")/" + bytes;
        lenSet = "val.length";
        limit  = null;
        size   = "variable";
      }
      else {
        throw new RuntimeException("Could not determine length of " + t.getTypeName()+" for "+this);
      }
    }
    else {
      throw new RuntimeException("Could not determine length of " + t.getTypeName()+" for "+this);
    }

    String html = toHtmlDoc(descr,"").trim();
    if (html.length() == 0) {
      html = methodName;
    }
    if (!html.endsWith(".")) {
      html += ".";
    }
    if (Character.isUpperCase(html.charAt(0)) && Character.isLowerCase(html.charAt(1))) {
      html = html.substring(0, 1).toLowerCase() + html.substring(1);
    }
    return new CodeParams(html, size, limit, lenGet, lenSet);
  }

//  private static String toTypeString (Type t) {
//    if (t.isArrayType()) {
//      return t.getTypeName() + "[" + t.
//
//
//    }
//    else {
//
//    }
//  }
  

  @Override
  public void writeHTML (PacketGen outParams, PrintStream html, PrintStream index, String fileName) {
    String typeStr = type.toString();
    
    if (name.length() > 0) {
      html.println("        <tr>");
      html.println("          <td style='text-align:right;' ><tt>" + offset    + "</tt></td>");
      html.println("          <td style='text-align:center;'><tt>" + typeStr   + "</tt></td>");
      html.println("          <td style='text-align:center;'><b>"  + name      +  "</b></td>");
      html.println("          <td style='text-align:center;'>"     + units     +      "</td>");
      html.println("          <td style='text-align:center;'><tt>" + range     + "</tt></td>");
      html.println("          <td style='text-align:left;'  >"     + descr     +      "</td>");
      html.println("        </tr>");
    }
    else {
      html.println("        <tr>");
      html.println("          <td style='text-align:right;' ><tt>" + offset    + "</tt></td>");
      html.println("          <td style='text-align:center;'><tt>" + typeStr   + "</tt></td>");
      html.println("          <td style='text-align:center;'><i>     reserved      </i></td>");
      html.println("          <td style='text-align:center;'>                          </td>");
      html.println("          <td style='text-align:center;'><tt>                 </tt></td>");
      html.println("          <td style='text-align:left;'  ><i>Reserved for future use, must be zero.</i></td>");
      html.println("        </tr>");
    }
  }

  @Override
  public void writeCPP (PacketGen outParams, PrintStream hdr, PrintStream cpp) {
    writeCPP(outParams, hdr, cpp, "");
  }

  public void writeCPP (PacketGen outParams, PrintStream hdr, PrintStream cpp, String indent) {
    if (methodName == null) return;
    CodeParams cp = getCodeParams(false);
    writeCPPGet(hdr, cpp, "public", cp.html, cp.size, cp.limit, cp.lenGet, indent);
    writeCPPSet(hdr, cpp, "public", cp.html, cp.size, cp.limit, cp.lenSet, indent);
  }

  @Override
  public void writeJava (PacketGen outParams, PrintStream out) {
    writeJava(outParams, out, "");
  }

  public void writeJava (PacketGen outParams, PrintStream out, String indent) {
    if (methodName == null) return;
    CodeParams cp = getCodeParams(true);
    writeJavaGet(out, "public", cp.html, cp.size, cp.limit, cp.lenGet, indent);
    writeJavaSet(out, "public", cp.html, cp.size, cp.limit, cp.lenSet, indent);
  }


  /** Write the get method. */
  private void writeCPPGet (PrintStream hdr, PrintStream cpp, String permission, String html, String size, String limit, String len, String indent) {
    html = html.replace("\n",    "\n     *  ");
    Type t = getType();
    // === DOCS ===
    hdr.println(indent + "    ");
    hdr.println(indent + "    /** Get " + html + " ");
    if ((units != null) && (units.length() > 0)) {
      hdr.println(indent + "     *  <br> Units: <i>" + units + "</i>");
    }
    if ((range != null) && (range.length() > 0)) {
      hdr.println(indent + "     *  <br> Range: <tt>" + range + "</tt>");
    }
    if (t.isStringType() || t.isArrayType()) {
      hdr.println(indent + "     *  <br> Size: <tt>" + size + "</tt>");
    }
    hdr.println(indent + "     *  @return The value read from the packet.");
    hdr.println(indent + "     */");
    // === HEADER ===
    if (!t.isArrayType()) {
      hdr.println(indent + "    " + permission + ": inline " + t.getTypeCPPGet() + " get" + methodName + " () const {");
      if (t instanceof Record) {
        hdr.println(indent + "      " + t.getTypeJavaGet() + " r;");
        hdr.println(indent + "      " + getUnpackFuncCPP(t, offset, len)+";");
        hdr.println(indent + "      return r;");
      }
      else {
        hdr.println(indent + "      return " + getUnpackFuncCPP(t, offset, len) + ";");
      }
      hdr.println(indent + "    }");
    }
    else {
      Type   arrayType  = t.getArrayType();
      int    count      = t.getArrayLength();
      int    bytes      = arrayType.getTypeBytes();
      String singleName = notPlural(methodName);
      String getCount   = "get" + singleName + "Count()";

      hdr.println(indent + "    " + permission + ": vector<" + arrayType.getTypeCPPGet() + "> getAll" + methodName + " () const;");
      hdr.println(indent + "    ");
      hdr.println(indent + "    /** Get method used by the HasFields functions. */");
      if (!isCPPNativeType(t)) {
        hdr.println(indent + "    private: vector<VRTObject*>* _getAll" + methodName + " () const;");
      }
      else {
        hdr.println(indent + "    private: vector<" + arrayType.getTypeCPPGet() + ">* _getAll" + methodName + " () const;");
      }
      hdr.println(indent + "    ");
      hdr.println(indent + "    /** Get item in " + html + " ");
      hdr.println(indent + "     *  @param i The index.");
      hdr.println(indent + "     *  @return The value read from the packet.");
      hdr.println(indent + "     *  @throws VRTException if <tt>i</tt> is greater than or equal to " + getCount + "");
      hdr.println(indent + "     */");
      hdr.println(indent + "    " + permission + ": " + arrayType.getTypeCPPGet() + " get" + singleName + " (int32_t i) const;");
      hdr.println(indent + "    ");
      hdr.println(indent + "    /** Get length of " + html + " ");
      if (count >= 0) {
        hdr.println(indent + "     *  @return The number of entries, which is always " + count + ".");
      }
      else {
        hdr.println(indent + "     *  @return The number of entries.");
      }
      hdr.println(indent + "     */");
      hdr.println(indent + "    " + permission + ": inline int32_t get" + singleName + "Count () const {");
      if (count >= 0) {
        hdr.println(indent + "      return " + len + ";");
      }
      else {
        hdr.println(indent + "      return (getPayloadLength()-" + offset + ")/" + bytes + ";");
      }
      hdr.println(indent + "    }");

      // === CODE ===
      cpp.println(indent + "vector<" + arrayType.getTypeCPPGet() + "> "+record.name+"::getAll" + methodName + " () const {");
      if (t.getArrayType() == BYTE) {
        cpp.println(indent + "  // Use fast method for unpacking an array of bytes");
        cpp.println(indent + "  return unpackPayloadBytes(" + offset + ", " + getCount + ");");
      }
      else {
        cpp.println(indent + "  int32_t len = " + getCount + ";");
        cpp.println(indent + "  vector<" + arrayType.getTypeCPPGet() + "> all(len);");
        cpp.println(indent + "  for (int32_t i = 0; i < len; i++) {");
        cpp.println(indent + "    all[i] = get" + singleName + "(i);");
        cpp.println(indent + "  }");
        cpp.println(indent + "  return all;");
      }
      cpp.println(indent + "}");
      cpp.println(indent + "");
      cpp.println(indent + "" + arrayType.getTypeCPPGet() + " " + record.name +"::get" + singleName + " (int32_t i) const {");
      cpp.println(indent + "  if ((i < 0) || (i > " + getCount + ")) {");
      cpp.println(indent + "    throw VRTException(\"Invalid index %d into "+name+"\", i);");
      cpp.println(indent + "  }");
      cpp.println(indent + "  int32_t off = " + offset + " + (" + bytes + " * i);");
      if (arrayType instanceof Record) {
        cpp.println(indent + "  " + arrayType.getTypeJavaGet() + " r;");
        cpp.println(indent + "  " + getUnpackFuncCPP(arrayType, "off", bytes)+";");
        cpp.println(indent + "  return r;");
      }
      else {
        cpp.println(indent + "  return " + getUnpackFuncCPP(arrayType, "off", bytes) + ";");
      }
      cpp.println(indent + "}");
      cpp.println(indent + "");
      
      if (!isCPPNativeType(t)) {
        cpp.println(indent + "vector<VRTObject*>* "+record.name+"::_getAll" + methodName + " () const {");
        cpp.println(indent + "  int32_t len = " + getCount + ";");
        cpp.println(indent + "  vector<VRTObject*>* all = new vector<VRTObject*>;");
        cpp.println(indent + "  for (int32_t i = 0; i < len; i++) {");
        cpp.println(indent + "    all->push_back(new "+arrayType.getTypeCPPGet()+"(get" + singleName + "(i)));");
        cpp.println(indent + "  }");
        cpp.println(indent + "  return all;");
        cpp.println(indent + "}");
        cpp.println(indent + "");
      }
      else {
        cpp.println(indent + "vector<"+arrayType.getTypeCPPGet()+">* "+record.name+"::_getAll" + methodName + " () const {");
        cpp.println(indent + "  int32_t len = " + getCount + ";");
        cpp.println(indent + "  vector<"+arrayType.getTypeCPPGet()+">* all = new vector<"+arrayType.getTypeCPPGet()+">;");
        cpp.println(indent + "  for (int32_t i = 0; i < len; i++) {");
        cpp.println(indent + "    all.push_back(get" + singleName + "(i));");
        cpp.println(indent + "  }");
        cpp.println(indent + "  return all;");
        cpp.println(indent + "}");
        cpp.println(indent + "");
      }
    }
  }

  /** Write the set method. */
  private void writeCPPSet (PrintStream hdr, PrintStream cpp, String permission, String html,
                            String size, String limit, String len, String indent) {
    Type t = getType();
    writeCPPSet(hdr, cpp, permission, html, size, limit, len, indent, t);

    if (t instanceof Type.UTF8Type) {
      Type s = ((Type.UTF8Type)t).asString();
      writeCPPSet(hdr, cpp, permission, html, size, limit, len, indent, s);
    }
  }

  /** Write the set method. */
  private void writeCPPSet (PrintStream hdr, PrintStream cpp, String permission, String html,
                            String size, String limit, String len, String indent, Type t) {
    html = html.replace("\n",    "\n     *  ");
    int b = t.getTypeBytes();
    // === DOCS ===
    hdr.println(indent + "    ");
    hdr.println(indent + "    /** Set " + html + " ");
    if ((units != null) && (units.length() > 0)) {
      hdr.println(indent + "     *  <br> Units: <i>" + units + "</i>");
    }
    if ((range != null) && (range.length() > 0)) {
      hdr.println(indent + "     *  <br> Range: <tt>" + range + "</tt>");
    }
    if (t.isArrayType() || t.isStringType()) {
      hdr.println(indent + "     *  <br> Size: <tt>" + size + "</tt>");
    }
    if (t.isStringType()) {
      hdr.println(indent + "     *  @param val The new value to write to the packet.");
      if (t.isAsciiType()) {
        hdr.println(indent + "     *             Any non-ASCII characters in the string (numeric value &gt;0x7F) or ASCII");
        hdr.println(indent + "     *             control characters (numeric value &lt;0x20, 0x7F) will be replaced with \'?\'.");
      }
      if (b >= 0) {
        hdr.println(indent + "     *             If the string is longer than the size allotted, it will be");
        hdr.println(indent + "     *             truncated without warning. ");
      }
    }
    else {
      hdr.println(indent + "     *  @param val The new value to write to the packet.");
    }
    if (t.isArrayType()) {
      hdr.println(indent + "     *  @throws VRTException If the length of <tt>val</tt> is invalid");
    }
    hdr.println(indent + "     */");
    // === CODE ===
    if (!t.isArrayType()) {
      hdr.println(indent + "    " + permission + ": inline void set" + methodName + " (" + t.getTypeCPPSet() + " val) {");
      if (b < 0) {
        if (limit != null) {
          hdr.println(indent + "    int32_t limit  = " + limit + ";");
          hdr.println(indent + "    int32_t length = " + len + ";");
          hdr.println(indent + "    int32_t len    = (length > limit)? ((limit  + 3) & 0x7FFFFFFC)");
          hdr.println(indent + "                                     : ((length + 3) & 0x7FFFFFFC); // length to next multiple of 4");
          hdr.println(indent + "      setPayloadLength(" + offset + " + len);");
        }
        else {
          hdr.println(indent + "      int32_t len = (" + len + " + 3) & 0x7FFFFFFC; // length to next multiple of 4");
          hdr.println(indent + "      setPayloadLength(" + offset + " + len);");
        }
        len = "len";
      }
      hdr.println(indent + "      " + getPackFuncCPP(t, offset, len) + ";");
      hdr.println(indent + "    }");
    }
    else {
      Type   arrayType  = t.getArrayType();
      int    count      = t.getArrayLength();
      int    bytes      = arrayType.getTypeBytes();
      String singleName = notPlural(methodName);
      String getCount   = "get" + singleName + "Count()";

      hdr.println(indent + "    " + permission + ": void setAll" + methodName + " (const vector<" + arrayType.getTypeCPPGet() + "> &val);");
      hdr.println(indent + "    ");
      hdr.println(indent + "    /** Set method used by the HasFields functions. */");
      hdr.println(indent + "    private: void _setAll" + methodName + " (const Value *val);");
      hdr.println(indent + "    ");
      hdr.println(indent + "    /** Set item in " + html + " ");
      hdr.println(indent + "     *  @param i   The index.");
      hdr.println(indent + "     *  @param val The new value.");
      hdr.println(indent + "     *  @throws VRTException if <tt>i</tt> is greater than or equal to " + getCount + "");
      hdr.println(indent + "     */");
      hdr.println(indent + "    " + permission + ": void set" + singleName + " (int32_t i, " + arrayType.getTypeCPPSet() + " val);");
      if (count < 0) {
        hdr.println(indent + "    ");
        hdr.println(indent + "    /** Set length of " + html + " ");
        hdr.println(indent + "     *  @param length The number of entries.");
        hdr.println(indent + "     *  @throws VRTException if <tt>length</tt> is negative");
        hdr.println(indent + "     */");
        hdr.println(indent + "    " + permission + ": void set" + singleName + "Count (int32_t length);");
      }
      
      cpp.println(indent + "void "+record.name+"::setAll" + methodName + " (const vector<" + arrayType.getTypeCPPGet() + "> &val) {");
      if (t.getArrayType() == BYTE) {
        cpp.println(indent + "  // Use fast method for packing an array of bytes");
        cpp.println(indent + "  packPayloadBytes(" + offset + ", val, " + getCount + ");");
      }
      else {
        if (count >= 0) {
          cpp.println(indent + "  if (val.size() != " + b + ") {");
          cpp.println(indent + "    throw VRTException(\"Invalid array length %d for " + name + "\", val.size());");
          cpp.println(indent + "  }");
        }
        else {
          cpp.println(indent + "  set" + singleName + "Count(val.size());");
        }
        cpp.println(indent + "  for (size_t i = 0; i < val.size(); i++) {");
        cpp.println(indent + "    set" + singleName + "(i, val[i]);");
        cpp.println(indent + "  }");
      }
      cpp.println(indent + "}");
      cpp.println(indent + "");
      cpp.println(indent + "void "+record.name+"::set" + singleName + " (int32_t i, " + arrayType.getTypeCPPSet() + " val) {");
      cpp.println(indent + "  if ((i < 0) || (i > " + getCount + ")) {");
      cpp.println(indent + "    throw VRTException(\"Invalid index %d into " + name + "\", i);");
      cpp.println(indent + "  }");
      cpp.println(indent + "  int32_t off = " + offset + " + (" + bytes + " * i);");
      cpp.println(indent + "  " + getPackFuncCPP(arrayType, "off", bytes) + ";");
      cpp.println(indent + "}");
      if (count < 0) {
        cpp.println(indent + "");
        cpp.println(indent + "void "+record.name+"::set" + singleName + "Count (int32_t length) {");
        cpp.println(indent + "  if (length < 0) {");
        cpp.println(indent + "    throw VRTException(\"Illegal length %d for " + name + "\", length);");
        cpp.println(indent + "  }");
        cpp.println(indent + "  int32_t len = ((length * " + bytes + ") + 3) & 0x7FFFFFFC; // length to next multiple of 4");
        cpp.println(indent + "  setPayloadLength(" + offset + " + len);");
        cpp.println(indent + "}");
      }
      cpp.println(indent + "");
      cpp.println(indent + "void "+record.name+"::_setAll" + methodName + " (const Value* val) {");
      if (count >= 0) {
        cpp.println(indent + "  if (val->size() != " + b + ") {");
        cpp.println(indent + "    throw VRTException(\"Invalid array length %d for " + name + "\", val->size());");
        cpp.println(indent + "  }");
      }
      else {
        cpp.println(indent + "  set" + singleName + "Count(val->size());");
      }
      cpp.println(indent + "  for (size_t i = 0; i < val->size(); i++) {");
      cpp.println(indent + "    Value *v = val->at(i);");
      if (isCPPNativeType(t)) {
        cpp.println(indent + "    set" + singleName + "(i, v->as<"+arrayType.getTypeCPPGet()+">());");
      }
      else {
        cpp.println(indent + "    set" + singleName + "(i, *v->cast<"+arrayType.getTypeCPPGet()+"*>());");
      }
      cpp.println(indent + "    delete v;");
      cpp.println(indent + "  }");
      cpp.println(indent + "}");
      cpp.println(indent + "");
    }
  }

  /** Write the get method. */
  private void writeJavaGet (PrintStream out, String permission, String html, String size, String limit, String len, String indent) {
    html = html.replace("\n",    "\n   *  ");
    Type t = getType();
    // === DOCS ===
    out.println(indent + "  ");
    out.println(indent + "  /** Get " + html + " ");
    if ((units != null) && (units.length() > 0)) {
      out.println(indent + "   *  <br> Units: <i>" + units + "</i>");
    }
    if ((range != null) && (range.length() > 0)) {
      out.println(indent + "   *  <br> Range: <tt>" + range + "</tt>");
    }
    if (t.isStringType() || t.isArrayType()) {
      out.println(indent + "   *  <br> Size: <tt>" + size + "</tt>");
    }
    out.println(indent + "   *  @return The value read from the packet.");
    out.println(indent + "   */");
    // === CODE ===
    if (!t.isArrayType()) {
      out.println(indent + "  " + permission + " " + t.getTypeJavaGet() + " get" + methodName + " () {");
      out.println(indent + "    return " + getUnpackFuncJava(t, offset, len) + ";");
      out.println(indent + "  }");
    }
    else {
      Type   arrayType  = t.getArrayType();
      int    count      = t.getArrayLength();
      int    bytes      = arrayType.getTypeBytes();
      String singleName = notPlural(methodName);
      String getCount   = "get" + singleName + "Count()";
      
      out.println(indent + "  " + permission + " " + arrayType.getTypeJavaGet() + "[] getAll" + methodName + " () {");
      if (t.getArrayType() == BYTE) {
        out.println(indent + "    // Use fast method for unpacking an array of bytes");
        out.println(indent + "    return unpackPayloadBytes(" + offset + ", " + getCount + ");");
      }
      else {
        out.println(indent + "    int len = " + getCount + ";");
        out.println(indent + "    " + arrayType.getTypeJavaGet() + "[] all = new " + arrayType.getTypeJavaGet() + "[len];");
        out.println(indent + "    for (int i = 0; i < len; i++) {");
        out.println(indent + "      all[i] = get" + singleName + "(i);");
        out.println(indent + "    }");
        out.println(indent + "    return all;");
      }
      out.println(indent + "  }");
      out.println(indent + "  ");
      out.println(indent + "  /** Get item in " + html + " ");
      out.println(indent + "   *  @param i The index.");
      out.println(indent + "   *  @return The value read from the packet.");
      out.println(indent + "   *  @throws java.lang.IllegalArgumentException if <tt>i</tt> is greater than or equal to " + getCount + "");
      out.println(indent + "   */");
      out.println(indent + "  " + permission + " " + arrayType.getTypeJavaGet() + " get" + singleName + " (int i) {");
      out.println(indent + "    if ((i < 0) || (i > " + getCount + ")) {");
      out.println(indent + "      throw new java.lang.IllegalArgumentException(\"Invalid index \"+i+\" into " + name + "\");");
      out.println(indent + "    }");
      out.println(indent + "    int off = " + offset + " + (" + bytes + " * i);");
      out.println(indent + "    return " + getUnpackFuncJava(arrayType, "off", bytes) + ";");
      out.println(indent + "  }");
      out.println(indent + "  ");
      out.println(indent + "  /** Get length of " + html + " ");
      if (count >= 0) {
        out.println(indent + "   *  @return The number of entries, which is always " + count + ".");
      }
      else {
        out.println(indent + "   *  @return The number of entries.");
      }
      out.println(indent + "   */");
      out.println(indent + "  " + permission + " int get" + singleName + "Count () {");
      if (count >= 0) {
        out.println(indent + "    return " + len + ";");
      }
      else {
        out.println(indent + "    return (getPayloadLength()-" + offset + ")/" + bytes + ";");
      }
      out.println(indent + "  }");
    }
  }

  /** Write the set method. */
  private void writeJavaSet (PrintStream out, String permission, String html, String size, String limit, String len, String indent) {
    html = html.replace("\n",    "\n   *  ");
    Type t = getType();
    int b = t.getTypeBytes();
    // === DOCS ===
    out.println(indent + "  ");
    out.println(indent + "  /** Set " + html + " ");
    if ((units != null) && (units.length() > 0)) {
      out.println(indent + "   *  <br> Units: <i>" + units + "</i>");
    }
    if ((range != null) && (range.length() > 0)) {
      out.println(indent + "   *  <br> Range: <tt>" + range + "</tt>");
    }
    if (t.isArrayType() || t.isStringType()) {
      out.println(indent + "   *  <br> Size: <tt>" + size + "</tt>");
    }
    if (t.isStringType()) {
      out.println(indent + "   *  @param val The new value to write to the packet.");
      if (t.isAsciiType()) {
        out.println(indent + "   *             Any non-ASCII characters in the string (numeric value &gt;0x7F) or ASCII");
        out.println(indent + "   *             control characters (numeric value &lt;0x20, 0x7F) will be replaced with \'?\'.");
      }
      if (b >= 0) {
        out.println(indent + "   *             If the string is longer than the size allotted, it will be");
        out.println(indent + "   *             truncated without warning. ");
      }
    }
    else {
      out.println(indent + "   *  @param val The new value to write to the packet.");
    }
    if (t.isArrayType()) {
      out.println(indent + "   *  @throws java.lang.IllegalArgumentException If the length of <tt>val</tt> is invalid");
    }
    out.println(indent + "   */");
    // === CODE ===
    if (!t.isArrayType()) {
      out.println(indent + "  " + permission + " void set" + methodName + " (" + t.getTypeJavaSet() + " val) {");
      if (b < 0) {
        if (limit != null) {
          out.println(indent + "    int limit  = " + limit + ";");
          out.println(indent + "    int length = " + len + ";");
          out.println(indent + "    int len    = (length > limit)? ((limit  + 3) & 0x7FFFFFFC)");
          out.println(indent + "                                 : ((length + 3) & 0x7FFFFFFC); // length to next multiple of 4");
          out.println(indent + "    setPayloadLength(" + offset + " + len);");
        }
        else {
          out.println(indent + "    int len = (" + len + " + 3) & 0x7FFFFFFC; // length to next multiple of 4");
          out.println(indent + "    setPayloadLength(" + offset + " + len);");
        }
        len = "len";
      }
      out.println(indent + "    " + getPackFuncJava(t, offset, len) + ";");
      out.println(indent + "  }");
    }
    else {
      Type   arrayType  = t.getArrayType();
      int    count      = t.getArrayLength();
      int    bytes      = arrayType.getTypeBytes();
      String singleName = notPlural(methodName);
      String getCount   = "get" + singleName + "Count()";
      
      out.println(indent + "  " + permission + " void setAll" + methodName + " (" + arrayType.getTypeJavaGet() + " ... val) {");
      if (t.getArrayType() == BYTE) {
        out.println(indent + "    // Use fast method for packing an array of bytes");
        out.println(indent + "    packPayloadBytes(" + offset + ", val, " + getCount + ");");
      }
      else {
        if (count >= 0) {
          out.println(indent + "    if (val.length != " + b + ") {");
          out.println(indent + "      throw new java.lang.IllegalArgumentException(\"Invalid array length \"+val.length+\" for " + name + "\");");
          out.println(indent + "    }");
        }
        else {
          out.println(indent + "    set" + singleName + "Count(val.length);");
        }
        out.println(indent + "    for (int i = 0; i < val.length; i++) {");
        out.println(indent + "      set" + singleName + "(i, val[i]);");
        out.println(indent + "    }");
      }
      out.println(indent + "  }");
      out.println(indent + "  ");
      out.println(indent + "  /** Set item in " + html + " ");
      out.println(indent + "   *  @param i   The index.");
      out.println(indent + "   *  @param val The new value.");
      out.println(indent + "   *  @throws java.lang.IllegalArgumentException if <tt>i</tt> is greater than or equal to " + getCount + "");
      out.println(indent + "   */");
      out.println(indent + "  " + permission + " void set" + singleName + " (int i, " + arrayType.getTypeJavaSet() + " val) {");
      out.println(indent + "    if ((i < 0) || (i > " + getCount + ")) {");
      out.println(indent + "      throw new java.lang.IllegalArgumentException(\"Invalid index \"+i+\" into " + name + "\");");
      out.println(indent + "    }");
      out.println(indent + "    int off = " + offset + " + (" + bytes + " * i);");
      out.println(indent + "    " + getPackFuncJava(arrayType, "off", bytes) + ";");
      out.println(indent + "  }");
      if (count < 0) {
        out.println(indent + "  ");
        out.println(indent + "  /** Set length of " + html + " ");
        out.println(indent + "   *  @param length The number of entries.");
        out.println(indent + "   *  @throws java.lang.IllegalArgumentException if <tt>length</tt> is negative");
        out.println(indent + "   */");
        out.println(indent + "  " + permission + " void set" + singleName + "Count (int length) {");
        out.println(indent + "    if (length < 0) {");
        out.println(indent + "      throw new java.lang.IllegalArgumentException(\"Illegal length \"+length+\" for " + name + "\");");
        out.println(indent + "    }");
        out.println(indent + "    int len = ((length * " + bytes + ") + 3) & 0x7FFFFFFC; // length to next multiple of 4");
        out.println(indent + "    setPayloadLength(" + offset + " + len);");
        out.println(indent + "  }");
      }
    }
  }

  /** Returns the function call to unpack a given type. */
  private String getUnpackFuncJava (Type t, Object off, Object len) {
    String prefix = (record != packetType) ? "unpack" : "unpackPayload";

    if (t == BOOLEAN            ) return prefix + "Boolean("   + off + ")";
    if (t == BOOLNULL           ) return prefix + "Boolean("   + off + ")";
    if (t == INETADDR           ) return prefix + "InetAddr("  + off + ")";
    if (t == UUID               ) return prefix + "UUID("      + off + ")";
    if (t == BYTE               ) return prefix + "Byte("      + off + ")";
    if (t == SHORT              ) return prefix + "Short("     + off + ")";
    if (t == INT                ) return prefix + "Int("       + off + ")";
    if (t == LONG               ) return prefix + "Long("      + off + ")";
    if (t == FLOAT              ) return prefix + "Float("     + off + ")";
    if (t == DOUBLE             ) return prefix + "Double("    + off + ")";
    if (t == UTCTIME            ) return prefix + "TimeStamp(" + off + ", nxm.vrt.lib.TimeStamp.IntegerMode.UTC)";
    if (t == GPSTIME            ) return prefix + "TimeStamp(" + off + ", nxm.vrt.lib.TimeStamp.IntegerMode.GPS)";
    if (t.isAsciiType()         ) return prefix + "Ascii("     + off + ", " + len + ")";
    if (t.isUTF8Type()          ) return prefix + "UTF8("      + off + ", " + len + ")";
    if (t.isAsciiTypeStr()      ) return prefix + "Ascii("     + off + ", " + len + ")";
    if (t.isUTF8TypeStr()       ) return prefix + "UTF8("      + off + ", " + len + ")";
    if (t.isMetadataType()      ) return prefix + "Metadata("  + off + ", " + len + ")";
    if (t instanceof Record     ) return prefix + "Record("    + off + ", new " + t.getTypeJavaGet() + "())";
    if (t instanceof Enumeration) return "" + t.getTypeJavaGet() + ".forValue(" + prefix + "Byte(" + off + "))";
    throw new RuntimeException("Unsupported type " + t.getTypeName());
  }

  /** Returns the function call to pack a given type. */
  private String getPackFuncJava (Type t, Object off, Object len) {
    String prefix = (record != packetType) ? "pack" : "packPayload";

    if (t == BOOLEAN            ) return prefix + "Boolean("   + off + ", val)";
    if (t == BOOLNULL           ) return prefix + "Boolean("   + off + ", val)";
    if (t == INETADDR           ) return prefix + "InetAddr("  + off + ", val)";
    if (t == UUID               ) return prefix + "UUID("      + off + ", val)";
    if (t == BYTE               ) return prefix + "Byte("      + off + ", val)";
    if (t == SHORT              ) return prefix + "Short("     + off + ", val)";
    if (t == INT                ) return prefix + "Int("       + off + ", val)";
    if (t == LONG               ) return prefix + "Long("      + off + ", val)";
    if (t == FLOAT              ) return prefix + "Float("     + off + ", val)";
    if (t == DOUBLE             ) return prefix + "Double("    + off + ", val)";
    if (t == UTCTIME            ) return prefix + "TimeStamp(" + off + ", val, nxm.vrt.lib.TimeStamp.IntegerMode.UTC)";
    if (t == GPSTIME            ) return prefix + "TimeStamp(" + off + ", val, nxm.vrt.lib.TimeStamp.IntegerMode.GPS)";
    if (t.isAsciiType()         ) return prefix + "Ascii("     + off + ", val, " + len + ")";
    if (t.isUTF8Type()          ) return prefix + "UTF8("      + off + ", val, " + len + ")";
    if (t.isAsciiTypeStr()      ) return prefix + "Ascii("     + off + ", val, " + len + ")";
    if (t.isUTF8TypeStr()       ) return prefix + "UTF8("      + off + ", val, " + len + ")";
    if (t.isMetadataType()      ) return prefix + "Metadata("  + off + ", val, " + len + ")";
    if (t instanceof Record     ) return prefix + "Record("    + off + ", val)";
    if (t instanceof Enumeration) return prefix + "Byte("      + off + ", " + t.getTypeJavaGet() + ".getValue(val))";
    throw new RuntimeException("Unsupported type " + t.getTypeName());
  }

  /** Returns the function call to unpack a given type. */
  private String getUnpackFuncCPP (Type t, Object off, Object len) {
    String prefix = (record != packetType) ? "unpack" : "unpackPayload";

    if (t == BOOLEAN            ) return prefix + "Boolean("   + off + ")";
    if (t == BOOLNULL           ) return prefix + "Boolean("   + off + ")";
    if (t == INETADDR           ) return prefix + "InetAddr("  + off + ")";
    if (t == UUID               ) return prefix + "UUID("      + off + ")";
    if (t == BYTE               ) return prefix + "Byte("      + off + ")";
    if (t == SHORT              ) return prefix + "Short("     + off + ")";
    if (t == INT                ) return prefix + "Int("       + off + ")";
    if (t == LONG               ) return prefix + "Long("      + off + ")";
    if (t == FLOAT              ) return prefix + "Float("     + off + ")";
    if (t == DOUBLE             ) return prefix + "Double("    + off + ")";
    if (t == UTCTIME            ) return prefix + "TimeStamp(" + off + ", IntegerMode_UTC)";
    if (t == GPSTIME            ) return prefix + "TimeStamp(" + off + ", IntegerMode_GPS)";
    if (t.isAsciiType()         ) return prefix + "Ascii("     + off + ", " + len + ")";
    if (t.isUTF8Type()          ) return prefix + "UTF8("      + off + ", " + len + ")";
    if (t.isAsciiTypeStr()      ) return prefix + "Ascii("     + off + ", " + len + ")";
    if (t.isUTF8TypeStr()       ) return prefix + "UTF8("      + off + ", " + len + ")";
    if (t.isMetadataType()      ) return prefix + "Metadata("  + off + ", " + len + ")";
    if (t instanceof Record     ) return prefix + "Record("    + off + ", r)"; // <-- special handling
    if (t instanceof Enumeration) return "(" + t.getTypeJavaGet() + ")" + prefix + "Byte(" + off + ")";
    throw new RuntimeException("Unsupported type " + t.getTypeName());
  }

  /** Returns the function call to pack a given type. */
  private String getPackFuncCPP (Type t, Object off, Object len) {
    String prefix = (record != packetType) ? "pack" : "packPayload";

    if (t == BOOLEAN            ) return prefix + "Boolean("   + off + ", val)";
    if (t == BOOLNULL           ) return prefix + "Boolean("   + off + ", val)";
    if (t == INETADDR           ) return prefix + "InetAddr("  + off + ", val)";
    if (t == UUID               ) return prefix + "UUID("      + off + ", val)";
    if (t == BYTE               ) return prefix + "Byte("      + off + ", val)";
    if (t == SHORT              ) return prefix + "Short("     + off + ", val)";
    if (t == INT                ) return prefix + "Int("       + off + ", val)";
    if (t == LONG               ) return prefix + "Long("      + off + ", val)";
    if (t == FLOAT              ) return prefix + "Float("     + off + ", val)";
    if (t == DOUBLE             ) return prefix + "Double("    + off + ", val)";
    if (t == UTCTIME            ) return prefix + "TimeStamp(" + off + ", val, IntegerMode_UTC)";
    if (t == GPSTIME            ) return prefix + "TimeStamp(" + off + ", val, IntegerMode_GPS)";
    if (t.isAsciiType()         ) return prefix + "Ascii("     + off + ", val, " + len + ")";
    if (t.isUTF8Type()          ) return prefix + "UTF8("      + off + ", val, " + len + ")";
    if (t.isAsciiTypeStr()      ) return prefix + "Ascii("     + off + ", val, " + len + ")";
    if (t.isUTF8TypeStr()       ) return prefix + "UTF8("      + off + ", val, " + len + ")";
    if (t.isMetadataType()      ) return prefix + "Metadata("  + off + ", val, " + len + ")";
    if (t instanceof Record     ) return prefix + "Record("    + off + ", val)";
    if (t instanceof Enumeration) return prefix + "Byte("      + off + ", (int8_t)val)";
    throw new RuntimeException("Unsupported type " + t.getTypeName());
  }

  /** Is the given type a native type in C++ (or an array of native types)? */
  private boolean isCPPNativeType (Type t) {
    if (t.isArrayType()) return isCPPNativeType(t.getArrayType());
    
    if (t == BOOLEAN            ) return true;
    if (t == BOOLNULL           ) return true;
    if (t == BYTE               ) return true;
    if (t == SHORT              ) return true;
    if (t == INT                ) return true;
    if (t == LONG               ) return true;
    if (t == FLOAT              ) return true;
    if (t == DOUBLE             ) return true;
    if (t.isAsciiType()         ) return true; // string
    if (t.isUTF8Type()          ) return true; // wstring
    if (t.isAsciiTypeStr()      ) return true; // string
    if (t.isUTF8TypeStr()       ) return true; // wstring
    if (t instanceof Enumeration) return true; // int8_t
    
    return false;
  }

  /** Gets the <tt>HasFields</tt> type for C++. */
  public CharSequence getCPPFieldType () {
    Type   t      = getType();
    String prefix = "";
    if (t.isArrayType()) {
      prefix = "(ValueType)-";
      t      = t.getArrayType();
    }

    if (t == BOOLEAN            ) return prefix+"ValueType_Bool";
    if (t == BOOLNULL           ) return prefix+"ValueType_BoolNull";
    if (t == INETADDR           ) return prefix+"ValueType_VRTObject";
    if (t == UUID               ) return prefix+"ValueType_VRTObject";
    if (t == BYTE               ) return prefix+"ValueType_Int8";
    if (t == SHORT              ) return prefix+"ValueType_Int16";
    if (t == INT                ) return prefix+"ValueType_Int32";
    if (t == LONG               ) return prefix+"ValueType_Int64";
    if (t == FLOAT              ) return prefix+"ValueType_Float";
    if (t == DOUBLE             ) return prefix+"ValueType_Double";
    if (t == UTCTIME            ) return prefix+"ValueType_VRTObject";
    if (t == GPSTIME            ) return prefix+"ValueType_VRTObject";
    if (t.isAsciiType()         ) return prefix+"ValueType_String";
    if (t.isUTF8Type()          ) return prefix+"ValueType_WString";
    if (t.isAsciiTypeStr()      ) return prefix+"ValueType_String";
    if (t.isUTF8TypeStr()       ) return prefix+"ValueType_WString";
    if (t.isMetadataType()      ) return prefix+"ValueType_VRTObject";
    if (t instanceof Record     ) return prefix+"ValueType_VRTObject";
    if (t instanceof Enumeration) return prefix+"ValueType_Int8";
    throw new RuntimeException("Unsupported type " + t.getTypeName());
  }
  
  /** Gets the <tt>HasFields</tt> get value call for C++. */
  public CharSequence getCPPGetField () {
    Type t = getType();
    
    if (t.isArrayType()) {
      return "new Value(_getAll"+methodName+"())";
    }
    else if (getCPPFieldType().equals("ValueType_VRTObject")) {
      return "new Value(new "+t.getTypeCPPGet()+"(get"+methodName+"()), true)";
    }
    else if (t instanceof Enumeration) {
      return "new Value((int8_t)get"+methodName+"())";
    }
    else {
      return "new Value(get"+methodName+"())";
    }
  }
  
  /** Gets the <tt>HasFields</tt> set value call for C++. */
  public CharSequence getCPPSetField () {
    Type t = getType();
    
    if (t.isArrayType()) {
      return "_setAll"+methodName+"(val)";
    }
    else if (getCPPFieldType().equals("ValueType_VRTObject")) {
      return "set"+methodName+"(*val->cast<"+t.getTypeCPPSet()+"*>())";
    }
    else if (t instanceof Enumeration) {
      return "set"+methodName+"(("+t.getTypeCPPSet()+")val->as<int8_t>())";
    }
    else {
      return "set"+methodName+"(val->as<"+t.getTypeCPPGet()+">())";
    }
  }
  
  /** Gets the <tt>HasFields</tt> type for Java. */
  public CharSequence getJavaFieldType () {
    Type t = getType();
    return getTypeJavaClass(t);
  }
  
  /** Gets the <tt>HasFields</tt> get value call for Java. */
  public CharSequence getJavaGetField () {
    Type   t   = getType();
    String all = (t.isArrayType())? "All" : "";
    
    if (t == INETADDR) return "nxm.vrt.lib.Utilities.asHasFields(get"+all+methodName+"())";
    if (t == UUID    ) return "nxm.vrt.lib.Utilities.asHasFields(get"+all+methodName+"())";
    
    return "get"+all+methodName+"()";
  }
  
  /** Gets the <tt>HasFields</tt> set value call for Java. */
  public CharSequence getJavaSetField () {
    Type   t   = getType();
    String all = (t.isArrayType())? "All" : "";
    
    return "set"+all+methodName+"(nxm.vrt.lib.Utilities.castValue("+getTypeJavaClass(t)+",val))";
  }
  
  /** Converts Java type to object (for example "int" to "Integer"). */
  private static CharSequence getTypeJavaClass (Type t) {
    if (t.isArrayType()) {
      Type tt = t.getArrayType();
      
      if (tt == BOOLEAN) return "boolean[].class";
      if (tt == BYTE   ) return "byte[].class";
      if (tt == SHORT  ) return "short[].class";
      if (tt == INT    ) return "int[].class";
      if (tt == LONG   ) return "long[].class";
      if (tt == FLOAT  ) return "float[].class";
      if (tt == DOUBLE ) return "double[].class";
      return tt.getTypeJavaGet()+"[].class";
    }
    else {
      if (t == BOOLEAN) return "Boolean.TYPE";
      if (t == BYTE   ) return "Byte.TYPE";
      if (t == SHORT  ) return "Short.TYPE";
      if (t == INT    ) return "Integer.TYPE";
      if (t == LONG   ) return "Long.TYPE";
      if (t == FLOAT  ) return "Float.TYPE";
      if (t == DOUBLE ) return "Double.TYPE";
      return t.getTypeJavaGet()+".class";
    }
  }
  
  /** Parameters used when generating C++ and Java code. */
  private static class CodeParams {
    final String html;
    final String size;
    final String limit;
    final String lenGet;
    final String lenSet;
    
    public CodeParams (String html, String size, String limit, String lenGet, String lenSet) {
      this.html   = html;
      this.size   = size;
      this.limit  = limit;
      this.lenGet = lenGet;
      this.lenSet = lenSet;
    }
  }
}

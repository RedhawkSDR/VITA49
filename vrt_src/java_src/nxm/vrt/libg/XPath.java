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
import static nxm.vrt.libg.Type.UTCTIME;
import static nxm.vrt.libg.Type.GPSTIME;

/** An XPath expression for initializing a field in the packet.
 *
 *  @author 
 */
class XPath extends Definition {
  final PacketType packetType;
  final String     name;
  final String     func;
  final String     xpath;

  /** Creates a new XPath definition.
   *  @param pt   Packet type.
   *  @param node Node from the XML.
   */
  public XPath (PacketType pt, Node node) {
    packetType = pt;
    name       = evalXPath("./@NAME",  node, "");
    func       = evalXPath("./@FUNC",  node, "");
    xpath      = evalXPath("./@XPATH", node, "");
  }

  public boolean inRecord () { return name.indexOf('.') > 0; }
  public boolean inArray ()  { return name.indexOf('[') > 0; }

  @Override
  public String toString () {
    return "XPath: "+name+"="+xpath;
  }

  public Field getField () {
    String n = name;
    int i;
    if ((i = n.indexOf('.')) > 0) n = n.substring(0, i);
    if ((i = n.indexOf('[')) > 0) n = n.substring(0, i);
    return packetType.fieldMap.get(n);
  }

  public void writeConstant (PacketGen outParams, PrintStream out) {
    if ((xpath == null) || (xpath.length() == 0)) {
      return;
    }
    Field f = getField();
    if (f == null) {
      return;
    }
    Type t = f.getType();
    if (t.isArrayType()) {
      String n = "__" + name.replace('.', '_').replace("[n]", "$n$");
      String x = xpath.replaceAll("[$][{](.*)n(.*)[}]", "\"+($1n$2)+\"");
      out.println("  private static final org.jdom.xpath.XPath " + n + "(int n) { return nxm.vrt.lib.XmlUtilities.xpath(\"" + x + "\"); }");
      if (t.getArrayLength() > 0) {
        for (int i = 0; i < t.getArrayLength(); i++) {
          String nn = n.replace("$n$", "$" + i + "$");
          out.println("  private static final org.jdom.xpath.XPath " + nn + " = " + n + "(" + i + ");");
        }
      }
    }
    else {
      String n = "__" + name.replace(".", "__");
      out.println("  private static final org.jdom.xpath.XPath " + n + " = nxm.vrt.lib.XmlUtilities.xpath(\"" + xpath + "\");");
    }
  }

  public void writeComment (PacketGen outParams, PrintStream out) {
    if ((xpath == null) || (xpath.length() == 0)) {
      return;
    }
    Field f = getField();
    if (f == null) {
      return;
    }
    String xp = xpath.replace("*", "&x002A;").replace("@", "&x0040;"); // prevents issues with "@" in path which is common
    String n  = name;

    while (n.length() < 16) n += " ";

    if ((func == null) || (func.length() == 0)) {
      out.println("        " + n + "  =  " + xp);
    }
    else {
      String ff = func;
      while (ff.length() < 16) ff += " ";
      out.println("        " + n + "  =  " + ff + " where #=" + xp);
    }
  }

  @Override
  public void writeJava (PacketGen outParams, PrintStream out) {
    if ((xpath == null) || (xpath.length() == 0)) {
      return;
    }
    Field f = getField();
    if (f == null) {
      return;
    }
    Type t = f.getType();
    if (t.isArrayType()) {
      if (t.getArrayLength() > 0) {
        for (int i = 0; i < t.getArrayLength(); i++) {
          String n = "__" + name.replace('.', '_').replace("[n]", "$" + i + "$");
          writeJava(outParams, out, n, i, f, t.getArrayType());
        }
      }
    }
    else {
      String n = "__" + name.replace('.', '_');
      writeJava(outParams, out, n, -1, f, t);
    }
  }

  private void writeJava (PacketGen outParams, PrintStream out, String n, int index, Field f, Type t) {
    String ind = (index < 0) ? "" : Integer.toString(index); // element index (blank if n/a)
    String idx = (index < 0) ? "" : ind + ", ";              // element index with trailing comma
    String val;
    Record r   = null;
    
    if (t instanceof Record) {
      r = (Record)t;
      f = r.fieldMap.get(name.substring(name.indexOf('.') + 1));
      t = f.getType();
    }
    
    out.println("    { // initialize "+name);
    if (t.isNumericType()) {
      out.println("      java.lang.Number n = __" + name + ".numberValueOf(xml);");
      val = "n." + t.getTypeJavaGet() + "Value()";
    }
    else {
      out.println("      java.lang.String s = __" + name + ".valueOf(xml);");
      val = "s";
    }
    
    if (r != null) {
      out.println("      " + r.name + " r = get" + f.methodName + "(" + ind + ");");
      out.println("      r.set" + f.methodName + "(" + javaSetFunc(func, t, val) + ");");
      out.println("      set" + f.methodName + "(" + idx + "r);");
    }
    else {
      out.println("      set" + f.methodName + "(" + idx + javaSetFunc(func, t, val) + ");");
    }
    out.println("    }");
  }

  private String javaSetFunc (String func, Type t, String val) {
    String cast = "";
    if ((func != null) && (func.length() > 0)) {
      if (t.isNumericType() && (t != DOUBLE)) {
        cast = "(" + t + ")";
      }
      val = func.replace("toDegrees(",    "java.lang.Math.toDegrees(")
                .replace("toRadians(",    "java.lang.Math.toRadians(")
                .replace("trim(",         "nxm.vrt.lib.Utilities.trim(")
                .replace("trimNA(",       "nxm.vrt.lib.Utilities.trimNA(")
                .replace("present(",      "nxm.vrt.lib.Utilities.present(")
                .replace("isString(",     "nxm.vrt.lib.Utilities.isString(")
                .replace("#",             val);
      if (!val.equals("s") && !val.matches("[a-zA-Z.]*[(].*[)]")) {
        val = "(" + val + ")";
      }
    }
    if (t instanceof Enumeration) return ((Enumeration)t).name + ".forName(" + val + ")";
    if (t == BOOLEAN            ) return "nxm.vrt.lib.Utilities.toBooleanValue(" + val + ")";
    if (t == BOOLNULL           ) return "nxm.vrt.lib.Utilities.toBoolean(" + val + ")";
    if (t == UTCTIME            ) return "nxm.vrt.lib.TimeStamp.parseTime(" + val + ", nxm.vrt.lib.TimeStamp.IntegerMode.UTC)";
    if (t == GPSTIME            ) return "nxm.vrt.lib.TimeStamp.parseTime(" + val + ", nxm.vrt.lib.TimeStamp.IntegerMode.GPS)";
    if (t == BYTE               ) return "((n == null)? (byte)0 : "  + cast + val + ")";
    if (t == SHORT              ) return "((n == null)? (short)0 : " + cast + val + ")";
    if (t == INT                ) return "((n == null)? 0 : "        + cast + val + ")";
    if (t == LONG               ) return "((n == null)? 0L : "       + cast + val + ")";
    if (t == FLOAT              ) return "((n == null)? 0f : "       + cast + val + ")";
    if (t == DOUBLE             ) return "((n == null)? 0d : "       + cast + val + ")";
    if (t.isStringType()        ) return val;
    throw new RuntimeException("Type " + t.getTypeName() + " not supported with XPath at this time.");
  }
}

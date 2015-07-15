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

package nxm.vita.intr;

import nxm.sys.lib.*;

import java.util.Map;
import java.util.LinkedHashMap;

/**  Documents the packets.
 *
 *  @author         
 */
public class Pipegen extends Intrinsic {
  
  @Override
  public int process() {
    String            packetClass = MA.getCS("PACKETCLASS");
    String            recordName  = MA.getCS("RECORDNAME");
    String            optionTree  = MA.getS("OPT");
    String            blueToVRT   = MA.getCS("BLUE2VRT");
    String            vrtToBLUE   = MA.getCS("VRT2BLUE");
    Table             conv        = MA.getTable("CONVERSIONS");
    Map<String,Table> conversions = new LinkedHashMap<String,Table>();
    boolean           interactive = MA.getState("/INTERACTIVE", (conv == null));
    boolean           doJava      = MA.getState("/JAVA");
    boolean           doCPP       = MA.getState("/CPP");

    if (conv != null) {
      for (Object val : conv.values()) {
        Table  tbl = Convert.o2t(val);
        String key = tbl.getString("BLUE_NAME");
        conversions.put(key, tbl);
      }
    }

    // INTERACTIVE PROMPTS =========================================================================
    if (interactive) {
      String fname;

      packetClass = promptUser("Packet Class          (required)", packetClass, REQUIRED_INPUT);
      recordName  = promptUser("Record Name           (required)", recordName,  REQUIRED_INPUT);
      
      if ((blueToVRT == null) || (blueToVRT.length() == 0)) blueToVRT = "VRT2"+recordName.toUpperCase();
      if ((vrtToBLUE == null) || (vrtToBLUE.length() == 0)) vrtToBLUE = recordName.toUpperCase()+"2VRT";
      
      optionTree  = promptUser("Option tree           (required)", optionTree,  null);
      blueToVRT   = promptUser("BLUE-To-VRT primitive (optional)", blueToVRT,   null);
      vrtToBLUE   = promptUser("VRT-to-BLUE primitive (optional)", vrtToBLUE,   null);
      fname       = promptUser("Template File         (optional)", null,        null);
      
      if (fname != null) {
        DataFile template = new DataFile(this, fname);
        template.open();

        Table recs = template.getRecordDefs();
        for (String key : recs.getKeys()) {
          Table val = recs.getTable(key);
          Table con = conversions.get(key);

          if (con == null) {
            con = new Table();
            con.put("BLUE_NAME",   val.getS("NAME"));
            con.put("BLUE_FORMAT", val.getS("FORMAT"));
            con.put("BLUE_UNITS",  val.getS("UNITS"));
            con.put("VRT_NAME",    val.getS("NAME"));
            con.put("BLUE_TO_VRT", "");
            con.put("VRT_TO_BLUE", "");
            conversions.put(key, con);
          }
        }
        template.close();
      }

      for (Table tbl : conversions.values()) {
        String key = tbl.getString("BLUE_NAME");
        Table  con = promptConv(tbl);
        if (con == null) conversions.remove(key);
        else             conversions.put(con.getS("BLUE_NAME"), con);
      }

      Table con;
      while ((con = promptConv(null)) != null) {
        String key = con.getString("BLUE_NAME");
        conversions.put(key, con);
      }
    }

    // INTIALIZATION ===============================================================================
    Class<?> clazz;
    String   switches = "";
    Table    convTbl  = new Table();

    try {
      clazz = Class.forName(packetClass);
    }
    catch (Exception e) {
      throw new MidasException("Unable to access packet class '"+packetClass+"'", e);
    }
    String packetClazz = packetClass.substring(packetClass.lastIndexOf('.')+1);


    for (Table c : conversions.values()) {
      convTbl.put(c.getS("BLUE_NAME"), c);
    }

    // OPEN THE FILES ==============================================================================
    TextFile javaToVRT   = null;
    TextFile javaFromVRT = null;
    TextFile cppToVRT    = null;
    TextFile cppFromVRT  = null;

    if (doJava) {
      switches   += "/JAVA";
      javaToVRT   = openTextFile(optionTree, "prim", blueToVRT, ".java");
      javaFromVRT = openTextFile(optionTree, "prim", vrtToBLUE, ".java");
    }
    if (doCPP) {
      switches  += "/CPP";
      cppToVRT   = openTextFile(optionTree, "host", blueToVRT, ".cc");
      cppFromVRT = openTextFile(optionTree, "host", vrtToBLUE, ".cc");
    }
    

    // BUILD THE JAVA CODE =========================================================================
    if (doJava) {
      println(javaToVRT, javaFromVRT, "// This file was auto-generated using the following command:");
      println(javaToVRT, javaFromVRT, "//  nM> PIPEGEN;VRT"+switches+" PACKETCLASS=\""+packetClass+"\" RECORDNAME=\""+recordName+"\" "
                                              +"OPT=\""+optionTree+"\" BLUE2VRT=\""+blueToVRT+"\" VRT2BLUE=\""+vrtToBLUE+"\" &");
      println(javaToVRT, javaFromVRT, "//      CONVERSIONS="+convTbl);
      println(javaToVRT, javaFromVRT, "//");
      println(javaToVRT, javaFromVRT, "package nxm."+optionTree.toLowerCase()+".prim;");
      println(javaToVRT, javaFromVRT, "");
      println(javaToVRT, javaFromVRT, "import static java.nio.ByteOrder.BIG_ENDIAN;");
      println(javaToVRT, javaFromVRT, "");
      println(javaToVRT,              "/** Basic primitive to convert a BLUE pipe into a VRT pipe for the type "+recordName+". */");
      println(           javaFromVRT, "/** Basic primitive to convert a VRT pipe into a BLUE pipe for the type "+recordName+". */");
      println(javaToVRT,              "public class "+toClassName(blueToVRT)+" extends nxm.vita.prim.blue2vrt {");
      println(           javaFromVRT, "public class "+toClassName(vrtToBLUE)+" extends nxm.vita.prim.vrt2blue {");
      println(javaToVRT, javaFromVRT, "  ");
      println(javaToVRT, javaFromVRT, "  private static final java.lang.String[] FIELD_DEFS = {");
      int index = 0;
      for (Table c : conversions.values()) {
        println(javaToVRT, javaFromVRT, "    /* offset["+(index++)+"] => */"
                                      + "    \""+c.getS("BLUE_FORMAT")+"\","
                                      + "    \""+c.getS("BLUE_NAME")+"\",");
      }
      println(javaToVRT, javaFromVRT, "  };");
      println(javaToVRT, javaFromVRT, "  ");
      println(javaToVRT, javaFromVRT, "  /**  Creates a new instance. */");
      println(javaToVRT,              "  public "+toClassName(blueToVRT)+" () {");
      println(           javaFromVRT, "  public "+toClassName(vrtToBLUE)+" () {");
      println(javaToVRT, javaFromVRT, "    super(FIELD_DEFS);");
      println(javaToVRT, javaFromVRT, "  }");
      println(javaToVRT, javaFromVRT, "  ");
      println(javaToVRT, javaFromVRT, "  @Override");
      println(javaToVRT, javaFromVRT, "  @SuppressWarnings(value={\"cast\"})");
      println(javaToVRT,              "  protected nxm.vrt.lib.VRTPacket toVRTPacket (int count) {");
      println(javaToVRT,              "    "+packetClass+" p = new "+packetClass+"();");
      println(javaToVRT,              "    p.set"+recordName+"Count(count);");
      println(javaToVRT,              "    for (int i = 0; i < count; i++) {");
      println(javaToVRT,              "      "+packetClass+"."+recordName+" r = new "+packetClass+"."+recordName+"();");

      println(           javaFromVRT, "  protected void fromVRTPacket (nxm.vrt.lib.VRTPacket pkt) {");
      println(           javaFromVRT, "    "+packetClass+" p = (pkt instanceof "+packetClass+")? ("+packetClass+")pkt"
                                                                                            +" : new "+packetClass+"(pkt,false);");
      println(           javaFromVRT, "    int count = p.get"+recordName+"Count();");
      println(           javaFromVRT, "    for (int i = 0; i < count; i++) {");
      println(           javaFromVRT, "      "+packetClass+"."+recordName+" r = p.get"+recordName+"(i);");

      index = 0;
      for (Table c : conversions.values()) {
        String blue_format = c.getS("BLUE_FORMAT");
        String blue_name   = c.getS("BLUE_NAME");
        String vrt_name    = c.getS("VRT_NAME");
        String toConvert   = c.getS("BLUE_TO_VRT");
        String fromConvert = c.getS("VRT_TO_BLUE");
        String vrt_type    = getJavaType(clazz, recordName, vrt_name);
        String blue_type   = getJavaType(blue_format);
        String pack        = "nxm.vrt.lib.VRTMath.pack"+getType(blue_format)+"(dataBuffer, offset["+index+"], ("+blue_type+")#, BIG_ENDIAN)";
        String unpack      = "nxm.vrt.lib.VRTMath.unpack"+getType(blue_format)+"(dataBuffer, offset["+index+"], BIG_ENDIAN)";
        String recGet      = "r.get"+vrt_name+"()";
        String recSet      = "r.set"+vrt_name+"(("+vrt_type+")#)";

        if ((toConvert != null) && (toConvert.length() > 0) && !toConvert.equals("#")) {
          unpack = "("+toConvert.replace("#", unpack)+")";
        }
        if ((fromConvert != null) && (fromConvert.length() > 0) && !fromConvert.equals("#")) {
          pack = pack.replace("#", "("+fromConvert+")");
        }

        println(javaToVRT,              "      "+recSet.replace("#",unpack)+"; // "+blue_name);
        println(           javaFromVRT, "      "+pack.replace("#",recGet)+"; // "+blue_name);
        index++;
      }
      println(javaToVRT,              "      p.set"+recordName+"(i,r);");
      println(javaToVRT, javaFromVRT, "      nextRecord();");
      println(javaToVRT, javaFromVRT, "    }");
      println(javaToVRT,              "    return p;");
      println(javaToVRT, javaFromVRT, "  }");
      println(javaToVRT, javaFromVRT, "}");
      println(javaToVRT, javaFromVRT, "");
    }

    // BUILD THE C++ CODE ==========================================================================
    if (doCPP) {
      println(cppToVRT, cppFromVRT, "// This file was auto-generated using the following command:");
      println(cppToVRT, cppFromVRT, "//  nM> PIPEGEN;VRT"+switches+" PACKETCLASS=\""+packetClass+"\" RECORDNAME=\""+recordName+"\" "
                                              +"OPT=\""+optionTree+"\" BLUE2VRT=\""+blueToVRT+"\" VRT2BLUE=\""+vrtToBLUE+"\" &");
      println(cppToVRT, cppFromVRT, "//      CONVERSIONS="+convTbl);
      println(cppToVRT, cppFromVRT, "//");
      println(cppToVRT, cppFromVRT, "#include \""+packetClazz+".h\"");
      println(cppToVRT,             "#include \"blue2vrt.cc\" // declares mainroutine()");
      println(          cppFromVRT, "#include \"vrt2blue.cc\" // declares mainroutine()");
      println(cppToVRT, cppFromVRT, "");
      println(cppToVRT, cppFromVRT, "// used by included .cc file");
      println(cppToVRT, cppFromVRT, "inline vector<string> getFieldDefs () {");
      println(cppToVRT, cppFromVRT, "  vector<string> FIELD_DEFS("+(conversions.size()*2)+");");
      int index = 0;
      for (Table c : conversions.values()) {
        println(cppToVRT, cppFromVRT, "  /* offset["+(index/2)+"] => */"
                                    + "  FIELD_DEFS["+(index++)+"] = \""+c.getS("BLUE_FORMAT")+"\";"
                                    + "  FIELD_DEFS["+(index++)+"] = \""+c.getS("BLUE_NAME")+"\";");
      }
      println(cppToVRT, cppFromVRT, "  return FIELD_DEFS;");
      println(cppToVRT, cppFromVRT, "}");
      println(cppToVRT, cppFromVRT, "");
      println(cppToVRT, cppFromVRT, "// used by included .cc file");
      println(cppToVRT,             "inline BasicVRTPacket toVRTPacket (const vector<int32_t> &offset, const vector<char> &dataBuffer, int32_t count) {");
      println(cppToVRT,             "  "+packetClazz+" p;");
      println(cppToVRT,             "  p.set"+recordName+"Count(count);");
      println(cppToVRT,             "  for (int i = 0; i < count; i++) {");
      println(cppToVRT,             "    "+recordName+" r = "+recordName+"();");

      println(          cppFromVRT, "inline void fromVRTPacket (const vector<int32_t> &offset, vector<char> &dataBuffer, const VRTPacket &pkt) {");
      println(          cppFromVRT, "  "+packetClazz+" p(pkt);");
      println(          cppFromVRT, "  int32_t count = p.get"+recordName+"Count();");
      println(          cppFromVRT, "  for (int32_t i = 0; i < count; i++) {");
      println(          cppFromVRT, "    "+recordName+" r = p.get"+recordName+"(i);");

      index = 0;
      for (Table c : conversions.values()) {
        String blue_format = c.getS("BLUE_FORMAT");
        String blue_name   = c.getS("BLUE_NAME");
        String vrt_name    = c.getS("VRT_NAME");
        String toConvert   = c.getS("BLUE_TO_VRT");
        String fromConvert = c.getS("VRT_TO_BLUE");
        String vrt_type    = getCppType(clazz, recordName, vrt_name);
        String blue_type   = getCppType(blue_format);
        String pack        = "VRTMath::pack"+getType(blue_format)+"(dataBuffer, offset["+index+"], ("+blue_type+")#, BIG_ENDIAN)";
        String unpack      = "VRTMath::unpack"+getType(blue_format)+"(dataBuffer, offset["+index+"], BIG_ENDIAN)";
        String recGet      = "r.get"+vrt_name+"()";
        String recSet      = "r.set"+vrt_name+"(("+vrt_type+")#)";

        if ((toConvert != null) && (toConvert.length() > 0) && !toConvert.equals("#")) {
          unpack = "("+toConvert.replace("#", unpack)+")";
        }
        if ((fromConvert != null) && (fromConvert.length() > 0) && !fromConvert.equals("#")) {
          pack = pack.replace("#", "("+fromConvert+")");
        }

        println(cppToVRT,             "    "+recSet.replace("#",unpack)+"; // "+blue_name);
        println(          cppFromVRT, "    "+pack.replace("#",recGet)+"; // "+blue_name);
        index++;
      }
      println(cppToVRT,             "    p.set"+recordName+"(i,r);");
      println(cppToVRT, cppFromVRT, "    nextRecord();");
      println(cppToVRT, cppFromVRT, "  }");
      println(cppToVRT,             "  return p;");
      println(cppToVRT, cppFromVRT, "}");
      println(cppToVRT, cppFromVRT, "");
    }

    // CLOSE THE FILES =============================================================================
    if (javaToVRT   != null) { M.info("Wrote output to "+javaToVRT.getName()  ); javaToVRT.close();   }
    if (javaFromVRT != null) { M.info("Wrote output to "+javaFromVRT.getName()); javaFromVRT.close(); }
    if (cppToVRT    != null) { M.info("Wrote output to "+cppToVRT.getName()   ); cppToVRT.close();    }
    if (cppFromVRT  != null) { M.info("Wrote output to "+cppFromVRT.getName() ); cppFromVRT.close();  }

    if (verbose) {
      M.info("Use UPDATE to update the commands in the dictionary, each command is a primitive that ");
      M.info("takes 2 files as arguments IN= and OUT=.");
    }
    return FINISH;
  }
  
  private TextFile openTextFile (String optionTree, String area, String name, String ext) {
    if ((name == null) || (name.length() == 0)) return null;
    if ((name.indexOf('/') < 0) && (name.indexOf('\\') < 0)) {
      String path = FileUtil.terminatePath(OptionTree.getOptPath(optionTree, area));
      name = path + name.toLowerCase();
    }
    
    if (name.indexOf('.') < 0) {
      name = name + ext;
    }

    TextFile tf = new TextFile(this, name);
    tf.open(TextFile.OUTPUT);
    return tf;
  }

  private static String toClassName (String name) {
    if (name == null) return null;
    int start = Math.max(name.lastIndexOf('/')+1, name.lastIndexOf('\\')+1);
    int end   = name.length();

    if (name.endsWith(".java")) end = end - 5;

    return name.substring(start, end).toLowerCase();
  }
  
  /**  Print to a file, if open. */
  private static void println (TextFile f, String str) {
    if (f != null) f.writeln(str);
  }
  /**  Print to a file, if open. */
  private static void print (TextFile f, String str) {
    if (f != null) f.write(str);
  }
  /**  Print to a file, if open. */
  private static void println (TextFile f1, TextFile f2, String str) {
    if (f1 != null) f1.writeln(str);
    if (f2 != null) f2.writeln(str);
  }
  /**  Print to a file, if open. */
  private static void print (TextFile f1, TextFile f2, String str) {
    if (f1 != null) f1.write(str);
    if (f2 != null) f2.write(str);
  }

  /**  Gets the type for a field in the class. */
  private static String getJavaType (Class<?> clazz, String recordName, String name) {
    try {
      Class<?> c = Class.forName(clazz.getName()+"$"+recordName);
      String   m = "get"+name;
      Class<?> t = c.getMethod(m).getReturnType();

      return t.getName();
    }
    catch (Exception e) {
      throw new MidasException("Can not find type for "+name, e);
    }
  }

  /**  Get a type matching a BLUE format. */
  private static String getType (String format) {
    switch (format.charAt(1)) {
      case 'D': return "Double";
      case 'F': return "Float";
      case 'X': return "Long";
      case 'L': return "Int";
      case 'I': return "Short";
      case 'B': return "Byte";
      case 'A': return "String";
      default:  throw new MidasException("Unsupported format '"+format+"'");
    }
  }

  /**  Get a type matching a BLUE format. */
  private static String getJavaType (String format) {
    switch (format.charAt(1)) {
      case 'D': return "double";
      case 'F': return "float";
      case 'X': return "long";
      case 'L': return "int";
      case 'I': return "short";
      case 'B': return "byte";
      case 'A': return "String";
      default:  throw new MidasException("Unsupported format '"+format+"'");
    }
  }

  /**  Gets the type for a field in the class. */
  private static String getCppType (Class<?> clazz, String recordName, String name) {
    return toCppType(getJavaType(clazz, recordName, name));
  }

  /**  Get a type matching a BLUE format. */
  private static String getCppType (String format) {
    return toCppType(getJavaType(format));
  }

  /**  Get a type matching a BLUE format. */
  private static String toCppType (String javaType) {
    if (javaType.equals("double")) return "double";
    if (javaType.equals("float" )) return "float";
    if (javaType.equals("long"  )) return "int64_t";
    if (javaType.equals("int"   )) return "int32_t";
    if (javaType.equals("short" )) return "int16_t";
    if (javaType.equals("byte"  )) return "int8_t";
    if (javaType.equals("String")) return "string";
    throw new MidasException("Unsupported Java type '"+javaType+"'");
  }

  /**  Prompt user regarding conversions. */
  private Table promptConv (Table in) {
    String blue_name;
    String blue_format;
    String vrt_name;
    String blue_to_vrt;
    String vrt_to_blue;

    if (in == null) {
      blue_name   = promptUser("Name in BLUE file ('-' if done)",      null,      null);
      if (blue_name.equals("-")) return null;
      blue_format = promptUser("Format in BLUE file (e.g. 'SL')",      null,      REQUIRED_INPUT);
      vrt_name    = promptUser("Name in VRT packet",                   blue_name, null);
      blue_to_vrt = promptUser("BLUE->VRT conversion (blank if n/a)",  "#",       null);
      vrt_to_blue = promptUser("VRT->BLUE conversion (blank if n/a)",  "#",       null);
    }
    else {
      blue_name   = promptUser("Name in BLUE file ('-' to skip)",      in.getS("BLUE_NAME"),           null);
      if (blue_name.equals("-")) return null;
      blue_format = promptUser("Format in BLUE file (e.g. 'SL')",      in.getS("BLUE_FORMAT"),         REQUIRED_INPUT);
      vrt_name    = promptUser("Name in VRT packet",                   in.getS("VRT_NAME", blue_name), null);
      blue_to_vrt = promptUser("BLUE->VRT conversion (blank if n/a)",  in.getS("BLUE_TO_VRT"),         null);
      vrt_to_blue = promptUser("VRT->BLUE conversion (blank if n/a)",  in.getS("VRT_TO_BLUE"),         null);
    }

    Table out = new Table();
    out.put("BLUE_NAME",   blue_name);
    out.put("BLUE_FORMAT", blue_format);
    out.put("VRT_NAME",    vrt_name);
    out.put("BLUE_TO_VRT", blue_to_vrt);
    out.put("VRT_TO_BLUE", vrt_to_blue);
    return out;
  }


  private static final String REQUIRED_INPUT = "";
  /**  Prompt the user for input. */
  private String promptUser (String prompt, String def, String errorMsg) {
    String promptText = (def == null)? prompt+" -> " : prompt+" ["+def+"] -> ";
    String strInput   = M.terminal.getInput(promptText,"",Terminal.INPUT_NO_ESC);

    if (strInput.startsWith("!") || strInput.startsWith("[") || strInput.startsWith("^")) {
      Shell.warning("Invalid input. Setting to default, if applicable");
      strInput = "";
    }

    if (strInput.length() == 0) {
      if ((errorMsg != null) && ((def == null) || (def.length() == 0))) {
        if (errorMsg.equals(REQUIRED_INPUT)) {
          int i = prompt.indexOf('(');
          if (i < 0) i = prompt.length();
          errorMsg = "Missing required input: "+prompt.substring(0,i);
        }
        throw new MidasException(errorMsg);
      }
      strInput = def;
    }
    return strInput;
  }
}

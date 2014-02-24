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

package nxm.vrt.dat;

import java.io.*;
import java.util.*;

import static java.lang.Long.toHexString;

/** <b>Internal Use Only:</b> Generates the applicable pack/unpack code based on the
 *  <tt>PackUnpackGen.code</tt> file. During processing the following replacements
 *  are made within the code:
 *  <pre>
 *    Token               |     Java Replacement     |     C++ Replacement      |
 *    --------------------+--------------------------+--------------------------+
 *    #CPP#               | false                    | true                     |
 *    #JAVA#              | true                     | false                    |
 *    #flatten#           | *delete*                 | __attribute__((flatten)) |
 *    #inline_recursive#  | *delete*                 | [0]                      |
 *    #namespace#         | *delete*                 | [1]                      |
 *    #loop1N#            | *delete*                 | [6,7]                    |
 *    #loopAB#            | *delete*                 | [7]                      |
 *    DataItemFormat_     | [8]                      | DataItemFormat_          |
 *    &gt;&gt;&gt;                 | &gt;&gt;&gt;                      | &gt;&gt;                       |
 *    const               | *delete*                 | const                    |
 *    public static       | public static            | *delete*                 |
 *    private static      | private static           | static                   |
 *    inline              | *delete*                 | inline                   |
 *    unsigned type       | type                     | utype                    |
 *    byte[] buf          | byte[] buf               | [2]                      |
 *    #param buf          | #param buf               | [3]                      |
 *    type[] name         | type[] name              | type *name               |
 *    null                | null                     | NULL                     |
 *    IllegalArgumentError| new IllegalArgEx...      | VRTException             |
 *    UnsupportedError    | new UnsupportedOpEx...   | VRTException             |
 *    --------------------+--------------------------+--------------------------+
 *    #type#              | External array type   (lower case)                  |
 *    #Type#              | External array type   (upper case)                  |
 *    #typebytes#         | Number of bytes in given type                       |
 *    #typecheck#         | Number of bytes in given integer type [4]           |
 *    #form#              | Data format in buffer (lower case)                  |
 *    #Form#              | Data format in buffer (upper case)                  |
 *    #formbytes#         | Number of bytes in given format [5]                 |
 *    #formcheck#         | Number of bytes in given integer format [4]         |
 *    #formbits#          | Number of bits  in given format                     |
 *    #word#              | Data format for word (Int32 or Int64) (lower case)  |
 *    #Word#              | Data format for word (Int32 or Int64) (upper case)  |
 *    #wordbits#          | Number of bits  in given word (32 or 64)            |
 *    #wordbytes#         | Number of bytes in given word ( 4 or  8)            |
 *    #typemask#          | Bit mast for given type as a 64-bit value           |
 *    #mask#              | Bit mask for given format / container               |
 *    #container#         | Container type unpacked bits                        |
 *    #continerbits#      | Number of bits in container type                    |
 *    (#ct#)              | Container type if double (or float), else delete.   |
 *    --------------------+--------------------------+--------------------------+
 *
 *    Notes:
 *      [0] Replacement is intended to be '_Intel_Pragma("inline recursive")' but
 *          the current Intel compilers seem to take issue with it, so it is
 *          simply deleted for now.
 *      [1] "vrt::PackUnpack::" in .cc file, but omitted in .h file
 *      [2] Replaces with "void *ptr" and adds "char *buf = (char*)ptr" on next line
 *      [3] Replaces javadoc with "ptr" in place of "buf" and adds "pointer" to end
 *      [4] This will be -4 for float and -8 for double.
 *      [5] This will be -8 for bits and -2 for nibbles.
 *      [6] The replacement is '#loop1B#' where B is '8-#formbytes#'.
 *      [7] The replacement is '_Intel_Pragma("loop_count min=A, max=B")'.
 *      [8] Deleted if following a 'case' but changed to 'DataItemFormat.' if
 *          anywhere else.
 *  </pre>
 *  @author 
 */
public class PackUnpackGen {
  private PackUnpackGen () { } // prevent instantiation

  /** The various data types used. */
  private static enum DataType {
    Double(      "double",  "double",  8, 64,   -8),
    Float(       "float",   "float",   4, 32,   -4),
    Long(        "long",    "int64_t", 8, 64,    8),
    Int(         "int",     "int32_t", 4, 32,    4),
    Int24(       "#int24#", "#int24#", 3, 24,    3),
    Short(       "short",   "int16_t", 2, 16,    2),
    Int12(       "#int12#", "#int12#", 0, 12,    0),
    Byte(        "byte",    "int8_t",  1,  8,    1),
    Nibble(      "#int4#",  "#int4#",  0,  4,    0),
    Bit(         "#int1#",  "#int1#",  0,  1,    0),
    Boolean(     "boolean", "bool",    0,  1,    0),
    SignedVRT1(  "long",    "int64_t", 0,  0, -100),
    SignedVRT2(  "long",    "int64_t", 0,  0, -100),
    SignedVRT3(  "long",    "int64_t", 0,  0, -100),
    SignedVRT4(  "long",    "int64_t", 0,  0, -100),
    SignedVRT5(  "long",    "int64_t", 0,  0, -100),
    SignedVRT6(  "long",    "int64_t", 0,  0, -100),
    UnsignedVRT1("long",    "int64_t", 0,  0, -100),
    UnsignedVRT2("long",    "int64_t", 0,  0, -100),
    UnsignedVRT3("long",    "int64_t", 0,  0, -100),
    UnsignedVRT4("long",    "int64_t", 0,  0, -100),
    UnsignedVRT5("long",    "int64_t", 0,  0, -100),
    UnsignedVRT6("long",    "int64_t", 0,  0, -100);
    
    
    /** All the available values. */
    public static final DataType[] VALUES = values();
    
    /** All the available values. */
    public static final DataType[] VRT_VALUES = {
      SignedVRT1,   SignedVRT2,   SignedVRT3,   SignedVRT4,   SignedVRT5,   SignedVRT6,
      UnsignedVRT1, UnsignedVRT2, UnsignedVRT3, UnsignedVRT4, UnsignedVRT5, UnsignedVRT6,
    };
    
    final String java;
    final String cpp;
    final int    bytes;
    final int    bits;
    final int    check;
    
    /** Constructor.
     *  @param java  The Java type.
     *  @param cpp   The C++ type.
     *  @param bytes The number of bytes (0 if a VRT type or not an integral number of bytes).
     *  @param bits  The number of bits (-N if a VRT float type).
     *  @param check The type-check code (number of bytes for integer types,
     *               number of bytes * -1 for IEEE-754 float, -100 for VRT float).
     */
    DataType(String java, String cpp, int bytes, int bits, int check) {
      this.java  = java;
      this.cpp   = cpp;
      this.bytes = bytes;
      this.bits  = bits;
      this.check = check;
    }
    
    /** Is this a native type? */
    public boolean isNativeType () {
      return (bytes != 0) && (this != DataType.Int24);
    }
    
    /** Is this a float/double value? */
    public boolean isFloat () {
      return (this == DataType.Double) || (this == DataType.Float);
    }

    /** Gets the applicable mask value. */
    public long getMask () {
      return 0xFFFFFFFFFFFFFFFFL >>> (64 - bits);
    }
    
    /** Gets the applicable type check value value. */
    public int getTypeCheck () {
      return check;
    }
  }
  
  private static enum Mode {
    /** Java source file.                         */ JAVA,
    /** C++ header file (arrays as vectors mode). */ HDR_VECTOR,
    /** C++ source file (arrays as vectors mode). */ CPP_VECTOR,
    /** C++ header file (arrays as pointers).     */ HDR_POINTER,
    /** C++ source file (arrays as pointers).     */ CPP_POINTER;
    
    
    /** Is this for Java code? */
    public boolean isJava () { return (this == JAVA); }
    
    /** Is this for C++ code/header? */
    public boolean isCPP () { return (this != JAVA); }
    
    /** Is this for a source file (not header)? */
    public boolean isSource () { return isJava() || !isHeader(); }
    
    /** Is this for C++ header? */
    public boolean isHeader () {
      return (this == HDR_VECTOR) || (this == HDR_POINTER);
    }
    
    /** Include javadoc/doxygen comments? */
    public boolean isDocumented () { return isJava() || isHeader(); }
    
    /** Gets the appropriate code indentation. */
    public String getIndent () {
           if (isJava()  ) return "  ";
      else if (isSource()) return "";
      else                 return "    ";
    }
  };

  /** Gets the container type appropriate for the given type/format. If either
   *  is a long or uses a floating-point type the container type will be long,
   *  for all others it will be int.
   *  @param t        The data type in the user's array.
   *  @param f        The format type in the buffer.
   *  @param wordSize The minimum word size (-1=n/a).
   *  @return The container type.
   */
  private static DataType getContainerType (DataType t, DataType f, int wordSize) {
    if ((wordSize > 32)) return DataType.Long;
    if ((t == DataType.Double) || (f == DataType.Double)) return DataType.Long;
    if ((t == DataType.Long  ) || (f == DataType.Long  )) return DataType.Long;
    if ((t == DataType.Float ) || (f == DataType.Float )) return DataType.Long;
    return DataType.Int;
  }
  
  /** Gets the mask string. If a container type is specified, the output will be
   *  the intersection of the mask value for the data type and the container type.
   *  @param mode The code output mode.
   *  @param t    The data type (or format) in the user's array.
   *  @param c    The container type (null if n/a).
   *  @param bits The number of bits in the constant value.
   *  @return The mask value.
   */
  private static String getMaskStr (Mode mode, DataType t, DataType c, Integer bits) {
    long mask = (c == null)? t.getMask() : t.getMask() & c.getMask();
    
    if (bits == null) {
      bits = (c == null)? t.bits : c.bits;
    }
    
    if (bits <= 32) {
      return "0x"+toHexString(mask).toUpperCase();
    }
    else if (mode.isJava()) {
      return "0x"+toHexString(mask).toUpperCase()+"L";
    }
    else {
      return "__INT64_C(0x"+toHexString(mask).toUpperCase()+")";
    }
  }
  
  /** Applies the specified variables to a block of code.
   *  @param mode      The code output mode.
   *  @param codeLines The lines of code.
   *  @param t         The data type in the user's array.
   *  @param f         The format type in the buffer.
   */
  private static CharSequence apply (Mode mode, List<String> codeLines, DataType t, DataType f) {
    //return apply(mode, codeLines, t, f, Math.max(t.bits, f.bits));
    return apply(mode, codeLines, t, f, -1);
  }
  
  /** Applies the specified variables to a block of code.
   *  @param mode      The code output mode.
   *  @param codeLines The lines of code.
   *  @param t         The data type in the user's array.
   *  @param f         The format type in the buffer.
   */
  private static CharSequence apply (Mode mode, List<String> codeLines, DataType t, DataType f, int wordSize) {
    DataType     c    = getContainerType(t,f,wordSize);
    DataType     w    = (wordSize <= 32)? DataType.Int : DataType.Long;
    List<String> code = new ArrayList<String>(codeLines.size());
    
    for (String line : codeLines) {
      String ct     = "";             // value for "(#ct#)"
      String word32 = (mode.isJava())? DataType.Int.java  : DataType.Int.cpp;
      //String word64 = (mode.isJava())? DataType.Long.java : DataType.Long.cpp;
      
      // Setting #ct# is necessary since it makes sure that the low-order bits
      // are consistent when doing conversions. Technically this is a "garbage
      // in garbage out" scenario since the values exceed the limits, but we
      // want to maintain some level of consistency. The casting for 'double' is
      // required all the time, the casting for 'float' is only required for
      // 32-bit C++ (not sure why, but GCC 4.4 and Intel 11.1 give us 0 when
      // casting to an int8_t if we don't) but we do it all the time on C++
      // just to be sure.
      if (t == DataType.Double) ct = (mode.isJava())? "("+c.java+")" : "("+c.cpp+")";
      if (t == DataType.Float ) ct = (mode.isJava())? ""             : "("+c.cpp+")";
      
      line = line.replace("#type#",                 (mode.isJava())? t.java : t.cpp)
                 .replace("#form#",                 (mode.isJava())? f.java : f.cpp)
                 .replace("#container#",            (mode.isJava())? c.java : c.cpp)
                 .replace("#word#",                 (mode.isJava())? w.java : w.cpp)
                 .replace("#int24#",                word32)
                 .replace("(#ct#)",                 ct)
                 .replace("#Type#",                 t.toString())
                 .replace("#Form#",                 f.toString())
                 .replace("#Word#",                 w.toString())
                 .replace("#typecheck#",            Integer.toString(t.getTypeCheck()))
                 .replace("#formcheck#",            Integer.toString(f.getTypeCheck()))
                 .replace("#typebytes#",            Integer.toString(t.bytes))
                 .replace("#formbytes#",            Integer.toString(f.bytes))
                 .replace("#wordbytes#",            Integer.toString(w.bytes))
                 .replace("#formbits#",             Integer.toString(f.bits))
                 .replace("#continerbits#",         Integer.toString(c.bits))
                 .replace("#wordbits#",             Integer.toString(w.bits))
                 .replace("#typemask#",             getMaskStr(mode, t, null, 64))
                 .replace("#mask#",                 getMaskStr(mode, f, c, null))
                 .replace("#loop1N#",               "#loop1"+(8-f.bytes)+"#");
      
      if (mode.isJava()) {
        line = line.replaceAll("#loop([0-9])([0-9])#", "");
      }
      else {
        line = line.replaceAll("#loop([0-9])([0-9])#", "_Intel_Pragma(\"loop_count min=$1, max=$2\")");
      }
      
      if (!line.trim().isEmpty()) {
        code.add(line);
      }
    }

    StringBuilder src = new StringBuilder();
    for (String line : preProcess(code)) {
      src.append(line).append('\n');
    }
    return src;
  }
  
  /** Fix a javadoc line with any Java-specific replacements. */
  private static String fixDocJava (Mode mode, String line) {
    return line;
  }
  
  /** Fix a doxygen line with any C++-specific replacements. */
  private static String fixDocCPP (Mode mode, String line) {
    if ((mode == Mode.CPP_POINTER) || (mode == Mode.HDR_POINTER)) {
      // Support passing as buffer via void* pointer
      if (line.contains("@param buf")) {
        line = line.replace("@param buf", "@param ptr") + " pointer";
      }
    }
    return line;
  }

  /** Fix a line with any Java-specific replacements. */
  private static String fixLineJava (Mode mode, boolean first, String line, boolean isInline) {
    return line.replace("const ",                     "")
               .replace("unsigned ",                  "")
               .replace("inline ",                    "")
               .replace("case DataItemFormat_",       "case ")  // not needed in a Java switch statement
               .replace("DataItemFormat_",            "DataItemFormat.")
               .replace("#flatten#",                  "")  // not used in Java
               .replace("#inline_recursive#",         "")  // not used in Java
               .replace("#namespace#",                "")  // not used in Java
               .replace("#JAVA#",                     "true")
               .replace("#CPP#",                      "false")
               .replace("throw IllegalArgumentError", "throw new IllegalArgumentException")
               .replace("throw UnsupportedError",     "throw new UnsupportedOperationException")
               .replaceAll("_loop_count[(](.*)[)]", "");
  }
  
  /** Fix a line with any **generic** C++-specific replacements. */
  private static String fixLineCPP (Mode mode, boolean first, String origLine, boolean isInline) {
    String line = origLine;
    // Replace "#namespace#" with namespace in source, but omit in header; here
    // we use the full "vrt::PackUnpack::" prefix to avoid issues with GCC 3.4.6
    // on RHEL 4 (32-bit).
    String namespace = (mode.isHeader())? "" : "vrt::PackUnpack::";
    
    if (first) {
      if (line.contains("private static ")) namespace = "";
      
      
      line = line.replace("public static ",  ""       )   // default for function in namespace
                 .replace("private static ", "static ")   // "private" function in a file
                 .replace("#namespace#",     namespace);
      
      if ((mode == Mode.CPP_VECTOR) || (mode == Mode.HDR_VECTOR)) {
        // Support passing as buffer via vector<char>
        line = line.replace("byte[] buf", "std::vector<char> &buf");
        // Support passing of other arrays via vector<?>
        int i = line.indexOf('(');
        line = line.substring(0,i).replaceAll("([a-z#]+)\\[\\] ", "std::vector<$1>& ")
             + line.substring( i ).replaceAll("([a-z#]+)\\[\\] ", "std::vector<$1> &");
      }
      else {
        // Support passing as buffer via void* pointer
        line = line.replace("byte[] buf", "void *ptr");
        // Support passing of other arrays via pointers
        int i = line.indexOf('(');
        line = line.substring(0,i).replaceAll("([a-z#]+)\\[\\] ", "$1* ")
             + line.substring( i ).replaceAll("([a-z#]+)\\[\\] ", "$1 *");
      }
      
      // For header file make it end with a ";" rather than "{"
      if (!line.endsWith(" {")) {
        throw new RuntimeException("Error in '"+origLine+"' expected line to end with ' {'");
      }
      if ((mode == Mode.HDR_VECTOR) || (mode == Mode.HDR_POINTER)) {
        line = line.substring(0, line.length()-2) + ";";
      }
    }
    else {
      // Convert "type[] name = new type[len];" to "vector<type> name(len);" or
      // "type *name = new type[len];"
      if ((mode == Mode.CPP_VECTOR) || (mode == Mode.HDR_VECTOR)) {
        line = line.replaceAll("([a-z#]+)\\[\\][ ]+([a-zA-Z0-9_]+)[ ]*=[ ]*new[ ]*[a-z#]+\\[([^\\]]+)\\];",
                               "std::vector<$1> $2($3);");
      }
      else {
        line = line.replaceAll("([a-z#]+)\\[\\][ ]+([a-zA-Z0-9_]+)[ ]*=[ ]*new[ ]*[a-z#]+\\[([^\\]]+)\\];",
                               "$1 *$2 = new $1[$3];");
      }
    }
    
    // Fix standard replacements
    line = line.replaceAll("([ <(#])boolean([# >),])", "$1bool$2")
               .replace(">>>",                        ">>") // unsigned shift
               .replace("#JAVA#",                     "false")
               .replace("#CPP#",                      "true")
               .replace("#flatten#",                  "__attribute__((flatten))")
               .replace("null",                       "NULL")
               .replace("#inline_recursive#",         "")
               .replace("throw IllegalArgumentError", "throw VRTException")
               .replace("throw UnsupportedError",     "throw VRTException")
               .replaceAll("_loop_count[(](.*)[)]",   "_Intel_Pragma(\"$1\")");

    // Fix types (e.g. "long" to "int64_t")
    for (DataType t : DataType.VALUES) {
      line = line.replaceAll("([ <(#])unsigned "+t.java+"([# >),])", "$1u"+t.cpp+"$2")
                 .replaceAll("([ <(#])"+t.java+"([# >),])", "$1"+t.cpp+"$2");
    }

    // Additional changes
    if (first && (mode == Mode.CPP_POINTER) && !isInline) {
      // Support passing as buffer via void* pointer
      if (line.contains("const void *ptr")) {
        line += "\n" + mode.getIndent() + "  const char *buf = (const char*)ptr;";
      }
      else if (line.contains("void *ptr")) {
        line += "\n" + mode.getIndent() + "  char *buf = (char*)ptr;";
      }
    }
    
    return line;
  }
  
  /** Runs a limited pre-processor on the code. This is modeled after the C/C++
   *  preprocessor but only supports the following:
   *  <pre>
   *    conditional_start := "#IF " test optional_comment | "#ELSEIF " test optional_comment
   *    conditional_else  := "#ELSE"  optional_comment
   *    conditional_end   := "#ENDIF" optional_comment
   *
   *    test              := "(" test ")" | "true" | "false"
   *                                      | test_and | test_or | test_eq
   *                                      | test_le | test_lt | test_ge | test_gt
   *    test_and          := integer "&&" integer
   *    test_or           := integer "||" integer
   *    test_eq           := integer "==" integer
   *    test_le           := integer "&lt;=" integer
   *    test_lt           := integer "&lt;" integer
   *    test_ge           := integer "&gt;=" integer
   *    test_gt           := integer "&gt;" integer
   *
   *    integer           := "true" | "false" | REGEX([-+]?[1-9][0-9]+)
   *
   *    optional_comment  := "" | "//" REGEX(.*)
   *  </pre>
   *  Note for the purposes of integer-based tests, "true" and "false" are equal
   *  to "1" and "0" respectively.
   */
  private static List<String> preProcess (List<String> lines) {
    List<String> code  = new ArrayList<String>(lines.size());
    int          state = 0;  // 0=Normal, +1=In Block, -1=Skip Block, +2=Done Block
    
    for (int i = 0; i < lines.size(); i++) {
      String line     = lines.get(i);
      String trimLine = line.trim();
      
      if (trimLine.isEmpty()) continue;

      if (trimLine.startsWith("#ENDIF")) {
        // skip this line, but then start in normal mode
        state = 0;
        continue;
      }
      else if (state > 0) {
        if (trimLine.startsWith("#ELSE")) {
          // skip until #ENDIF found
          state = +2;
          continue;
        }
        else if (state == 1) {
          // process this line
        }
        else {
          continue; // skip until at #ENDIF
        }
      }
      else if (state < 0) {
        if (trimLine.equals("#ELSE")) {
          // skip this line, but then start this block
          state = +1;
          continue;
        }
        else if (trimLine.startsWith("#ELSEIF")) {
          // evaluate conditional (below)
          state = 0;
        }
        else {
          continue; // not at starting point yet
        }
      }
      
      
      if (trimLine.startsWith("#IF") || trimLine.startsWith("#ELSEIF")) {
        line = line.replace("#ELSEIF ", "").replace("#IF ", "")
                   .replaceAll("//.*", "").trim();
        
        if (evaluateTest(line)) {
          // skip this line, but then continue on until end of block
          state = +1;
        }
        else {
          // skip this block
          state = -1;
        }
      }
      else {
        code.add(line);
      }
    }
    return code;
  }
  
  /** Evaluates a pre-processor style conditional. Only used by preProcess(..). */
  private static boolean evaluateTest (String test) {
    test = test.trim();
    
//    if (test.equals("true" )) return true;
//    if (test.equals("false")) return false;
    
    if (test.startsWith("(")) {
      // Handle parenthesis.
      //   Example:
      //     (1 == 1) && true
      //
      //     evaluateTest(evaluateTest("1 == 1") + " " + " && true")
      //     evaluateTest("true" + " " + " && true")
      int level = 1;
      for (int i = 1; i < test.length(); i++) {
        char c = test.charAt(i);
        if (c == ')') {
          level--;
          if (level == 0) {
            return evaluateTest(evaluateTest(test.substring(1,i)) + " " + test.substring(i+1));
          }
        }
        if (c == '(') {
          level++;
        }
      }
      throw new RuntimeException("Could not evaluate conditional '"+test+"'");
    }
    
    String fixString = test.replace("&&", " && ").replace("||", " || ")
                           .replace("==", " == ").replace("!=", " != ")
                           .replace("<",  " <"  ).replace("<=",  "<= ")
                           .replace(">",  " >"  ).replace(">=",  ">= ")
                           .replaceAll("<([^=<])", "< $1")
                           .replaceAll(">([^=>])", "> $1");
    
    String[] tokens = fixString.split("[ ]+");
    
    if (tokens.length == 0) {
      // ERROR
    }
    else if (tokens.length == 1) {
      return toInt(tokens[0]) != 0;
    }
    else if (tokens.length == 2) {
      // ERROR
    }
    else if (tokens.length == 3) {
      if (tokens[1].equals("==")) return  toInt(tokens[0])     ==  toInt(tokens[2]);
      if (tokens[1].equals("!=")) return  toInt(tokens[0])     !=  toInt(tokens[2]);
      if (tokens[1].equals("<=")) return  toInt(tokens[0])     <=  toInt(tokens[2]);
      if (tokens[1].equals(">=")) return  toInt(tokens[0])     >=  toInt(tokens[2]);
      if (tokens[1].equals("<" )) return  toInt(tokens[0])     <   toInt(tokens[2]);
      if (tokens[1].equals(">" )) return  toInt(tokens[0])     >   toInt(tokens[2]);
      if (tokens[1].equals("&&")) return (toInt(tokens[0])!=0) && (toInt(tokens[2])!=0);
      if (tokens[1].equals("||")) return (toInt(tokens[0])!=0) || (toInt(tokens[2])!=0);
      // ERROR
    }
    else if (tokens[1].equals("&&") || tokens[1].equals("||")) {
      if (tokens[1].equals("&&") && !evaluateTest(tokens[0])) return false;
      if (tokens[1].equals("||") &&  evaluateTest(tokens[0])) return true;

      StringBuilder str = new StringBuilder(tokens[2]);
      for (int i = 3; i < tokens.length; i++) {
        str.append(' ').append(tokens[i]);
      }
      return evaluateTest(str.toString());
    }
    throw new RuntimeException("Could not evaluate conditional '"+test+"'");
  }
  
  /** Converts String to int. Only used by evaluateTest(..). */
  private static int toInt (String str) {
    if (str.equals("false")) return 0;
    if (str.equals("true" )) return 1;
    return Integer.parseInt(str);
  }
  
  /** Preprocesses the input code. This handles a number of mode-specific filters. */
  private static Map<String,List<String>> preprocessCode (Mode mode, List<String> lines) {
    LinkedHashMap<String,List<String>> map = new LinkedHashMap<String,List<String>>();

    String       name     = null;  // The section name
    List<String> code     = null;  // The lines of the code to emit
    List<String> comments = null;  // The lines of the javadoc comment
    boolean      first    = true;  // First code line in function?
    boolean      skip     = false; // Skip code for current function?
    boolean      comment  = false; // Currently in a javadoc comment?
    int          lineNum  = 0;     // Source code line number

    for (String origLine : lines) {
      String  trimLine     = origLine.trim();
      String  line         = mode.getIndent() + origLine;
      String  directive    = (trimLine.startsWith("##"))? trimLine.substring(2).trim() : null;
      boolean startComment = trimLine.startsWith("/**");
      boolean isInline     = false; // Is this an inline function?
      lineNum++;
      
      if (trimLine.isEmpty()) continue; // skip blank lines

      if (directive != null) {
        if (directive.startsWith("START ")) {
          name    = directive.substring(6).trim();
          code     = new LinkedList<String>();
          comments = new LinkedList<String>();
        }
        else if (directive.equals("END")) {
          if (name == null) throw new RuntimeException("Error on on line "+lineNum+": END without START");
          if (name.equals("COMMENTS")) {
            map.put(name, comments);
          }
          else {
            map.put(name, code);
          }
          name     = null;
          code     = null;
          comments = null;
        }
        first = true;
        skip  = false;
      }
      else if (comment || startComment) {
        if (startComment) comments.clear();
        
             if (mode.isJava()) line = fixDocJava(mode, line);
        else if (mode.isCPP() ) line = fixDocCPP( mode, line);
        comments.add(line);
        
        comment = !trimLine.contains("*/");
        first   = true;
      }
      else if (skip) {
        // skip this block (e.g. static function in header)
      }
      else {
        boolean annotation   = trimLine.startsWith("@");
        boolean preprocessor = trimLine.startsWith("#if")
                            || trimLine.startsWith("#else")
                            || trimLine.startsWith("#endif")
                            || trimLine.startsWith("#pragma")
                            || trimLine.startsWith("#define");
        boolean isPrivate    = trimLine.contains("private ");
        
        // ---- Check to see if it should be skipped in the header -------------
        if (first && !annotation && !preprocessor) {
          if (isPrivate && !mode.isSource()) {
            // totally ignore it in the header
            skip = true;
            continue;
          }
          else if (isPrivate) {
            // document it in the source file
            code.addAll(comments);
          }
          else if (mode.isDocumented()) {
            code.addAll(comments);
          }
        }
        
        if (first) {
          isInline = line.contains("inline ");
        }
        
        // ---- Make required mods to the line ---------------------------------
             if (annotation   ) { } // special case - skip changes
        else if (preprocessor ) { } // special case - skip changes
        else if (mode.isJava()) line = fixLineJava(mode, first, line, isInline);
        else if (mode.isCPP() ) line = fixLineCPP( mode, first, line, isInline);
        
        // ---- Write the line out (as required) -------------------------------
        if (annotation) {
          // Put in comments so it goes just before any code we include
          if (mode.isJava()) comments.add(line);
        }
        else if (preprocessor) {
          if (mode.isCPP()) code.add(line);
        }
        else if (mode.isSource()) {
          code.add(line);
        }
        else if (mode.isHeader()) {
          if (first) code.add(line);
        }
        else {
          throw new AssertionError("This should be impossible");
        }
        first = (first && annotation);
      }
    }
    return map;
  }
  
  /** Prints out the code for the various pack/unpack functions.
   *  @param out    The stream to print to.
   *  @param mode   The mode being used.
   *  @param code   The template code as read in.
   */
  private static void printFunctions (PrintStream out, Mode mode, Map<String,List<String>> code) throws Exception {
    for (DataType t : DataType.VALUES) {
      if (!t.isNativeType()) continue;
      out.print(apply(mode, code.get("FPOINT" ), t, DataType.Double));
      out.print(apply(mode, code.get("FPOINT" ), t, DataType.Float));
      out.print(apply(mode, code.get("INTEGER"), t, DataType.Long));
      out.print(apply(mode, code.get("INTEGER"), t, DataType.Int));
      out.print(apply(mode, code.get("INTEGER"), t, DataType.Int24));
      out.print(apply(mode, code.get("INTEGER"), t, DataType.Short));
      out.print(apply(mode, code.get("INT12S" ), t, DataType.Int12));
      out.print(apply(mode, code.get("INTEGER"), t, DataType.Byte));
      out.print(apply(mode, code.get("NIBBLES"), t, DataType.Nibble));
      out.print(apply(mode, code.get("BITS"   ), t, DataType.Bit));
      out.print(apply(mode, code.get("WORD"   ), t, DataType.Int,          32));
      out.print(apply(mode, code.get("WORD"   ), t, DataType.Int,          64));
      out.print(apply(mode, code.get("WORD"   ), t, DataType.Float,        64));
      out.print(apply(mode, code.get("BUFFER" ), t, DataType.Int,          32));
      out.print(apply(mode, code.get("BUFFER" ), t, DataType.Int,          64));
      out.print(apply(mode, code.get("BUFFER" ), t, DataType.Float,        64));
      for (DataType f : DataType.VRT_VALUES) {
        out.print(apply(mode, code.get("BUFFER" ), t, f, 32));
        out.print(apply(mode, code.get("BUFFER" ), t, f, 64));
      }
      out.print(apply(mode, code.get("GENERIC"), t, DataType.Byte));
    }
  }

  @SuppressWarnings("UseOfSystemOutOrSystemErr")
  public static void main (String[] args) throws Exception {
    // Note that we are using:
    //    new PrintStream(new BufferedOutputStream(new FileOutputStream(...)));
    // rather than:
    //    new PrintStream(...);
    // as the later version can be extremely slow on some systems.
    
    InputStream    is = PackUnpackGen.class.getClassLoader().getResourceAsStream("nxm/vrt/dat/PackUnpackGen.code");
    BufferedReader in = new BufferedReader(new InputStreamReader(is));
    LinkedList<String> inputLines = new LinkedList<String>();
    String line;
    while ((line = in.readLine()) != null) {
      inputLines.add(line);
    }
    in.close();

    if ((args.length == 2) && args[0].equals("--java")) {
      Mode                     mode = Mode.JAVA;
      Map<String,List<String>> code = preprocessCode(mode, inputLines);
      PrintStream out = new PrintStream(new BufferedOutputStream(new FileOutputStream(args[1])));

      out.print("//\n");
      out.print("//  THIS FILE IS AUTO-GENERATED, DO NOT MODIFY\n");
      out.print("//\n");
      out.print("package nxm.vrt.lib;\n");
      out.print("\n");
      out.print("import nxm.vrt.lib.VRTPacket.DataItemFormat;\n");
      out.print("import nxm.vrt.lib.VRTPacket.PayloadFormat;\n");
      out.print("\n");
      out.print("import static java.lang.Double.doubleToRawLongBits;\n");
      out.print("import static java.lang.Double.longBitsToDouble;\n");
      out.print("import static java.lang.Float.floatToRawIntBits;\n");
      out.print("import static java.lang.Float.intBitsToFloat;\n");
      out.print("import static nxm.vrt.lib.VRTMath.*;\n");
      out.print("\n");
      for (String c : code.get("COMMENTS")) {
        c = c.substring(mode.getIndent().length());
        out.print(c+"\n");
      }
      out.print("public final class PackUnpack {\n");
      out.print("  private PackUnpack () { } // prevents instantiation\n");
      out.print("\n");
      printFunctions(out, mode, code);
      out.print("}\n");
      out.close();
    }
    else if ((args.length == 2) && args[0].equals("--cpp")) {
      Mode                     mode = Mode.CPP_POINTER;
      Map<String,List<String>> code = preprocessCode(mode, inputLines);
      PrintStream out = new PrintStream(new BufferedOutputStream(new FileOutputStream(args[1])));

      out.print("//\n");
      out.print("//  THIS FILE IS AUTO-GENERATED, DO NOT MODIFY\n");
      out.print("//\n");
      out.print("#include \"PackUnpack.h\"\n");
      out.print("\n");
      out.print("using namespace std;\n");
      out.print("using namespace vrt;\n");
      out.print("using namespace vrt::VRTMath;\n");
      out.print("using namespace vrt::Utilities;\n");
      out.print("\n");
      printFunctions(out, mode, code);
      out.print("\n");
      out.close();
    }
    else if ((args.length == 2) && args[0].equals("--hdr")) {
      Mode                     mode = Mode.HDR_POINTER;
      Map<String,List<String>> code = preprocessCode(mode, inputLines);
      PrintStream out = new PrintStream(new BufferedOutputStream(new FileOutputStream(args[1])));

      out.print("//\n");
      out.print("//  THIS FILE IS AUTO-GENERATED, DO NOT MODIFY\n");
      out.print("//\n");
      out.print("#ifndef _PackUnpack_h\n");
      out.print("#define _PackUnpack_h\n");
      out.print("\n");
      out.print("#include \"VRTObject.h\"\n");
      out.print("#include \"VRTMath.h\"\n");
      out.print("#include \"BasicVRTPacket.h\"\n");
      out.print("#include <vector>\n");
      out.print("\n");
      out.print("using namespace std;\n");
      out.print("using namespace vrt;\n");
      out.print("\n");
      out.print("namespace vrt {\n");
      for (String c : code.get("COMMENTS")) {
        c = c.substring(mode.getIndent().length());
        out.print("  "+c+"\n");
      }
      out.print("  namespace PackUnpack {\n");
      printFunctions(out, mode, code);
      out.print("  };\n");
      out.print("};\n");
      out.print("#endif /* _PackUnpack_h */\n");
      out.print("\n");
      out.close();      
    }
    else {
      System.err.println("ERROR: expected --java, --cpp, or --hdr");
    }
  }
}

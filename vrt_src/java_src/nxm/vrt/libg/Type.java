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

import nxm.vrt.lib.TimeStamp;
import nxm.vrt.lib.VRTPacket;
import java.util.Map;

import static nxm.vrt.lib.Utilities.asMap;
import static java.util.Collections.unmodifiableMap;
import static nxm.vrt.libg.PacketGen.getLengthSpecifier;

/**  Defines a type that can be in the packet.
 *
 *  @author 
 */
interface Type {
  static final int  STAR_LENGTH = Integer.MIN_VALUE;


  static final Type BYTE     = new BasicType("byte",     true,  1, "$0$",    "byte",                 "int8_t" );
  static final Type SHORT    = new BasicType("short",    true,  2, "$0$",    "short",                "int16_t");
  static final Type INT      = new BasicType("int",      true,  4, "$0$",    "int",                  "int32_t");
  static final Type LONG     = new BasicType("long",     true,  8, "$0$",    "long",                 "int64_t");
  static final Type FLOAT    = new BasicType("float",    true,  4, "$0.0$",  "float",                "float"  );
  static final Type DOUBLE   = new BasicType("double",   true,  8, "$0.0$",  "double",               "double" );
  static final Type BOOLEAN  = new BasicType("boolean",  false, 1, null,     "boolean",              "bool"   );
  static final Type BOOLNULL = new BasicType("boolNull", false, 1, "$null$", "java.lang.Boolean",    "boolNull");
  static final Type INETADDR = new BasicType("inetAddr", false, 16, null,    "java.net.InetAddress", "InetAddress");
  static final Type UUID     = new BasicType("uuid",     false, 16, null,    "java.util.UUID",       "UUID");
  static final Type UTCTIME  = new TimeStampType("utcTime", TimeStamp.IntegerMode.UTC, "$1970-01-01T00:00:00Z$");
  static final Type GPSTIME  = new TimeStampType("gpsTime", TimeStamp.IntegerMode.GPS, "$1980-01-06T00:00:00Z$");

  /**  The basic types indexed by name. */
  static final Map<String,Type> BASIC_TYPES = unmodifiableMap(asMap(String.class, Type.class,
                                                                    "byte",     BYTE,      "short",    SHORT,
                                                                    "int",      INT,       "long",     LONG,
                                                                    "float",    FLOAT,     "double",   DOUBLE,
                                                                    "boolean",  BOOLEAN,   "boolNull", BOOLNULL,
                                                                    "inetAddr", INETADDR,  "uuid",     UUID,
                                                                    "utcTime",  UTCTIME,   "gpsTime",  GPSTIME ));
  /**  Is this an array? */
  public boolean isArrayType ();

  /**  Is this an ascii type? */
  public boolean isNumericType ();

  /**  Is this an ASCII type? */
  public boolean isAsciiType ();

  /**  Is this a UTF-8 type? */
  public boolean isUTF8Type ();

  /** Is this an ASCII type (string form)? */
  public boolean isAsciiTypeStr ();

  /** Is this a UTF-8 type (string form)? */
  public boolean isUTF8TypeStr ();

  /**  Is this a string (ASCII or UTF8) type? */
  public boolean isStringType ();

  /**  Is this a metadata type? */
  public boolean isMetadataType ();

  /**  Gets the name of the type. */
  public String getTypeName ();

  /**  Gets the number of bytes taken by the type (-1 for variable). */
  public int getTypeBytes ();

  /**  Gets the default value for the type. */
  public String getTypeDefault ();

  /**  Gets the C++ name of the type as used in an accessor. */
  public String getTypeCPPGet ();

  /**  Gets the C++ name of the type as used in an modifier. */
  public String getTypeCPPSet ();

  /**  Gets the Java name of the type as used in an accessor. */
  public String getTypeJavaGet ();

  /**  Gets the Java name of the type as used in an modifier. */
  public String getTypeJavaSet ();

  /**  Gets the array length (-1 for variable, -2 if not an array). */
  public int getArrayLength ();

  /**  Gets the array type (null if not an array). */
  public Type getArrayType ();


  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Some Defined Types
  //////////////////////////////////////////////////////////////////////////////////////////////////

  class PrecompiledType implements Type {
    final Class<?> clazz;

    public PrecompiledType (Class<?> clazz) {
      this.clazz = clazz;
    }

    @Override public String  toString ()       { return clazz.getName(); }
    @Override public boolean isArrayType ()    { return false; }
    @Override public boolean isAsciiType ()    { return false; }
    @Override public boolean isUTF8Type ()     { return false; }
    @Override public boolean isAsciiTypeStr () { return false; }
    @Override public boolean isUTF8TypeStr ()  { return false; }
    @Override public boolean isStringType ()   { return false; }
    @Override public boolean isMetadataType () { return false; }
    @Override public boolean isNumericType ()  { return false; }
    @Override public String  getTypeName ()    { return "/"+clazz.getSimpleName()+"/"; }
    @Override public String  getTypeDefault () { return null; }
    @Override public String  getTypeCPPGet ()  { return clazz.getSimpleName(); }
    @Override public String  getTypeCPPSet ()  { return clazz.getSimpleName(); }
    @Override public String  getTypeJavaGet () { return clazz.getName(); }
    @Override public String  getTypeJavaSet () { return clazz.getName(); }
    @Override public int     getArrayLength () { return -2; }
    @Override public Type    getArrayType ()   { return null; }

    @Override
    public int getTypeBytes () {
      if (clazz.isInstance(Enum.class)) {
        return 1; // enumerations always use a byte for storage (at least for now)
      }
      else if (clazz.isInstance(nxm.vrt.lib.Record.class)) {
        try {
          return clazz.getField("RECORD_LENGTH").getInt(null);
        }
        catch (Exception e) {
          throw new AssertionError("Could not access RECORD_LENGTH in "+clazz+": "+e);
        }
      }
      else if (clazz.isInstance(VRTPacket.class)) {
        try {
          return clazz.getField("PACKET_LENGTH").getInt(null);
        }
        catch (Exception e) {
          throw new AssertionError("Could not access PACKET_LENGTH in "+clazz+": "+e);
        }
      }
      throw new AssertionError("Unknown type: "+clazz);
    }
  }

  
  class BasicType implements Type {
    final String  name;
    final boolean numeric;
    final int     bytes;
    final String  defVal;
    final String  cppGet;
    final String  cppSet;
    final String  javaGet;
    final String  javaSet;

    public BasicType (String name, boolean numeric, int bytes, String defVal, String javaType, String cppType) {
      this(name, numeric, bytes, defVal, javaType, javaType, cppType, cppType);
    }

    public BasicType (String name, boolean numeric, int bytes, String defVal,
                      String javaGet, String javaSet, String cppGet, String cppSet) {
      super();
      this.name    = name;
      this.numeric = numeric;
      this.bytes   = bytes;
      this.defVal  = defVal;
      this.cppGet  = cppGet;
      this.cppSet  = cppSet;
      this.javaGet = javaGet;
      this.javaSet = javaSet;
    }

    @Override public String  toString ()       { return name; }
    @Override public boolean isArrayType ()    { return false; }
    @Override public boolean isAsciiType ()    { return false; }
    @Override public boolean isUTF8Type ()     { return false; }
    @Override public boolean isAsciiTypeStr () { return false; }
    @Override public boolean isUTF8TypeStr ()  { return false; }
    @Override public boolean isStringType ()   { return isAsciiType()    || isUTF8Type()
                                                     || isAsciiTypeStr() || isUTF8TypeStr(); }
    @Override public boolean isMetadataType () { return false; }
    @Override public boolean isNumericType ()  { return numeric; }
    @Override public String  getTypeName ()    { return name; }
    @Override public int     getTypeBytes ()   { return bytes; }
    @Override public String  getTypeDefault () { return defVal; }
    @Override public String  getTypeCPPGet ()  { return cppGet; }
    @Override public String  getTypeCPPSet ()  { return cppSet; }
    @Override public String  getTypeJavaGet () { return javaGet; }
    @Override public String  getTypeJavaSet () { return javaSet; }
    @Override public int     getArrayLength () { return -2; }
    @Override public Type    getArrayType ()   { return null; }
  }

  /**  Special type for a VRT TimeStamp. */
  static class TimeStampType extends BasicType {
    final TimeStamp.IntegerMode epoch;

    public TimeStampType (String name, TimeStamp.IntegerMode epoch, String defVal) {
      super(name, false, 12, defVal, "nxm.vrt.lib.TimeStamp", "TimeStamp");
      this.epoch = epoch;
    }
  }

  /**  Special type for an ASCII string. */
  static class AsciiType extends BasicType {
    private final AsciiTypeString str;

    public AsciiType (int count) {
      super((count < 0)? "ascii[*]" : "ascii["+count+"]",
            false,
            count,
            "$``''$",
            "nxm.vrt.lib.AsciiCharSequence",
            "java.lang.CharSequence",
            "string",
            "string");
      this.str = new AsciiTypeString(count);
    }

    public AsciiType (String count) {
      this(getLengthSpecifier(count));
    }

    @Override
    public boolean isAsciiType () {
      return true;
    }

    public Type asString () {
      return str;
    }
  }

  /** Special type for an ASCII string. */
  static class AsciiTypeString extends BasicType {
    public AsciiTypeString (int count) {
      super((count < 0)? "ascii[*]" : "ascii["+count+"]",
            false,
            count,
            "$``''$",
            "java.lang.String",
            "java.lang.CharSequence",
            "string",
            "string");
    }

    @Override
    public boolean isAsciiTypeStr () {
      return true;
    }
  }

  /**  Special type for an UTF-8 string. */
  static class UTF8Type extends BasicType {
    private final UTF8TypeString str;

    public UTF8Type (int count) {
      super((count < 0)? "utf8[*]" : "utf8["+count+"]",
            false,
            count,
            "$``''$",
            "java.lang.CharSequence",
            "java.lang.CharSequence",
            "wstring",
            "wstring");
      this.str = new UTF8TypeString(count);
    }

    public UTF8Type (String count) {
      this(getLengthSpecifier(count));
    }

    @Override
    public boolean isUTF8Type () {
      return true;
    }

    public Type asString () {
      return str;
    }
  }

  /** Special type for an UTF-8 string. */
  static class UTF8TypeString extends BasicType {
    public UTF8TypeString (int count) {
      super((count < 0)? "utf8[*]" : "utf8["+count+"]",
            false,
            count,
            "$``''$",
            "java.lang.String",
            "java.lang.CharSequence",
            "string",
            "string");
    }

    @Override
    public boolean isUTF8TypeStr () {
      return true;
    }
  }

  /**  Special type for a metadata block. */
  static class MetadataType extends BasicType {
    public MetadataType (int count) {
      super((count < 0)? "meta[*]" : "meta["+count+"]",
            false,
            count,
            "$``''$",
            "nxm.vrt.lib.MetadataBlock",
            "nxm.vrt.lib.MetadataBlock",
            "MetadataBlock",
            "MetadataBlock");
    }

    public MetadataType (String count) {
      this(getLengthSpecifier(count));
    }

    @Override
    public boolean isMetadataType () {
      return true;
    }
  }

  /**  Special type for an array. */
  static class ArrayType extends BasicType {
    final Type type;
    final int  count;

    public ArrayType (Type type, int count) {
      super(type.getTypeName() + ((count < 0)? "[*]" : "["+count+"]"),
            false,
            type.getTypeBytes() * count,
            type.getTypeDefault(),
            null,
            null,
            null,
            null);
      this.type  = type;
      this.count = count;
    }

    public ArrayType (Type type, String count) {
      this(type, getLengthSpecifier(count));
    }
    
    @Override public boolean isArrayType ()  { return true; }
    @Override public Type    getArrayType () { return type; }
    @Override public int     getTypeBytes () { return (count < 0)? count : count*type.getTypeBytes(); }
  }

}

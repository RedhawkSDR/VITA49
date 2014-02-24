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
import java.util.List;

/** A unit in the Nomenclature & Units section.
 *
 *  @author 
 */
class PacketFactory extends Definition {
  final List<PacketType> packetTypes;
  final String                 name;
  final String                 author;
  
  public PacketFactory (List<PacketType> packetTypes) {
    this.packetTypes = packetTypes;
    this.name        = "PacketFactory";
    this.author      = null;
  }

  @Override
  public String toString () {
    return "PacketTypes: packetTypes='"+packetTypes+"'";
  }

  @Override
  public void writeJava (PacketGen outParams, PrintStream out) {
    out.println("//");
    out.println("//  This class was auto-generated, do not modify.");
    out.println("//");
    out.println("package " + outParams.javaPackage + ";");
    out.println("");
    out.println("/** Packet factory for auto-generated classes.");
    out.println(" *  ");
    out.println(" *  @author " + author);
    out.println(" */");
    out.println("public class " + name + " extends nxm.vrt.lib.AbstractPacketFactory {");
    out.println("  ");
    out.println("  /** Creates a new instance with no parent class. */");
    out.println("  public " + name + " () {");
    out.println("    super();");
    out.println("  }");
    out.println("  ");
    out.println("  /** Creates a new instance with a parent class. */");
    out.println("  public " + name + " (nxm.vrt.lib.PacketFactory parent) {");
    out.println("    super(parent);");
    out.println("  }");
    out.println("  ");
    out.println("  @Override");
    out.println("  public java.lang.Class<? extends nxm.vrt.lib.VRTPacket> getPacketClass (nxm.vrt.lib.VRTPacket.PacketType type, java.lang.Long id) {");
    out.println("    if (id == null) return super.getPacketClass(type, id);");
    out.println("    long classIdentifier = id;");
    out.println("    ");
    out.println("    if (type.isData()) {");
    for (PacketType pt : packetTypes) {
      if (pt.type.isData()) {
        out.println("      if (classIdentifier == "+pt.name+".CLASS_IDENTIFIER) return "+pt.name+".class;");
      }
    }
      out.println("    }");
    out.println("    else {");
    for (PacketType pt : packetTypes) {
      if (!pt.type.isData()) {
        out.println("      if (classIdentifier == "+pt.name+".CLASS_IDENTIFIER) return "+pt.name+".class;");
      }
    }
    out.println("    }");
    out.println("    return super.getPacketClass(type, id);");
    out.println("  }");
    out.println("  ");
    out.println("  @Override");
    out.println("  protected nxm.vrt.lib.VRTPacket getPacket (java.lang.Class<? extends nxm.vrt.lib.VRTPacket> clazz,");
    out.println("                                             nxm.vrt.lib.VRTPacket.PacketType type, java.lang.Long id,");
    out.println("                                             nxm.vrt.lib.VRTPacket p, boolean copy) {");
    out.println("    if (p == null) {");
    for (PacketType pt : packetTypes) {
      out.println("      if (clazz == "+pt.name+".class) return new "+pt.name+"();");
    }
    out.println("    }");
    out.println("    else {");
    for (PacketType pt : packetTypes) {
      out.println("      if (clazz == "+pt.name+".class) return new "+pt.name+"(nxm.vrt.lib.BasicVRTPacket.toBasicVRTPacket(p), copy);");
    }
    out.println("    }");
    out.println("    return super.getPacket(clazz, type, id, p, copy);");
    out.println("  }");
    out.println("}");
  }

  @Override
  public void writeCPP (PacketGen outParams, PrintStream hdr, PrintStream cpp) {
    hdr.println("//");
    hdr.println("//  This class was auto-generated, do not modify.");
    hdr.println("//");
    hdr.println("#ifndef _"+name+"_h");
    hdr.println("#define _"+name+"_h");
    hdr.println("");
    hdr.println("#include \"AbstractPacketFactory.h\"");
    hdr.println("#include \"BasicVRTPacket.h\"");
    hdr.println("");
    hdr.println("using namespace std;");
    hdr.println("using namespace vrt;");
    hdr.println("");
    hdr.println("namespace " + outParams.cppNamespace + " {");
    hdr.println("  ");
    hdr.println("  /** Packet factory for auto-generated classes.");
    hdr.println("   *  ");
    hdr.println("   *  @author " + author);
    hdr.println("   */");
    hdr.println("  class " + name + ": public AbstractPacketFactory {");
    hdr.println("    ");
    hdr.println("    /** Destructor for the class. */");
    hdr.println("    public: ~" + name + " ();");
    hdr.println("    ");
    hdr.println("    /** Creates a new instance with no parent class. */");
    hdr.println("    public: " + name + " ();");
    hdr.println("    ");
    hdr.println("    /** Creates a new instance with a parent class. */");
    hdr.println("    public: " + name + " (AbstractPacketFactory *parent);");
    hdr.println("    ");
    hdr.println("    protected: virtual BasicVRTPacket *getPacket (PacketType type, int64_t id, const BasicVRTPacket *p) const;");
    hdr.println("  };");
    hdr.println("};");
    hdr.println("");
    hdr.println("#endif /* _"+name+"_h */");
    hdr.println("");


    cpp.println("//");
    cpp.println("//  This class was auto-generated, do not modify.");
    cpp.println("//");
    cpp.println("#include \""+name+".h\"");
    cpp.println("#include \"BasicVRTPacket.h\"");
    for (PacketType pt : packetTypes) {
        cpp.println("#include \""+pt.name+".h\"");
      }
    cpp.println("");
    cpp.println("using namespace std;");
    cpp.println("using namespace vrt;");
    if (!outParams.cppNamespace.equals("vrt")) {
      cpp.println("using namespace "+outParams.cppNamespace+";");
    }
    cpp.println("");
    cpp.println("" + name + "::~" + name + " () {");
    cpp.println("  // done");
    cpp.println("}");
    cpp.println("");
    cpp.println("" + name + "::" + name + " () :");
    cpp.println("  AbstractPacketFactory()");
    cpp.println("{");
    cpp.println("  // done");
    cpp.println("}");
    cpp.println("");
    cpp.println("" + name + "::" + name + " (AbstractPacketFactory *parent) :");
    cpp.println("  AbstractPacketFactory(parent)");
    cpp.println("{");
    cpp.println("  // done");
    cpp.println("}");
    cpp.println("");
    cpp.println("BasicVRTPacket *" + name + "::getPacket (PacketType type, int64_t id, const BasicVRTPacket *p) const {");
    cpp.println("  if (p == NULL) {");
    cpp.println("    if ((type == PacketType_UnidentifiedData   ) || (type == PacketType_Data   ) ||");
    cpp.println("        (type == PacketType_UnidentifiedExtData) || (type == PacketType_ExtData)) {");
    for (PacketType pt : packetTypes) {
      if (pt.type.isData()) {
        cpp.println("      if (id == "+pt.name+"::CLASS_IDENTIFIER) return new "+pt.name+";");
      }
    }
      cpp.println("    }");
    cpp.println("    else {");
    for (PacketType pt : packetTypes) {
      if (!pt.type.isData()) {
        cpp.println("      if (id == "+pt.name+"::CLASS_IDENTIFIER) return new "+pt.name+";");
    }
    }
    cpp.println("    }");
    cpp.println("  }");
    cpp.println("  else {");
    cpp.println("    if ((type == PacketType_UnidentifiedData   ) || (type == PacketType_Data   ) ||");
    cpp.println("        (type == PacketType_UnidentifiedExtData) || (type == PacketType_ExtData)) {");
    for (PacketType pt : packetTypes) {
      if (pt.type.isData()) {
        cpp.println("      if (id == "+pt.name+"::CLASS_IDENTIFIER) return new "+pt.name+"(*p);");
      }
    }
      cpp.println("    }");
    cpp.println("    else {");
    for (PacketType pt : packetTypes) {
      if (!pt.type.isData()) {
        cpp.println("      if (id == "+pt.name+"::CLASS_IDENTIFIER) return new "+pt.name+"(*p);");
    }
    }
    cpp.println("    }");
    cpp.println("  }");
    cpp.println("  return AbstractPacketFactory::getPacket(type, id, p);");
    cpp.println("}");
    cpp.println("");
    cpp.println("/** Creates a new instance of the factory via a C-linkable method. This method is");
    cpp.println(" *  used for dynamically creating an instance of the class at run-time; this method");
    cpp.println(" *  should never be called directly.");
    cpp.println(" */");
    cpp.println("extern \"C\"");
    cpp.println("AbstractPacketFactory* _new_"+outParams.cppNamespace.replace("::","__")+"__"+name+"_0 () {");
    cpp.println("  return new "+name+";");
    cpp.println("}");
    cpp.println("");
    cpp.println("/** Creates a new instance of the factory via a C-linkable method. This method is");
    cpp.println(" *  used for dynamically creating an instance of the class at run-time; this method");
    cpp.println(" *  should never be called directly.");
    cpp.println(" *  @param parent The parent packet factory.");
    cpp.println(" */");
    cpp.println("extern \"C\"");
    cpp.println("AbstractPacketFactory* _new_"+outParams.cppNamespace.replace("::","__")+"__"+name+"_1 (AbstractPacketFactory *parent) {");
    cpp.println("  return new "+name+"(parent);");
    cpp.println("}");
    cpp.println("");
  }
}

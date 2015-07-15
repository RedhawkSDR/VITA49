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
import java.util.*;
import org.jdom.*;
import org.jdom.input.SAXBuilder;


/**  Documents the packets.
 *
 *  @author         
 */
public class Schemapath extends Intrinsic {
  enum Type { ALL, ANY, ATTRIBUTE, ATTRIBUTE_GROUP, CHOICE, COMPLEX_CONTENT, COMPLEX_TYPE,
              ELEMENT, ENUMERATION, EXTENSION, GROUP, LIST, OPEN_CONTENT, RESTRICTION,
              SEQUENCE, SIMPLE_CONTENT, SIMPLE_TYPE, UNION };

  private static final char MIN_INCLUSIVE = '['; // Flag for inclusive use of minValue
  private static final char MAX_INCLUSIVE = ']'; // Flag for inclusive use of maxValue
  private static final char MIN_EXCLUSIVE = '('; // Flag for exclusive use of minValue
  private static final char MAX_EXCLUSIVE = ')'; // Flag for exclusive use of maxValue

  private static final Comparator<Field> ALPHABETICAL = new Comparator<Field>() {
    @Override
    public int compare (Field a, Field b) {
      if (a.name == b.name) return 0;
      if (a.name == null  ) return -1;
      if (b.name == null  ) return +1;

      return a.name.compareTo(b.name);
    }
  };

  private TextFile            in;
  private String              tag;
  private TextFile            out;
  private Map<String,Element> elements        = new LinkedHashMap<String,Element>();
  private Map<String,Element> attributeGroups = new LinkedHashMap<String,Element>();
  private Map<String,Element> definedTypes    = new LinkedHashMap<String,Element>();
  private List<Field>         fields          = new LinkedList<Field>();
  
  @Override
  public int process() {
    try {
      // INITIALIZE
      in  = MA.getTextFile("IN");
      tag = MA.getCS("TAG");
      out = MA.getTextFile("OUT");

      in.open(TextFile.INPUT|TextFile.FORCEABORT);
      out.open(TextFile.OUTPUT|TextFile.FORCEABORT);

      SAXBuilder builder  = new SAXBuilder();
      Document   document = builder.build(in.getResource().getInputStream());
      Element    e        = document.getRootElement();
      in.close();

      for (Object child : e.getChildren()) {
        Element c = (Element)child;
        String  n = c.getQualifiedName();
        if (n.equals("xs:element")) {
          elements.put(c.getAttributeValue("name"), c);
        }
        else if (n.equals("xs:attributeGroup")) {
          attributeGroups.put(c.getAttributeValue("name"), c);
        }
        else if (n.equals("xs:simpleType") || n.equals("xs:complexType")) {
          definedTypes.put(c.getAttributeValue("name"), c);
        }
        else {
          throw new MidasException("Unknown tag <"+c.getQualifiedName()+"/> in "+in.getName());
        }
      }

      // PROCESS
      boolean all = tag.equals("*");
      String  t   = "|"+tag.replace(',','|')+"|";
      for (Element c : elements.values()) {
        String n = c.getAttributeValue("name");
        if (all || t.contains("|"+n+"|")) {
          Field v = new Field();
          fields.add(v);
          processChild(Type.SEQUENCE, v, e, c);
        }
      }

      if (MA.getState("/SORT")) {
        Collections.sort(fields, ALPHABETICAL);
      }

      // OUTPUT
      for (Field f : fields) {
//        if (f.type != null) {
          println(f.getFieldDef());
          println(f.getXPathDef());
          println("");
//        }
      }

      println("");
      Map<Field,List<EnumValue>> enums = new LinkedHashMap<Field,List<EnumValue>>();
      for (Field f : fields) {
        if ((f.type != null) && (f.enumValues.size() > 0) && !enums.containsValue(f.enumValues)) {
          enums.put(f, f.enumValues);
        }
      }
      for (Field f : enums.keySet()) {
        println("");
        println(f.xpath);
        println(f.getEnumDef());
      }

      // DONE
      out.close();
      return FINISH;
    }
    catch (RuntimeException e) {
      throw e;
    }
    catch (Exception e) {
      throw new MidasException("Error in processing.", e);
    }
  }

  /**  Finds a child in a given element. */
  private Element findChild (Element e, String qualifiedName) {
    for (Object child : e.getChildren()) {
      Element c = (Element)child;
      if (c.getQualifiedName().equals(qualifiedName)) {
        return c;
      }
    }
    return null;
  }

  /**  Process all children of the given element. */
  private void processChildren (Type t, Field v, Element e) {
    for (Object child : e.getChildren()) {
      processChild(t, v, e, (Element)child);
    }
  }

  /**  Process a single specified children of the given element. */
  private void processChild (Type t, Field f, Element e, Element c) {
    if (t == Type.CHOICE         ) t = Type.SEQUENCE;       // below here, they are identical
    if (t == Type.UNION          ) t = Type.LIST;           // below here, they are identical
    if (t == Type.COMPLEX_CONTENT) t = Type.SIMPLE_CONTENT; // below here, they are identical

    String n = c.getQualifiedName();

    if (n.equals("xs:all") && (t==Type.GROUP || t==Type.COMPLEX_TYPE)) {
      processChildren(Type.ALL, f, c);
    }
    else if (n.equals("xs:alternative") && (t==Type.ELEMENT)) {
      // tbd
    }
    else if (n.equals("xs:annotation")) {
      Element d = findChild(c, "xs:documentation");
      if (d != null) {
        String documentation = d.getTextNormalize();
        if (documentation.length() > 0){
          if (t == Type.ENUMERATION) {
            f.enumValues.get(f.enumValues.size()-1).docs = documentation;
          }
          else if ((f.docs != null) && (f.docs.length() > 0) && !documentation.equals(f.docs)) {
            f.docs = f.docs + " :: " + documentation;
          }
          else {
            f.docs = documentation;
          }
        }
      }
    }
    else if (n.equals("xs:any") && (t==Type.SEQUENCE || t==Type.GROUP || t==Type.OPEN_CONTENT)) {
      processChildren(Type.ANY, f, c);
    }
    else if (n.equals("xs:anyAttribute") && (t==Type.COMPLEX_TYPE || t==Type.EXTENSION || t==Type.RESTRICTION || t==Type.ATTRIBUTE_GROUP)) {
      // tbd
    }
    else if (n.equals("xs:assert") && (t==Type.COMPLEX_TYPE || t==Type.EXTENSION || t==Type.RESTRICTION)) {
      // tbd
    }
    else if (n.equals("xs:attribute") && (t==Type.COMPLEX_TYPE || t==Type.EXTENSION || t==Type.RESTRICTION || t==Type.ATTRIBUTE_GROUP)) {
      String name = c.getAttributeValue("name");
      String type = c.getAttributeValue("type");
      Field   fld = new Field(f,name,false);
      Element typ = (type == null)? null : definedTypes.get(type);

      fields.add(fld);
      // ignore ref, targetNamespace
      fld.def         = c.getAttributeValue("default");
      fld.fixed       = c.getAttributeValue("fixed");
      fld.form        = c.getAttributeValue("form");
      fld.type        = (typ==null)? type : null;
      fld.use         = c.getAttributeValue("use", "optional");
      fld.inheritable = c.getAttributeValue("inheritable");

      if (typ != null) processChild(Type.ATTRIBUTE, fld, c, typ);
      processChildren(Type.ATTRIBUTE, fld, c);
    }
    else if (n.equals("xs:attributeGroup") && (t==Type.COMPLEX_TYPE || t==Type.EXTENSION || t==Type.RESTRICTION || t==Type.ATTRIBUTE_GROUP)) {
      String  ref = c.getAttributeValue("ref");
      Element g   = attributeGroups.get(ref);
      if (g == null) {
        throw new MidasException("Could not find referenced attribute group "+ref);
      }
      processChildren(Type.ATTRIBUTE_GROUP, f, g);
    }
    else if (n.equals("xs:choice") && (t==Type.SEQUENCE || t==Type.GROUP || t==Type.COMPLEX_TYPE)) {
      processChildren(Type.CHOICE, f, c);
    }
    else if (n.equals("xs:complexContent") && (t==Type.COMPLEX_TYPE)) {
      // tbd
    }
    else if (n.equals("xs:complexType") && (t==Type.ELEMENT)) {
      // ignore abstract, block, final, mixed, name, defaultAttributesApply
      processChildren(Type.COMPLEX_TYPE, f, c);
    }
    else if (n.equals("xs:element") && (t==Type.SEQUENCE)) {
      String name = c.getAttributeValue("name");
      String type = c.getAttributeValue("type");
      Field   fld = new Field(f,name,true);
      Element typ = (type == null)? null : definedTypes.get(type);

      fields.add(fld);
      // ignore abstract, block, final, ref, substitutionGroup, targetNamespace
      fld.def         = c.getAttributeValue("default");
      fld.fixed       = c.getAttributeValue("fixed");
      fld.form        = c.getAttributeValue("form");
      fld.maxOccurs   = c.getAttributeValue("maxOccurs", "1");
      fld.minOccurs   = c.getAttributeValue("minOccurs", "1");
      fld.nillable    = c.getAttributeValue("nillable");
      fld.type        = (typ==null)? type : null;

      if (typ != null) processChild(Type.ELEMENT, fld, c, typ);
      processChildren(Type.ELEMENT, fld, c);
    }
    else if (n.equals("xs:enumeration") && (t==Type.RESTRICTION)) {
      f.type = "enumeration";
      f.enumValues.add(new EnumValue(c.getAttributeValue("value")));
      processChildren(Type.ENUMERATION, f, c);
    }
    else if (n.equals("xs:extension") && (t==Type.SIMPLE_CONTENT)) {
      String base = c.getAttributeValue("base");
      Element typ = (base == null)? null : definedTypes.get(base);
      f.base = base;

      if (typ != null) processChild(Type.ELEMENT, f, c, typ);
      processChildren(Type.EXTENSION, f, c);
    }
    else if (n.equals("xs:group") && (t==Type.SEQUENCE || t==Type.COMPLEX_TYPE)) {
      processChildren(Type.GROUP, f, c);
    }
    else if (n.equals("xs:key") && (t==Type.ELEMENT)) {
      // tbd
    }
    else if (n.equals("xs:keyref") && (t==Type.ELEMENT)) {
      // tbd
    }
    else if (n.equals("xs:list") && (t==Type.SIMPLE_TYPE)) {
      processChildren(Type.LIST, f, c);
    }
    else if (n.equals("xs:openContent") && (t==Type.COMPLEX_TYPE)) {
      // ignore mode
      processChildren(Type.OPEN_CONTENT, f, c);
    }
    else if (n.equals("xs:restriction") && (t==Type.SIMPLE_CONTENT || t==Type.SIMPLE_TYPE)) {
      String base = c.getAttributeValue("base");
      Element typ = (base == null)? null : definedTypes.get(base);
      f.base = base;

      if (typ != null) processChild(Type.ELEMENT, f, c, typ);
      processChildren(Type.RESTRICTION, f, c);
    }
    else if (n.equals("xs:sequence") && (t==Type.SEQUENCE || t==Type.GROUP || t==Type.COMPLEX_TYPE)) {
      processChildren(Type.SEQUENCE, f, c);
    }
    else if (n.equals("xs:simpleContent") && (t==Type.COMPLEX_TYPE)) {
      processChildren(Type.SIMPLE_CONTENT, f, c);
    }
    else if (n.equals("xs:simpleType") && (t==Type.ELEMENT || t==Type.RESTRICTION || t==Type.ATTRIBUTE || t==Type.LIST)) {
      // ignore final, name
      processChildren(Type.SIMPLE_TYPE, f, c);
    }
    else if (n.equals("xs:union") && (t==Type.SIMPLE_TYPE)) {
      processChildren(Type.UNION, f, c);
    }
    else if (n.equals("xs:unique") && (t==Type.ELEMENT)) {
      // tbd
    }
    else if (t==Type.RESTRICTION) {
      String val = c.getAttributeValue("value");
      
           if (n.equals("xs:assertion"       )) { f.assertion        = val; }
      else if (n.equals("xs:explicitTimezone")) { f.explicitTimezone = val; }
      else if (n.equals("xs:fractionDigits"  )) { f.fractionDigits   = val; }
      else if (n.equals("xs:length"          )) { f.length           = val; }
      else if (n.equals("xs:maxExclusive"    )) { f.maxValue         = val; f.maxInExcl = MAX_EXCLUSIVE; }
      else if (n.equals("xs:maxInclusive"    )) { f.maxValue         = val; f.maxInExcl = MAX_INCLUSIVE; }
      else if (n.equals("xs:maxLength"       )) { f.maxLength        = val; }
      else if (n.equals("xs:maxScale"        )) { f.maxScale         = val; }
      else if (n.equals("xs:minExclusive"    )) { f.minValue         = val; f.minInExcl = MIN_EXCLUSIVE; }
      else if (n.equals("xs:minInclusive"    )) { f.minValue         = val; f.minInExcl = MIN_INCLUSIVE; }
      else if (n.equals("xs:minLength"       )) { f.minLength        = val; }
      else if (n.equals("xs:minScale"        )) { f.minScale         = val; }
      else if (n.equals("xs:pattern"         )) { f.pattern          = val; }
      else if (n.equals("xs:totalDigits"     )) { f.totalDigits      = val; }
      else if (n.equals("xs:whiteSpace"      )) { f.whiteSpace       = val; }
      else throw new MidasException("Unsupported <"+n+"/> in <"+e.getQualifiedName()+"/>");
    }
    else {
      throw new MidasException("Unsupported <"+n+"/> in <"+e.getQualifiedName()+"/>");
    }
  }

  /**  Outputs a line if the given val is not null. */
  private void println (String str, Object val) {
    if (val != null) println(str+val);
  }

  /**  Outputs a line. */
  private void println (Object obj) {
    String str = (obj == null)? null : obj.toString();
    if (verbose) System.out.println(str);
    out.writeln(str);
  }

  /**  Defines a field. */
  private static class Field {
    String          xpath            = null;
    String          name             = null;
    String          use              = null;
    String          def              = null;
    String          docs             = null;
    String          type             = null;
    String          base             = null;
    List<EnumValue> enumValues       = new LinkedList<EnumValue>();
    char            minInExcl        = ' ';
    String          minValue         = null;
    String          maxValue         = null;
    char            maxInExcl        = ' ';
    String          minLength        = null;
    String          maxLength        = null;
    String          minScale         = null;
    String          maxScale         = null;
    String          fractionDigits   = null;
    String          totalDigits      = null;
    String          pattern          = null;
    String          assertion        = null;
    String          explicitTimezone = null;
    String          length           = null;
    String          whiteSpace       = null;
    String          fixed            = null;
    String          form             = null;
    String          inheritable      = null;
    String          maxOccurs        = null;
    String          minOccurs        = null;
    String          nillable         = null;

    public Field () {
      this.xpath = "";
      this.name  = "";
    }

    public Field (Field v, String name, boolean element) {
      this.xpath = (element)? v.xpath+"/"+name
                            : v.xpath+"/@"+name;
      this.name  = name;
    }


    /**  Gets the field definition. */
    public String getFieldDef () {
      String  paramName   = (name == null)? "xxxx" : name;
      String  paramType   = (type == null)? "xxxx" : type;
      boolean numeric     = false;
      boolean optional    = !"required".equals(use);
      String  nullComment = "";
      StringBuilder str = new StringBuilder();
      str.append("        <FIELD");

      if (paramType.equals("enumeration")) {
        paramType = "/xxxx/";
      }
      else if ((base != null) && ((type == null) || (type.length() == 0))) {
        paramType = base;
      }
      
      if (paramType.equals("xs:string")) {
        if (optional) nullComment = " (blank if not specified)";

             if (maxLength != null) paramType = "ascii["+maxLength+"]";
        else if (length    != null) paramType = "ascii["+length+"]";
        else                        paramType = "ascii[xx]";
      }
      else if (paramType.equals("xs:boolean")) {
        paramType = (optional)? "boolNull" : "boolean";
      }
      else if (paramType.equals("xs:double") || paramType.equals("xs:float")) {
        if (optional) nullComment = " (zero if not specified)";
        numeric = true;
        paramType = paramType.substring(3);
      }
      else if (paramType.equals("xs:long") || paramType.equals("xs:int")) {
        if (optional) nullComment = " (zero if not specified)";

        numeric = true;
        if ((minValue != null) && (maxValue != null)) {
          try {
            long min = Long.parseLong(minValue);
            long max = Long.parseLong(maxValue);

                 if ((min >    Byte.MIN_VALUE) && (max <    Byte.MAX_VALUE)) paramType = "byte";
            else if ((min >   Short.MIN_VALUE) && (max <   Short.MAX_VALUE)) paramType = "short";
            else if ((min > Integer.MIN_VALUE) && (max < Integer.MAX_VALUE)) paramType = "int";
            else                                                             paramType = "long";
          }
          catch (NumberFormatException e) {
            paramType = paramType.substring(3);
          }
        }
        else {
          paramType = paramType.substring(3);
        }
      }
      str.append(" TYPE='").append(paramType).append("'");
      for (int i = paramType.length(); i < 12; i++) {
        str.append(' ');
      }
      str.append(" NAME='").append(paramName).append("'");
      for (int i = paramName.length(); i < 20; i++) {
        str.append(' ');
      }
      if (numeric) {
        str.append(" UNITS='xxxx' ");
      }
      else {
        str.append("              ");
      }
      if ((minValue != null) || (maxValue != null)) {
        if (minValue == null) { minValue = ""; minInExcl = MIN_INCLUSIVE; }
        if (maxValue == null) { maxValue = ""; maxInExcl = MAX_INCLUSIVE;  }
        str.append(" RANGE='").append(minInExcl).append(minValue).append(',')
                              .append(maxValue).append(maxInExcl).append("'");
        for (int i = minValue.length()+maxValue.length(); i < 12; i++) {
          str.append(' ');
        }
      }
      else {
        str.append("                        ");
      }
      str.append(" DOCUMENTATION='").append(docs).append(nullComment).append("'");
      str.append(" />");


      StringBuilder comment = new StringBuilder();
      if (optional && paramType.equals("enumeration")) {
        comment.append(" OPTIONAL");
      }

      if (minLength        != null) comment.append(" minLength="       ).append(minLength       );
      if (maxLength        != null) comment.append(" maxLength="       ).append(maxLength       );
      if (minScale         != null) comment.append(" minScale="        ).append(minScale        );
      if (maxScale         != null) comment.append(" maxScale="        ).append(maxScale        );
      if (fractionDigits   != null) comment.append(" fractionDigits="  ).append(fractionDigits  );
      if (totalDigits      != null) comment.append(" totalDigits="     ).append(totalDigits     );
      if (pattern          != null) comment.append(" pattern="         ).append(pattern         );
      if (assertion        != null) comment.append(" assertion="       ).append(assertion       );
      if (explicitTimezone != null) comment.append(" explicitTimezone=").append(explicitTimezone);
      if (length           != null) comment.append(" length="          ).append(length          );
      if (whiteSpace       != null) comment.append(" whiteSpace="      ).append(whiteSpace      );
      if (fixed            != null) comment.append(" fixed="           ).append(fixed           );
      if (form             != null) comment.append(" form="            ).append(form            );
      if (inheritable      != null) comment.append(" inheritable="     ).append(inheritable     );
      if (maxOccurs        != null) comment.append(" maxOccurs="       ).append(maxOccurs       );
      if (minOccurs        != null) comment.append(" minOccurs="       ).append(minOccurs       );
      if (nillable         != null) comment.append(" nillable="        ).append(nillable        );

      if (comment.length() > 0) {
        str.append("\n         <!--").append(comment).append(" -->");
      }
      if (enumValues.size() > 0) {
        str.append("\n         <!-- EnumValues:");
        for (EnumValue ev : enumValues) {
          str.append(" ").append(ev.value);
        }
        str.append(" -->");
      }
      return str.toString();
    }

    /**  Gets the XPath definition. */
    public String getXPathDef () {
      StringBuilder str = new StringBuilder();
      str.append("        <FIELD");
      str.append(" NAME='").append(name).append("'");
      for (int i = name.length(); i < 20; i++) {
        str.append(' ');
      }
      str.append(" FUNC=''              ");
      str.append(" XPATH='").append(xpath).append("'");
      str.append(" />");
      return str.toString();
    }

    /**  Gets the enumerated type definition. */
    public String getEnumDef () {
      StringBuilder str = new StringBuilder();
      if (enumValues.size() > 0) {
        str.append("        <ENUMERATION>\n");
        str.append("          <NAME>xxxxxx</NAME>\n");
        str.append("          <TITLE>xxxx</TITLE>\n");
        str.append("          <VALUES>\n");
        
        
        for (EnumValue ev : enumValues) {
          String valName  = ev.value.replace(' ', '_');
          String valAlias = null;
          String valDocs  = ev.docs;

          if (valName.equals("NA")) {
            valAlias = valName;
            valName  = "null";
          }
          if (valDocs == null) {
            valDocs = "";
          }

          str.append("            <VALUE VAL='xx'");
          str.append(" NAME='").append(valName).append("'");
          for (int i = valName.length(); i < 20; i++) {
            str.append(' ');
          }
          if (valAlias != null) {
            str.append(" ALIAS='").append(valAlias).append("'");
            for (int i = valAlias.length(); i < 8; i++) {
              str.append(' ');
            }
          }
          else {
            str.append("                 ");
          }
          str.append(" DESCRIPTION='").append(valDocs).append("' />\n");
        }
        str.append("          </VALUES>\n");
        str.append("        </ENUMERATION>\n");
      }
      return str.toString();
    }
    
    @Override
    public String toString () {
      return getFieldDef() + getXPathDef() + getEnumDef();
    }
  }

  private static class EnumValue {
    String value;
    String docs;

    public EnumValue (String value) {
      this.value = value;
    }

    @Override
    public String toString () {
      return value;
    }

    @Override
    public boolean equals (Object obj) {
      if (obj == this) return true;
      if (!(obj instanceof EnumValue)) return false;
      EnumValue ev = (EnumValue)obj;
      return Util.equals(value, ev.value)
          && Util.equals(docs,  ev.docs);
      
    }

    @Override
    public int hashCode () {
      return (value == null)? 0 : value.hashCode();
    }
  }
}

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

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.PrintStream;
import java.io.IOException;
import java.util.Map;
import java.util.Arrays;
import java.util.LinkedHashMap;
import java.util.LinkedList;
import nxm.vrt.lib.Main;
import nxm.vrt.lib.VRTConfig;
import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

import static nxm.vrt.lib.XmlUtilities.evalXPath;
import static nxm.vrt.lib.XmlUtilities.readDocumentXML;

/** <b>Command-Line Use Only:</b> Documents the user-defined packets and creates
 *  the Java/C++ code required to read/write them.
 *
 *  @author 
 */
public class PacketGen extends Main {
  private static final String[] OPTIONS = { "--HTML=", "--CPP=", "--Java=", "--Python=",
                                            "--exclude=", "--namespace=", "--package=", "--verbose",
                                            "-v" };
  private static final String[] VERSION = { "PacketGen "+VRTConfig.LIBRARY_VERSION };
  private static final String[] USAGE   = {
      "Usage: packetgen [OPTION]... input_file.xml",
      " or:   java -classpath <classpath> nxm.vrt.libg.PacketGen [OPTION]... input_file.xml",
      "",
      "--HTML      <directory>      Output HTML   docs  in   the specified directory (all files in a single directory).",
      "--CPP       <directory>      Output C++    code  in   the specified directory (all files in a single directory).",
      "--Java      <directory>      Output Java   code under the specified directory (subdirectories created as needed).",
      "--Python    <directory>      Output Python code  in   the specified directory. (not implemented yet)",
      "--namespace <namespace>      Use the namespace for all C++ classes (default is 'vita').",
      "--package   <package>        Use the specified package for all Java classes (default is 'nxm.vita.libm').",
      "--exclude   <name(s)>        Exclude the specified packet definition(s) (multiple names separated with a ':')",
      "--help       or -H           Prints usage information and exits.",
      "--verbose    or -v           Include verbose information in output?",
      "--version    or -V           Prints version information and exits.",
      "",
      "Example:",
      "  packetgen --Java      ${HOME}/vrt/java_src/ \\",
      "            --package   nxm.vita.libm \\",
      "            --CPP       ${HOME}/vrt/cpp_src/vita/libm \\",
      "            --namespace vita \\",
      "            ${HOME}/vrt/java_src/nxm/vita/dat/PacketDefinitions.xml"
  };

  static final String BEGIN_TABLE = "\\begin{table}[!ht] \n  \\begin{wide} \n     \\centering";
  static final String END_TABLE   = "  \\end{wide} \n \\end{table}";

  /** Files that must be excluded (this must be in alphabetical order). This is here since users
   *  will often include a "definition" for some of these to add some amplification to the existing
   *  set of docs. This is fine, but we can't generate code as there may be conflicts.
   */
  static final String[] ALWAYS_EXCLUDE = {
    "BasicContextPacket",
    "BasicDataPacket",
    "ContextPacket",
    "DataPacket",
    "StandardDataPacket",
    "VRTPacket",
  };
  
  final InputStream  inputStream;
  final String[]     inputFiles;
  final String       dirHTML;
  final String       dirCPP;
  final String       dirJava;
  final String       dirPython;
  final String       javaPackage;
  final String       cppNamespace;
  final boolean      verbose;
  final String[]     exclude;
  
  /** <b>Internal Use Only:</b> Creates a new instance.
   *  @param args The command-line arguments.
   */
  public PacketGen (String[] args) {
    super(VERSION, USAGE, OPTIONS, 0, -1, args);
    
    this.inputStream  = null;
    this.inputFiles   = parameters.toArray(new String[parameters.size()]);
    this.dirHTML      = options.get("--HTML");
    this.dirCPP       = options.get("--CPP");
    this.dirJava      = options.get("--Java");
    this.dirPython    = options.get("--Python");
    this.exclude      = (options.containsKey("--exclude"))? options.get("--exclude").split(":") : new String[0];
    this.cppNamespace = (options.containsKey("--namespace"))? options.get("--namespace") : "vita";
    this.javaPackage  = (options.containsKey("--package"  ))? options.get("--package"  ) : "nxm.vita.libm";
    this.verbose      = options.containsKey("--verbose") || options.containsKey("-v");

    if (dirPython != null) {
      throw new UnsupportedOperationException("Command-line option --Python <directory> not supported at this time.");
    }
  }

  /** Main method for the class.
   *  @param args The command line arguments.
   */
  public static void main (String ... args) {
    runMain(PacketGen.class, args);
  }

  /** Converts the length specifier.
   *  <pre>
   *    getLength( "*" ) = Type.STAR_LENGTH
   *    getLength( "N" ) = N
   *    getLength("0-N") = -N
   *  </pre>
   *  @param len The length specifier.
   *  @return The length value.
   */
  static int getLengthSpecifier (String len) {
    len = len.trim();
    if (len.equals("*")) {
      return Type.STAR_LENGTH;
    }
    if (len.matches("0[ ]*[-][ ]*[0-9]+")) {
      int i = len.indexOf('-');
      return Integer.parseInt(len.substring(i).trim());
    }
    return Integer.parseInt(len);
  }

  /** Only used in main() */
  private String getHtmlName (String fileName) {
    int i = fileName.lastIndexOf('/');
    if (i > 0) fileName = fileName.substring(i+1);
    
    i = fileName.lastIndexOf('\\');
    if (i > 0) fileName = fileName.substring(i+1);

    return "index_"
         + fileName.replace(".xml", "")
         + ".html";
  }

  /** The overall details of the file. */
  private static final class FileDetails {
    final String title;
    final String author;
    final String descr;

    /** Creates a new instance.
     *  @param node The 'HEAD' node.
     */
    public FileDetails (Node node) {
      this.title    = evalXPath("./TITLE",          node, "");
      this.author   = evalXPath("./AUTHOR",         node, "");
      this.descr    = evalXPath("./DESCRIPTION",    node, "");
    }
  }

  @Override
  @SuppressWarnings("UseOfSystemOutOrSystemErr")
  protected void main () throws Exception {
    File                    javaDir     = (dirJava == null)? null : new File(dirJava, javaPackage.replace('.', File.separatorChar));
    Map<String,PacketType>  packetTypes = new LinkedHashMap<String,PacketType>();
    Map<String,FileDetails> fileDetails = new LinkedHashMap<String,FileDetails>();
    LinkedList<PacketType>  pktFactList = new LinkedList<PacketType>();
    File                    idxFile     = (dirHTML == null)? null : new_File(dirHTML, "index-src.html");
    PrintStream             idxHTML     = (dirHTML == null)? null : new_PrintStream(idxFile);
    File                    rootFile    = (dirHTML == null)? null : new_File(dirHTML, "index.html");
    PrintStream             rootHTML    = (dirHTML == null)? null : new_PrintStream(rootFile);
    LinkedHashMap<String,LinkedList<PacketType>>
                            typesBySrc  = new LinkedHashMap<String,LinkedList<PacketType>>();
    String                  firstName   = null;

    // ==== READ IN THE DEFINITIONS
    Arrays.sort(exclude); // <-- must do this so Arrays.binarySearch(exclude, ..) works below
    for (String f : inputFiles) {
      Document        xmlDocument = readDocumentXML(new File(f));
      Node            headNode    = evalXPath("/XML/HEAD",                     xmlDocument, Node.class);
      NodeList        typeList    = evalXPath("/XML/PACKET_TYPES/PACKET_TYPE", xmlDocument, NodeList.class);
      
      for (int i = 0; i < typeList.getLength(); i++) {
        PacketType p = new PacketType(typeList.item(i), packetTypes, f);
        packetTypes.put(p.name, p);
      }
      if (headNode != null) {
        fileDetails.put(f, new FileDetails(headNode));
      }
    }

    // ==== SORT THE DEFINITIONS
    for (PacketType p : packetTypes.values()) {
      if (!typesBySrc.containsKey(p.srcFileName)) {
        typesBySrc.put(p.srcFileName, new LinkedList<PacketType>());
      }
      typesBySrc.get(p.srcFileName).add(p);

      if (firstName == null) {
        firstName = getHtmlName(p.srcFileName);
      }
    }
    if (firstName == null) {
      firstName = "index-src.html";
    }
    
    // ==== DOCUMENTATION FILES (initial)
    if (dirHTML != null) {
      // The frameset layout is modeled after what is done in the javadoc output since it should be
      // familiar to most users
      rootHTML.println("<html>");
      rootHTML.println("  <head>");
      rootHTML.println("    <title>User-Defined VITA-49 Packet Definitions</title>");
      rootHTML.println("    <script type='text/javascript'>");
      rootHTML.println("      targetPage = \"\" + window.location.search;");
      rootHTML.println("      if (targetPage != \"\" && targetPage != \"undefined\") {");
      rootHTML.println("        targetPage = targetPage.substring(1);");
      rootHTML.println("      }");
      rootHTML.println("      function loadFrames() {");
      rootHTML.println("        if (targetPage != \"\" && targetPage != \"undefined\") {");
      rootHTML.println("          top.docFrame.location = top.targetPage;");
      rootHTML.println("        }");
      rootHTML.println("      }");
      rootHTML.println("    </script>");
      rootHTML.println("    <noscript></noscript>");
      rootHTML.println("  </head>");
      rootHTML.println("  <frameset cols='20%,80%' title='' onLoad='top.loadFrames()'>");
      rootHTML.println("    <frame src='index-src.html' name='sourceFrame' title='All Sources'>");
      rootHTML.println("    <frame src='"+firstName+"'  name='docFrame'    title='Documentation'>");
      rootHTML.println("  </frameset>");
      rootHTML.println("  <noframes>");
      rootHTML.println("    This document uses frames, see <a href='index-src.html'>No Frame Version</a>.");
      rootHTML.println("  </noframes>");
      rootHTML.println("</html>");
      
      idxHTML.println("<html>");
      idxHTML.println("  <head>");
      idxHTML.println("    <title>User-Defined VITA-49 Packet Definitions (index)</title>");
      idxHTML.println("    <style>");
      idxHTML.println("      body { font-family:  serif, 'Times New Roman', 'Times', serif;");
      idxHTML.println("             font-size:    10pt;  font-style: normal;");
      idxHTML.println("             background-color: #FFFFFF; color:#000000; }");
      idxHTML.println("    </style>");
      idxHTML.println("  </head>");
      idxHTML.println("  <body>");
    }
    
    // ==== PACKET TYPES SECTION(S)
    for (String f : typesBySrc.keySet()) {
      String      docName = getHtmlName(f);
      File        docFile = (dirHTML == null)? null : new_File(dirHTML, docName);
      PrintStream docHTML = (dirHTML == null)? null : new_PrintStream(docFile);
      FileDetails details = fileDetails.get(f);

      if (dirHTML != null) {
        printHeaderHTML(docHTML, details.title, details.author, details.descr);

        idxHTML.println("    <br>");
        idxHTML.println("    &nbsp;&nbsp;<a href='" + docName + "' target='docFrame'>" + details.title + "</a><br>");
      }
      
      for (PacketType p : typesBySrc.get(f)) {
        boolean userSkip  = (Arrays.binarySearch(exclude,        p.name) >= 0);
        boolean forceSkip = (Arrays.binarySearch(ALWAYS_EXCLUDE, p.name) >= 0) && !userSkip;
        boolean docSkip   = userSkip;
        boolean codeSkip  = userSkip || forceSkip;
        
        if ((dirCPP != null) || (dirJava != null)) {
          if (forceSkip) {
            System.err.println("PacketGen: Excluding code generation for "+p.name+" as it "
                             + "matches one of the standard library definitions.");
          }
          if (!codeSkip) {
            pktFactList.add(p);
          }
        }
        
        if ((dirHTML != null) && !docSkip){
          String      htmFile = p.name+".html";
          File        outFile = new_File(dirHTML, htmFile);
          PrintStream outHTML = new_PrintStream(outFile);

          p.writeHTML(this, outHTML, docHTML, htmFile);
          outHTML.close();
        }
        if ((dirCPP != null) && !codeSkip) {
          File        hdrFile = new_File(dirCPP, p.name+".h");
          File        cppFile = new_File(dirCPP, p.name+".cc");
          PrintStream outHdr  = new_PrintStream(hdrFile);
          PrintStream outCPP  = new_PrintStream(cppFile);

          p.writeCPP(this, outHdr, outCPP);
          outHdr.close();
          outCPP.close();
        }
        if ((dirJava != null) && !codeSkip)  {
          File        outFile = new_File(javaDir, p.name+".java");
          PrintStream outJava = new_PrintStream(outFile);

          p.writeJava(this, outJava);
          outJava.close();
        }
      }
    
      if (dirHTML != null) {
        printFooterHTML(docHTML);
        docHTML.close();
      }
    }
    
    // ==== PACKET FACTORY
    PacketFactory pf = new PacketFactory(pktFactList);

    if (dirCPP != null) {
      File        hdrFile = new_File(dirCPP, pf.name+".h");
      File        cppFile = new_File(dirCPP, pf.name+".cc");
      PrintStream outHdr  = new_PrintStream(hdrFile);
      PrintStream outCPP  = new_PrintStream(cppFile);

      pf.writeCPP(this, outHdr, outCPP);
      outHdr.close();
      outCPP.close();
    }
    if (dirJava != null) {
      File        outFile = new_File(javaDir, pf.name+".java");
      PrintStream outJava = new_PrintStream(outFile);

      pf.writeJava(this, outJava);
      outJava.close();
    }
    
    // ==== DONE
    if (dirHTML != null) {
      idxHTML.println("  </body>");
      idxHTML.println("</html>");
      idxHTML.close();
      rootHTML.close();
    }
  }
  
  /** Creates a new File, but handles any required directory creation. */
  private static File new_File (String dir, String file) {
    File f = new File(dir, file);
    f.getParentFile().mkdirs();
    return f;
  }

  /** Creates a new File, but handles any required directory creation. */
  private static File new_File (File dir, String file) {
    File f = new File(dir, file);
    f.getParentFile().mkdirs();
    return f;
  }
  
  /** Creates a new PrintStream for a File, but handles any required buffering. */
  private static PrintStream new_PrintStream (File f) throws IOException {
    // Note that we are using:
    //    new PrintStream(new BufferedOutputStream(new FileOutputStream(...)));
    // rather than:
    //    new PrintStream(...);
    // as the later version can be extremely slow on some systems.
    return new PrintStream(new BufferedOutputStream(new FileOutputStream(f)));
  }

  /** Prints an HTML header. */
  static void printHeaderHTML (PrintStream out, String title) {
    printHeaderHTML(out, title, null, null);
  }

  /** Prints an HTML header with title block. */
  static void printHeaderHTML (PrintStream out, String title, String author, String description) {
    out.println("<html>");
    out.println("  <head>");
    if ((title != null) && (title.length() > 0)) {
      out.println("    <title>" + toHtml(title) + "</title>");
    }
    out.println("    <style>");
    out.println("      body { font-family:  serif, 'Times New Roman', 'Times', serif;");
    out.println("             font-size:    10pt;  font-style: normal;");
    out.println("             background-color: #FFFFFF; color:#000000; width:        600pt; }");
    out.println("      ");
    out.println("      h1 { font-size:16pt; font-weight:bold; text-align:center; font-style: normal;");
    out.println("           margin: 0pt 0pt 0pt 0pt; padding: 8pt 8pt 8pt 8pt; }");
    out.println("      ");
    out.println("      h2 { font-size:14pt; font-weight:bold; text-align:left; font-style: normal;");
    out.println("           margin: 0pt 0pt 0pt 0pt; padding: 0pt 0pt 0pt 0pt; }");
    out.println("      ");
    out.println("      h3 { font-size:12pt; font-weight:bold; text-align:left; font-style: normal;");
    out.println("           margin: 0pt 0pt 0pt 0pt; padding: 0pt 0pt 0pt 0pt; }");
    out.println("      ");
    out.println("      h4 { font-size:12pt; font-weight:bold; text-align:left;   font-style: normal;");
    out.println("           margin: 0pt 0pt 0pt 0pt; padding: 0pt 0pt 0pt 0pt; }");
    out.println("      ");
    out.println("      h5 { font-size:12pt; font-weight:bold; text-align:left;   font-style: italic;");
    out.println("           margin: 0pt 0pt 0pt 0pt; padding: 0pt 0pt 0pt 0pt; }");
    out.println("      ");
    out.println("      pre    { font-family:  monospace, 'Courier New', 'Courier', monospace;");
    out.println("               font-size:    10pt;");
    out.println("               font-weight:  bold;");
    out.println("               text-align:   left;");
    out.println("               border-style: outset; border-width: 0pt;");
    out.println("               margin: 5pt 5pt 5pt 5pt; padding: 5pt 5pt 5pt 5pt; }");
    out.println("      ");
    out.println("      pre.led{ font-size:6pt; font-weight:bold; }");
    out.println("      ");
    out.println("      code   { font-family:  monospace, 'Courier New', 'Courier', monospace;");
    out.println("               font-size:    10pt;");
    out.println("               font-weight:  bold;");
    out.println("               text-align:   left; }");
    out.println("      ");
    out.println("      ol { margin: 0pt 20pt 5pt 20pt; padding: 0pt 20pt 5pt 20pt; }");
    out.println("      ul { margin: 0pt 20pt 5pt 20pt; padding: 0pt 20pt 5pt 20pt; }");
    out.println("      ");
    out.println("      li { margin-top: 10pt; }");
    out.println("      ");
    out.println("      table       { margin-left:  20pt; margin-right: 20pt;");
    out.println("                    font-size:    10pt; font-style: normal;");
    out.println("                    width:        560pt;");
    out.println("                    border-style: outset; border-width: 1pt;");
    out.println("                    empty-cells:  show; }");
    out.println("      ");
    out.println("      table.nested{ margin-left:  0pt;  margin-right: 0pt;");
    out.println("                    font-size:    10pt; font-style: normal;");
    out.println("                    width:        100%;");
    out.println("                    border-style: none;");
    out.println("                    empty-cells:  show; }");
    out.println("      ");
    out.println("      td          { border-style: inset; border-width: 1pt;");
    out.println("                    text-align:left; vertical-align: text-top;");
    out.println("                    background-color: #FFFFFF; color:#000000; }");
    out.println("      th          { border-style: inset; border-width: 1pt;");
    out.println("                    text-align:left; vertical-align: text-top;");
    out.println("                    background-color: #888888; color:#000000; }");
    out.println("      ");
    out.println("      th.title   { background-color: #FFFF66; text-align:center; }");
    out.println("      th.section { background-color: #9999FF; text-align:center; }");
    out.println("      th.col     {                            text-align:center; }");
    out.println("    </style>");
    out.println("  </head>");
    out.println("  <body>");
    if ((title != null) && (title.length() > 0)) {
      out.println("    <h1>" + toHtml(title) + "</h1>");
    }
    if ((author != null) && (author.length() > 0)) {
      out.println("    ");
      out.println("    <center>" + toHtml(author) + "</center>");
      out.println("    <br>");
    }
    if ((description != null) && (description.length() > 0)) {
      out.println("    ");
      out.println("    " + toHtml(description,"    "));
      out.println("    <br>");
    }
  }

  /** Prints an HTML footer. */
  static void printFooterHTML (PrintStream out) {
    out.println("  </body>");
    out.println("</html>");
  }

  /** Prints an HTML index. */
  static void printIndexHTML (PrintStream out, String title, String author,
                              String description, String fileName) {
    out.println("    <br>");
    out.println("    &nbsp;&nbsp;&nbsp;&nbsp;<a href='" + fileName + "'>" + title + "</a>");
    out.println("    <br>");
  }

  /** Converts a boolean/null to a usage statement. */
  static String use (Boolean use) {
    if (use == null) return "Sometimes Used";
    return (use)? "Used" : "Not Used";
  }

  /** Special plural-to-singular conversions. */
  private static final String[] PLURAL_TO_SINGULAR = {
    // Plural     Singular         Plural    Singular         Plural  Singular
      "Status",  "Status",        "Movies", "Movie",         "Ties", "Tie",
      "Sources", "Source"
  };
  
  /** Converts a plural name to not-plural. */
  static String notPlural (String str) {
    int len = str.length();

    for (int i = 0; i < PLURAL_TO_SINGULAR.length; i+=2) {
      String s = PLURAL_TO_SINGULAR[i];

      if (str.endsWith(s)) {
        return str.substring(0,len-s.length()) + PLURAL_TO_SINGULAR[i+1];
      }
    }

    if (str.endsWith("ies")) return str.substring(0,len-3)+"y";
    if (str.endsWith("es" )) return str.substring(0,len-2);
    if (str.endsWith("s"  )) return str.substring(0,len-1);

    return str;
  }
  
  /** Converts text to HTML, with any applicable adjustments required for inclusion in C++ and
   *  Java documentation that will be generated by javadoc or doxygen.
   */
  static String toHtmlDoc (String str, String indent) {
    if (str == null) return null;
    return toHtml(str,indent).replace("/*",  "/<!-- -->*")                   // Don't insert "/*"
                             .replace("*/",  "*<!-- -->/")                   // Don't insert "*/"
                             .replace("<u>", "<b>").replace("</u>", "</b>"); // "<u>" not supported by doxygen
  }

  /** Converts text to HTML, with any applicable adjustments required for inclusion in the title
   *  line of C++ and Java documentation that will be generated by javadoc or doxygen.
   *  @param str    The text.
   *  @param indent The indentation string (not null).
   */
  static String toHtmlTitle (String str, String indent, boolean isJava) {
    if (str == null) return null;
    String txt = toHtmlDoc(str,indent+" *  ");
    if (txt.endsWith(".") && !txt.endsWith("..")) {
      txt = txt.substring(0, txt.length() - 1);
    }
    return (isJava)? "<b>"+txt.replace(".", "&#46;") + "</b>. "
                   : "<b>"+txt.replace(". ", ".\\ ") + "</b>. ";
  }

  /** Converts text to HTML. */
  static String toHtml (String str) {
    return toHtml(str,"");
  }

  /** Converts text to HTML.
   *  @param str    The text.
   *  @param indent The indentation string (not null).
   */
  static String toHtml (String str, String indent) {
    if (str == null) return null;
    str = str.replace("[tt]",  "<tt>" ).replace("[/tt]",  "</tt>")
             .replace("[b]",   "<b>"  ).replace("[/b]",   "</b>")
             .replace("[i]",   "<i>"  ).replace("[/i]",   "</i>")
             .replace("[u]",   "<u>"  ).replace("[/u]",   "</u>")
             .replace("[pre]", "<pre>").replace("[/pre]", "</pre>")
             .replace("[ul]",  "<ul>" ).replace("[/ul]",  "</ul>")
             .replace("[ol]",  "<ol>" ).replace("[/ol]",  "</ol>")
             .replace("[li]",  "<li>" ).replace("[/li]",  "</li>")
             .replace("[br]",  "<br>\n")
             .replace("[br/]", "<br>\n")
             .replace("[[",    "[")
             .replace("]]",    "]");
    return fixNewLine(str, indent);
  }
  
  /** Fixes the newline characters in the string with special handling for 'pre' blocks.
   *  @param str    The text.
   *  @param indent The indentation string (not null).
   */
  private static String fixNewLine (String str, String indent) {
    return str.replaceAll("\n[ ]*\n", "<br><br>\n\n")
              .replaceAll("\n",   "\n"+indent);
//    int i = str.indexOf("<pre>");
//    if (i < 0) {
//      return str.replaceAll("\n[ ]*\n", "<br>\n<br>\n")
//                .replaceAll("\n[ ]*",   "\n"+indent);
//    }
//    int j = str.indexOf("</pre>",i);
//    if (j < 0) return str; // ERROR
//    
//    return fixNewLine(str.substring(0,i), indent)
//         + str.substring(i,j+4).replace("\n", "\n"+indent)
//         + fixNewLine(str.substring(j+4), indent);
  }
}

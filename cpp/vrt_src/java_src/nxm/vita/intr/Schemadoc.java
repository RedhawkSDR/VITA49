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

/**  Documents the packets.
 *
 *  @author         
 */
public class Schemadoc extends Intrinsic {
  
  @Override
  public int process() {
    TextFile in  = MA.getTextFile("IN");
    TextFile out = MA.getTextFile("OUT");
    
    in.open(TextFile.INPUT|TextFile.FORCEABORT);
    out.open(TextFile.OUTPUT|TextFile.FORCEABORT);

    String[] lines = in.readAllLines();
    out.writeln("<html>");
    out.writeln("  <body bgcolor='lightgray'>");
    out.writeln("    <pre>");
    for (String line : lines) {
      line = line.replaceAll("<([/?]?[a-zA-Z0-9_:]+)(([ ]+[A-Za-z0-9_:]+[=](([\'][^\']*[\'])|([\"][^\"]*[\"])))+)*[ ]*([/?]?)>",  "&&&&red&&&&&lt;$1&&&&/red&&&&$2&&&&red&&&&$7&gt;&&&&/red&&&&")
                 //             -------------------                         --------------   --------------            -----
                 //                       1                                        5               6                     7
                 //                                                        ---------------------------------
                 //                                                                        4
                 //                                   -------------------------------------------------------
                 //                                                            3
                 //                                  ---------------------------------------------------------
                 //                                                            2
                 .replaceAll("<([^>]*)>",          "&lt;$1&gt;")
                 .replaceAll("<!--",               "&lt;!--")
                 .replaceAll("-->",                "--&gt;")
                 .replaceAll( "&&&&red&&&&",       "<font color='red'>")
                 .replaceAll( "&&&&/red&&&&",      "</font>")
                 .replaceAll( " ref=\"([^\"]+)\"",  " ref=<font color='green'>\"<a href='#$1'>$1</a>\"</font>")
                 .replaceAll(" base=\"([^\"]+)\"", " base=<font color='green'>\"<a href='#$1'>$1</a>\"</font>")
                 .replaceAll(" type=\"([^\"]+)\"", " type=<font color='green'>\"<a href='#$1'>$1</a>\"</font>")
                 .replaceAll(" name=\"([^\"]+)\"", " name=<font color='green'>\"<a name='$1'>$1</a>\"</font>")
                 .replaceAll(     "=\"([^\"]+)\"",      "=<font color='green'>\"$1\"</font>")
                 ;
      out.writeln(line);
    }
    out.writeln("    </pre>");
    out.writeln("  </body>");
    out.writeln("</html>");

    in.close();
    out.close();

    return FINISH;
  }
  
}

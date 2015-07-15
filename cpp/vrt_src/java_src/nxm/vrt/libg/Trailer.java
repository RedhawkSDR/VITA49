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
import static nxm.vrt.lib.Utilities.toBoolean;
import static nxm.vrt.libg.PacketGen.toHtml;
import static nxm.vrt.libg.PacketGen.use;

/** A packet trailer value.
 *
 *  @author 
 */
class Trailer extends Definition {
  final PacketType packetType;
  final String     title;
  final Boolean    usage;

  final Boolean calibratedTimeUsed;                       final String calibratedTimeDescr;
  final Boolean validDataUsed;                            final String validDataDescr;
  final Boolean referenceLockUsed;                        final String referenceLockDescr;
  final Boolean agcMgcUsed;                               final String agcMgcDescr;
  final Boolean detectedSignalUsed;                       final String detectedSignalDescr;
  final Boolean spectralInversionUsed;                    final String spectralInversionDescr;
  final Boolean overRangeUsed;                            final String overRangeDescr;
  final Boolean sampleLossUsed;                           final String sampleLossDescr;
  final Boolean bit11Used;      final String bit11Name;   final String bit11Descr;
  final Boolean bit10Used;      final String bit10Name;   final String bit10Descr;
  final Boolean bit9Used;       final String bit9Name;    final String bit9Descr;
  final Boolean bit8Used;       final String bit8Name;    final String bit8Descr;

  /** Creates a new trailer definition.
   *  @param pt   Packet type.
   *  @param node Node from the XML.
   */
  public Trailer (PacketType pt, Node node) {
    packetType             = pt;
    title                  = evalXPath("TITLE", node, "");
    usage                  = toBool(evalXPath("FIELDS/USAGE", node , "null"));
    
    calibratedTimeUsed     = toBool(evalXPath("FIELDS/FIELD[@BIT=CALIBRATED_TIME]/@USED",  node, "null"));
    validDataUsed          = toBool(evalXPath("FIELDS/FIELD[@BIT=VALID_DATA]/@USED",       node, "null"));
    referenceLockUsed      = toBool(evalXPath("FIELDS/FIELD[@BIT=REF_LOCK]/@USED",         node, "null")); 
    agcMgcUsed             = toBool(evalXPath("FIELDS/FIELD[@BIT=AGC_MGC]/@USED",          node, "null"));
    detectedSignalUsed     = toBool(evalXPath("FIELDS/FIELD[@BIT=DETECTED_SIGNAL]/@USED",  node, "null"));
    spectralInversionUsed  = toBool(evalXPath("FIELDS/FIELD[@BIT=SPECTRAL_INVERSE]/@USED", node, "null"));
    overRangeUsed          = toBool(evalXPath("FIELDS/FIELD[@BIT=OVER_RANGE]/@USED",       node, "null"));
    sampleLossUsed         = toBool(evalXPath("FIELDS/FIELD[@BIT=SAMPLE_LOSS]/@USED",      node, "null"));
    bit11Used              = toBool(evalXPath("FIELDS/FIELD[@BIT=BIT_11]/@USED",           node, "null"));
    bit10Used              = toBool(evalXPath("FIELDS/FIELD[@BIT=BIT_10]/@USED",           node, "null"));
    bit9Used               = toBool(evalXPath("FIELDS/FIELD[@BIT=BIT_9]/@USED",            node, "null"));
    bit8Used               = toBool(evalXPath("FIELDS/FIELD[@BIT=BIT_8]/@USED",            node, "null"));
    
    String bit11Def = ((bit11Used == null) || bit11Used)? "" : "[i]Reserved for future use, set to zero.[/i]";
    String bit10Def = ((bit10Used == null) || bit10Used)? "" : "[i]Reserved for future use, set to zero.[/i]";
    String bit9Def  = ((bit9Used  == null) || bit9Used )? "" : "[i]Reserved for future use, set to zero.[/i]";
    String bit8Def  = ((bit8Used  == null) || bit8Used )? "" : "[i]Reserved for future use, set to zero.[/i]";
    
    calibratedTimeDescr    = evalXPath("FIELDS/FIELD[@BIT=CALIBRATED_TIME]/@DESCRIPTION",  node, "");
    validDataDescr         = evalXPath("FIELDS/FIELD[@BIT=VALID_DATA]/@DESCRIPTION",       node, "");
    referenceLockDescr     = evalXPath("FIELDS/FIELD[@BIT=REF_LOCK]/@DESCRIPTION",         node, "");
    agcMgcDescr            = evalXPath("FIELDS/FIELD[@BIT=AGC_MGC]/@DESCRIPTION",          node, "");
    detectedSignalDescr    = evalXPath("FIELDS/FIELD[@BIT=DETECTED_SIGNAL]/@DESCRIPTION",  node, "");
    spectralInversionDescr = evalXPath("FIELDS/FIELD[@BIT=SPECTRAL_INVERSE]/@DESCRIPTION", node, "");
    overRangeDescr         = evalXPath("FIELDS/FIELD[@BIT=OVER_RANGE]/@DESCRIPTION",       node, "");
    sampleLossDescr        = evalXPath("FIELDS/FIELD[@BIT=SAMPLE_LOSS]/@DESCRIPTION",      node, "");
    bit11Descr             = evalXPath("FIELDS/FIELD[@BIT=BIT_11]/@DESCRIPTION",           node, bit11Def);
    bit10Descr             = evalXPath("FIELDS/FIELD[@BIT=BIT_10]/@DESCRIPTION",           node, bit10Def);
    bit9Descr              = evalXPath("FIELDS/FIELD[@BIT=BIT_9]/@DESCRIPTION",            node, bit9Def);
    bit8Descr              = evalXPath("FIELDS/FIELD[@BIT=BIT_8]/@DESCRIPTION",            node, bit8Def);

    bit11Name              = evalXPath("FIELDS/FIELD[@BIT=BIT_11]/@NAME",                  node, "");
    bit10Name              = evalXPath("FIELDS/FIELD[@BIT=BIT_10]/@NAME",                  node, "");
    bit9Name               = evalXPath("FIELDS/FIELD[@BIT=BIT_9]/@NAME",                   node, "");
    bit8Name               = evalXPath("FIELDS/FIELD[@BIT=BIT_8]/@NAME",                   node, "");
  }

  /** Converts string to boolean where "optional" and "null" match null. */
  private static Boolean toBool (String str) {
    if ((str == null) || str.equalsIgnoreCase("optional") || str.equalsIgnoreCase("null")) {
      return null;
    }
    return toBoolean(str);
  }

  @Override
  public void writeHTML (PacketGen outParams, PrintStream html, PrintStream index, String fileName) {
    String bit11 = (bit11Name.isEmpty())? "User Defined Indicator #11" : "User Defined Indicator #11 ("+bit11Name+")";
    String bit10 = (bit10Name.isEmpty())? "User Defined Indicator #10" : "User Defined Indicator #10 ("+bit10Name+")";
    String bit9  = (bit9Name.isEmpty() )? "User Defined Indicator #9 " : "User Defined Indicator #9  ("+bit9Name +")";
    String bit8  = (bit8Name.isEmpty() )? "User Defined Indicator #8 " : "User Defined Indicator #8  ("+bit8Name +")";
    
    html.println("    <tr><th class='col'>Parameter</th><th class='col'>Allowed Options</th><th class='col'>Comments</th></tr>");
    html.println("    <tr><th>Calibrated Time    Indicator</th><td>" + use(calibratedTimeUsed   ) + "</td><td>"+toHtml(calibratedTimeDescr   )+"</td></tr>");
    html.println("    <tr><th>Valid Data         Indicator</th><td>" + use(validDataUsed        ) + "</td><td>"+toHtml(validDataDescr        )+"</td></tr>");
    html.println("    <tr><th>Reference Lock     Indicator</th><td>" + use(referenceLockUsed    ) + "</td><td>"+toHtml(referenceLockDescr    )+"</td></tr>");
    html.println("    <tr><th>AGC/MGC            Indicator</th><td>" + use(agcMgcUsed           ) + "</td><td>"+toHtml(agcMgcDescr           )+"</td></tr>");
    html.println("    <tr><th>Detected Signal    Indicator</th><td>" + use(detectedSignalUsed   ) + "</td><td>"+toHtml(detectedSignalDescr   )+"</td></tr>");
    html.println("    <tr><th>Spectral-Inversion Indicator</th><td>" + use(spectralInversionUsed) + "</td><td>"+toHtml(spectralInversionDescr)+"</td></tr>");
    html.println("    <tr><th>Over-Range         Indicator</th><td>" + use(overRangeUsed        ) + "</td><td>"+toHtml(overRangeDescr        )+"</td></tr>");
    html.println("    <tr><th>Sample Loss        Indicator</th><td>" + use(sampleLossUsed       ) + "</td><td>"+toHtml(sampleLossDescr       )+"</td></tr>");
    html.println("    <tr><th>" + bit11                + "</th><td>" + use(bit11Used            ) + "</td><td>"+toHtml(bit11Descr            )+"</td></tr>");
    html.println("    <tr><th>" + bit10                + "</th><td>" + use(bit10Used            ) + "</td><td>"+toHtml(bit10Descr            )+"</td></tr>");
    html.println("    <tr><th>" + bit9                 + "</th><td>" + use(bit9Used             ) + "</td><td>"+toHtml(bit9Descr             )+"</td></tr>");
    html.println("    <tr><th>" + bit8                 + "</th><td>" + use(bit8Used             ) + "</td><td>"+toHtml(bit8Descr             )+"</td></tr>");
    html.println("    <tr><th>Associated Context Packet Count</th><td>" + use(null              ) + "</td><td>&nbsp;   </td></tr>");
  }
}

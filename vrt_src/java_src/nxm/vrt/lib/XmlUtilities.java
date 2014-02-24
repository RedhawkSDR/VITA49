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

package nxm.vrt.lib;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import javax.xml.namespace.QName;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.xpath.XPath;
import javax.xml.xpath.XPathConstants;
import javax.xml.xpath.XPathExpression;
import javax.xml.xpath.XPathExpressionException;
import javax.xml.xpath.XPathFactory;
import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;

/** Various XML utilities methods.
 *
 *  @author 
 */
public final class XmlUtilities {
  private XmlUtilities() { } // prevent instantiation
  
  private static XPath xPath = null;
  
  /** Builds an XPath. This is identical to calling
   *  <tt>XPathFactory.newInstance().newXPath().compile(expression)</tt> except
   *  that rather than throwing an {@link XPathExpressionException} it throws an
   *  {@link AssertionError}. <br>
   *  <br>
   *  This is intended for use with pre-defined XPaths stored as constants,
   *  where runtime exceptions should not be possible.
   *  @param expression The XPath expression to compile.
   *  @throws AssertionError If the nested call to <tt>XPath.newInstance(path)</tt>
   *                         throws an {@link XPathExpressionException}.
   */
  public static synchronized XPathExpression xpath (String expression) {
    if (xPath == null) xPath =  XPathFactory.newInstance().newXPath();
    
    try {
      return xPath.compile(expression);
    }
    catch (XPathExpressionException e) {
      throw new AssertionError("Error initializing XPath instance for '"+expression+"': "+e);
    }
  }
  
  /** Evaluates an XPath. */
  private static synchronized Object evalXPath (String expression, Object item, QName type) {
    if (xPath == null) xPath =  XPathFactory.newInstance().newXPath();
    
    try {
      XPathExpression exp  = xPath.compile(expression);
      Object          node = exp.evaluate(item, XPathConstants.NODE);
      
      return (node == null)? null : exp.evaluate(item, type);
    }
    catch (XPathExpressionException e) {
      throw new AssertionError("Error initializing XPath instance for '"+expression+"': "+e);
    }
  }
  
  /** Evaluates an XPath returning null if not found. <br>
   *  <br>
   *  This is intended for use with pre-defined XPaths stored as constants,
   *  where runtime exceptions should not be possible.
   *  @param expression The XPath expression to evaluate.
   *  @param item       The {@link Node} or other item to evaluate the XPath on.
   *  @param type       The type to return, this must be one of the following:
   *                    {@link String}, {@link CharSequence}, {@link Boolean},
   *                    {@link Node}, {@link NodeList}, {@link Double}, or
   *                    {@link Number}.
   *  @throws AssertionError If the nested call to <tt>XPath.newInstance(path)</tt>
   *                         throws an {@link XPathExpressionException}.
   */
  public static <T> T evalXPath (String expression, Object item, Class<T> type) {
    Object val;
    
         if (type == String.class      ) val = evalXPath(expression, item, XPathConstants.STRING );
    else if (type == CharSequence.class) val = evalXPath(expression, item, XPathConstants.STRING );
    else if (type == Boolean.class     ) val = evalXPath(expression, item, XPathConstants.BOOLEAN);
    else if (type == Boolean.TYPE      ) val = evalXPath(expression, item, XPathConstants.BOOLEAN);
    else if (type == Node.class        ) val = evalXPath(expression, item, XPathConstants.NODE   );
    else if (type == NodeList.class    ) val = evalXPath(expression, item, XPathConstants.NODESET);
    else if (type == Double.class      ) val = evalXPath(expression, item, XPathConstants.NUMBER );
    else if (type == Double.TYPE       ) val = evalXPath(expression, item, XPathConstants.NUMBER );
    else if (type == Number.class      ) val = evalXPath(expression, item, XPathConstants.NUMBER );
    else                                 throw new IllegalArgumentException("Invalid type given "+type);
    
    return type.cast(val);
  }
  
  /** Evaluates an XPath returning a default value if not found. <br>
   *  <br>
   *  This is intended for use with pre-defined XPaths stored as constants,
   *  where runtime exceptions should not be possible.
   *  @param expression The XPath expression to evaluate.
   *  @param item       The {@link Node} or other item to evaluate the XPath on.
   *  @param def        The default value to return if the evaluation of the
   *                    expression returns null. The default value must be an
   *                    instance of one of the following: {@link String},
   *                    {@link CharSequence}, {@link Boolean}, {@link Node},
   *                    {@link NodeList}, {@link Double}, or {@link Number}.
   *  @throws NullPointerException If <tt>def</tt> is null.
   *  @throws AssertionError If the nested call to <tt>XPath.newInstance(path)</tt>
   *                         throws an {@link XPathExpressionException}.
   */
  @SuppressWarnings("unchecked")
  public static <T> T evalXPath (String expression, Object item, T def) {
    T val = (T)evalXPath(expression, item, def.getClass());
    return (val == null)? def : val;
  }
  
  /** Reads in an XML document from a local file.
   *  @param file The file to read in.
   *  @return The parsed XML document.
   *  @throws ParserConfigurationException If there is an error in the default
   *                                       XML parser configuration.
   *  @throws IOException                  If there is an error reading the file.
   *  @throws SAXException                 If there is an error in parsing the file.
   */
  public static Document readDocumentXML (File file) throws ParserConfigurationException, IOException, SAXException {
    // Note that we are using:
    //    new BufferedInputStream(new FileInputStream(...))
    // rather than:
    //    new FileInputStream(...)
    // as the later version can be extremely slow on some systems.
    DocumentBuilder builder = DocumentBuilderFactory.newInstance().newDocumentBuilder();
    InputStream     in      = new BufferedInputStream(new FileInputStream(file));
    return builder.parse(in);
  }
}

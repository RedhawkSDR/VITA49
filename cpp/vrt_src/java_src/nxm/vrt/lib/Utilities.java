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

import java.lang.reflect.Array;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.Collection;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.UUID;
import java.util.concurrent.AbstractExecutorService;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadFactory;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;

import static java.lang.Boolean.FALSE;
import static java.lang.Boolean.TRUE;
import static java.lang.Integer.parseInt;

/** Various utility methods.
 *
 *  @author 
 */
public final class Utilities {
  private Utilities() { } // prevent instantiation
  
  private static final char[] hexChar = { '0', '1', '2', '3', '4', '5', '6', '7',
                                          '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

  /** Calls <tt>Thread.sleep(millis)</tt> but ignores any {@link InterruptedException}. */
  public static void sleep (long millis) {
    try {
      Thread.sleep(millis);
    }
    catch (InterruptedException ex) {
      // ignore
    }
  }
  
  /** Creates a map from a set of keys and values.
   *  @param <K>      The key type.
   *  @param <V>      The value type.
   *  @param keyClass The key type.
   *  @param valClass The value type.
   *  @param entries  The entries, this must be in the form { key0, val0, key1, val1, ..., keyN, valN }.
   *  @return The map containing the entries specified.
   */
  public static <K extends Object, V extends Object> Map<K,V> asMap (Class<K> keyClass, Class<V> valClass, Object ... entries) {
    Map<K,V> map = new LinkedHashMap<K,V>();

    for (int i = 0; i < entries.length; ) {
      K key = keyClass.cast(entries[i++]);
      V val = valClass.cast(entries[i++]);
      map.put(key, val);
    }
    return map;
  }

  /** Tests equality between two objects with a check for null. */
  public static boolean equal (Object r1, Object r2) {
    return (r1 == r2) || ((r1 != null) && r1.equals(r2));
  }

  /** Appends a value to a string buffer if not null.
   *  @param str  The string buffer
   *  @param name The name of the value.
   *  @param val  The value.
   */
  public static void append (StringBuilder str, String name, Object val) {
    if (val != null) str.append(name).append(val);
  }

  /** Appends a value to a string buffer if not null.
   *  @param str    The string buffer
   *  @param prefix Prefix to include.
   *  @param val    The value.
   *  @param suffix Suffix to include.
   */
  public static void append (StringBuilder str, String prefix, Object val, String suffix) {
    if (val != null) str.append(prefix).append(val).append(suffix);
  }

  /** Creates a new thread factory that will prefix each thread with the given name. This is
   *  useful when looking at the threads via a profiler.
   *  @param name The name to use.
   *  @return The enw thread factory.
   */
  public static ThreadFactory newNamedThreadFactory (String name) {
    return new NamedThreadFactory(name);
  }

  /** This is a near copy of the {@link java.util.concurrent.Executors#newSingleThreadExecutor()}
   *  method, except that it allows a maximum queue capacity to be specified.
   *
   *  @param  capacity the capacity of this queue
   *  @return The newly created single-threaded Executor
   *  @throws IllegalArgumentException If <tt>capacity</tt> is zero or negative.
   */
  public static AdvancedExecutorService newSingleThreadExecutor (int capacity) {
    ThreadPoolExecutor tpe = new ThreadPoolExecutor(1, 1, 0L, TimeUnit.MILLISECONDS,
                                                    new LinkedBlockingQueue<Runnable>(capacity));
    return new FinalizableDelegatedExecutorService(tpe);
  }

  /** This is a near copy of the {@link java.util.concurrent.Executors#newSingleThreadExecutor(ThreadFactory)}
   *  method, except that it allows a maximum queue capacity to be specified.
   *
   *  @param  capacity      the capacity of this queue
   *  @param  threadFactory the factory to use when creating new threads
   *  @return The newly created single-threaded Executor
   *  @throws IllegalArgumentException If <tt>capacity</tt> is zero or negative.
   */
  public static AdvancedExecutorService newSingleThreadExecutor (int capacity, ThreadFactory threadFactory) {
    ThreadPoolExecutor tpe = new ThreadPoolExecutor(1, 1,  0L, TimeUnit.MILLISECONDS,
                                                    new LinkedBlockingQueue<Runnable>(capacity),
                                                    threadFactory);
    return new FinalizableDelegatedExecutorService(tpe);
  }

  /** Determines if a given character sequence is equal to an expected one or null.
   *  @param expected The expected value (not null).
   *  @param actual   The actual value.
   *  @return <tt>null</tt> if actual is null, <tt>true</tt> if they are equal, <tt>false</tt> otherwise.
   */
  public static Boolean isString (String expected, CharSequence actual) {
    return (actual == null)? null : expected.contentEquals(actual);
  }

  /** Determines if a value is present.
   *  @param str The string to check.
   *  @return <tt>false</tt> if the parameter is null or the empty string, <tt>true</tt> otherwise.
   */
  public static boolean present (CharSequence str) {
    return ((str != null) && (str.length() > 0));
  }

  /** Converts a "boolean" string to a boolean value. The following (case-insensitive) conversions
   *  are used:
   *  <pre>
   *    null:        null,   ""
   *    TRUE:       "true",  "t", "1", "yes", "y", "on"
   *    FALSE:      "false", "f", "0", "no",  "n", "off"
   *  </pre>
   *  @param obj The object to convert.
   *  @return <tt>null</tt>, {@link Boolean#TRUE}, or {@link Boolean#FALSE} as appropriate.
   */
  public static Boolean toBoolean (Object obj) {
    if (obj == null) return null;
    if (obj instanceof Boolean) return (Boolean)obj;

    String s = obj.toString().toLowerCase();
    if (s.length()  ==  0) return null;
    if (s.equals("true" )) return TRUE;
    if (s.equals("false")) return FALSE;
    if (s.equals("t"    )) return TRUE;
    if (s.equals("f"    )) return FALSE;
    if (s.equals("1"    )) return TRUE;
    if (s.equals("0"    )) return FALSE;
    if (s.equals("yes"  )) return TRUE;
    if (s.equals("no"   )) return FALSE;
    if (s.equals("y"    )) return TRUE;
    if (s.equals("n"    )) return FALSE;
    if (s.equals("on"   )) return TRUE;
    if (s.equals("off"  )) return FALSE;
    throw new IllegalArgumentException("Can not convert '"+obj+"' to boolean.");
  }

  /** Converts a "boolean" string to a boolean value. The is identical to {@link #toBoolean}
      except that it returns <tt>false</tt> in place of <tt>null</tt>.
      @param obj The object to convert.
      @return <tt>true</tt> or <tt>false</tt> as appropriate.
   */
  public static boolean toBooleanValue (Object obj) {
    Boolean b = toBoolean(obj);
    return (b == null)? false : b;
  }

  /** Trims excess whitespace off the front and end of a string and then returns <tt>""</tt> if
   *  the result is "NA" or "N/A" (case insensitive).
   *  @param str The input string.
   *  @return The trimmed output or <tt>str</tt> if no trimming is required.
   */
  public static CharSequence trimNA (CharSequence str) {
    str = trim(str);
    if (str.length() == 2) {
      char c0 = str.charAt(0);
      char c1 = str.charAt(1);
      if ((c0=='n' || c0=='N') && (c1=='a' || c1=='A')) return "";
    }
    else if (str.length() == 3) {
      char c0 = str.charAt(0);
      char c1 = str.charAt(1);
      char c2 = str.charAt(2);
      if ((c0=='n' || c0=='N') && (c1 == '/') && (c2=='a' || c2=='A')) return "";
    }
    return str;
  }

  /** Trims excess whitespace off the front and end of a string. This is modeled after
   *  {@link java.lang.String#trim()} and is used within the auto-generated packet code.
   *  @param str The input string.
   *  @return The trimmed output or <tt>str</tt> if no trimming is required.
   */
  public static CharSequence trim (CharSequence str) {
    int length = str.length();
    int start  = 0;
    int end    = length;
    while ((start < end) && (str.charAt(start) <= ' ')) start++;
    while ((start < end) && (str.charAt(end-1) <= ' ')) end--;
    return ((start == 0) && (end == length))? str : str.subSequence(start,end);
  }

  /** Creates a hexdump-like output for a given data buffer. This is identical to
   *  <tt>toHexDump(buffer,offset,length,true,true,true,"\n")</tt>.
   *  @param buffer  The data buffer.
   *  @param offset  The offset into the buffer to start with.
   *  @param length  The number of bytes to display.
   *  @return The hexdump text (this will be an empty character sequence if both <tt>hex</tt> and
   *          <tt>ascii</tt> are false).
   */
  public static CharSequence toHexDump (byte[] buffer, int offset, int length) {
    return toHexDump(buffer,offset,length,true,true,true,"\n");
  }

  /** Creates a hexdump-like output for a given data buffer. This is identical to
   *  <tt>toHexDump(buffer,offset,length,hex,ascii,true,"\n")</tt>.
   *  @param buffer  The data buffer.
   *  @param offset  The offset into the buffer to start with.
   *  @param length  The number of bytes to display.
   *  @param hex     Show hex version?
   *  @param ascii   Show ASCII version?
   *  @return The hexdump text (this will be an empty character sequence if both <tt>hex</tt> and
   *          <tt>ascii</tt> are false).
   */
  public static CharSequence toHexDump (byte[] buffer, int offset, int length,
                                        boolean hex, boolean ascii) {
    return toHexDump(buffer,offset,length,hex,ascii,true,"\n");
  }

  /** Creates a hexdump-like output for a given data buffer.
   *  @param buffer  The data buffer.
   *  @param offset  The offset into the buffer to start with.
   *  @param length  The number of bytes to display.
   *  @param hex     Show hex version?
   *  @param ascii   Show ASCII version?
   *  @param index   Show index values?
   *  @param eol     End of line character (usually "\n").
   *  @return The hexdump text (this will be an empty character sequence if both <tt>hex</tt> and
   *          <tt>ascii</tt> are false).
   */
  public static CharSequence toHexDump (byte[] buffer, int offset, int length,
                                        boolean hex, boolean ascii, boolean index,
                                        String eol) {
    StringBuilder str = new StringBuilder();
    StringBuilder txt = new StringBuilder();

    if (hex) {
      for (int i=offset, j=0; j < length;) {
        if (index) {
          str.append(j).append(":  \t");
        }

        for (int k=0; k < 16; i++,j++,k++) {
          if (j >= length) {
            str.append("   ");
            txt.append(" ");
          }
          else {
            int  b  = buffer[i] & 0xFF;
            char c  = ((b < 0x20) || (b >= 0x7F))? '.' : (char)b;
            int  n1 = b >> 4;
            int  n2 = b & 0xF;
            int  q1 = n1 & ((n1 << 1) | (n1 << 2)) & 0x8; // 0x0 if <0xA, 0x8 if >=0xA
            int  q2 = n2 & ((n2 << 1) | (n2 << 2)) & 0x8; // 0x0 if <0xA, 0x8 if >=0xA
            char c1 = (char)(n1 + 0x30 + q1 - (q1 >> 3)); // q-(q>>3) = 0x8-0x1 = 'A'-'9'-1 if >=0xA
            char c2 = (char)(n2 + 0x30 + q2 - (q2 >> 3)); // q-(q>>3) = 0x8-0x1 = 'A'-'9'-1 if >=0xA
            str.append(c1).append(c2).append(' ');
            txt.append(c);
          }
          if (k == 7) str.append(' ');
        }
        if (ascii) {
          str.append("  |  ").append(txt);
          txt.setLength(0);
        }
        str.append(eol);
      }
    }
    else if (ascii) {
      for (int i=offset, j=0; j < length;) {
        if (index) {
          str.append(j).append(":  \t");
        }

        for (int k=0; k < 80; i++,j++,k++) {
          if (j >= length) {
            str.append(" ");
          }
          else {
            int  b  = buffer[i] & 0xFF;
            char c  = ((b < 0x20) || (b >= 0x7F))? '.' : (char)b;
            str.append(c);
          }
        }
        str.append(eol);
      }
    }
    return str;
  }
  
  /** Quick conversion to hex string. Unlike {@link Integer#toHexString} this returns a string
   *  that is in upper-case and padded with leading zeros.
   *  @param val The value to convert.
   */
  public static String toHexString (int val) {
    return toHexString(val, 4);
  }

  /** Quick conversion to hex string. Unlike {@link Integer#toHexString} this returns a string
   *  that is in upper-case and padded with leading zeros.
   *  @param val The value to convert.
   */
  public static String toHexString (long val) {
    return toHexString(val, 8);
  }

  /** Quick conversion to hex string. Unlike {@link Integer#toHexString} this returns a string
   *  that is in upper-case and padded with leading zeros.
   *  @param val   The value to convert.
   *  @param bytes The number of bytes to consider.
   */
  public static String toHexString (int val, int bytes) {
    char[] chars = new char[bytes*2];

    for (int i = chars.length-1; i >= 0; i--) {
      chars[i] = hexChar[val&0xF];
      val = val >>> 4;
    }
    return new String(chars);
  }
  
  /** Quick conversion to hex string. Unlike {@link Integer#toHexString} this returns a string
   *  that is in upper-case and padded with leading zeros.
   *  @param val   The value to convert.
   *  @param bytes The number of bytes to consider.
   */
  public static String toHexString (long val, int bytes) {
    char[] chars = new char[bytes*2];
    
    for (int i = chars.length-1; i >= 0; i--) {
      chars[i] = hexChar[(int)(val&0xF)];
      val = val >>> 4;
    }
    return new String(chars);
  }
  
  /** Converts the numeric form of an IEEE Organizationally Unique Identifier (OUI) to a string.
   *  @param oui The 24-bit OUI (top 8 bits of the int must be 0x00).
   *  @return The string form of the OUI (IEEE standard format).
   */
  public static String toStringOUI (Integer oui) {
    if (oui == null) return null;
    
    if ((oui & 0xFF000000) != 0) {
      throw new IllegalArgumentException("Invalid OUI ("+oui+").");
    }
    return toHexString((oui >> 16), 1) + "-"
         + toHexString((oui >>  8), 1) + "-"
         + toHexString((oui      ), 1);
  }
  
  /** Converts the string form of an IEEE Organizationally Unique Identifier (OUI) to a numeric.
   *  @param oui The string form of the OUI (IEEE standard format).
   *  @return The 24-bit OUI.
   */
  public static Integer fromStringOUI (CharSequence oui) {
    if (oui == null) return null;
    
    if ((oui.length() != 8) || (oui.charAt(2) != '-') || (oui.charAt(5) != '-')) {
      throw new IllegalArgumentException("Invalid OUI ("+oui+").");
    }
    try {
      return (parseInt(oui.subSequence(0,2).toString(), 16) << 16)
           | (parseInt(oui.subSequence(3,5).toString(), 16) <<  8)
           | (parseInt(oui.subSequence(6,8).toString(), 16)      );
    }
    catch (NumberFormatException e) {
      throw new IllegalArgumentException("Invalid OUI ("+oui+").");
    }
  }
  
  /** Converts the numeric form of a VRT class identifier to a string.
   *  @param id The 56-bit class ID (top 8 bits of the long must be 0x00).
   *  @return The string form of the class identifier.
   */
  public static String toStringClassID (Long id) {
    if (id == null) return null;
    
    int   oui = (int)(id >>> 32);
    short icc = (short)(id >> 16);
    short pcc = id.shortValue();
    
    return toStringClassID(oui, icc, pcc);
  }
  
  /** Converts the numeric form of a VRT class identifier to a string.
   *  @param oui The 24-bit OUI (top 8 bits of the int must be 0x00).
   *  @param icc The 16-bit (unsigned) information class code (ICC).
   *  @param pcc The 16-bit (unsigned) packet class code (PCC).
   *  @return The string form of the class identifier.
   */
  public static String toStringClassID (int oui, short icc, short pcc) {
    return toStringOUI(oui) + ":" + toHexString(icc,2) + "." + toHexString(pcc,2);
  }
  
  /** Converts the string form of a VRT class identifier to a numeric.
   *  @param classID The string form of the class identifier.
   *  @return The 56-bit class ID.
   */
  public static Long fromStringClassID (CharSequence classID) {
    if (classID == null) return null;
    String id = classID.toString();
    
    int i = id.indexOf(':');
    int j = id.indexOf('.');

    if ((id.length() < 12) || (i != 8) || (j < 10)) {
      throw new IllegalArgumentException("Invalid class ID ("+id+").");
    }
    try {
      long oui = 0x00FFFFFF00000000L & (((long)fromStringOUI(id.substring(0,8))) << 32);
      long icc = 0x00000000FFFF0000L & (parseInt(id.substring(9,j), 16)  << 16);
      long pcc = 0x000000000000FFFFL & (parseInt(id.substring(j+1), 16)       );
      return oui|icc|pcc;
    }
    catch (NumberFormatException e) {
      throw new IllegalArgumentException("Invalid class ID ("+id+").");
    }
  }
  
  /** Converts the numeric form of a device identifier to a string.
   *  @param id The 40-bit manufacturer OUI stored in a 64-bit space as:
   *            <tt>0x00xxxxxx0000yyyy</tt> where <tt>xxxxxx</tt> is the 24-bit manufacturer
   *            OUI and <tt>yyyy</tt> is the 16-bit device ID.
   *  @return The string form of the device identifier.
   */
  public static String toStringDeviceID (Long id) {
    if (id == null) return null;
    int   oui = (int)(id >>> 32);
    short dev = id.shortValue();
    
    return toStringDeviceID(oui, dev);
  }
  
  /** Converts the numeric form of a device identifier to a string.
   *  @param oui The 24-bit manufacturer OUI (top 8 bits of the int must be 0x00).
   *  @param dev The 16-bit (unsigned) device identifier.
   *  @return The string form of the device identifier.
   */
  public static String toStringDeviceID (int oui, short dev) {
    return toStringOUI(oui) + ":" + toHexString(dev,2);
  }
  
  /** Converts the string form of a device identifier to a numeric.
   *  @param devID The string form of the device identifier.
   *  @return The numeric form of the device identifier.
   */
  public static Long fromStringDeviceID (CharSequence devID) {
    if (devID == null) return null;
    String id = devID.toString();
    
    int i = id.indexOf(':');

    if ((id.length() < 10) || (i != 8)) {
      throw new IllegalArgumentException("Invalid device ID ("+id+").");
    }
    try {
      long oui = 0x00FFFFFF00000000L & (((long)fromStringOUI(id.substring(0,8))) << 32);
      long dev = 0x000000000000FFFFL & (parseInt(id.substring(9),16));
      return oui|dev;
    }
    catch (NumberFormatException e) {
      throw new IllegalArgumentException("Invalid device ID ("+id+").", e);
    }
  }
  
  /** Normalizes an angle such that it is on the range of [0,360).
   *  @param deg The input angle in degrees.
   *  @return The corresponding angle in the range [0,360); or null if given null.
   */
  public static Double normalizeAngle360 (Double deg) {
    if (deg == null) return null;
    return (deg == null)? null : normalizeAngle360((double)deg);
  }
  
  /** Normalizes an angle such that it is on the range of [0,360).
   *  @param deg The input angle in degrees.
   *  @return The corresponding angle in the range [0,360).
   */
  public static double normalizeAngle360 (double deg) {
    // We add 360 the the input value in an effort to minimize the occurrence
    // of the deg<0 case which is computationally costly due to the extra %360
    // at the end which is necessary prevent a result of 360.0 when deg=-360.0*N
    deg += 360;
    return (deg < 0.0)? (360.0-((-deg) % 360.0)) % 360.0 : deg % 360.0;
  }
  
  /** Normalizes an angle such that it is on the range of [-180,+180).
   *  @param deg The input angle in degrees.
   *  @return The corresponding angle in the range [-180,+180); or null if given null.
   */
  public static Double normalizeAngle180 (Double deg) {
    if (deg == null) return null;
    return normalizeAngle180((double)deg);
  }
  
  /** Normalizes an angle such that it is on the range of [-180,+180).
   *  @param deg The input angle in degrees.
   *  @return The corresponding angle in the range [-180,+180).
   */
  public static double normalizeAngle180 (double deg) {
    return normalizeAngle360(deg + 180.0) - 180.0;
  }
  
  /** This provides the mandatory functionality required for the {@link HasFields#getFieldID}
   *  method. <b>This should only be called by a class that implements {@link HasFields}.</b>
   *  @param self The {@link HasFields} object calling this function.
   *  @param name The field name.
   *  @return     The field ID (on range <tt>[0,getFieldCount())</tt>).
   *  @throws IllegalArgumentException If name is invalid.
   *  @throws NullPointerException     If self or name is null.
   */
  public static int getFieldID (HasFields self, String name) {
    if (name == null) {
      throw new NullPointerException("Invalid field name <null> in "+self.getClass().getName());
    }
    for (int i = 0; i < self.getFieldCount(); i++) {
      if (name.equals(self.getFieldName(i))) return i;
    }
    throw new IllegalArgumentException("Invalid field name '"+name+"' in "+self.getClass().getName());
  }
  
  /** This provides the mandatory functionality required for the {@link HasFields#getFieldByName}
   *  method. <b>This should only be called by a class that implements {@link HasFields}.</b>
   *  @param self The {@link HasFields} object calling this function.
   *  @param name The field name.
   *  @return The given value (see {@link HasFields#getField} for details); may be null.
   *  @throws IllegalArgumentException If name is invalid.
   *  @throws NullPointerException     If self is null.
   */
  public static Object getFieldByName (HasFields self, String name) {
    ParsedFieldName pfn = new ParsedFieldName(name);
    int             id  = getFieldID(self, pfn.first);
    Object          val = self.getField(id);

    if (val      == null  ) return null;        // <-- null value
    if (pfn.next == null  ) return val;         // <-- use current value
    if (pfn.idx  >= 0     ) val = Array.get(val, pfn.idx);

    if (!(val instanceof HasFields)) {
      // Throw exception if trying to get a sub-field on something that doesn't
      // have sub-fields (e.g. a double). If the value was null the above code
      // will hide this error by simply returning null; but there isn't much
      // we can do to avoid this.
      throw new RuntimeException("Can not get '"+name+"' in "+self.getClass().getName()
                                +" when "+pfn.first+" is "+val);
    }
    return getFieldByName((HasFields)val, pfn.next);
  }
  
  /** This provides the mandatory functionality required for the {@link HasFields#setFieldByName}
   *  method. <b>This should only be called by a class that implements {@link HasFields}.</b>
   *  @param self  The {@link HasFields} object calling this function.
   *  @param name  The field name.
   *  @param value The value to set (must be of the proper type).
   *  @throws IllegalArgumentException If name is invalid.
   *  @throws NullPointerException     If self is null.
   */
  public static void setFieldByName (HasFields self, String name, Object value) {
    ParsedFieldName pfn = new ParsedFieldName(name);
    int             id  = getFieldID(self, pfn.first);

    if ((pfn.idx < 0) && (pfn.next == null)) { // no next, no idx
      self.setField(id, value);
      return;
    }

    if (pfn.idx < 0) { // next only, no idx
      Object val = self.getField(id);
      
      if (!(val instanceof HasFields)) {
        throw new RuntimeException("Can not set '"+name+"' in "+self.getClass().getName()
                                  +" when "+pfn.first+" is "+val);
        
      }
      setFieldByName((HasFields)val,pfn.next, value);
      self.setField(id, val);
      return;
    }

    if (pfn.next == null) { // idx only, no next
      Object   val  = self.getField(id);
      Array.set(val, id, castValueForArray(val.getClass(), value));
      self.setField(id, val);
      return;
    }

    // idx and next
    Object val = self.getField(id);
    if (val == null) {
      throw new RuntimeException("Can not set '"+name+"' in "+self.getClass().getName()
                                +" when "+pfn.first+" is "+val);
    }
    Object v = Array.get(val, pfn.idx);
    if (!(v instanceof HasFields)) {
      throw new RuntimeException("Can not set '"+name+"' in "+self.getClass().getName()
                                +" when "+pfn.first+" is "+val);
    }
    setFieldByName((HasFields)v, pfn.next, value);
    Array.set(val, pfn.idx, castValueForArray(val.getClass(), v));
    self.setField(id, val);
  }
  
  /** Indicates if a given class is a subclass of {@link Number} or if it's boxed
   *  version is. For example <tt>Integer.TYPE</tt> and <tt>Integer.class</tt> will
   *  both return true since their object forms extend {@link Number}.
   */
  public static boolean isNumericType (Class<?> clazz) {
    return Number.class.isAssignableFrom(clazz)
        || (clazz == Byte.TYPE   )
        || (clazz == Short.TYPE  )
        || (clazz == Integer.TYPE)
        || (clazz == Long.TYPE   )
        || (clazz == Double.TYPE );
  }
  
  /** <b>Internal Use Only:</b> Variant of castValue(..) where input type is an
   *  array type and the value is to be an element in that array.
   */
  private static Object castValueForArray (Class<?> clazz, Object obj) {
    if (!clazz.isArray()) {
      throw new RuntimeException("Expected an array type but found "+clazz+" when "
                                +"trying to set "+obj);
    }
    return castValue(clazz.getComponentType(), obj);
  }
  
  /** Casts an unknown object to the given type. This will support the following
   *  conversions which aren't strictly a cast:
   *  <pre>
   *    Desired ClassType | Given Object
   *    ------------------+-----------------------------------------------------
   *    any               | null (or 0 for primitive types)
   *    ------------------+-----------------------------------------------------
   *    Byte, Short,      | Any instance of Number
   *    Integer, Long     |
   *    Float, Double     |
   *    <i>(class or type)</i>   |
   *    ------------------+-----------------------------------------------------
   *    Boolean.TYPE      | Any number (0=false, 1=true)
   *    ------------------+-----------------------------------------------------
   *    Boolean.class     | Any number (-1=false, 0=null, +1=true)
   *    ------------------+-----------------------------------------------------
   *    String            | Any instance of Object (uses Object.toString())
   *    ------------------+-----------------------------------------------------
   *    CharSequence      | Any non-CharSequence   (uses Object.toString())
   *    ------------------+-----------------------------------------------------
   *    AsciiCharSequence | Any non-AsciiCharSequence (converts to CharSequence
   *                      | then AsciiCharSequence.fromCharSequence(..))
   *    ------------------+-----------------------------------------------------
   *    T                 | HasFieldsWrapper&lt;T&gt;
   *    ------------------+-----------------------------------------------------
   *  </pre>
   *  @param clazz This must be one of the primitive numeric types or its boxed
   *               form (e.g. <tt>Integer.TYPE</tt> or <tt>Integer.class</tt>).
   *  @param obj   The object to cast (may be null).
   *  @return The value converted as applicable.
   */
  @SuppressWarnings("unchecked")
  public static <T> T castValue (Class<T> clazz, Object obj) {
    Object val = _castValue(clazz, obj);
    if (clazz.isPrimitive()) {
      if (clazz == Byte.TYPE     ) return (T)Byte.class.cast(val);
      if (clazz == Short.TYPE    ) return (T)Short.class.cast(val);
      if (clazz == Integer.TYPE  ) return (T)Integer.class.cast(val);
      if (clazz == Long.TYPE     ) return (T)Long.class.cast(val);
      if (clazz == Float.TYPE    ) return (T)Float.class.cast(val);
      if (clazz == Double.TYPE   ) return (T)Double.class.cast(val);
      if (clazz == Boolean.TYPE  ) return (T)Boolean.class.cast(val);
      if (clazz == Character.TYPE) return (T)Character.class.cast(val);
    }
    return clazz.cast(val);
  }
  
  /** This is identical to <tt>castValue(..)</tt> except for returning Object. */
  private static <T> Object _castValue (Class<T> clazz, Object obj) {
    if (obj == null) {
      if (clazz == Byte.TYPE     ) return Byte.valueOf((byte)0);
      if (clazz == Short.TYPE    ) return Short.valueOf((short)0);
      if (clazz == Integer.TYPE  ) return Integer.valueOf(0);
      if (clazz == Long.TYPE     ) return Long.valueOf(0L);
      if (clazz == Float.TYPE    ) return Float.valueOf(0f);
      if (clazz == Double.TYPE   ) return Double.valueOf(0d);
      if (clazz == Boolean.TYPE  ) return Boolean.FALSE;
      if (clazz == Character.TYPE) return Character.valueOf('\0');
      return null;
    }
    if (clazz.isInstance(obj)) {
      return obj;
    }
    if (obj instanceof HasFieldsWrapper) {
      Object wrapped = ((HasFieldsWrapper<?>)obj).getWrappedObject();
      if (clazz.isInstance(wrapped)) {
        return wrapped;
      }
    }
    if (obj instanceof Number) {
      Number num = (Number)obj;

      if (clazz.isPrimitive()) {
        if (clazz == Byte.TYPE   ) return Byte.valueOf(num.byteValue());
        if (clazz == Short.TYPE  ) return Short.valueOf(num.shortValue());
        if (clazz == Integer.TYPE) return Integer.valueOf(num.intValue());
        if (clazz == Long.TYPE   ) return Long.valueOf(num.longValue());
        if (clazz == Float.TYPE  ) return Float.valueOf(num.floatValue());
        if (clazz == Double.TYPE ) return Double.valueOf(num.doubleValue());
      
        if (clazz == Boolean.TYPE) {
          double sign = Math.signum(num.doubleValue());
          if (sign > 0) return Boolean.TRUE;
          if (sign < 0) return Boolean.TRUE;
          return Boolean.FALSE;
        }
      }
      else {
        if (clazz == Byte.class   ) return Byte.valueOf(num.byteValue());
        if (clazz == Short.class  ) return Short.valueOf(num.shortValue());
        if (clazz == Integer.class) return Integer.valueOf(num.intValue());
        if (clazz == Long.class   ) return Long.valueOf(num.longValue());
        if (clazz == Float.class  ) return Float.valueOf(num.floatValue());
        if (clazz == Double.class ) return Double.valueOf(num.doubleValue());
        
        if (clazz == Boolean.class) {
          double sign = Math.signum(num.doubleValue());
          if (sign > 0) return Boolean.TRUE;
          if (sign < 0) return Boolean.FALSE;
          return null;
        }
      }
    }
    if ((clazz == Boolean.TYPE) && (obj instanceof Boolean)) {
      return obj; // Boolean -> boolean
    }
    if ((clazz == Character.TYPE) && (obj instanceof Character)) {
      return obj; // Character -> char
    }
    if ((clazz == String.class) || (clazz == CharSequence.class)) {
      return obj.toString();
    }
    if (clazz == AsciiCharSequence.class) {
      if (obj instanceof CharSequence) {
        return AsciiCharSequence.fromCharSequence((CharSequence)obj);
      }
      else {
        return AsciiCharSequence.fromCharSequence(obj.toString());
      }
    }
    throw new ClassCastException("Can not convert "+obj.getClass()+" to "+clazz+" given "+obj);
  }
  
  /** Wraps an {@link InetAddress} as a {@link HasFields} instance to match the
   *  C++ version where <tt>InetAddress</tt> implements {@link HasFields}.
   */
  public static HasFieldsWrapper<InetAddress> asHasFields (InetAddress o) {
    return new HasFieldsWrapper<InetAddress>(o, "HostAddress", String.class) {
      @Override
      public Object getField (int id) {
        switch (id) {
          case  0: return super.obj.getHostAddress();
          default: throw new IllegalArgumentException("Invalid field #"+id+" in "+getClass());
        }
      }

      @Override
      public void setField (int id, Object val) {
        try {
          switch (id) {
            case  0: super.obj = InetAddress.getByName(castValue(String.class, val)); break;
            default: throw new IllegalArgumentException("Invalid field #"+id+" in "+getClass());
          }
        }
        catch (UnknownHostException e) {
          throw new IllegalArgumentException("Invalid HostAddress '"+val+"'", e);
        }
      }
    };
  }
  
  /** Wraps a {@link UUID} as a {@link HasFields} instance to match the
   *  C++ version where <tt>UUID</tt> implements {@link HasFields}.
   */
  public static HasFieldsWrapper<UUID> asHasFields (UUID o) {
    return new HasFieldsWrapper<UUID>(o, "UUID", String.class) {
      @Override
      public Object getField (int id) {
        switch (id) {
          case  0: return toString();
          default: throw new IllegalArgumentException("Invalid field #"+id+" in "+getClass());
        }
      }

      @Override
      public void setField (int id, Object val) {
        switch (id) {
          case  0: super.obj = UUID.fromString(castValue(String.class, val)); break;
          default: throw new IllegalArgumentException("Invalid field #"+id+" in "+getClass());
        }
      }
    };
  }
  
  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Nested classes
  //////////////////////////////////////////////////////////////////////////////////////////////////
  /** A thread factory creating threads with the specified name. */
  private static final class NamedThreadFactory implements ThreadFactory {
    private final String name;
    private       int    counter;
    
    public NamedThreadFactory (String name) {
      this.name    = name;
      this.counter = 0;
    }
    
    @Override
    public synchronized Thread newThread (Runnable r) {
      return new Thread(r, name+"-"+(counter++));
    }
  }


  /** A slightly more advanced executor service. */
  public static interface AdvancedExecutorService extends ExecutorService {
    /** Gets the executor used. */
    public ThreadPoolExecutor getThreadPoolExecutor ();
  }

  /** A near-copy of the version in {@link java.util.concurrent.Executors} except that it
   *  implements AdvancedExecutorService.
   */
  private static final class FinalizableDelegatedExecutorService extends AbstractExecutorService
                                                                 implements AdvancedExecutorService {
    private final ThreadPoolExecutor e;

    /** Creates a new instance. */
    FinalizableDelegatedExecutorService (ThreadPoolExecutor executor) {
      this.e = executor;
    }

    @Override public void               execute      (Runnable    task) {        e.execute(task);  }
    @Override public ThreadPoolExecutor getThreadPoolExecutor (       ) { return e;                }
    @Override public boolean            isShutdown   (                ) { return e.isShutdown();   }
    @Override public boolean            isTerminated (                ) { return e.isTerminated(); }
    @Override public void               shutdown     (                ) {        e.shutdown();     }
    @Override public List<Runnable>     shutdownNow  (                ) { return e.shutdownNow();  }
    @Override public Future<?>          submit       (Runnable    task) { return e.submit(task);   }
    @Override public <T> Future<T>      submit       (Callable<T> task) { return e.submit(task);   }

    @Override
    public boolean awaitTermination (long timeout, TimeUnit unit) throws InterruptedException {
      return e.awaitTermination(timeout, unit);
    }
    
    @SuppressWarnings("FinalizeDeclaration")
    @Override
    protected void finalize () throws Throwable {
      shutdown();
      super.finalize();
    }

    @Override
    public <T> List<Future<T>> invokeAll (Collection<? extends Callable<T>> tasks) throws InterruptedException {
      return e.invokeAll(tasks);
    }

    @Override
    public <T> List<Future<T>> invokeAll (Collection<? extends Callable<T>> tasks, long timeout, TimeUnit unit)
                                         throws InterruptedException {
      return e.invokeAll(tasks, timeout, unit);
    }

    @Override
    public <T> T invokeAny (Collection<? extends Callable<T>> tasks) throws InterruptedException, ExecutionException {
      return e.invokeAny(tasks);
    }

    @Override
    public <T> T invokeAny (Collection<? extends Callable<T>> tasks, long timeout, TimeUnit unit)
                           throws InterruptedException, ExecutionException, TimeoutException {
      return e.invokeAny(tasks, timeout, unit);
    }

    @Override
    public <T> Future<T> submit (Runnable task, T result) {
      return e.submit(task, result);
    }
  }
  
  /** Internal use only: A parsed field name. */
  private static final class ParsedFieldName {
    /** Internal use only. */ public final String first;
    /** Internal use only. */ public final int    idx;
    /** Internal use only. */ public final String next;

    /** Internal use only. */
    public ParsedFieldName (String name) {
      if (name == null) {
        throw new NullPointerException("Invalid field name <null>");
      }
      int dotIndex   = name.indexOf('.');
      int startIndex = name.indexOf('[');

      if ((dotIndex < 0) && (startIndex < 0)) {
        first = name;
        idx   = -1;
        next  = null;
      }
      else if (startIndex < 0) {
        first = name.substring(0,dotIndex);
        idx   = -1;
        next  = name.substring(dotIndex+1);
      }
      else if ((dotIndex < 0) ||  (startIndex < dotIndex)) {
        int endIndex = name.indexOf(']');

        if ((endIndex < 0) || (endIndex < startIndex)) {
          throw new IllegalArgumentException("Invalid field name '"+name+"'");
        }

        String num = name.substring(startIndex+1,endIndex);

        first = name.substring(0,startIndex);
        idx   = Integer.parseInt(num);
        next  = name.substring(endIndex+1);
      }
      else {
        first = name.substring(0,dotIndex);
        idx   = -1;
        next  = name.substring(dotIndex+1);
      }
    }
  }
  
  /** Abstract HasFields wrapper class. */
  public static abstract class HasFieldsWrapper<T> implements HasFields {
    private       T        obj; // The wrapped object
    private final Object[] fieldDefs;
    
    /** Creates a new instance.
     *  @param obj       The wrapped object.
     *  @param fieldDefs The field definitions. This should be an array of
     *                   { name0, type0, name1, type1, ..., nameN, typeN }.
     */
    HasFieldsWrapper (T obj, Object ... fieldDefs) {
      this.obj       = obj;
      this.fieldDefs = fieldDefs;
    }
    
    /** Gets the wrapped object. */
    public T getWrappedObject () {
      return obj;
    }
    
    @Override
    public String toString () {
      return obj.toString();
    }
    
    @Override
    public boolean equals (Object o) {
      return (o instanceof HasFieldsWrapper)
          && (obj.equals(((HasFieldsWrapper)o).obj));
    }
    
    @Override
    public int hashCode () {
      return obj.hashCode();
    }
    
    @Override
    public int getFieldCount () {
      return fieldDefs.length / 2;
    }
    
    @Override
    public String getFieldName (int id) {
      int i = id * 2;
      if ((i >= 0) && (i < fieldDefs.length)) {
        return (String)fieldDefs[i];
      }
      throw new IllegalArgumentException("Invalid field #"+id+" in "+getClass());
    }
    
    @Override
    public Class<?> getFieldType (int id) {
      int i = (id * 2) + 1;
      if ((i >= 1) && (i < fieldDefs.length)) {
        return (Class<?>)fieldDefs[i];
      }
      throw new IllegalArgumentException("Invalid field #"+id+" in "+getClass());
    }
    
    @Override
    public final int getFieldID (String name) {
      return Utilities.getFieldID(this, name);
    }
    
    
    @Override
    public final Object getFieldByName (String name) {
      return Utilities.getFieldByName(this, name);
    }
    
    @Override
    public final void setFieldByName (String name, Object val) {
      Utilities.setFieldByName(this, name, val);
    }
  }
}

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

/** An immutable {@link CharSequence} that uses ASCII characters in a <tt>byte[]</tt> array for
 *  its source. Any non-ASCII characters are replaced by '?' (ASCII control characters will not be
 *  replaced by '?').
 *  @author 
 */
public final class AsciiCharSequence implements CharSequence {
  private static final Byte QUESTION_MARK = (byte)'?';

  private final     byte[] chars;  // The ASCII chars
  private final     int    offset; // The offset into the array
  private final     int    length; // The length of the sequence
  private transient String string; // The results of calling toString() saved for fast reuse

  /** Fast private constructor. */
  private AsciiCharSequence (byte[] chars, int offset, int length, String string) {
    this.chars  = chars;
    this.offset = offset;
    this.length = length;
    this.string = string;
  }

  /** Creates a new instance.
      @param chars   The ASCII chars in a <tt>byte[]</tt> array.
      @param offset  The offset into the <tt>chars</tt> array.
      @param length  The number of chars in the <tt>chars</tt> array starting at <tt>offset</tt>.
      @throws ArrayIndexOutOfBoundsException If <tt>offset</tt> or <tt>length</tt> are invalid.
   */
  public AsciiCharSequence (byte[] chars, int offset, int length) {
    this(chars, offset, length, false);
  }

  /** Creates a new instance.
      @param chars    The ASCII chars in a <tt>byte[]</tt> array.
      @param offset   The offset into the <tt>chars</tt> array.
      @param length   The number of chars in the <tt>chars</tt> array starting at <tt>offset</tt>.
      @param nullTerm If <tt>true</tt> assume a NUL-terminated string and reduce the length to N
                      where N is the index of the first NUL (if any).
      @throws ArrayIndexOutOfBoundsException If <tt>offset</tt> or <tt>length</tt> are invalid.
   */
  public AsciiCharSequence (byte[] chars, int offset, int length, boolean nullTerm) {
    super();
    int max = chars.length - offset;
    
    if (offset <  0 ) throw new ArrayIndexOutOfBoundsException("Invalid offset ("+offset+")");
    if (length <  0 ) throw new ArrayIndexOutOfBoundsException("Invalid length ("+length+")");
    if (length > max) throw new ArrayIndexOutOfBoundsException("Invalid length ("+length+")");

    if (nullTerm) {
      for (int i=0, j=offset; i < length; i++,j++) {
        if (chars[j] == 0) length = i; // trim length at first null
      }
    }

    this.chars  = new byte[length];   System.arraycopy(chars, offset, this.chars, 0, length);
    this.offset = 0;
    this.length = length;
  }

  /** Creates a new instance, but does not copy the values. <b>This should only be used in cases
      where the values of <tt>chars[offset]</tt> through <tt>chars[offset+length-1]</tt> can be
      guaranteed to be fixed throughout the life of this object. Failure to follow this will result
      in undefined behavior.</b>
      @param chars   The ASCII chars in a <tt>byte[]</tt> array.
      @param offset  The offset into the <tt>chars</tt> array.
      @param length  The number of chars in the <tt>chars</tt> array starting at <tt>offset</tt>.
      @return An applicable {@link AsciiCharSequence}
      @throws ArrayIndexOutOfBoundsException If <tt>offset</tt> or <tt>length</tt> are invalid.
   */
  public static AsciiCharSequence wrap (byte[] chars, int offset, int length) {
    int max = chars.length - offset;

    if (offset <  0 ) throw new ArrayIndexOutOfBoundsException("Invalid offset ("+offset+")");
    if (length <  0 ) throw new ArrayIndexOutOfBoundsException("Invalid length ("+length+")");
    if (length > max) throw new ArrayIndexOutOfBoundsException("Invalid length ("+length+")");

    return new AsciiCharSequence(chars, offset, length, null);
  }

  /** Creates a string using the values in the array.
      @param chars   The ASCII chars in a <tt>byte[]</tt> array.
      @param offset  The offset into the <tt>chars</tt> array.
      @param length  The number of chars in the <tt>chars</tt> array starting at <tt>offset</tt>.
      @return An applicable {@link AsciiCharSequence}
      @throws ArrayIndexOutOfBoundsException If <tt>offset</tt> or <tt>length</tt> are invalid.
   */
  public static String toString (byte[] chars, int offset, int length) {
    return wrap(chars, offset, length).toString();
  }
  
  /** Converts a string to an ASCII string. This is identical to <tt>fromCharSequence(seq,(byte)'?')</tt>.
      @param seq The char sequence to convert.
      @return An applicable {@link AsciiCharSequence}
   */
  public static AsciiCharSequence fromCharSequence (CharSequence seq) {
    return fromCharSequence(seq, QUESTION_MARK);
  }

  /** Converts a string to an ASCII string. 
      @param seq         The char sequence to convert.
      @param replacement The replacement character to use if a non-ASCII character is encountered
                         (typically '?').
      @return An applicable {@link AsciiCharSequence}
      @throws IllegalArgumentException If <tt>replacement</tt> is null and a non-ASCII character is
              encountered.
   */
  public static AsciiCharSequence fromCharSequence (CharSequence seq, Byte replacement) {
    if (seq instanceof AsciiCharSequence) return (AsciiCharSequence)seq;

    byte[] chars = new byte[seq.length()];
    for (int i = 0; i < chars.length; i++) {
      int c = seq.charAt(i);
      if (c <= 0x7F) {
        chars[i] = (byte)c;
      }
      else if (replacement != null) {
        chars[i] = replacement;
      }
      else {
        throw new IllegalArgumentException("Non-ASCII string given '"+seq+"'");
      }
    }
    return wrap(chars, 0, chars.length);
  }
  
  @Override
  public int length () {
    return length;
  }

  @Override
  public char charAt (int index) {
    char c = (char)chars[offset + index];
    return (c <= 0x7F)? c : '?';
  }

  @Override
  public CharSequence subSequence (int start, int end) {
    String str = (string == null)? null : string.substring(start, end);
    return new AsciiCharSequence(chars, offset + start, end-start, str);
  }

  @Override
  public String toString () {
    if (string == null) {
      StringBuilder str = new StringBuilder();
      string = str.append(this).toString();
    }
    return string;
  }

  @Override
  public int hashCode () {
    int hash = length;
    for (int i = 0; (i < length()) && (i < 32); i++) {
      hash ^= (charAt(i) << i);
    }
    return hash;
  }

  @Override
  public boolean equals (Object obj) {
    return (obj instanceof AsciiCharSequence) && contentEquals((AsciiCharSequence)obj);
  }

  /** Compares this {@link AsciiCharSequence} to an arbitrary {@link CharSequence}. This method
   *  will return <tt>true</tt> if all the character values represented by this object match the
   *  one passed in. <i>This method was modeled after {@link String#contentEquals(CharSequence)}.
   *  @param  cs The sequence to compare this object.
   *  @return <tt>true</tt> if this object represents the same sequence of char values as the one
   *          passed in, otherwise <tt>false</tt> is returned.
   */
  public boolean contentEquals (CharSequence cs) {
    if (cs ==  null ) return false;
    if (cs ==  this ) return true;
    if (cs == string) return true;   // <-- must come after "if (cs == null)" test
    
    if (length != cs.length()) return false;
    if (cs instanceof AsciiCharSequence) {
      AsciiCharSequence acs = (AsciiCharSequence)cs;
      if ((this.offset == acs.offset) && (this.chars == acs.chars)) return true; // same array, same place
    }
    
    for (int i = 0; i < length; i++) {
      if (this.charAt(i) != cs.charAt(i)) return false;
    }
    return true;
  }
}

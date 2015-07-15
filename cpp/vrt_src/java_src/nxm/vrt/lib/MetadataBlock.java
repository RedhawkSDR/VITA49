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

import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;

import static java.lang.Math.max;
import static java.util.Arrays.fill;

/**  Class to support the Binary Metadata Language. Each entry has three pieces of information:
    <ul>
      <li>LEVEL - The level applicable to the entry (0 if n/a) (unsigned integer 0..4095).</li>
      <li>KEY   - The key or tag for the entry. (ASCII 0..32767 characters or NULL)</li>
      <li>VALUE - The key or tag for the entry. (UTF-8 0..2147483647 characters or NULL)
          <i>(Most implementations limit the value length to 32767)</i></li>
    </ul>
     Each entry has a 2-/4-/8-octet header describing the entry followed by the octets comprising
    the KEY followed by the octets comprising the VALUE.
    <pre>
       NULL FORM (1-byte)
         +-----------------+
         | 0 0 0 0 0 0 0 0 | (no data, just an ignored byte, typically at the end of a field)
         +-----------------+

       SHORT FORM (2-byte)
         +-----------------+-----------------+
         | 1 0 N N K K K K | L L L L L L L L |
         +-----------------+-----------------+

       STANDARD FORM (4-byte)
         +-----------------+-----------------+-----------------+-----------------+
         | 1 1 0 N N N N N | K K K K K K K K | L L L L L L L L | L L L L L L L L |
         +-----------------+-----------------+-----------------+-----------------+

       LONG FORM (8-byte)
         +-----------------+-----------------+-----------------+-----------------+
         | 1 1 1 0 N N N N | N N N N N N N N | 0 K K K K K K K | K K K K K K K K |
         +-----------------+-----------------+-----------------+-----------------+

         +-----------------+-----------------+-----------------+-----------------+
         | 0 L L L L L L L | L L L L L L L L | L L L L L L L L | L L L L L L L L |
         +-----------------+-----------------+-----------------+-----------------+

       0 - Zero (required)
       1 - One (required)
       N - Level
       K - Key Length
       L - Total Length
    </pre>
     The length of the KEY shall represent the number of octets reserved for holding the
    NUL-terminated KEY. A non-null key may contain mode octets than the minimum required. The null
    value shall be indicated by a KEY length of 0. An empty string shall be encoded by a KEY length
    of at least one where the first octet is the NUL terminator. <br>
    <br>
     The length of the VALUE shall be computed by subtracting the length of the KEY and the
    length of the header from the total length. The VALUE shall be NUL-terminated where a VALUE with
    length 0 shall represent the null value and an empty string shall be encoded with a length
    of at least 1 where the first octet is the NUL terminator.
 
    @author         
 */
public final class MetadataBlock implements Iterable<MetadataBlock.MetadataEntry> {
  private static final int    NULL_FORM_LEN     = 1;
  private static final int    SHORT_FORM_LEN    = 2;
  private static final int    STANDARD_FORM_LEN = 4;
  private static final int    LONG_FORM_LEN     = 8;
  private static final byte[] EMPTY_BUF         = new byte[0];

  private List<MetadataEntry> entries;

  /**  Creates a new uninitialized instance. */
  public MetadataBlock () {
    this.entries = new LinkedList<MetadataEntry>();
  }

  /**  Creates a new instance with the given entries. */
  public MetadataBlock (List<MetadataEntry> entries) {
    this.entries = entries;
  }

  /**  Prints the entry in string form. */
  @Override
  public String toString () {
    StringBuilder str = new StringBuilder();
    str.append("MetadataBlock:");
    for (MetadataEntry e : entries) {
      str.append("\n  ");
      for (int i = 1; i < e.getLevel(); i++) {
        str.append(" | ");
      }
      str.append(" +- ").append(e.getKey()).append("=").append(e.getValue());
    }
    return str.toString();
  }

  /**  Gets the length of the metadata block in bytes. This method is intended for use during
      space allocation, frequent use of this method is discouraged since this method must assess
      the length of each metadata element and that requires scanning through the metadata values
      and assessing the number of bytes required when packing as UTF-8.
   */
  public int getLengthInBytes () {
    return writeBytes(EMPTY_BUF, 0, 0);
  }
  
  /**  Gets a copy of the entries in the metadata block. */
  public List<MetadataEntry> getEntries () {
    return entries;
  }

  /**  Sets the metadata block to match the specified entries. */
  public void setEntries (List<MetadataEntry> entries) {
    this.entries = entries;
  }

  /**  Returns an iterator that will iterate over the entries in the metatata block. */
  @Override
  public Iterator<MetadataEntry> iterator () {
    return getEntries().iterator();
  }

  /**  Reads the entries from a byte buffer. Any null entries encountered will be discarded.
      @param buf  The buffer.
      @param off  Offset into the buffer.
      @param len  Length in the byte buffer.
      @throws IllegalArgumentException If the block is invalid.
   */
  public void readBytes (byte[] buf, int off, int len) {
    readBytes(buf, off, len, false);
  }

  /**  Reads the entries from a byte buffer.
      @param buf      The buffer.
      @param off      Offset into the buffer.
      @param len      Length in the byte buffer.
      @param keepNull Keep null entries? (This is usually false.)
      @throws IllegalArgumentException If the block is invalid.
   */
  public void readBytes (byte[] buf, int off, int len, boolean keepNull) {
    List<MetadataEntry> list = new LinkedList<MetadataEntry>();

    int end = off + len;
    while (off < end) {
      MetadataEntry e = new MetadataEntry();
      int num = e.readBytes(buf, off, len);
      off += num;
      len -= num;
      if (keepNull || !e.isNull()) {
        list.add(e);
      }
    }
    this.entries = list;
  }

  /**  Writes the entries to a byte buffer.
      @param buf  The buffer.
      @param off  Offset into the buffer.
      @param len  Length in the byte buffer.
      @return The number of bytes required within the byte buffer. If there are insufficient bytes
              to include the entire metadata block this will return a number larger than the length
              passed in.
   */
  public int writeBytes (byte[] buf, int off, int len) {
    int length = 0;

    for (MetadataEntry e : entries) {
      int num = e.writeBytes(buf, off, len);
      if (num < 0) {
        off = 0;
        len = 0;
        length += -num;
      }
      else {
        off += num;
        len -= num;
        length += num;
      }
    }
    fill(buf, off, off+len, (byte)0);
    return length;
  }

  /**  A metadata entry in the metadata block. */
  public final static class MetadataEntry {
    private int               level;  // The level
    private AsciiCharSequence key;    // The key
    private CharSequence      val;    // The value

    /**  Creates a new uninitialized (null) instance. */
    public MetadataEntry () {
      this.level = -1;
      this.key   = null;
      this.val   = null;
    }

    /**  Creates a new instance with the given entries.
       @param level The level.
       @param key   The key.
       @param val   The value.
     */
    public MetadataEntry (int level, AsciiCharSequence key, CharSequence val) {
      this();
      this.setLevel(level);
      this.setKey(key);
      this.setValue(val);
    }

    /**  Prints the entry in string form. */
    @Override
    public String toString () {
      return ""+level+": "+key+"="+val;
    }

    /**  Is this a null-form packet? */
    boolean isNull () {
      return (level == -1);
    }

    /**  Gets the level of the entry. */
    public int getLevel () {
      return level;
    }

    /**  Sets the level of the entry. */
    public void setLevel (int level) {
      if ((level < 0) || (level > 4095)) {
        throw new IllegalArgumentException("Invalid level ("+level+"), value must be in range [0,4095]");
      }
      this.level = level;
    }
    
    /**  Gets the key name. */
    public AsciiCharSequence getKey () {
      return key;
    }

    /**  Sets the key name. */
    public void setKey (AsciiCharSequence key) {
      if (key == null) {
        this.key = null;
      }
      else {
        AsciiCharSequence k = AsciiCharSequence.fromCharSequence(key);
        if (k.length() > 32767) {
          throw new IllegalArgumentException("Given key ("+key+"), exceeds 32767 length limit.");
        }
        this.key = k;
      }
    }
    
    /**  Gets the value. */
    public CharSequence getValue () {
      return val;
    }

    /**  Gets the value. */
    public void setValue (CharSequence val) {
      if (val == null) {
        this.val = null;
      }
      else {
        int lval = VRTMath.packUTF8(EMPTY_BUF, 0, val, 0);
        if (lval > 32767) {
          throw new IllegalArgumentException("Given value ("+val+"), exceeds 32767 length limit.");
        }
        this.val = null;
      }
    }

    /**  Writes the entry to a byte buffer.
        @param buf  The buffer.
        @param off  Offset into the buffer.
        @param len  Length in the byte buffer.
        @return The number of bytes required within the byte buffer. If there are insufficient bytes
                to include the entire metadata block this will return a number larger than the length
                passed in.
     */
    public int writeBytes (byte[] buf, int off, int len) {
      int keyLength = (key == null)? 0 : max(1, key.length());
      int valLength = (val == null)? 0 : max(1, VRTMath.packUTF8(EMPTY_BUF, 0, val, 0));

      int headLength;
      if (level == -1) {
        headLength = NULL_FORM_LEN;
        keyLength  = 0; // <-- make sure this is written as null
        valLength  = 0; // <-- make sure this is written as null
      }
      else if ((level > 0x1F) || (keyLength > 0xFF) || (valLength + keyLength + 8 > 0xFFFF)) {
        headLength = LONG_FORM_LEN;
      }
      else if ((level > 0x03) || (keyLength > 0x0F) || (valLength+keyLength+4 > 0xFF)) {
        headLength = STANDARD_FORM_LEN;
      }
      else {
        headLength = SHORT_FORM_LEN;
      }

      int totalLength = headLength+keyLength+valLength;
      if (totalLength > len) return -totalLength;

      switch (headLength) {
        case NULL_FORM_LEN:
          buf[ off ] = 0;
          break;
        case SHORT_FORM_LEN:
          buf[ off ] = (byte)(((level & 0x3) << 4) | (keyLength & 0x0F));
          buf[off+1] = (byte)(totalLength & 0xFF);
          break;
        case STANDARD_FORM_LEN:
          buf[ off ] = (byte)(0xC0 | (level & 0x1F));
          buf[off+1] = (byte)(keyLength & 0xFF);
          buf[off+2] = (byte)((totalLength >> 8) & 0xFF);
          buf[off+3] = (byte)(  totalLength      & 0xFF);
          break;
        case LONG_FORM_LEN:
          buf[ off ] = (byte)(0xC0 | ((level >> 8) & 0x0F));
          buf[off+1] = (byte)(level & 0xFF);
          buf[off+2] = (byte)((keyLength   >>  8) & 0xFF);
          buf[off+3] = (byte)( keyLength          & 0xFF);
          buf[off+4] = (byte)((totalLength >> 24) & 0xFF);
          buf[off+5] = (byte)((totalLength >> 16) & 0xFF);
          buf[off+6] = (byte)((totalLength >>  8) & 0xFF);
          buf[off+7] = (byte)(  totalLength       & 0xFF);
          break;
      }

      if (keyLength > 0) VRTMath.packAscii(buf, off+headLength,           key, keyLength);
      if (valLength > 0) VRTMath.packUTF8( buf, off+headLength+keyLength, val, valLength);
      return totalLength;
    }

    /**  Reads the entry from a byte buffer.
        @param buf  The buffer.
        @param off  Offset into the buffer.
        @param len  Length in the byte buffer.
        @throws IllegalArgumentException If the block is invalid.
     */
    public int readBytes (byte[] buf, int off, int len) {
      if (len < 1) throw new IllegalArgumentException("Unreadable metadata block (invalid length).");
      int headLength;

           if ((buf[off] & 0x80) == 0x00) headLength = NULL_FORM_LEN;
      else if ((buf[off] & 0xC0) == 0x80) headLength = SHORT_FORM_LEN;
      else if ((buf[off] & 0xE0) == 0xC0) headLength = STANDARD_FORM_LEN;
      else if ((buf[off] & 0xF0) == 0xE0) headLength = LONG_FORM_LEN;
      else throw new IllegalArgumentException("Unreadable metadata block (invalid entry header).");

      if (len < headLength) throw new IllegalArgumentException("Unreadable metadata block (invalid length).");

      int keyLength;
      int totalLength;
      switch (headLength) {
        case NULL_FORM_LEN:
          level       = 0;
          keyLength   = 0;
          totalLength = 1;
          break;
        case SHORT_FORM_LEN:
          level       = (buf[ off ] >> 4) & 0x07;
          keyLength   =  buf[ off ]       & 0x0F;
          totalLength =  buf[off+1]       & 0xFF;
          break;
        case STANDARD_FORM_LEN:
          level       = buf[ off ] & 0x1F;
          keyLength   = buf[off+1] & 0xFF;
          totalLength = (buf[off+2] << 8) | (buf[off+3] & 0xFF);
          break;
        case LONG_FORM_LEN:
          level       = ((buf[ off ] & 0x0F) << 8) | (buf[off+1] & 0xFF);
          keyLength   = ((buf[off+2] & 0x7F) << 8) | (buf[off+3] & 0xFF);
          totalLength = ((buf[off+4] & 0x7F) << 24) | ((buf[off+5] & 0xFF) << 16)
                      | ((buf[off+6] & 0xFF) <<  8) | (buf[off+7] & 0xFF);
          break;
        default:
          throw new AssertionError("This should be impossible");
      }
      
      int valLength = totalLength - headLength - keyLength;
      if (len < totalLength) throw new IllegalArgumentException("Unreadable metadata block (invalid length).");
      key = (keyLength == 0)? null : VRTMath.unpackAscii(buf, off+headLength,           len-headLength);
      val = (valLength == 0)? null : VRTMath.unpackUTF8( buf, off+headLength+keyLength, len-headLength-keyLength);

      return totalLength;
    }
  }
}

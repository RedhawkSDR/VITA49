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

import java.util.Arrays;
import java.net.InetAddress;
import java.util.UUID;

/** Defines a record that could be in a {@link VRTPacket}.
 *
 *  @author 
 */
public class Record implements HasFields {
  private   final boolean strictEquality;
  protected       byte[]  buf;

  /** Creates a new instance sharing the buffers of the one specified. */
  protected Record (Record r) {
    this.strictEquality = r.strictEquality;
    this.buf            = r.buf;
  }

  /** Creates a new instance with the given length. This is the same as <tt>new Record(true,len)</tt>.
   *  @param len The initial length of the record.
   */
  protected Record (int len) {
    this(true,len);
  }
  
  /** Creates a new instance with the given length.
   *  @param strictEquality Should the {@link #equals} check ensure strict equality by checking to
   *                        that both the data and class match? If false, only the data is checked.
   *  @param len            The initial length of the record.
   */
  protected Record (boolean strictEquality, int len) {
    this.strictEquality = strictEquality;
    this.buf            = new byte[len];
  }

  /** Creates a copy of this packet. */
  public Record copy () {
    Record r = new Record(strictEquality, buf.length);
    System.arraycopy(buf, 0, r.buf, 0, buf.length);
    return r;
  }

  /** Reads bytes from this record to the specified buffer.
   *  @param buffer The buffer to write to.
   *  @param offset  The offset in the buffer to start at.
   */
  public void readBytes  (byte[] buffer, int offset) {
    System.arraycopy(buf, 0, buffer, offset, buf.length);
  }

  /** Writes bytes to this record from the specified buffer.
   *  @param buffer The buffer to read from.
   *  @param offset  The offset in the buffer to start at.
   */
  public void writeBytes (byte[] buffer, int offset) {
    System.arraycopy(buffer, offset, buf, 0, buf.length);
  }

  /** Gets the length in bytes. */
  public int getByteLength () {
    return buf.length;
  }

  /** Sets the length in bytes. */
  protected void setByteLength (int len) {
    setByteLength(len, -1);
  }

  /** Sets the length in bytes with any additional bytes added/removed from the given point (-1=end). */
  protected void setByteLength (int len, int off) {
    if (len == buf.length) return;
    byte[] buffer = new byte[len];

    if (off == -1) {
      System.arraycopy(buf, 0, buffer, 0, Math.min(len, buf.length));
    }
    else {
      int old   = buf.length;
      int delta = len - old;

      if (delta > 0) { // ADD
        System.arraycopy(buf, 0,   buffer, 0,         off);
        System.arraycopy(buf, off, buffer, off+delta, old-off);
      }
      else { // REMOVE
        System.arraycopy(buf, 0,   buffer, 0,         off+delta);
        System.arraycopy(buf, off, buffer, off+delta, len-off);
      }
    }
    buf = buffer;
  }
  
  /** Gets a basic description of the object, useful for debugging.
   *  @return A basic description of the object.
   */
  @Override
  public final String toString () {
    return toStringBuilder().toString();
  }

  /** Gets a basic description of the object, useful for debugging.
   *  @return A basic description of the object.
   */
  protected StringBuilder toStringBuilder () {
    return new StringBuilder();
  }

  @Override
  public boolean equals (Object o) {
    if (o == this) return true;
    if (o == null) return false;
    if (strictEquality) {
      if (o.getClass() != getClass()) return false;
    }
    else {
      if (!(o instanceof Record)) return false;
    }
    Record r = (Record)o;
    return Arrays.equals(buf, r.buf);
  }

  @Override
  public int hashCode () {
    return Arrays.hashCode(buf);
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // PACK/UNPACK METHODS
  //////////////////////////////////////////////////////////////////////////////////////////////////
  protected void packByte      (int off, byte          val                             ) { buf[off] = val; }
  protected void packShort     (int off, short         val                             ) { VRTMath.packShort(    buf, off, val); }
  protected void packInt       (int off, int           val                             ) { VRTMath.packInt(      buf, off, val); }
  protected void packLong      (int off, long          val                             ) { VRTMath.packLong(     buf, off, val); }
  protected void packFloat     (int off, float         val                             ) { VRTMath.packFloat(    buf, off, val); }
  protected void packDouble    (int off, double        val                             ) { VRTMath.packDouble(   buf, off, val); }
  protected void packBoolNull  (int off, Boolean       val                             ) { VRTMath.packBoolNull( buf, off, val); }
  protected void packBoolean   (int off, boolean       val                             ) { VRTMath.packBoolean(  buf, off, val); }
  protected void packAscii     (int off, CharSequence  val,           int len          ) { VRTMath.packAscii(    buf, off, val, len); }
  protected void packUTF8      (int off, CharSequence  val,           int len          ) { VRTMath.packUTF8(     buf, off, val, len); }
  protected void packMetadata  (int off, MetadataBlock val,           int len          ) { VRTMath.packMetadata( buf, off, val, len); }
  protected void packBytes     (int off, byte[]        val,           int len          ) { VRTMath.packBytes(    buf, off, val, len); }
  protected void packBytes     (int off, byte[]        val, int boff, int len          ) { VRTMath.packBytes(    buf, off, val, boff, len); }
  protected void packTimeStamp (int off, TimeStamp     val, TimeStamp.IntegerMode epoch) { VRTMath.packTimeStamp(buf, off, val, epoch); }
  protected void packInetAddr  (int off, InetAddress   val                             ) { VRTMath.packInetAddr( buf, off, val); }
  protected void packUUID      (int off, UUID          val                             ) { VRTMath.packUUID(     buf, off, val); }

  protected byte              unpackByte      (int off                               ) { return buf[off]; }
  protected short             unpackShort     (int off                               ) { return VRTMath.unpackShort(    buf, off); }
  protected int               unpackInt       (int off                               ) { return VRTMath.unpackInt(      buf, off); }
  protected long              unpackLong      (int off                               ) { return VRTMath.unpackLong(     buf, off); }
  protected float             unpackFloat     (int off                               ) { return VRTMath.unpackFloat(    buf, off); }
  protected double            unpackDouble    (int off                               ) { return VRTMath.unpackDouble(   buf, off); }
  protected Boolean           unpackBoolNull  (int off                               ) { return VRTMath.unpackBoolNull( buf, off); }
  protected boolean           unpackBoolean   (int off                               ) { return VRTMath.unpackBoolean(  buf, off); }
  protected AsciiCharSequence unpackAscii     (int off,                       int len) { return VRTMath.unpackAscii(    buf, off, len); }
  protected CharSequence      unpackUTF8      (int off,                       int len) { return VRTMath.unpackUTF8(     buf, off, len); }
  protected MetadataBlock     unpackMetadata  (int off,                       int len) { return VRTMath.unpackMetadata( buf, off, len); }
  protected byte[]            unpackBytes     (int off,                       int len) { return VRTMath.unpackBytes(    buf, off, len); }
  protected byte[]            unpackBytes     (int off, byte[] val, int boff, int len) { return VRTMath.unpackBytes(    buf, off, val, boff, len); }
  protected TimeStamp         unpackTimeStamp (int off, TimeStamp.IntegerMode epoch  ) { return VRTMath.unpackTimeStamp(buf, off, epoch); }
  protected InetAddress       unpackInetAddr  (int off                               ) { return VRTMath.unpackInetAddr( buf, off); }
  protected UUID              unpackUUID      (int off                               ) { return VRTMath.unpackUUID(     buf, off); }
  
  //////////////////////////////////////////////////////////////////////////////////////////////////
  // HASFIELDS METHODS
  //////////////////////////////////////////////////////////////////////////////////////////////////
  @Override
  public int getFieldCount () {
    return 0;
  }

  @Override
  public String getFieldName (int id) {
    throw new IllegalArgumentException("Invalid field #"+id+" in "+getClass());
  }

  @Override
  public Class<?> getFieldType (int id) {
    throw new IllegalArgumentException("Invalid field #"+id+" in "+getClass());
  }

  @Override
  public Object getField (int id) {
    throw new IllegalArgumentException("Invalid field #"+id+" in "+getClass());
  }

  @Override
  public void setField (int id, Object val) {
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

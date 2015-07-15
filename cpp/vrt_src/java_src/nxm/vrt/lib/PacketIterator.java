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

import java.util.Enumeration;
import java.util.Iterator;
import java.util.NoSuchElementException;

/** <b>Internal Use Only:</b> Provides iteration capabilities over a given
 *  {@link PacketContainer}. The intent here is that the packet container will
 *  hold the underlying iteration capabilities that can be accessed via:
 *  <pre>
 *    {@link PacketContainer#hasNextPacket}
 *    {@link PacketContainer#getNextPacket}
 *    {@link PacketContainer#removePrevPacket}  <i>(OPTIONAL)</i>
 *  </pre>
 *  @author 
 */
public final class PacketIterator implements Iterator<VRTPacket>, Enumeration<VRTPacket> {
  final PacketContainer container; // The container.
  final boolean         direct;    // Should the packet be direct?
  final boolean         readOnly;  // Should the packet be read-only?
  final boolean         resolve;   // Should the packet type be resolved?
        long            offset;    // The current offset.
        long            last;      // The previous offset (-1 if n/a)
        byte[]          buf;       // Buffer used when reading the packet.

  /** <b>Internal Use Only:</b> Creates a new instance.
   *  @param container The container.
   *  @param offset    The starting offset.
   *  @param direct    Should the packet be direct?
   *  @param readOnly  Should the packet be read-only?       (usually true)
   *  @param resolve   Should the packet type be resolved?   (usually true)
   */
  PacketIterator (PacketContainer container, long offset, boolean direct,
                  boolean readOnly, boolean resolve) {
    this.container = container;
    this.direct    = direct;
    this.readOnly  = readOnly;
    this.resolve   = resolve;
    this.offset    = offset;
    this.last      = -1L;
    this.buf       = new byte[4];
  }

  @Override public final boolean   hasMoreElements () { return hasNext(); }
  @Override public final VRTPacket nextElement ()     { return next(); }
  @Override public final boolean   hasNext ()         { return container.hasNextPacket(this); }
  @Override public final void      remove ()          { container.removePrevPacket(this); }
  @Override public final String    toString ()        { return "PacketIterator for "+container; }
  @Override public final VRTPacket next ()            { return container.getNextPacket(this,false); }
  
  /** <b>Internal Use Only:</b> Skip over next packet, but update internal
   *  counters. This is used to iterating over the container in a situation
   *  where only the offset values are important, such as validity checks and
   *  indexing.
   */
  final void skipNext () {
    container.getNextPacket(this,true);
  }
  
  /** <b>Internal Use Only:</b> Identifies a container for VRT packets that can
   *  be iterated over via {@link PacketIterator}.
   *  @author 
   */
  public static interface PacketContainer extends Iterable<VRTPacket> {
    /** <b>Internal Use Only:</b> Called by the iterator to determine if there is
     *  a next packet that begins at the specified offset.
     *  @param pi The packet iterator being used.
     *  @return <tt>true</tt> if there is another packet beginning at the specified
     *          offset and <tt>false</tt> otherwise.
     */
    boolean hasNextPacket (PacketIterator pi);

    /** <b>Internal Use Only:</b> Called by the iterator to get the next packet
     *  beginning at the specified offset.
     *  @param pi   The packet iterator being used.
     *  @param skip Skip over packet returning null, but update internal counters.
     *  @return The packet.
     *  @throws NoSuchElementException If no such element exists.
     */
    VRTPacket getNextPacket (PacketIterator pi, boolean skip);

    /** <b>Internal Use Only:</b> Called by the iterator to remove the previous
     *  packet beginning at the last specified offset.
     *  @param pi The packet iterator being used.
     *  @throws UnsupportedOperationException If removal is unsupported (normally
     *                                        removal is NOT supported).
     */
    void removePrevPacket (PacketIterator pi);

    /** Gets an iterator over the packets in this object. */
    @Override
    PacketIterator iterator();
  }
}

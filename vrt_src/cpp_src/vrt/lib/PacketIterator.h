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

#ifndef _PacketIterator_h
#define _PacketIterator_h
#include "BasicVRTPacket.h"

// Note: This file is titled "PacketIterator" and not "ConstPacketIterator" as
//       it is intended that a future version may provide a non-const iterator
//       in here titled "PacketIterator" (which would match the Java).


namespace vrt {
  class PacketContainer;
  
  /** Provides iteration capabilities over a given {@link PacketContainer}. The
   *  intent here is that the packet container will hold the underlying iteration
   *  capabilities that can be accessed via:
   *  <pre>
   *    {@link PacketContainer#gotoNextPacket}
   *    {@link PacketContainer#getThisPacket}
   *  </pre>
   *  @author 
   */
  class ConstPacketIterator : public VRTObject {
    public: friend class AbstractVRAFile;
    public: friend class BasicVRLFrame;
    public: friend class PacketContainer;
    
    private: const PacketContainer *container; // The container.
    private: bool                   resolve;   // Should the packet type be resolved?
    private: int64_t                offset;    // The current offset.
    private: int64_t                length;    // Length of current packet (-1 if n/a)
    private: vector<char>           buf;       // Buffer used when reading the packet.
  
    /** <b>Internal Use Only:</b> Creates a new instance.
     *  @param container The container.
     *  @param offset    The starting offset.
     *  @param resolve   Should the packet type be resolved?   (usually true)
     */
    public: ConstPacketIterator (const PacketContainer *container, int64_t offset, bool resolve=true);
    
    /** Basic copy-constructor for the class. */
    public: ConstPacketIterator (const ConstPacketIterator &pi);
    
    /** Basic destructor for the class. */
    public: ~ConstPacketIterator () { }
    
    /** String describing the object. */
    public: virtual string toString () const;
    
    /** String describing the object. */
    public: virtual bool equals (const VRTObject &obj) const {
      try {
        const ConstPacketIterator *pi = checked_dynamic_cast<const ConstPacketIterator*>(&obj);
        return equals(*pi);
      }
      catch (VRTException e) {
        return false;
      }
    }
    
    /** String describing the object. */
    public: virtual bool equals (const ConstPacketIterator &pi) const;
    
    /** Moves to the next element. */
    public: ConstPacketIterator operator++ (int); // postfix ++
    
    /** Moves to the next element. */
    public: ConstPacketIterator operator++ (); // prefix ++
    
    /** Gets the packet at the current offset. */
    public: BasicVRTPacket* operator* ();
  };
  
  
  /** <b>Internal Use Only:</b> Identifies a container for VRT packets that can
   *  be iterated over via {@link ConstPacketIterator}.
   *  @author 
   */
  class PacketContainer {
    /** Basic constructor for the class. */
    public: PacketContainer () { }
    
    /** Basic destructor for the class. */
    public: ~PacketContainer () { }
    
    /** <b>Internal Use Only:</b> Called by the iterator to determine if there is
     *  a next packet that begins at the specified offset.
     *  @param pi The packet iterator being used.
     *  @return <tt>true</tt> if there is another packet beginning at the specified
     *          offset and <tt>false</tt> otherwise.
     */
    public: virtual void gotoNextPacket (ConstPacketIterator &pi) const = 0;

    /** <b>Internal Use Only:</b> Called by the iterator to get the next packet
     *  beginning at the specified offset.
     *  @param pi   The packet iterator being used.
     *  @param skip Skip over packet returning NULL, but update internal counters.
     *  @return The packet.
     *  @throws NoSuchElementException If no such element exists.
     */
    public: virtual BasicVRTPacket* getThisPacket (ConstPacketIterator &pi, bool skip) const = 0;

    /** Gets an iterator over the packets in the container. This is intended to
     *  be used similar to the following:
     *  <pre>
     *    for (ConstPacketIterator pi = x.begin(); pi != x.end(); ++pi) {
     *      std::cout &lt;&lt; "packet = " &lt;&lt; (*pi) &lt;&lt; std::endl;
     *    }
     *  </pre>
     */
    public: virtual ConstPacketIterator begin () const = 0;

    /** Gets an iterator pointing to one position past the end of the packets
     *  in the container. See {@link #begin} for a usage example.
     */
    public: virtual ConstPacketIterator end () const = 0;
  };
};

#endif /* _PacketIterator_h */

/* ===================== COPYRIGHT NOTICE =====================
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
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see http://www.gnu.org/licenses/.
 * ============================================================
 */

#ifndef _AbstractPacketFactory_h
#define _AbstractPacketFactory_h
#include "BasicVRTPacket.h"

namespace vrt {
  /** A basic but full-featured implementation of {@link PacketFactory}. Typical implementations
   *  will look like:
   *  <pre>
   *    class MyPacketFactory : public AbstractPacketFactory {
   *      public: MyPacketFactory () : AbstractPacketFactory() { }
   *
   *      public: MyPacketFactory (PacketFactory parent) : AbstractPacketFactory(parent) { }
   *
   *      protected: virtual BasicVRTPacket *getPacket (PacketType type, int64_t id,
   *                                                    const BasicVRTPacket *p) const {
   *        ...
   *        return AbstractPacketFactory::getPacket(clazz, type, id, p, copy);
   *      }
   *    }
   *  </pre>
   */
  class AbstractPacketFactory : public VRTObject {
    /** The payload format to assume. */
    private: const AbstractPacketFactory *parent;

    /** Basic destructor for the class. */
    public: ~AbstractPacketFactory ();

    /** Basic copy constructor for the class. */
    public: AbstractPacketFactory (const AbstractPacketFactory *pf);

    /** Creates a new instance that can be written to. Initially this will just be a simple data
     *  packet with no fields set (other than the required packet length of 4).
     */
    public: AbstractPacketFactory ();

    /** Gets a specific packet from the factory when given a generic packet.
     *  @param p The existing (generic) packet.
     *  @return The applicable packet. This will return null if not supported.
     *  @throws VRTException If <tt>p</tt> is null.
     *  @throws VRTException If initialization of the given type is supported, but the packet given
     *                       does not match it.
     */
    public: inline BasicVRTPacket *getPacket (const BasicVRTPacket *p) const
                                           __attribute__((warn_unused_result)) {
      if (p == NULL) {
        throw VRTException("Can not determine type for NULL packet.");
      }
      PacketType type = p->getPacketType();
      int64_t    id   = p->getClassIdentifier();
      return getPacket(type, id, p);
    }

    /** Gets a specific packet from the factory when given a generic packet.
     *  @param type  The packet type (VRT allows one Data/ExtData and one Context/ExtContext per class).
     *  @param id    The class ID of the packet.
     *  @return The applicable packet. This will return null if not supported.
     *  @throws VRTException  If initialization of the given type is supported, but <tt>data</tt>
     *                        and/or <tt>id</tt> do not match it.
     */
    public: inline BasicVRTPacket *getPacket (PacketType type, int64_t id)
                                     const __attribute__((warn_unused_result)) {
      return getPacket(type, id, NULL);
    }

    /** Initializes a packet. This is a utility method called by both
     *  <tt>getPacket(BasicVRTPacket,bool)</tt> and <tt>getPacket(PacketType,int64_t)</tt>.
     *  @param type  The packet type (VRT allows one Data/ExtData and one Context/ExtContext per class).
     *  @param id    The class ID of the packet.
     *  @param p     The packet to initialize with (null if n/a).
     *  @return The packet or null if n/a.
     */
    protected: virtual BasicVRTPacket *getPacket (PacketType type, int64_t id,
                                                  const BasicVRTPacket *p) const
                                            __attribute__((warn_unused_result));
  };

  namespace VRTConfig {
    /** Gets the current packet factory. */
    AbstractPacketFactory *getPacketFactory (); // code in VRTConfig.cc

    /** Sets the current packet factory. */
    void setPacketFactory (AbstractPacketFactory *pf); // code in VRTConfig.cc

    /** Gets a specific packet from the factory when given a generic packet.
     *  @param p    The existing (generic) packet.
     *  @return The applicable packet. This will never return null, the result of an unknown packet
     *          may result in a new <tt>BasicVRTPacket</tt>.
     *  @throws VRTException If <tt>p</tt> is null.
     */
    BasicVRTPacket *getPacket (const BasicVRTPacket *p)
                                            __attribute__((warn_unused_result)); // code in VRTConfig.cc

    /** Gets a specific packet from the factory when given a generic packet.
     *  @param p    The existing (generic) packet.
     *  @return The applicable packet. This will never return null, the result of an unknown packet
     *          may result in a new <tt>BasicVRTPacket</tt>.
     *  @throws VRTException If <tt>p</tt> is null.
     */
    BasicVRTPacket *getPacket (const BasicVRTPacket &p)
                                            __attribute__((warn_unused_result));

    /** Gets a specific packet from the factory when given a generic packet.
     *  @param type The packet type (VRT allows one Data/ExtData and one Context/ExtContext per class).
     *  @param id   The class ID of the packet (null if not specified).
     *  @return The applicable packet. This will never return null, the result of an unknown packet
     *          may result in a new <tt>BasicVRTPacket</tt>.
     */
    BasicVRTPacket *getPacket (PacketType type, int64_t id)
                                            __attribute__((warn_unused_result)); // code in VRTConfig.cc

    /** Creates a new instance based on the given data buffer. Note that when the buffer lengths
     *  are given, only the most minimal of error checking is done. Users should call
     *  <tt>BasicVRTPacket.isPacketValid()</tt> to verify that the packet is valid. Invalid packets can
     *  result in unpredictable behavior, but this is explicitly allowed (to the extent possible) so
     *  that applications creating packets can use this class even if the packet isn't yet "valid".<br>
     *  <br>
     *  This function will also detect if a VRL frame was given and return null if that happens.
     *  The <tt>getPackets()</tt> method can be used in cases where VRL frames are expected.
     *  @param bbuf     The data buffer to use.
     *  @param boff     The byte offset into the bbuf to start reading/writing at.
     *  @param blen     The length of bbuf in bytes (-1=use size in header of packet).
     *  @return The applicable type of packet.
     *  @throws VRTException If the buffer is null.
     *  @throws VRTException If the offset is negative or greater than the
     *          length of the buffer given. Also thrown if the buffer length is -1 and a copy
     *          can not be made because the sizes in the header of the packet are invalid.
     */
    BasicVRTPacket *getPacket (vector<char> &bbuf, int32_t boff, int32_t blen)
                                            __attribute__((warn_unused_result)); // code in VRTConfig.cc

    /** <b>Internal Use Only:</b> Gets new packet with buffer swaps. */
    BasicVRTPacket *getPacketSwap (vector<char> &bbuf, int32_t boff, int32_t blen)
                                            __attribute__((warn_unused_result)); // code in VRTConfig.cc
    /** <b>Internal Use Only:</b> Gets new packet with buffer swaps. */
    BasicVRTPacket *getPacketSwap (BasicVRTPacket *p)
                                            __attribute__((warn_unused_result)); // code in VRTConfig.cc
  } END_NAMESPACE
} END_NAMESPACE
#endif /* _AbstractPacketFactory_h */

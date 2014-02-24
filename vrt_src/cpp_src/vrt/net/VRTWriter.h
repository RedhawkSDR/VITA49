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

#ifndef _VRTWriter_h
#define _VRTWriter_h

#include "BasicVRLFrame.h"
#include "BasicVRTPacket.h"
#include "VRTMath.h"
#include "NetUtilities.h"
#include "multicast.h"
#include <map>
#include <stdarg.h>

namespace vrt {
  using namespace std;

  /**  Writes VRT packets which may optionally be included in VRL frames. The choice between
   *  un-framed VRT packets and using VRL frames must be made at the time the class is instantiated.
   *  If frames are used, there is also an option to use CRC protection for the frames.<br>
   *  <br>
   *   The proper way to send a single packet is via the {@link #sendPacket} function:
   *  <pre>
   *    VRTWriter      w(...);
   *    BasicVRTPacket p;
   *    ...
   *    w.sendPacket(p);
   *  </pre>
   *   The proper way to send a multiple packets (which could be included in a single VRL frame)
   *  is via the <tt>sendPackets</tt> macro:
   *  <pre>
   *    VRTWriter      w(...);
   *    BasicVRTPacket firstPacket;
   *    BasicVRTPacket secondPacket;
   *    ...
   *    w.sendPackets(&firstPacket, &secondPacket);
   *    //            ^             ^
   *    //      must pass pointers to the macro
   *  </pre>
   *   Note that the above macro must be passed pointers to the packets. This function (other
   *  that the explicit sending of pointers) matches the Java API, but in C++ is implemented via a
   *  call to:
   *  <pre>
   *    VRTWriter              w(...);
   *    vector<BasicVRTPacket> packets;
   *    ...
   *    w.sendAllPackets(packets);
   *  </pre>
   *  @author         
   */
  class VRTWriter : public VRTObject {
    /**  Self-imposed point at which a frame is split into two frames. */
    private: static const int32_t FRAME_BREAK = NetUtilities::MAX_IPv4_UDP_LEN & 0xFFFFF000;


    /**  Packet counters. */
    private: map<int64_t,int32_t> packetCounters;

    private: multicast_t multicast;    // The socket.
    private: bool        frames;       // Use frames?
    private: bool        crc;          // Use CRC?
    private: int32_t     frameCounter; // Frame counter

    /**  Creates a new instance.
     *  @param host      The host to connect to (null = wildcard address).
     *  @param port      The port number.
     *  @param device    The device to connect to (e.g. "eth0", "eth1.101") (null = use default).
     *  @param useFrames Should packets be sent embedded in VRL frames?
     *  @param useCRC    Should frames provide CRC protection? (This parameter only applies if frames
     *                   are in use.)
     *  @param ttl       Time to live for the multicast packet (this should usually be set to 1).
     *  @throws VRTException If an I/O exection occurrs.
     */
    public: VRTWriter (const string &host, int32_t port, const string &device, bool useFrames, bool useCRC, uint8_t ttl=1);

    /**  Destroys the current instance, closing the socket if required. */
    public: ~VRTWriter ();

    /**  Close the socket. */
    public: void close ();
    
    /**  Updates the counter in the given packet. */
    private: void updateCounter (BasicVRTPacket &p);

    /**  <b>Deprecated:</b> Use {@link #sendPacket}. */
    public: inline void transmit (const BasicVRTPacket &p) {
      sendPacket(p);
    }

    /**  <b>Deprecated:</b> Use {@link #sendPacket}. */
    public: inline void send (const BasicVRTPacket &p) {
      sendPacket(p);
    }

    /**  Sends a packet via the socket. */
    public: void sendPacket (const BasicVRTPacket &p);

    /**  Sends a packet via the socket. This inline method simply calls <tt>sendPacket(*p)</tt>
     *  but is provided for compatibility with <tt>sendPackets(..)</tt> which must take in pointers.
     */
    public: inline void sendPacket (const BasicVRTPacket *p) {
      sendPacket(*p);
    }

    /**  Sends packets via the socket. See the example at the top of this file for details. */
    #define sendPackets(firstPacket, ...) sendPacketsVaArgs(firstPacket, ## __VA_ARGS__, ((BasicVRTPacket*)NULL))
    // The ## in the line above tells the preprocessor to delete previous comma if no arguments
    // are given to va_args (i.e. only sending a single packet). Note that this is done for
    // completeness, users are encouraged to use sendPacket(..) if just sending one packet since
    // it will be faster than the expanded call to sendPacketsVaArgs(..).
    //
    // Note that as a result of the NULL entry (which must be defined exactly as shown above), the
    // call to sendPacketsVaArgs(..) will always have at least one va_arg to look at.

    /**  <b>Internal use only:</b> Use <tt>sendPackets</tt> macro (as shown in the example at the
     *  top of this file). When this method is called, the last entry in the list must be
     *  <tt>((BasicVRTPacket*)NULL)</tt>, the <tt>sendPackets</tt> macro takes care of this.
     */
    public: void sendPacketsVaArgs (const BasicVRTPacket *firstPacket, ...);

    /**  Sends a set of packets via the socket. */
    public: void sendAllPackets (const vector<BasicVRTPacket> &packets);

    /**  Sends a set of packets via the socket. */
    private: void sendAllPackets0 (vector<BasicVRTPacket> &packets, bool updateCounters=true);
  };
};

#endif /* _VRTWriter_h */

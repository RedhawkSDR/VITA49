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

#ifndef _VRTReader_h
#define _VRTReader_h

#include "BasicVRLFrame.h"
#include "BasicVRTPacket.h"
#include "BasicContextPacket.h"
#include "BasicDataPacket.h"
#include "Threadable.h"
#include "VRTContextListener.h"
#include "VRTMath.h"
#include "ThreadSafeQueue.h"
#include "multicast.h"
#include <map>
#include <set>
#include <queue>

namespace vrt {
  using namespace std;

  /** Reads VRT packets.
   *
   *  @author 
   */
  class VRTReader : public VRTObject {

    private: class ThreadedQueueReader : public Threadable {
      friend class VRTReader;
      private: VRTReader* reader;

      ThreadedQueueReader(VRTReader* reader) : reader(reader) {};

      public: virtual void runThread();

    };

    private: class ThreadedMulticastReader : public Threadable {
      friend class VRTReader;
      private: VRTReader* reader;

      ThreadedMulticastReader(VRTReader* reader) : reader(reader) {};

      public: virtual void runThread();

    };

    private: ThreadedQueueReader queueReader;
    private: ThreadedMulticastReader multicastReader;

    public: static const int32_t DEFAULT_MAX_QUEUE_SIZE = 2500;

    /** The "timeout" value indicating the default wait of 60.0 sec. */
    public: static const int32_t DEFAULT_TIMEOUT;

    /** The "timeout" value indicating an unlimited wait. */
    public: static const int32_t UNLIMITED_TIMEOUT;

    /** The "timeout" value indicating that "legacy mode" should be used. */
    protected: static const int32_t LEGACY_MODE;

    /** The start time indicator that the initial context has already been found. */
    protected: static const int32_t FOUND_INITIAL;

    private: map<int64_t,int32_t>             packetCounters;    // the VRTPacket counters (per stream id)
    private: int32_t                          frameCounter;      // the VRLFrame counter
    private: multicast_t                      multicast;         // the multicast socket
    private: vector<char>                     buffer;            // the read buffer (huge performance boost from re-using the same buffer)
    private: string                           host;              // host address to read from
    private: int32_t                          port;              // port to read from
    private: string                           device;            // network device to use (e.g. "eth0" or "eth1.101")
    private: int32_t                          recvBufSize;       // receive buffer size
    private: int32_t                          packetQueueSize;   // packet queue size
    private: int64_t                          timeoutMS;         // timeout in ms (-1=LegacyMode)
    private: int64_t                          startTimeMS;       // start time in ms (0=not started)
    private: BasicDataPacket                  initialData;       // initial data packet received
    private: BasicContextPacket               initialCtx;        // initial (paired) context received
    private: map<int32_t,BasicContextPacket>  initialContext;    // initial (all) context received
    private: set<int32_t>                     requiredContext;   // stream IDs for required context
    private: int32_t                          idContext;         // ID for context packet
    private: VRTContextListener              *listener;          // the listener to send to (NULL if n/a)
    private: ThreadSafeQueue<BasicVRTPacket>  packetQueue;       // received packet queue
    private: bool                             deletePointers;    // delete pointers?
    private: bool                             readOnly;          // read only packets?
    private: VRTEvent                         event;             // shared VRTEvent object

    /** Creates a new instance.
     *  @param host            The host to connect to (null = wildcard address).
     *  @param port            The port number.
     *  @param device          The device to connect to (e.g. "eth0", "eth1.101") (null = use default).
     *  @param recvBufSize     The SO_RCVBUF size in bytes.
     *  @param packetQueueSize The packet queue size in packets.
     *  @param readOnly        Are packets read-only?
     *  @throws VRTException If an I/O exception occurs.
     */
    public: VRTReader (const string &host, int32_t port, const string &device,
                       int32_t recvBufSize=DEFAULT_SO_RCVBUF,
                       int32_t packetQueueSize=DEFAULT_MAX_QUEUE_SIZE,
                       bool readOnly=false);

    /** Creates a new instance. <br>
     *  <br>
     *  <i>For historical reasons (i.e. to prevent incompatibilities with the Java version in VRT
     *  version 1.0 and earlier), the default timeout value (i.e. {@link #DEFAULT_TIMEOUT}) must
     *  be explicitly provided.</i>
     *  @param host            The host to connect to (null = wildcard address).
     *  @param port            The port number.
     *  @param device          The device to connect to (e.g. "eth0", "eth1.101") (null = use default).
     *  @param listener        The listener used to handle any errors/warnings and to receive any packets.
     *  @param timeout         The approximate timeout (in seconds) applied when checking for the initial
     *                         packets. If the complete initial context is not received before the first
     *                         packet received after this time has elapsed, it will be reported as an error
     *                         (via <tt>errorMsg</tt> in {@link VRTContextListener#receivedInitialContext}
     *                         if appropriate). Setting this to {@link #UNLIMITED_TIMEOUT} disables the
     *                         timeout. The default timeout value to use is {@link #DEFAULT_TIMEOUT}.
     *  @param recvBufSize     The SO_RCVBUF size in bytes.
     *  @param packetQueueSize The packet queue size in packets.
     *  @param readOnly        Are packets read-only?
     *  @param deletePointers  Delete pointers when done?
     *  @throws VRTException If an I/O exception occurs.
     */
    public: VRTReader (const string &host, int32_t port, const string &device,
                       VRTContextListener *listener, double timeout,
                       int32_t recvBufSize=DEFAULT_SO_RCVBUF,
                       int32_t packetQueueSize=DEFAULT_MAX_QUEUE_SIZE,
                       bool readOnly=false, bool deletePointers=true);

    /** Destroys the current instance, closing the socket if required. */
    public: ~VRTReader ();

    public: size_t getQueueSize() {
      return packetQueue.getSize();
    }

    public: size_t getMaxQueueSize() {
      return packetQueue.getMaxSize();
    }

    /** Converts this class its string form. */
    public: virtual string toString () const;
    
    /** Close the socket. */
    public: virtual void close ();

    /** Closes any connections, This will always be called at the end of a call to {@link #run()}. */
    protected: virtual void shutdown ();

    /** <b>Deprecated:</b> Use {@link #receivePackets}. */
    public: inline BasicVRTPacket* receive (float timeout) {
      vector<BasicVRTPacket*> packets = doReceivePackets(timeout);
      if (packets.size() == 0) {
        return packets[0];
      }
      else {
        cout << "ERROR: Deprecated VRTReader::receive(..) method received "<<packets.size()<<" packets"<<endl;
        return new BasicVRTPacket(vector<char>(0), true);
      }
    }

    private: vector<BasicVRTPacket*> splitFrame(const vector<char> &buffer, ssize_t numRead);
    private: vector<BasicVRTPacket*> _splitFrame(const vector<char> &buffer, ssize_t numRead);

    /** Receives packets off of the socket.
     *  @param timeout The socket timeout in seconds.
     *  @param count   The number of packets requested. Note that this is only a suggestion,
     *                 if reading a stream with VRL frames the number of packets read will
     *                 depend on the number includes in the frame as this will never read
     *                 a partial VRL frame.
     *  @return The packets received. If a VRT packet is received (not in a VRL frame) this will
     *          have a length of one (i.e. the packet), if a VRL frame is received this will
     *          contain all of VRT packets enclosed in it (in rare cases this could be a length
     *          of zero). In the event of a timeout a zero-length vector will be returned.
     */
    public: vector<BasicVRTPacket*> receivePackets (float timeout, size_t count=1);

    // Functions to support receivePackets(..)
    private: vector<BasicVRTPacket*> doReceivePackets (float timeout);
//    private: vector<BasicVRTPacket*> _handle (BasicVRLFrame  *frame);
    private: vector<BasicVRTPacket*> _handle (BasicVRTPacket *packet);
    private: bool                   _handlePacket (const BasicVRTPacket *packet, bool fromFrames);

    // Functions to support run(..)
//    private: void                   handle (BasicVRLFrame  *frame);
    private: void                   handle (BasicVRTPacket *packet);


    private: inline PacketType getPacketType (const vector<char>& bbuf) const {
      unsigned char c = bbuf[0];
      return (PacketType)((c >> 4) & 0xF);
    }

    /** Blocks until the threads are done.
     *  @throws VRTException If already running.
     */
    public: void join ();

    /** Starts the subthreads
     *  @throws VRTException If already running.
     */
    public: void start ();

    public: void stop (bool wait=true);
  };
};

#endif /* _VRTReader_h */

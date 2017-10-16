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

#ifndef _VRTConfig_h
#define _VRTConfig_h

#include <stdarg.h>
#include <stdlib.h> // some platforms need this for getenv(..)

using namespace std;

namespace vrt {
  /** Global settings for the VRT package. Most of these can be controlled from the
   *  command line with the following environment variables (the Java uses similarly-named
   *  options, but set on the command line):
   *  <pre>
   *    VRT_LEAP_SECONDS=[file]               - The name of the leap seconds file to use (must be a
   *                                            path relative to the CLASSPATH). The default is
   *                                            "$VRTHOME/cpp_lib/tai-utc.dat". <i>(Prior to Jan 2012
   *                                            this defaulted to "$VRTHOME/cpp_lib/tai_utc_data.txt".)</i>
   *
   *    VRT_NORAD_LS_COUNTED=[true/false]     - Turns on/off the default mode for counting mid-year
   *                                            leap seconds in NORAD time (default is on).
   *
   *    VRT_PACKET_FACTORY=[factory]          - Sets the packet factory. The [factory] must be the
   *                                            fully-qualified class name and the class must have
   *                                            a no-argument constructor. (This can be altered at
   *                                            run time.)
   *
   *    VRT_PACKET_FACTORY=[f0]:[f1]:...:[fN] - Sets the packet factory. Each entry must be the
   *                                            fully-qualified of a packet factory. The zero'th
   *                                            entry the list must have a no-argument constructor.
   *                                            Subsequent entries must have a one-argument
   *                                            constructor that takes in a parent PacketFactory as
   *                                            its argument. (This can be altered at run time.)
   *
   *    VRT_PREFER_IPV6_ADDRESSES=[true/false]- Turns on preferences to use IPv6 over IPv4 (e.g.
   *                                            "localhost" becomes "::1" vs "127.0.0.1"), the
   *                                            default is to prefer IPv4 (false). This setting is
   *                                            exclusive to C++ since Java users must set the
   *                                            "java.net.preferIPv6Addresses" and/or
   *                                            "java.net.preferIPv4Stack" as these are handled
   *                                            at the Java VM level (in Java).
   *
   *    VRT_STRICT=[true/false]               - Turns on/off strict checking of input packets
   *                                            (default is off).
   *
   *    VRT_TEST_DELAY=[ms]                   - <b>Testing Use Only:</b> Sets the delay between sets of
   *                                            networking tests to permit socket close-out. Leave blank
   *                                            (default) or set to 0 to skip any waits.
   *
   *    VRT_TEST_DEVICE=[dev]                 - <b>Testing Use Only:</b> Sets the local network device
   *                                            to use while testing (e.g. "eth0"). Leave blank (default)
   *                                            to let o/s select the device.
   *
   *    VRT_TEST_FIRST_MCAST=[addr]           - <b>Testing Use Only:</b> Sets the UDP/Multicast address
   *                                            range to use as part of the network tests. This
   *                                            will be an implied range of 8 addresses starting
   *                                            at the given address. Setting this to "" will
   *                                            disable any tests that require using UDP/Multicast
   *                                            (default is "").
   *
   *    VRT_TEST_FIRST_PORT=[port]            - <b>Testing Use Only:</b> Sets the port numbers to use
   *                                            as part of the network tests. This will be an
   *                                            implied range of 10 ports starting at the given
   *                                            port number. Setting this to 0 will disable any
   *                                            tests that require opening a port (default is 0).
   *
   *    VRT_TEST_QUICK=[true/false]           - <b>Testing Use Only:</b> Turns on/off "quick test" mode
   *                                            which abbreviates some of the test cases in order
   *                                            to speed up the testing process (default is on).
   *                                            In "quick test" mode the tests generally take less
   *                                            than a minute to completel; when not in "quick
   *                                            test" mode the tests can take 15+ minutes.
   *
   *    VRT_TEST_SERVER=[host:port]           - Sets the host:port to run the server on. This should
   *                                            *not* specify "localhost" as the local-host interface
   *                                            (e.g. "lo0") is generally incompatible with some of
   *                                            the test protocols.
   *
   *    VRT_TEST_SERVER_TIMEOUT=[sec]         - Sets the test server's timeout in seconds. This is
   *                                            present to ensure that the server is shutdown when
   *                                            done, even if the client-side terminates abnormally.
   *
   *    VRT_VERSION=[V49/V49b]                - Sets the protocol version to use. Valid values are:
   *                                              "V49"  = VITA 49.0
   *                                              "V49b" = VITA 49.0b
   *  </pre>
   *  Except where explicitly stated, the above values are constants that can not be altered at
   *  after the program is started.
   */
  namespace VRTConfig {
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //  Initializes the libraries
    //////////////////////////////////////////////////////////////////////////////////////////////////
    /** Initializes the any internal fields for the libraries. Failure to call
     *  this immediately after loading the libraries can result in initialization
     *  delays when some functions are called. <br>
     *  <br>
     *  Once the initialization is complete, subsequent calls to this function
     *  will have no adverse effects.
     */
    void libraryInit ();

    //////////////////////////////////////////////////////////////////////////////////////////////////
    //  Version
    //////////////////////////////////////////////////////////////////////////////////////////////////
    /** Supported protocol versions. (<tt>V49</tt> = VITA 49.0, <tt>V49b</tt> = VITA 49.0b) */
    enum VITAVersion { VITAVersion_V49, VITAVersion_V49b, VITAVersion_V49d2 };

    /** The version number for the library, always <tt>0.1.0</tt> with this version. */
    string getLibraryVersion ();

    /** Gets the protocol version to use (<tt>VRT_VERSION</tt>). */
    VITAVersion getVRTVersion ();

    //////////////////////////////////////////////////////////////////////////////////////////////////
    //  Testing
    //////////////////////////////////////////////////////////////////////////////////////////////////
    /** The delay between sets of networking tests (<tt>VRT_TEST_DELAY</tt>). */
    int32_t getTestDelay ();

    /** The local network device to use (<tt>VRT_TEST_DEVICE</tt>). */
    string getTestDevice ();

    /** Gives the first (of 8) multicast addresses to use for testing purposes (""=disable) (<tt>vrt.test_first_mcast</tt>). */
    string getTestFirstMCast ();

    /** Gives the first (of 10) port to use for testing purposes (0=disable) (<tt>vrt.test_first_port</tt>). */
    int32_t getTestFirstPort ();

    /** Enable quicker unit tests (<tt>VRT_TEST_QUICK</tt>). */
    bool getTestQuick ();

    /** The test server to use (<tt>VRT_TEST_SERVER</tt>). */
    string getTestServer ();

    /** The test server timeout in seconds (<tt>VRT_TEST_SERVER_TIMEOUT</tt>). */
    int32_t getTestServerTimeout ();

    //////////////////////////////////////////////////////////////////////////////////////////////////
    //  Miscellaneous
    //////////////////////////////////////////////////////////////////////////////////////////////////
    /** Enable strict checks (<tt>VRT_STRICT</tt>). */
    bool getStrict ();

    /** Prefer IPv6 over IPv4 (<tt>VRT_PREFER_IPV6_ADDRESSES</tt>). */
    bool getPreferIPv6Addresses ();

    /** Default leap-seconds file (<tt>VRT_LEAP_SECONDS</tt>). */
    string getLeapSecondsFile ();

    /** Enables counting mid-year leap seconds for NORAD time (<tt>VRT_NORAD_LS_COUNTED</tt>). */
    bool getNoradLeapSecCounted ();

    //////////////////////////////////////////////////////////////////////////////////////////////////
    //  PacketFactory and VRTPacket creation
    //////////////////////////////////////////////////////////////////////////////////////////////////

    // Declared in AbstractPacketFactory.h:
    //   AbstractPacketFactory *getPacketFactory ();
    //   void setPacketFactory (AbstractPacketFactory *pf);
    //   BasicVRTPacket *getPacket (const BasicVRTPacket *p);
    //   BasicVRTPacket *getPacket (const BasicVRTPacket &p);
    //   BasicVRTPacket *getPacket (PacketType type, int64_t id);
    //   BasicVRTPacket *getPacket (vector<char> &bbuf, int32_t boff, int32_t blen);
    //   BasicVRTPacket *getPacketSwap (vector<char> &bbuf, int32_t boff, int32_t blen);
    //   BasicVRTPacket *getPacketSwap (BasicVRTPacket *p);
  } END_NAMESPACE
} END_NAMESPACE

/** Supports appending a {@link vrt::VRTConfig::VITAVersion} value to an output stream. */
ostream& operator<<(ostream &s, vrt::VRTConfig::VITAVersion val);

/** Supports appending a {@link vrt::VRTConfig::VITAVersion} value to a string. */
string   operator+ (string  &s, vrt::VRTConfig::VITAVersion val);

#endif /* _VRTConfig_h */

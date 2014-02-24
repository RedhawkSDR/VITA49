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
   *    VRT_QUICK_TEST=[true/false]           - Turns on/off "quick test" mode which skips some of
   *                                            the loop iterations in some of the tests in order
   *                                            to speed up the testing process (default is on).
   *                                            <i>(This is only applicable to the running of the
   *                                            internal unit tests.)</i>
   *
   *    VRT_STRICT=[true/false]               - Turns on/off strict checking of input packets
   *                                            (default is off).
   *
   *    VRT_VERSION=[V49/V49b]                - Sets the protocol version to use. Valid values are:
   *                                              "V49"  = VITA 49.0
   *                                              "V49b" = VITA 49.0b
   *  </pre>
   *  Except where explicitly stated, the above values are constants that can not be altered at
   *  after the program is started.
   *  @author 
   */
  namespace VRTConfig {
    /** Supported protocol versions. (<tt>V49</tt> = VITA 49.0, <tt>V49b</tt> = VITA 49.0b) */
    enum VITAVersion { VITAVersion_V49, VITAVersion_V49b };

    /** The version number for the library, always <tt>0.1.0</tt> with this version. */
    string getLibraryVersion ();

    /** Gets the protocol version to use (<tt>VRT_VERSION</tt>). */
    VITAVersion getVRTVersion ();

    /** Enable strict checks (<tt>VRT_QUICK_TEST</tt>). */
    bool getQuickTest ();

    /** Enable strict checks (<tt>VRT_STRICT</tt>). */
    bool getStrict ();
    
    /** Default leap-seconds file (<tt>VRT_LEAP_SECONDS</tt>). */
    string getLeapSecondsFile ();

    /** Enables counting mid-year leap seconds for NORAD time (<tt>VRT_NORAD_LS_COUNTED</tt>). */
    bool getNoradLeapSecCounted ();
    
    // Declared in AbstractPacketFactory.h:
    //   AbstractPacketFactory *getPacketFactory ();
    //   void setPacketFactory (AbstractPacketFactory *pf);
    //   BasicVRTPacket *getPacket (const BasicVRTPacket *p);
    //   BasicVRTPacket *getPacket (const BasicVRTPacket &p);
    //   BasicVRTPacket *getPacket (PacketType type, int64_t id);
  };
};

/** Supports appending a {@link vrt::VRTConfig::VITAVersion} value to an output stream. */
ostream& operator<<(ostream &s, vrt::VRTConfig::VITAVersion val);

/** Supports appending a {@link vrt::VRTConfig::VITAVersion} value to a string. */
string   operator+ (string  &s, vrt::VRTConfig::VITAVersion val);

#endif /* _VRTConfig_h */

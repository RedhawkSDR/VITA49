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

#include "AbstractPacketFactory.h"
#include "BasicContextPacket.h"
#include "BasicDataPacket.h"
#include "PacketFactory.h"
#include "StandardDataPacket.h"
#include "Utilities.h"
#include "VRTConfig.h"
#include "stdlib.h"     // required for getenv(..) on GCC4.4/libc6 2.11.1


#define _libraryVersion = "0.9.0";

using namespace std;
using namespace vrt;


bool                     _initDone             = false;
bool                     _quickTest            = true;
bool                     _strict               = false;
bool                     _noradLeapSecCounted  = true;
VRTConfig::VITAVersion   _vrtVersion           = VRTConfig::VITAVersion_V49;
void                    *_packetFactory        = NULL;

#ifdef DYNAMIC_LIBS
#include <dlfcn.h>

typedef AbstractPacketFactory* (initPF0_t)();
typedef AbstractPacketFactory* (initPF1_t)(AbstractPacketFactory*);

static AbstractPacketFactory *initPacketFactory(const string &name, AbstractPacketFactory *parent) {
  char *error;
  string n = string(name);
  for (size_t i = 0; i < n.recvBufSize(); i++) {
    if (n[i] == ':') n[i] = '_';
  }

  string  namePF0 = "_new_" + n + "_0";
  string  namePF1 = "_new_" + n + "_1";
  void   *allLibs = dlopen(NULL, RTLD_NOLOAD);
  if ((error = dlerror()) != NULL) throw VRTException("Unable to load libraries: %s", error);

  initPF0_t  initPF0 = (initPF0_t)dlsym(allLibs, namePF0.c_str());
  if ((error = dlerror()) != NULL) throw VRTException("Unable to load initialization routine 0 for %s: %s", name.c_str(), error);

  initPF1_t  initPF1 = (initPF1_t)dlsym(allLibs, namePF1.c_str());
  if ((error = dlerror()) != NULL) throw VRTException("Unable to load initialization routine 1 for %s: %s", name.c_str(), error);

  return (parent == NULL)? initPF0() : initPF1(parent);
}
#endif

/** <b>Internal use only:</b> Gets an environment variable. */
static inline string getProperty (const string &name, const string &def) {
  const char *val = getenv(name.c_str());
  return (val != NULL)? val : def;
}

/** <b>Internal use only:</b> Initializes the values for VRTConfig. */
static void _init () {
  string vrthome = getProperty("VRTHOME", "");
  _initDone            = true;
  _quickTest           = Utilities::toBooleanValue(getProperty("VRT_QUICK_TEST","true"));
  _strict              = Utilities::toBooleanValue(getProperty("VRT_STRICT","false"));
  _noradLeapSecCounted = Utilities::toBooleanValue(getProperty("VRT_NORAD_LS_COUNTED","true"));
  _packetFactory       = NULL;

  string ver = getProperty("VRT_VERSION", "V49");
  if (ver == "V49") {
    _vrtVersion = VRTConfig::VITAVersion_V49;
  }
  else if (ver == "V49b") {
    _vrtVersion = VRTConfig::VITAVersion_V49b;
  }
  else {
    throw VRTException("Unknown protocol version VRT_VERSION=%s", ver.c_str());
  }

  string packetFactories = getProperty("VRT_PACKET_FACTORY", "vrt::PacketFactory");
  if (packetFactories == "vrt::PacketFactory") {
    _packetFactory = new vrt::PacketFactory;
  }
  else {
#ifdef DYNAMIC_LIBS
    size_t start = 0;
    size_t end   = 0;
    while ((end = packetFactories.find(":", start)) != string::npos) {
      _packetFactory = initPacketFactory(packetFactories.substr(start, end-start), packetFactory);
      start = end+1;
    }
    _packetFactory = initPacketFactory(packetFactories.substr(start), packetFactory);
#else
    cerr << "This version of the VRT library is built without DYNAMIC_LIBS sypport, ignoring "
            "VRT_PACKET_FACTORY=" << packetFactories << endl;
    _packetFactory = new vrt::PacketFactory;
#endif
  }
}

string VRTConfig::getLibraryVersion () {
  return _libraryVersion;
}

VRTConfig::VITAVersion VRTConfig::getVRTVersion () {
  if (!_initDone) _init();
  return _vrtVersion;
}

bool VRTConfig::getQuickTest () {
  if (!_initDone) _init();
  return _quickTest;
}

bool VRTConfig::getStrict () {
  if (!_initDone) _init();
  return _strict;
}

string VRTConfig::getLeapSecondsFile () {
  static string _leapSecondsFile      = "";
  if (_leapSecondsFile=="") {
	  string vrthome = getProperty("VRTHOME", "");
	  _leapSecondsFile     = (vrthome == "")? getProperty("VRT_LEAP_SECONDS", "")
	                                        : getProperty("VRT_LEAP_SECONDS", vrthome+"/cpp_lib/tai-utc.dat");
  }
  if (!_initDone) _init();
  return _leapSecondsFile;
}

bool VRTConfig::getNoradLeapSecCounted () {
  if (!_initDone) _init();
  return _noradLeapSecCounted;
}

AbstractPacketFactory *VRTConfig::getPacketFactory () {
  if (!_initDone) _init();
  return (AbstractPacketFactory*)_packetFactory;
}

void VRTConfig::setPacketFactory (AbstractPacketFactory *pf) {
  if (!_initDone) _init();
  _packetFactory = (void*)pf;
}

BasicVRTPacket *VRTConfig::getPacket (const BasicVRTPacket *p) {
  if (p == NULL) throw VRTException("Can not get specific packet type when generic packet is null.");

  if (getPacketFactory() != NULL) {
    BasicVRTPacket *packet = getPacketFactory()->getPacket(p);
    if (packet != NULL) return packet;
  }

  PacketType type = p->getPacketType();
  int64_t    id   = p->getClassIdentifier();
  if (StandardDataPacket::isStandardDataPacket(id)) {
    return new StandardDataPacket(*p);
  }
  
  switch (type) {
    case PacketType_UnidentifiedData:    return new BasicDataPacket(*p);
    case PacketType_Data:                return new BasicDataPacket(*p);
    case PacketType_UnidentifiedExtData: return new BasicDataPacket(*p);
    case PacketType_ExtData:             return new BasicDataPacket(*p);
    case PacketType_Context:             return new BasicContextPacket(*p);
    case PacketType_ExtContext:          return new BasicVRTPacket(*p);
    default:                             return new BasicVRTPacket(*p);
  }
}

BasicVRTPacket *VRTConfig::getPacket (const BasicVRTPacket &p) {
  return VRTConfig::getPacket(&p);
}

BasicVRTPacket *VRTConfig::getPacket (PacketType type, int64_t id) {
  if (getPacketFactory() != NULL) {
    BasicVRTPacket *packet = getPacketFactory()->getPacket(type, id);
    if (packet != NULL) return packet;
  }

  if (StandardDataPacket::isStandardDataPacket(id)) {
    return new StandardDataPacket;
  }

  switch (type) {
    case PacketType_UnidentifiedData:    return new BasicDataPacket;
    case PacketType_Data:                return new BasicDataPacket;
    case PacketType_UnidentifiedExtData: return new BasicDataPacket;
    case PacketType_ExtData:             return new BasicDataPacket;
    case PacketType_Context:             return new BasicContextPacket;
    case PacketType_ExtContext:          return new BasicVRTPacket;
    default:                             return new BasicVRTPacket;
  }
}

ostream& operator<< (ostream &s, VRTConfig::VITAVersion val) {
  switch (val) {
    case VRTConfig::VITAVersion_V49:  return s << "V49";
    case VRTConfig::VITAVersion_V49b: return s << "V49b";
    default:                          return s << "Unknown VITAVersion " << (int32_t)val;
  }
}

string operator+ (string  &s, VRTConfig::VITAVersion val) {
  ostringstream str;
  str << s << val;
  return str.str();
}

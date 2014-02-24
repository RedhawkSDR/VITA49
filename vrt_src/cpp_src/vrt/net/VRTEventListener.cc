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

#include "VRTEventListener.h"

using namespace std;
using namespace vrt;

/** No argument constructor. */
VRTEventListener::VRTEventListener () :
  _receivedPacket(NULL),
  _sentPacket(NULL),
  _errorOccurred(NULL),
  _warningOccurred(NULL)
{
  // done
}

VRTEventListener::VRTEventListener (void (*receivedPacket ) (const VRTEvent &e, const BasicVRTPacket *p),
                                    void (*sentPacket     ) (const VRTEvent &e, const BasicVRTPacket *p),
                                    void (*errorOccurred  ) (const VRTEvent &e, const string &msg, const VRTException &t),
                                    void (*warningOccurred) (const VRTEvent &e, const string &msg, const VRTException &t)) :
  _receivedPacket(receivedPacket),
  _sentPacket(sentPacket),
  _errorOccurred(errorOccurred),
  _warningOccurred(warningOccurred)
{
  // done
}

VRTEventListener::VRTEventListener (const VRTEventListener &l) :
  _receivedPacket(l._receivedPacket),
  _sentPacket(l._sentPacket),
  _errorOccurred(l._errorOccurred),
  _warningOccurred(l._warningOccurred)
{
  // done
}

VRTEventListener::~VRTEventListener () {
  // done
}

void VRTEventListener::receivedPacket (const VRTEvent &e, const BasicVRTPacket *p) {
  if (_receivedPacket != NULL) {
    _receivedPacket(e, p);
  }
  else {
    delete p;
  }
}

void VRTEventListener::sentPacket (const VRTEvent &e, const BasicVRTPacket *p) {
  if (_sentPacket != NULL) {
    _sentPacket(e, p);
  }
  else {
    delete p;
  }
}

void VRTEventListener::errorOccurred (const VRTEvent &e, const string &msg, const VRTException &t) {
  if (_errorOccurred != NULL) {
    _errorOccurred(e, msg, t);
  }

}

void VRTEventListener::warningOccurred (const VRTEvent &e, const string &msg, const VRTException &t) {
  if (_warningOccurred != NULL) {
    _warningOccurred(e, msg, t);
  }
}

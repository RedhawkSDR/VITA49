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

#include "VRTContextListener.h"

using namespace std;
using namespace vrt;

string VRTContextListener::NO_CONTEXT_STREAM = "Timeout with no context streams found.";
string VRTContextListener::NO_DATA_STREAM    = "Timeout with no data streams found.";

/** No argument constructor. */
VRTContextListener::VRTContextListener () :
  VRTEventListener(NULL, NULL, NULL, NULL),
  _receivedDataPacket(NULL),
  _receivedContextPacket(NULL),
  _receivedInitialContext(NULL)
{
  // done
}

VRTContextListener::VRTContextListener (void (*receivedPacket        ) (const VRTEvent &e, const BasicVRTPacket *p),
                                        void (*receivedDataPacket    ) (const VRTEvent &e, const BasicDataPacket *p),
                                        void (*receivedContextPacket ) (const VRTEvent &e, const BasicContextPacket *p),
                                        void (*receivedInitialContext) (const VRTEvent &e, const string &errorMsg,
                                                                                           const BasicDataPacket &data,
                                                                                           const BasicContextPacket &ctx,
                                                                                           const map<int32_t,BasicContextPacket> &context),
                                        void (*sentPacket            ) (const VRTEvent &e, const BasicVRTPacket *p),
                                        void (*errorOccurred         ) (const VRTEvent &e, const string &msg, const VRTException &t),
                                        void (*warningOccurred       ) (const VRTEvent &e, const string &msg, const VRTException &t)) :
  VRTEventListener(receivedPacket, sentPacket, errorOccurred, warningOccurred),
  _receivedDataPacket(receivedDataPacket),
  _receivedContextPacket(receivedContextPacket),
  _receivedInitialContext(receivedInitialContext)
{
  // done
}

VRTContextListener::VRTContextListener (void (*receivedDataPacket    ) (const VRTEvent &e, const BasicDataPacket *p),
                                        void (*receivedContextPacket ) (const VRTEvent &e, const BasicContextPacket *p),
                                        void (*receivedInitialContext) (const VRTEvent &e, const string &errorMsg,
                                                                                           const BasicDataPacket &data,
                                                                                           const BasicContextPacket &ctx,
                                                                                           const map<int32_t,BasicContextPacket> &context),
                                        void (*errorOccurred         ) (const VRTEvent &e, const string &msg, const VRTException &t),
                                        void (*warningOccurred       ) (const VRTEvent &e, const string &msg, const VRTException &t)) :
  VRTEventListener(NULL, NULL, errorOccurred, warningOccurred),
  _receivedDataPacket(receivedDataPacket),
  _receivedContextPacket(receivedContextPacket),
  _receivedInitialContext(receivedInitialContext)
{
  // done
}

VRTContextListener::VRTContextListener (const VRTContextListener &l) :
  VRTEventListener(l),
  _receivedDataPacket(l._receivedDataPacket),
  _receivedContextPacket(l._receivedContextPacket),
  _receivedInitialContext(l._receivedInitialContext)
{
  // done
}

VRTContextListener::~VRTContextListener () {
  // done
}

void VRTContextListener::receivedDataPacket (const VRTEvent &e, const BasicDataPacket *p) {
  if (_receivedDataPacket != NULL) {
    _receivedDataPacket(e, p);
  }
  else {
    delete p;
  }
}

void VRTContextListener::receivedContextPacket (const VRTEvent &e, const BasicContextPacket *p) {
  if (_receivedContextPacket != NULL) {
    _receivedContextPacket(e, p);
  }
  else {
    delete p;
  }
}

void VRTContextListener::receivedInitialContext (const VRTEvent &e, const string &errorMsg,
                                                 const BasicDataPacket &data, const BasicContextPacket &ctx,
                                                 const map<int32_t,BasicContextPacket> &context) {
  if (_receivedInitialContext != NULL) {
    _receivedInitialContext(e, errorMsg, data, ctx, context);
  }
}

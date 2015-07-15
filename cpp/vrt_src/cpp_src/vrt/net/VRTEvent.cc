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

#include "VRTEvent.h"

using namespace std;
using namespace vrt;

//VRTEvent::VRTEvent (VRTObject &source) :
//  source(&source),
//  sourceStr(source.toString()),
//  packet()
//{
//  // done
//}

VRTEvent::VRTEvent (VRTObject *_source) :
  source(_source),
  sourceStr(),
  packet()
{
  if (_source == NULL) {
    sourceStr = "<null>";
  }
  else {
    sourceStr = _source->toString();
  }
}

//VRTEvent::VRTEvent (VRTObject &source, BasicVRTPacket &packet) :
//  source(&source),
//  sourceStr(),
//  packet(packet)
//{
//  sourceStr = source.toString();
//}

VRTEvent::VRTEvent (VRTObject *_source, BasicVRTPacket &packet) :
  source(_source),
  sourceStr("???"),
  packet(packet)
{
  if (_source == NULL) {
    sourceStr = "<null>";
  }
  else {
    sourceStr = _source->toString();
  }
}

VRTEvent::VRTEvent (const VRTEvent &e) :
  source(e.source),
  sourceStr(e.sourceStr),
  packet(e.packet)
{
  // done
}

string VRTEvent::toString () const {
  ostringstream str;
  str << getClassName() << " [source=" << sourceStr << "]";
  return str.str();
}

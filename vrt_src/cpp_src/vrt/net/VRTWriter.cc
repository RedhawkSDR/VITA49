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

#include "VRTWriter.h"
#include <string>
#include <unistd.h>

using namespace std;
using namespace vrt;

VRTWriter::VRTWriter (const string &host, int32_t port, const string &device, bool useFrames, bool useCRC, uint8_t ttl) :
  multicast(multicast_server(device.c_str(), host.c_str(), port, ttl)),
  frames(useFrames),
  crc(useCRC),
  frameCounter(-1)
{
  // done
}

VRTWriter::~VRTWriter () {
  close();
}

void VRTWriter::close () {
  multicast_close(multicast);
}

void VRTWriter::updateCounter (BasicVRTPacket &p) {
  int64_t code = p.getStreamCode();

  int32_t count;
  if (packetCounters.count(code) == 0) {
    count = 0;
  }
  else {
    count = packetCounters[code];
  }
  count++;
  packetCounters[code] = count;
  p.setPacketCount(count % 16);
}

void VRTWriter::sendPacket (const BasicVRTPacket &p) {
  if (frames) {
    vector<BasicVRTPacket> packets(1);
    packets[0] = p;
    sendAllPackets0(packets);
    return;
  }
  
  // SANITY CHECKS
  string err = p.getPacketValid(false);
  if (err != "") throw VRTException(err);

  // UPDATE COUNTERS (our copy only)
  BasicVRTPacket packet = p;
  updateCounter(packet);

  // SEND THE PACKET
  multicast_transmit(multicast, packet.getPacketPointer(), packet.getPacketLength());
}

void VRTWriter::sendAllPackets0 (vector<BasicVRTPacket> &packets, bool updateCounters) {
  BasicVRLFrame frame;

  // UPDATE COUNTERS (our copy only) AND CRC (if applicable)
  if (updateCounters) {
    for (size_t i = 0; i < packets.size(); i++) {
      updateCounter(packets[i]);
    }
  }
  frame.setFrameCount((++frameCounter) & 0xFFF);
  if (crc) frame.updateCRC();

  // FIT PACKETS INTO FRAME/DATAGRAM
  int32_t count = frame.setVRTPackets(FRAME_BREAK, packets);
  if (count < 1) {
    frame.setVRTPackets(BasicVRLFrame::MAX_FRAME_LENGTH, packets);
    count = 1;
  }

  // SEND THE FRAME
  multicast_transmit(multicast, frame.getFramePointer(), frame.getFrameLength());
  
  // SEND ANY PACKETS THAT DID NOT FIT IN THIS FRAME
  if (((size_t)count) < packets.size()) {
    vector<BasicVRTPacket> next(packets.size()-count);
    memcpy(&next[0], &packets[count], next.size());
    sendAllPackets0(next, false);
  }
}

void VRTWriter::sendAllPackets (const vector<BasicVRTPacket> &packets) {
  if (!frames) {
    for (size_t i = 0; i < packets.size(); i++) {
      sendPacket(packets[i]);
    }
  }
  else {
    vector<BasicVRTPacket> modPackets = packets;
    sendAllPackets0(modPackets);
  }
}

void VRTWriter::sendPacketsVaArgs (const BasicVRTPacket *firstPacket, ...) {
  const BasicVRTPacket*  packet;
  va_list ap;

  if (frames) {
    vector<BasicVRTPacket> packets(1);
    int32_t                count = 1;

    packets[0] = *firstPacket;
    
    va_start(ap, firstPacket);
    while ((packet = va_arg(ap,BasicVRTPacket*)) != NULL) {
      packets.resize(count+1);
      packets[count++] = *packet;
    }
    va_end(ap);

    sendAllPackets0(packets);
  }
  else {
    sendPacket(*firstPacket);

    va_start(ap, firstPacket);
    while ((packet = va_arg(ap,BasicVRTPacket*)) != NULL) {
      sendPacket(*packet);
    }
    va_end(ap);
  }
}

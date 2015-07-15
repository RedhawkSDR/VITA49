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

#include "VRTReader.h"
#include <ctime>
#include <cstring>
#include <string>
#include <map>
#include <vector>
#include <unistd.h>


using namespace std;
using namespace vrt;

const int32_t VRTReader::DEFAULT_TIMEOUT   = 60;
const int32_t VRTReader::UNLIMITED_TIMEOUT = -1;
const int32_t VRTReader::LEGACY_MODE       = -2;
const int32_t VRTReader::FOUND_INITIAL     = -3;

void VRTReader::ThreadedMulticastReader::runThread() {
  while (!stopNow()) {
    // RECONNECT, IF REQUESTED
    // ...not implemented yet (see Java code)

    // RECEIVE THE PACKET AND ADD IT TO THE QUEUE
    ssize_t numRead = multicast_receive(reader->multicast, &reader->buffer[0], reader->buffer.size(), 0.100);

    if (numRead <= 0) { // numRead=0 on time-out
      continue;
    }

    // Don't use the STL resize(..) function since it does not actually trim the size of the
    // buffer on many of the Linux platforms. Rather create a new buffer that has the exact
    // size required. This is silly, but not doing this can result in a huge performance hit.

    if ((reader->buffer[0] == BasicVRLFrame::VRL_FAW_0) && (reader->buffer[1] == BasicVRLFrame::VRL_FAW_1) &&
        (reader->buffer[2] == BasicVRLFrame::VRL_FAW_2) && (reader->buffer[3] == BasicVRLFrame::VRL_FAW_3)) {
      reader->splitFrame(reader->buffer, numRead);
    }
    else if ((reader->buffer[0] & 0xC0) == 0) {
      reader->packetQueue.push(new BasicDataPacket(reader->buffer, 0, numRead, reader->readOnly), 100000000);
    }
    else {
      reader->packetQueue.push(new BasicContextPacket(reader->buffer, 0, numRead, reader->readOnly), 100000000);
    }
  }
}

void VRTReader::ThreadedQueueReader::runThread() {
  while (!stopNow()) {
    BasicVRTPacket* p = reader->packetQueue.pop(100000000);
    if (p != NULL) {
      reader->handle(p);
      if (reader->deletePointers)
        delete p;
    }
  }
}

#define NO_CONTEXT_STREAM  VRTContextListener::NO_CONTEXT_STREAM.c_str()
#define NO_DATA_STREAM     VRTContextListener::NO_DATA_STREAM.c_str()


#define listenerSupport_fireErrorOccurred(...) \
  if (listener != NULL) { listener->errorOccurred(event, Utilities::format(__VA_ARGS__), VRTException()); }

#define listenerSupport_warningErrorOccurred(...) \
  if (listener != NULL) { listener->warningOccurred(event, Utilities::format(__VA_ARGS__), VRTException()); }

#define listenerSupport_fireReceivedPacket(pkt) \
  if (listener != NULL) { listener->receivedPacket(event, pkt); }

#define listenerSupport_fireReceivedDataPacket(pkt) \
  if (listener != NULL) { listener->receivedDataPacket(event, pkt); }

#define listenerSupport_fireReceivedContextPacket(pkt) \
  if (listener != NULL) { listener->receivedContextPacket(event, pkt); }

#define fireReceivedInitialContext(...) \
  if (listener != NULL) { \
    listener->receivedInitialContext(VRTEvent(this), Utilities::format(__VA_ARGS__), \
                                     initialData, initialCtx, \
                                     initialContext); \
  } \
  startTimeMS     = FOUND_INITIAL; \
  initialData     = BasicDataPacket(vector<char>(0)); \
  initialCtx      = BasicContextPacket(vector<char>(0)); \
  idContext       = INT32_NULL; \
  initialContext.clear(); \
  requiredContext.clear();

/** The following definition should be used in place of 'fireReceivedInitialContext("")'
 *  to avoid a "zero-length gnu_printf format string" warning in GCC (seen in GCC 4.4.5).
 */
#define fireReceivedInitialContextDone() fireReceivedInitialContext("%s","")

/** Gets the current time in ms. */
static inline int64_t currentTimeMillis () {
  struct timeval tv;

  if (gettimeofday(&tv, NULL) == 0) {
    return (((int64_t)tv.tv_sec ) * 1000)
         + (((int64_t)tv.tv_usec) / 1000);
  }
  else {
    throw VRTException("Unable to get current time using gettimeofday(..)");
  }
}

/** Checks to see if all the required keys are present in a map. */
template <typename T>
static inline bool containsAll (map<int32_t,T> present, set<int32_t> required) {
  set<int32_t>::iterator it;
  for (it=required.begin(); it!=required.end(); it++) {
    if (present.count(*it) == 0) return false;
  }
  return true;
}

/** Checks to see if all the required keys are present in a map. */
static inline string toStringSet (set<int32_t> myset) {
  set<int32_t>::iterator it;
  ostringstream str;

  str << "[ ";
  for (it=myset.begin(); it!=myset.end(); it++) {
    str << *it << " ";
  }
  str << "]";
  return str.str();
}

/** Checks to see if all the required keys are present in a map. */

template <typename T>
static inline string toStringKeys (map<int32_t,T> mymap) {
  typename map<int32_t,T>::iterator it;
  ostringstream str;

  str << "[ ";
  for (it=mymap.begin(); it!=mymap.end(); it++) {
    str << (*it).first << " ";
  }
  str << "]";
  return str.str();
}

VRTReader::VRTReader (const string &host, int32_t port, const string &device,
                      int32_t recvBufSize, int32_t packetQueueSize, bool readOnly) :
  queueReader(this),
  multicastReader(this),
  frameCounter(-1),
  multicast(multicast_client(device.c_str(), host.c_str(), port, (int)recvBufSize)),
  buffer(BasicVRTPacket::MAX_PACKET_LENGTH),
  host(host),
  port(port),
  device(device),
  recvBufSize(recvBufSize),
  packetQueueSize(packetQueueSize),
  timeoutMS(LEGACY_MODE),
  startTimeMS(LEGACY_MODE),
  initialData(vector<char>(0)),
  initialCtx(vector<char>(0)),
  initialContext(),
  requiredContext(),
  idContext(INT32_NULL),
  listener(NULL),
  packetQueue(packetQueueSize),
  deletePointers(false),
  readOnly(readOnly),
  event(this)
{

  if (multicast.sock < 0) {
    throw VRTException("Could not create socket host=%s port=%d device=%s", host.c_str(), port, device.c_str());
  }
}

VRTReader::VRTReader (const string &host, int32_t port, const string &device,
                      VRTContextListener *listener, double timeout,
                      int32_t recvBufSize, int32_t packetQueueSize,
                      bool readOnly, bool deletePointers) :
  queueReader(this),
  multicastReader(this),
  frameCounter(-1),
  multicast(multicast_client(device.c_str(), host.c_str(), port, (int)recvBufSize)),
  buffer(BasicVRTPacket::MAX_PACKET_LENGTH),
  host(host),
  port(port),
  device(device),
  recvBufSize(recvBufSize),
  packetQueueSize(packetQueueSize),
  timeoutMS((timeout < 0)? ((int64_t)timeout) : ((int64_t)(timeout * 1000))),
  startTimeMS(0),
  initialData(vector<char>(0)),
  initialCtx(vector<char>(0)),
  initialContext(),
  requiredContext(),
  idContext(INT32_NULL),
  listener(listener),
  packetQueue(packetQueueSize),
  deletePointers(deletePointers),
  readOnly(readOnly),
  event(this)
{
  if (multicast.sock < 0) {
    throw VRTException("Could not create socket host=%s port=%d device=%s", host.c_str(), port, device.c_str());
  }
}

VRTReader::~VRTReader () {
  close();
}



string VRTReader::toString () const {
  ostringstream str;
  str << getClassName() << " on ";
  
  if (multicast.addr.sin_family == AF_INET) {
    str << inet_ntoa(multicast.addr.sin_addr);
    str << ":" << ntohs(multicast.addr.sin_port);
  }
  else {
    str << "<unknown>";
  }
  return str.str();
}

void VRTReader::close () {
  multicast_close(multicast);
}

void VRTReader::shutdown () {
  multicast_close(multicast);
}

//void VRTReader::runThread () {
//  while (!stopNow()) {
//    // RECONNECT, IF REQUESTED
//    // ...not implemented yet (see Java code)
//
//    // RECEIVE THE PACKET AND ADD IT TO THE QUEUE
//    ssize_t numRead = multicast_receive(multicast, &buffer[0], buffer.size(), 0.100);
//
//    if (numRead <= 0) { // numRead=0 on time-out
//      continue;
//    }
//
//    // Don't use the STL resize(..) function since it does not actually trim the size of the
//    // buffer on many of the Linux platforms. Rather create a new buffer that has the exact
//    // size required. This is silly, but not doing this can result in a huge performance hit.
//
//    if ((buffer[0] == BasicVRLFrame::VRL_FAW_0) && (buffer[1] == BasicVRLFrame::VRL_FAW_1) &&
//        (buffer[2] == BasicVRLFrame::VRL_FAW_2) && (buffer[3] == BasicVRLFrame::VRL_FAW_3)) {
//      splitFrame(buffer, numRead);
//    }
//    else if ((buffer[0] & 0xC0) == 0) {
//      packetQueue.push(new BasicDataPacket(buffer, 0, numRead, false));
////      handle(new BasicDataPacket(buffer, 0, numRead, false));
//    }
//    else {
//      packetQueue.push(new BasicContextPacket(buffer, 0, numRead, false));
////      handle(new BasicContextPacket(buffer, 0, numRead, false));
//    }
//  }
//}

vector<BasicVRTPacket*> VRTReader::_splitFrame(const vector<char> &buffer, ssize_t numRead) {
  vector<BasicVRTPacket*> packets = BasicVRLFrame::getVRTPackets(buffer, numRead, readOnly);
  for (vector<BasicVRTPacket*>::iterator it = packets.begin(); it != packets.end(); ++it) {
    if (!_handlePacket(*it, true)) {
      it = packets.erase(it);
    }
  }
  return packets;
}

vector<BasicVRTPacket*> VRTReader::splitFrame(const vector<char> &buffer, ssize_t numRead) {
  vector<BasicVRTPacket*> packets = BasicVRLFrame::getVRTPackets(buffer, numRead, readOnly);
  for (size_t i=0; i<packets.size(); i++) {
    packetQueue.push(packets[i]);
  }
  return packets;
}

vector<BasicVRTPacket*> VRTReader::doReceivePackets (float timeout) {
  ssize_t numRead = multicast_receive(multicast, &buffer[0], buffer.size(), timeout);
  
  if (numRead <= 0) { // numRead=0 on time-out
    return vector<BasicVRTPacket*>(0);
  }
  
  // Don't use the STL resize(..) function since it does not actually trim the size of the
  // buffer on many of the Linux platforms. Rather create a new buffer that has the exact
  // size required. This is silly, but not doing this can result in a huge performance hit.

  if ((buffer[0] == BasicVRLFrame::VRL_FAW_0) && (buffer[1] == BasicVRLFrame::VRL_FAW_1) &&
      (buffer[2] == BasicVRLFrame::VRL_FAW_2) && (buffer[3] == BasicVRLFrame::VRL_FAW_3)) {
    return _splitFrame(buffer, numRead);
  }
  else if ((buffer[0] & 0xC0) == 0x00) {
    return _handle(new BasicDataPacket(buffer, 0, numRead, readOnly));
  }
  else {
    return _handle(new BasicContextPacket(buffer, 0, numRead, readOnly));
  }
}

vector<BasicVRTPacket*> VRTReader::receivePackets (float timeout, size_t count) {
  vector<BasicVRTPacket*> packets = doReceivePackets(timeout);
  
  if (packets.size() > 0) {
    while (packets.size() < count) {
      vector<BasicVRTPacket*> pkts = doReceivePackets(0.0f);
      if (pkts.size() == 0) break;
      
      for (size_t i = 0; i < pkts.size(); i++) {
        packets.push_back(pkts[i]);
      }
    }
  }
  return packets;
}

bool VRTReader::_handlePacket (const BasicVRTPacket *packet, bool fromFrames) {
  string err = packet->getPacketValid(false);
  if (err != "") {
    cout << err << endl;
    return false;
  }
  int64_t code     = packet->getStreamCode();
  int32_t count    = packet->getPacketCount();
  int32_t expected = (packetCounters.count(code) == 0)? count : packetCounters[code];
  int32_t next     = (count+1) & 0xF;
  
  packetCounters[code] = next;
  
  if (count != expected) {
    cout << "Missed packets " << expected << " (inclusive) to " << count << " (exclusive)." << endl;
  }
  return true;
}

//vector<BasicVRTPacket*> VRTReader::_handle (BasicVRLFrame *frame) {
//  if (!frame->isFrameValid()) {
//    if (frame->isCRCValid()) cout << "Received invalid frame" << endl;
//    else                     cout << "Received corrupt frame" << endl;
//    return vector<BasicVRTPacket*>(0);
//  }
//  int32_t count    = frame->getFrameCount();
//  int32_t next     = (count+1) & 0xFFF;
//  int32_t expected = frameCounter;
//
//  frameCounter = next;
//
//  if ((expected != count) && (expected >= 0)) {
//    cout << "Missed frames " << expected << " (inclusive) to " << count << " (exclusive)." << endl;
//    //packetCounters = map<int64_t,int32_t>(); // clear packet counters
//  }
//
//  vector<BasicVRTPacket*> packets = frame->getVRTPackets();
//  for (size_t i = 0; i < packets.size(); i++) {
//    if (!_handlePacket(packets[i], true)) {
//      return vector<BasicVRTPacket*>(0); // ERROR
//    }
//  }
//  return packets;
//}

vector<BasicVRTPacket*> VRTReader::_handle (BasicVRTPacket *packet) {
  vector<BasicVRTPacket*> packets(1);
  packets[0] = packet;
  
  if (!_handlePacket(packets[0], false)) {
    delete packet;
    return vector<BasicVRTPacket*>(0); // ERROR
  }
  return packets;
}

//void VRTReader::handle (BasicVRLFrame *frame) {
//  if (startTimeMS == 0) {
//    // start the clock on the first datagram received
//    startTimeMS = currentTimeMillis();
//  }
//  if (!frame->isFrameValid()) {
//    if (frame->isCRCValid()) { listenerSupport_fireErrorOccurred("Received invalid frame"); }
//    else                    { listenerSupport_fireErrorOccurred("Received corrupt frame"); }
//    return;
//  }
//  int32_t count    = frame->getFrameCount();
//  int32_t next     = (count+1) & 0xFFF;
//  int32_t expected = frameCounter;
//
//  frameCounter = next;
//
//  if ((expected != count) && (expected >= 0)) {
//    listenerSupport_fireErrorOccurred("Missed frames %d (inclusive) to %d (exclusive).", expected, count);
//  }
//
//  vector<BasicVRTPacket*> packets = frame->getVRTPackets();
//  for (size_t i = 0; i < packets.size(); i++) {
//    handle(packets[i]);
//  }
//}

void VRTReader::handle (BasicVRTPacket *packet) {
  if (startTimeMS == 0) {
    // start the clock on the first datagram received
    startTimeMS = currentTimeMillis();
  }
  string err = packet->getPacketValid(false);
  if (err != "") {
    listenerSupport_fireErrorOccurred("%s", err.c_str());
    return;
  }
  int64_t code     = packet->getStreamCode();
  int32_t count    = packet->getPacketCount();
  int32_t expected = (packetCounters.count(code) == 0)? count : packetCounters[code];
  int32_t next     = (count+1) & 0xF;

  packetCounters[code] = next;

  if (count != expected) {
    listenerSupport_fireErrorOccurred("Missed packets %d (inclusive) to %d (exclusive).", expected, count);
  }

  // SEE IF WE ARE USING LEGACY MODE ========================================
  if (startTimeMS == LEGACY_MODE) {
    listenerSupport_fireReceivedPacket(packet);
    return;
  }

  // SEE IF WE ALREADY FOUND INITIAL CONTEXT ================================
  if (startTimeMS == FOUND_INITIAL) {
    if (packet->isData()) {
      listenerSupport_fireReceivedDataPacket(checked_dynamic_cast<BasicDataPacket*>(packet));
      return;
    }
    else {
      listenerSupport_fireReceivedContextPacket(checked_dynamic_cast<BasicContextPacket*>(packet));
      return;
    }
  }

  // STILL NEED MORE CONTEXT (OR INITIAL DATA PACKET) =======================
  long now     = currentTimeMillis();
  bool timeout = (timeoutMS > 0) && (startTimeMS+timeoutMS <= now);

  // ---- If this is a DataPacket, handle it as such ------------------------
  if (packet->isData()) {
    initialData = BasicDataPacket(*packet);
    idContext   = initialData.getStreamIdentifier();

    if (!deletePointers)
      delete packet;

    if (isNull(idContext)) {
      cout<<"FIRING INITIAL CONTEXT 1"<<endl;
      fireReceivedInitialContextDone(); // unidentified stream (i.e. done)
      return;
    }
    else if (timeout) {
      cout<<"FIRING INITIAL CONTEXT 2"<<endl;
      fireReceivedInitialContext(NO_CONTEXT_STREAM);
      return;
    }
    else {
      return;
    }
  }

  // ---- Found a context packet --------------------------------------------
  int id = packet->getStreamIdentifier();
  initialContext[id] = *checked_dynamic_cast<BasicContextPacket*>(packet);

  // ---- Is this a non-ContextPacket primary stream (rare)? ----------------
  if (!isNull(idContext) && (id == idContext) && (packet->getPacketType() != PacketType_Context)) {

    if (!deletePointers)
      delete packet;
    if (initialContext.size() == 1) {
      cout<<"FIRING INITIAL CONTEXT 3"<<endl;
      fireReceivedInitialContextDone();
      return;
    }
    else {
      cout<<"FIRING INITIAL CONTEXT 4"<<endl;
      fireReceivedInitialContext("Context packets do not follow stream ID rules (found streams %s "
                                 "but expected only %s).", toStringKeys(initialContext).c_str(),
                                  toStringSet(requiredContext).c_str());
      return;
    }
  }

  // ---- For any ContextPackets, check assoc. lists first ------------------
  if (packet->getPacketType() == PacketType_Context) {
    BasicContextPacket *ctx = checked_dynamic_cast<BasicContextPacket*> (packet);
    ContextAssocLists  assoc = ctx->getContextAssocLists();

    if (!isNull(idContext) && (id == idContext)) {
      // it is the primary, set it and mark as required
      initialCtx = BasicContextPacket(*ctx);
      requiredContext.insert(id);
    }
    if (!isNull(assoc)) {
      vector<int32_t> src = assoc.getSourceContext();
      vector<int32_t> sys = assoc.getSystemContext();
      
      for (size_t i = 0; i < src.size(); i++) requiredContext.insert(src[i]);
      for (size_t i = 0; i < sys.size(); i++) requiredContext.insert(sys[i]);
    }
  }

  // ---- Check to see if done ----------------------------------------------
  bool foundCtx = !isNull(initialCtx);
  bool sameSize = initialContext.size() == requiredContext.size();
  bool foundAll = containsAll(initialContext, requiredContext);

  if (foundCtx && foundAll) {

    if (!deletePointers)
      delete packet;
    // found all required context
    if (sameSize) {
      fireReceivedInitialContextDone();
      return;
    }
    else {
      fireReceivedInitialContext("Context packets do not follow stream ID rules (found streams %s "
                                 "but expected %s).", toStringKeys(initialContext).c_str(),
                                  toStringSet(requiredContext).c_str());
      return;
    }
  }

  if (timeout && foundCtx) {

    if (!deletePointers)
      delete packet;
    // timeout before finding all required context
    if (sameSize) {
      fireReceivedInitialContext("Context packets do not follow stream ID rules (found streams %s "
                                 "but expected %s).", toStringKeys(initialContext).c_str(),
                                  toStringSet(requiredContext).c_str());
      return;
    }
    else {
      cout<<"FIRING INITIAL CONTEXT 8"<<endl;
      fireReceivedInitialContext("Timeout before all required context could be found (found streams %s "
                                 "but expected %s).", toStringKeys(initialContext).c_str(),
                                  toStringSet(requiredContext).c_str());
      return;
    }
  }

  if (timeout) {

    if (!deletePointers)
      delete packet;
    // timeout before finding primary context
    if (isNull(initialData)) {
      cout<<"FIRING INITIAL CONTEXT 9"<<endl;
      fireReceivedInitialContext(NO_DATA_STREAM);
      return;
    }
    else {
      cout<<"FIRING INITIAL CONTEXT 10"<<endl;
      fireReceivedInitialContext("Could not find IF Context for stream ID %s.",
                                 initialData.getStreamID().c_str());
      return;
    }
  }

  if (!deletePointers)
    delete packet;
}


void VRTReader::join() {
  queueReader.join();
  multicastReader.join();
}


void VRTReader::start() {
  queueReader.start();
  multicastReader.start();
}


void VRTReader::stop(bool wait) {
  queueReader.stop(wait);
  multicastReader.stop(wait);
}


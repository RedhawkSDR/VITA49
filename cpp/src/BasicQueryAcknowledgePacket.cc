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

#include "BasicQueryAcknowledgePacket.h"
#include <bitset> // includes the popcount functions // TODO needed?

using namespace vrt;

//TODO if the below is needed, consider moving to a .h file to be included
//     rather than duplicating in multiple .cc files (also in BAsicContextPacket.cc)
/** Basic population count. This algorithm comes from [1] and is believed to be
 *  nearly-identical to what is being done in Java. The use of GCC's
 *  <tt>__builtin_popcount(..)</tt> was considered, but excluded since it is
 *  slower than <tt>__builtin_popcountll(..)</tt> due to a GCC bug (see [2]).
 *  Use of <tt>__builtin_popcountll(..)</tt> was excluded since, in practice,
 *  it is comparable to what is done here (see measurement results in [2]).
 *  <pre>
 *    References:
 *     [1] Wikipedia. "Hamming Weight." http://en.wikipedia.org/wiki/Hamming_weight
 *         (Last Accessed 23 April 2013)
 *
 *     [2] Dalke, Andrew. "Update: Faster population counts." 2 November 2011.
 *         http://www.dalkescientific.com/writings/diary/archive/2011/11/02/faster_popcount_update.html
 *         (Last Accessed 23 April 2013)
 *  </pre>
 */
# if defined(__INTEL_COMPILER)
#  define bitCount(val) _popcnt32(val)
# else
static inline int32_t bitCount (int32_t val) {
  uint32_t n = (uint32_t)val;
  n = n - ((n >> 1) & 0x55555555);
  n = (n & 0x33333333) + ((n >> 2) & 0x33333333);
  n = (n + (n >> 4)) & 0x0F0F0F0F;
  n = (n + (n >> 8));
  n = (n + (n >> 16));
  return n & 0x3F;
}
#endif


  /*****************************************************************************/
  /*****************************************************************************/
  /**                                                                         **/
  /**                        BasicQueryAcknowledgePacket (AckS)               **/
  /**                                                                         **/
  /*****************************************************************************/
  /*****************************************************************************/

/**  Creates the default prologue for the packet. This is defined separate from the class to account
 *  for the systems/compilers that disallow calling of subclass methods from the constructor.
 */
static inline vector<char> BasicQueryAcknowledgePacket_createDefaultPacket () {
  vector<char> buf(BasicVRTPacket::MAX_PROLOGUE_LENGTH + 4);
  buf[0]  = 0x6C; // Ack w/ CID, not StaleTs, not Cancel
  buf[1]  = 0x60; // TSI: UTC, TSF: Real-Time (ps) fractional timestamp, packet count =0
  buf[2]  = 0x00; // 
  buf[3]  = 0x12; // Packet size = 18 (full header, full psp, +1 for CIF0)
  buf[28] = 0xF0; // Has 128-bit ControlleeID and 128-bit ControllerID
  buf[29] = 0x04; // AckS/Query
  buf[68] = 0x80; // ChangeIndicator bit set (default true for AckS Packets)
  return buf;
}

//======================================================================
// CONSTRUCTORS
//======================================================================

// TODO constructors
BasicQueryAcknowledgePacket::BasicQueryAcknowledgePacket (const BasicVRTPacket &p) :
  BasicVRTPacket(p)
{
  if (!isNullValue() && (getPacketType() != PacketType_Command)) {
    throw VRTException("Can not create QueryAcknowledgePacket from given packet");
  }
}

BasicQueryAcknowledgePacket::BasicQueryAcknowledgePacket (const BasicVRTPacket &p, int64_t classID) :
  BasicVRTPacket(p)
{
  if (getPacketType() != PacketType_Command) {
    throw VRTException("Can not create QueryAcknowledgePacket from given packet");
  }
  setClassIdentifier(classID);
}

BasicQueryAcknowledgePacket::BasicQueryAcknowledgePacket (const void *buf, size_t len, bool readOnly) :
  BasicVRTPacket(buf,len,readOnly)
{
  // done
}

BasicQueryAcknowledgePacket::BasicQueryAcknowledgePacket () :
  BasicVRTPacket(BasicQueryAcknowledgePacket_createDefaultPacket(),false)
{
  // done
}

BasicQueryAcknowledgePacket::BasicQueryAcknowledgePacket (int32_t bufsize) :
  BasicVRTPacket(bufsize)
{
  // done
}

BasicQueryAcknowledgePacket::BasicQueryAcknowledgePacket (const vector<char> &buf, ssize_t start, ssize_t end, bool readOnly) :
  BasicVRTPacket(buf, start, end, readOnly)
{
  // done
}

BasicQueryAcknowledgePacket::BasicQueryAcknowledgePacket (const vector<char> &buf, bool readOnly) :
  BasicVRTPacket::BasicVRTPacket(buf,readOnly)
{
  // done
}

BasicQueryAcknowledgePacket::BasicQueryAcknowledgePacket (vector<char> *buf_ptr, bool readOnly) :
  BasicVRTPacket(buf_ptr, readOnly)
{
  // done
}

BasicQueryAcknowledgePacket::BasicQueryAcknowledgePacket (const BasicVRTPacket &p, PacketType type, int64_t classID,
                                        int32_t minPayloadLength, int32_t maxPayloadLength) :
  BasicVRTPacket(p,type,classID,minPayloadLength,maxPayloadLength)
{
  if (type != PacketType_Command) {
    throw VRTException("BasicQueryAcknowledgePacket can only be created with type=Command");
  }
}


// UTILITIES

void BasicQueryAcknowledgePacket::toStringStream (std::ostringstream &str) const {
  BasicControlPacket::toStringStream(str);
  // TODO - anything new to add?
}


/** TODO - implement HasFields methods for BasicQueryAcknowledgePacket
 *  XXX - probably can just use impl from BasicControlPacket... nothing new here
    public: virtual int32_t   getFieldCount () const;
    public: virtual string    getFieldName  (int32_t id) const;
    public: virtual ValueType getFieldType  (int32_t id) const;
    public: virtual Value*    getField      (int32_t id) const __attribute__((warn_unused_result));
    public: virtual void      setField      (int32_t id, const Value* val);
*/


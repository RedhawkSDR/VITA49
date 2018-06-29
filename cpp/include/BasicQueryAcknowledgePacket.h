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

#ifndef _BasicQueryAcknowledgePacket_h
#define _BasicQueryAcknowledgePacket_h

#include "BasicControlPacket.h"
#include <cstring>
#include <vector>
#include <sstream>
#include <iomanip>


namespace vrt {
  using namespace std;

  /*****************************************************************************/
  /*****************************************************************************/
  /**                                                                         **/
  /**                   BasicQueryAcknowledgePacket (AckS)                    **/
  /**                                                                         **/
  /*****************************************************************************/
  /*****************************************************************************/

  class BasicQueryAcknowledgePacket : public BasicControlPacket {

    //======================================================================
    // CONSTRUCTORS
    //======================================================================
    // TODO - set header to necessary settings
    //======================================================================

    /** Basic destructor. */
    public: ~BasicQueryAcknowledgePacket () { }

    /** Basic copy constructor. */
    public: BasicQueryAcknowledgePacket (const BasicVRTPacket &p);

    /** Internal constructor, for use by subclasses.
     *  @param p       The packet (already initialized).
     *  @param classID The expected classID.
     *  @throws VRTException If packet type or classID of the packet are invalid.
     */
    public: BasicQueryAcknowledgePacket (const BasicVRTPacket &p, int64_t classID);

    /** Creates a new instance accessing the given data buffer.
     *  @param buf      The pointer to the packet buffer.
     *  @param len      The length  of the packet buffer.
     *  @param readOnly Should users of this instance be able to modify the underlying data buffer?
     */
    public: BasicQueryAcknowledgePacket (const void *buf, size_t len, bool readOnly=false);

    /** Creates a new instance that can be written to. Initially this will just be a simple command
     *  packet with StreamID, ClassID, Ack Indicator set, TSI=UTC, TSF=Real-time, CAM field,
     *  MessageID, 128-bit ControlleeID, 128-bit ControllerID, Query-State Acknowledge bit set, CIF0,
     *  and a packet size of 18. The Change Indicator bit of CIF0 is set as well.
     */
    public: BasicQueryAcknowledgePacket ();

    /** Creates a new instance with a default anticipated length that can be written to.
     *  Initially this will just be a simple context packet with no fields set (other than
     *  the required packet length of 8 and a default stream ID  of 0), but will have the
     *  underlying buffers pre-allocated as required.
     *  @param bufsize The anticipated buffer size.
     */
    public: BasicQueryAcknowledgePacket (int32_t bufsize);

    public: BasicQueryAcknowledgePacket (const vector<char> &buf, ssize_t start, ssize_t end, bool readOnly = false);

    /** Creates a new instance accessing the given data buffer. Note that when the buffer lengths
     *  are given, only the most minimal of error checking is done. Users should call
     *  <tt>isPacketValid()</tt> to verify that the packet is valid. Invalid packets can result
     *  unpredictable behavior, but this is explicitly allowed (to the extent possible) so that
     *  applications creating packets can use this class even if the packet isn't yet "valid".
     *  @param buf      The data buffer to use.
     *  @param readOnly Should users of this instance be able to modify the underlying data buffer?
     *  @throws VRTException If the offset is negative or greater than the
     *          length of the buffer given. Also thrown if the buffer length is -1 and a copy
     *          can not be made because the sizes in the header of the packet are invalid.
     */
    public: BasicQueryAcknowledgePacket (const vector<char> &buf, bool readOnly = false);

    public: BasicQueryAcknowledgePacket (vector<char> *buf_ptr, bool readOnly = false);

    /** Internal constructor, for use by subclasses.
     *  @param p                The packet (already initialized).
     *  @param type             The expected packet type.
     *  @param classID          The expected classID.
     *  @param minPayloadLength The minimum payload length.
     *  @param maxPayloadLength The maximum payload length (&lt;0 if not pre-defined).
     *  @throws VRTException If packet type or classID of the packet are invalid, or if
     *                       the payload length is invalid.
     */
    protected: BasicQueryAcknowledgePacket (const BasicVRTPacket &p, PacketType type, int64_t classID,
                                   int32_t minPayloadLength, int32_t maxPayloadLength);


    //======================================================================
    // IMPLEMENTATIONS OF VIRTUAL METHODS
    //======================================================================

    protected: virtual void      toStringStream (ostringstream &str) const;
    /*public:    virtual string    getPacketValid (bool strict, int32_t length) const {
      return BasicCommandPacket::getPacketValid(strict, length);
    }*/
    public: virtual inline void setPacketType (PacketType t) {
      if (t != PacketType_Command) {
        throw VRTException("Can not change packet type to non-Command type when using BasicQueryAcknowledgePacket");
      }
      BasicVRTPacket::setPacketType(t);
    }

    // TODO - what new things need to be reset?
    /** Prepares a packet for resend resetting any non-persistent values and clearing the context
     *  change flags. This is implemented as follows:
     *  <!--
     *  <pre>
     *    bool BasicQueryAcknowledgePacket::resetForResend (const TimeStamp &t) {
     *      BasicControlPacket::resetForResend(t);
     *      return true;
     *    }
     *  </pre>
     *  @param t The new time stamp.
     *  @return true
     *  -->
     */
    //public: virtual bool resetForResend (const TimeStamp &t);


    //======================================================================
    // QUERY/ACK PACKET METHODS
    //======================================================================
    // TODO - are there any special functions, special cases?


    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Implement HasFields
    //////////////////////////////////////////////////////////////////////////////////////////////////
    /* XXX - These probably can just use the impl from BasicControlPacket
    public: virtual int32_t   getFieldCount () const;
    public: virtual string    getFieldName  (int32_t id) const;
    public: virtual ValueType getFieldType  (int32_t id) const;
    public: virtual Value*    getField      (int32_t id) const __attribute__((warn_unused_result));
    public: virtual void      setField      (int32_t id, const Value* val);
    */

  }; /* BasicQueryAcknowledgePacket */


} END_NAMESPACE /* vrt namespace */

#endif /* _BasicQueryAcknowledgePacket_h */

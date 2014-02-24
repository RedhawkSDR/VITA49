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

#ifndef _VRTContextListener_h
#define _VRTContextListener_h

#include <map>
#include <string>
#include "VRTEventListener.h"
#include "BasicVRTPacket.h"
#include "BasicContextPacket.h"
#include "BasicDataPacket.h"

namespace vrt {
  using namespace std;

  /** An extension to the normal {@link VRTEventListener} that supports additional handling for
   *  context packets. The use of {@link #receivedDataPacket}, {@link #receivedContextPacket},
   *  and/or {@link #receivedInitialContext} are mutually exclusive with {@link #receivedPacket}. <br>
   *  <br>
   *  Note that it is an error to call {@link #receivedInitialContext} more than once (without a
   *  "reset") or to call {@link #receivedDataPacket} or {@link #receivedContextPacket} before the
   *  call to {@link #receivedInitialContext}.
   *
   *  @author         
   */
  class VRTContextListener : public VRTEventListener {
    private: void (*_receivedDataPacket    ) (const VRTEvent &e, const BasicDataPacket *p);
    private: void (*_receivedContextPacket ) (const VRTEvent &e, const BasicContextPacket *p);
    private: void (*_receivedInitialContext) (const VRTEvent &e, const string &errorMsg,
                                              const BasicDataPacket &data, const BasicContextPacket &ctx,
                                              const map<int32_t,BasicContextPacket> &context);

    /** Error message passed with {@link #receivedInitialContext} when no context streams
     *  have been found before the timeout expired.
     */
    public: static string NO_CONTEXT_STREAM;

    /** Error message passed with {@link #receivedInitialContext} when no data stream
     *  has been found before the timeout expired.
     */
    public: static string NO_DATA_STREAM;
      
    /** No argument constructor. */
    public: VRTContextListener ();

    /** Traditional C-style callback constructor. This is similar in concept to the C signal(..)
     *  which function takes pointers to the applicable functions to call.
     *  @param receivedPacket         The function to call in place of receivedPacket(..).
     *  @param receivedDataPacket     The function to call in place of receivedDataPacket(..).
     *  @param receivedContextPacket  The function to call in place of receivedContextPacket(..).
     *  @param receivedInitialContext The function to call in place of receivedInitialContext(..).
     *  @param sentPacket             The function to call in place of sentPacket(..).
     *  @param errorOccurred          The function to call in place of errorOccurred(..).
     *  @param warningOccurred        The function to call in place of warningOccurred(..).
     */
    public: VRTContextListener (void (*receivedPacket        ) (const VRTEvent &e, const BasicVRTPacket *p),
                                void (*receivedDataPacket    ) (const VRTEvent &e, const BasicDataPacket *p),
                                void (*receivedContextPacket ) (const VRTEvent &e, const BasicContextPacket *p),
                                void (*receivedInitialContext) (const VRTEvent &e, const string &errorMsg,
                                                                                   const BasicDataPacket &data,
                                                                                   const BasicContextPacket &ctx,
                                                                                   const map<int32_t,BasicContextPacket> &context),
                                void (*sentPacket            ) (const VRTEvent &e, const BasicVRTPacket *p),
                                void (*errorOccurred         ) (const VRTEvent &e, const string &msg, const VRTException &t),
                                void (*warningOccurred       ) (const VRTEvent &e, const string &msg, const VRTException &t));


    /** Traditional C-style callback constructor. This is similar in concept to the C signal(..)
     *  which function takes pointers to the applicable functions to call.
     *  @param receivedDataPacket     The function to call in place of receivedDataPacket(..).
     *  @param receivedContextPacket  The function to call in place of receivedContextPacket(..).
     *  @param receivedInitialContext The function to call in place of receivedInitialContext(..).
     *  @param errorOccurred          The function to call in place of errorOccurred(..).
     *  @param warningOccurred        The function to call in place of warningOccurred(..).
     */
    public: VRTContextListener (void (*receivedDataPacket    ) (const VRTEvent &e, const BasicDataPacket *p),
                                void (*receivedContextPacket ) (const VRTEvent &e, const BasicContextPacket *p),
                                void (*receivedInitialContext) (const VRTEvent &e, const string &errorMsg,
                                                                                   const BasicDataPacket &data,
                                                                                   const BasicContextPacket &ctx,
                                                                                   const map<int32_t,BasicContextPacket> &context),
                                void (*errorOccurred         ) (const VRTEvent &e, const string &msg, const VRTException &t),
                                void (*warningOccurred       ) (const VRTEvent &e, const string &msg, const VRTException &t));
    
    /** Copy constructor. */
    public: VRTContextListener (const VRTContextListener &l);

    /** Destructor. */
    public: ~VRTContextListener ();
    
    /** This method is called when a VRT data (IF Data or Extension Data) packet has been received
     *  following the receipt of the initial context (see {@link #receivedInitialContext}).
     *  @param e The event object.
     *  @param p The packet received (may be read-only).
     */
    public: virtual void receivedDataPacket (const VRTEvent &e, const BasicDataPacket *p);

    /** This method is called when a VRT context (IF Context or Extension Context) packet has been
     *  received, excluding the initial context (see {@link #receivedInitialContext}).
     *  @param e The event object.
     *  @param p The packet received (may be read-only).
     */
    public: virtual void receivedContextPacket (const VRTEvent &e, const BasicContextPacket *p);

    /** This method is called when the initial set of VRT context packets have been received. Note
     *  that calling this function with a non-null <tt>errorMsg</tt> (as detailed below) is a
     *  substitute for calling {@link #errorOccurred} where it directly relates to the initial
     *  context.
     *  @param e        The event object.
     *  @param errorMsg Did an error occur while trying to get the initial context? This will be set
     *                  to the empty string (<tt>""</tt>) if there were no errors and the attendant
     *                  information is believed to be correct. This will return <tt>NO_CONTEXT_STREAM</tt>
     *                  or <tt>NO_DATA_STREAM</tt> in the event of a timeout where there was data without
     *                  context or context without data -- both of these are valid under the VITA-49
     *                  standard, but often discouraged as they present an ambiguous situation where
     *                  it is impossible to tell what the primary context should be (i.e. <tt>ctx</tt>
     *                  will be <tt>null</tt> and <tt>data</tt> and <tt>context</tt> may be
     *                  incomplete). <br>
     *                  <br>
     *                  Note: <i>The <tt>NO_CONTEXT_STREAM</tt> and <tt>NO_DATA_STREAM</tt> will
     *                  not be used in situations where external information is available that
     *                  identifies the primary context stream or the lack there of. In this
     *                  situation there is no ambiguity and the attendant information is
     *                  believed to be correct.</i><br>
     *                  <br>
     *                  The way the "timeout" is implemented, it results in a call to this function
     *                  at the time the first packet is received following the timeout -- as such
     *                  it is impossible to have a situation where this function is called and no
     *                  packets (data or context) have been received. <br>
     *                  <br>
     *                  A summary of the special cases:
     *                  <pre>
     *                           Error       |   Found     |    Found Any    | Found Primary  |
     *                          Message      | Data Stream | Context Streams | Context Stream |
     *                    -------------------+-------------+-----------------+----------------+
     *                    NO_CONTEXT_STREAM  |     Yes     |       No        |       No       |
     *                    -------------------+-------------+-----------------+----------------+
     *                    NO_DATA_STREAM     |     No      |       Yes       |     Unknown    |
     *                    -------------------+-------------+-----------------+----------------+
     *                    ""                 |     Yes     |       Yes       |       Yes      |
     *                    -------------------+-------------+-----------------+----------------+
     *                    not possible       |     No      |       No        |       No       |
     *                    -------------------+-------------+-----------------+----------------+
     *                  </pre>
     *                  Any other error messages reflect a situation where there has been a clear
     *                  error (i.e. a violation of the VITA-49 specification) and the attendant
     *                  information is the "best available" but clearly believed incorrect -- it is
     *                  however, included as some systems may be able to "recover" from the error
     *                  and/or provide a meaningful diagnostic message to the user.
     *  @param data     The data packet that was used to identify the primary context. Note that this
     *                  data packet should be considered "out of band" and should be discarded (i.e.
     *                  its chronological relationship between <tt>ctx</tt> and any other packets
     *                  received is intentionally undefined); it is present only as an indication that
     *                  there is (or is not) a data stream and its stream identifier. If this data
     *                  packet is valid to process at this time, a subsequent call to
     *                  {@link #receivedDataPacket} will be made.
     *  @param ctx      The primary context packet.
     *  @param context  The context packets received (may be read-only). This will be a mapping of
     *                  stream identifier to context packet. <i>(This will include <tt>ctx</tt>.)</i>
     *                  This map may be empty, but it will never be null. None of the packets included
     *                  in this map will be included in a subsequent {@link #receivedContextPacket}
     *                  call.
     */
    public: virtual void receivedInitialContext (const VRTEvent &e, const string &errorMsg,
                                                 const BasicDataPacket &data, const BasicContextPacket &ctx,
                                                 const map<int32_t,BasicContextPacket> &context);
  };
};
#endif /* _VRTContextListener_h */

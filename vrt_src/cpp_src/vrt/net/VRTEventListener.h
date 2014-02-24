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

#ifndef _VRTEventListener_h
#define _VRTEventListener_h

#include <string>
#include "BasicVRTPacket.h"
#include "VRTEvent.h"

namespace vrt {
  using namespace std;
  
  /** The function definitions for a listener for VRT events. This can either be used as a class to
   *  be extended (similar to what is done in Java or Qt) or to facilitate traditional C-style
   *  callbacks.
   *
   *  @author         
   */
  class VRTEventListener : public VRTObject {
    private: void (*_receivedPacket ) (const VRTEvent &e, const BasicVRTPacket *p);
    private: void (*_sentPacket     ) (const VRTEvent &e, const BasicVRTPacket *p);
    private: void (*_errorOccurred  ) (const VRTEvent &e, const string &msg, const VRTException &t);
    private: void (*_warningOccurred) (const VRTEvent &e, const string &msg, const VRTException &t);

    /** No argument constructor. */
    public: VRTEventListener ();

    /** Traditional C-style callback constructor. This is similar in concept to the C signal(..)
     *  which function takes pointers to the applicable functions to call.
     *  @param receivedPacket  The function to call in place of receivedPacket(..).
     *  @param sentPacket      The function to call in place of sentPacket(..).
     *  @param errorOccurred   The function to call in place of errorOccurred(..).
     *  @param warningOccurred The function to call in place of warningOccurred(..).
     */
    public: VRTEventListener (void (*receivedPacket ) (const VRTEvent &e, const BasicVRTPacket *p),
                              void (*sentPacket     ) (const VRTEvent &e, const BasicVRTPacket *p),
                              void (*errorOccurred  ) (const VRTEvent &e, const string &msg, const VRTException &t),
                              void (*warningOccurred) (const VRTEvent &e, const string &msg, const VRTException &t));
    
    /** Copy constructor. */
    public: VRTEventListener (const VRTEventListener &l);

    /** Destructor. */
    public: ~VRTEventListener ();

    /** This method is called when a VRT packet has been received.
     *  @param e The event object.
     *  @param p The packet received (may be read-only).
     */
    public: virtual void receivedPacket (const VRTEvent &e, const BasicVRTPacket *p);

    /** This method is called when a VRT packet has been sent.
     *  @param e The event object.
     *  @param p The packet received (may be read-only).
     */
    public: virtual void sentPacket (const VRTEvent &e, const BasicVRTPacket *p);

    /** This method is called when an error condition is detected.
     *  @param e   The event object.
     *  @param msg A message describing the error condition.
     *  @param t   The error, if available.
     */
    public: virtual void errorOccurred (const VRTEvent &e, const string &msg, const VRTException &t);

    /** This method is called when a warning condition is detected.
     *  @param e   The event object.
     *  @param msg A message describing the error condition.
     *  @param t   The error, if available.
     */
    public: virtual void warningOccurred (const VRTEvent &e, const string &msg, const VRTException &t);
  };
};
#endif /* _VRTEventListener_h */

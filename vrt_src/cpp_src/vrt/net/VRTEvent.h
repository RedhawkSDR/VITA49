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

#ifndef _VRTEvent_h
#define _VRTEvent_h

#include <map>
#include <string>
#include "VRTObject.h"
#include "BasicVRTPacket.h"


namespace vrt {
  using namespace std;

  /** A simple holder for event data. This class is used to parallel the Java code and so we can
   *  add functionality in the future if required.
   *
   *  @author         
   */
  class VRTEvent : public VRTObject {
    private: VRTObject*      source;
    private: string          sourceStr;
    private: BasicVRTPacket  packet;

    /** Creates a new instance.
     *  @param source The event source.
     */
    public: VRTEvent (VRTObject &source);

    /** Creates a new instance.
     *  @param source The event source.
     */
    public: VRTEvent (VRTObject *source);
    
    /** Creates a new instance.
     *  @param source The event source.
     *  @param packet The VRT packet, if applicable.
     */
    public: VRTEvent (VRTObject &source, BasicVRTPacket &packet);

    /** Creates a new instance.
     *  @param source The event source.
     *  @param packet The VRT packet, if applicable.
     */
    public: VRTEvent (VRTObject *source, BasicVRTPacket &packet);

    /** Copy constructor. */
    public: VRTEvent (const VRTEvent &e);

    /** Destroys the current instance. */
    public: ~VRTEvent () {
      // done
    }

    /** Converts this class its string form. */
    public: virtual string toString () const;

    /** Gets the source object associated with this error, if avaliable.
     *  @return The packet or null if not available
     */
    public: inline VRTObject* getSource () const {
      return source;
    }

    /** Gets the packet associated with this error, if avaliable. Note that the packet may be
     *  invalid.
     *  @return The packet or null if not available
     */
    public: inline BasicVRTPacket getPacket () const {
      return packet;
    }
  };
};

#endif /* _VRTEvent_h */

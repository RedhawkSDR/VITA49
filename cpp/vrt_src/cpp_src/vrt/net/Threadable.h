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

#ifndef _Threadable_h
#define _Threadable_h

#include "VRTObject.h"
#include <pthread.h>
#include <iostream>

namespace vrt {
  using namespace std;

  /** Utility class that handles common issues regarding threadable objects.
   *
   *  @author         
   */
  class Threadable : public VRTObject {
    private: pthread_t thread;
    private: bool      started;
    private: bool      term;
    private: bool      done;

    /** Basic no-argument constructor. */
    public: Threadable ();

    /** Basic copy constructor. */
    public: Threadable (const Threadable &t);

    /** Basic copy constructor. */
    public: ~Threadable ();

    /** Blocks until the thread is done.
        @throws VRTException If already running.
     */
    public: void join ();

    /** Starts the object running in a new thread.
        @throws VRTException If already running.
     */
    public: void start ();

    /** Runs the processing in the current thread. In general, {@link #start} should be used
     *  to start processing.
     */
    public: void run ();
    
    /** Restarts the object in a new thread. If an object supports restarts it should define the
        following:
        <pre>
          public: void restart () { super_restart(); }
        </pre>
     */
    protected: inline void super_restart () {
      stop();
      started = false;
      start();
    }

    // This is incompatible with the old version of the C++ code for VRTReader...
    //
    // /** Stops the object running. This is identical to <tt>stop(true)</tt>. */
    // public: inline void close () { stop(true); }

    /** Tells the object to stop running.
        @param wait Wait for the thread to complete (true)?
     */
    public: void stop (bool wait=true);

    /** Is this thread done? */
    public: inline bool isDone () { return started && done; }

    /** Is this thread started? */
    public: inline bool isStarted () { return started; }

    /** Should this thread stop now? */
    protected: inline bool stopNow () { return term; }

    /** Runs the thread. */
    protected: virtual void runThread () = 0;

    /** Closes any connections, This will always be called at the end of a call to {@link #run()}. */
    protected: virtual void shutdown ();
  };
};

#endif /* _Threadable_h */

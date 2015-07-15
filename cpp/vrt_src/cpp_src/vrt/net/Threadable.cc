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

#include "Threadable.h"
#include <string>
#include <ostream>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;
using namespace vrt;

static void *_run (void *ptr) {
  Threadable *t = (Threadable*)ptr;
  t->run();
  return NULL;
}


Threadable::Threadable () :
  started(false),
  term(false),
  done(false)
{
  // done
}

Threadable::Threadable (const Threadable &t) :
  started(t.started),
  term(t.term),
  done(t.done)
{
  // done
}

Threadable::~Threadable () {
  // done
}

void Threadable::join () {
  if (!started) throw VRTException("Cannot join unstarted thread");
  if (done) return;

  if (!started || done)
    return;

  void *rval;
  int errnum = pthread_join(thread,&rval);

  if (errnum != 0) {
    throw VRTException(errnum);
  }
}

void Threadable::start () {
  if (started) throw VRTException("Already started");

  term    = false;
  done    = false;
  started = true;

  int errnum = pthread_create(&thread, NULL, _run, (void*)this);

  if (errnum != 0) {
    throw VRTException(errnum);
  }
}

void Threadable::run () {
  // RUN THREAD
  try {
    runThread();
  }
  catch (VRTException e) {
    cout << "ERROR: Exception in thread: " << e << endl;
  }
  catch (exception e) {
    cout << "ERROR: Exception in thread: " << e.what() << endl;
  }
  catch (...) {
    cout << "ERROR: Exception in thread: <unknown exception>" << endl;
  }

  // SHUTDOWN
  try {
    shutdown();
  }
  catch (VRTException e) {
    cout << "ERROR: Exception in thread shutdown: " << e << endl;
  }
  catch (exception e) {
    cout << "ERROR: Exception in thread shutdown: " << e.what() << endl;
  }
  catch (...) {
    cout << "ERROR: Exception in thread shutdown: <unknown exception>" << endl;
  }

  // DONE
  done = true;
}

void Threadable::stop (bool wait) {
  term = true;
  if (wait && !done) {
//    if (cpid == getpid()) {
//      throw VRTException("Can not call stop(true) from within the processing thread.");
//    }

    int errnum = pthread_join(thread, NULL);
    if (errnum != 0) {
      throw VRTException(errnum);
    }
  }
}

void Threadable::shutdown () {
  // done
}

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

#ifndef _ThreadSafeQueue_h
#define _ThreadSafeQueue_h

#include "VRTObject.h"
#include <queue>
#include <pthread.h>
#include <errno.h>
#include <iostream>
//#include <time.h>
#include <sys/time.h>

using namespace std;

namespace vrt {
  /** A basic implementation of a thread-safe queue.
   *  @author
   */
  template <typename T>
  class ThreadSafeQueue {
    private: queue<T*>       q;
    private: pthread_mutex_t lock;
    private: pthread_cond_t  cond;
    private: size_t          maxSize;
    private: timeval         tv;
    private: timespec        ts;
    private: bool            purgeWhenFull;

    /** Creates a new instance.
     *  @param maxSize       The max queue size.
     *  @param purgeWhenFull Purge queue when full?
     */
    public: ThreadSafeQueue (size_t maxSize, bool purgeWhenFull=false) :
      maxSize(maxSize),
      purgeWhenFull(purgeWhenFull)
    {
      pthread_mutex_init(&lock,NULL);
      pthread_cond_init(&cond,NULL);
    }

    /** destructor */
    public: ~ThreadSafeQueue () {
      pthread_mutex_destroy(&lock);
    }

    /** Lock the mutex.
     *  @return Results of the conditional lock.
     */
    private: inline int lockMutex () {
      return pthread_mutex_lock(&lock);
    }

    /** Unlock the mutex.
     *  @return Results of the conditional unlock.
     */
    private: inline int unlockMutex () {
      return pthread_mutex_unlock(&lock);
    }

    /** Notify any waiting threads.
     *  @return Results of the conditional signal.
     */
    private: inline int notify () {
      return pthread_cond_signal(&cond);
    }

    /** Waits to obtain the lock.
     *  @param timeout The timeout in ns.
     *  @return Results of the conditional wait.
     */
    private: inline int wait (uint64_t timeout) {
      gettimeofday(&tv, NULL);
      ts.tv_nsec = tv.tv_usec*1000 + timeout;
      ts.tv_sec  = tv.tv_sec + ts.tv_nsec / 1000000000;
      ts.tv_nsec %= 1000000000;
      return pthread_cond_timedwait(&cond, &lock, &ts);
    }

    /** Waits to obtain the lock.
     *  @return Results of the conditional wait.
     */
    private: inline int wait () {
      return pthread_cond_wait(&cond, &lock);
    }

    /** Pushes an object reference onto the queue. Blocks until free space is available on queue.
     *  @param obj The object reference to put onto the queue.
     */
    public: void push (T* obj) {
      lockMutex();
      if (q.size() >= maxSize) {
        if (purgeWhenFull) {
          cout << "WARNING: queue filled, dropping " << (maxSize/4) << " packets" << endl;
          for (size_t i = 0; i < maxSize/4; i++) {
            q.pop();
          }
        }
        else {
          wait();
        }
      }
      q.push(obj);
      notify();
      unlockMutex();
    }

    /** Gets the current queue size
     *  @return current queue size
     */
    public: size_t getSize () {
      return q.size();
    }

    /** Gets the current queue size
     *  @return current queue size
     */
    public: size_t getMaxSize () {
      return maxSize;
    }

    /** Pushes an object reference onto the queue. Blocks until free space is available on queue
     *  or timeout occurs, whichever happens first.
     *  @param obj     The object reference to put onto the queue.
     *  @param timeout The timeout in ???.
     *  @return true if item pushed onto queue, false if not.
     */
    public: bool push (T* obj, uint32_t timeout) {
      lockMutex();
      if (q.size() >= maxSize) {
        if (ETIMEDOUT == wait(timeout)) {
          unlockMutex();
          return false;
        }
      }
      q.push(obj);
      notify();
      unlockMutex();
      return true;
    }

    /** Pops an object reference off of the queue. */
    public: T* pop () {
      lockMutex();
      T* front = NULL;
      if (q.size()==0) {
        wait();
      }
      front = q.front();
      q.pop();
      notify();
      unlockMutex();
      return front;
    }

    /** Pops an object reference off of the queue.
     *  @param timeout The timeout in ???.
     *  @return The object from the queue or NULL on timeout.
     */
    public: T* pop (uint64_t timeout) {
      lockMutex();
      T* front = NULL;
      if (q.size()==0) {
        int blah = wait(timeout);
        if (ETIMEDOUT == blah) {
          unlockMutex();
          return NULL;
        }
      }
      front = q.front();
      q.pop();
      notify();
      unlockMutex();
      return front;
    }
  };
}

#endif /* _ThreadSafeQueue_h */

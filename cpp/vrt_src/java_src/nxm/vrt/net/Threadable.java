/**
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

package nxm.vrt.net;

import java.lang.Thread.UncaughtExceptionHandler;
import java.io.Closeable;
import java.util.concurrent.ThreadFactory;

import static nxm.vrt.lib.Utilities.newNamedThreadFactory;

/** Utility class that handles common issues regarding threadable objects.
 *
 *  @author 
 */
public abstract class Threadable implements Closeable, Runnable {
  private final    ThreadFactory            threadFactory;
  private volatile Thread                   thread  = null;
  private volatile UncaughtExceptionHandler handler = null;
  private volatile boolean                  term    = false; // formerly called 'stop' changed to match C++

  public Threadable () {
    String name = getClass().getSimpleName();
    if (name.length() == 0) name = "Threadable";

    this.threadFactory = newNamedThreadFactory(name);
  }

  /** Starts the object running in a new thread.
   *  @throws RuntimeException If already running.
   */
  public void start () {
    if (thread != null) throw new RuntimeException("Already started");
    term   = false;
    thread = threadFactory.newThread(this);
    if (handler != null) {
      thread.setUncaughtExceptionHandler(handler);
    }
    thread.start();
  }

  /** Restarts the object in a new thread. If an object supports restarts it should define the
   *  following:
   *  <pre>
   *    public void restart () { super.restart(); }
   *  </pre>
   */
  protected void restart () {
    stop();
    thread = null;
    start();
  }
  
  @Override
  public final void run () {
    try {
      runThread();
    }
    catch (Throwable t) {
      if (t instanceof RuntimeException) throw (RuntimeException)t;
      throw new RuntimeException("Exception in thread", t);
    }
    finally {
      shutdown();
    }
  }

  /** Stops the object running. This is identical to <tt>stop(true)</tt>. */
  @Override
  public final void close () { stop(true); }

  /** Stops the object running. This is identical to <tt>stop(true)</tt>. */
  public final void stop () { stop(true); }

  /** Tells the object to stop running. <br>
   *  <br>
   *  <i>Previous versions of this class allowed {@link #close()} and {@link #stop()} to
   *  be overridden. This unintentionally created a situation where users were calling
   *  those functions in cases where they should have called <tt>stop(false)</tt> and
   *  were making their code more deadlock-prone in some situations where the stop was
   *  instigated within the active thread. The solution was to add the {@link #stop(boolean)}
   *  function and require all code go through there.</i>
   *  @param wait Wait for the thread to complete (true)?
   */
  public void stop (boolean wait) {
    term = true;
    if (wait && (thread != null)) {
      if (thread == Thread.currentThread()) {
        throw new RuntimeException("Can not call stop(true) from within the processing thread.");
      }

      try {
        thread.interrupt();
        thread.join();
      }
      catch (InterruptedException e) {
        throw new RuntimeException("Thread stopped on interrupt", e);
      }
    }
  }

  /** Sets the {@link UncaughtExceptionHandler} to use. This will simply call the
   *  {@link VRTEventListener#errorOccurred} method when an exception is caught.
   *  @param h The exception handler to use.
   */
  public final void setUncaughtExceptionHandler (final VRTEventListener h) {
    setUncaughtExceptionHandler(new UncaughtExceptionHandler() {
      @Override
      public void uncaughtException(Thread t, Throwable e) {
        h.errorOccurred(new VRTEvent(t), "Exception in thread "+t+": "+e, e);
      }
    });
  }

  /** Sets the {@link UncaughtExceptionHandler} to use.
   *  @param h The exception handler to use.
   */
  public final void setUncaughtExceptionHandler (UncaughtExceptionHandler h) {
    handler = h;
  }
  
  /** Is this thread done? */
  public boolean isDone () { return (thread == null) || (!thread.isAlive()); }

  /** Is this thread started? */
  public final boolean isStarted () { return (thread != null); }
  
  /** Should this thread stop now? */
  protected final boolean stopNow () { return term || (thread == null) || thread.isInterrupted(); }

  /** Runs the thread. */
  protected abstract void runThread () throws Exception;

  /** Closes any connections, This will always be called at the end of a call to {@link #run()}. */
  protected void shutdown () { }  
}

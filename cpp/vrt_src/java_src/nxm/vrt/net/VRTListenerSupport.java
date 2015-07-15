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

import nxm.vrt.lib.*;
import java.util.List;
import java.util.LinkedList;
import java.util.Map;

/**  Provides listener support for {@link VRTEventListener} classes.
 *
 *  @author         
 */
public final class VRTListenerSupport {
  private final List<ListenerDef> recvPktListeners = new LinkedList<ListenerDef>();
  private final List<ListenerDef> sentPktListeners = new LinkedList<ListenerDef>();
  private final List<ListenerDef> errorListeners   = new LinkedList<ListenerDef>();
  private final Object            source;
  private final VRTEvent          event;
  private       Class<?>          mode              = null; // = {null, VRTEventListener, VRTContextListener}

  
  public VRTListenerSupport (Object source) {
    this.source = source;
    this.event  = new VRTEvent(source);
  }
  
  /** Resets the listener to allow a re-send of the initial context. */
  public void reset () {
    mode = null;
  }
  
  public void shutdown () {
    removeAllListeners(recvPktListeners);
    removeAllListeners(sentPktListeners);
    removeAllListeners(errorListeners);
  }
  
  private synchronized void addListener (final List<ListenerDef> list, final VRTEventListener l, final Long classID) {
    if (l == null) throw new NullPointerException("listener is null");
    ListenerDef def = new ListenerDef(l, classID);
    if (!list.contains(def)) list.add(def);
  }
  
  private synchronized void removeListener (final List<ListenerDef> list, final VRTEventListener l, final Long classID) {
    if (l == null) throw new NullPointerException("listener is null");
    ListenerDef def = new ListenerDef(l, classID);
    list.remove(def);
  }
  
  private synchronized void removeAllListeners (final List<ListenerDef> list) {
    list.clear();
  }
  
  private synchronized List<VRTEventListener> getListeners (final List<ListenerDef> list, final Long classID, final boolean all) {
    List<VRTEventListener> out = new LinkedList<VRTEventListener>();
    
    for (ListenerDef def : list) {
      if (all) {
        out.add(def.listener);
      }
      else if (classID == null) {
        if (def.classID == null) out.add(def.listener);
      }
      else if (classID.equals(def.classID)) {
        out.add(def.listener);
      }
    }
    return out;
  }
  
  /**  Fires a received packet message. */
  public synchronized void fireReceivedPacket (VRTPacket p) {
    if ((mode != VRTEventListener.class) && (mode != null)) {
      throw new UnsupportedOperationException("Can not combine receivedPacket with use of "
                                             +"receivedDataPacket, receivedContextPacket, and/or "
                                             +"receivedInitialContext.");
    }
    Long classID = p.getClassIdentifier();
        
    for (ListenerDef def : recvPktListeners) {
      if ((def.classID == null) || ((classID != null) && classID.equals(def.classID))) {
        def.listener.receivedPacket(event, p);
      }
    }
    mode = VRTEventListener.class;
  }
  
  /** Fires a received data packet message. */
  public synchronized void fireReceivedDataPacket (DataPacket p) {
    if (mode != VRTContextListener.class) {
      throw new UnsupportedOperationException("Can not combine receivedDataPacket with use of "
                                             +"receivedPacket and can not call receivedDataPacket "
                                             +"before calling receivedInitialContext");
    }
    Long classID = p.getClassIdentifier();
    
    for (ListenerDef def : recvPktListeners) {
      if ((def.classID == null) || ((classID != null) && classID.equals(def.classID))) {
        if (def.listener instanceof VRTContextListener) {
          ((VRTContextListener)def.listener).receivedDataPacket(event, p);
        }
        else {
          def.listener.receivedPacket(event, p);
        }
      }
    }
  }

  /** Fires a received context packet message. */
  public synchronized void fireReceivedContextPacket (VRTPacket p) {
    if (mode != VRTContextListener.class) {
      throw new UnsupportedOperationException("Can not combine receivedContextPacket with use of "
                                             +"receivedPacket and can not call receivedContextPacket "
                                             +"before calling receivedInitialContext");
    }
    Long classID = p.getClassIdentifier();

    for (ListenerDef def : recvPktListeners) {
      if ((def.classID == null) || ((classID != null) && classID.equals(def.classID))) {
        if (def.listener instanceof VRTContextListener) {
          ((VRTContextListener)def.listener).receivedContextPacket(event, p);
        }
        else {
        def.listener.receivedPacket(event, p);
      }
    }
  }
  }

  /** Fires a received initial context message. */
  public synchronized void fireReceivedInitialContext (String errorMsg, DataPacket data,
                                                ContextPacket ctx, Map<Integer,VRTPacket> context) {
    if (mode == VRTContextListener.class) {
      throw new UnsupportedOperationException("Illegal duplicate call to receivedInitialContext.");
    }
    if (mode != null) {
      throw new UnsupportedOperationException("Can not combine receivedInitialContext with use of "
                                             +"receivedPacket.");
    }
    Long classID = (ctx  != null)? ctx.getClassIdentifier()
                 : (data != null)? data.getClassIdentifier() : null;
    
    for (ListenerDef def : recvPktListeners) {
      if ((def.classID == null) || ((classID != null) && classID.equals(def.classID))) {
        if (def.listener instanceof VRTContextListener) {
          ((VRTContextListener)def.listener).receivedInitialContext(event, errorMsg, data, ctx, context);
        }
        else {
          if ((errorMsg != null) && !errorMsg.equals(VRTContextListener.NO_CONTEXT_STREAM)
                                 && !errorMsg.equals(VRTContextListener.NO_DATA_STREAM)) {
            def.listener.errorOccurred(event, errorMsg, null);
          }
          for (VRTPacket p : context.values()) {
            def.listener.receivedPacket(event, p);
          }
        }
      }
    }
    mode = VRTContextListener.class;
  }
  
  /**  Fires a sent packet message, does not wait for it to be processed. */
  public synchronized void fireSentPacket (VRTPacket p) {
    Long classID = p.getClassIdentifier();
    
    for (ListenerDef def : sentPktListeners) {
      if ((def.classID == null) || classID.equals(def.classID)) {
        def.listener.sentPacket(new VRTEvent(source, p), p);
      }
    }
  }

  /**  Fires a received packet message, does not wait for it to be processed. */
  public final void fireErrorOccurred (String msg, Throwable t) {
    fireErrorOccurred(event, msg, t);
  }
  /**  Fires a received packet message, does not wait for it to be processed. */
  public final void fireErrorOccurred (String msg, Throwable t, VRTPacket p) {
    fireErrorOccurred(new VRTEvent(source, p), msg, t);
  }
  /**  Fires a received packet message, does not wait for it to be processed. */
  public synchronized void fireErrorOccurred (VRTEvent src, String msg, Throwable t) {
    for (ListenerDef def : errorListeners) {
      def.listener.errorOccurred(src, msg, t);
    }
  }

  /**  Fires a received packet message, does not wait for it to be processed. */
  public final void fireWarningOccurred (String msg, Throwable t) {
    fireWarningOccurred(event, msg, t);
  }
  /**  Fires a received packet message, does not wait for it to be processed. */
  public final void fireWarningOccurred (String msg, Throwable t, VRTPacket p) {
    fireWarningOccurred(new VRTEvent(source, p), msg, t);
  }
  /**  Fires a received packet message, does not wait for it to be processed. */
  public synchronized void fireWarningOccurred (VRTEvent src, String msg, Throwable t) {
    for (ListenerDef def : errorListeners) {
      def.listener.warningOccurred(src, msg, t);
    }
  }

  /**  Adds a listener to listen to the given messages. If <tt>classID</tt> is non-null, only the
   *  packets matching the given class ID will be passed on to the listener. If <tt>classID</tt> is
   *  null, all packets (including those without a class ID) will be passed on to the listener.
   *  @param l       The listener.
   *  @param classID The class ID of the messages to listen to (null=listen to all).
   *  @throws NullPointerException if the listener is null.
   */
  public void addReceivedPacketListener (VRTEventListener l, Long classID) {
    addListener(recvPktListeners, l, classID);
  }
  /**  Removes a given listener's binding.
   *  @param l       The listener.
   *  @param classID The class ID of the messages to listen to (null=listen to all).
   *  @throws NullPointerException if the listener is null.
   */
  public void removeReceivedPacketListener (VRTEventListener l, Long classID) {
    removeListener(recvPktListeners, l, classID);
  }
  /**  Removes all listeners. */
  public void removeAllReceivedPacketListeners () {
    removeAllListeners(recvPktListeners);
  }
  /**  Gets all active listeners matching the given class ID.
   *  @param classID The class ID of the messages to listen to (null=listen to all).
   */
  public List<VRTEventListener> getReceivedPacketListeners (Long classID) {
    return getListeners(recvPktListeners, classID, false);
  }
  /**  Gets all active listeners. */
  public List<VRTEventListener> getAllReceivedPacketListeners () {
    return getListeners(recvPktListeners, null, true);
  }

  @Deprecated
  public void addReveivedPacketListener (VRTEventListener l, Long classID) {
    addReceivedPacketListener(l, classID);
  }
  @Deprecated
  public void removeReveivedPacketListener (VRTEventListener l, Long classID) {
    removeReceivedPacketListener(l, classID);
  }
  @Deprecated
  public void removeAllReveivedPacketListeners () {
    removeAllReceivedPacketListeners();
  }
  @Deprecated
  public List<VRTEventListener> getReveivedPacketListeners (Long classID) {
    return getReceivedPacketListeners(classID);
  }
  @Deprecated
  public List<VRTEventListener> getAllReveivedPacketListeners () {
    return getAllReceivedPacketListeners();
  }
  
  /**  Adds a listener to listen to the given messages. If <tt>classID</tt> is non-null, only the
   *  packets matching the given class ID will be passed on to the listener. If <tt>classID</tt> is
   *  null, all packets (including those without a class ID) will be passed on to the listener.
   *  @param l       The listener.
   *  @param classID The class ID of the messages to listen to (null=listen to all).
   *  @throws NullPointerException if the listener is null.
   */
  public void addSentPacketListener (VRTEventListener l, Long classID) {
    addListener(sentPktListeners, l, classID);
  }
  /**  Removes a given listener's binding.
   *  @param l       The listener.
   *  @param classID The class ID of the messages to listen to (null=listen to all).
   *  @throws NullPointerException if the listener is null.
   */
  public void removeSentPacketListener (VRTEventListener l, Long classID) {
    removeListener(sentPktListeners, l, classID);
  }
  /**  Removes all listeners. */
  public void removeAllSentPacketListeners () {
    removeAllListeners(sentPktListeners);
  }
  /**  Gets all active listeners matching the given class ID.
   *  @param classID The class ID of the messages to listen to (null=listen to all).
   */
  public List<VRTEventListener> getSentPacketListeners (Long classID) {
    return getListeners(sentPktListeners, classID, false);
  }
  /**  Gets all active listeners. */
  public List<VRTEventListener> getAllSentPacketListeners () {
    return getListeners(sentPktListeners, null, true);
  }
  
  
  /**  Adds a listener to listen to the given messages. If <tt>classID</tt> is non-null, only the
   *  packets matching the given class ID will be passed on to the listener. If <tt>classID</tt> is
   *  null, all packets (including those without a class ID) will be passed on to the listener.
   *  @param l The listener.
   *  @throws NullPointerException if the listener is null.
   */
  public void addErrorWarningListener (VRTEventListener l) {
    addListener(errorListeners, l, null);
  }
  /**  Removes all listeners. */
  public void removeAllErrorWarningListeners () {
    removeAllListeners(errorListeners);
  }
  /**  Removes a given listener's binding.
   *  @param l The listener.
   *  @throws NullPointerException if the listener is null.
   */
  public void removeErrorWarningListener (VRTEventListener l) {
    removeListener(errorListeners, l, null);
  }
  /**  Gets all active listeners. */
  public List<VRTEventListener> getAllErrorWarningListeners () {
    return getListeners(errorListeners, null, true);
  }
  
  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Nested Classes
  //////////////////////////////////////////////////////////////////////////////////////////////////
  /** A listener definition. */
  private static class ListenerDef {
    final VRTEventListener listener;
    final Long             classID;
    
    public ListenerDef (VRTEventListener listener, Long classID) {
      this.listener = listener;
      this.classID  = classID;
    }
    
    @Override
    public boolean equals (Object o) {
      if (!(o instanceof ListenerDef)) return false;
      
      ListenerDef ld = (ListenerDef)o;
      if (listener != ld.listener) return false;
      if (classID == null) return (ld.classID == null);
      
      return classID.equals(ld.classID);
    }
    
    @Override
    public int hashCode () {
      if (classID == null) return listener.hashCode();
      return (int)(listener.hashCode() ^ (classID >>> 32) ^ (classID & 0xFFFFFFFF));
    }
  }
}

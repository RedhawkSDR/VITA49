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

package nxm.vrt.lib;

/** Specifies the functionality associated with the trailer in a {@link DataPacket},
 *  an {@link ExtDataPacket} or in the state-event fields in a {@link ContextPacket}.
 *
 *  @author 
 */
interface HasStateEventBits extends VRTPacket {
  /** Gets the calibrated time indicator flag.
   *  <pre>
   *    true  = The time stamp in this packet is calibrated to an external reference (e.g. atomic clock)
   *    false = The time stamp in this packet is *not* calibrated to an external reference
   *            and may be inaccurate
   *    null  = <i>This information is not available</i>
   *  </pre>
   *  @return true if the flag is set, false if it is not set, null if this optional flag isn't
   *          specified (null is returned for all context packets).
   */
  Boolean isCalibratedTimeStamp ();
  
  /** Gets the valid data indicator flag.
   *  <pre>
   *    true  = The data in the packet is valid
   *    false = A condition exists that *may* invalidate some or all of the data in the packet
   *    null  = <i>This information is not available</i>
   *  </pre>
   *  <i>Note that the definition of "valid" and "invalid" data is application specific, so it is
   *  important to consult the documentation for the relevant packet class before using this field.</i>
   *  @return true if the flag is set, false if it is not set, null if this optional flag isn't
   *          specified (null is returned for all context packets).
   */
  Boolean isDataValid ();
  
  /** Gets the reference lock indicator flag.
   *  <pre>
   *    true  = All of the phase-locked loops affecting the data are locked and stable
   *    false = At least one of the phase-locked loops affecting the data is not locked and stable
   *    null  = <i>This information is not available</i>
   *  </pre>
   *  @return true if the flag is set, false if it is not set, null if this optional flag isn't
   *          specified (null is returned for all context packets).
   */
  Boolean isReferenceLocked ();
  
  /** Gets the AGC/MGC indicator flag.
   *  <pre>
   *    true  = AGC (automatic gain control) is being used
   *    false = MGC (manual gain control) is being used
   *    null  = <i>This information is not available</i>
   *  </pre>
   *  @return true if the flag is set, false if it is not set, null if this optional flag isn't
   *          specified (null is returned for all context packets).
   */
  Boolean isAutomaticGainControl ();
  
  /** Gets the signal detected indicator flag.
   *  <pre>
   *    true  = Some signal detected in this packet
   *    false = No signal detected in this packet
   *    null  = <i>This information is not available</i>
   *  </pre>
   *  <i>Note that the definition of "valid" and "invalid" data is application specific, so it is
   *  important to consult the documentation for the relevant packet class before using this field.</i>
   *  @return true if the flag is set, false if it is not set, null if this optional flag isn't
   *          specified (null is returned for all context packets).
   */
  Boolean isSignalDetected ();
  
  /** Gets the spectral inversion indicator flag.
   *  <pre>
   *    true  = Signal conveyed in the payload has an inverted spectrum w.r.t. an up-stream
   *            reference point
   *    false = Spectrum is not inverted
   *    null  = <i>This information is not available</i>
   *  </pre>
   *  @return true if the flag is set, false if it is not set, null if this optional flag isn't
   *          specified (null is returned for all context packets).
   */
  Boolean isInvertedSpectrum ();
  
  /** Gets the over-range indicator flag.
   *  <pre>
   *    true  = The data in the payload is invalid since one or more data samples exceeded the
   *            range of the data item
   *    false = No samples over range
   *    null  = <i>This information is not available</i>
   *  </pre>
   *  @return true if the flag is set, false if it is not set, null if this optional flag isn't
   *          specified (null is returned for all context packets).
   */
  Boolean isOverRange ();
  
  /** Gets the sample loss indicator flag.
   *  <pre>
   *    true  = One or more sample discontinuities present in the data due to processing errors
   *            and/or data overflow
   *    false = No discontinuities present
   *    null  = <i>This information is not available</i>
   *  </pre>
   *  @return true if the flag is set, false if it is not set, null if this optional flag isn't
   *          specified (null is returned for all context packets).
   */
  Boolean isDiscontinuous ();
  
  /** <b>Deprecated:</b> Please change to using <tt>isDiscontinuous</tt>. */
  @Deprecated
  Boolean isDiscontinuious ();
  
  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Set
  //////////////////////////////////////////////////////////////////////////////////////////////////
  
  /** <i>Optional functionality:</i> Sets the calibrated time indicator flag.
   *  @param v true if the flag is set, false if not set, null if this optional flag isn't specified.
   *  @throws UnsupportedOperationException If this method is not supported or if called on a
   *                                        context packet.
   */
  void setCalibratedTimeStamp (Boolean v);
  
  /** <i>Optional functionality:</i> Gets the valid data indicator flag.
   *  @param v true if the flag is set, false if not set, null if this optional flag isn't specified.
   *  @throws UnsupportedOperationException If this method is not supported or if called on a
   *                                        context packet.
   */
  void setDataValid (Boolean v);
  
  /** <i>Optional functionality:</i> Gets the reference lock indicator flag.
   *  @param v true if the flag is set, false if not set, null if this optional flag isn't specified.
   *  @throws UnsupportedOperationException If this method is not supported or if called on a
   *                                        context packet.
   */
  void setReferenceLocked (Boolean v);
  
  /** <i>Optional functionality:</i> Gets the AGC/MGC indicator flag.
   *  @param v true if the flag is set, false if not set, null if this optional flag isn't specified.
   *  @throws UnsupportedOperationException If this method is not supported or if called on a
   *                                        context packet.
   */
  void setAutomaticGainControl (Boolean v);
  
  /** <i>Optional functionality:</i> Gets the signal detected indicator flag.
   *  @param v true if the flag is set, false if not set, null if this optional flag isn't specified.
   *  @throws UnsupportedOperationException If this method is not supported or if called on a
   *                                        context packet.
   */
  void setSignalDetected (Boolean v);
  
  /** <i>Optional functionality:</i> Gets the spectral inversion indicator flag.
   *  @param v true if the flag is set, false if not set, null if this optional flag isn't specified.
   *  @throws UnsupportedOperationException If this method is not supported or if called on a
   *                                        context packet.
   */
  void setInvertedSpectrum (Boolean v);
  
  /** <i>Optional functionality:</i> Gets the over-range indicator flag.
   *  @param v true if the flag is set, false if not set, null if this optional flag isn't specified.
   *  @throws UnsupportedOperationException If this method is not supported or if called on a
   *                                        context packet.
   */
  void setOverRange (Boolean v);
  
  /** <i>Optional functionality:</i> Gets the sample loss indicator flag.
   *  @param v true if the flag is set, false if not set, null if this optional flag isn't specified.
   *  @throws UnsupportedOperationException If this method is not supported or if called on a
   *                                        context packet.
   */
  void setDiscontinuous (Boolean v);
  
  /** <b>Deprecated:</b> Please change to using <tt>setDiscontinuous</tt>. */
  @Deprecated
  void setDiscontinuious (Boolean v);
}

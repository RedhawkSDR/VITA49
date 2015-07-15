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

package nxm.vita.test.junit.lib;

import nxm.sys.inc.*;
import nxm.sys.lib.*;
import nxm.sys.test.junit.NMTestCase;
import nxm.vrt.lib.ContextPacket;
import nxm.vrt.lib.ContextPacket.GeoSentences;
import nxm.vrt.lib.DataPacket;
import nxm.vrt.lib.VRTPacket;
import nxm.vrt.lib.VRTPacket.PacketType;
import nxm.vrt.lib.TimeStamp.FractionalMode;
import nxm.vrt.lib.TimeStamp.IntegerMode;
import nxm.vrt.lib.BasicContextPacket;

import static nxm.sys.inc.DataTypes.IEEE;

import static java.lang.Boolean.TRUE;
import static java.lang.Boolean.FALSE;

/** This will test the BasicVRTPacket class.
 *
 *  @author         
 */
public final class BasicContextPacketTest extends NMTestCase {

  /** public constructor to allow running this test case directly with JUnit. */
  public BasicContextPacketTest (String methodName) { this(methodName, null); }

  /** This constructor is required by NeXtMidas Unit Testing framework.
      @param methodName The name of the method being called.
      @param midas      The midas context to use when running the tests.
   */
  public BasicContextPacketTest(String methodName, Midas midas) {
    super(methodName, midas);
  }
  
                                                                 //          Type | C - T - | TSI TSF | Count | Size 
  private static final int[] PACKET0 = { 0x20000060,             // HEADER:  0010 | 0 0 0 0 | 00  00  | 0x0   | 0x0063
                                         0xFFFFFF00,             // BITS
                                         0x12345678,             // Ref Point
                                         0x00000000, 0x00100000, // Bandwidth (1 Hz)
                                         0x00000000, 0x00200000, // IF Freq (2 Hz)
                                         0x00000000, 0x00300000, // RF Freq (3 Hz)
                                         0x00000000, 0x00400000, // RF Freq Offset (4 Hz)
                                         0x00000000, 0x00500000, // Band Freq Offset (5 Hz)
                                         0x00000080,             // Ref Level (1dBm)
                                         0x0180FF80,             // Gain (3 dB, -1 dB)
                                         0x12345678,             // Over Range
                                         0x00000000, 0x00600000, // Sample Rate (6.0 Hz)
                                         0x01234567, 0x89abcdef, // Time Adjust
                                         0x02468ace,             // Time Calib
                                         0x000001c0,             // Temperature (7.0 C)
                                         0x00112233, 0x0000ABCD, // Device Identifier (11-22-33:ABCD)
                                         // State/Event
                                                                 // CVRA | DSOS | 0000 | CVRA | DSOS | 0000 | --USER--
                                         0xFA0AC0AB,             // 1111 | 1010 | 0000 | 1010 | 1100 | 0000 | 0xAB
                                         // Data Format
                                                                 // P RC F|ORMA | R ETS | CTSZ | 0000 | PACK|SZ IT|EMSZ | RepeatCount VectorSize
                                         0x8D5F056A, 0x1234ABCD, // 1 00 0|1110 | 0 101 | 1111 | 0000 | 0101|01 10|1010 | 0x1234      0xABCD
                                         // GPS
                                         0x00123456,             // TSI=00 TSF=00 Manufacturer=12-34-56
                                         0x00000000,             // WSEC
                                         0x00000000, 0x00000000, // FSEC
                                         0x00000000,             // LAT
                                         0x00000000,             // LON
                                         0x00000000,             // ALT
                                         0x00000000,             // SPEED
                                         0x00000000,             // HEADING
                                         0x00000000,             // TRACK
                                         0x00000000,             // MAG VAR
                                         // INS
                                         0x00112233,             // TSI=00 TSF=00 Manufacturer=11-22-33
                                         0x00000000,             // WSEC
                                         0x00000000, 0x00000000, // FSEC
                                         0x00000000,             // LAT
                                         0x00000000,             // LON
                                         0x00000000,             // ALT
                                         0x00000000,             // SPEED
                                         0x00000000,             // HEADING
                                         0x00000000,             // TRACK
                                         0x00000000,             // MAG VAR
                                         // ECEF
                                         0x007890AB,             // TSI=00 TSF=00 Manufacturer=78-90-AB
                                         0x00000000,             // WSEC
                                         0x00000000, 0x00000000, // FSEC
                                         0x00000000,             // X
                                         0x00000000,             // Y
                                         0x00000000,             // Z
                                         0x00000000,             // Alpha
                                         0x00000000,             // Beta
                                         0x00000000,             // Phi
                                         0x00000000,             // dX
                                         0x00000000,             // dY
                                         0x00000000,             // dZ
                                         // Relative
                                         0x00224466,             // TSI=00 TSF=00 Manufacturer=22-44-66
                                         0x00000000,             // WSEC
                                         0x00000000, 0x00000000, // FSEC
                                         0x00000000,             // X
                                         0x00000000,             // Y
                                         0x00000000,             // Z
                                         0x00000000,             // Alpha
                                         0x00000000,             // Beta
                                         0x00000000,             // Phi
                                         0x00000000,             // dX
                                         0x00000000,             // dY
                                         0x00000000,             // dZ
                                         // Reference
                                         0x00000000,
                                         // GPS ASCII
                                         0x00AABBCC,             // Manufacturer=AA-BB-CC
                                         0x0000000A,             // NumberOfWords=10
                                         0x24474c4c, 0x2c343931, // "$GLL,491"
                                         0x362e3435, 0x2c4e2c31, // "6.45,N,1"
                                         0x32333131, 0x2e31322c, // "2311.12,"
                                         0x572c3232, 0x35343434, // "W,225444"
                                         0x2c412c2a, 0x31440000, // ",A,*1D"
                                         // Context Assoc Lists
                                         0x00020002,             // SourceListSize=2 SystemListSize=2
                                         0x00018002,             // VectorListSize=1 A=1 AsynchListSize=2
                                         0x11000001, 0x11000002, // SourceList
                                         0x22000001, 0x22000002, // SystemList
                                         0x33000001,             // VectorList
                                         0x44000001, 0x44000002, // AsynchList
                                         0x55000001, 0x55000002, // AsynchTags
                                       };
  
  private <T extends Object> void check (int i, T exp, T act) {
    assertEquals("packets["+i+"]", exp, act);
  }
  private <T extends Object> void check (T exp, T act) {
    assertEquals(exp, act);
  }
  
  private static ContextPacket toPacket (int[] buf) {
    byte[] bbuf = new byte[buf.length * 4];
    
    for (int i = 0; i < buf.length; i++) {
      Convert.packL(bbuf, i*4, buf[i], IEEE);
    }
//    System.out.println("-------------------------");
//    for (int i = 0; i < buf.length; i++) {
//      System.out.println("0x"+StringUtil.padLeft(Integer.toHexString(buf[i]), 8, '0') + "  ->  "
//                             +StringUtil.padLeft(Integer.toHexString(bbuf[i*4+0]&0xFF), 2, '0') + " "
//                             +StringUtil.padLeft(Integer.toHexString(bbuf[i*4+1]&0xFF), 2, '0') + " "
//                             +StringUtil.padLeft(Integer.toHexString(bbuf[i*4+2]&0xFF), 2, '0') + " "
//                             +StringUtil.padLeft(Integer.toHexString(bbuf[i*4+3]&0xFF), 2, '0') + " ");
//    }
    return new BasicContextPacket(bbuf, 0, bbuf.length, false, false);
  }
  
  public void testIsChangePacket () {
    ContextPacket p = toPacket(PACKET0);
    check(true, p.isChangePacket());
  }
  public void testSetChangePacket () {
    ContextPacket p = toPacket(PACKET0);
    p.setChangePacket(true ); check(true , p.isChangePacket());
    p.setChangePacket(false); check(false, p.isChangePacket());
    
    p = new BasicContextPacket();
    p.setChangePacket(true ); check(true , p.isChangePacket());
    p.setChangePacket(false); check(false, p.isChangePacket());
  }
  
  
  public void testGetReferencePointIdentifier () {
    ContextPacket p = toPacket(PACKET0);
    check(0x12345678, p.getReferencePointIdentifier());
  }
  public void testSetReferencePointIdentifier () {
    ContextPacket p = toPacket(PACKET0);
    p.setReferencePointIdentifier(0xABCDEF00); check(0xABCDEF00, p.getReferencePointIdentifier());
    p.setReferencePointIdentifier(0x11223344); check(0x11223344, p.getReferencePointIdentifier());
    
    p = new BasicContextPacket();
    p.setReferencePointIdentifier(0xABCDEF00); check(0xABCDEF00, p.getReferencePointIdentifier());
    p.setReferencePointIdentifier(   null   ); check(   null   , p.getReferencePointIdentifier());
    p.setReferencePointIdentifier(0x11223344); check(0x11223344, p.getReferencePointIdentifier());
  }
  
  
  public void testGetBandwidth () {
    ContextPacket p = toPacket(PACKET0);
    check(1.0, p.getBandwidth());
  }
  public void testSetBandwidth () {
    ContextPacket p = toPacket(PACKET0);
    p.setBandwidth(1.0); check(1.0, p.getBandwidth());
    p.setBandwidth(7.0); check(7.0, p.getBandwidth());
    
    p = new BasicContextPacket();
    p.setBandwidth(1.0d); check(1.0d, p.getBandwidth());
    p.setBandwidth(null); check(null, p.getBandwidth());
    p.setBandwidth(7.0d); check(7.0d, p.getBandwidth());
  }
  
  
  public void testGetFrequencyIF () {
    ContextPacket p = toPacket(PACKET0);
    check(2.0, p.getFrequencyIF());
  }
  public void testSetFrequencyIF () {
    ContextPacket p = toPacket(PACKET0);
    p.setFrequencyIF(1.0); check(1.0, p.getFrequencyIF());
    p.setFrequencyIF(7.0); check(7.0, p.getFrequencyIF());
    
    p = new BasicContextPacket();
    p.setFrequencyIF(1.0d); check(1.0d, p.getFrequencyIF());
    p.setFrequencyIF(null); check(null, p.getFrequencyIF());
    p.setFrequencyIF(7.0d); check(7.0d, p.getFrequencyIF());
  }
  
  
  public void testGetFrequencyRF () {
    ContextPacket p = toPacket(PACKET0);
    check(3.0, p.getFrequencyRF());
  }
  public void testSetFrequencyRF () {
    ContextPacket p = toPacket(PACKET0);
    p.setFrequencyRF(1.0); check(1.0, p.getFrequencyRF());
    p.setFrequencyRF(7.0); check(7.0, p.getFrequencyRF());
    
    p = new BasicContextPacket();
    p.setFrequencyRF(1.0d); check(1.0d, p.getFrequencyRF());
    p.setFrequencyRF(null); check(null, p.getFrequencyRF());
    p.setFrequencyRF(7.0d); check(7.0d, p.getFrequencyRF());
  }
  
  
  public void testGetFrequencyOffsetRF () {
    ContextPacket p = toPacket(PACKET0);
    check(4.0, p.getFrequencyOffsetRF());
  }
  public void testSetFrequencyOffsetRF () {
    ContextPacket p = toPacket(PACKET0);
    p.setFrequencyOffsetRF(1.0); check(1.0, p.getFrequencyOffsetRF());
    p.setFrequencyOffsetRF(7.0); check(7.0, p.getFrequencyOffsetRF());
    
    p = new BasicContextPacket();
    p.setFrequencyOffsetRF(1.0d); check(1.0d, p.getFrequencyOffsetRF());
    p.setFrequencyOffsetRF(null); check(null, p.getFrequencyOffsetRF());
    p.setFrequencyOffsetRF(7.0d); check(7.0d, p.getFrequencyOffsetRF());
  }
  
  
  public void testGetBandOffsetIF () {
    ContextPacket p = toPacket(PACKET0);
    check(5.0, p.getBandOffsetIF());
  }
  public void testSetBandOffsetIF () {
    ContextPacket p = toPacket(PACKET0);
    p.setBandOffsetIF(1.0); check(1.0, p.getBandOffsetIF());
    p.setBandOffsetIF(7.0); check(7.0, p.getBandOffsetIF());
    
    p = new BasicContextPacket();
    p.setBandOffsetIF(1.0d); check(1.0d, p.getBandOffsetIF());
    p.setBandOffsetIF(null); check(null, p.getBandOffsetIF());
    p.setBandOffsetIF(7.0d); check(7.0d, p.getBandOffsetIF());
  }
  
  
  public void testGetReferenceLevel () {
    ContextPacket p = toPacket(PACKET0);
    check(1.0f, p.getReferenceLevel());
  }
  public void testSetReferenceLevel () {
    ContextPacket p = toPacket(PACKET0);
    p.setReferenceLevel(1.0f); check(1.0f, p.getReferenceLevel());
    p.setReferenceLevel(7.0f); check(7.0f, p.getReferenceLevel());
    
    p = new BasicContextPacket();
    p.setReferenceLevel(1.0f); check(1.0f, p.getReferenceLevel());
    p.setReferenceLevel(null); check(null, p.getReferenceLevel());
    p.setReferenceLevel(7.0f); check(7.0f, p.getReferenceLevel());
  }
  
  
  public void testGetGain1 () { } // tested in testGetGain
  public void testSetGain1 () { } // tested in testSetGain
  public void testGetGain2 () { } // tested in testGetGain
  public void testSetGain2 () { } // tested in testSetGain
  
  public void testGetGain () {
    ContextPacket p = toPacket(PACKET0);
    check( 3.0f, p.getGain1());
    check(-1.0f, p.getGain2());
    check( 2.0f, p.getGain());
  }
  public void testSetGain () {
    ContextPacket p = toPacket(PACKET0);
    p.setGain(1.0f);
    check(1.0f, p.getGain1());
    check(0.0f, p.getGain2());
    check(1.0f, p.getGain());
    
    p.setGain(null);
    check(null, p.getGain1());
    check(null, p.getGain2());
    check(null, p.getGain());
    
    p.setGain(7.0f);
    check(7.0f, p.getGain1());
    check(0.0f, p.getGain2());
    check(7.0f, p.getGain());
    
    p = new BasicContextPacket();
    p.setGain(1.0f, 2.0f);
    check(1.0f, p.getGain1());
    check(2.0f, p.getGain2());
    check(3.0f, p.getGain());
    
    p.setGain(7.0f, -9.0f);
    check( 7.0f, p.getGain1());
    check(-9.0f, p.getGain2());
    check(-2.0f, p.getGain());
    
    p = new BasicContextPacket();
    p.setGain1(1.0f);
    p.setGain2(2.0f);
    check(1.0f, p.getGain1());
    check(2.0f, p.getGain2());
    check(3.0f, p.getGain());
    
    p = new BasicContextPacket();
    p.setGain2(2.0f);
    p.setGain1(1.0f);
    check(1.0f, p.getGain1());
    check(2.0f, p.getGain2());
    check(3.0f, p.getGain());
  }
  
  
  public void testGetOverRangeCount () {
    ContextPacket p = toPacket(PACKET0);
    check(0x12345678L, p.getOverRangeCount());
  }
  public void testSetOverRangeCount () {
    ContextPacket p = toPacket(PACKET0);
    p.setOverRangeCount(16L); check(16L, p.getOverRangeCount());
    p.setOverRangeCount(42L); check(42L, p.getOverRangeCount());
    
    p = new BasicContextPacket();
    p.setOverRangeCount( 7L ); check( 7L , p.getOverRangeCount());
    p.setOverRangeCount(null); check(null, p.getOverRangeCount());
    p.setOverRangeCount( 0L ); check( 0L , p.getOverRangeCount());
  }
  
  
  public void testGetSampleRate () {
    ContextPacket p = toPacket(PACKET0);
    check(6.0, p.getSampleRate());
  }
  public void testSetSampleRate () {
    ContextPacket p = toPacket(PACKET0);
    p.setSampleRate(42e6); check(42e6, p.getSampleRate());
    p.setSampleRate(10e9); check(10e9, p.getSampleRate());
    
    p = new BasicContextPacket();
    p.setSampleRate(42e6); check(42e6, p.getSampleRate());
    p.setSampleRate(10e9); check(10e9, p.getSampleRate());
    p.setSampleRate(null); check(null, p.getSampleRate());
  }
  
  
  public void testGetTimeStampAdjustment () {
    ContextPacket p = toPacket(PACKET0);
    check(0x0123456789abcdefL, p.getTimeStampAdjustment());
  }
  public void testSetTimeStampAdjustment () {
    ContextPacket p = toPacket(PACKET0);
    p.setTimeStampAdjustment(        0L); check(       0L,  p.getTimeStampAdjustment());
    p.setTimeStampAdjustment(123456789L); check(123456789L, p.getTimeStampAdjustment());
    
    p = new BasicContextPacket();
    p.setTimeStampAdjustment(        0L); check(       0L,  p.getTimeStampAdjustment());
    p.setTimeStampAdjustment(   null   ); check(   null   , p.getTimeStampAdjustment());
    p.setTimeStampAdjustment(123456789L); check(123456789L, p.getTimeStampAdjustment());
  }
  
  
  public void testGetTimeStampCalibration () {
    ContextPacket p = toPacket(PACKET0);
    check(0x02468ace, p.getTimeStampCalibration());
  }
  public void testSetTimeStampCalibration () {
    ContextPacket p = toPacket(PACKET0);
    p.setTimeStampCalibration(     0); check(     0, p.getTimeStampCalibration());
    p.setTimeStampCalibration(123456); check(123456, p.getTimeStampCalibration());
    
    p = new BasicContextPacket();
    p.setTimeStampCalibration(    52); check(    52, p.getTimeStampCalibration());
    p.setTimeStampCalibration( null ); check( null , p.getTimeStampCalibration());
    p.setTimeStampCalibration(123456); check(123456, p.getTimeStampCalibration());
  }
  
  
  public void testGetTemperature () {
    ContextPacket p = toPacket(PACKET0);
    check(7.0f, p.getTemperature());
  }
  public void testSetTemperature () {
    ContextPacket p = toPacket(PACKET0);
    p.setTemperature(1.0f); check(1.0f, p.getTemperature());
    p.setTemperature(7.0f); check(7.0f, p.getTemperature());
    
    p = new BasicContextPacket();
    p.setTemperature(1.0f); check(1.0f, p.getTemperature());
    p.setTemperature(7.0f); check(7.0f, p.getTemperature());
    p.setTemperature(null); check(null, p.getTemperature());
  }
  
  
  public void testGetDeviceID () { } // tested in testGetDeviceIdentifier
  public void testSetDeviceID () { } // tested in testSetDeviceIdentifier
  
  public void testGetDeviceIdentifier () {
    ContextPacket p = toPacket(PACKET0);
    check(0x001122330000ABCDL, p.getDeviceIdentifier());
    check("11-22-33:ABCD",     p.getDeviceID());
  }
  public void testSetDeviceIdentifier () {
    ContextPacket p = toPacket(PACKET0);
    p.setDeviceIdentifier(0x00ABCDEF00001234L);
    check(0x00ABCDEF00001234L, p.getDeviceIdentifier());
    check("AB-CD-EF:1234",     p.getDeviceID());
    
    p = new BasicContextPacket();
    p.setDeviceID("AB-CD-EF:1234");
    check(0x00ABCDEF00001234L, p.getDeviceIdentifier());
    check("AB-CD-EF:1234",     p.getDeviceID());
    
    p.setDeviceID(null);
    check(null, p.getDeviceIdentifier());
    check(null, p.getDeviceID());
  }
  
  
  public void testIsCalibratedTimeStamp () {
    ContextPacket p = toPacket(PACKET0);
    check(TRUE, p.isCalibratedTimeStamp());
  }
  public void testSetCalibratedTimeStamp () {
    ContextPacket p = toPacket(PACKET0);
    p.setCalibratedTimeStamp(null);  check(null,  p.isCalibratedTimeStamp());
    p.setCalibratedTimeStamp(TRUE);  check(TRUE,  p.isCalibratedTimeStamp());
    p.setCalibratedTimeStamp(FALSE); check(FALSE, p.isCalibratedTimeStamp());
  }
  
  
  public void testIsDataValid () {
    ContextPacket p = toPacket(PACKET0);
    check(FALSE, p.isDataValid());
  }
  public void testSetDataValid () {
    ContextPacket p = toPacket(PACKET0);
    p.setDataValid(null);  check(null,  p.isDataValid());
    p.setDataValid(TRUE);  check(TRUE,  p.isDataValid());
    p.setDataValid(FALSE); check(FALSE, p.isDataValid());
  }
  
  
  public void testIsReferenceLocked () {
    ContextPacket p = toPacket(PACKET0);
    check(TRUE, p.isReferenceLocked());
  }
  public void testSetReferenceLocked () {
    ContextPacket p = toPacket(PACKET0);
    p.setReferenceLocked(null);  check(null,  p.isReferenceLocked());
    p.setReferenceLocked(TRUE);  check(TRUE,  p.isReferenceLocked());
    p.setReferenceLocked(FALSE); check(FALSE, p.isReferenceLocked());
  }
  
  
  public void testIsAutomaticGainControl () {
    ContextPacket p = toPacket(PACKET0);
    check(FALSE, p.isAutomaticGainControl());
  }
  public void testSetAutomaticGainControl () {
    ContextPacket p = toPacket(PACKET0);
    p.setAutomaticGainControl(null);  check(null,  p.isAutomaticGainControl());
    p.setAutomaticGainControl(TRUE);  check(TRUE,  p.isAutomaticGainControl());
    p.setAutomaticGainControl(FALSE); check(FALSE, p.isAutomaticGainControl());
  }
  
  
  public void testIsSignalDetected () {
    ContextPacket p = toPacket(PACKET0);
    check(TRUE, p.isSignalDetected());
  }
  public void testSetSignalDetected () {
    ContextPacket p = toPacket(PACKET0);
    p.setSignalDetected(null);  check(null,  p.isSignalDetected());
    p.setSignalDetected(TRUE);  check(TRUE,  p.isSignalDetected());
    p.setSignalDetected(FALSE); check(FALSE, p.isSignalDetected());
  }
  
  
  public void testIsInvertedSpectrum () {
    ContextPacket p = toPacket(PACKET0);
    check(null, p.isInvertedSpectrum());
  }
  public void testSetInvertedSpectrum () {
    ContextPacket p = toPacket(PACKET0);
    p.setInvertedSpectrum(null);  check(null,  p.isInvertedSpectrum());
    p.setInvertedSpectrum(TRUE);  check(TRUE,  p.isInvertedSpectrum());
    p.setInvertedSpectrum(FALSE); check(FALSE, p.isInvertedSpectrum());
  }
  
  
  public void testIsOverRange () {
    ContextPacket p = toPacket(PACKET0);
    check(FALSE, p.isOverRange());
  }
  public void testSetOverRange () {
    ContextPacket p = toPacket(PACKET0);
    p.setOverRange(null);  check(null,  p.isOverRange());
    p.setOverRange(TRUE);  check(TRUE,  p.isOverRange());
    p.setOverRange(FALSE); check(FALSE, p.isOverRange());
  }
  
  
  public void testIsDiscontinuious () {
    ContextPacket p = toPacket(PACKET0);
    check(null, p.isDiscontinuious());
  }
  public void testSetDiscontinuious () {
    ContextPacket p = toPacket(PACKET0);
    p.setDiscontinuious(null);  check(null,  p.isDiscontinuious());
    p.setDiscontinuious(TRUE);  check(TRUE,  p.isDiscontinuious());
    p.setDiscontinuious(FALSE); check(FALSE, p.isDiscontinuious());
  }
  
  
  public void testGetUserDefinedBits () {
    ContextPacket p = toPacket(PACKET0);
    check((byte)0xAB, p.getUserDefinedBits());
  }
  public void testSetUserDefinedBits () {
    ContextPacket p = toPacket(PACKET0);
    p.setUserDefinedBits((byte)0x12); check((byte)0x12, p.getUserDefinedBits());
    p.setUserDefinedBits((byte)0x34); check((byte)0x34, p.getUserDefinedBits());
    
    p = new BasicContextPacket();
    p.setUserDefinedBits((byte)0x12); check((byte)0x12, p.getUserDefinedBits());
    p.setUserDefinedBits((byte)0x34); check((byte)0x34, p.getUserDefinedBits());
  }
  
  
  
  
  
  
  
  
  
  
  
  
  public void testGetGeoSentences () {
    ContextPacket p = toPacket(PACKET0);
    GeoSentences  g = p.getGeoSentences();
    
    // equals
    check(TRUE,  g.equals(p.getGeoSentences()));
    check(FALSE, g.equals(new GeoSentences()));

    // hashCode
    check(p.getGeoSentences().hashCode(), g.hashCode());
    
    // toString (ignored)
    
    // getManufacturerIdentifier & getManufacturerID
    check(0x00AABBCC,  g.getManufacturerIdentifier());
    check("AA-BB-CC",  g.getManufacturerID());
    
    
    // getSentences
    check("$GLL,4916.45,N,12311.12,W,225444,A,*1D", g.getSentences());
  }
  public void testSetGeoSentences () {
    ContextPacket p;
    GeoSentences  g;
    
    // setManufacturerIdentifier
    p = new BasicContextPacket();
    g = new GeoSentences();
    
    g.setManufacturerIdentifier(0x00123456);
    p.setGeoSentences(g);
    
    check(0x00123456, p.getGeoSentences().getManufacturerIdentifier());
    check("12-34-56", p.getGeoSentences().getManufacturerID());
    
    // setManufacturerID
    p = new BasicContextPacket();
    g = new GeoSentences();
    
    g.setManufacturerID("11-22-33");
    p.setGeoSentences(g);
    
    check(0x00112233, p.getGeoSentences().getManufacturerIdentifier());
    check("11-22-33", p.getGeoSentences().getManufacturerID());
    
    // setSentences
    p = new BasicContextPacket();
    g = new GeoSentences();
    
    g.setSentences("$GLL,4916.45,N,12311.12,W,225444,A,*1D");
    p.setGeoSentences(g);
    
    check("$GLL,4916.45,N,12311.12,W,225444,A,*1D", g.getSentences());
  }
  
  
}

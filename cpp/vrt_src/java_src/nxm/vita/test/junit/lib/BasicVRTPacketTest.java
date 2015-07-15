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

import nxm.sys.lib.*;
import nxm.sys.test.junit.NMTestCase;
import nxm.vrt.lib.DataPacket;
import nxm.vrt.lib.VRTConfig;
import nxm.vrt.lib.VRTPacket;
import nxm.vrt.lib.VRTPacket.PacketType;
import nxm.vrt.lib.TimeStamp;
import nxm.vrt.lib.TimeStamp.FractionalMode;
import nxm.vrt.lib.TimeStamp.IntegerMode;

import nxm.vrt.lib.BasicVRTPacket;

import static nxm.sys.inc.DataTypes.IEEE;

import static java.lang.Boolean.TRUE;
import static java.lang.Boolean.FALSE;

/** This will test the BasicVRTPacket class.
 *
 *  @author         
 */
public final class BasicVRTPacketTest extends NMTestCase {

  /** public constructor to allow running this test case directly with JUnit. */
  public BasicVRTPacketTest (String methodName) { this(methodName, null); }

  /** This constructor is required by NeXtMidas Unit Testing framework.
      @param methodName The name of the method being called.
      @param midas      The midas context to use when running the tests.
   */
  public BasicVRTPacketTest(String methodName, Midas midas) {
    super(methodName, midas);
  }
  
                                                     //          Type | C T - - | TSI TSF | Count | Size 
  private static final int[] PACKET0 = { 0x00000002, // HEADER:  0000 | 0 0 0 0 | 00  00  | 0x0   | 0x0002
                                         0x12345678, // DATA
                                       };
  
                                                     //          Type | C T - - | TSI TSF | Count | Size 
  private static final int[] PACKET1 = { 0x10010003, // HEADER:  0001 | 0 0 0 0 | 00  00  | 0x1   | 0x0003
                                         0x00000001, // STREAM:  0x01
                                         0x12345678, // DATA
                                       };
  
                                                     //          Type | C T - - | TSI TSF | Count | Size 
  private static final int[] PACKET2 = { 0x1C020006, // HEADER:  0001 | 1 1 0 0 | 00  00  | 0x2   | 0x0006
                                         0x00000001, // STREAM:  0x01
                                         0x00123456, // CLASS:   OUI=0x123456
                                         0xABCD1234, // CLASS:   ICC=0xABCD PCC=0x1234
                                         0x12345678, // DATA
                                         0xFACACBFF, // TRAILER: 1111 | 1010 | 1100 | 1010 | 1100 | 1011 | 1 0x7F
                                       };            //          CVRA | DSOS | USER | CVRA | DSOS | USER | E Assoc
  
                                                     //          Type | C T - - | TSI TSF | Count | Size 
  private static final int[] PACKET3 = { 0x1C63000A, // HEADER:  0001 | 1 1 0 0 | 01  10  | 0x3   | 0x000A
                                         0x12345678, // STREAM:  0x12345678
                                         0x00123456, // CLASS:   OUI=0x123456
                                         0xABCD1234, // CLASS:   ICC=0xABCD PCC=0x1234
                                         0x12345678, // TSI
                                         0x00000012, // TSF
                                         0x34567890, // TSF
                                         0x10203040, // DATA
                                         0x01020304, // DATA
                                         0xFACACBFF, // TRAILER: 1111 | 1010 | 1100 | 1010 | 1100 | 1011 | 1 0x7F
                                       };            //          CVRA | DSOS | USER | CVRA | DSOS | USER | E Assoc
  public 
  static VRTPacket[]           packets       = { toPacket(PACKET0)          , toPacket(PACKET1)       , toPacket(PACKET2)       , toPacket(PACKET3)            };
  static boolean[]             valid         = { true                       , true                    , true                    , true                         };
  static PacketType[]          packetTypes   = { PacketType.UnidentifiedData, PacketType.Data         , PacketType.Data         , PacketType.Data              };
  static Boolean[]             tsm           = { null                       , null                    , null                    , null                         };
  static IntegerMode[]         tsi           = { IntegerMode.None           , IntegerMode.None        , IntegerMode.None        , IntegerMode.UTC              };
  static FractionalMode[]      tsf           = { FractionalMode.None        , FractionalMode.None     , FractionalMode.None     , FractionalMode.RealTime      };
  static Long[]                wsec          = { null                       , null                    , null                    , 0x12345678L                  };
  static Long[]                fsec          = { null                       , null                    , null                    , 0x0000001234567890L          };
  static int[]                 count         = { 0x0                        , 0x1                     , 0x2                     , 0x3                          };
  static int[]                 packetLength  = { PACKET0.length*4           , PACKET1.length*4        , PACKET2.length*4        , PACKET3.length*4             };
  static int[]                 headerLength  = { 1*4                        , 2*4                     , 4*4                     , 7*4                          };
  static int[]                 trailerLength = { 0*4                        , 0*4                     , 1*4                     , 1*4                          };
  static String[]              streamID      = { null                       , "1"                     , "1"                     , Integer.toString(0x12345678) };
  static Integer[]             streamIdent   = { null                       , 0x01                    , 0x01                    , 0x12345678                   };
  static String[]              classID       = { null                       , null                    , "12-34-56:ABCD.1234"    , "12-34-56:ABCD.1234"         };
  static Long[]                classIdent    = { null                       , null                    , 0x00123456ABCD1234L     , 0x00123456ABCD1234L          };
  static Integer[]             classOUI      = { null                       , null                    , 0x00123456              , 0x00123456                   };
  static Short[]               classICC      = { null                       , null                    , (short)0xABCD           , (short)0xABCD                };
  static Short[]               classPCC      = { null                       , null                    , (short)0x1234           , (short)0x1234                };
  static Boolean[]             calTimeStamp  = { null                       , null                    , TRUE                    , TRUE                         }; // C
  static Boolean[]             dataValid     = { null                       , null                    , FALSE                   , FALSE                        }; // V
  static Boolean[]             refLock       = { null                       , null                    , TRUE                    , TRUE                         }; // R
  static Boolean[]             agc           = { null                       , null                    , FALSE                   , FALSE                        }; // A
  static Boolean[]             signalDet     = { null                       , null                    , TRUE                    , TRUE                         }; // D
  static Boolean[]             inverted      = { null                       , null                    , null                    , null                         }; // S
  static Boolean[]             overRange     = { null                       , null                    , FALSE                   , FALSE                        }; // O
  static Boolean[]             discontin     = { null                       , null                    , null                    , null                         }; // S
  static Boolean[]             bit11         = { null                       , null                    , TRUE                    , TRUE                         }; // U
  static Boolean[]             bit10         = { null                       , null                    , FALSE                   , FALSE                        }; // S
  static Boolean[]             bit9          = { null                       , null                    , null                    , null                         }; // E
  static Boolean[]             bit8          = { null                       , null                    , null                    , null                         }; // R
  static Byte[]                assocPacket   = { null                       , null                    , (byte)0x7F              , (byte)0x7F                   };

  private void check (int i, Object exp, Object act) {
    assertEquals("packets["+i+"]", exp, act);
  }
  private void check (Object exp, Object act) {
    assertEquals(exp, act);
  }
  
  private static VRTPacket toPacket (int[] buf) {
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
    return new BasicVRTPacket(bbuf, 0, bbuf.length, false, false);
  }

  private static DataPacket asDataPacket (VRTPacket p) {
    VRTPacket packet = VRTConfig.getPacket(p, false);
    return (packet instanceof DataPacket)? (DataPacket)packet : null;
  }

  public void testEquals () {
    for (int i = 0; i < packets.length; i++) {
      for (int j = 0; j < packets.length; j++) {
        check(i, (i == j), packets[i].equals(packets[j]));
      }
    }
  }
  
  public void testHashCode () {
    for (int i = 0; i < packets.length; i++) {
      for (int j = 0; j < packets.length; j++) {
        if (i == j) check(i, packets[i].hashCode(), packets[j].hashCode());
      }
    }
  }
  
  public void testIsPacketValid () {
    for (int i = 0; i < packets.length; i++) {
      check(i, valid[i], packets[i].isPacketValid());
    }
  }
  
  public void testGetPacketType () {
    for (int i = 0; i < packets.length; i++) {
      check(i, packetTypes[i], packets[i].getPacketType());
    }
  }
  
  public void testIsTimeStampMode () {
    for (int i = 0; i < packets.length; i++) {
      check(i, tsm[i], packets[i].isTimeStampMode());
    }
  }
  
  public void testGetTimeStamp () {
    for (int i = 0; i < packets.length; i++) {
      TimeStamp exp = new TimeStamp(tsi[i], tsf[i], wsec[i], fsec[i]);
      check(i, exp, packets[i].getTimeStamp());
    }
  }
  
  public void testGetPacketCount () {
    for (int i = 0; i < packets.length; i++) {
      check(i, count[i], packets[i].getPacketCount());
    }
  }
  
  public void testGetPacketLength () {
    for (int i = 0; i < packets.length; i++) {
      check(i, packetLength[i], packets[i].getPacketLength());
    }
  }
  
  public void testGetPayloadLength () {
    for (int i = 0; i < packets.length; i++) {
      check(i, packetLength[i]-headerLength[i]-trailerLength[i], packets[i].getPayloadLength());
    }
  }
  
  public void testGetHeaderLength () {
    for (int i = 0; i < packets.length; i++) {
      check(i, headerLength[i], packets[i].getHeaderLength());
    }
  }
  
  public void testGetTrailerLength () {
    for (int i = 0; i < packets.length; i++) {
      check(i, trailerLength[i], packets[i].getTrailerLength());
    }
  }
  
  public void testGetStreamID () {
    for (int i = 0; i < packets.length; i++) {
      check(i, streamID[i], packets[i].getStreamID());
    }
  }
  
  public void testGetStreamIdentifier () {
    for (int i = 0; i < packets.length; i++) {
      check(i, streamIdent[i], packets[i].getStreamIdentifier());
    }
  }
  
  public void testGetClassID () {
    for (int i = 0; i < packets.length; i++) {
      check(i, classID[i], packets[i].getClassID());
    }
  }
  
  public void testGetClassIdentifier () {
    for (int i = 0; i < packets.length; i++) {
      check(i, classIdent[i], packets[i].getClassIdentifier());
    }
  }
  
  public void testGetClassIdentifierOUI () {
    for (int i = 0; i < packets.length; i++) {
      check(i, classOUI[i], packets[i].getClassIdentifierOUI());
    }
  }
  
  public void testGetClassIdentifierICC  () {
    for (int i = 0; i < packets.length; i++) {
      check(i, classICC[i], packets[i].getClassIdentifierICC ());
    }
  }
  
  public void testGetClassIdentifierPCC () {
    for (int i = 0; i < packets.length; i++) {
      check(i, classPCC[i], packets[i].getClassIdentifierPCC());
    }
  }
  
  public void testIsCalibratedTimeStamp () {
    for (int i = 0; i < packets.length; i++) {
      DataPacket pkt = asDataPacket(packets[i]);
      if (pkt != null) {
        check(i, calTimeStamp[i], pkt.isCalibratedTimeStamp());
      }
    }
  }
  
  public void testIsDataValid () {
    for (int i = 0; i < packets.length; i++) {
      DataPacket pkt = asDataPacket(packets[i]);
      if (pkt != null) {
        check(i, dataValid[i], pkt.isDataValid());
      }
    }
  }
  
  public void testIsReferenceLocked () {
    for (int i = 0; i < packets.length; i++) {
      DataPacket pkt = asDataPacket(packets[i]);
      if (pkt != null) {
        check(i, refLock[i], pkt.isReferenceLocked());
      }
    }
  }
  
  public void testIsAutomaticGainControl () {
    for (int i = 0; i < packets.length; i++) {
      DataPacket pkt = asDataPacket(packets[i]);
      if (pkt != null) {
        check(i, agc[i], pkt.isAutomaticGainControl());
      }
    }
  }
  
  public void testIsSignalDetected () {
    for (int i = 0; i < packets.length; i++) {
      DataPacket pkt = asDataPacket(packets[i]);
      if (pkt != null) {
        check(i, signalDet[i], pkt.isSignalDetected());
      }
    }
  }
  
  public void testIsInvertedSpectrum () {
    for (int i = 0; i < packets.length; i++) {
      DataPacket pkt = asDataPacket(packets[i]);
      if (pkt != null) {
        check(i, inverted[i], pkt.isInvertedSpectrum());
      }
    }
  }
  
  public void testIsOverRange () {
    for (int i = 0; i < packets.length; i++) {
      DataPacket pkt = asDataPacket(packets[i]);
      if (pkt != null) {
        check(i, overRange[i], pkt.isOverRange());
      }
    }
  }
  
  public void testIsDiscontinuious () {
    for (int i = 0; i < packets.length; i++) {
      DataPacket pkt = asDataPacket(packets[i]);
      if (pkt != null) {
        check(i, discontin[i], pkt.isDiscontinuious());
      }
    }
  }
  
  public void testIsBit11 () {
    for (int i = 0; i < packets.length; i++) {
      DataPacket pkt = asDataPacket(packets[i]);
      if (pkt != null) {
        check(i, bit11[i], pkt.isBit11());
      }
    }
  }
  
  public void testIsBit10 () {
    for (int i = 0; i < packets.length; i++) {
      DataPacket pkt = asDataPacket(packets[i]);
      if (pkt != null) {
        check(i, bit10[i], pkt.isBit10());
      }
    }
  }
  
  public void testIsBit9 () {
    for (int i = 0; i < packets.length; i++) {
      DataPacket pkt = asDataPacket(packets[i]);
      if (pkt != null) {
        check(i, bit9[i], pkt.isBit9());
      }
    }
  }
  
  public void testIsBit8 () {
    for (int i = 0; i < packets.length; i++) {
      DataPacket pkt = asDataPacket(packets[i]);
      if (pkt != null) {
        check(i, bit8[i], pkt.isBit8());
      }
    }
  }
  
  public void testGetAssocPacketCount () {
    for (int i = 0; i < packets.length; i++) {
      DataPacket pkt = asDataPacket(packets[i]);
      if (pkt != null) {
        check(i, assocPacket[i], pkt.getAssocPacketCount());
      }
    }
  }
  
  public void testSetPacketType () {
    VRTPacket p = toPacket(PACKET3);
    for (PacketType val : PacketType.VALUES) {
      if (!val.isSupported()) continue;
      p.setPacketType(val);
      check(val, p.getPacketType());
    }
  }
  
  public void testSetTimeStamp () {
    VRTPacket p = toPacket(PACKET3);
    for (IntegerMode im : IntegerMode.VALUES) {
      for (FractionalMode fm : FractionalMode.VALUES) {
        TimeStamp ts = new TimeStamp(im, fm, 12345L, 67890L);
        
        p.setTimeStamp(ts);
        check(ts, p.getTimeStamp());
      }
    }
  }
  
  ////////////////////////
  ////////////////////////

  public void testSetPacketCount () {
    DataPacket p = asDataPacket(toPacket(PACKET3));
    for (int i = 0; i <= 0xF; i++) {
      p.setPacketCount(i);
      check(i, p.getPacketCount());
    }
  }
  
  ////////////////////////
  ////////////////////////
  
  public void testSetCalibratedTimeStamp () {
    DataPacket p = asDataPacket(toPacket(PACKET3));
    p.setCalibratedTimeStamp(null);  check(null,  p.isCalibratedTimeStamp());
    p.setCalibratedTimeStamp(TRUE);  check(TRUE,  p.isCalibratedTimeStamp());
    p.setCalibratedTimeStamp(FALSE); check(FALSE, p.isCalibratedTimeStamp());
  }
  
  public void testSetDataValid () {
    DataPacket p = asDataPacket(toPacket(PACKET3));
    p.setDataValid(null);  check(null,  p.isDataValid());
    p.setDataValid(TRUE);  check(TRUE,  p.isDataValid());
    p.setDataValid(FALSE); check(FALSE, p.isDataValid());
  }
  
  public void testSetReferenceLocked () {
    DataPacket p = asDataPacket(toPacket(PACKET3));
    p.setReferenceLocked(null);  check(null,  p.isReferenceLocked());
    p.setReferenceLocked(TRUE);  check(TRUE,  p.isReferenceLocked());
    p.setReferenceLocked(FALSE); check(FALSE, p.isReferenceLocked());
  }
  
  public void testSetAutomaticGainControl () {
    DataPacket p = asDataPacket(toPacket(PACKET3));
    p.setAutomaticGainControl(null);  check(null,  p.isAutomaticGainControl());
    p.setAutomaticGainControl(TRUE);  check(TRUE,  p.isAutomaticGainControl());
    p.setAutomaticGainControl(FALSE); check(FALSE, p.isAutomaticGainControl());
  }
  
  public void testSetSignalDetected () {
    DataPacket p = asDataPacket(toPacket(PACKET3));
    p.setSignalDetected(null);  check(null,  p.isSignalDetected());
    p.setSignalDetected(TRUE);  check(TRUE,  p.isSignalDetected());
    p.setSignalDetected(FALSE); check(FALSE, p.isSignalDetected());
  }
  
  public void testSetInvertedSpectrum () {
    DataPacket p = asDataPacket(toPacket(PACKET3));
    p.setInvertedSpectrum(null);  check(null,  p.isInvertedSpectrum());
    p.setInvertedSpectrum(TRUE);  check(TRUE,  p.isInvertedSpectrum());
    p.setInvertedSpectrum(FALSE); check(FALSE, p.isInvertedSpectrum());
  }
  
  public void testSetOverRange () {
    DataPacket p = asDataPacket(toPacket(PACKET3));
    p.setOverRange(null);  check(null,  p.isOverRange());
    p.setOverRange(TRUE);  check(TRUE,  p.isOverRange());
    p.setOverRange(FALSE); check(FALSE, p.isOverRange());
  }
  
  public void testSetDiscontinuious () {
    DataPacket p = asDataPacket(toPacket(PACKET3));
    p.setDiscontinuious(null);  check(null,  p.isDiscontinuious());
    p.setDiscontinuious(TRUE);  check(TRUE,  p.isDiscontinuious());
    p.setDiscontinuious(FALSE); check(FALSE, p.isDiscontinuious());
  }
  
  public void testSetBit11 () {
    DataPacket p = asDataPacket(toPacket(PACKET3));
    p.setBit11(null);  check(null,  p.isBit11());
    p.setBit11(TRUE);  check(TRUE,  p.isBit11());
    p.setBit11(FALSE); check(FALSE, p.isBit11());
  }
  
  public void testSetBit10 () {
    DataPacket p = asDataPacket(toPacket(PACKET3));
    p.setBit10(null);  check(null,  p.isBit10());
    p.setBit10(TRUE);  check(TRUE,  p.isBit10());
    p.setBit10(FALSE); check(FALSE, p.isBit10());
  }
  
  public void testSetBit9 () {
    DataPacket p = asDataPacket(toPacket(PACKET3));
    p.setBit9(null);  check(null,  p.isBit9());
    p.setBit9(TRUE);  check(TRUE,  p.isBit9());
    p.setBit9(FALSE); check(FALSE, p.isBit9());
  }
  
  public void testSetBit8 () {
    DataPacket p = asDataPacket(toPacket(PACKET3));
    p.setBit8(null);  check(null,  p.isBit8());
    p.setBit8(TRUE);  check(TRUE,  p.isBit8());
    p.setBit8(FALSE); check(FALSE, p.isBit8());
  }
  
  public void testSetAssocPacketCount () {
    DataPacket p = asDataPacket(toPacket(PACKET3));
    p.setAssocPacketCount(null); check(null, p.getAssocPacketCount());
    for (int i = 0; i <= 127; i++) {
      byte b = (byte)i;
      p.setAssocPacketCount(b); check(b, p.getAssocPacketCount());
    }
  }
}

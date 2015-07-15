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

import static nxm.vrt.lib.VRTMath.*;

/** This will test the FixedPointMath class.
 *
 *  @author         
 */
public final class FixedPointMathTest extends NMTestCase {

  /** public constructor to allow running this test case directly with JUnit. */
  public FixedPointMathTest (String methodName) { this(methodName, null); }

  /** This constructor is required by NeXtMidas Unit Testing framework.
      @param methodName The name of the method being called.
      @param midas      The midas context to use when running the tests.
   */
  public FixedPointMathTest(String methodName, Midas midas) {
    super(methodName, midas);
  }
  
  private static Test64[] TEST64 = {
    // These examples come from VRT 7.1.5.4 & 7.1.5.12
    new Test64(1.00e00, 20, 0x0000000000100000L, 0.00),
    new Test64(0.95e-6, 20, 0x0000000000000001L, 0.05e-6),
    
    // These examples come from VRT 7.1.5.5 & 7.1.5.6 & 7.1.5.7 & 7.1.5.8
    new Test64(+1.00e00, 20, 0x0000000000100000L, 0.00),
    new Test64(-1.00e00, 20, 0xFFFFFFFFFFF00000L, 0.00),
    new Test64(+0.95e-6, 20, 0x0000000000000001L, 0.05e-6),
    new Test64(-0.95e-6, 20, 0xFFFFFFFFFFFFFFFFL, 0.05e-6),
  };
  
  private static Test32[] TEST32 = {
    // These examples based on VRT 7.1.5.19-2
    new Test32(+1.00000, 22, 0x00400000, 0.00),
    new Test32(-1.00000, 22, 0xFFC00000, 0.00),
    new Test32(+2.37e-7, 22, 0x00000001, 5e-6),
    new Test32(-2.38e-7, 22, 0xFFFFFFFF, 5e-6),
    
    // These examples based on VRT 7.1.5.19-4
    new Test32(+1.000, 5, 0x00000020, 0.00),
    new Test32(-1.000, 5, 0xFFFFFFE0, 0.00),
    new Test32(+0.031, 5, 0x00000001, 5e-4),
    new Test32(-0.031, 5, 0xFFFFFFFF, 5e-4),
    
    // These examples based on VRT 7.1.5.19-5
    new Test32(+1.000, 16, 0x00010000, 0.00),
    new Test32(1.5e-5, 16, 0x00000001, 5e-6),
  };
  
  private static Test16[] TEST16 = {
    // These examples come from VRT 7.1.5.9, 7.1.5.10 (only applicable 16 bits considered)
    new Test16(+1.0000000, 7, 0x0080, 0.00),
    new Test16(-1.0000000, 7, 0xFF80, 0.00),
    new Test16(+0.0078125, 7, 0x0001, 5e-8),
    new Test16(-0.0078125, 7, 0xFFFF, 5e-8),
    
    // These examples come from VRT 7.1.5.15 (only applicable 16 bits considered)
    new Test16(+1.000000, 6, 0x0040, 0.00),
    new Test16(-1.000000, 6, 0xFFC0, 0.00),
    new Test16(+0.015625, 6, 0x0001, 5e-7),
    new Test16(-0.015625, 6, 0xFFFF, 5e-7),
  };
  
  
  ////////////////////////////////////
  public void testToDouble64 () {
    for (Test64 test : TEST64) {
      assertEquals(test.val, toDouble64(test.radix, test.bits), test.tol);
    }
  }  
  public void testToDouble32 () {
    for (Test32 test : TEST32) {
      assertEquals(test.val, toDouble32(test.radix, test.bits), test.tol);
    }
  }
  public void testToDouble16 () {
    for (Test16 test : TEST16) {
      assertEquals(test.val, toDouble16(test.radix, test.bits), test.tol);
    }
  }
  public void testFromDouble64 () {
    for (Test64 test : TEST64) {
      assertEquals(test.bits, fromDouble64(test.radix, test.val));
      assertEquals(test.val, toDouble64(test.radix, fromDouble64(test.radix, test.val)), test.tol);
    }
  }
  public void testFromDouble32 () {
    for (Test32 test : TEST32) {
      assertEquals(test.bits, fromDouble32(test.radix, test.val));
      assertEquals(test.val, toDouble32(test.radix, fromDouble32(test.radix, test.val)), test.tol);
    }
  }
  public void testFromDouble16 () {
    for (Test16 test : TEST16) {
      assertEquals(test.bits, fromDouble16(test.radix, test.val));
      assertEquals(test.val, toDouble16(test.radix, fromDouble16(test.radix, test.val)), test.tol);
    }
  }
  
  
  ////////////////////////////////////
  public void testToFloat64 () {
    for (Test64 test : TEST64) {
      assertEquals((float)test.val, toFloat64(test.radix, test.bits), test.tol);
    }
  }  
  public void testToFloat32 () {
    for (Test32 test : TEST32) {
      assertEquals((float)test.val, toFloat32(test.radix, test.bits), test.tol);
    }
  }
  public void testToFloat16 () {
    for (Test16 test : TEST16) {
      assertEquals((float)test.val, toFloat16(test.radix, test.bits), test.tol);
    }
  }
  public void testFromFloat64 () {
    for (Test64 test : TEST64) {
      assertEquals((float)test.val, toFloat64(test.radix, fromFloat64(test.radix, (float)test.val)), test.tol);
    }
  }
  public void testFromFloat32 () {
    for (Test32 test : TEST32) {
      assertEquals((float)test.val, toFloat32(test.radix, fromFloat32(test.radix, (float)test.val)), test.tol);
    }
  }
  public void testFromFloat16 () {
    for (Test16 test : TEST16) {
      assertEquals((float)test.val, toFloat16(test.radix, fromFloat16(test.radix, (float)test.val)), test.tol);
    }
  }
  
  
  ////////////////////////////////////
  public void testToLong64 () {
    for (Test64 test : TEST64) {
      assertEquals((long)test.val, toLong64(test.radix, test.bits));
    }
  }  
  public void testToLong32 () {
    for (Test32 test : TEST32) {
      assertEquals((long)test.val, toLong32(test.radix, test.bits));
    }
  }
  public void testToLong16 () {
    for (Test16 test : TEST16) {
      assertEquals((long)test.val, toLong16(test.radix, test.bits));
    }
  }
  public void testFromLong64 () {
    for (Test64 test : TEST64) {
      assertEquals((long)test.val, toLong64(test.radix, fromLong64(test.radix, (long)test.val)));
    }
  }
  public void testFromLong32 () {
    for (Test32 test : TEST32) {
      assertEquals((long)test.val, toLong32(test.radix, fromLong32(test.radix, (long)test.val)));
    }
  }
  public void testFromLong16 () {
    for (Test16 test : TEST16) {
      assertEquals((long)test.val, toLong16(test.radix, fromLong16(test.radix, (long)test.val)));
    }
  }
  
  
  ////////////////////////////////////
  public void testToInt64 () {
    for (Test64 test : TEST64) {
      assertEquals((int)test.val, toInt64(test.radix, test.bits));
    }
  }  
  public void testToInt32 () {
    for (Test32 test : TEST32) {
      assertEquals((int)test.val, toInt32(test.radix, test.bits));
    }
  }
  public void testToInt16 () {
    for (Test16 test : TEST16) {
      assertEquals((int)test.val, toInt16(test.radix, test.bits));
    }
  }
  public void testFromInt64 () {
    for (Test64 test : TEST64) {
      assertEquals((int)test.val, toInt64(test.radix, fromInt64(test.radix, (int)test.val)));
    }
  }
  public void testFromInt32 () {
    for (Test32 test : TEST32) {
      assertEquals((int)test.val, toInt32(test.radix, fromInt32(test.radix, (int)test.val)));
    }
  }
  public void testFromInt16 () {
    for (Test16 test : TEST16) {
      assertEquals((int)test.val, toInt16(test.radix, fromInt16(test.radix, (int)test.val)));
    }
  }
  
  
  
  
  
  private static class Test64 {
    final double val;
    final int    radix;
    final long   bits;
    final double tol;
    
    public Test64 (double val, int radix, long bits, double tol) {
      this.val   = val;
      this.radix = radix;
      this.bits  = bits;
      this.tol   = tol;
    }
  }
  
  
  private static class Test32 {
    final double val;
    final int    radix;
    final int    bits;
    final double tol;
    
    public Test32 (double val, int radix, int bits, double tol) {
      this.val   = val;
      this.radix = radix;
      this.bits  = bits;
      this.tol   = tol;
    }
  }
  
  
  private static class Test16 {
    final double val;
    final int    radix;
    final short  bits;
    final double tol;
    
    public Test16 (double val, int radix, int bits, double tol) {
      this.val   = val;
      this.radix = radix;
      this.bits  = (short)bits;
      this.tol   = tol;
    }
  }
}

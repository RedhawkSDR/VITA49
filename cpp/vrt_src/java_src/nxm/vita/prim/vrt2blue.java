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

package nxm.vita.prim;

import nxm.sys.lib.*;
import nxm.vrt.lib.*;

/**  Parent class for the auto-generated VRT-to-BLUE primitives.

    @author         
 */
public abstract class vrt2blue extends Primitive {
  private   final String[] FIELD_DEFS; // field definitions in the form { form0, name0, form1, name1, ... }
  protected final int[]    offset;
  private         Data     dataBuf;
  protected       byte[]   dataBuffer;
  private         DataFile inFile;
  private         DataFile outFile;


  /**  For use by subclasses only. */
  protected vrt2blue (String[] FIELD_DEFS) {
    this.FIELD_DEFS = FIELD_DEFS;
    this.offset     = new int[FIELD_DEFS.length/2];
  }
  
  @Override
  public int open () {
    inFile     = MA.getDataFile("IN");
    outFile    = MA.getDataFile("OUT");

    // OPEN INPUT FILE AND CHECK THE SUBRECORDS IN IT ==============================================
    inFile.open();

    // OPEN OUTPUT FILE ============================================================================
    int type = MA.getL("/TYPE", 3000);
    
    outFile.open(DataFile.OUTPUT);
    outFile.setType(type);

    for (int i = 0; i < FIELD_DEFS.length; i+=2) {
      String form = FIELD_DEFS[ i ];
      String name = FIELD_DEFS[i+1];
      outFile.addSubRec(name, form);
    }

    for (int i = 0; i < FIELD_DEFS.length/2; i++) {
      offset[i] = inFile.getRecOffset(i);
    }

    dataBuf    = outFile.getDataBuffer(1);
    dataBuffer = dataBuf.buf;

    return NORMAL;
  }
  
  @Override
  public int process () {
    if (!inFile.isReady(4)) return NOOP; // not ready for reading yet

    byte[] buf = new byte[VRTPacket.MAX_PACKET_LENGTH];
    int numRead = inFile.read(buf, 0, 4);
    if (numRead < 0) return FINISH;


    int len = (0xFF&buf[2])<<10 | (0xFF&buf[3])<<2;
    numRead = inFile.read(buf, 4, len-4);
    if (numRead != len-4) {
      throw new MidasException("Failure while reading, expected "+len+" bytes but got "+(numRead+4));
    }
    
    VRTPacket p = VRTConfig.getPacket(buf, 0, len, true, true);
    fromVRTPacket(p);
    
    return NORMAL;
  }
  
  @Override
  public int close () {
    if (inFile  != null) inFile.close();
    if (outFile != null) outFile.close();
    
    inFile  = null;
    outFile = null;
    
    return NORMAL;
  }
  
  /**  Converts a packet read in to a record. */
  protected abstract void fromVRTPacket (VRTPacket pkt);

  /**  Queue up the next record. */
  protected void nextRecord () {
    outFile.write(dataBuf);
  }
}

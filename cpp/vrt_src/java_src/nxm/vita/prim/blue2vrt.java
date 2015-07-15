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

/**  Parent class for the auto-generated BLUE-to-VRT primitives.

    @author         
 */
public abstract class blue2vrt extends Primitive {
  private   final String[] FIELD_DEFS; // field definitions in the form { form0, name0, form1, name1, ... }
  protected final int[]    offset;
  private         Data     dataBuf;
  protected       byte[]   dataBuffer;
  private         int      dataOffset;
  private         int      recLength;
  private         DataFile inFile;
  private         DataFile outFile;

  /**  For use by subclasses only. */
  protected blue2vrt (String[] FIELD_DEFS) {
    this.FIELD_DEFS = FIELD_DEFS;
    this.offset     = new int[FIELD_DEFS.length/2];
  }

  @Override
  public int open () {
    inFile     = MA.getDataFile("IN");
    outFile    = MA.getDataFile("OUT");
    xfer       = MA.getL("/TL", 1);

    // OPEN INPUT FILE AND CHECK THE SUBRECORDS IN IT ==============================================
    inFile.open();
    recLength  = inFile.getRecLength();
    dataBuf    = inFile.getDataBuffer(xfer);
    dataBuffer = new byte[recLength];

    if (inFile.isRecordBased()) {
      throw new MidasException("Expected a record-based input file but got "+inFile.getName());
    }

    for (int i = 0; i < FIELD_DEFS.length; i+=2) {
      String form = FIELD_DEFS[ i ];
      String name = FIELD_DEFS[i+1];
      int    idx  = inFile.findRec(name);

      if (idx < 0) {
        throw new MidasException("Could not find subrecord '"+name+"' in "+inFile.getName());
      }

      String fmt = inFile.getRecFormat(idx);
      if (!form.equals(fmt)) {
        throw new MidasException("Expected subrecord '"+name+"' to have format '"+form+"' but found '"+fmt+"' in "+inFile.getName());
      }

      offset[i/2] = inFile.getRecOffset(idx);
    }

    // OPEN OUTPUT FILE ============================================================================
    outFile.open(DataFile.OUTPUT);
    outFile.setType(1049);
    outFile.setFormat("SB");
    outFile.setDataRep("IEEE");
    outFile.setComment("Raw VRT packets");

    return NORMAL;
  }

  @Override
  public int process () {
    int numRead = inFile.read(dataBuf);
    if (numRead <  0) return FINISH;
    if (numRead == 0) return NOOP;


    dataOffset = 0;
    System.arraycopy(dataBuf.buf, dataOffset, dataBuffer, dataOffset, recLength);
    VRTPacket p = toVRTPacket(numRead);

    byte[] buf = p.getPacket();
    outFile.write(buf, 0, buf.length);

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

  /**  Converts records read in to a packet. */
  protected abstract VRTPacket toVRTPacket (int count);

  /**  Queue up the next record. */
  protected void nextRecord () {
    dataOffset += recLength;
    System.arraycopy(dataBuf.buf, dataOffset, dataBuffer, dataOffset, recLength);
  }
}

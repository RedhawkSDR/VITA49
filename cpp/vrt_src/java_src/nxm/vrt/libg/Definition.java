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

package nxm.vrt.libg;

import java.io.PrintStream;

/** A generic definition.
 *
 *  @author 
 */
abstract class Definition {
  /** Writes the definition to an HTML file and HTML index file. */
  public void writeHTML (PacketGen outParams, PrintStream html, PrintStream index, String fileName) {
    // default does nothing
  }

  /** Writes the definition to C++ header and source files. */
  public void writeCPP (PacketGen outParams, PrintStream hdr, PrintStream cpp) {
    // default does nothing
  }

  /** Writes the definition to a Java file. */
  public void writeJava (PacketGen outParams, PrintStream out) {
    // default does nothing
  }
}

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


/**  Exception indicating that a queue has filled up.
 *
 *  @author         
 */
public class QueueFullException extends RuntimeException {
  private static final long serialVersionUID = 0x2010091300000001L; // 2010-09-13 rev 00000001

  private final Object owner;
  private final int    count;

  /**  Creates a new instance.
      @param owner The owner of the queue.
      @param msg   Description of the error.
      @param count Number of items discarded as a result (-1 if unknown).
   */
  public QueueFullException (Object owner, String msg, int count) {
    super(msg);
    this.owner = owner;
    this.count = count;
  }

  /**  Creates a new instance.
      @param owner The owner of the queue.
      @param msg   Description of the error.
      @param count Number of items discarded as a result (-1 if unknown).
      @param t     Nested exception.
   */
  public QueueFullException (Object owner, String msg, int count, Throwable t) {
    super(msg, t);
    this.owner = owner;
    this.count = count;
  }

  /**  Gets the owner of the queue.. */
  public Object getQueueOwner () {
    return owner;
  }

  /**  Gets the number of packets discarded as a result of the queue being full (-1 if unknown). */
  public int getDiscardedCount () {
    return count;
  }
}

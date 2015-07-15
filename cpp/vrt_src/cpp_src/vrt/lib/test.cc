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

//
//  g++ -g -Wall VRTException.cc LeapSeconds.cc TimeStamp.cc temp.cc
//
//

#include <iostream>
#include "VRTObject.h"
#include "BasicDataPacket.h"
#include "BasicContextPacket.h"
#include "stdio.h"   // required for printf(..) on GCC4.4/libc6 2.11.1
#include "stdlib.h"  // required for EXIT_constants on GCC4.4/libc6 2.11.1


using namespace std;
using namespace vrt;
using namespace VRTMath;

int main () {
  try {
    vector<char> buffer(16);
    for (size_t i = 0; i < buffer.size(); i++) {
      buffer[i] = i;
    }
    printf("unpackByte (data, 3) = 0x0%x\n", (int)unpackByte(buffer, 3));
    printf("unpackShort(data, 1) = 0x0%x\n", (int)unpackShort(buffer, 1, BIG_ENDIAN));
    printf("unpackInt  (data, 1) = 0x0%x\n", (int)unpackInt(buffer, 1, BIG_ENDIAN));
    printf("unpackLong (data, 1) = 0x0%" PRIx64 "\n", (uint64_t)unpackLong(buffer, 1, BIG_ENDIAN));

    cout << "-------------------" << endl;
    cout << "INT8_NULL  = " << (int32_t)INT8_NULL << endl;
    cout << "INT16_NULL = " << (int32_t)INT16_NULL << endl;
    cout << "INT32_NULL = " << INT32_NULL << endl;
    cout << "INT64_NULL = " << INT64_NULL << endl;
    cout << "FLOAT_NAN  = " << FLOAT_NAN << endl;
    cout << "DOUBLE_NAN = " << DOUBLE_NAN << endl;
    cout << "_FALSE     = " << _FALSE << endl;
    cout << "_NULL      = " << _NULL << endl;
    cout << "_TRUE      = " << _TRUE << endl;
    cout << "+2         = " << ((boolNull)+2) << endl;
    cout << "-2         = " << ((boolNull)-2) << endl;
    cout << "-------------------" << endl;
    cout << "LeapSeconds::getDefaultInstance() = " << LeapSeconds::getDefaultInstance() << endl;
    cout << "PacketType_ExtContext             = " << PacketType_ExtContext << endl;
    cout << "DataItemFormat_Float              = " << DataItemFormat_Float  << endl;
    cout << "RealComplexType_Real              = " << RealComplexType_Real  << endl;
    cout << "PayloadFormat_INT8                = " << PayloadFormat_INT8    << endl;
    cout << "-------------------" << endl;

    BasicVRTPacket basic;
    BasicDataPacket data;
    BasicContextPacket context;
    TimeStamp time = TimeStamp::parseTime("2010-01-02T18:19:20.123", IntegerMode_GPS);

    data.setCalibratedTimeStamp(_TRUE);
    data.setTimeStamp(time);

    context.setClassID("12-34-56:ABCD.1234");
    context.setFrequencyRF(3e9);
    context.setBandwidth(1e6);

    cout << "time    = " << time    << endl;
    cout << "basic   = " << basic   << endl;
    cout << "data    = " << data    << endl;
    cout << "context = " << context << endl;
    
    VRTObject o;
    TimeStamp ts = TimeStamp::parseTime("2000-01-01T00:00:00.000Z", IntegerMode_GPS);
    cout << "-------------------" << endl;
    cout << "time = " << TimeStamp(IntegerMode_None, FractionalMode_None, 0, 0) << endl;
    cout << "ts   = " << ts.toGPS() << endl;
    cout << "ts   = " << ts.toUTC() << endl;

    return EXIT_SUCCESS;
  }
  catch (VRTException &e) {
    cout << "ERROR: " << e << endl;
    return EXIT_FAILURE;
  }
  catch (exception &e) {
    cout << "ERROR: " << e.what() << endl;
    return EXIT_FAILURE;
  }
}

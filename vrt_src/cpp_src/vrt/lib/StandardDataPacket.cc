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

#include "StandardDataPacket.h"

static const int64_t ERROR = __INT64_C(-1);

static const int64_t ID_TO_FORMAT[] = {
  ERROR,                            // reserved
  __INT64_C(0x000000C300000000),    //  4-bit Signed Fixed Point
  __INT64_C(0x000001C700000000),    //  8-bit Signed Fixed Point
  __INT64_C(0x000003CF00000000),    // 16-bit Signed Fixed Point
  __INT64_C(0x000007DF00000000),    // 32-bit Signed Fixed Point
  __INT64_C(0x00000FFF00000000),    // 64-bit Signed Fixed Point
  __INT64_C(0x0E0007DF00000000),    // 32-bit IEEE-754 Single-Precision
  __INT64_C(0x0F000FFF00000000),    // 64-bit IEEE-754 Double-Precision
  __INT64_C(0x1000000000000000),    //  1-bit Unsigned Fixed Point
  __INT64_C(0x100000C300000000),    //  4-bit Unsigned Fixed Point
  __INT64_C(0x100001C700000000),    //  8-bit Unsigned Fixed Point
  __INT64_C(0x100003CF00000000),    // 16-bit Unsigned Fixed Point
  __INT64_C(0x100007DF00000000),    // 32-bit Unsigned Fixed Point
  __INT64_C(0x10000FFF00000000),    // 64-bit Unsigned Fixed Point
  ERROR,                            // reserved
  ERROR,                            // reserved
};

static const int64_t SIZE_TO_TYPE[] = {
  __INT64_C(0x00FFFFFA00000000),   ERROR,   ERROR,  __INT64_C(0x00FFFFFA00010000),  //  1-bit to  4-bit
                          ERROR,   ERROR,   ERROR,  __INT64_C(0x00FFFFFA00020000),  //  5-bit to  8-bit
                          ERROR,   ERROR,   ERROR,                          ERROR,  //  9-bit to 12-bit
                          ERROR,   ERROR,   ERROR,  __INT64_C(0x00FFFFFA00030000),  // 13-bit to 16-bit
                          ERROR,   ERROR,   ERROR,                          ERROR,  // 17-bit to 20-bit
                          ERROR,   ERROR,   ERROR,                          ERROR,  // 21-bit to 24-bit
                          ERROR,   ERROR,   ERROR,                          ERROR,  // 25-bit to 28-bit
                          ERROR,   ERROR,   ERROR,  __INT64_C(0x00FFFFFA00040000),  // 29-bit to 32-bit
                          ERROR,   ERROR,   ERROR,                          ERROR,  // 33-bit to 36-bit
                          ERROR,   ERROR,   ERROR,                          ERROR,  // 37-bit to 40-bit
                          ERROR,   ERROR,   ERROR,                          ERROR,  // 41-bit to 44-bit
                          ERROR,   ERROR,   ERROR,                          ERROR,  // 45-bit to 48-bit
                          ERROR,   ERROR,   ERROR,                          ERROR,  // 49-bit to 52-bit
                          ERROR,   ERROR,   ERROR,                          ERROR,  // 53-bit to 56-bit
                          ERROR,   ERROR,   ERROR,                          ERROR,  // 57-bit to 60-bit
                          ERROR,   ERROR,   ERROR,  __INT64_C(0x00FFFFFA00050000),  // 61-bit to 64-bit
};

string StandardDataPacket::getPacketValid (bool strict, int32_t length) const {
  string        err = BasicDataPacket::getPacketValid(strict, length);
  PayloadFormat fmt = toPayloadFormat(getClassIdentifier());
  if (err != "") return err;

  if (isNull(fmt)) {
  ostringstream str;
    str << "Invalid use of " << getClassName() << "with ClassID=" << getClassID() << ".";
    return str.str();
  }
  return "";
}

 void StandardDataPacket::toStringStream (ostringstream &str) const {
  BasicVRTPacket::toStringStream(str);
  str << " DataType="        << getDataType();
  str << " RealComplexType=" << getRealComplexType();
  str << " FrameSize="       << getFrameSize();
  str << " DataLength="      << getDataLength() << " samples";
}

void StandardDataPacket::setPayloadFormat (const PayloadFormat &pf) {
  int64_t classID = toClassID(pf);
  if (classID == ERROR) {
    throw VRTException("Invalid payload format for use with StandardDataPacket");
  }
  BasicDataPacket::setClassIdentifier(classID);
  BasicDataPacket::setPayloadFormat(pf);
}

void StandardDataPacket::setPayloadFormat (DataType type, RealComplexType realComplex, int32_t frameSize) {
  setPayloadFormat(_toPayloadFormat(type, realComplex, frameSize));
}

PayloadFormat StandardDataPacket::toPayloadFormat (int64_t classID) {
  if (!isStandardDataPacket(classID)) return PayloadFormat(ERROR); // ERROR

  int64_t id   = classID;
  int64_t bits = ID_TO_FORMAT[(int32_t)((id & 0x000F0000) >> 16)]  // Item Format + Packing Size + Item Size
               | ((id & __INT64_C(0x00300000)) << 41)              // Real/Complex
               | ((id & __INT64_C(0x0000FFFF))      );             // Vector Size

  return PayloadFormat(bits); // note that ERROR=-1 is the same as NULL for PayloadFormat
}

int64_t StandardDataPacket::toClassID (const PayloadFormat &pf) {
  if (isNull(pf)) return ERROR;

  int64_t bits  = pf.getBits();
  int64_t check = bits & __INT64_C(0x0FFFF000FFFF0000);
  int64_t itmSz = bits & __INT64_C(0x0000001F00000000);
  int64_t fmtSz = bits & __INT64_C(0x00000FE000000000);

  if ((itmSz << 6) != fmtSz) {
   std::cout << "WARNING: DOUBLE SUPPORPT DOES NOT WORK - TODO: Fix this!" << std::endl;
  }
  if (check == __INT64_C(0x0000000000000000)) {
    return SIZE_TO_TYPE[(int)(itmSz>>32)]                  // Type size
         | ((bits >> 41) & __INT64_C(0x00380000))          // Real/Complex + Signed/Unsigned
         | ((bits      ) & __INT64_C(0x0000FFFF));         // Vector Size
  }
  else if (check == __INT64_C(0x0E00000000000000)) {
    return __INT64_C(0x00FFFFFA00060000)                   // 32-bit IEEE-754 Double-Precision
         | ((bits >> 41) & __INT64_C(0x00300000))          // Real/Complex
         | ((bits      ) & __INT64_C(0x0000FFFF));         // Vector Size
  }
  else if (check == __INT64_C(0x0F00000000000000)) {
    return __INT64_C(0x00FFFFFA00070000)                   // 64-bit IEEE-754 Double-Precision
         | ((bits >> 41) & __INT64_C(0x00300000))          // Real/Complex
         | ((bits      ) & __INT64_C(0x0000FFFF));         // Vector Size
  }
  else {
    return ERROR;
  }
}

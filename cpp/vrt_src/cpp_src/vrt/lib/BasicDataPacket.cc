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

#include "BasicDataPacket.h"
#define isStandardDataType(type) (((type) >= 0x0) && ((type) <= 0xF))  // This should probably be a function...

using namespace vrt;

static const char TRAILER_BIT  = 0x4;  // Trailer present bit in buf[0]

BasicDataPacket::BasicDataPacket (const BasicVRTPacket &p) :
		  BasicVRTPacket(p)
{
	if (!isData()) {
		throw VRTException("Can not create DataPacket when input is not data");
	}
}

BasicDataPacket::BasicDataPacket () :
		  BasicVRTPacket()
{
	// done
}

BasicDataPacket::BasicDataPacket(const int32_t pktsize) :
		  BasicVRTPacket(pktsize)
{

}

BasicDataPacket::BasicDataPacket (const void *buf, size_t len, bool readOnly) :
		  BasicVRTPacket(buf,len,readOnly)
{
	// done
}

BasicDataPacket::BasicDataPacket (const vector<char> &buf, ssize_t start, ssize_t end, bool readOnly) :
		  BasicVRTPacket(buf, start, end, readOnly)
{
	// done
}

BasicDataPacket::BasicDataPacket (vector<char> *buf, bool readOnly) :
		  BasicVRTPacket(buf, readOnly)
{
	// done
}


BasicDataPacket::BasicDataPacket (const vector<char> &buf, bool readOnly) :
		  BasicVRTPacket(buf, readOnly)
{
	// done
}

BasicDataPacket::BasicDataPacket (const BasicVRTPacket &p, PacketType type, int64_t classID,
		int32_t minPayloadLength, int32_t maxPayloadLength) :
		BasicVRTPacket(p,type,classID,minPayloadLength,maxPayloadLength)
{
	if (!isData()) {
		throw VRTException("Can not create DataPacket when input is not data");
	}
}


string BasicDataPacket::getPacketValid (bool strict, int32_t length) const {
	string err = BasicVRTPacket::getPacketValid(strict, length);
	if (err != "") return err;

	if (!isData()) {
		ostringstream str;
		str << "Invalid use of " << getClassName() << " with " << getPacketType() << " packet.";
		return str.str();
	}
	return "";
}

void BasicDataPacket::toStringStream (ostringstream &str) const {
	BasicVRTPacket::toStringStream(str);
	Utilities::append(str, " CalibratedTimeStamp=", isCalibratedTimeStamp());
	Utilities::append(str, " DataValid=",           isDataValid());
	Utilities::append(str, " ReferenceLocked=",     isReferenceLocked());
	Utilities::append(str, " AGC=",                 isAutomaticGainControl());
	Utilities::append(str, " SignalDetected=",      isSignalDetected());
	Utilities::append(str, " InvertedSpectrum=",    isInvertedSpectrum());
	Utilities::append(str, " OverRange=",           isOverRange());
	Utilities::append(str, " Discontinuous=",       isDiscontinuous());
	Utilities::append(str, " Bit11=",               isBit11());
	Utilities::append(str, " Bit10=",               isBit10());
	Utilities::append(str, " Bit9=",                isBit9());
	Utilities::append(str, " Bit8=",                isBit8());
	Utilities::append(str, " AssocPacketCount=",    getAssocPacketCount());
}

int8_t BasicDataPacket::getAssocPacketCount () const {
	if (!hasTrailer()) return INT8_NULL; // no trailer
	int8_t val = bbuf[getPacketLength()-1];
	return ((val &0x80) == 0)? INT8_NULL : (int8_t)(val & 0x7F);
}

void BasicDataPacket::setAssocPacketCount (int8_t v) {
	if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");
	if ((v != INT8_NULL) && (v < 0)) {
		throw VRTException("Invalid associated packet count %d", v);
	}
	if (!hasTrailer()) shiftTrailer(+4);

	int8_t val = (v == INT8_NULL)? 0 : (0x80 | v);
	bbuf[getPacketLength()-1] = val;
}

int32_t BasicDataPacket::getTrailer() const {
	if (!hasTrailer()) return INT32_NULL;
	return VRTMath::unpackInt(bbuf,getPayloadLength()+getHeaderLength(),BIG_ENDIAN);
}

boolNull BasicDataPacket::getTrailerBit (int32_t enable, int32_t indicator) const {
	if (!hasTrailer()) return _NULL; // no trailer
	return getStateEventBit(bbuf, getPacketLength()-4, enable, indicator);
}

void BasicDataPacket::setTrailerBit (int32_t enable, int32_t indicator, boolNull value) {
	if (readOnly) throw VRTException("Can not write to read-only VRTPacket.");
	if (!hasTrailer()) {
		if (value == _NULL) return; // no trailer, no need to set to null
		shiftTrailer(+4);
		bbuf[0] |= TRAILER_BIT;
	}
	setStateEventBit(bbuf, getPacketLength()-4, enable, indicator, value);
}

int32_t BasicDataPacket::getDataLength (const PayloadFormat &pf) const {
	if (isNull(pf)) throw VRTException("Payload format is null");

	int32_t bitsPerSample = pf.getItemPackingFieldSize();
	int32_t complexMult   = (pf.isComplex())? 2 : 1;
	bool    naturalSize   = (bitsPerSample ==  8) || (bitsPerSample == 16) ||
			(bitsPerSample == 32) || (bitsPerSample == 64);

	if (naturalSize || (!pf.isProcessingEfficient())) {
		// LinkEfficient (or naturally sized, in which case ProcessingEfficient = LinkEfficient)
		int32_t totalBits = (getPayloadLength() * 8) - getPadBitCount();
		return (totalBits / bitsPerSample) / complexMult;
	}
	else if (bitsPerSample <= 32) {
		// ProcessingEfficient with 32-bit word size
		int32_t samplesPerWord = 32 / bitsPerSample;
		int32_t totalWords   = getPayloadLength() / 4;
		int32_t totalSamples   = samplesPerWord * totalWords;
		int32_t padSamples     = getPadBitCount() / bitsPerSample;
		return (totalSamples - padSamples) / complexMult;
	}
	else {
		// ProcessingEfficient with 64-bit word size
		int32_t samplesPerWord = 64 / bitsPerSample;
		int32_t totalWords   = getPayloadLength() / 8;
		int32_t totalSamples   = samplesPerWord * totalWords;
		int32_t padSamples     = getPadBitCount() / bitsPerSample;
		return (totalSamples - padSamples) / complexMult;
	}
}

TimeStamp BasicDataPacket::getNextTimeStamp (double sampleRate, const PayloadFormat &pf) const {
	TimeStamp ts = getTimeStamp();
	if (isNull(ts)) return ts;
	double dt = getDataLength(pf) / sampleRate; // seconds
	return ts.addPicoSeconds((int64_t)(dt / TimeStamp::ONE_SEC));
}

int32_t BasicDataPacket::getLostSamples (const TimeStamp &expected, double sampleRate) const {
	TimeStamp ts = getTimeStamp();
	if (isNull(ts) || isNull(expected)) {
		throw VRTException("Can not compute number of lost samples, time stamp is null.");
	}
	int64_t timeDeltaSec = ts.getSecondsUTC()  - expected.getSecondsUTC();
	int64_t timeDeltaPS  = ts.getPicoSeconds() - expected.getPicoSeconds() + (timeDeltaSec * TimeStamp::ONE_SEC);
	return (int32_t)(timeDeltaPS / (sampleRate * TimeStamp::ONE_SEC));
}

void BasicDataPacket::setDataLength (const PayloadFormat &pf, int32_t length) {
	if (isNull(pf)) throw VRTException("Payload format is null");

	int32_t bitsPerSample = pf.getItemPackingFieldSize();
	int32_t samples       = (pf.isComplex())? 2*length : length;

	bool    naturalSize   = (bitsPerSample ==  8) || (bitsPerSample == 16) ||
			(bitsPerSample == 32) || (bitsPerSample == 64);
	if (naturalSize || (!pf.isProcessingEfficient())) {
		// LinkEfficient (if sample size is multiple of 8-bits, ProcessingEfficient = LinkEfficient)
		int32_t totalBits  = samples * bitsPerSample;
		int32_t totalBytes = (totalBits  + 7) / 8;   // +7 to make it round up
		int32_t totalWords = (totalBytes + 3) / 4;   // +3 to make it round up
		setPadBitCount(totalWords*32 - totalBits, bitsPerSample); // words -> bits
		setPayloadLength(totalWords * 4);            // words -> bytes
	}
	else if (bitsPerSample <= 32) {
		// ProcessingEfficient with 32-bit word size
		int32_t samplesPerWord = 32 / bitsPerSample;
		int32_t totalWords      = (samples + samplesPerWord-1) / samplesPerWord;  // +(samplesPerWord-1) to round up
		int32_t samplesLastWord = samples - ((totalWords-1) * samplesPerWord);

		setPadBitCount(32 - samplesLastWord*bitsPerSample, bitsPerSample);
		setPayloadLength(totalWords * 4);
	}
	else {
		// ProcessingEfficient with 64-bit word size
		int32_t samplesPerWord = 64 / bitsPerSample;
		int32_t totalWords      = (samples + samplesPerWord-1) / samplesPerWord;  // +(samplesPerWord-1) to round up
		int32_t samplesLastWord = samples - ((totalWords-1) * samplesPerWord);

		setPadBitCount(64 - samplesLastWord*bitsPerSample, bitsPerSample);
		setPayloadLength(totalWords * 8);
	}
}

//added for basic copy back of pointer
void* BasicDataPacket::getData(){
	return &bbuf[getHeaderLength()];
}



void* BasicDataPacket::getDataShort (const PayloadFormat &pf, bool raw){
	if (isNull(pf)) throw VRTException("Payload format is null");
	int32_t len = getScalarDataLength(pf);
	if (pf.getDataType() == DataType_Int16) {
		// Fast version where no conversion (other than byte order) is required
		vector<int16_t> array(len);
		getData(pf, &array[0]);
		return &array[0];
	}
	else {
		vector<int16_t> array(len);
		PackUnpack::unpackAsShort(pf, &bbuf[0], getHeaderLength(), &array[0], NULL, NULL, len);
		return &array[0];
	}
}
void BasicDataPacket::swapPayloadBytes(const PayloadFormat &pf, const void* array){
	int32_t len  = getPayloadLength() - getPadBitCount()/8; // only care if PadBitCount > 8
	int32_t off  = getHeaderLength();
	int32_t size = pf.getDataItemSize();

	if (size == 16) {
		// BIG_ENDIAN -> LITTLE_ENDIAN
		int16_t *src  = ((int16_t*)&bbuf[off]);
		int16_t *dest = ((int16_t*)&bbuf[off]);

		for (int32_t i = 0; i < len/2; i++) {
			dest[i] = __bswap_16(src[i]);
		}
	}
	else if (size == 32) {
		// BIG_ENDIAN -> LITTLE_ENDIAN
		int32_t *src  = ((int32_t*)&bbuf[off]);
		int32_t *dest = ((int32_t*)&bbuf[off]);

		for (int32_t i = 0; i < len/4; i++) {
			dest[i] = __bswap_32(src[i]);
		}
	}
	else { // size == 64
		// BIG_ENDIAN -> LITTLE_ENDIAN
		int64_t *src  = ((int64_t*)&bbuf[off]);
		int64_t *dest = ((int64_t*)&bbuf[off]);

		for (int64_t i = 0; i < len/8; i++) {
			dest[i] = __bswap_64(src[i]);
		}
	}
}

void* BasicDataPacket::getData_normal(const PayloadFormat &pf,int position){
	DataType type = pf.getDataType();

	if (type == -1) {
		throw VRTException("Fast unpacking of given data format not supported");
	}
	int32_t len  = getPayloadLength() - getPadBitCount()/8; // only care if PadBitCount > 8
	int32_t off  = getHeaderLength()+position;

	return &bbuf[off];
}

void __attribute__((hot)) *BasicDataPacket::getData (const PayloadFormat &pf, void *array, bool convert) const {
	DataType type = pf.getDataType();

	if (type == -1) {
		throw VRTException("Fast unpacking of given data format not supported");
	}

	int32_t len  = getPayloadLength() - getPadBitCount()/8; // only care if PadBitCount > 8
	int32_t off  = getHeaderLength();
	int32_t size = pf.getDataItemSize();

	if (!convert || (size <= 8) || (BYTE_ORDER == BIG_ENDIAN)) {
		// Able to use a basic memory copy
		memcpy(array, &bbuf[off], len);
	}
	else if (size == 16) {
		// BIG_ENDIAN -> LITTLE_ENDIAN
		int16_t *src  = ((int16_t*)&bbuf[off]);
		int16_t *dest = ((int16_t*)array);

		for (int32_t i = 0; i < len/2; i++) {
			dest[i] = __bswap_16(src[i]);
		}
	}
	else if (size == 32) {
		// BIG_ENDIAN -> LITTLE_ENDIAN
		int32_t *src  = ((int32_t*)&bbuf[off]);
		int32_t *dest = ((int32_t*)array);

		for (int32_t i = 0; i < len/4; i++) {
			dest[i] = __bswap_32(src[i]);
		}
	}
	else { // size == 64
		// BIG_ENDIAN -> LITTLE_ENDIAN
		int64_t *src  = ((int64_t*)&bbuf[off]);
		int64_t *dest = ((int64_t*)array);

		for (int64_t i = 0; i < len/8; i++) {
			dest[i] = __bswap_64(src[i]);
		}
	}
	return array;
}

void __attribute__((hot)) BasicDataPacket::setData (const PayloadFormat &pf, const void *array, int32_t len, bool convert) {
	DataType type = pf.getDataType();

	if (type == -1) {
		throw VRTException("Fast packing of given data format not supported");
	}
	int32_t off           = getHeaderLength();
	int32_t size          = pf.getDataItemSize();
	int32_t bitsPerSample = pf.getItemPackingFieldSize();

	if (((len*8) % size) != 0) {
		throw VRTException("Given length of %d bytes is not a multiple of %d bits", len, size);
	}

	int32_t overflow = (len % 4);
	int32_t padBytes = (4 - overflow) % 4; // %4 to make 4->0
	setPadBitCount(padBytes * 8, bitsPerSample);
	setPayloadLength(len + padBytes);

	if (!convert || (size <= 8) || (BYTE_ORDER == BIG_ENDIAN)) {
		// Able to use a basic memory copy
		memcpy(&bbuf[off], array, len);
	}
	else if (size == 16) {
		// LITTLE_ENDIAN -> BIG_ENDIAN
		int16_t *src  = ((int16_t*)array);
		int16_t *dest = ((int16_t*)&bbuf[off]);

		for (int32_t i = 0; i < len/2; i++) {
			dest[i] = __bswap_16(src[i]);
		}
	}
	else if (size == 32) {
		// LITTLE_ENDIAN -> BIG_ENDIAN
		int32_t *src  = ((int32_t*)array);
		int32_t *dest = ((int32_t*)&bbuf[off]);

		for (int32_t i = 0; i < len/4; i++) {
			dest[i] = __bswap_32(src[i]);
		}
	}
	else { // size == 64
		// LITTLE_ENDIAN -> BIG_ENDIAN
		int64_t *src  = ((int64_t*)array);
		int64_t *dest = ((int64_t*)&bbuf[off]);

		for (int64_t i = 0; i < len/8; i++) {
			dest[i] = __bswap_64(src[i]);
		}
	}

	off += len;
	switch (padBytes) { // be nice and set pad bytes to 0 so it is easier to detect errors
	case 3: bbuf[off++] = 0; // FALLTHROUGH
	case 2: bbuf[off++] = 0; // FALLTHROUGH
	case 1: bbuf[off++] = 0; // FALLTHROUGH
	}
}

int32_t BasicDataPacket::getFieldCount () const {
	return BasicVRTPacket::getFieldCount() + 14;
}

string BasicDataPacket::getFieldName (int32_t id) const {
	switch (id - BasicVRTPacket::getFieldCount()) {
	case  0: return "CalibratedTimeStamp";
	case  1: return "DataValid";
	case  2: return "ReferenceLocked";
	case  3: return "AGC";
	case  4: return "SignalDetected";
	case  5: return "InvertedSpectrum";
	case  6: return "OverRange";
	case  7: return "Discontinuous";
	case  8: return "Bit11";
	case  9: return "Bit10";
	case 10: return "Bit9";
	case 11: return "Bit8";
	case 12: return "AssocPacketCount";
	case 13: return "PayloadFormat";
	default: return BasicVRTPacket::getFieldName(id);
	}
}

ValueType BasicDataPacket::getFieldType (int32_t id) const {
	switch (id - BasicVRTPacket::getFieldCount()) {
	case  0: return ValueType_BoolNull;
	case  1: return ValueType_BoolNull;
	case  2: return ValueType_BoolNull;
	case  3: return ValueType_BoolNull;
	case  4: return ValueType_BoolNull;
	case  5: return ValueType_BoolNull;
	case  6: return ValueType_BoolNull;
	case  7: return ValueType_BoolNull;
	case  8: return ValueType_BoolNull;
	case  9: return ValueType_BoolNull;
	case 10: return ValueType_BoolNull;
	case 11: return ValueType_BoolNull;
	case 12: return ValueType_Int8;
	case 13: return ValueType_VRTObject;
	default: return BasicVRTPacket::getFieldType(id);
	}
}

Value* BasicDataPacket::getField (int32_t id) const {
	switch (id - BasicVRTPacket::getFieldCount()) {
	case  0: return new Value(isCalibratedTimeStamp());
	case  1: return new Value(isDataValid());
	case  2: return new Value(isReferenceLocked());
	case  3: return new Value(isAutomaticGainControl());
	case  4: return new Value(isSignalDetected());
	case  5: return new Value(isInvertedSpectrum());
	case  6: return new Value(isOverRange());
	case  7: return new Value(isDiscontinuous());
	case  8: return new Value(isBit11());
	case  9: return new Value(isBit10());
	case 10: return new Value(isBit9());
	case 11: return new Value(isBit8());
	case 12: return new Value(getAssocPacketCount());
	case 13: return new Value(new PayloadFormat(getPayloadFormat()), true);
	default: return BasicVRTPacket::getField(id);
	}
}

void BasicDataPacket::setField (int32_t id, const Value* val) {
	switch (id - BasicVRTPacket::getFieldCount()) {
	case  0: setCalibratedTimeStamp(     val->as<boolNull>()); break;
	case  1: setDataValid(               val->as<boolNull>()); break;
	case  2: setReferenceLocked(         val->as<boolNull>()); break;
	case  3: setAutomaticGainControl(    val->as<boolNull>()); break;
	case  4: setSignalDetected(          val->as<boolNull>()); break;
	case  5: setInvertedSpectrum(        val->as<boolNull>()); break;
	case  6: setOverRange(               val->as<boolNull>()); break;
	case  7: setDiscontinuous(           val->as<boolNull>()); break;
	case  8: setBit11(                   val->as<boolNull>()); break;
	case  9: setBit10(                   val->as<boolNull>()); break;
	case 10: setBit9(                    val->as<boolNull>()); break;
	case 11: setBit8(                    val->as<boolNull>()); break;
	case 12: setAssocPacketCount(        val->as<int8_t  >()); break;
	case 13: setPayloadFormat(  *val->cast<PayloadFormat*>()); break;
	default: BasicVRTPacket::setField(id,val); break;
	}
}

/* ===================== COPYRIGHT NOTICE =====================
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
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see http://www.gnu.org/licenses/.
 * ============================================================
 */

#include "BasicVRTState.h"
#include <time.h>
#include <iomanip>

using namespace std;
using namespace private_BasicContextPacket;

#define DEFAULT_CONTEXT_TRIGGER 0x3E308000
#define DEFAULT_EVENT_TRIGGER   0x00000000

namespace vrt {


BasicVRTState::~BasicVRTState () {
    // done
}

BasicVRTState::BasicVRTState () :
  contextTrigger(DEFAULT_CONTEXT_TRIGGER),
  eventTrigger(DEFAULT_EVENT_TRIGGER),
  lastChanged(TimeStamp()),
  lastUpdated(TimeStamp()),
  initialized(false),
  totalOverRangeSamples(0),
  totalOverRangePackets(0),
  totalDiscontinuousPackets(0),
  latestContext(),
  currentContextState()
{
  // done
}

BasicVRTState::BasicVRTState (const BasicContextPacket& ctx) :
  contextTrigger(DEFAULT_CONTEXT_TRIGGER),
  eventTrigger(DEFAULT_EVENT_TRIGGER),
  lastChanged(TimeStamp()),
  lastUpdated(TimeStamp()),
  initialized(false),
  totalOverRangeSamples(0),
  totalOverRangePackets(0),
  totalDiscontinuousPackets(0),
  latestContext(ctx),
  currentContextState(ctx)
{
  initState(currentContextState);
}

BasicVRTState::BasicVRTState (int32_t contextTrigger, int32_t eventTrigger) :
  contextTrigger(contextTrigger),
  eventTrigger(eventTrigger),
  lastChanged(TimeStamp()),
  lastUpdated(TimeStamp()),
  initialized(false),
  totalOverRangeSamples(0),
  totalOverRangePackets(0),
  totalDiscontinuousPackets(0),
  latestContext(),
  currentContextState()
{

}

BasicVRTState::BasicVRTState (int32_t contextTrigger, int32_t eventTrigger, const BasicContextPacket& ctx) :
  contextTrigger(contextTrigger),
  eventTrigger(eventTrigger),
  lastChanged(TimeStamp()),
  lastUpdated(TimeStamp()),
  initialized(false),
  totalOverRangeSamples(0),
  totalOverRangePackets(0),
  totalDiscontinuousPackets(0),
  latestContext(BasicContextPacket(ctx)),
  currentContextState(BasicContextPacket(ctx))
{
  initState(currentContextState);
}

void BasicVRTState::initState(BasicContextPacket& ctx) {
  updateState(ctx);
}

#define PERSISTENT_MASK     0xFC000000
#define SINGLE_PACKET_MASK  0x03000000
#define ENABLE_MASK         0xFF000000
#define INDICATOR_MASK      0x000FF000

#define UPDATED   0x80
#define TRIGGERED 0x40
#define ERRORED   0x20

int8_t BasicVRTState::copy(BasicContextPacket& dest, const BasicContextPacket& src, int32_t& field) {
  int32_t srcOffset = src.getOffset(field);
  int32_t destOffset = dest.getOffset(field);
  int32_t fieldLen = src.getFieldLen(field);
  bool updated = false;
  bool triggered = false;
  if (field != STATE_EVENT) {
    destOffset = dest.shiftPayload(destOffset, fieldLen, true);
    updated = memcmp(&dest.bbuf[destOffset] + dest.getHeaderLength(),&src.bbuf[srcOffset] + src.getHeaderLength(),fieldLen) != 0;

    dest.setContextIndicatorFieldBit(field,true);
    memcpy(&dest.bbuf[destOffset] + dest.getHeaderLength(),&src.bbuf[srcOffset] + src.getHeaderLength(),fieldLen);
    triggered = ((contextTrigger & field) != 0) && updated;
  }
  else {
    destOffset = dest.shiftPayload(destOffset, fieldLen, true);
    dest.setContextIndicatorFieldBit(STATE_EVENT, true);
    int32_t srcBits = src.getL(STATE_EVENT);
    int32_t srcEnable = srcBits & ENABLE_MASK;
    int32_t srcEnableMask = srcEnable | (srcEnable >> 12);
    int32_t destBits = dest.getL(STATE_EVENT);
    int32_t destEnable = destBits & ENABLE_MASK;

    updated = (srcBits & destBits) != srcBits;
    triggered = ((srcBits ^ destBits) & eventTrigger) != 0;
    destBits = (destBits & ~(srcEnableMask)) | (srcBits & (srcEnableMask)) | srcEnable | destEnable;
    memcpy(&dest.bbuf[destOffset] + dest.getHeaderLength(),&destBits,fieldLen);

  }
  if (updated) currentContextState.cacheIndicator = currentContextState.cacheIndicator & ~field;
  return (int8_t)(((triggered)? TRIGGERED : 0) | ((updated)? UPDATED : 0));
}

bool BasicVRTState::updateState(const BasicDataPacket& src) {
  if (src.hasTrailer()) {
    lastUpdated = src.getTimeStamp();
    int32_t destOffset = currentContextState.getOffset(STATE_EVENT);
    int32_t fieldLen = src.getTrailerLength();
    destOffset = currentContextState.shiftPayload(destOffset, fieldLen, true);
    currentContextState.setContextIndicatorFieldBit(STATE_EVENT, true);

    int32_t srcBits = src.getTrailer();
    int32_t srcEnable = srcBits & ENABLE_MASK;
    int32_t srcEnableMask = srcEnable | (srcEnable >> 12);
    int32_t destBits = currentContextState.getL(STATE_EVENT);
    int32_t destEnable = destBits & ENABLE_MASK;

    bool updated = (srcBits & destBits) != srcBits;
    //bool triggered = ((srcBits ^ destBits) & eventTrigger) != 0;
    destBits = (destBits & ~(srcEnableMask)) | (srcBits & (srcEnableMask)) | srcEnable | destEnable;
    currentContextState.setL(STATE_EVENT, destBits);

    bool val;
    if ((val = src.isOverRange()) != _NULL) { if (isNull(val)) totalOverRangePackets = 0; totalOverRangePackets += (val == _TRUE ? 1 : 0); }
    if ((val = src.isDiscontinuous()) != _NULL) { if (isNull(val)) totalDiscontinuousPackets = 0; totalDiscontinuousPackets += (val == _TRUE ? 1 : 0); }

    if (updated) {
      lastChanged = lastUpdated;
    }
  }

  return false;
}

bool BasicVRTState::updateState(const TimeStamp& ts) {
  currentContextState.setTimeStamp(ts);
  return false;
}

bool BasicVRTState::updateState(const BasicContextPacket& ctx) {
  lastUpdated = ctx.getTimeStamp();
  latestContext = ctx;

  if (ctx.isChangePacket() == false && initialized) return false; // NOTHING CHANGED, ASSUME THIS IS CORRECT

  bool triggered = false;

  // GET CONTEXT FIELDS
  int32_t cif = ctx.getContextIndicatorField();
  for (int i=31, j=0x1<<31; i>=0; i--, j=0x1<<i) {
    int32_t field = j&cif;
    if (field > 0){
      int8_t copied = copy(currentContextState, ctx, field);
      bool upd  = (copied & UPDATED  ) != 0;
      bool trig = (copied & TRIGGERED) != 0;
      triggered = triggered || trig;
      if (upd) {
        // Just in case sender did not transmit CHANGE_IND
        lastChanged = ctx.getTimeStamp();
      }
    }

    switch (field) {
    case CHANGE_IND:
    {
      lastChanged = ctx.getTimeStamp();
      break;
    }
    case OVER_RANGE:
    {
      if (isNull(totalOverRangeSamples)) totalOverRangeSamples = 0;
      totalOverRangeSamples += ctx.getOverRangeCount();
      break;
    }
    case STATE_EVENT:
    {
      boolNull val;
      if ((val = ctx.isOverRange()) != _NULL) { if (isNull(val)) totalOverRangePackets = 0; totalOverRangePackets += (val == _TRUE ? 1 : 0); }
      if ((val = ctx.isDiscontinuous()) != _NULL) { if (isNull(val)) totalDiscontinuousPackets = 0; totalDiscontinuousPackets += (val == _TRUE ? 1 : 0); }
      break;
    }
    }
  }
  initialized = true;
  return triggered;
}


string BasicVRTState::toString () const {
  ostringstream str;
  Utilities::append(str, " ReferencePointIdentifier=",getReferencePointIdentifier());
  Utilities::append(str, " Bandwidth="               ,getBandwidth()         ,"Hz" );
  Utilities::append(str, " FrequencyIF="             ,getFrequencyIF()       ,"Hz" );
  Utilities::append(str, " FrequencyRF="             ,getFrequencyRF()       ,"Hz" );
  Utilities::append(str, " FrequencyOffsetRF="       ,getFrequencyOffsetRF() ,"Hz" );
  Utilities::append(str, " BandOffsetIF="            ,getBandOffsetIF()      ,"Hz" );
  Utilities::append(str, " ReferenceLevel="          ,getReferenceLevel()    ,"dBm");
  Utilities::append(str, " Gain1="                   ,getGain1()             ,"dB" );
  Utilities::append(str, " Gain2="                   ,getGain2()             ,"dB" );
  Utilities::append(str, " SampleRate="              ,getSampleRate()        ,"Hz" );
  Utilities::append(str, " TimeStampAdjustment="     ,getTimeStampAdjustment()     );
  Utilities::append(str, " TimeStampCalibration="    ,getTimeStampCalibration()    );
  Utilities::append(str, " Temperature="             ,getTemperature()             );
  Utilities::append(str, " DeviceID="                ,getDeviceID()                );
  Utilities::append(str, " CalibratedTimeStamp="     ,isCalibratedTimeStamp()      );
  Utilities::append(str, " DataValid="               ,isDataValid()                );
  Utilities::append(str, " ReferenceLocked="         ,isReferenceLocked()          );
  Utilities::append(str, " AGC="                     ,isAutomaticGainControl()     );
  Utilities::append(str, " SignalDetected="          ,isSignalDetected()           );
  Utilities::append(str, " InvertedSpectrum="        ,isInvertedSpectrum()         );
  Utilities::append(str, " UserDefinedBits="         ,getUserDefinedBits()         );
  Utilities::append(str, " DeviceIdentifier="                ,getDeviceIdentifier());
  Utilities::append(str, " TotalOverRangePackets="   ,getTotalOverRangePackets()   );
  Utilities::append(str, " TotalOverRangeSamples="   ,getTotalOverRangeSamples()   );
  Utilities::append(str, " TotalDiscontinuousPackets=",getTotalDiscontinuousPackets());
  Utilities::append(str, " DataPayloadFormat={"      ,getDataPayloadFormat()   ,"}");
  Utilities::append(str, " GeolocationGPS={"         ,getGeolocationGPS()      ,"}");
  Utilities::append(str, " GeolocationINS={"         ,getGeolocationINS()      ,"}");
  Utilities::append(str, " EphemerisECEF={"          ,getEphemerisECEF()       ,"}");
  Utilities::append(str, " EphemerisRelative={"      ,getEphemerisRelative()   ,"}");
  Utilities::append(str, " EphemerisReference={"     ,getEphemerisReference()  ,"}");
  Utilities::append(str, " GeoSentences={"           ,getGeoSentences()        ,"}");
  Utilities::append(str, " ContextAssocLists={"      ,getContextAssocLists()   ,"}");
  return str.str();
}
}


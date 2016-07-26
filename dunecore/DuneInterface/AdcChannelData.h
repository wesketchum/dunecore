// AdcChannelData.h
//
// David Adams
// June 2016
//
// Struct to hold the prepared time samples for single TPC channel.
//
//    channel - Offline channel number
//   pedestal - Pedestal subtracted from the raw count
//        raw - Uncompressed array holding the raw ADC count for each tick
//    samples - Array holding the prepared signal value for each tick
//      flags - Array holding the status flag for each tick
//     signal - Array holding bools indicating which ticks have signals
//       rois - Array of ROIs indicating which ticks have signals
//      digit - Corresponding raw digit
//       wire - Correponding wire

#ifndef AdcChannelData_H
#define AdcChannelData_H

#include <map>
#include "dune/DuneInterface/AdcTypes.h"

namespace raw {
  class RawDigit;
}
namespace recob {
  class Wire;
}

class AdcChannelData {

public:

  AdcChannel channel =-1;
  AdcSignal pedestal = -99999;
  AdcCountVector raw;
  AdcSignalVector samples;
  AdcFlagVector flags;
  AdcFilterVector signal;
  AdcRoiVector rois;
  const raw::RawDigit* digit =nullptr;
  const recob::Wire* wire =nullptr;

};

typedef std::map<AdcChannel, AdcChannelData> AdcChannelDataMap;

#endif

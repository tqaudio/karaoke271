#pragma once

#include "pluginterfaces/base/ustring.h"
#include "public.sdk/source/vst/vsteditcontroller.h"

using namespace Steinberg;
using namespace Steinberg::Vst;

namespace Karaoke271 {
enum Parameters : ParamID {
  kBypassId = 100,
  kLeftLChannelId = 101,
  kLeftRChannelId = 102,
  kRightLChannelId = 103,
  kRightRChannelId = 104,
  kMidSideBalanceId = 105
};

struct AutomationParameter {
  ParamValue value = 0.0;
  bool hasChanged = false;
};

class ToggleParameter : public Parameter {
public:
  ToggleParameter(const char *title, int32 flags, int32 id);
  void toString(ParamValue normValue, String128 string) const SMTG_OVERRIDE;
  bool fromString(const TChar *string,
                  ParamValue &normValue) const SMTG_OVERRIDE;
};

class LinearParameter : public Parameter {
public:
  LinearParameter(const char *title, const char *unit, double maxValue,
                  int32 precision, int32 flags, int32 id);
  void toString(ParamValue normValue, String128 text) const SMTG_OVERRIDE;
  bool fromString(const TChar *string,
                  ParamValue &normValue) const SMTG_OVERRIDE;

private:
  double mMaxValue;
  int32 mPrecision;
};
} // namespace Karaoke271

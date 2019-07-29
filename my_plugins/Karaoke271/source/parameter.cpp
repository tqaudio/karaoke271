#include <math.h>

#include "../include/parameter.h"

namespace Karaoke271 {
ToggleParameter::ToggleParameter(const char *title, int32 flags, int32 id) {
  UString(info.title, USTRINGSIZE(info.title)).fromAscii(title);
  info.flags = flags;
  info.id = id;
  info.stepCount = 2;
  info.unitId = kRootUnitId;
}

void ToggleParameter::toString(ParamValue normValue, String128 text) const {
  if (normValue == 0.0) {
    UString(text, 128).fromAscii("OFF");
  } else {
    UString(text, 128).fromAscii("ON");
  }
}

bool ToggleParameter::fromString(const TChar *text,
                                 ParamValue &normValue) const {
  UString wrapper((TChar *)text, -1);

  return wrapper.scanFloat(normValue);
}

LinearParameter::LinearParameter(const char *title, const char *unit,
                                 double maxValue, int32 precision, int32 flags,
                                 int32 id)
    : mMaxValue(maxValue), mPrecision(precision) {
  UString(info.title, USTRINGSIZE(info.title)).fromAscii(title);
  UString(info.units, USTRINGSIZE(info.units)).fromAscii(unit);
  info.flags = flags;
  info.id = id;
  info.stepCount = 0;
  info.unitId = kRootUnitId;
}

void LinearParameter::toString(ParamValue normValue, String128 text) const {
  UString(text, 128).printFloat(normValue * mMaxValue, mPrecision);
}

bool LinearParameter::fromString(const TChar *text,
                                 ParamValue &normValue) const {
  double value{0.0};
  UString wrapper((TChar *)text, -1);

  if (!wrapper.scanFloat(value)) {
    return false;
  }
  if (value < 0.0) {
    value = 0.0;
  }
  if (value > mMaxValue) {
    value = mMaxValue;
  }

  normValue = value / mMaxValue;

  return true;
}
} // namespace Karaoke271

#include "../include/controller.h"

namespace Karaoke271 {
tresult PLUGIN_API Controller::initialize(FUnknown *context) {
  tresult result = EditController::initialize(context);

  if (result != kResultOk) {
    return result;
  }

  parameters.addParameter(STR16("Bypass"), 0, 1, 0,
                          ParameterInfo::kCanAutomate |
                              ParameterInfo::kIsBypass,
                          Parameters::kBypassId);

  ToggleParameter *leftL =
      new ToggleParameter("Left L channel", ParameterInfo::kCanAutomate,
                          Parameters::kLeftLChannelId);
  parameters.addParameter(leftL);

  ToggleParameter *leftR =
      new ToggleParameter("Left R channel", ParameterInfo::kCanAutomate,
                          Parameters::kLeftRChannelId);
  parameters.addParameter(leftR);

  ToggleParameter *rightL =
      new ToggleParameter("Right L channel", ParameterInfo::kCanAutomate,
                          Parameters::kRightLChannelId);
  parameters.addParameter(rightL);

  ToggleParameter *rightR =
      new ToggleParameter("Right R channel", ParameterInfo::kCanAutomate,
                          Parameters::kRightRChannelId);
  parameters.addParameter(rightR);

  LinearParameter *midSideBalance = new LinearParameter(
      "Mid Side Balance", "", 100.0, 2, ParameterInfo::kCanAutomate,
      Parameters::kMidSideBalanceId);
  parameters.addParameter(midSideBalance);

  return result;
}

tresult PLUGIN_API Controller::setComponentState(IBStream *state) {
  if (!state) {
    return kResultFalse;
  }

  IBStreamer streamer(state, kLittleEndian);
  int32 savedBypass;
  int32 savedLeftL;
  int32 savedLeftR;
  int32 savedRightL;
  int32 savedRightR;
  float savedMidSideBalance;

  if (!streamer.readInt32(savedBypass)) {
    return kResultFalse;
  }
  if (!streamer.readInt32(savedLeftL)) {
    return kResultFalse;
  }
  if (!streamer.readInt32(savedLeftR)) {
    return kResultFalse;
  }
  if (!streamer.readInt32(savedRightL)) {
    return kResultFalse;
  }
  if (!streamer.readInt32(savedRightR)) {
    return kResultFalse;
  }
  if (!streamer.readFloat(savedMidSideBalance)) {
    return kResultFalse;
  }

  setParamNormalized(Parameters::kBypassId, savedBypass ? 1 : 0);
  setParamNormalized(Parameters::kLeftLChannelId, savedLeftL ? 1 : 0);
  setParamNormalized(Parameters::kLeftRChannelId, savedLeftR ? 1 : 0);
  setParamNormalized(Parameters::kRightLChannelId, savedRightL ? 1 : 0);
  setParamNormalized(Parameters::kRightRChannelId, savedRightR ? 1 : 0);
  setParamNormalized(Parameters::kMidSideBalanceId, savedMidSideBalance);

  return kResultOk;
}

IPlugView *PLUGIN_API Controller::createView(const char *name) {
  if (name && strcmp(name, "editor") == 0) {
    auto *view = new VST3Editor(this, "Karaoke271", "main.uidesc");
    return view;
  }
  return nullptr;
}
} // namespace Karaoke271

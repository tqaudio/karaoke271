#include "../include/processor.h"

namespace Karaoke271 {
Processor::Processor()
    : mLeftL(true), mRightL(false), mRightR(true), mLeftR(false),
      mMidSideBalance(DEFAULT_MID_SIDE_BALANCE_NORMALIZED) {
  setControllerClass(ControllerID);
}

tresult PLUGIN_API Processor::initialize(FUnknown *context) {
  tresult result = AudioEffect::initialize(context);

  if (result != kResultTrue) {
    return kResultFalse;
  }

  addAudioInput(STR16("AudioInput"), SpeakerArr::kStereo);
  addAudioOutput(STR16("AudioOutput"), SpeakerArr::kStereo);

  return kResultTrue;
}

tresult PLUGIN_API Processor::setBusArrangements(SpeakerArrangement *inputs,
                                                 int32 numIns,
                                                 SpeakerArrangement *outputs,
                                                 int32 numOuts) {
  if (numIns == 1 && numOuts == 1 && inputs[0] == outputs[0]) {
    return AudioEffect::setBusArrangements(inputs, numIns, outputs, numOuts);
  }

  return kResultFalse;
}

tresult PLUGIN_API Processor::setupProcessing(ProcessSetup &newSetup) {
  return AudioEffect::setupProcessing(newSetup);
}

tresult PLUGIN_API Processor::setActive(TBool state) {
  SpeakerArrangement arrangement;

  if (getBusArrangement(kOutput, 0, arrangement) != kResultTrue) {
    return kResultFalse;
  }

  int channelCount = SpeakerArr::getChannelCount(arrangement);

  if (channelCount != 2) {
    return kResultFalse;
  }
  if (state) {
    mMidSideBalances = new AutomationParameter[processSetup.maxSamplesPerBlock];
  } else {
    delete mMidSideBalances;
  }

  return AudioEffect::setActive(state);
}

tresult PLUGIN_API Processor::process(ProcessData &data) {
  if (data.inputParameterChanges) {
    int32 numParamsChanged = data.inputParameterChanges->getParameterCount();

    for (int32 index = 0; index < numParamsChanged; index++) {
      IParamValueQueue *paramQueue =
          data.inputParameterChanges->getParameterData(index);

      if (paramQueue == nullptr) {
        continue;
      }

      int32 numPoints = paramQueue->getPointCount();
      int32 sampleOffset;
      ParamValue value;

      if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) !=
          kResultTrue) {
        continue;
      }
      if (sampleOffset < 0) {
        sampleOffset = 0;
      }
      if (sampleOffset > data.numSamples - 1) {
        sampleOffset = data.numSamples - 1;
      }
      switch (paramQueue->getParameterId()) {
      case Parameters::kBypassId:
        mBypass = (value > 0.5f);
        break;
      case Parameters::kLeftLChannelId:
        mLeftL = (value > 0.5f);
        break;
      case Parameters::kLeftRChannelId:
        mLeftR = (value > 0.5f);
        break;
      case Parameters::kRightLChannelId:
        mRightL = (value > 0.5f);
        break;
      case Parameters::kRightRChannelId:
        mRightR = (value > 0.5f);
        break;
      case Parameters::kMidSideBalanceId:
        mMidSideBalance = value;
        mMidSideBalances[sampleOffset].value = value;
        mMidSideBalances[sampleOffset].hasChanged = true;
        break;
      }
    }
  }
  if (data.numInputs == 0 || data.numOutputs == 0) {
    return kResultOk;
  }
  if (data.numSamples > 0) {
    SpeakerArrangement arrangement;
    getBusArrangement(kOutput, 0, arrangement);
    int32 channelCount = SpeakerArr::getChannelCount(arrangement);

    for (int32 sample = 0; sample < data.numSamples; sample++) {
      float *inputLChannel = data.inputs[0].channelBuffers32[0];
      float *inputRChannel = data.inputs[0].channelBuffers32[1];
      float *outputLChannel = data.outputs[0].channelBuffers32[0];
      float *outputRChannel = data.outputs[0].channelBuffers32[1];

      double lSignal = 0.0f;
      double rSignal = 0.0f;

      if (mLeftL && mLeftR) {
        lSignal = (inputLChannel[sample] + inputRChannel[sample]) / 2.0f;
      } else if (mLeftL) {
        lSignal = inputLChannel[sample];
      } else if (mLeftR) {
        lSignal = inputRChannel[sample];
      }
      if (mRightL && mRightR) {
        rSignal = (inputLChannel[sample] + inputRChannel[sample]) / 2.0f;
      } else if (mRightL) {
        rSignal = inputLChannel[sample];
      } else if (mRightR) {
        rSignal = inputRChannel[sample];
      }

      double mid = (inputLChannel[sample] + inputRChannel[sample]) / 2.0f;
      double lSide = lSignal - mid;
      double rSide = rSignal - mid;

      if (mMidSideBalance < 0.5f) {
        outputLChannel[sample] = 2.0f * mMidSideBalance * lSignal +
                                 2.0f * (0.5f - mMidSideBalance) * mid;
        outputRChannel[sample] = 2.0f * mMidSideBalance * rSignal +
                                 2.0f * (0.5f - mMidSideBalance) * mid;
      } else if (mMidSideBalance > 0.5f) {
        outputLChannel[sample] = 2.0f * (1.0f - mMidSideBalance) * lSignal +
                                 4.0f * (mMidSideBalance - 0.5f) * lSide;
        outputRChannel[sample] = 2.0f * (1.0f - mMidSideBalance) * rSignal +
                                 4.0f * (mMidSideBalance - 0.5f) * rSide;
      } else {
        outputLChannel[sample] = lSignal;
        outputRChannel[sample] = rSignal;
      }
    }
  }

  return kResultOk;
}

tresult PLUGIN_API Processor::setState(IBStream *state) {
  if (!state) {
    return kResultFalse;
  }

  IBStreamer streamer(state, kLittleEndian);
  int32 savedBypass = 0;
  int32 savedLeftL = 0;
  int32 savedLeftR = 0;
  int32 savedRightL = 0;
  int32 savedRightR = 0;
  float savedMidSideBalance = 0.0f;

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

  mBypass = savedBypass > 0;
  mLeftL = savedLeftL > 0;
  mLeftR = savedLeftR > 0;
  mRightL = savedRightL > 0;
  mRightR = savedRightR > 0;
  mMidSideBalance = savedMidSideBalance;

  return kResultOk;
}

tresult PLUGIN_API Processor::getState(IBStream *state) {
  IBStreamer streamer(state, kLittleEndian);
  int32 saveBypass = mBypass ? 1 : 0;
  int32 saveLeftL = mLeftL ? 1 : 0;
  int32 saveLeftR = mLeftR ? 1 : 0;
  int32 saveRightL = mRightL ? 1 : 0;
  int32 saveRightR = mRightR ? 1 : 0;
  float saveMidSideBalance = mMidSideBalance;

  streamer.writeInt32(saveBypass);
  streamer.writeInt32(saveLeftL);
  streamer.writeInt32(saveLeftR);
  streamer.writeInt32(saveRightL);
  streamer.writeInt32(saveRightR);
  streamer.writeFloat(saveMidSideBalance);

  return kResultOk;
}
} // namespace Karaoke271

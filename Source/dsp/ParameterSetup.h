#pragma once
#include "Bones/ToneStack.h"
#include "dsp/NoiseGate.h"

struct ParameterSetup
{
    recursive_linear_filter::BiquadParams mBassParams{ 44100., 1., 0., 0. };
    recursive_linear_filter::BiquadParams mMidParams{ 44100., 1., 0., 0. };
    recursive_linear_filter::BiquadParams mHighParams{ 44100., 1., 0., 0. };
    dsp::noise_gate::TriggerParams mNoiseGateParams{ 0.1, 0., 1., 0., 0.1, 0. };
}; 
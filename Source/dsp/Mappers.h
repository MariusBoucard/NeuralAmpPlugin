#include "Bones/ToneStack.h"


class Mappers
{
private:
	Mappers()
		: mSampleRate(44100.0) {} 
	Mappers(const Mappers&) = delete; 
	Mappers& operator=(const Mappers&) = delete;

public:
	static Mappers& getMapperInstance()
	{
		static Mappers instance;
		return instance;
	}

	static void setSampleRate(double sampleRate)
	{
		getMapperInstance().mSampleRate = sampleRate;
	}


	static void setToneStackBass(recursive_linear_filter::BiquadParams& inBassSetup, const double val) {
		const double sampleRate = getMapperInstance().mSampleRate; 
		const double bassGainDB = 10 * (val - 0.5); // +/- 10
		const double bassFrequency = 400.0;
		const double bassQuality = 0.707;
		recursive_linear_filter::BiquadParams bassParams(sampleRate, bassFrequency, bassQuality, bassGainDB);
		inBassSetup = bassParams;
	}

	static void setToneStackMid(recursive_linear_filter::BiquadParams& inMidSetup, const double val) {

		const double sampleRate = getMapperInstance().mSampleRate;
		const double midGainDB = 10 * (val - 0.5); // +/- 10
		const double midFrequency = 425.0;
		const double midQuality = midGainDB < 0.0 ? 1.5 : 0.7;
		recursive_linear_filter::BiquadParams midParams(sampleRate, midFrequency, midQuality, midGainDB);
		inMidSetup = midParams;
	}
	static void setToneStackHigh(recursive_linear_filter::BiquadParams& inHighSetup, const double val) 
	{
		const double sampleRate = getMapperInstance().mSampleRate; 
		const double trebleGainDB = 10 * (val - 0.5); // +/- 10
		const double trebleFrequency = 1800.0;
		const double trebleQuality = 0.707;
		recursive_linear_filter::BiquadParams trebleParams(sampleRate, trebleFrequency, trebleQuality, trebleGainDB);
		inHighSetup = trebleParams;
	}
	static void setNoiseGateParams(dsp::noise_gate::TriggerParams& inTriggerParams, const double val)
	{
		const double time = 0.01;
		const double threshold = val * (-160);
		const double ratio = 0.1;
		const double openTime = 0.005;
		const double holdTime = 0.01;
		const double closeTime = 0.05;
		const dsp::noise_gate::TriggerParams triggerParams(time, threshold, ratio, openTime, holdTime, closeTime);
		inTriggerParams = triggerParams;
	}

private:
	double mSampleRate;
};

/*
  ==============================================================================

    AmpAudioProcessor.cpp
    Created: 18 Apr 2025 6:14:04pm
    Author:  Marius

  ==============================================================================
*/

#include "Processor.h"

#include <filesystem>
#include <iostream>


juce::File SkeletonAudioProcessor::createJucePathFromFile(const juce::String& filePath)
{
    juce::File file(filePath);
    if (!file.existsAsFile() and !file.exists())
    {
        DBG("File does not exist: " << filePath);
        return juce::File();
    }


    return file;
}

SkeletonAudioProcessor::SkeletonAudioProcessor(juce::AudioProcessorValueTreeState& inParameters, ParameterSetup& inParameterSetup)
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::mono())
        .withOutput("Output", juce::AudioChannelSet::stereo()))
    , mParameters(inParameters)
    , mParameterSetup(inParameterSetup)
    , mBlockSize(256)
    , mSampleRate(44100)
    , mToneStack(new dsp::tone_stack::BasicNamToneStack(mParameterSetup))
    , mNoiseGateTrigger(new dsp::noise_gate::Trigger())
    , mNoiseGateGain(new dsp::noise_gate::Gain())
    , mParamListener(*this,mToneStack, mNoiseGateGain, mNoiseGateTrigger, mParameterSetup)
    , mIsNAMEnabled(false)
    , mIRPath()
    , mIRVerbPath()
    , mIsIRLoading(true)
    , mIsIRVerbLoading(true)
    , mNAMPath()
    , mDirectNAMPath()
    , mDirectIRPath()
    , mDirectIRVerbPath()
{
    mNoiseGateTrigger->AddListener(mNoiseGateGain);

    setRateAndBufferSizeDetails(mSampleRate, mBlockSize);

    if (!mFloatBuffer)
    {
        mFloatBuffer = new float* [getNumInputChannels()];
        for (int channel = 0; channel < getNumInputChannels(); ++channel)
        {
            mFloatBuffer[channel] = new float[1024];
        }
    }
    if (!mTempFloatBuffer)
    {
        mTempFloatBuffer = new float* [getNumInputChannels()];
        for (int channel = 0; channel < getNumInputChannels(); ++channel)
        {
            mTempFloatBuffer[channel] = new float[1024];
        }
    }

    mParameters.addParameterListener("bass", &mParamListener);
    mParameters.addParameterListener("mid", &mParamListener);
    mParameters.addParameterListener("high", &mParamListener);
    mParameters.addParameterListener("gate", &mParamListener);

    mToneStack->Reset(getSampleRate(), getBlockSize());

}
SkeletonAudioProcessor::~SkeletonAudioProcessor()
{
    for (int channel = 0; channel < getNumInputChannels(); ++channel)
    {
        delete[] mFloatBuffer[channel];
        delete[] mTempFloatBuffer[channel];
    }
    delete[] mFloatBuffer;
    delete[] mTempFloatBuffer;
}  
void SkeletonAudioProcessor::updateMeter(bool isOutput, juce::AudioBuffer<float>& buffer,int numSamples,int numChannels)
{
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = buffer.getReadPointer(channel);
        float sum = 0.0f;

        for (int i = 0; i < numSamples; ++i)
        {
            sum += channelData[i] * channelData[i];
        }

        float rms = std::sqrt(sum / numSamples);

        if (channel == 0)
        {
            mRmsLevelLeft.store(rms);
            mRmsLevelRight.store(rms);
        }
    }
}
void SkeletonAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    buffer.applyGain(mParameters.getParameterAsValue("input").getValue());

    int isMono = 1;

    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    updateMeter(false, buffer, numSamples, isMono);
    

    if (!mFloatBuffer)
    {
        mFloatBuffer = new float* [isMono];
        mTempFloatBuffer = new float* [isMono];
        for (int channel = 0; channel < isMono; ++channel)
        {
            mFloatBuffer[channel] = new float[1024];
            mTempFloatBuffer[channel] = new float[1024];
        }
    }

    if (!mDoubleBuffer)
    {
        mDoubleBuffer = new double* [isMono];
        mTempDoubleBuffer = new double* [isMono];
        mVerbDoubleBuffer = new double* [isMono];
        for (int channel = 0; channel < isMono; ++channel)
        {
            mDoubleBuffer[channel] = new double[1024];
            mTempDoubleBuffer[channel] = new double[1024];
            mVerbDoubleBuffer[channel] = new double[1024];
        }
    }
    for (int channel = 0; channel < isMono; ++channel)
    {
        auto* floatData = buffer.getReadPointer(channel);

        std::copy(floatData, floatData + numSamples, mFloatBuffer[channel]);
    }


    for (int channel = 0; channel < isMono; ++channel)
    {
        std::transform(mFloatBuffer[channel], mFloatBuffer[channel] + numSamples, mDoubleBuffer[channel], [](float sample) {
            return static_cast<double>(sample);
            });
    }

    mTempDoubleBuffer = mNoiseGateTrigger->Process(mDoubleBuffer, isMono, numSamples);

    if (mIsNAMEnabled and mParameters.getParameterAsValue("namEnabled").getValue())
    {
        const int maxBlockSize = mBlockSize;
        for (int startSample = 0; startSample < numSamples; startSample += maxBlockSize)
        {
            const int blockSize = std::min(maxBlockSize, numSamples - startSample);


            for (int channel = 0; channel < isMono; ++channel)
            {
                mModel->process(mTempDoubleBuffer[channel] + startSample, mDoubleBuffer[channel] + startSample, blockSize);
            }
        }
    }
    else
    {
        for (int channel = 0; channel < isMono; ++channel)
        {
            std::copy(mTempDoubleBuffer[channel], mTempDoubleBuffer[channel] + numSamples, mDoubleBuffer[channel]);
        }
    }

    
    mTempDoubleBuffer = mNoiseGateGain->Process(mDoubleBuffer, isMono, numSamples);



    mDoubleBuffer = mToneStack->Process(mTempDoubleBuffer, isMono, numSamples);

    if (mIR != nullptr and mParameters.getParameterAsValue("irEnabled").getValue() and ! mIsIRLoading) 
        mTempDoubleBuffer = mIR->Process(mDoubleBuffer, isMono, numSamples);
    else
    {
        for (int channel = 0; channel < isMono; ++channel)
        {
            std::copy(mDoubleBuffer[channel], mDoubleBuffer[channel] + numSamples, mTempDoubleBuffer[channel]);
        }
    }

    if (mIRVerb != nullptr and mParameters.getParameterAsValue("irVerbEnabled").getValue() and ! mIsIRVerbLoading)
    {
        for (int channel = 0; channel < isMono; ++channel)
        {
            std::copy(mTempDoubleBuffer[channel], mTempDoubleBuffer[channel] + numSamples, mVerbDoubleBuffer[channel]);
        }


        mDoubleBuffer = mIRVerb->Process(mTempDoubleBuffer, isMono, numSamples);
        float mix = mParameters.getParameterAsValue("irVerbMix").getValue();

        for (int channel = 0; channel < isMono; ++channel)
        {
            for (int sample = 0; sample < numSamples; ++sample)
            {
                mDoubleBuffer[channel][sample] = (1.0f - mix) * mVerbDoubleBuffer[channel][sample] +
                    mix * mDoubleBuffer[channel][sample];
            }
        }

    }
    else
    {
        for (int channel = 0; channel < isMono; ++channel)
        {
            std::copy(mTempDoubleBuffer[channel], mTempDoubleBuffer[channel] + numSamples, mDoubleBuffer[channel]);
        }
    }

    for (int channel = 0; channel < isMono; ++channel)
    {
        std::transform(mDoubleBuffer[channel], mDoubleBuffer[channel] + numSamples, mTempFloatBuffer[channel], [](double sample) {
            return static_cast<float>(sample);
            });
    }
    for (int channel = 0; channel < isMono; ++channel)
    {
        auto* floatData = buffer.getWritePointer(channel);
        std::copy(mTempFloatBuffer[channel], mTempFloatBuffer[channel] + numSamples, floatData);
    }

    bool mMonoToStereo = true;
    if (mMonoToStereo && numChannels > 1)
    {
        auto* floatData = buffer.getWritePointer(1);
        auto* monoData = buffer.getReadPointer(0);
        std::copy(monoData, monoData + numSamples, floatData);
    }


    buffer.applyGain(mParameters.getParameterAsValue("output").getValue());

    updateMeter(true, buffer, numSamples, numChannels); 
}

void SkeletonAudioProcessor::loadImpulseResponse(const juce::File& inIRFile)
{
    mIsIRLoading = true;

    stageIR(inIRFile);
    if (mStagedIR != nullptr)
    {
        mIsIRLoading = true;
        mIR = std::move(mStagedIR);
        mStagedIR = nullptr;
    }
    else
    {
        std::cerr << "Failed to load impulse response." << std::endl;
    }
    mIsIRLoading = false;

}

void SkeletonAudioProcessor::loadImpulseResponseVerb(const juce::File& inIRFile)
{
    mIsIRVerbLoading = true;

    stageIRVerb(inIRFile);
    if (mStagedIRVerb != nullptr)
    {
        mIRVerb = std::move(mStagedIRVerb);
        mStagedIRVerb = nullptr;
        mIsIRVerbLoading = false;
    }
    else
    {
        std::cerr << "Failed to load impulse response." << std::endl;
    }
}

void SkeletonAudioProcessor::loadNAMFile(const juce::File& inNAMFile)
{
    try
    {
        mIsNAMEnabled = false;
        mModel = nam::get_dsp(std::filesystem::path(inNAMFile.getFullPathName().toStdString()));
        mModel->ResetAndPrewarm(mSampleRate, mBlockSize);
        setDirectNAMPath(inNAMFile);
        mIsNAMEnabled = true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error loading NAM file: " << e.what() << std::endl;
    }
}

dsp::wav::LoadReturnCode SkeletonAudioProcessor::stageIR(const juce::File& irPath)
{
    juce::File previousIRPath = mDirectIRPath;
    const double sampleRate = getSampleRate();
    dsp::wav::LoadReturnCode wavState = dsp::wav::LoadReturnCode::ERROR_OTHER;
    try
    {
        juce::String irPathString = irPath.getFullPathName();
        const char* irPathChar = irPathString.toRawUTF8();

        mStagedIR = std::make_unique<dsp::ImpulseResponse>(irPathChar, sampleRate);
        wavState = mStagedIR->GetWavState();
    }
    catch (std::runtime_error& e)
    {
        wavState = dsp::wav::LoadReturnCode::ERROR_OTHER;
        std::cerr << "Caught unhandled exception while attempting to load IR:" << std::endl;
        std::cerr << e.what() << std::endl;
    }

    if (wavState == dsp::wav::LoadReturnCode::SUCCESS)
    {
        setDirectIRPath(irPath);
    }
    else
    {
        if (mStagedIR != nullptr)
        {
            mStagedIR = nullptr;
        }
        setDirectIRPath(previousIRPath);
    }

    return wavState;
}

dsp::wav::LoadReturnCode SkeletonAudioProcessor::stageIRVerb(const juce::File& irPath)
{
    juce::File previousIRPath = mDirectIRVerbPath;
    const double sampleRate = getSampleRate();
    dsp::wav::LoadReturnCode wavState = dsp::wav::LoadReturnCode::ERROR_OTHER;
    try
    {
        juce::String irPathString = irPath.getFullPathName();;
        const char* irPathChar = irPathString.toRawUTF8();

        mStagedIRVerb = std::make_unique<dsp::ImpulseResponse>(irPathChar, sampleRate);
        wavState = mStagedIRVerb->GetWavState();
    }
    catch (std::runtime_error& e)
    {
        wavState = dsp::wav::LoadReturnCode::ERROR_OTHER;
        std::cerr << "Caught unhandled exception while attempting to load IR:" << std::endl;
        std::cerr << e.what() << std::endl;
    }

    if (wavState == dsp::wav::LoadReturnCode::SUCCESS)
    {
        setDirectIRVerbPath(irPath);
    }
    else
    {
        if (mStagedIRVerb != nullptr)
        {
            mStagedIRVerb = nullptr;
        }
        setDirectIRVerbPath(previousIRPath);
    }

    return wavState;
}
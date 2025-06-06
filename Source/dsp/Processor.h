#pragma once
#include <JuceHeader.h>
#include "NAM/dsp.h"
#include "Bones/ToneStack.h"
#include "dsp/NoiseGate.h"
#include "dsp/ImpulseResponse.h"
#include "NAM/get_dsp.h"
#include "ParameterSetup.h"
#include "Mappers.h"

#include "Processor.hpp"
//==============================================================================
class SkeletonAudioProcessor final : public AudioProcessor
{
public:

    //==============================================================================
    SkeletonAudioProcessor(juce::AudioProcessorValueTreeState& inParameters, ParameterSetup& inParameterSetup);


    ~SkeletonAudioProcessor() override;

    juce::File createJucePathFromFile(const juce::String& filePath);

    void prepareToPlay(double, int) override
    {
        mSampleRate = getSampleRate();
		mBlockSize = getBlockSize();
        mModel->ResetAndPrewarm(mSampleRate, mBlockSize);
        Mappers::getMapperInstance().setSampleRate(mSampleRate);
    }
    void releaseResources() override {}
    void processBlock(AudioBuffer<float>& buffer, MidiBuffer&) override;
    void updateMeter(bool isOutput, AudioBuffer<float>& buffer, int numSamples, int numChannels);

    //==============================================================================
    AudioProcessorEditor* createEditor() override {
        return nullptr;
    }
    bool hasEditor() const override { return false; }

    //==============================================================================
    const String getName() const override { return "Ballzzy's NAM"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0; }

    //==============================================================================
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const String getProgramName(int) override { return "None"; }
    void changeProgramName(int, const String&) override {}

    void loadImpulseResponse(const juce::File& path);
    void loadImpulseResponseVerb(const juce::File& path);
    void loadNAMFile(const juce::File& path);

    dsp::wav::LoadReturnCode stageIR(const juce::File& path);
    dsp::wav::LoadReturnCode stageIRVerb(const juce::File& path);

    double getRmsLevelLeft() const { return mRmsLevelLeft.load(); }
    double getRmsLevelRight() const { return mRmsLevelRight.load(); }
    double getRmsOutputLevelLeft() const { return mRmsOutputLevelLeft.load(); }
    double getRmsOutputLevelRight() const { return mRmsOutputLevelRight.load(); }

    //==============================================================================
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override
    {
        return true;
        const auto& mainInLayout = layouts.getChannelSet(true, 0);
        const auto& mainOutLayout = layouts.getChannelSet(false, 0);

        return (mainInLayout == mainOutLayout && (!mainInLayout.isDisabled()));
    }

    juce::AudioProcessorValueTreeState& SkeletonAudioProcessor::getCustomParameterTree()
    {
        return mParameters;
    }

    juce::File& getIRPath()
    {
        return mIRPath;
    }
    juce::File& getDirectIRPath()
    {
        return mDirectIRPath;
    }

    juce::File& getIRVerbPath()
    {
        return mIRVerbPath;
    }
    juce::File& getDirectIRVerbPath()
    {
        return mDirectIRVerbPath;
    }
    juce::File& getNAMPath()
    {
        return mNAMPath;
    }
    juce::File& getDirectNAMPath()
    {
        return mDirectNAMPath;
    }

    void setIRPath(const juce::File& path)
    {
        mIRPath = path;
        mParameters.state.setProperty("irPath", path.getFullPathName(), nullptr);
    }
    void setDirectIRPath(const juce::File& path)
    {
        mDirectIRPath = path;
        mParameters.state.setProperty("directIRPath", path.getFullPathName(), nullptr);
    }
    void setNAMPath(const juce::File& path)
    {
        mNAMPath = path;
        mParameters.state.setProperty("namPath", path.getFullPathName(), nullptr);
    }
    void setDirectNAMPath(const juce::File& path)
    {
        mDirectNAMPath = path;
        mParameters.state.setProperty("directNAMPath", path.getFullPathName(), nullptr);
    }

    void setIRVerbPath(const juce::File& path)
    {
        mIRVerbPath = path;
        mParameters.state.setProperty("irVerbPath", path.getFullPathName(), nullptr);
    }
    void setDirectIRVerbPath(const juce::File& path)
    {
        mDirectIRVerbPath = path;
        mParameters.state.setProperty("directIRVerbPath", path.getFullPathName(), nullptr);
    }

    void setStateInformation(const void* data, int sizeInBytes) override
    {}

    juce::File createTemporaryFileFromMemory(const void* binaryData, size_t dataSize, const juce::String& fileName)
    {
        juce::File tempFile = juce::File::getSpecialLocation(juce::File::tempDirectory).getChildFile(fileName);

        tempFile.deleteFile();
        tempFile.create();    

        juce::FileOutputStream outputStream(tempFile);
        if (outputStream.openedOk())
        {
            outputStream.write(binaryData, dataSize);
            outputStream.flush();
        }
        else
        {
            DBG("Failed to create temporary file.");
        }

        return tempFile;
    }

    void loadDefaultIRFile()
	{
        juce::File tempFile = createTemporaryFileFromMemory(BinaryData::HolyGrail_wav, BinaryData::HolyGrail_wavSize, "holygrail.wav");

        if (tempFile.existsAsFile())
        {
            loadImpulseResponse(tempFile);
        }
        else
        {
            DBG("Failed to create temporary file for impulse response.");
        }
	}
    void loadDefaultIRVerb()
    {
        juce::File tempFile = createTemporaryFileFromMemory(BinaryData::room_wav, BinaryData::room_wavSize, "room.wav");

        if (tempFile.existsAsFile())
        {
            loadImpulseResponseVerb(tempFile);
        }
        else
        {
            DBG("Failed to create temporary file for impulse response.");
        }
}
    void loadDefaultNAMFile()
    {
        juce::File tempFile = createTemporaryFileFromMemory(BinaryData::MetalLead_nam, BinaryData::MetalLead_namSize, "metal.nam");

        if (tempFile.existsAsFile())
        {
            loadNAMFile(tempFile);
        }
        else
        {
            DBG("Failed to create temporary file for impulse response.");
        }

    }


    void initState()
    {
        mIRPath = createJucePathFromFile(mParameters.state.getProperty("irPath").toString());
        mIRVerbPath = createJucePathFromFile(mParameters.state.getProperty("irVerbPath").toString());
        mNAMPath = createJucePathFromFile(mParameters.state.getProperty("namPath").toString());

        mDirectNAMPath = createJucePathFromFile(mParameters.state.getProperty("directNAMPath").toString());
        mDirectIRPath = createJucePathFromFile(mParameters.state.getProperty("directIRPath").toString());
        mDirectIRVerbPath = createJucePathFromFile(mParameters.state.getProperty("directIRVerbPath").toString());

        mDirectIRPath.existsAsFile() ? loadImpulseResponse(mDirectNAMPath) : loadDefaultIRFile();
        mDirectIRVerbPath.existsAsFile() ? loadImpulseResponseVerb(mDirectIRVerbPath) : loadDefaultIRVerb();
        mDirectNAMPath.existsAsFile() ? loadNAMFile(mDirectNAMPath) : loadDefaultNAMFile();
        
    }

    void getStateInformation(MemoryBlock& destData) override
    {
	}

private:
    //==============================================================================
    AudioParameterFloat* gain;
    juce::AudioProcessorValueTreeState& mParameters;
    ParameterSetup& mParameterSetup;
    std::unique_ptr<nam::DSP> mModel;
    dsp::tone_stack::AbstractToneStack* mToneStack;
    dsp::noise_gate::Gain* mNoiseGateGain;
    dsp::noise_gate::Trigger* mNoiseGateTrigger;

    std::unique_ptr<dsp::ImpulseResponse> mIR;
    std::unique_ptr<dsp::ImpulseResponse> mStagedIR;
    std::unique_ptr<dsp::ImpulseResponse> mIRVerb;
    std::unique_ptr<dsp::ImpulseResponse> mStagedIRVerb;

    juce::File mIRPath;
    juce::File mIRVerbPath;
    juce::File mNAMPath;

    juce::File mDirectNAMPath;
    juce::File mDirectIRPath;
    juce::File mDirectIRVerbPath;

    bool mIsNAMEnabled;
    bool mIsIRLoading;
    bool mIsIRVerbLoading;

    float** mFloatBuffer = nullptr;
    float** mTempFloatBuffer = nullptr;

    double** mDoubleBuffer = nullptr;
    double** mTempDoubleBuffer = nullptr;
    double** mVerbDoubleBuffer = nullptr;

    ParamListener mParamListener;

private:
    std::atomic<float> mRmsLevelLeft{ 0.0f };
    std::atomic<float> mRmsLevelRight{ 0.0f };
    std::atomic<float> mRmsOutputLevelLeft{ 0.0f };
    std::atomic<float> mRmsOutputLevelRight{ 0.0f };

    double mBlockSize;
    double mSampleRate;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SkeletonAudioProcessor)
};

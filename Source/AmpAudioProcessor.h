
/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

 name:             GainPlugin
 version:          1.0.0
 vendor:           JUCE
 website:          http://juce.com
 description:      Gain audio plugin.

 dependencies:     juce_audio_basics, juce_audio_devices, juce_audio_formats,
                   juce_audio_plugin_client, juce_audio_processors,
                   juce_audio_utils, juce_core, juce_data_structures,
                   juce_events, juce_graphics, juce_gui_basics, juce_gui_extra
 exporters:        xcode_mac, vs2022

 moduleFlags:      JUCE_STRICT_REFCOUNTEDPOINTER=1

 type:             AudioProcessor
 mainClass:        GainProcessor

 useLocalCopy:     1

 END_JUCE_PIP_METADATA

*******************************************************************************/

#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_processors_headless/juce_audio_processors_headless.h>
#include "ui/gui.h"
#include "NAM/dsp.h"
#include "dsp/Bones/ToneStack.h"
#include "dsp/NoiseGate.h"
#include "dsp/ImpulseResponse.h"
#include "NAM/get_dsp.h"
#include "dsp/Processor.h"
#include "dsp/ParameterSetup.h"
class AmpAudioProcessor final : public juce::AudioProcessor
{
public:

    AmpAudioProcessor();
    ~AmpAudioProcessor() override;

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

        params.push_back (std::make_unique<juce::AudioParameterFloat> ("input", "Input", 0.0f, 1.0f, 0.5f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>("gate", "Gate", 0.0f, 1.0f, 0.5f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>("bass", "Bass", 0.0f, 1.0f, 0.5f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>("mid", "Mid", 0.0f, 1.0f, 0.5f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>("high", "High", 0.0f, 1.0f, 0.5f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>("output", "Output", 0.0f, 1.0f, 0.5f));
        params.push_back(std::make_unique<juce::AudioParameterBool>("irEnabled", "IREnabled", false));
        params.push_back(std::make_unique<juce::AudioParameterBool>("namEnabled", "NAMEnabled", true));
        params.push_back(std::make_unique<juce::AudioParameterBool>("irVerbEnabled", "IRVerbEnabled", false));
        params.push_back(std::make_unique<juce::AudioParameterFloat>("irVerbMix", "irVerbMix", 0.0f, 1.0f, 0.5f));

        return { params.begin(), params.end() };
    }

    void prepareToPlay (double, int) override {}
    void releaseResources() override {}

    auto processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &) -> void override;

    juce::AudioProcessorEditor* createEditor() override {
        auto editor = new RootViewComponent(mSkeletonAmpProcessor);
        editor->updatePath();
        return editor;
    }
    bool hasEditor() const override                        { return true;   }

    const juce::String getName() const override                  { return "Ballzzy's NAM"; }
    bool acceptsMidi() const override                      { return false; }
    bool producesMidi() const override                     { return false; }
    double getTailLengthSeconds() const override           { return 0; }

    int getNumPrograms() override                          { return 1; }
    int getCurrentProgram() override                       { return 0; }
    void setCurrentProgram (int) override                  {}
    const juce::String getProgramName (int) override             { return "None"; }
    void changeProgramName (int, const juce::String&) override   {}


    void getStateInformation (juce::MemoryBlock& destData) override
    {
        juce::MemoryOutputStream stream(destData, true);
        mParameters.state.writeToStream(stream);  
    }

    juce::File createJucePathFromFile(const juce::String& filePath);

    void setStateInformation (const void* data, int sizeInBytes) override
    {
        juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);
        auto newState = juce::ValueTree::readFromStream(stream);

        if (newState.isValid())
        {
            mParameters.state = newState;
            mSkeletonAmpProcessor.initState();

            if(getActiveEditor() != nullptr)
			{
                RootViewComponent* rootView = dynamic_cast<RootViewComponent*>(getActiveEditor());
				rootView->updatePath();
			}  
        }
    }

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override
    {
        return true;
        const auto& mainInLayout  = layouts.getChannelSet (true,  0);
        const auto& mainOutLayout = layouts.getChannelSet (false, 0);

        return (mainInLayout == mainOutLayout && (! mainInLayout.isDisabled()));
    }

    juce::AudioProcessorValueTreeState& getCustomParameterTree()
    {
        return mParameters;
    }

private:
    juce::AudioProcessorValueTreeState mParameters; 
    SkeletonAudioProcessor mSkeletonAmpProcessor; 
    ParameterSetup mParameterSetup;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmpAudioProcessor)
};

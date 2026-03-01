/*
  ==============================================================================

    AmpAudioProcessor.cpp
    Created: 18 Apr 2025 6:14:04pm
    Author:  Marius

  ==============================================================================
*/

#include "AmpAudioProcessor.h"

#include <filesystem>
#include <iostream>


juce::File AmpAudioProcessor::createJucePathFromFile(const juce::String& filePath)
{
    juce::File file(filePath); 
    if (!file.existsAsFile() and !file.exists())
    {
        DBG("File does not exist: " << filePath);
        return juce::File(); 
    }


    return file;
}

AmpAudioProcessor::AmpAudioProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::mono())
    . withOutput("Output", juce::AudioChannelSet::stereo()))
    , mParameters(*this, nullptr, "PARAMETERS", createParameterLayout())
    , mParameterSetup()
    , mSkeletonAmpProcessor(mParameters,mParameterSetup)
{

}
AmpAudioProcessor::~AmpAudioProcessor()
{
   
}

void AmpAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& inMidi)
{
  mSkeletonAmpProcessor.processBlock(buffer, inMidi);

}

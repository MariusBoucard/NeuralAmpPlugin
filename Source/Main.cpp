/*
  ==============================================================================

    This file was auto-generated and contains the startup code for a PIP.

  ==============================================================================
*/

#include "AmpAudioProcessor.h"
#include <juce_audio_processors_headless/juce_audio_processors_headless.h>

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AmpAudioProcessor();
}

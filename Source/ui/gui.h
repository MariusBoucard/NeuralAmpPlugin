#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "../looknfeel/StripKnob_look_n_feel.h"
#include "../looknfeel/ToogleButtonLookAndFeel.h"
#include "../looknfeel/TopBarLookAndFeel.h"
#include "components/TopBarComponent.h"

class RootViewComponent : public juce::AudioProcessorEditor
{
public:
    struct KnobLayout {
        struct input {
            int x;
            int y;
            int frameWidth ;
            int frameHeight;
            float ratio;
            int textboxHeight;
            int textboxPadding;
        };
        struct output {
            int x;
            int y;
            int sliderWidth;
            int sliderHeight;
        };
        input inLayout;
        output outLayout;
    };

    static int ROOT_WIDTH;
    static int ROOT_HEIGHT;
    RootViewComponent(juce::AudioProcessor& processor);
    ~RootViewComponent() override;  

    void setSliderAttachement(juce::AudioProcessor& inProcessor);
    void computeKnobLayout(KnobLayout& inKnobLayout)
    {
	    inKnobLayout.outLayout.x = inKnobLayout.inLayout.x;
        inKnobLayout.outLayout.y = inKnobLayout.inLayout.y;
        inKnobLayout.outLayout.sliderWidth = inKnobLayout.inLayout.frameWidth * inKnobLayout.inLayout.ratio;
        inKnobLayout.outLayout.sliderHeight = (inKnobLayout.inLayout.frameHeight + inKnobLayout.inLayout.textboxHeight + 10) * inKnobLayout.inLayout.ratio;
	}

    void handleSelectedFile(const juce::File& file);
    void handleSelectedNAMFile(const juce::File& file);
    void handleSelectedVerbIRFile(const juce::File& file);

    void updatePath();
    void paint(juce::Graphics& g) override;
    void resized() override;

    class MeterComponent : public juce::Component, private juce::Timer
    {
    public:
        MeterComponent(juce::AudioProcessor& processor, bool isInput = true)
            : audioProcessor(processor)
            , isInput(isInput)
        {
            startTimerHz(30);
        }

        void paint(juce::Graphics& g) override;


    private:
        bool isInput;
        juce::AudioProcessor& audioProcessor;

        void timerCallback() override
        {
            repaint();
        }
    };

private:
    juce::Image mImage;
    MeterComponent mInputMeter;
    KnobLayout mInputMeterLayout;

    MeterComponent mOutputMeter;
	KnobLayout mOutputMeterLayout;

    juce::Slider mInputKnob;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mInputAttachment;
    KnobLayout mInputKnobLayout;

    juce::Slider mGateKnob;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mGateKnobAttachment;
    KnobLayout mGateKnobLayout;

    juce::Slider mBassKnob;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mBassKnobAttachment;
    KnobLayout mBassKnobLayout;

    juce::Slider mMidKnob;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mMidKnobAttachment;
    KnobLayout mMidKnobLayout;

    juce::Slider mHighKnob;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mHighKnobAttachment;
    KnobLayout mHighKnobLayout;

    juce::Slider mOutputKnob;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mOutputKnobAttachment;
    KnobLayout mOutputKnobLayout;

    juce::Slider mVerbMixKnob;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mVerbMixKnobAttachment;
    KnobLayout mVerbMixKnobLayout;

    juce::ToggleButton mIRButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> mIRButtonAttachment;
    KnobLayout mIRButtonLayout;

    juce::ToggleButton mIRVerbButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> mIRVerbButtonAttachment;
    KnobLayout mIRVerbButtonLayout;

    juce::ToggleButton mNAMButton;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> mNAMButtonAttachment;
    KnobLayout mNAMButtonLayout;

    juce::ComboBox mFileChooserButton;
    KnobLayout mFileChooserButtonLayout;

    KnobLayout mNAMChooserButtonLayout;
    juce::ComboBox mNAMChooserButton;

    juce::ComboBox mVerbDropDown;
    KnobLayout mVerbDropDownLayout;

    juce::OwnedArray<juce::File> mIRFileList;
    juce::OwnedArray<juce::File> mNAMFileList;
    juce::OwnedArray<juce::File> mVerbFileList;


    TopBarComponent mTopBar;

    KnobLayout mTopBarLayout;

    juce::FileChooser mFileChooser;

    KnobLookAndFeel mKnobLookAndFeel; 
    DropDownLookAndFeel mDropDownLookAndFeel; 
    ToggleButtonLookAndFeel mToggleLookAndFeel;

#include "gui.hpp"
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RootViewComponent)
};

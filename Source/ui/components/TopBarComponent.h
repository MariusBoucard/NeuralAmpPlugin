#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors_headless/juce_audio_processors_headless.h>
#include "../../looknfeel/TopBarLookAndFeel.h"

class TopBarComponent : public juce::Component
{
public:
    TopBarComponent(juce::AudioProcessor& processor)
        : audioProcessor(processor)
    {
        mAboutButton.setLookAndFeel(&mButtonLookNFeel);
        addAndMakeVisible(mAboutButton);
        mAboutButton.setButtonText("About");
        mAboutButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
        mAboutButton.onClick = [this]() { showAboutModal(); };

        mSettingButton.setLookAndFeel(&mButtonLookNFeel);
        addAndMakeVisible(mSettingButton);
        mSettingButton.setButtonText("Setting");
        mSettingButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
        mSettingButton.onClick = [this]() { showSettingsModal(audioProcessor); };

		mAmpNameLabel.setLookAndFeel(&mLabelLookAndFeel);

        addAndMakeVisible(mAmpNameLabel);
        mAmpNameLabel.setText("Ballzzy Audio", juce::dontSendNotification); 
        mAmpNameLabel.setJustificationType(juce::Justification::centred); 
        mAmpNameLabel.setFont(juce::Font(18.0f, juce::Font::bold));

    }

    ~TopBarComponent() override
	{
		mAboutButton.setLookAndFeel(nullptr);
		mSettingButton.setLookAndFeel(nullptr);
		mAmpNameLabel.setLookAndFeel(nullptr);
		setLookAndFeel(nullptr); 
	}

    void paint(juce::Graphics& g) override;

    void resized() override
    {
        auto buttonSize = getHeight();
        mAboutButton.setBounds(0, 0, buttonSize*2, buttonSize);
        mSettingButton.setBounds(getWidth() - buttonSize*2, 0, buttonSize*2, buttonSize);
        mAmpNameLabel.setBounds(buttonSize*2, 0, getWidth() - 2 * buttonSize*2, buttonSize);
    }

    class SettingsModal : public juce::Component
    {
    public:

        enum class DirectoryType{
            NAMDir,
            IRDir,
            VerbIRDir
        } ;

        SettingsModal(juce::AudioProcessor& inProcessor);
     
        ~SettingsModal() override {
            mDirectoryChooserNAMButton.setLookAndFeel(nullptr);
		    mDirectoryChooserIRButton.setLookAndFeel(nullptr);
            mCloseButton.setLookAndFeel(nullptr);
		    mTitleLabel.setLookAndFeel(nullptr);
            mNAMPathLabel.setLookAndFeel(nullptr);
	    	mIRPathLabel.setLookAndFeel(nullptr);
            mIRVerbPathLabel.setLookAndFeel(nullptr);
            mDirectoryChooserIRVerbButton.setLookAndFeel(nullptr);

            setLookAndFeel(nullptr); 
        };

        void paint(juce::Graphics& g) override
        {
            mLookAndFeel.drawComponentBackground(g, *this);
        }

        void resized() override
        {
            auto area = getLocalBounds().reduced(40); 
            mTitleLabel.setBounds(area.removeFromTop(50)); 
            mNAMPathLabel.setBounds(area.removeFromTop(30).reduced(0, 5));
            mDirectoryChooserNAMButton.setBounds(area.removeFromTop(50).reduced(0, 10));

            mIRPathLabel.setBounds(area.removeFromTop(30).reduced(0, 5));
            mDirectoryChooserIRButton.setBounds(area.removeFromTop(50).reduced(0, 10));

            mIRVerbPathLabel.setBounds(area.removeFromTop(30).reduced(0, 5));
            mDirectoryChooserIRVerbButton.setBounds(area.removeFromTop(50).reduced(0, 10));

            mCloseButton.setBounds(area.removeFromTop(50).reduced(0, 10));
        }

        void openDirectoryChooser(DirectoryType inDirectoryType);
        void handleSelectedDirectory(const juce::File& directory, DirectoryType inDirectoryType);

    private:
        juce::AudioProcessor& mProcessor;
        juce::TextButton mDirectoryChooserNAMButton{ "Choose Directory" };
        juce::TextButton mDirectoryChooserIRButton{ "Close" };
        juce::TextButton mDirectoryChooserIRVerbButton{ "Choose Directory for cab IR" };
        juce::TextButton mCloseButton{ "Close" };
        juce::Label mNAMPathLabel{ "No NAM Path Selected" };
        juce::Label mIRPathLabel{ "No IR Path Selected" };
        juce::Label mTitleLabel{"Amplifier Settings"};
        juce::Label mIRVerbPathLabel{ "No IR Path Selected" };

        SettingsModalLookAndFeel mLookAndFeel; 
        ModalButtonDefaultLookAndFeel mModalButtonLookAndFeel;
    };

private:
    void showSettingsModal(juce::AudioProcessor& inProcessor);


private:
    TopBarButtonLookAndFeel mButtonLookNFeel;

    juce::AudioProcessor& audioProcessor;
    juce::TextButton mAboutButton{ "About" };
    juce::TextButton mSettingButton{ "settings" };
    juce::Label mAmpNameLabel{ "Amp Name" };
    BeautifulLabelLookAndFeel mLabelLookAndFeel; 

    void showAboutModal()
    {
        juce::AlertWindow::showMessageBoxAsync(
            juce::AlertWindow::InfoIcon,
            "About",
            "Ballzzy's DSP | NAM - version 0.1",
            "Lets get back to it");
    }


};

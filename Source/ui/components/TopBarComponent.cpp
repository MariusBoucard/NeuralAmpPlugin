#include "TopBarComponent.h"
#include "../../dsp/processor.h"
#include "../gui.h"

void TopBarComponent::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::darkgrey.withAlpha(0.5f));
    juce::Rectangle<float> bounds(0, 0, getWidth(), getHeight());
    g.fillRoundedRectangle(bounds, 5.0f);

    g.setColour(juce::Colours::limegreen);
    g.fillRect(bounds.removeFromBottom(1.0f));
}

void TopBarComponent::showSettingsModal(juce::AudioProcessor& inProcessor)
{
    auto* modal = new SettingsModal(inProcessor);

    if (auto* parentEditor = dynamic_cast<juce::AudioProcessorEditor*>(getParentComponent()))
    {
        parentEditor->addAndMakeVisible(modal);
        modal->setBounds(parentEditor->getLocalBounds().reduced(parentEditor->getWidth() / 4, parentEditor->getHeight() / 8));
    }
    modal->setAlwaysOnTop(true);
    modal->setVisible(true);
    modal->enterModalState(false, nullptr, true);

}

TopBarComponent::SettingsModal::SettingsModal(juce::AudioProcessor& inProcessor)
    : mProcessor(inProcessor)
{
    setLookAndFeel(&mLookAndFeel);

    SkeletonAudioProcessor& processor = dynamic_cast<SkeletonAudioProcessor&>(mProcessor);

    addAndMakeVisible(mTitleLabel);
    mTitleLabel.setText("Amplifier Settings", juce::dontSendNotification); 
    mTitleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    mTitleLabel.setFont(juce::Font(20.0f, juce::Font::bold));
    mTitleLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(mNAMPathLabel);
    juce::File namPath = processor.getNAMPath();
    mNAMPathLabel.setText("Nam path selected : " + namPath.getFullPathName(), juce::dontSendNotification);
    mNAMPathLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    mNAMPathLabel.setJustificationType(juce::Justification::centredLeft);

    addAndMakeVisible(mDirectoryChooserNAMButton);
    mDirectoryChooserNAMButton.setButtonText("Choose Directory for NAM Models");
    mDirectoryChooserNAMButton.setColour(juce::TextButton::buttonColourId, juce::Colours::blue);
    mDirectoryChooserNAMButton.onClick = [this]() { openDirectoryChooser(DirectoryType::NAMDir); };

    // IR Path Label
    addAndMakeVisible(mIRPathLabel);
    juce::File irPath = processor.getIRPath();
    mIRPathLabel.setText("IR path selected : " + irPath.getFullPathName(), juce::dontSendNotification);
    mIRPathLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    mIRPathLabel.setJustificationType(juce::Justification::centredLeft);

    addAndMakeVisible(mDirectoryChooserIRButton);
    mDirectoryChooserIRButton.setButtonText("Choose Directory for cab IR");
 //   mDirectoryChooserIRButton.setColour(juce::TextButton::buttonColourId, juce::Colours::blue);
    mDirectoryChooserIRButton.onClick = [this]() { openDirectoryChooser(DirectoryType::IRDir); };


    addAndMakeVisible(mIRVerbPathLabel);
    juce::File irVerbPath = processor.getIRVerbPath();
    mIRVerbPathLabel.setText("IR path selected : " + irPath.getFullPathName(), juce::dontSendNotification);
    mIRVerbPathLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    mIRVerbPathLabel.setJustificationType(juce::Justification::centredLeft);

    addAndMakeVisible(mDirectoryChooserIRVerbButton);
    mDirectoryChooserIRVerbButton.setButtonText("Choose Directory for Verb IR");
   // mDirectoryChooserIRButton.setColour(juce::TextButton::buttonColourId, juce::Colours::blue);
    mDirectoryChooserIRVerbButton.onClick = [this]() { openDirectoryChooser(DirectoryType::VerbIRDir); };

    addAndMakeVisible(mCloseButton);
    mCloseButton.setButtonText("Close");
    mCloseButton.onClick = [this]() { exitModalState(0); };

    mTitleLabel.setLookAndFeel(&mModalButtonLookAndFeel);
    mDirectoryChooserNAMButton.setLookAndFeel(&mModalButtonLookAndFeel);
    mDirectoryChooserIRButton.setLookAndFeel(&mModalButtonLookAndFeel);
    mDirectoryChooserIRVerbButton.setLookAndFeel(&mModalButtonLookAndFeel);
	mCloseButton.setLookAndFeel(&mModalButtonLookAndFeel);
    mNAMPathLabel.setLookAndFeel(&mModalButtonLookAndFeel);
    mIRPathLabel.setLookAndFeel(&mModalButtonLookAndFeel);
    mIRVerbPathLabel.setLookAndFeel(&mModalButtonLookAndFeel);

}


void TopBarComponent::SettingsModal::openDirectoryChooser(DirectoryType inDirectoryType)
{
    juce::FileChooser chooser("Select a Directory",
                            juce::File::getSpecialLocation(juce::File::userHomeDirectory),
                            "*",
                            true);


        if (chooser.browseForDirectory())
        {
            handleSelectedDirectory(chooser.getResult(), inDirectoryType);
        }
    
}

void TopBarComponent::SettingsModal::handleSelectedDirectory(const juce::File& directory, DirectoryType inDirectoryType)
{
        if (directory.exists() && directory.isDirectory())
	        {
            SkeletonAudioProcessor* processor = dynamic_cast<SkeletonAudioProcessor*>(&mProcessor);
            if (processor == nullptr)
            {
	            DBG("Failed to cast inProcessor to AmpAudioProcessor.");
	            return;
            }
            else
            {
                switch (inDirectoryType)
                {
                case TopBarComponent::SettingsModal::DirectoryType::NAMDir:
                    processor->setNAMPath(directory.getFullPathName());
                    mNAMPathLabel.setText("NAM Path : "+directory.getFullPathName(), juce::dontSendNotification);
                    break;
                case TopBarComponent::SettingsModal::DirectoryType::IRDir:
                    processor->setIRPath(directory.getFullPathName());
                    mIRPathLabel.setText("IR Path : " + directory.getFullPathName(), juce::dontSendNotification);
                    break;
                case TopBarComponent::SettingsModal::DirectoryType::VerbIRDir:
                    processor->setIRVerbPath(directory.getFullPathName());
                    mIRVerbPathLabel.setText("Verb IR Path : " + directory.getFullPathName(), juce::dontSendNotification);
                    break;
                default:
                    break;
                }
                if (auto* parentEditor = dynamic_cast<RootViewComponent*>(getParentComponent()))
                {
                    parentEditor->updatePath();
                }
            }
	    }
	    else
	    {
		    DBG("Selected path is not a valid directory.");
	    }
    DBG("Handling directory: " << directory.getFullPathName());

}
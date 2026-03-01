#include "gui.h"

#include "../dsp/Processor.h"
int RootViewComponent::ROOT_WIDTH = 980;
int RootViewComponent::ROOT_HEIGHT = 550;

RootViewComponent::RootViewComponent(juce::AudioProcessor& processor)
    : AudioProcessorEditor(processor)
    , mInputMeter(processor)
    , mOutputMeter(processor,false)
    , mFileChooser("choose NAM file")
    , mTopBar(processor)
{
    auto& gainProcessor = processor; 

    auto imageData = BinaryData::plate_png;
    auto imageDataSize = BinaryData::plate_pngSize;

    mImage = juce::ImageFileFormat::loadFrom(imageData, imageDataSize);

    if (mImage.isNull())
        DBG("Failed to load image from resources");

    setSize(ROOT_WIDTH,ROOT_HEIGHT);
    defineKnobLayout();
    configureNodes(gainProcessor);
}

RootViewComponent::~RootViewComponent()
{
	mInputKnob.setLookAndFeel(nullptr);
	mGateKnob.setLookAndFeel(nullptr);
	mBassKnob.setLookAndFeel(nullptr);
	mMidKnob.setLookAndFeel(nullptr);
	mHighKnob.setLookAndFeel(nullptr);
	mOutputKnob.setLookAndFeel(nullptr);
	mVerbMixKnob.setLookAndFeel(nullptr);
	mFileChooserButton.setLookAndFeel(nullptr);
	mNAMChooserButton.setLookAndFeel(nullptr);
    mIRButton.setLookAndFeel(nullptr);
    mNAMButton.setLookAndFeel(nullptr);
    mIRVerbButton.setLookAndFeel(nullptr);
    mVerbDropDown.setLookAndFeel(nullptr);
}

void RootViewComponent::setSliderAttachement(juce::AudioProcessor& inProcessoe)
{
    SkeletonAudioProcessor* ampAudioProcessor = dynamic_cast<SkeletonAudioProcessor*>(&inProcessoe);
    mInputAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        ampAudioProcessor->getCustomParameterTree(), "input", mInputKnob);
    mBassKnobAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        ampAudioProcessor->getCustomParameterTree(), "bass", mBassKnob);
    mMidKnobAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        ampAudioProcessor->getCustomParameterTree(), "mid", mMidKnob);
    mHighKnobAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        ampAudioProcessor->getCustomParameterTree(), "high", mHighKnob);
    mOutputKnobAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        ampAudioProcessor->getCustomParameterTree(), "output", mOutputKnob);
    mGateKnobAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        ampAudioProcessor->getCustomParameterTree(), "gate", mGateKnob);
    mIRButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        ampAudioProcessor->getCustomParameterTree(), "irEnabled", mIRButton);
    mNAMButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        ampAudioProcessor->getCustomParameterTree(), "namEnabled", mNAMButton);
    mIRVerbButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        ampAudioProcessor->getCustomParameterTree(), "irVerbEnabled", mIRVerbButton);
    mVerbMixKnobAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        ampAudioProcessor->getCustomParameterTree(), "irVerbMix", mVerbMixKnob);
}

void RootViewComponent::updatePath()
{
    SkeletonAudioProcessor* ampAudioProcessor = dynamic_cast<SkeletonAudioProcessor*>(&processor);

    juce::DirectoryIterator iter(ampAudioProcessor->getNAMPath(), false, "*", juce::File::findFiles);
    int itemId = 1;
    mNAMFileList.clear();
	mNAMChooserButton.clear();
    while (iter.next())
    {
        auto file = iter.getFile();
        mNAMFileList.add(new juce::File(file));
        mNAMChooserButton.addItem(file.getFileName(), itemId++);
    }

    mNAMChooserButton.setTextWhenNothingSelected("Select NAM File");
    mNAMChooserButton.setJustificationType(juce::Justification::centred);

    mNAMChooserButton.onChange = [this, ampAudioProcessor]() {
        int selectedId = mNAMChooserButton.getSelectedId();
            if (selectedId > 0 && selectedId <= mNAMFileList.size())
            {
                juce::File selectedFile = *mNAMFileList[selectedId - 1];
                handleSelectedNAMFile(selectedFile);
            }
        };

    juce::File selectedNAMFile = ampAudioProcessor->getDirectNAMPath();
    if (selectedNAMFile.existsAsFile())
    {
        mNAMChooserButton.setText(selectedNAMFile.getFileName(), juce::dontSendNotification);
    }
    else
    {
        mNAMChooserButton.setTextWhenNothingSelected("Select Fucking NAM Bro");
    }

    mFileChooserButton.setJustificationType(juce::Justification::centred);


    juce::DirectoryIterator iter2(ampAudioProcessor->getIRPath(), false, "*", juce::File::findFiles);
    int itemId2 = 1;

    mIRFileList.clear();
    mFileChooserButton.clear();
    while (iter2.next())
    {
        auto file = iter2.getFile();
        mIRFileList.add(new juce::File(file));
        mFileChooserButton.addItem(file.getFileName(), itemId2++);
    }

    mFileChooserButton.setJustificationType(juce::Justification::centred);

    juce::File selectedIRFile = ampAudioProcessor->getDirectIRPath();
    if (selectedIRFile.existsAsFile())
    {
		mFileChooserButton.setText(selectedIRFile.getFileName(), juce::dontSendNotification);
	}
    else
    {
		mFileChooserButton.setTextWhenNothingSelected("Select IR File");
	}

    mFileChooserButton.onChange = [this, ampAudioProcessor]() {
        int selectedId = mFileChooserButton.getSelectedId();
        if (selectedId > 0 && selectedId <= mIRFileList.size())
        {
            juce::File selectedFile = *mIRFileList[selectedId - 1];
            handleSelectedFile(selectedFile);
        }
    };

    juce::DirectoryIterator iter3(ampAudioProcessor->getIRVerbPath(), false, "*", juce::File::findFiles);
    int itemId3 = 1;

    mVerbFileList.clear();
    mVerbDropDown.clear();
    while (iter3.next())
    {
		auto file = iter3.getFile();
		mVerbFileList.add(new juce::File(file));
		mVerbDropDown.addItem(file.getFileName(), itemId3++);
	}

    mVerbDropDown.setJustificationType(juce::Justification::centred);

	juce::File selectedIRVerbFile = ampAudioProcessor->getDirectIRVerbPath();
    if (selectedIRVerbFile.existsAsFile())
    {
		mVerbDropDown.setText(selectedIRVerbFile.getFileName(), juce::dontSendNotification);
	}
    else
    {
		mVerbDropDown.setTextWhenNothingSelected("Select IR Verb File");
	}

    mVerbDropDown.onChange = [this, ampAudioProcessor]() {
		int selectedId = mVerbDropDown.getSelectedId();
        if (selectedId > 0 && selectedId <= mVerbFileList.size())
        {
			juce::File selectedFile = *mVerbFileList[selectedId - 1];
			handleSelectedVerbIRFile(selectedFile);
		}
	};

}

void RootViewComponent::handleSelectedFile(const juce::File& file)
{
    SkeletonAudioProcessor& audioProcessor = static_cast<SkeletonAudioProcessor&>(processor);
    audioProcessor.loadImpulseResponse(file);
}

void RootViewComponent::handleSelectedNAMFile(const juce::File& file)
{
    SkeletonAudioProcessor& audioProcessor = static_cast<SkeletonAudioProcessor&>(processor);
	audioProcessor.loadNAMFile(file);
}

void RootViewComponent::handleSelectedVerbIRFile(const juce::File& file)
{
    SkeletonAudioProcessor& audioProcessor = static_cast<SkeletonAudioProcessor&>(processor);
	audioProcessor.loadImpulseResponseVerb(file);
}

void RootViewComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);

    if (!mImage.isNull())
    {
        auto bounds = getLocalBounds().toFloat();
        auto imageBounds = mImage.getBounds().toFloat();
        auto scale = juce::jmin(bounds.getWidth() / imageBounds.getWidth(),
            bounds.getHeight() / imageBounds.getHeight());
        auto scaledImageBounds = imageBounds.withSize(imageBounds.getWidth() * scale,
            imageBounds.getHeight() * scale);
        g.drawImage(mImage, scaledImageBounds);
    }
    else
    {
        g.setColour(juce::Colours::white);
        g.drawText("Image not found", getLocalBounds(), juce::Justification::centred);
    }
}

void RootViewComponent::resized()
{
    auto bounds = getLocalBounds();
}

void RootViewComponent::MeterComponent::paint(juce::Graphics& g)
{
    SkeletonAudioProcessor& meter = static_cast<SkeletonAudioProcessor&>(audioProcessor);
    float rmsLeft = 0.0f;
    float rmsRight = 0.0f;

    if (isInput)
    {
        rmsLeft = meter.getRmsLevelLeft();
        rmsRight = meter.getRmsLevelRight();
        g.setColour(juce::Colours::white);
        rmsLeft = rmsLeft * 10;
    }
    else
    {
        rmsLeft = meter.getRmsOutputLevelLeft();
        rmsRight = meter.getRmsOutputLevelRight();
        g.setColour(juce::Colours::green);
        rmsLeft = rmsLeft * 10;
    }

    juce::Rectangle<float> meterBounds(0, 0, getWidth(), getHeight());
    g.setColour(juce::Colours::darkgrey.withAlpha(0.8f)); 
    g.fillRoundedRectangle(meterBounds, 5.0f); 

    juce::Rectangle<float> leftMeterBounds(10, getHeight() - (rmsLeft * getHeight()), getWidth() / 2 - 15, rmsLeft * getHeight());
    juce::ColourGradient leftGradient(juce::Colours::green, leftMeterBounds.getBottomLeft(),
        juce::Colours::limegreen, leftMeterBounds.getTopLeft(), false);
    g.setGradientFill(leftGradient);
    g.fillRoundedRectangle(leftMeterBounds, 3.0f); 

    juce::Rectangle<float> rightMeterBounds(getWidth() / 2 + 5, getHeight() - (rmsLeft * getHeight()), getWidth() / 2 - 15, rmsLeft * getHeight());
    juce::ColourGradient rightGradient(juce::Colours::green, rightMeterBounds.getBottomLeft(),
        juce::Colours::limegreen, rightMeterBounds.getTopLeft(), false);
    g.setGradientFill(rightGradient);
    g.fillRoundedRectangle(rightMeterBounds, 3.0f); 

    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.drawRoundedRectangle(meterBounds, 5.0f, 1.0f);
}


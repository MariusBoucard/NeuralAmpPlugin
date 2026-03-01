#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "BinaryData.h"

class KnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    KnobLookAndFeel()
    {
        // Load the strip image from BinaryData
        auto imageData = BinaryData::knob_strip_png; // Replace with your strip image name
        auto imageDataSize = BinaryData::knob_strip_pngSize; // Replace with your strip image size
        mKnobStrip = juce::ImageFileFormat::loadFrom(imageData, imageDataSize);

        if (mKnobStrip.isNull())
            DBG("Failed to load knob strip image from resources");
    }


    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider& slider) override
    {
        if (mKnobStrip.isNull())
            return;

        const int numFrames = 260;
        int column = 5;
        const int row = numFrames / column; // Assuming the first row of the image
        const int frameIndex = (int)std::round(sliderPosProportional * (numFrames - 1)) % numFrames;
        int frame_width = 135;
        int frame_height = 153;

        int line = frameIndex / column;
        int columnIndex = frameIndex % column;
        const auto sourceRect = juce::Rectangle<int>(columnIndex*frame_width, line*frame_height, frame_width, frame_height);

        g.drawImage(mKnobStrip, x, y, width, height, sourceRect.getX(), sourceRect.getY(),
                    sourceRect.getWidth(), sourceRect.getHeight());
    }


    void drawLabel(juce::Graphics& g, juce::Label& label) override
    {
        if (auto* slider = dynamic_cast<juce::Slider*>(label.getParentComponent()))
        {
            if (!slider->isMouseOverOrDragging())
            {
                // If not hovered, don't draw the label
                return;
            }
        }
        juce::Rectangle bounds = label.getLocalBounds();
        bounds.setX(15);
        bounds.setWidth(bounds.getWidth() -15);

        g.setColour(juce::Colours::darkgrey);
        g.fillRect(bounds);
        g.setColour(juce::Colours::limegreen);
        g.drawRect(bounds, 2.0f); // Border thickness of 2.0f

        // Draw the text
        g.setColour(juce::Colours::white);
        g.setFont(label.getFont());
        g.drawText(label.getText(), bounds, juce::Justification::centred, true);
    }
private:
    juce::Image mKnobStrip;
};




class ToggleButtonLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ToggleButtonLookAndFeel()
    {
        // Load the strip image from BinaryData
        auto imageData = BinaryData::toggleButtonStrip_png; // Replace with your strip image name
        auto imageDataSize = BinaryData::toggleButtonStrip_pngSize; // Replace with your strip image size
        mToggleStrip = juce::ImageFileFormat::loadFrom(imageData, imageDataSize);

        if (mToggleStrip.isNull())
            DBG("Failed to load toggle strip image from resources");
    }

    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button, bool isMouseOverButton, bool isButtonDown) override
    {
        auto bounds = button.getLocalBounds();

        const int numFrames = 2; // Assuming the strip has 2 frames: "off" and "on"
        const int frameHeight = mToggleStrip.getHeight() / numFrames;

        if (frameHeight <= 0)
        {
            DBG("Invalid frameHeight. Check your image dimensions.");
            return;
        }

        // Determine the frame to display based on the toggle state
        int frameIndex = button.getToggleState() ? 1 : 0;

        if (frameIndex * frameHeight >= mToggleStrip.getHeight())
        {
            DBG("Invalid frameIndex. Check your calculations.");
            return;
        }
        // Destination coordinates and dimensions
        int destX = bounds.getX();
        int destY = bounds.getY();
        int destWidth = bounds.getWidth();
        int destHeight = bounds.getHeight();

        // Source coordinates and dimensions
        int sourceX = 0;
        int sourceY = frameIndex * frameHeight;
        int sourceWidth = mToggleStrip.getWidth();
        int sourceHeight = frameHeight;

        // Draw the appropriate frame from the strip
        g.drawImage(mToggleStrip,
            destX, destY, destWidth, destHeight, // Destination
            sourceX, sourceY, sourceWidth, sourceHeight, // Source
            false); // Do not fill alpha c
    }

private:
    juce::Image mToggleStrip; // The image strip for the toggle button
};
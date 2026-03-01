#pragma once

class DropDownLookAndFeel : public juce::LookAndFeel_V4
{
public:
    DropDownLookAndFeel() {}
    // Customize the ComboBox background and border
    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
        int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox& box) override
    {
        auto bounds = juce::Rectangle<int>(0, 0, width, height).toFloat();

        // Background color: black with 0.8 transparency
        g.setColour(juce::Colours::black.withAlpha(0.2f));
        g.fillRoundedRectangle(bounds, 6.0f); // Rounded corners

        // Border color: green with 0.8 alpha
        g.setColour(juce::Colours::limegreen.withAlpha(0.8f));
     //   g.drawRoundedRectangle(bounds, 6.0f, 2.0f); // Rounded border

        // Draw the arrow button (dropdown indicator)
        juce::Path arrow;
        arrow.addTriangle(buttonX + 5.0f, buttonY + buttonH * 0.4f,
            buttonX + buttonW - 5.0f, buttonY + buttonH * 0.4f,
            buttonX + buttonW * 0.5f, buttonY + buttonH * 0.7f);

        g.setColour(juce::Colours::white);
        g.fillPath(arrow);
    }

    // Draw the text inside the dropdown button
    void drawButtonText(juce::Graphics& g, juce::TextButton& button, bool isMouseOverButton, bool isButtonDown) override
    {
        // Text color: white by default
        juce::Colour textColour = juce::Colours::white;

        g.setColour(textColour);
        g.setFont(juce::Font(16.0f, juce::Font::bold)); // Font size and style

        // Center the text inside the button
        g.drawText(button.getButtonText(), button.getLocalBounds(), juce::Justification::centred, true);
    }

    // Draw the items in the dropdown menu
    void drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
        const bool isSeparator, const bool isActive, const bool isHighlighted, const bool isTicked,
        const bool hasSubMenu, const juce::String& text, const juce::String& shortcutKeyText,
        const juce::Drawable* icon, const juce::Colour* textColour) override
    {
        if (isSeparator)
        {
            // Draw a separator line
            g.setColour(juce::Colours::grey);
            g.fillRect(area.reduced(5, 0).removeFromTop(1));
            return;
        }

        // Background color for items
        juce::Colour bgColour = juce::Colours::black.withAlpha(0.8f); // Default transparent black
        if (isHighlighted)
        {
            bgColour = juce::Colours::limegreen.darker();// juce::Colour(150, 150, 150); // Lighter grey when hovered
        }

        g.setColour(bgColour);
        g.fillRect(area);

        // Text color for items
        juce::Colour itemTextColour = isHighlighted ? juce::Colours::white : juce::Colours::limegreen;

        g.setColour(itemTextColour);
        g.setFont(juce::Font(14.0f)); // Font size for items
        g.drawText(text, area.reduced(10, 0), juce::Justification::centredLeft, true);
    }

    // Customize the popup menu background
    void drawPopupMenuBackground(juce::Graphics& g, int width, int height) override
    {
        // Background color: black with 0.8 transparency
        g.setColour(juce::Colours::black.withAlpha(0.8f));
        g.fillRoundedRectangle(0, 0, (float)width, (float)height, 6.0f); // Rounded corners
    }
};
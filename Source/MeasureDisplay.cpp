#include "MeasureDisplay.h"

MeasureDisplay::MeasureDisplay(MeasureCounterAudioProcessor& p)
    : audioProcessor(p)
{
    // Setup reset button
    resetButton.setButtonText("RESET");
    resetButton.addListener(this);
    addChildComponent(resetButton); // Hidden by default
    
    // Setup cycle length selector
    cycleLengthSelector.addItem("4 Bars", 1);
    cycleLengthSelector.addItem("8 Bars", 2);
    cycleLengthSelector.addItem("16 Bars", 3);
    cycleLengthSelector.addItem("32 Bars", 4);
    
    // Set initial selection based on processor's cycle length
    int currentCycle = audioProcessor.getCycleLength();
    if (currentCycle == 4) cycleLengthSelector.setSelectedId(1);
    else if (currentCycle == 8) cycleLengthSelector.setSelectedId(2);
    else if (currentCycle == 16) cycleLengthSelector.setSelectedId(3);
    else if (currentCycle == 32) cycleLengthSelector.setSelectedId(4);
    
    cycleLengthSelector.onChange = [this]
    {
        int selectedId = cycleLengthSelector.getSelectedId();
        int newCycleLength = 8; // default
        
        if (selectedId == 1) newCycleLength = 4;
        else if (selectedId == 2) newCycleLength = 8;
        else if (selectedId == 3) newCycleLength = 16;
        else if (selectedId == 4) newCycleLength = 32;
        
        audioProcessor.setCycleLength(newCycleLength);
    };
    
    addChildComponent(cycleLengthSelector); // Hidden by default
    
    // Setup settings icon (gear)
    settingsIcon.setButtonText(juce::CharPointer_UTF8("\xE2\x9A\x99")); // ⚙ gear symbol
    settingsIcon.addListener(this);
    settingsIcon.setAlwaysOnTop(true); // Keep it on top
    addAndMakeVisible(settingsIcon); // Add as visible child
    settingsIcon.setVisible(false); // Start hidden
    
    // Setup close icon (X)
    closeIcon.setButtonText(juce::CharPointer_UTF8("\xE2\x9C\x95")); // ✕ X symbol
    closeIcon.addListener(this);
    closeIcon.setAlwaysOnTop(true); // Keep it on top
    addAndMakeVisible(closeIcon); // Add as visible child
    closeIcon.setVisible(false); // Start hidden
}

MeasureDisplay::~MeasureDisplay()
{
    resetButton.removeListener(this);
    settingsIcon.removeListener(this);
    closeIcon.removeListener(this);
}

void MeasureDisplay::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // Background
    g.fillAll(getBackgroundColor());
    
    // Main display area - single cycle progress bar (use full height now)
    auto displaySection = bounds;
    
    // ===== CYCLE PROGRESS (segmented by bars) =====
    displaySection.reduce(10, 10);
    
    // Segmented cycle progress
    int cycleLength = audioProcessor.getCycleLength();
    int numBars = cycleLength;
    float segmentGap = 4.0f;
    float totalGapWidth = segmentGap * (numBars - 1);
    float segmentWidth = (displaySection.getWidth() - totalGapWidth) / numBars;
    
    for (int i = 0; i < numBars; ++i)
    {
        float x = displaySection.getX() + i * (segmentWidth + segmentGap);
        juce::Rectangle<float> segment(x, displaySection.getY(), segmentWidth, displaySection.getHeight());
        
        // Determine color for this bar segment
        int barNum = i + 1;
        bool isCurrentBar = (barNum == transportInfo.barInCycle);
        bool isBeforeCurrent = (barNum < transportInfo.barInCycle);
        
        // Get highly saturated, bright colors for maximum visibility
        juce::Colour segmentColor;
        if (barNum == cycleLength)
            segmentColor = juce::Colour(255, 40, 40); // Bright red
        else if (barNum == cycleLength - 1)
            segmentColor = juce::Colour(255, 220, 0); // Pure bright yellow
        else
            segmentColor = juce::Colour(0, 180, 255); // Vivid blue (less cyan, more saturated)
        
        // Background colors
        juce::Colour bgColor(200, 195, 185); // Warm beige for unfilled
        juce::Colour darkBg(60, 60, 65); // Dark grey background
        
        if (isBeforeCurrent)
        {
            // Fully filled - completed bars with beat segments on dark background
            int beatsInBar = transportInfo.timeSigNumerator;
            float beatGap = 2.0f;
            float totalGaps = beatGap * (beatsInBar - 1);
            float beatWidth = (segment.getWidth() - totalGaps) / beatsInBar;
            
            // Dark background for completed bars
            g.setColour(darkBg);
            g.fillRect(segment);
            
            for (int beat = 0; beat < beatsInBar; ++beat)
            {
                float beatX = segment.getX() + beat * (beatWidth + beatGap);
                juce::Rectangle<float> beatRect(beatX, segment.getY(), beatWidth, segment.getHeight());
                // Dim the color for completed bars - darker like future bars
                g.setColour(segmentColor.withAlpha(0.3f));
                g.fillRect(beatRect);
            }
        }
        else if (isCurrentBar)
        {
            // Partially filled based on position within bar - fill by beat segments
            int beatsInBar = transportInfo.timeSigNumerator;
            int currentBeatInBar = transportInfo.currentBeat - 1; // 0-indexed
            
            float beatGap = 2.0f;
            float totalGaps = beatGap * (beatsInBar - 1);
            float beatWidth = (segment.getWidth() - totalGaps) / beatsInBar;
            
            // Background - warm light grey for high contrast with black text and blue
            g.setColour(bgColor);
            g.fillRect(segment);
            
            // Fill beats up to and including the current beat
            for (int beat = 0; beat < beatsInBar; ++beat)
            {
                float beatX = segment.getX() + beat * (beatWidth + beatGap);
                juce::Rectangle<float> beatRect(beatX, segment.getY(), beatWidth, segment.getHeight());
                
                if (beat <= currentBeatInBar)
                {
                    // Current beat and all previous beats - fully filled
                    g.setColour(segmentColor);
                    g.fillRect(beatRect);
                }
                // Future beats remain as background (beige)
            }
            
            // Border highlight for current bar - bright green for maximum visibility
            g.setColour(juce::Colour(0, 255, 0));
            g.drawRect(segment, 3.0f);
        }
        else
        {
            // Empty - future bars - dark grey for clear differentiation
            g.setColour(darkBg);
            g.fillRect(segment);
        }
        
        // Draw segment border - medium grey for subtle definition
        g.setColour(juce::Colour(80, 80, 85));
        g.drawRect(segment, 1.0f);
        
        // Draw bar number in segment - all numbers same size based on available space
        if (segmentWidth > 20) // Only if there's enough space
        {
            g.setColour(juce::Colours::black);
            
            // Calculate font size assuming worst case (2-digit numbers)
            // This ensures all numbers are consistently sized
            float fontSize = segmentWidth * 0.45f; // 45% of segment width for 2-digit numbers
            
            // Cap between reasonable limits
            fontSize = juce::jmax(12.0f, juce::jmin(fontSize, segment.getHeight() * 0.7f));
            
            g.setFont(juce::FontOptions(fontSize).withStyle("Bold"));
            g.drawText(juce::String(barNum), segment, juce::Justification::centred, false);
        }
    }
    
    // If not playing, show indicator
    if (!transportInfo.isPlaying)
    {
        g.setColour(juce::Colours::black.withAlpha(0.7f));
        g.fillAll();
        
        g.setColour(juce::Colours::white);
        g.setFont(bounds.getHeight() * 0.15f);
        g.drawText("STOPPED", getLocalBounds(), juce::Justification::centred, false);
    }
    
    // Draw settings overlay if visible
    if (settingsVisible)
    {
        // Black overlay covering everything
        g.setColour(juce::Colours::black.withAlpha(0.85f));
        g.fillAll();
        
        // Semi-transparent panel behind controls (centered, tight fit with 5px padding)
        int panelWidth = 260;  // Tight fit for controls
        int panelHeight = 50;  // Just enough for one row of controls with padding
        auto panelBounds = getLocalBounds().withSizeKeepingCentre(panelWidth, panelHeight);
        g.setColour(juce::Colour(40, 40, 45).withAlpha(0.9f));
        g.fillRoundedRectangle(panelBounds.toFloat(), 8.0f);
        
        // Optional: Draw border around panel
        g.setColour(juce::Colour(100, 100, 105));
        g.drawRoundedRectangle(panelBounds.toFloat(), 8.0f, 2.0f);
    }
}

void MeasureDisplay::resized()
{
    auto bounds = getLocalBounds();
    
    // Calculate font sizes based on component height
    labelFontSize = juce::jmax(14.0f, bounds.getHeight() * 0.12f);
    
    // Position settings icon in top-right corner
    int iconSize = 30;
    int padding = 10;
    auto iconBounds = juce::Rectangle<int>(bounds.getWidth() - iconSize - padding, 
                                           padding, 
                                           iconSize, 
                                           iconSize);
    settingsIcon.setBounds(iconBounds);
    closeIcon.setBounds(iconBounds); // Same position
    
    // Position controls in centered panel (tight fit)
    int panelWidth = 260;
    int panelHeight = 50;
    auto panelBounds = bounds.withSizeKeepingCentre(panelWidth, panelHeight);
    auto controlArea = panelBounds.reduced(5); // 5px padding
    
    // Position controls (side by side, centered within panel)
    int buttonWidth = 100;
    int comboWidth = 120;
    int spacing = 20;
    int totalWidth = buttonWidth + spacing + comboWidth;
    int startX = controlArea.getX() + (controlArea.getWidth() - totalWidth) / 2;
    int controlY = controlArea.getY() + (controlArea.getHeight() - 30) / 2; // Center vertically
    
    resetButton.setBounds(startX, controlY, buttonWidth, 30);
    cycleLengthSelector.setBounds(startX + buttonWidth + spacing, controlY, comboWidth, 30);
}

void MeasureDisplay::setTransportInfo(const MeasureCounterAudioProcessor::TransportInfo& info)
{
    transportInfo = info;
    repaint();
}

void MeasureDisplay::mouseEnter(const juce::MouseEvent& event)
{
    juce::ignoreUnused(event);
    isHovering = true;
    updateIconVisibility();
}

void MeasureDisplay::mouseExit(const juce::MouseEvent& event)
{
    juce::ignoreUnused(event);
    // Check if mouse is actually outside the component bounds
    if (!getLocalBounds().contains(event.getPosition()))
    {
        isHovering = false;
        updateIconVisibility();
    }
}

void MeasureDisplay::buttonClicked(juce::Button* button)
{
    if (button == &resetButton)
    {
        audioProcessor.resetCycle();
    }
    else if (button == &settingsIcon)
    {
        toggleSettings(true);
    }
    else if (button == &closeIcon)
    {
        toggleSettings(false);
    }
}

void MeasureDisplay::toggleSettings(bool show)
{
    settingsVisible = show;
    updateIconVisibility();
    resetButton.setVisible(show);
    cycleLengthSelector.setVisible(show);
    repaint();
}

void MeasureDisplay::updateIconVisibility()
{
    if (settingsVisible)
    {
        // Settings are open - show X, hide gear
        settingsIcon.setVisible(false);
        closeIcon.setVisible(true);
    }
    else
    {
        // Settings are closed - show gear only on hover
        settingsIcon.setVisible(isHovering);
        closeIcon.setVisible(false);
    }
}

juce::Colour MeasureDisplay::getBarColor() const
{
    int cycleLength = audioProcessor.getCycleLength();
    int barInCycle = transportInfo.barInCycle;
    
    // Last bar - RED (critical)
    if (barInCycle == cycleLength)
        return juce::Colour(255, 50, 50); // Bright red
    
    // Second-to-last bar - YELLOW (warning)
    if (barInCycle == cycleLength - 1)
        return juce::Colour(255, 200, 50); // Bright yellow
    
    // Normal bars - CYAN
    return juce::Colour(50, 200, 255); // Bright cyan
}

juce::Colour MeasureDisplay::getBackgroundColor() const
{
    return juce::Colour(20, 20, 25); // Very dark blue-grey
}

juce::Colour MeasureDisplay::getTextColor() const
{
    return juce::Colours::white;
}

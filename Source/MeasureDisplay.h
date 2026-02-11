#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

class MeasureDisplay : public juce::Component,
                       public juce::Button::Listener
{
public:
    MeasureDisplay(MeasureCounterAudioProcessor& p);
    ~MeasureDisplay() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void setTransportInfo(const MeasureCounterAudioProcessor::TransportInfo& info);
    
    // Mouse events for hover detection
    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;
    
    // Button listener
    void buttonClicked(juce::Button* button) override;

private:
    MeasureCounterAudioProcessor& audioProcessor;
    MeasureCounterAudioProcessor::TransportInfo transportInfo;
    
    // UI Components
    juce::TextButton resetButton;
    juce::ComboBox cycleLengthSelector;
    juce::TextButton settingsIcon;
    juce::TextButton closeIcon;
    
    // Settings overlay state
    bool settingsVisible = false;
    bool isHovering = false;
    
    // Colors for different bar states
    juce::Colour getBarColor() const;
    juce::Colour getBackgroundColor() const;
    juce::Colour getTextColor() const;
    
    // Settings management
    void toggleSettings(bool show);
    void updateIconVisibility();
    
    // Font size (calculated in resized())
    float labelFontSize = 20.0f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MeasureDisplay)
};

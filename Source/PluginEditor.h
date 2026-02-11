#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "MeasureDisplay.h"

class MeasureCounterAudioProcessorEditor : public juce::AudioProcessorEditor,
                                           private juce::Timer
{
public:
    MeasureCounterAudioProcessorEditor (MeasureCounterAudioProcessor&);
    ~MeasureCounterAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    // Timer callback for updating display
    void timerCallback() override;

private:
    MeasureCounterAudioProcessor& audioProcessor;
    MeasureDisplay measureDisplay;
    
    // Constraints for resizing
    static constexpr int minWidth = 600;
    static constexpr int minHeight = 100;
    static constexpr int maxWidth = 3000;
    static constexpr int maxHeight = 800;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MeasureCounterAudioProcessorEditor)
};

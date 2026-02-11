#include "PluginProcessor.h"
#include "PluginEditor.h"

MeasureCounterAudioProcessorEditor::MeasureCounterAudioProcessorEditor (MeasureCounterAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), measureDisplay(p)
{
    addAndMakeVisible(measureDisplay);
    
    // Get saved window size or use default
    auto savedSize = audioProcessor.getWindowSize();
    setSize (savedSize.x, savedSize.y);
    
    // Make window resizable with constraints
    setResizable(true, true);
    getConstrainer()->setMinimumSize(minWidth, minHeight);
    getConstrainer()->setMaximumSize(maxWidth, maxHeight);
    
    // Start timer for updating display (30 Hz / ~33ms)
    startTimer(33);
}

MeasureCounterAudioProcessorEditor::~MeasureCounterAudioProcessorEditor()
{
    // Save window size when closing
    audioProcessor.setWindowSize(getWidth(), getHeight());
    stopTimer();
}

void MeasureCounterAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Just fill with a dark background (MeasureDisplay handles all painting)
    g.fillAll(juce::Colour(20, 20, 25));
}

void MeasureCounterAudioProcessorEditor::resized()
{
    // Make display fill entire editor
    measureDisplay.setBounds(getLocalBounds());
    
    // Save new size to processor
    audioProcessor.setWindowSize(getWidth(), getHeight());
}

void MeasureCounterAudioProcessorEditor::timerCallback()
{
    // Update display with current transport info
    measureDisplay.setTransportInfo(audioProcessor.getTransportInfo());
}

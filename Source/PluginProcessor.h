#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>

class MeasureCounterAudioProcessor : public juce::AudioProcessor
{
public:
    MeasureCounterAudioProcessor();
    ~MeasureCounterAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Transport and measure information
    struct TransportInfo
    {
        bool isPlaying = false;
        double bpm = 120.0;
        int timeSigNumerator = 4;
        int timeSigDenominator = 4;
        int currentBar = 1;
        int currentBeat = 1;
        int barInCycle = 1;
        double positionInBar = 0.0;      // 0.0 to 1.0
        double positionInCycle = 0.0;    // 0.0 to 1.0
    };

    TransportInfo getTransportInfo() const { return transportInfo; }
    
    // Cycle length control (4, 8, 16, or 32 bars)
    void setCycleLength(int bars);
    int getCycleLength() const { return cycleLength; }
    
    // Manual reset functionality
    void resetCycle();
    
    // Window size persistence
    void setWindowSize(int width, int height);
    juce::Point<int> getWindowSize() const { return windowSize; }

private:
    void updateTransportInfo();
    
    TransportInfo transportInfo;
    int cycleLength = 8;           // Default to 8 bars
    int resetOffset = 0;            // Offset for manual reset
    juce::Point<int> windowSize{1200, 150};  // Default window size
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MeasureCounterAudioProcessor)
};

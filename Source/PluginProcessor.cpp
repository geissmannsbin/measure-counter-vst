#include "PluginProcessor.h"
#include "PluginEditor.h"

MeasureCounterAudioProcessor::MeasureCounterAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

MeasureCounterAudioProcessor::~MeasureCounterAudioProcessor()
{
}

const juce::String MeasureCounterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MeasureCounterAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MeasureCounterAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MeasureCounterAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MeasureCounterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MeasureCounterAudioProcessor::getNumPrograms()
{
    return 1;
}

int MeasureCounterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MeasureCounterAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused(index);
}

const juce::String MeasureCounterAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused(index);
    return {};
}

void MeasureCounterAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

void MeasureCounterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(sampleRate, samplesPerBlock);
}

void MeasureCounterAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MeasureCounterAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void MeasureCounterAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);
    juce::ScopedNoDenormals noDenormals;

    // This is a pass-through plugin - no audio processing needed
    // Just update transport information
    updateTransportInfo();
}

void MeasureCounterAudioProcessor::updateTransportInfo()
{
    if (auto* playHead = getPlayHead())
    {
        if (auto positionInfo = playHead->getPosition())
        {
            transportInfo.isPlaying = positionInfo->getIsPlaying();
            
            // Get BPM
            if (auto bpm = positionInfo->getBpm())
                transportInfo.bpm = *bpm;
            
            // Get time signature
            if (auto timeSig = positionInfo->getTimeSignature())
            {
                transportInfo.timeSigNumerator = timeSig->numerator;
                transportInfo.timeSigDenominator = timeSig->denominator;
            }
            
            // Get PPQ position
            if (auto ppq = positionInfo->getPpqPosition())
            {
                double ppqPosition = *ppq;
                
                // Calculate beats per bar
                double beatsPerBar = (double)transportInfo.timeSigNumerator * 
                                    (4.0 / (double)transportInfo.timeSigDenominator);
                
                // Calculate absolute bar number (0-based)
                int absoluteBar = (int)std::floor(ppqPosition / beatsPerBar);
                
                // Apply reset offset
                int adjustedBar = absoluteBar - resetOffset;
                if (adjustedBar < 0)
                    adjustedBar = 0;
                
                // Calculate bar within cycle (1-based for display)
                transportInfo.currentBar = adjustedBar + 1;
                transportInfo.barInCycle = (adjustedBar % cycleLength) + 1;
                
                // Calculate beat within bar (1-based for display)
                double positionInBar = std::fmod(ppqPosition, beatsPerBar);
                transportInfo.currentBeat = (int)std::floor(positionInBar / (4.0 / transportInfo.timeSigDenominator)) + 1;
                
                // Clamp beat to valid range
                if (transportInfo.currentBeat > transportInfo.timeSigNumerator)
                    transportInfo.currentBeat = transportInfo.timeSigNumerator;
                if (transportInfo.currentBeat < 1)
                    transportInfo.currentBeat = 1;
                
                // Calculate position within current bar (0.0 to 1.0)
                transportInfo.positionInBar = positionInBar / beatsPerBar;
                
                // Calculate position within cycle (0.0 to 1.0)
                double barsIntoCycle = (double)(adjustedBar % cycleLength);
                transportInfo.positionInCycle = (barsIntoCycle + transportInfo.positionInBar) / (double)cycleLength;
            }
        }
    }
}

void MeasureCounterAudioProcessor::setCycleLength(int bars)
{
    if (bars == 4 || bars == 8 || bars == 16 || bars == 32)
    {
        cycleLength = bars;
    }
}

void MeasureCounterAudioProcessor::resetCycle()
{
    if (auto* playHead = getPlayHead())
    {
        if (auto positionInfo = playHead->getPosition())
        {
            if (auto ppq = positionInfo->getPpqPosition())
            {
                double ppqPosition = *ppq;
                
                // Calculate current absolute bar
                double beatsPerBar = (double)transportInfo.timeSigNumerator * 
                                    (4.0 / (double)transportInfo.timeSigDenominator);
                int currentAbsoluteBar = (int)std::floor(ppqPosition / beatsPerBar);
                
                // Set reset offset to make current position bar 1
                resetOffset = currentAbsoluteBar;
            }
        }
    }
}

void MeasureCounterAudioProcessor::setWindowSize(int width, int height)
{
    windowSize = juce::Point<int>(width, height);
}

bool MeasureCounterAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* MeasureCounterAudioProcessor::createEditor()
{
    return new MeasureCounterAudioProcessorEditor (*this);
}

void MeasureCounterAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // Save state as XML
    juce::XmlElement xml("MeasureCounterState");
    
    xml.setAttribute("cycleLength", cycleLength);
    xml.setAttribute("resetOffset", resetOffset);
    xml.setAttribute("windowWidth", windowSize.x);
    xml.setAttribute("windowHeight", windowSize.y);
    
    copyXmlToBinary(xml, destData);
}

void MeasureCounterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // Restore state from XML
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    
    if (xmlState != nullptr)
    {
        if (xmlState->hasTagName("MeasureCounterState"))
        {
            cycleLength = xmlState->getIntAttribute("cycleLength", 8);
            resetOffset = xmlState->getIntAttribute("resetOffset", 0);
            windowSize.x = xmlState->getIntAttribute("windowWidth", 1200);
            windowSize.y = xmlState->getIntAttribute("windowHeight", 150);
        }
    }
}

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MeasureCounterAudioProcessor();
}

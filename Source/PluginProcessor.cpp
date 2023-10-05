/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Phillips_Assignment1AudioProcessor::Phillips_Assignment1AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), treeState(*this, nullptr, "PARAMETERS", createParameterLayout())
#endif

{
    treeState.addParameterListener("tiltID", this);
}

Phillips_Assignment1AudioProcessor::~Phillips_Assignment1AudioProcessor()
{
    treeState.removeParameterListener("tiltID", this);
}

//==============================================================================
const juce::String Phillips_Assignment1AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

juce::AudioProcessorValueTreeState::ParameterLayout Phillips_Assignment1AudioProcessor::createParameterLayout()
{
    std::vector <std::unique_ptr<juce::RangedAudioParameter>> params;
    
    params.reserve(1);
    
    juce::NormalisableRange<float> tiltRange (-10.f, 10.f);
    params.push_back(std::make_unique<juce::AudioParameterFloat>("tiltID", "tilt", tiltRange, 0.f));
    
    return { params.begin(), params.end() };
}

void Phillips_Assignment1AudioProcessor::parameterChanged(const juce::String &parameterID, float newValue)
{
    if (parameterID == "tiltID")
    {
        hiShelfParams.boostCut_dB = newValue;
        leftHiShelf.setParameters(hiShelfParams);
        rightHiShelf.setParameters(hiShelfParams);
        
        loShelfParams.boostCut_dB = -newValue; // inverting the gain here so that a combination of the shelf filters act like a tilt EQ
        leftLoShelf.setParameters(loShelfParams);
        rightLoShelf.setParameters(loShelfParams);
    }
}

bool Phillips_Assignment1AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Phillips_Assignment1AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Phillips_Assignment1AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Phillips_Assignment1AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Phillips_Assignment1AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Phillips_Assignment1AudioProcessor::getCurrentProgram()
{
    return 0;
}

void Phillips_Assignment1AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Phillips_Assignment1AudioProcessor::getProgramName (int index)
{
    return {};
}

void Phillips_Assignment1AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Phillips_Assignment1AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    leftHiShelf.reset(sampleRate);
    rightHiShelf.reset(sampleRate);
    hiShelfParams.algorithm = filterAlgorithm::schneiderHiShelf;
    hiShelfParams.Q = 0.5f;
    hiShelfParams.boostCut_dB = 0;
    hiShelfParams.fc = 350.;
    leftHiShelf.setParameters(hiShelfParams);
    rightHiShelf.setParameters(hiShelfParams);
    
    leftLoShelf.reset(sampleRate);
    rightLoShelf.reset(sampleRate);
    loShelfParams.algorithm = filterAlgorithm::schneiderLoShelf;
    loShelfParams.Q = 0.5f;
    loShelfParams.boostCut_dB = 0;
    loShelfParams.fc = 350.f;
    leftLoShelf.setParameters(loShelfParams);
    rightLoShelf.setParameters(loShelfParams);
}

void Phillips_Assignment1AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Phillips_Assignment1AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void Phillips_Assignment1AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    int numSamples = buffer.getNumSamples();
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, numSamples);

    for (int sample = 0; sample < numSamples; ++sample)
    {
        auto* xnL = buffer.getReadPointer(0);
        auto* ynL = buffer.getWritePointer(0);
        auto* xnR = buffer.getReadPointer(1);
        auto* ynR = buffer.getWritePointer(1);
    
        ynL[sample] = leftHiShelf.processAudioSample(xnL[sample]);
        ynR[sample] = rightHiShelf.processAudioSample(xnR[sample]);
        ynL[sample] = leftLoShelf.processAudioSample(ynL[sample]);
        ynR[sample] = rightLoShelf.processAudioSample(ynR[sample]);
    }
}

//==============================================================================
bool Phillips_Assignment1AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Phillips_Assignment1AudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void Phillips_Assignment1AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Phillips_Assignment1AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Phillips_Assignment1AudioProcessor();
}

/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class Phillips_Assignment1AudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    Phillips_Assignment1AudioProcessorEditor (Phillips_Assignment1AudioProcessor&);
    ~Phillips_Assignment1AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Phillips_Assignment1AudioProcessor& audioProcessor;
    
    juce::Slider tilt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> tiltAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Phillips_Assignment1AudioProcessorEditor)
};

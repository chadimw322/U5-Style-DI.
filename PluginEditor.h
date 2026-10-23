#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class U5StyleDIEditor final : public juce::AudioProcessorEditor
{
public:
    explicit U5StyleDIEditor (U5StyleDIProcessor&);
    ~U5StyleDIEditor() override = default;
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    U5StyleDIProcessor& processor;
    juce::Slider inputSlider, outputSlider;
    juce::ComboBox toneBox;
    juce::ToggleButton highCutButton, bypassButton;
    juce::Label title, inputLabel, outputLabel, toneLabel, footer;
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    std::unique_ptr<SliderAttachment> inputAttachment, outputAttachment;
    std::unique_ptr<ComboAttachment> toneAttachment;
    std::unique_ptr<ButtonAttachment> highCutAttachment, bypassAttachment;
    void styleKnob (juce::Slider&);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (U5StyleDIEditor)
};

#pragma once
#include <JuceHeader.h>
#include <array>

class U5StyleDIProcessor final : public juce::AudioProcessor
{
public:
    U5StyleDIProcessor();
    ~U5StyleDIProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}
    void getStateInformation (juce::MemoryBlock&) override;
    void setStateInformation (const void*, int) override;

    juce::AudioProcessorValueTreeState parameters;

private:
    using Filter = juce::dsp::IIR::Filter<float>;
    std::array<Filter, 2> lowShelf;
    std::array<Filter, 2> lowMid;
    std::array<Filter, 2> highMid;
    std::array<Filter, 2> highShelf;
    std::array<Filter, 2> highCut;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void updateToneFilters();
    double currentSampleRate = 44100.0;
    int lastTone = -1;
    bool lastHighCut = false;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (U5StyleDIProcessor)
};

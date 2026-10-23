#include "PluginProcessor.h"
#include "PluginEditor.h"

U5StyleDIProcessor::U5StyleDIProcessor()
    : AudioProcessor (BusesProperties()
        .withInput ("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      parameters (*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout U5StyleDIProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        "input", "Input Gain", juce::NormalisableRange<float>(-12.0f, 32.0f, 0.1f), 0.0f, " dB"));
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        "output", "Output", juce::NormalisableRange<float>(-24.0f, 12.0f, 0.1f), 0.0f, " dB"));
    layout.add (std::make_unique<juce::AudioParameterChoice>(
        "tone", "Tone Bank",
        juce::StringArray {"1 - Natural", "2 - Bass", "3 - Warm", "4 - Scoop", "5 - Presence", "6 - Full/Scoop"}, 0));
    layout.add (std::make_unique<juce::AudioParameterBool>("highcut", "High Cut", false));
    layout.add (std::make_unique<juce::AudioParameterBool>("bypass", "Bypass", false));
    return layout;
}

void U5StyleDIProcessor::prepareToPlay (double sampleRate, int)
{
    currentSampleRate = sampleRate;
    juce::dsp::ProcessSpec spec { sampleRate, 2048, (juce::uint32) getTotalNumOutputChannels() };
    for (auto* group : { &lowShelf, &lowMid, &highMid, &highShelf, &highCut })
        for (auto& f : *group)
            f.prepare (spec);
    updateToneFilters();
}

void U5StyleDIProcessor::releaseResources() {}

bool U5StyleDIProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto in = layouts.getMainInputChannelSet();
    const auto out = layouts.getMainOutputChannelSet();
    return in == out && (out == juce::AudioChannelSet::mono() || out == juce::AudioChannelSet::stereo());
}

void U5StyleDIProcessor::updateToneFilters()
{
    // Musical approximation of six passive tone-bank curves, not measured circuit models.
    const int tone = juce::roundToInt (parameters.getRawParameterValue("tone")->load());
    const bool hc = parameters.getRawParameterValue("highcut")->load() > 0.5f;
    if (tone == lastTone && hc == lastHighCut) return;
    lastTone = tone; lastHighCut = hc;

    struct Curve { float lowDb, lowMidDb, highMidDb, highDb; };
    static constexpr Curve curves[6] = {
        { 0.0f,  0.0f,  0.0f,  0.0f},  // Natural
        { 2.0f,  1.0f, -1.5f, -0.5f},  // Bass weight
        { 1.0f,  1.5f,  0.5f,  0.0f},  // Warm
        { 1.5f, -2.0f, -1.5f,  1.0f},  // Scoop
        { 0.0f, -0.5f,  2.0f,  1.5f},  // Presence
        { 2.0f, -2.0f, -1.0f,  1.5f}   // Full + scoop
    };
    const auto c = curves[juce::jlimit(0, 5, tone)];
    for (int ch = 0; ch < 2; ++ch)
    {
        lowShelf[(size_t)ch].coefficients = juce::dsp::IIR::Coefficients<float>::makeLowShelf(currentSampleRate, 90.0, 0.707f, juce::Decibels::decibelsToGain(c.lowDb));
        lowMid[(size_t)ch].coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(currentSampleRate, 260.0, 0.85f, juce::Decibels::decibelsToGain(c.lowMidDb));
        highMid[(size_t)ch].coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(currentSampleRate, 1100.0, 0.8f, juce::Decibels::decibelsToGain(c.highMidDb));
        highShelf[(size_t)ch].coefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf(currentSampleRate, 4200.0, 0.707f, juce::Decibels::decibelsToGain(c.highDb));
        highCut[(size_t)ch].coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(currentSampleRate, hc ? 8000.0 : 20000.0, 0.707f);
    }
}

void U5StyleDIProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int channels = juce::jmin (buffer.getNumChannels(), 2);
    if (parameters.getRawParameterValue("bypass")->load() > 0.5f)
        return;

    updateToneFilters();
    const float inGain = juce::Decibels::decibelsToGain(parameters.getRawParameterValue("input")->load());
    const float outGain = juce::Decibels::decibelsToGain(parameters.getRawParameterValue("output")->load());

    for (int ch = 0; ch < channels; ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        juce::dsp::AudioBlock<float> block (buffer);
        juce::dsp::ProcessContextReplacing<float> context (block);
        juce::ignoreUnused(context);
        for (int n = 0; n < buffer.getNumSamples(); ++n)
        {
            float x = data[n] * inGain;
            x = lowShelf[(size_t)ch].processSample(x);
            x = lowMid[(size_t)ch].processSample(x);
            x = highMid[(size_t)ch].processSample(x);
            x = highShelf[(size_t)ch].processSample(x);
            x = highCut[(size_t)ch].processSample(x);
            data[n] = x * outGain;
        }
    }
}

juce::AudioProcessorEditor* U5StyleDIProcessor::createEditor()
{
    return new U5StyleDIEditor (*this);
}

void U5StyleDIProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void U5StyleDIProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));
    if (xml && xml->hasTagName (parameters.state.getType()))
        parameters.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new U5StyleDIProcessor();
}

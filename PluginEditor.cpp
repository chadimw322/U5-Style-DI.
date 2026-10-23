#include "PluginEditor.h"

U5StyleDIEditor::U5StyleDIEditor (U5StyleDIProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setSize (520, 330);
    auto setupLabel = [](juce::Label& l, const juce::String& text)
    {
        l.setText(text, juce::dontSendNotification);
        l.setJustificationType(juce::Justification::centred);
        l.setColour(juce::Label::textColourId, juce::Colour(0xffe4e5e6));
        l.setFont(juce::Font(13.0f, juce::Font::bold));
    };
    setupLabel(title, "U5 STYLE DI  |  CLASS A INSPIRED");
    setupLabel(inputLabel, "INPUT GAIN");
    setupLabel(outputLabel, "OUTPUT");
    setupLabel(toneLabel, "TONE BANK");
    setupLabel(footer, "U5-inspired DSP • tone curves are approximations");
    addAndMakeVisible(title);
    addAndMakeVisible(inputLabel);
    addAndMakeVisible(outputLabel);
    addAndMakeVisible(toneLabel);
    addAndMakeVisible(footer);

    styleKnob(inputSlider);
    inputSlider.setRange(-12.0, 32.0, 0.1);
    inputSlider.setTextValueSuffix(" dB");
    styleKnob(outputSlider);
    outputSlider.setRange(-24.0, 12.0, 0.1);
    outputSlider.setTextValueSuffix(" dB");
    addAndMakeVisible(inputSlider);
    addAndMakeVisible(outputSlider);

    toneBox.addItemList({"1 - Natural", "2 - Bass", "3 - Warm", "4 - Scoop", "5 - Presence", "6 - Full/Scoop"}, 1);
    toneBox.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff3c3f42));
    toneBox.setColour(juce::ComboBox::textColourId, juce::Colour(0xfff0f0f0));
    toneBox.setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff8a8e91));
    addAndMakeVisible(toneBox);

    highCutButton.setButtonText("HIGH CUT");
    bypassButton.setButtonText("BYPASS");
    for (auto* b : { &highCutButton, &bypassButton })
    {
        b->setColour(juce::ToggleButton::textColourId, juce::Colour(0xffe4e5e6));
        b->setColour(juce::ToggleButton::tickColourId, juce::Colour(0xffbfc3c5));
        addAndMakeVisible(*b);
    }

    inputAttachment = std::make_unique<SliderAttachment>(processor.parameters, "input", inputSlider);
    outputAttachment = std::make_unique<SliderAttachment>(processor.parameters, "output", outputSlider);
    toneAttachment = std::make_unique<ComboAttachment>(processor.parameters, "tone", toneBox);
    highCutAttachment = std::make_unique<ButtonAttachment>(processor.parameters, "highcut", highCutButton);
    bypassAttachment = std::make_unique<ButtonAttachment>(processor.parameters, "bypass", bypassButton);
}

void U5StyleDIEditor::styleKnob (juce::Slider& s)
{
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 82, 22);
    s.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffb7bbbd));
    s.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff666a6d));
    s.setColour(juce::Slider::thumbColourId, juce::Colour(0xffe8e9ea));
    s.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xfff0f0f0));
    s.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff3c3f42));
    s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xff707477));
}

void U5StyleDIEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff303336));
    g.setColour(juce::Colour(0xff55595c));
    g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(10.0f), 9.0f);
    g.setColour(juce::Colour(0xff8c9093));
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(10.0f), 9.0f, 1.2f);
    g.setColour(juce::Colour(0xff252729));
    g.fillRect(24, 54, getWidth()-48, 1);
    g.fillRect(24, 238, getWidth()-48, 1);
}

void U5StyleDIEditor::resized()
{
    title.setBounds(20, 17, getWidth()-40, 28);
    inputLabel.setBounds(60, 62, 150, 22);
    outputLabel.setBounds(310, 62, 150, 22);
    inputSlider.setBounds(55, 83, 160, 130);
    outputSlider.setBounds(305, 83, 160, 130);
    toneLabel.setBounds(35, 218, 105, 24);
    toneBox.setBounds(135, 218, 230, 26);
    highCutButton.setBounds(365, 216, 95, 28);
    bypassButton.setBounds(215, 260, 100, 26);
    footer.setBounds(18, 295, getWidth()-36, 18);
}

#pragma once
#include <JuceHeader.h>
#include <array>

namespace horrorcastle {

class CurseInspectorComponent : public juce::Component, private juce::Timer
{
public:
    explicit CurseInspectorComponent(juce::AudioProcessorValueTreeState& state);
    ~CurseInspectorComponent() override;

    void selectLane(int laneIndex);
    int getSelectedLane() const noexcept { return selectedLane; }
    void setLiveActivity(float value);
    void setChainInfluence(float value);

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    juce::AudioProcessorValueTreeState& apvts;
    int selectedLane = -1;
    int attachedCharacterCurse = -1;
    float activity = 0.0f;
    float chainInfluence = 0.0f;
    float animation = 0.0f;
    juce::Image skinFrame;

    juce::Label title, routeLabel, behaviorTitle, behaviorBody, chainLabel;
    juce::ComboBox curseChoice;
    juce::Slider intensity, character;
    juce::Label intensityLabel, characterLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> curseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> intensityAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> characterAttachment;

    juce::String parameterPrefix() const;
    int readChoice(const juce::String& id, int fallback = 0) const;
    float readFloat(const juce::String& id, float fallback = 0.0f) const;
    juce::String sourceName(int index) const;
    juce::String destinationName(int index) const;
    juce::String curseName(int index) const;
    juce::String curseGlyph(int index) const;
    juce::String behavior(int index) const;
    juce::String characterParameter(int curseIndex) const;
    juce::String characterCaption(int curseIndex) const;
    juce::Colour curseColour(int index) const;
    void configureCharacterSlider(int curseIndex);
    void refreshText();
    void rebuildAttachments();
    void rebuildCharacterAttachment();
    void timerCallback() override;
    void drawSigil(juce::Graphics&, juce::Rectangle<float> area);
    void drawMeter(juce::Graphics&, juce::Rectangle<float> area, float value, const juce::String& caption);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CurseInspectorComponent)
};

} // namespace horrorcastle

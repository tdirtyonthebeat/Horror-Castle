#pragma once
#include <JuceHeader.h>
#include "CastleParameters.h"
#include "../UI/HexMatrixComponent.h"
#include "../UI/CurseInspectorComponent.h"
#include "../UI/Components/CastleHeaderComponent.h"
#include "../UI/Components/GrimoireComponent.h"

namespace horrorcastle {

class HorrorCastleProcessor;

class CastleLookAndFeel : public juce::LookAndFeel_V4 {
public:
    CastleLookAndFeel();
    void drawRotarySlider(juce::Graphics&, int, int, int, int, float, float, float, juce::Slider&) override;
    void drawLinearSlider(juce::Graphics&, int, int, int, int, float, float, float, juce::Slider::SliderStyle, juce::Slider&) override;
    void drawToggleButton(juce::Graphics&, juce::ToggleButton&, bool, bool) override;
    void drawComboBox(juce::Graphics&, int, int, bool, int, int, int, int, juce::ComboBox&) override;
    void positionComboBoxText(juce::ComboBox&, juce::Label&) override;
    void drawButtonBackground(juce::Graphics&, juce::Button&, const juce::Colour&, bool, bool) override;
    void drawButtonText(juce::Graphics&, juce::TextButton&, bool, bool) override;
};

class HorrorCastleEditor : public juce::AudioProcessorEditor, private juce::Timer {
public:
    explicit HorrorCastleEditor(HorrorCastleProcessor&);
    ~HorrorCastleEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    struct Control : juce::Component {
        std::unique_ptr<juce::Label> label;
        std::unique_ptr<juce::Slider> slider;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    };
    struct Choice : juce::Component {
        std::unique_ptr<juce::Label> label;
        std::unique_ptr<juce::ComboBox> box;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> attachment;
    };
    struct Toggle : juce::Component {
        std::unique_ptr<juce::ToggleButton> button;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> attachment;
    };
    struct ScenePanel : juce::Component {
        juce::Label title;
        juce::String accentKey;
        juce::Image frameImage;
        std::array<Control,3> genLevel, genShape, genTune;
        std::array<Choice,3> genType;
        Control noise, f1Cut, f1Res, f2Cut, f2Res, f1Drive, f2Drive;
        Choice route;
        Control master, balance, character;
        Toggle crossFm, crossRing;
        void setFrameImage(const juce::Image& image) { frameImage = image; repaint(); }
        void paint(juce::Graphics&) override;
        void resized() override;
        void bind(juce::AudioProcessorValueTreeState&, const char* scene);
    };

    HorrorCastleProcessor& processor;
    juce::AudioProcessorValueTreeState& apvts;
    CastleLookAndFeel look;
    CastleHeaderComponent header;
    ScenePanel crypt, tower;
    juce::Image backdropImage, ritualFrameImage, undercroftImage;
    juce::ImageComponent centerSpine;

    Choice ritualMode;
    Control ritualMix, ritualDepth, ritualDrive, ritualWidth, ritualFeedback;
    Control graveReverb, graveDelay, graveFeedback, graveTone, graveOutput, hexAmount;

    // Stone & Shadow secondary room: advanced cross-chamber controls + Spectral Corpse.
    Control corpsePosition, corpseRot, corpseFormant, corpseInharmonic;
    Control bloodFeed, aetherLeak, soulExchange, possessionHaunt;
    Toggle ritualsEnabled;
    Choice ritualsPattern, ritualsRate;
    Control ritualsBpm, ritualsGate, ritualsProbability, ritualsSwing, ritualsOctaves;
    juce::Label corpseTitle, corpseStatus, advancedCryptTitle, advancedTowerTitle;
    juce::Label ritualTitle, graveTitle, possessionTitle, ritualsTitle, undercroftTitle, status;

    juce::TextButton grimoireToggle { "GRIMOIRE" };
    juce::TextButton undercroftToggle { "UNDERCROFT" };
    GrimoireComponent grimoire;
    HexMatrixComponent hexMatrix;
    CurseInspectorComponent curseInspector;

    int inspectedLane = 0;
    bool uiReady = false;
    bool undercroftVisible = false;
    float atmospherePhase = 0.0f;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        ritualMixA, ritualDepthA, ritualDriveA, ritualWidthA, ritualFeedbackA,
        graveReverbA, graveDelayA, graveFeedbackA, graveToneA, graveOutputA, hexA;

    void makeControl(Control&, const juce::String&, const juce::String&);
    void makeChoice(Choice&, const juce::String&, const juce::String&);
    void makeToggle(Toggle&, const juce::String&, const juce::String&);
    void makeMasterControl(Control&, const juce::String&, const juce::String&);
    void setAccent(Control&, const juce::String&);
    void setAccent(Choice&, const juce::String&);
    void setAccent(Toggle&, const juce::String&);
    void timerCallback() override;
    void updateHexVisuals();
    void setGrimoireVisible(bool);
    void setUndercroftVisible(bool);
    void setAdvancedComponentsVisible(bool);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HorrorCastleEditor)
};

} // namespace horrorcastle

#pragma once
#include <JuceHeader.h>
#include "../HorrorCastle/CastleParameters.h"
#include <array>
#include <memory>

namespace horrorcastle {

class LivingEngineMacroPanel final : public juce::Component, private juce::Timer
{
public:
    explicit LivingEngineMacroPanel(juce::AudioProcessorValueTreeState& state)
        : apvts(state)
    {
        setOpaque(false);
        setupRoom(crypt, true);
        setupRoom(tower, false);
        startTimerHz(10);
        refreshBindings(true);
    }

    void paint(juce::Graphics& g) override
    {
        auto r = getLocalBounds().toFloat();
        g.setColour(juce::Colour(0xee05070a));
        g.fillRoundedRectangle(r, 7.0f);
        g.setColour(juce::Colour(0xff71563e));
        g.drawRoundedRectangle(r.reduced(0.5f), 7.0f, 1.3f);
        g.setColour(juce::Colour(0xffb89d72).withAlpha(.22f));
        g.drawRoundedRectangle(r.reduced(3.0f), 5.0f, .8f);

        const float mid = r.getCentreX();
        g.setColour(juce::Colour(0xff331a18).withAlpha(.72f));
        g.fillRect(r.getX() + 8.0f, r.getY() + 8.0f, mid - r.getX() - 12.0f, r.getHeight() - 16.0f);
        g.setColour(juce::Colour(0xff24162c).withAlpha(.72f));
        g.fillRect(mid + 4.0f, r.getY() + 8.0f, r.getRight() - mid - 12.0f, r.getHeight() - 16.0f);
        g.setColour(juce::Colour(0xff71563e).withAlpha(.55f));
        g.drawLine(mid, r.getY() + 8.0f, mid, r.getBottom() - 8.0f, 1.0f);

        g.setColour(juce::Colour(0xffd75b55));
        g.setFont(juce::Font(juce::FontOptions(14.5f)).boldened());
        g.drawText("CRYPT ENGINE PHYSICS  //  " + crypt.engineName,
                   18, 8, getWidth() / 2 - 28, 22, juce::Justification::centredLeft);
        g.setColour(juce::Colour(0xffba86d7));
        g.drawText("TOWER ENGINE PHYSICS  //  " + tower.engineName,
                   getWidth() / 2 + 18, 8, getWidth() / 2 - 36, 22, juce::Justification::centredLeft);

        g.setFont(juce::FontOptions(9.5f));
        g.setColour(juce::Colour(0xffb7aa98).withAlpha(.78f));
        g.drawFittedText("Macros follow the loudest active Living Engine in each room. They are host-automatable DSP parameters, not cosmetic controls.",
                         18, getHeight() - 24, getWidth() - 36, 16, juce::Justification::centred, 1);
    }

    void resized() override
    {
        const int half = getWidth() / 2;
        layoutRoom(crypt, 14, 34, half - 24, getHeight() - 66);
        layoutRoom(tower, half + 10, 34, half - 24, getHeight() - 66);
    }

private:
    struct MacroSpec
    {
        const char* engine;
        const char* ids[3];
        const char* labels[3];
    };

    static constexpr MacroSpec cryptSpecs[3] = {
        { "ABYSS", { "living.abyss.depth", "living.abyss.pressure", "living.abyss.dread" }, { "DEPTH", "PRESSURE", "DREAD" } },
        { "POLTERGEIST", { "living.poltergeist.charge", "living.poltergeist.arc", "living.poltergeist.instability" }, { "CHARGE", "ARC", "INSTABILITY" } },
        { "VORTEX", { "living.vortex.turbulence", "living.vortex.pressure", "living.vortex.collapse" }, { "TURBULENCE", "PRESSURE", "COLLAPSE" } }
    };

    static constexpr MacroSpec towerSpecs[3] = {
        { "MIRROR", { "living.mirror.reflection", "living.mirror.smear", "living.mirror.fracture" }, { "REFLECTION", "SMEAR", "FRACTURE" } },
        { "AURORA", { "living.aurora.field", "living.aurora.radiance", "living.aurora.instability" }, { "FIELD", "RADIANCE", "INSTABILITY" } },
        { "SIREN", { "living.siren.aperture", "living.siren.breath", "living.siren.edge" }, { "APERTURE", "BREATH", "EDGE" } }
    };

    struct Room
    {
        bool crypt = false;
        int specIndex = -1;
        juce::String engineName { "NONE" };
        std::array<juce::Label, 3> labels;
        std::array<juce::Slider, 3> sliders;
        std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, 3> attachments;
    };

    juce::AudioProcessorValueTreeState& apvts;
    Room crypt, tower;

    void setupRoom(Room& room, bool isCrypt)
    {
        room.crypt = isCrypt;
        const auto accent = isCrypt ? juce::Colour(0xffd75b55) : juce::Colour(0xffba86d7);
        for (int i = 0; i < 3; ++i)
        {
            auto& l = room.labels[(size_t)i];
            l.setJustificationType(juce::Justification::centred);
            l.setColour(juce::Label::textColourId, accent.withAlpha(.92f));
            l.setFont(juce::Font(juce::FontOptions(9.5f)).boldened());
            addAndMakeVisible(l);

            auto& s = room.sliders[(size_t)i];
            s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
            s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 58, 16);
            s.setColour(juce::Slider::rotarySliderFillColourId, accent);
            s.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff302b2d));
            s.setColour(juce::Slider::thumbColourId, juce::Colour(0xffd9cbb5));
            s.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffd9cbb5));
            s.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff050608));
            s.setColour(juce::Slider::textBoxOutlineColourId, accent.withAlpha(.35f));
            addAndMakeVisible(s);
        }
    }

    void layoutRoom(Room& room, int x, int y, int w, int h)
    {
        const int cell = w / 3;
        for (int i = 0; i < 3; ++i)
        {
            room.labels[(size_t)i].setBounds(x + i * cell, y, cell, 16);
            room.sliders[(size_t)i].setBounds(x + i * cell + 5, y + 17, cell - 10, h - 17);
        }
    }

    int chooseSpec(const char* scene) const
    {
        int best = -1;
        float bestLevel = -1.0f;
        for (int slot = 1; slot <= 3; ++slot)
        {
            const auto typeId = param::id(scene, slot, "type");
            const auto levelId = param::id(scene, slot, "level");
            const auto enabledId = param::id(scene, slot, "enabled");
            auto* type = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(typeId));
            auto* level = apvts.getRawParameterValue(levelId);
            auto* enabled = apvts.getRawParameterValue(enabledId);
            if (type == nullptr || type->getIndex() < 15 || type->getIndex() > 17) continue;
            const float v = (enabled == nullptr || enabled->load() >= .5f) && level != nullptr ? level->load() : 0.0f;
            if (v > bestLevel) { bestLevel = v; best = type->getIndex() - 15; }
        }
        if (best >= 0) return best;
        if (auto* type = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(param::id(scene, 1, "type"))))
            if (type->getIndex() >= 15 && type->getIndex() <= 17) return type->getIndex() - 15;
        return 0;
    }

    void bindRoom(Room& room, int specIndex)
    {
        if (room.specIndex == specIndex) return;
        room.specIndex = specIndex;
        const auto& spec = room.crypt ? cryptSpecs[specIndex] : towerSpecs[specIndex];
        room.engineName = spec.engine;
        for (int i = 0; i < 3; ++i)
        {
            room.attachments[(size_t)i].reset();
            room.labels[(size_t)i].setText(spec.labels[i], juce::dontSendNotification);
            room.attachments[(size_t)i] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                apvts, spec.ids[i], room.sliders[(size_t)i]);
        }
        repaint();
    }

    void refreshBindings(bool force = false)
    {
        const int c = chooseSpec("crypt");
        const int t = chooseSpec("tower");
        if (force) { crypt.specIndex = tower.specIndex = -1; }
        bindRoom(crypt, c);
        bindRoom(tower, t);
    }

    void timerCallback() override { refreshBindings(false); }
};

} // namespace horrorcastle

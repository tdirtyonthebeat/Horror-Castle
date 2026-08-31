#pragma once
#include <JuceHeader.h>
#include "../HorrorCastle/CastleParameters.h"

namespace horrorcastle {

// Read-only visual identity layer for the v1.3 Living Engines editor.
// It mirrors APVTS state only: no DSP ownership, no duplicate parameter state.
class LivingCastleChrome final : public juce::Component, private juce::Timer
{
public:
    explicit LivingCastleChrome(juce::AudioProcessorValueTreeState& state)
        : apvts(state)
    {
        setInterceptsMouseClicks(false, false);
        startTimerHz(15);
        refresh();
    }

    void paint(juce::Graphics& g) override
    {
        using namespace juce;
        const auto r = getLocalBounds().toFloat();
        const float cx = r.getCentreX();

        ColourGradient bg(Colour(0xff040506), 0.0f, 0.0f,
                          Colour(0xff0b0b10), r.getRight(), r.getBottom(), false);
        g.setGradientFill(bg);
        g.fillRect(r);

        drawStoneBand(g, Rectangle<float>(0, 0, r.getWidth(), 92), Colour(0xff8d6a42), .23f);
        drawStoneBand(g, Rectangle<float>(10, 94, r.getWidth() * .495f - 12, 348), Colour(0xffb62d2b), .20f);
        drawStoneBand(g, Rectangle<float>(r.getWidth() * .505f, 94, r.getWidth() * .495f - 10, 348), Colour(0xff8f4bb5), .20f);
        drawStoneBand(g, Rectangle<float>(10, 446, r.getWidth() - 20, 126), Colour(0xff94734f), .16f);
        drawStoneBand(g, Rectangle<float>(10, 576, r.getWidth() - 20, 196), Colour(0xff6f3e7c), .13f);
        drawStoneBand(g, Rectangle<float>(10, 776, r.getWidth() - 20, r.getHeight() - 786), Colour(0xff8f4bb5), .13f);

        // Central gothic spine.
        g.setColour(Colour(0xff020304).withAlpha(.94f));
        g.fillRect(cx - 7.0f, 92.0f, 14.0f, 350.0f);
        g.setColour(Colour(0xff826344).withAlpha(.58f));
        g.drawLine(cx, 100.0f, cx, 436.0f, 1.0f);
        for (int y = 116; y < 430; y += 24)
        {
            g.setColour(Colour(0xff6b1d1d).withAlpha(.25f));
            g.fillEllipse(cx - 2.5f, (float)y, 5.0f, 5.0f);
        }

        drawTitle(g, "CRYPT", Rectangle<float>(26, 100, r.getWidth() * .46f, 27), Colour(0xffcf5751));
        drawTitle(g, "TOWER", Rectangle<float>(cx + 24, 100, r.getWidth() * .43f, 27), Colour(0xffb885d6));

        // The central UX idea: every slot visibly announces what species powers it,
        // what kind of synthesis it uses, and what SHAPE means for that species.
        drawEngineRow(g, "crypt", 26.0f, 132.0f, cx - 44.0f, cryptSlots, Colour(0xffd85b55));
        drawEngineRow(g, "tower", cx + 24.0f, 132.0f, r.getRight() - 26.0f, towerSlots, Colour(0xffb985dd));

        drawTitle(g, "RITUAL BUS", Rectangle<float>(26, 454, 220, 24), Colour(0xffc76a5e));
        drawTitle(g, "GRAVE MASTER", Rectangle<float>(r.getWidth() * .53f, 454, 220, 24), Colour(0xffb887d0));
        drawTitle(g, "HEX // THE CURSE ENGINE", Rectangle<float>(26, 584, 360, 24), Colour(0xffc66b71));
        drawTitle(g, "GRIMOIRE", Rectangle<float>(26, 784, 260, 24), Colour(0xffb885d6));

        const auto cryptReadout = Rectangle<float>(26, 405, cx - 44, 25);
        const auto towerReadout = Rectangle<float>(cx + 24, 405, r.getRight() - cx - 50, 25);
        drawReadout(g, cryptReadout, "CRYPT // " + activeSummary(cryptSlots), Colour(0xffd86059));
        drawReadout(g, towerReadout, "TOWER // " + activeSummary(towerSlots), Colour(0xffb986dd));

        // Gargoyle guide niche. This is intentionally read-only until the full
        // interactive helper layer is wired; the copy is contextual and useful now.
        auto guide = Rectangle<float>(r.getWidth() - 300.0f, r.getHeight() - 154.0f, 278.0f, 132.0f);
        drawStoneBand(g, guide, Colour(0xff8b6a47), .18f);
        auto garg = guide.removeFromLeft(88).reduced(8);
        drawGargoyle(g, garg);
        g.setColour(Colour(0xffd5c8b2));
        g.setFont(FontOptions(13.0f));
        g.drawText("GARGOYLE GUIDE", guide.removeFromTop(25).toNearestInt(), Justification::centredLeft);
        g.setColour(Colour(0xffa99f91));
        g.setFont(FontOptions(10.5f));
        g.drawFittedText("Each engine has its own physics.\nThe card above each generator tells\nyou what SHAPE controls for it.",
                         guide.reduced(2).toNearestInt(), Justification::topLeft, 4);
    }

private:
    struct SlotInfo
    {
        juce::String name;
        juce::String family;
        juce::String shapeMeaning;
        float level = 0.0f;
        bool enabled = true;
    };

    juce::AudioProcessorValueTreeState& apvts;
    std::array<SlotInfo, 3> cryptSlots;
    std::array<SlotInfo, 3> towerSlots;

    static juce::String familyFor(bool crypt, int index)
    {
        static const char* common[] = {
            "Virtual Analog", "Wavetable", "Ritual FM", "Phase Modulation",
            "Vector Morph", "Chip / Digital", "Noise", "Resonator"
        };
        if (index >= 0 && index < 8) return common[index];
        if (crypt)
        {
            static const char* families[] = {
                "Subterranean Pressure", "Spectral Resynthesis", "Bone Resonance", "Rotational Coupling",
                "Breath / Air Column", "Body Resonance", "Marrow Exciter", "Waveguide Abyss",
                "Electrostatic Haunt", "Chaotic Fluid"
            };
            const int i = index - 8;
            return (i >= 0 && i < 10) ? families[i] : "Unknown Crypt Engine";
        }
        static const char* families[] = {
            "Bell Glass", "Spectral Spire", "Astral FM", "Prismatic Refraction",
            "Reliquary Resonance", "Choir Body", "Orbital Resonance", "Spectral Mirror",
            "Electromagnetic Field", "Air-Jet Siren"
        };
        const int i = index - 8;
        return (i >= 0 && i < 10) ? families[i] : "Unknown Tower Engine";
    }

    static juce::String shapeFor(bool crypt, int index)
    {
        static const char* common[] = {
            "WAVE", "SCAN", "INDEX", "PHASE", "MORPH", "BITS", "COLOR", "BODY"
        };
        if (index >= 0 && index < 8) return common[index];
        if (crypt)
        {
            static const char* shape[] = {
                "DEPTH", "POSITION", "BONE", "ROTATION", "BREATH", "LID", "MARROW", "PRESSURE", "CHARGE", "TURBULENCE"
            };
            const int i = index - 8;
            return (i >= 0 && i < 10) ? shape[i] : "SHAPE";
        }
        static const char* shape[] = {
            "GLASS", "HEIGHT", "INDEX", "REFRACT", "RESONANCE", "CHOIR", "ORBIT", "REFLECTION", "RADIANCE", "APERTURE"
        };
        const int i = index - 8;
        return (i >= 0 && i < 10) ? shape[i] : "SHAPE";
    }

    void refresh()
    {
        refreshScene("crypt", true, cryptSlots);
        refreshScene("tower", false, towerSlots);
        repaint();
    }

    void refreshScene(const char* scene, bool isCrypt, std::array<SlotInfo, 3>& slots)
    {
        for (int slot = 1; slot <= 3; ++slot)
        {
            auto& out = slots[(size_t)(slot - 1)];
            const auto typeId = param::id(scene, slot, "type");
            int index = 0;
            if (auto* p = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(typeId)))
            {
                index = juce::jlimit(0, p->choices.size() - 1, p->getIndex());
                out.name = p->choices[index];
            }
            out.family = familyFor(isCrypt, index);
            out.shapeMeaning = shapeFor(isCrypt, index);
            if (auto* raw = apvts.getRawParameterValue(param::id(scene, slot, "level"))) out.level = juce::jlimit(0.0f, 1.0f, raw->load());
            if (auto* raw = apvts.getRawParameterValue(param::id(scene, slot, "enabled"))) out.enabled = raw->load() >= 0.5f;
        }
    }

    void timerCallback() override { refresh(); }

    static void drawEngineRow(juce::Graphics& g, const char*, float x0, float y, float right,
                              const std::array<SlotInfo, 3>& slots, juce::Colour accent)
    {
        const float gap = 8.0f;
        const float total = right - x0;
        const float w = (total - gap * 2.0f) / 3.0f;
        for (int i = 0; i < 3; ++i)
        {
            auto card = juce::Rectangle<float>(x0 + i * (w + gap), y, w, 76.0f);
            g.setColour(juce::Colour(0xff050608).withAlpha(.91f));
            g.fillRoundedRectangle(card, 4.0f);
            g.setColour(accent.withAlpha(slots[(size_t)i].level > .01f ? .62f : .24f));
            g.drawRoundedRectangle(card.reduced(.5f), 4.0f, slots[(size_t)i].level > .01f ? 1.4f : .8f);

            auto head = card.reduced(8.0f, 6.0f);
            g.setColour(accent.withAlpha(slots[(size_t)i].enabled ? .96f : .38f));
            g.setFont(juce::Font(juce::FontOptions(12.0f)).boldened());
            g.drawFittedText("GEN " + juce::String(i + 1) + " // " + slots[(size_t)i].name,
                             head.removeFromTop(18).toNearestInt(), juce::Justification::centredLeft, 1);
            g.setColour(juce::Colour(0xffc7bcad).withAlpha(.78f));
            g.setFont(juce::FontOptions(9.5f));
            g.drawFittedText(slots[(size_t)i].family,
                             head.removeFromTop(17).toNearestInt(), juce::Justification::centredLeft, 1);
            g.setColour(accent.withAlpha(.72f));
            g.setFont(juce::FontOptions(8.7f));
            g.drawFittedText("SHAPE = " + slots[(size_t)i].shapeMeaning + "   •   LEVEL " + juce::String(slots[(size_t)i].level, 2),
                             head.removeFromTop(17).toNearestInt(), juce::Justification::centredLeft, 1);
        }
    }

    static juce::String activeSummary(const std::array<SlotInfo, 3>& slots)
    {
        juce::StringArray names;
        for (const auto& s : slots)
            if (s.enabled && s.level > .01f) names.add(s.name);
        return names.isEmpty() ? "NO ACTIVE ENGINES" : names.joinIntoString(" + ");
    }

    static void drawStoneBand(juce::Graphics& g, juce::Rectangle<float> r, juce::Colour accent, float alpha)
    {
        g.setColour(juce::Colour(0xff08090c).withAlpha(.97f));
        g.fillRoundedRectangle(r, 4.0f);
        g.setColour(juce::Colour(0xff29272a));
        g.drawRoundedRectangle(r.reduced(.5f), 4.0f, 1.2f);
        g.setColour(accent.withAlpha(alpha));
        g.drawRoundedRectangle(r.reduced(2.0f), 3.0f, 1.0f);
        g.setColour(juce::Colour(0xff000000).withAlpha(.44f));
        for (float y = r.getY() + 10; y < r.getBottom(); y += 17)
            g.drawLine(r.getX() + 5, y, r.getRight() - 5, y, .35f);
    }

    static void drawTitle(juce::Graphics& g, const juce::String& text, juce::Rectangle<float> r, juce::Colour accent)
    {
        g.setColour(accent.withAlpha(.96f));
        g.setFont(juce::Font(juce::FontOptions(17.0f)).boldened());
        g.drawText(text, r.toNearestInt(), juce::Justification::centredLeft);
        g.setColour(accent.withAlpha(.35f));
        g.drawLine(r.getX(), r.getBottom() - 1, r.getRight(), r.getBottom() - 1, .8f);
    }

    static void drawReadout(juce::Graphics& g, juce::Rectangle<float> r, const juce::String& text, juce::Colour accent)
    {
        g.setColour(juce::Colour(0xff030406).withAlpha(.90f));
        g.fillRoundedRectangle(r, 3.0f);
        g.setColour(accent.withAlpha(.32f));
        g.drawRoundedRectangle(r, 3.0f, .8f);
        g.setColour(accent.withAlpha(.90f));
        g.setFont(juce::FontOptions(10.0f));
        g.drawFittedText(text, r.reduced(8, 2).toNearestInt(), juce::Justification::centredLeft, 1);
    }

    static void drawGargoyle(juce::Graphics& g, juce::Rectangle<float> r)
    {
        const auto c = r.getCentre();
        g.setColour(juce::Colour(0xff17191c));
        g.fillEllipse(r.reduced(13));
        juce::Path wing;
        wing.startNewSubPath(c.x - 13, c.y - 5); wing.lineTo(r.getX() + 3, r.getY() + 10); wing.lineTo(c.x - 20, c.y + 14); wing.closeSubPath();
        wing.startNewSubPath(c.x + 13, c.y - 5); wing.lineTo(r.getRight() - 3, r.getY() + 10); wing.lineTo(c.x + 20, c.y + 14); wing.closeSubPath();
        g.setColour(juce::Colour(0xff292b2f)); g.fillPath(wing);
        g.setColour(juce::Colour(0xff5b5d61)); g.strokePath(wing, juce::PathStrokeType(1.0f));
        g.setColour(juce::Colour(0xffb72222));
        g.fillEllipse(c.x - 8, c.y - 4, 4, 3); g.fillEllipse(c.x + 4, c.y - 4, 4, 3);
        g.setColour(juce::Colour(0xff826344).withAlpha(.55f));
        g.drawEllipse(r.reduced(5), 1.0f);
    }
};

} // namespace horrorcastle

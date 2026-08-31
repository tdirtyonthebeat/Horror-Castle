#pragma once
#include <JuceHeader.h>
#include "../HorrorCastle/CastleParameters.h"

namespace horrorcastle {

// Decorative/read-only chrome for the v1.3 Living Engines editor.  This layer
// deliberately owns no DSP state: it mirrors APVTS values and sits behind the
// proven editor controls, so a visual redesign cannot silently change sound.
class LivingCastleChrome final : public juce::Component, private juce::Timer
{
public:
    explicit LivingCastleChrome(juce::AudioProcessorValueTreeState& state)
        : apvts(state)
    {
        setInterceptsMouseClicks(false, false);
        startTimerHz(12);
        refreshEngineNames();
    }

    void paint(juce::Graphics& g) override
    {
        using namespace juce;
        const auto r = getLocalBounds().toFloat();

        ColourGradient bg(Colour(0xff050608), 0.0f, 0.0f,
                          Colour(0xff0b0b10), r.getRight(), r.getBottom(), false);
        g.setGradientFill(bg);
        g.fillRect(r);

        // Architectural bands inspired by the approved concept: title/navigation,
        // twin engine rooms, ritual/grave strip, modulation floor and Grimoire.
        drawStoneBand(g, Rectangle<float>(0, 0, r.getWidth(), 92), Colour(0xff8d6a42), .23f);
        drawStoneBand(g, Rectangle<float>(10, 94, r.getWidth() * .495f - 12, 348), Colour(0xffb62d2b), .20f);
        drawStoneBand(g, Rectangle<float>(r.getWidth() * .505f, 94, r.getWidth() * .495f - 10, 348), Colour(0xff8f4bb5), .20f);
        drawStoneBand(g, Rectangle<float>(10, 446, r.getWidth() - 20, 126), Colour(0xff94734f), .16f);
        drawStoneBand(g, Rectangle<float>(10, 576, r.getWidth() - 20, 196), Colour(0xff6f3e7c), .13f);
        drawStoneBand(g, Rectangle<float>(10, 776, r.getWidth() - 20, r.getHeight() - 786), Colour(0xff8f4bb5), .13f);

        // Central gothic spine separates CRYPT and TOWER without sacrificing controls.
        const float cx = r.getCentreX();
        g.setColour(Colour(0xff020304).withAlpha(.92f));
        g.fillRect(cx - 7.0f, 92.0f, 14.0f, 350.0f);
        g.setColour(Colour(0xff826344).withAlpha(.55f));
        g.drawLine(cx, 100.0f, cx, 436.0f, 1.0f);
        for (int y = 116; y < 430; y += 24)
        {
            g.setColour(Colour(0xff6b1d1d).withAlpha(.25f));
            g.fillEllipse(cx - 2.5f, (float)y, 5.0f, 5.0f);
        }

        // Section engraving. Existing child controls remain on top of this chrome.
        drawTitle(g, "CRYPT", Rectangle<float>(26, 102, r.getWidth() * .46f, 28), Colour(0xffcf5751));
        drawTitle(g, "TOWER", Rectangle<float>(cx + 24, 102, r.getWidth() * .43f, 28), Colour(0xffb885d6));
        drawTitle(g, "RITUAL BUS", Rectangle<float>(26, 454, 220, 24), Colour(0xffc76a5e));
        drawTitle(g, "GRAVE MASTER", Rectangle<float>(r.getWidth() * .53f, 454, 220, 24), Colour(0xffb887d0));
        drawTitle(g, "HEX // THE CURSE ENGINE", Rectangle<float>(26, 584, 360, 24), Colour(0xffc66b71));
        drawTitle(g, "GRIMOIRE", Rectangle<float>(26, 784, 260, 24), Colour(0xffb885d6));

        // Living-engine readout teaches users what is actually powering each slot.
        const auto cryptReadout = Rectangle<float>(26, 132, r.getWidth() * .44f, 24);
        const auto towerReadout = Rectangle<float>(cx + 24, 132, r.getWidth() * .42f, 24);
        drawReadout(g, cryptReadout, "CRYPT ENGINES  //  " + cryptNames, Colour(0xffd86059));
        drawReadout(g, towerReadout, "TOWER ENGINES  //  " + towerNames, Colour(0xffb986dd));

        // Bottom-right helper niche: intentionally subtle and non-interactive here.
        const auto guide = Rectangle<float>(r.getWidth() - 300.0f, r.getHeight() - 154.0f, 278.0f, 132.0f);
        drawStoneBand(g, guide, Colour(0xff8b6a47), .18f);
        drawGargoyle(g, guide.removeFromLeft(88).reduced(8));
        g.setColour(Colour(0xffd5c8b2));
        g.setFont(FontOptions(13.0f));
        g.drawText("GARGOYLE GUIDE", guide.removeFromTop(25).toNearestInt(), Justification::centredLeft);
        g.setColour(Colour(0xffa99f91));
        g.setFont(FontOptions(10.5f));
        g.drawFittedText("INIT is an engine audition patch.\nRaise GEN 2 or GEN 3 to hear\nthe other Living Engines.",
                         guide.reduced(2).toNearestInt(), Justification::topLeft, 4);
    }

private:
    juce::AudioProcessorValueTreeState& apvts;
    juce::String cryptNames { "ABYSS  •  POLTERGEIST  •  VORTEX" };
    juce::String towerNames { "MIRROR  •  AURORA  •  SIREN" };

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

    void timerCallback() override
    {
        refreshEngineNames();
        repaint();
    }

    void refreshEngineNames()
    {
        cryptNames = readScene("crypt");
        towerNames = readScene("tower");
    }

    juce::String readScene(const char* scene) const
    {
        juce::StringArray names;
        for (int slot = 1; slot <= 3; ++slot)
        {
            const auto id = param::id(scene, slot, "type");
            if (auto* p = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(id)))
            {
                const int index = juce::jlimit(0, p->choices.size() - 1, p->getIndex());
                names.add(p->choices[index]);
            }
        }
        return names.joinIntoString("  •  ");
    }
};

} // namespace horrorcastle

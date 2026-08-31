#pragma once
#include "HorrorCastleEditor.h"
#include "../UI/NervousSystemComponent.h"
#include "../UI/LivingCastleChrome.h"
#include <cmath>

namespace horrorcastle {

// Production wrapper for the v1.3 Living Engines instrument.  The read-only
// chrome paints the approved visual hierarchy behind the established controls,
// while the Nervous System remains a self-contained functional overlay.
class LivingCastleEditor final : public HorrorCastleEditor
{
public:
    explicit LivingCastleEditor(HorrorCastleProcessor& p)
        : HorrorCastleEditor(p), chrome(p.getParameterState()), nervousSystem(p)
    {
        addAndMakeVisible(chrome);
        chrome.toBack();

        nervousToggle.setButtonText("NERVOUS SYSTEM");
        nervousToggle.setColour(juce::TextButton::buttonColourId,juce::Colour(0xff080a0d));
        nervousToggle.setColour(juce::TextButton::textColourOffId,juce::Colour(0xffb9d7dc));
        nervousToggle.setColour(juce::TextButton::textColourOnId,juce::Colour(0xffd6eef0));
        addAndMakeVisible(nervousToggle);
        addAndMakeVisible(nervousSystem);
        nervousSystem.setVisible(false);
        nervousToggle.onClick=[this]{
            const bool show=!nervousSystem.isVisible();
            nervousSystem.setVisible(show);
            nervousToggle.setButtonText(show?"CLOSE NERVOUS SYSTEM":"NERVOUS SYSTEM");
            if(show)nervousSystem.toFront(false);
        };
        resized();
    }

    void resized() override
    {
        HorrorCastleEditor::resized();
        chrome.setBounds(getLocalBounds());
        chrome.toBack();

        const float sx=(float)getWidth()/1448.0f;
        const float sy=(float)getHeight()/1086.0f;
        auto bounds=[&](int x,int y,int w,int h){return juce::Rectangle<int>((int)std::lround(x*sx),(int)std::lround(y*sy),(int)std::lround(w*sx),(int)std::lround(h*sy));};

        // Approved concept: Nervous System is a first-class navigation target,
        // not a hidden engineering panel.
        nervousToggle.setBounds(bounds(963,55,176,27));
        nervousSystem.setBounds(bounds(470,470,520,220));
    }

private:
    LivingCastleChrome chrome;
    NervousSystemComponent nervousSystem;
    juce::TextButton nervousToggle;
};

} // namespace horrorcastle

#pragma once
#include "HorrorCastleEditor.h"
#include "../UI/NervousSystemComponent.h"
#include "../UI/LivingCastleChrome.h"
#include "../UI/LivingEngineMacroPanel.h"
#include <cmath>

namespace horrorcastle {

// Production wrapper for the v1.3 Living Engines instrument. The base editor
// remains the proven control surface; Living Physics and the Nervous System are
// explicit, self-contained overlays so new architecture cannot destabilize it.
class LivingCastleEditor final : public HorrorCastleEditor
{
public:
    explicit LivingCastleEditor(HorrorCastleProcessor& p)
        : HorrorCastleEditor(p), chrome(p.getParameterState()), physics(p.getParameterState()), nervousSystem(p)
    {
        addAndMakeVisible(chrome);
        chrome.toBack();

        physicsToggle.setButtonText("ENGINE PHYSICS");
        physicsToggle.setColour(juce::TextButton::buttonColourId,juce::Colour(0xff080a0d));
        physicsToggle.setColour(juce::TextButton::textColourOffId,juce::Colour(0xffd1a39d));
        physicsToggle.setColour(juce::TextButton::textColourOnId,juce::Colour(0xffffc5b9));
        addAndMakeVisible(physicsToggle);
        addAndMakeVisible(physics);
        physics.setVisible(false);

        nervousToggle.setButtonText("NERVOUS SYSTEM");
        nervousToggle.setColour(juce::TextButton::buttonColourId,juce::Colour(0xff080a0d));
        nervousToggle.setColour(juce::TextButton::textColourOffId,juce::Colour(0xffb9d7dc));
        nervousToggle.setColour(juce::TextButton::textColourOnId,juce::Colour(0xffd6eef0));
        addAndMakeVisible(nervousToggle);
        addAndMakeVisible(nervousSystem);
        nervousSystem.setVisible(false);

        physicsToggle.onClick=[this]{
            const bool show=!physics.isVisible();
            nervousSystem.setVisible(false);
            nervousToggle.setButtonText("NERVOUS SYSTEM");
            physics.setVisible(show);
            physicsToggle.setButtonText(show?"CLOSE ENGINE PHYSICS":"ENGINE PHYSICS");
            if(show)physics.toFront(false);
        };
        nervousToggle.onClick=[this]{
            const bool show=!nervousSystem.isVisible();
            physics.setVisible(false);
            physicsToggle.setButtonText("ENGINE PHYSICS");
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

        physicsToggle.setBounds(bounds(775,55,176,27));
        nervousToggle.setBounds(bounds(963,55,176,27));
        physics.setBounds(bounds(176,462,1096,238));
        nervousSystem.setBounds(bounds(470,470,520,220));
    }

private:
    LivingCastleChrome chrome;
    LivingEngineMacroPanel physics;
    NervousSystemComponent nervousSystem;
    juce::TextButton physicsToggle;
    juce::TextButton nervousToggle;
};

} // namespace horrorcastle

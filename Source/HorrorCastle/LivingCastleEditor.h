#pragma once
#include "HorrorCastleEditor.h"
#include "../UI/NervousSystemComponent.h"

namespace horrorcastle {

// Thin production wrapper: preserves the proven Stone & Shadow editor layout
// and adds the Nervous System as a self-contained overlay instead of disturbing
// HorrorCastleEditor::resized().
class LivingCastleEditor final : public HorrorCastleEditor
{
public:
    explicit LivingCastleEditor(HorrorCastleProcessor& p)
        : HorrorCastleEditor(p), nervousSystem(p)
    {
        nervousToggle.setButtonText("NERVOUS SYSTEM");
        nervousToggle.setColour(juce::TextButton::buttonColourId,juce::Colour(0xff09090d));
        nervousToggle.setColour(juce::TextButton::textColourOffId,juce::Colour(0xffd2ba76));
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
        const float sx=(float)getWidth()/1448.0f;
        const float sy=(float)getHeight()/1086.0f;
        auto bounds=[&](int x,int y,int w,int h){return juce::Rectangle<int>((int)std::lround(x*sx),(int)std::lround(y*sy),(int)std::lround(w*sx),(int)std::lround(h*sy));};
        nervousToggle.setBounds(bounds(952,28,158,26));
        nervousSystem.setBounds(bounds(468,650,512,206));
    }

private:
    NervousSystemComponent nervousSystem;
    juce::TextButton nervousToggle;
};

} // namespace horrorcastle

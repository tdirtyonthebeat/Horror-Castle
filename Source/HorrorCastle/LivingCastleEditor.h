#pragma once
#include "HorrorCastleEditor.h"
#include "../UI/NervousSystemComponent.h"
#include "../UI/LivingCastleChrome.h"
#include "../UI/LivingEngineMacroPanel.h"
#include "../UI/NecromancerVisuals.h"
#include "../UI/CreaturePedestalComponent.h"
#include <cmath>

namespace horrorcastle {

// Production wrapper for the v1.3 Living Engines instrument. The base editor
// remains the proven control surface; Living Physics and the Nervous System are
// explicit, self-contained overlays so new architecture cannot destabilize it.
class LivingCastleEditor final : public HorrorCastleEditor
{
public:
    explicit LivingCastleEditor(HorrorCastleProcessor& p)
        : HorrorCastleEditor(p), oscillators{{
            std::make_unique<CreaturePedestalComponent>(p,"crypt",1),
            std::make_unique<CreaturePedestalComponent>(p,"tower",1)}}, chrome(p.getParameterState()), physics(p.getParameterState()), nervousSystem(p), creaturePortrait(p.getParameterState()), soulGlass(p)
    {
        addAndMakeVisible(chrome);
        chrome.toBack();
        addAndMakeVisible(creaturePortrait);
        addAndMakeVisible(soulGlass);
        for(auto& pedestal:oscillators)addAndMakeVisible(*pedestal);
        for(auto& pedestal:oscillators)pedestal->onFocus=[this](bool crypt,int type,int slot){
            focusedCrypt=crypt;focusedType=type;focusedSlot=slot;
            for(auto& p:oscillators)p->setFocused(false);
            oscillators[(size_t)(crypt?0:1)]->setFocused(true);
            creaturePortrait.focusCreature(crypt,type);
            soulGlass.focusCreature(crypt,type);
            showGrimoireCreatureGuide(crypt,type);
        };
        oscillators[0]->setFocused(true);

        flowTitle.setText("SUMMON  →  TRANSFORM  →  WITNESS  →  GRIMOIRE",juce::dontSendNotification);
        flowTitle.setJustificationType(juce::Justification::centred);
        flowTitle.setColour(juce::Label::textColourId,juce::Colour(0xffc9b99f));
        flowTitle.setFont(juce::Font(juce::FontOptions(10.5f)).boldened());
        addAndMakeVisible(flowTitle);

        laboratoryToggle.setButtonText("OPEN LABORATORY");
        laboratoryToggle.setColour(juce::TextButton::buttonColourId,juce::Colour(0xff080a0d));
        laboratoryToggle.setColour(juce::TextButton::textColourOffId,juce::Colour(0xffc9b99f));
        addAndMakeVisible(laboratoryToggle);
        laboratoryToggle.onClick=[this]{
            laboratory=!laboratory;
            setPerformanceMode(!laboratory);
            laboratoryToggle.setButtonText(laboratory?"RETURN TO PERFORMANCE":"OPEN LABORATORY");
            physicsToggle.setVisible(laboratory);nervousToggle.setVisible(laboratory);
            if(!laboratory){physics.setVisible(false);nervousSystem.setVisible(false);}
        };

        readGrimoire.setButtonText("READ GRIMOIRE");
        readGrimoire.setColour(juce::TextButton::buttonColourId,juce::Colour(0xff160f18));
        readGrimoire.setColour(juce::TextButton::textColourOffId,juce::Colour(0xffd6c4dc));
        addAndMakeVisible(readGrimoire);
        readGrimoire.onClick=[this]{openGrimoire();};

        physicsToggle.setButtonText("DEEP EDIT");
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
        setPerformanceMode(true);
        physicsToggle.setVisible(false);
        nervousToggle.setVisible(false);

        physicsToggle.onClick=[this]{
            const bool show=!physics.isVisible();
            nervousSystem.setVisible(false);
            nervousToggle.setButtonText("NERVOUS SYSTEM");
            physics.setVisible(show);
            physicsToggle.setButtonText(show?"CLOSE DEEP EDIT":"DEEP EDIT");
            if(show)physics.toFront(false);
        };
        nervousToggle.onClick=[this]{
            const bool show=!nervousSystem.isVisible();
            physics.setVisible(false);
            physicsToggle.setButtonText("DEEP EDIT");
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

        flowTitle.setBounds(bounds(390,55,520,27));
        laboratoryToggle.setBounds(bounds(930,55,180,27));
        readGrimoire.setBounds(bounds(1120,55,170,27));
        physicsToggle.setBounds(bounds(775,55,176,27));
        nervousToggle.setBounds(bounds(963,55,176,27));
        physics.setBounds(bounds(176,462,1096,238));
        nervousSystem.setBounds(bounds(470,470,520,220));

        // The lower left is now the summoned creature, not a decorative book.
        // Soul Glass sits between the creature and the right-hand Grimoire,
        // preserving the reference composition while making the sound visible.
        // Performance is deliberately two-oscillator: one CRYPT creature and one
        // TOWER creature. The Laboratory retains the legacy three-slot machinery.
        oscillators[0]->setBounds(bounds(132,112,360,292));
        oscillators[1]->setBounds(bounds(824,112,360,292));
        creaturePortrait.setBounds(bounds(24,805,400,245));
        soulGlass.setBounds(bounds(438,805,390,245));
    }

private:
    std::array<std::unique_ptr<CreaturePedestalComponent>,2> oscillators;
    LivingCastleChrome chrome;
    LivingEngineMacroPanel physics;
    NervousSystemComponent nervousSystem;
    CreaturePortraitComponent creaturePortrait;
    SoulGlassComponent soulGlass;
    juce::TextButton physicsToggle;
    juce::TextButton nervousToggle;
    juce::TextButton laboratoryToggle;
    juce::TextButton readGrimoire;
    juce::Label flowTitle;
    bool laboratory=false;
    bool focusedCrypt=true; int focusedType=0,focusedSlot=0;
};

} // namespace horrorcastle

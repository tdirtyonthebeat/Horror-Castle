#pragma once
#include <JuceHeader.h>
#include "../HorrorCastle/CastleParameters.h"
#include "../HorrorCastle/SynthesisFamilyContract.h"
#include <memory>

namespace horrorcastle {

// One generator, expressed as an instrument: summon a creature, transform it,
// and see whether it is actually contributing sound. Plumbing stays backstage.
class CreaturePedestalComponent final : public juce::Component, private juce::Timer
{
public:
    CreaturePedestalComponent(juce::AudioProcessorValueTreeState& s,const char* room,int slot)
      : state(s), scene(room), index(slot)
    {
        const bool crypt=scene=="crypt"; accent=crypt?juce::Colour(0xffc65b55):juce::Colour(0xffaa7ac8);
        if(auto* choice=dynamic_cast<juce::AudioParameterChoice*>(state.getParameter(param::id(scene,index,"type"))))
            for(int i=0;i<choice->choices.size();++i) creature.addItem(choice->choices[i],i+1);
        creature.setTextWhenNothingSelected("SUMMON CREATURE");
        creature.setColour(juce::ComboBox::backgroundColourId,juce::Colour(0xff07090c));
        creature.setColour(juce::ComboBox::textColourId,juce::Colour(0xffddd1bd));
        creature.setColour(juce::ComboBox::outlineColourId,accent.withAlpha(.55f));
        addAndMakeVisible(creature);
        typeA=std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(state,param::id(scene,index,"type"),creature);

        morph.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        morph.setTextBoxStyle(juce::Slider::NoTextBox,false,0,0);
        morph.setColour(juce::Slider::rotarySliderFillColourId,accent);
        morph.setColour(juce::Slider::rotarySliderOutlineColourId,juce::Colour(0xff242127));
        morph.setColour(juce::Slider::thumbColourId,juce::Colour(0xffe0d3bd));
        addAndMakeVisible(morph);
        morphA=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state,param::id(scene,index,"shape"),morph);
        startTimerHz(20);
    }

    void paint(juce::Graphics& g) override
    {
        auto r=getLocalBounds().toFloat();
        g.setColour(juce::Colour(0xdd05070a));g.fillRoundedRectangle(r,8.f);
        g.setColour(accent.withAlpha(.34f+.35f*activity));g.drawRoundedRectangle(r.reduced(.7f),8.f,1.2f);
        const int type=juce::jlimit(0,17,(int)std::lround(read("type")));
        const bool crypt=scene=="crypt";
        const auto& law=synthesis_contract::get(static_cast<GeneratorType>(type),crypt);
        g.setColour(accent);g.setFont(juce::Font(juce::FontOptions(10.f)).boldened());
        g.drawText("PEDESTAL "+juce::String(index),10,7,getWidth()-20,15,juce::Justification::centredLeft);
        g.setColour(juce::Colour(0xffb8ac9a));g.setFont(juce::FontOptions(8.f));
        g.drawFittedText(law.family,10,24,getWidth()-20,12,juce::Justification::centredLeft,1);
        g.setColour(juce::Colour(0xffd9ccb8));g.setFont(juce::Font(juce::FontOptions(9.f)).boldened());
        g.drawText("TRANSFORM",8,getHeight()-34,getWidth()-16,13,juce::Justification::centred);
        const auto lamp=juce::Rectangle<float>((float)getWidth()-22.f,9.f,9.f,9.f);
        g.setColour(accent.withAlpha(.12f+.88f*activity));g.fillEllipse(lamp);
        g.setColour(juce::Colour(0xff9c9385));g.setFont(juce::FontOptions(7.f));
        g.drawText(activity>.025f?"AWAKE":"DORMANT",getWidth()-76,7,50,12,juce::Justification::centredRight);
    }

    void resized() override
    {
        creature.setBounds(9,40,getWidth()-18,24);
        const int d=juce::jmin(88,getHeight()-105);
        morph.setBounds((getWidth()-d)/2,70,d,d);
    }
private:
    float read(const char* leaf) const {if(auto* p=state.getRawParameterValue(param::id(scene,index,leaf)))return p->load();return 0.f;}
    void timerCallback() override
    {
        const float target=(read("enabled")>.5f)?juce::jlimit(0.f,1.f,read("level")):0.f;
        activity += .22f*(target-activity); repaint();
    }
    juce::AudioProcessorValueTreeState& state;
    juce::String scene; int index=1; float activity=0.f; juce::Colour accent;
    juce::ComboBox creature; juce::Slider morph;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> typeA;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> morphA;
};
} // namespace horrorcastle

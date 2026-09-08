#pragma once
#include <JuceHeader.h>
#include "../Core/HorrorCastleProcessor.h"
#include "../HorrorCastle/CastleParameters.h"
#include "../HorrorCastle/SynthesisFamilyContract.h"
#include <memory>

namespace horrorcastle {

// Performance pedestal: one summon, one transformation, live behavior.
// Its animation is driven by the actual generator output and physical telemetry.
class CreaturePedestalComponent final : public juce::Component, private juce::Timer
{
public:
    CreaturePedestalComponent(HorrorCastleProcessor& p,const char* room,int slot)
      : processor(p), state(p.getParameterState()), scene(room), index(slot)
    {
        const bool crypt=scene=="crypt"; accent=crypt?juce::Colour(0xffc65b55):juce::Colour(0xffaa7ac8);
        if(auto* choice=dynamic_cast<juce::AudioParameterChoice*>(state.getParameter(param::id(scene.toRawUTF8(),index,"type"))))
            for(int i=0;i<choice->choices.size();++i) creature.addItem(choice->choices[i],i+1);
        creature.setTextWhenNothingSelected("SUMMON CREATURE");
        creature.setColour(juce::ComboBox::backgroundColourId,juce::Colour(0xff07090c));
        creature.setColour(juce::ComboBox::textColourId,juce::Colour(0xffddd1bd));
        creature.setColour(juce::ComboBox::outlineColourId,accent.withAlpha(.55f));
        addAndMakeVisible(creature);
        typeA=std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(state,param::id(scene.toRawUTF8(),index,"type"),creature);
        creature.onChange=[this]{signalFocus();};

        morph.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        morph.setTextBoxStyle(juce::Slider::NoTextBox,false,0,0);
        morph.setColour(juce::Slider::rotarySliderFillColourId,accent);
        morph.setColour(juce::Slider::rotarySliderOutlineColourId,juce::Colour(0xff242127));
        morph.setColour(juce::Slider::thumbColourId,juce::Colour(0xffe0d3bd));
        addAndMakeVisible(morph);
        morphA=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(state,param::id(scene.toRawUTF8(),index,"shape"),morph);
        morph.onDragStart=[this]{signalFocus();};
        startTimerHz(30);
    }

    std::function<void(bool,int,int)> onFocus;
    void setFocused(bool shouldFocus){focused=shouldFocus;repaint();}

    void paint(juce::Graphics& g) override
    {
        auto r=getLocalBounds().toFloat();
        const bool crypt=scene=="crypt";
        const int type=juce::jlimit(0,17,(int)std::lround(read("type")));
        const auto& law=synthesis_contract::get(static_cast<GeneratorType>(type),crypt);

        g.setColour(juce::Colour(0xdd05070a));g.fillRoundedRectangle(r,8.f);
        g.setColour(accent.withAlpha((focused?.62f:.28f)+.32f*energy));g.drawRoundedRectangle(r.reduced(.7f),8.f,focused?2.0f:1.2f);
        if(focused){g.setColour(accent.withAlpha(.08f+.12f*energy));g.fillRoundedRectangle(r.reduced(3.f),6.f);}

        // Live altar halo: real per-creature audio energy, not parameter level.
        auto altar=juce::Rectangle<float>(10.f,70.f,(float)getWidth()-20.f,104.f);
        g.setColour(accent.withAlpha(.025f+.18f*energy));g.fillEllipse(altar.reduced(8.f-energy*6.f));
        drawBehavior(g,altar,type,crypt);

        g.setColour(accent);g.setFont(juce::Font(juce::FontOptions(10.f)).boldened());
        g.drawText("PEDESTAL "+juce::String(index),10,7,getWidth()-20,15,juce::Justification::centredLeft);
        g.setColour(juce::Colour(0xffb8ac9a));g.setFont(juce::FontOptions(8.f));
        g.drawFittedText(law.family,10,24,getWidth()-20,12,juce::Justification::centredLeft,1);

        g.setColour(juce::Colour(0xffd9ccb8));g.setFont(juce::Font(juce::FontOptions(9.f)).boldened());
        g.drawText("TRANSFORM",8,getHeight()-34,getWidth()-16,13,juce::Justification::centred);

        const auto lamp=juce::Rectangle<float>((float)getWidth()-22.f,9.f,9.f,9.f);
        g.setColour(accent.withAlpha(.08f+.92f*energy));g.fillEllipse(lamp.expanded(energy*2.5f));
        g.setColour(juce::Colour(0xff9c9385));g.setFont(juce::FontOptions(7.f));
        g.drawText(energy>.025f?"AWAKE":"DORMANT",getWidth()-76,7,50,12,juce::Justification::centredRight);

        const float behavior=juce::jlimit(0.f,1.f,behaviorMeter(type,crypt));
        auto meter=juce::Rectangle<float>(12.f,181.f,(float)getWidth()-24.f,7.f);
        g.setColour(juce::Colour(0xff17191d));g.fillRoundedRectangle(meter,2.f);
        g.setColour(accent.withAlpha(.26f+.62f*energy));g.fillRoundedRectangle(meter.withWidth(meter.getWidth()*energy),2.f);
        g.setColour(juce::Colour(0xffa69b8c));g.setFont(juce::FontOptions(7.f));
        g.drawText("SOUL "+juce::String((int)std::lround(energy*100.f))+"%  //  "+behaviorName(type,crypt,behavior),
                   9,190,getWidth()-18,12,juce::Justification::centred);
    }

    void resized() override
    {
        creature.setBounds(9,40,getWidth()-18,24);
        const int d=juce::jmin(82,getHeight()-118);
        morph.setBounds((getWidth()-d)/2,getHeight()-116,d,d);
    }

private:
    float read(const char* leaf) const {if(auto* p=state.getRawParameterValue(param::id(scene.toRawUTF8(),index,leaf)))return p->load();return 0.f;}
    void signalFocus(){if(onFocus)onFocus(scene=="crypt",juce::jlimit(0,17,(int)std::lround(read("type"))),index-1);}

    juce::String behaviorName(int type,bool crypt,float behavior) const
    {
        if(crypt&&type==17)return behavior>.55f?"COLLAPSING":behavior>.18f?"TURBULENT":"SPINNING";
        if(crypt&&type==16)return behavior>.55f?"ARCING":behavior>.18f?"CHARGED":"HAUNTING";
        if(!crypt&&type==17)return behavior>.55f?"OVERBLOWN":behavior>.18f?"LOCKED":"BREATHING";
        if(!crypt&&type==16)return behavior>.55f?"RADIANT":behavior>.18f?"ALIGNED":"DRIFTING";
        if(crypt&&type==9)return energy>.32f?"DECOMPOSING":"RESYNTHESIZING";
        if(!crypt&&type==15)return energy>.32f?"FRACTURING":"REFLECTING";
        return energy>.42f?"MANIFEST":"STIRRING";
    }

    float behaviorMeter(int type,bool crypt) const
    {
        if(crypt&&type==17)return processor.getEcologyMeter(CastleEngine::VortexEvent);
        if(crypt&&type==16)return processor.getEcologyMeter(CastleEngine::PoltergeistInstability);
        if(!crypt&&type==17)return processor.getEcologyMeter(CastleEngine::SirenPressure);
        if(!crypt&&type==16)return processor.getEcologyMeter(CastleEngine::AuroraField);
        return energy;
    }

    void drawBehavior(juce::Graphics& g,juce::Rectangle<float> a,int type,bool crypt) const
    {
        const float behavior=juce::jlimit(0.f,1.f,behaviorMeter(type,crypt));
        const auto gt=static_cast<GeneratorType>(type);
        g.setColour(accent.withAlpha(.12f+.55f*energy));
        if(crypt&&gt==GeneratorType::ChamberX){ // VORTEX: rotating pressure spiral
            juce::Path p;const auto c=a.getCentre();for(int i=0;i<54;++i){const float t=i/53.f,ang=t*(8.f+behavior*10.f);const float rad=t*a.getWidth()*.36f*(.55f+.45f*energy);const float x=c.x+std::cos(ang)*rad,y=c.y+std::sin(ang)*rad;if(i==0)p.startNewSubPath(x,y);else p.lineTo(x,y);}g.strokePath(p,juce::PathStrokeType(1.f+behavior));
        }else if(crypt&&gt==GeneratorType::ChamberIX){ // POLTERGEIST: real instability flashes
            for(int i=0;i<5;++i){const float x=a.getX()+a.getWidth()*(.12f+i*.19f);const float y=a.getCentreY()+std::sin(phase*5.f+i)*a.getHeight()*.22f*behavior;g.drawLine(a.getCentreX(),a.getCentreY(),x,y,.7f+1.5f*behavior);}
        }else if(!crypt&&gt==GeneratorType::ChamberX){ // SIREN: jet lock / overblow stream
            for(int i=0;i<3;++i){const float y=a.getCentreY()+(i-1)*10.f;g.drawLine(a.getX()+8,y,a.getRight()-8,y+std::sin(phase*4.f+i)*12.f*behavior,.8f+behavior);}
        }else if((crypt&&gt==GeneratorType::ChamberII)||(!crypt&&gt==GeneratorType::ChamberVIII)){ // spectral shimmer/fracture
            for(int i=0;i<9;++i){const float h=a.getHeight()*(.08f+energy*.58f)*std::abs(std::sin(phase*2.f+i*.77f));const float x=a.getX()+8+i*(a.getWidth()-16)/8.f;g.drawLine(x,a.getCentreY()-h*.5f,x,a.getCentreY()+h*.5f,.7f+behavior);}
        }else{ // universal living pulse
            const float pulse=.72f+.28f*std::sin(phase*(2.f+4.f*energy));
            auto e=a.withSizeKeepingCentre(a.getWidth()*(.28f+.42f*energy)*pulse,a.getHeight()*(.28f+.42f*energy)*pulse);
            g.drawEllipse(e,1.f+energy);
        }
    }

    void timerCallback() override
    {
        const bool crypt=scene=="crypt";
        const float target=processor.getCreatureEnergy(crypt,index-1);
        energy += .32f*(target-energy);
        phase=std::fmod(phase+.035f+.12f*energy,juce::MathConstants<float>::twoPi);
        repaint();
    }

    HorrorCastleProcessor& processor;
    juce::AudioProcessorValueTreeState& state;
    juce::String scene; int index=1; float energy=0.f,phase=0.f; bool focused=false; juce::Colour accent;
    juce::ComboBox creature; juce::Slider morph;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> typeA;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> morphA;
};
} // namespace horrorcastle

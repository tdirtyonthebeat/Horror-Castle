#pragma once
#include <JuceHeader.h>
#include "../Core/HorrorCastleProcessor.h"
#include "../HorrorCastle/CastleParameters.h"
#include "../HorrorCastle/SynthesisFamilyContract.h"
#include "../HorrorCastle/PerformanceCreatureRoster.h"
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
        if(auto* choice=dynamic_cast<juce::AudioParameterChoice*>(state.getParameter(param::id(scene.toRawUTF8(),index,"type")))){
            auto addRoster=[&](const auto& roster){
                for(const int i:roster){
                    const auto& law=synthesis_contract::get(static_cast<GeneratorType>(i),scene=="crypt");
                    creature.addItem(choice->choices[i]+"  //  "+law.family,i+1);
                }
            };
            if(scene=="crypt")addRoster(performance_roster::crypt);
            else addRoster(performance_roster::tower);
        }
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

        power.setButtonText("ON");
        power.setClickingTogglesState(true);
        power.setColour(juce::TextButton::buttonColourId,juce::Colour(0xff090b0e));
        power.setColour(juce::TextButton::buttonOnColourId,accent.withAlpha(.34f));
        power.setColour(juce::TextButton::textColourOffId,juce::Colour(0xff8f887e));
        power.setColour(juce::TextButton::textColourOnId,juce::Colour(0xfff1e2c8));
        addAndMakeVisible(power);
        enabledA=std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            state,param::id(scene.toRawUTF8(),index,"enabled"),power);
        power.onClick=[this]{power.setButtonText(power.getToggleState()?"ON":"OFF");signalFocus();};
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

        juce::ColourGradient chamber(juce::Colour(0xff06080c),r.getCentreX(),r.getY(),
                                     juce::Colour(0xff111016),r.getCentreX(),r.getBottom(),false);
        chamber.addColour(.58,accent.withAlpha(.08f+.10f*energy));
        g.setGradientFill(chamber);g.fillRoundedRectangle(r,12.f);
        g.setColour(juce::Colours::black.withAlpha(.55f));g.fillRoundedRectangle(r.reduced(8.f).withTrimmedTop(86.f),10.f);
        g.setColour(accent.withAlpha((focused?.72f:.34f)+.24f*energy));g.drawRoundedRectangle(r.reduced(.8f),12.f,focused?2.2f:1.2f);
        g.setColour(accent.withAlpha(.10f));g.drawRoundedRectangle(r.reduced(5.f),9.f,.8f);

        // Live summoning chamber: manifestation geometry is synthesis-family aware
        // and driven by actual audio energy + MORPH, never by a decorative animation.
        auto altar=juce::Rectangle<float>(24.f,112.f,(float)getWidth()-48.f,158.f);
        g.setColour(accent.withAlpha(.025f+.18f*energy));g.fillEllipse(altar.reduced(8.f-energy*8.f));
        drawManifestation(g,altar,type,crypt,juce::jlimit(0.f,1.f,(float)morph.getValue()));
        drawBehavior(g,altar,type,crypt);

        g.setColour(accent);g.setFont(juce::Font(juce::FontOptions(10.f)).boldened());
        g.drawText(crypt?"OSCILLATOR A // CRYPT":"OSCILLATOR B // TOWER",10,7,getWidth()-86,15,juce::Justification::centredLeft);
        g.setColour(juce::Colour(0xfff0e2cd));g.setFont(juce::Font(juce::FontOptions(14.f)).boldened());
        g.drawFittedText(law.creature,12,28,getWidth()-94,20,juce::Justification::centredLeft,1);
        g.setColour(juce::Colour(0xffb8ac9a));g.setFont(juce::FontOptions(9.f));
        g.drawFittedText(law.family,12,49,getWidth()-24,14,juce::Justification::centredLeft,1);
        g.setColour(juce::Colour(0xff9f9689));g.setFont(juce::FontOptions(8.f));
        g.drawFittedText(juce::String("TRANSFORMATION // ")+law.morphTrajectory,12,66,getWidth()-24,18,juce::Justification::centredLeft,2);

        g.setColour(juce::Colour(0xffd9ccb8));g.setFont(juce::Font(juce::FontOptions(9.f)).boldened());
        g.drawText("TRANSFORM",8,getHeight()-204,getWidth()-16,14,juce::Justification::centred);

        const auto lamp=juce::Rectangle<float>((float)getWidth()-22.f,9.f,9.f,9.f);
        g.setColour(accent.withAlpha(.08f+.92f*energy));g.fillEllipse(lamp.expanded(energy*2.5f));
        g.setColour(juce::Colour(0xff9c9385));g.setFont(juce::FontOptions(7.f));
        const bool powered=read("enabled")>.5f;
        g.drawText(powered?(energy>.025f?"AWAKE":"READY"):"OFF",getWidth()-76,7,50,12,juce::Justification::centredRight);

        const float behavior=juce::jlimit(0.f,1.f,behaviorMeter(type,crypt));
        auto meter=juce::Rectangle<float>(18.f,(float)getHeight()-62.f,(float)getWidth()-36.f,8.f);
        g.setColour(juce::Colour(0xff17191d));g.fillRoundedRectangle(meter,2.f);
        g.setColour(accent.withAlpha(.26f+.62f*energy));g.fillRoundedRectangle(meter.withWidth(meter.getWidth()*energy),2.f);
        g.setColour(juce::Colour(0xffa69b8c));g.setFont(juce::FontOptions(7.f));
        g.drawText("SOUL "+juce::String((int)std::lround(energy*100.f))+"%  //  "+behaviorName(type,crypt,behavior),
                   12,getHeight()-50,getWidth()-24,13,juce::Justification::centred);
    }

    void resized() override
    {
        power.setBounds(getWidth()-74,18,62,26);
        creature.setBounds(14,70,getWidth()-28,32);
        const int d=juce::jmin(132,getHeight()-180);
        morph.setBounds((getWidth()-d)/2,getHeight()-190,d,d);
    }

private:
    float read(const char* leaf) const {if(auto* p=state.getRawParameterValue(param::id(scene.toRawUTF8(),index,leaf)))return p->load();return 0.f;}
    void signalFocus(){if(onFocus)onFocus(scene=="crypt",juce::jlimit(0,17,(int)std::lround(read("type"))),index-1);}

    void drawManifestation(juce::Graphics& g,juce::Rectangle<float> a,int type,bool crypt,float morphValue) const
    {
        const auto gt=static_cast<GeneratorType>(type);
        const auto c=a.getCentre();
        const float e=juce::jlimit(0.f,1.f,energy);
        g.setColour(accent.withAlpha(.10f+.42f*e));

        if(crypt&&gt==GeneratorType::VA){ // WEREWOLF: widening jaw / unstable teeth
            juce::Path jaw;jaw.startNewSubPath(c.x-a.getWidth()*.20f,c.y-a.getHeight()*.15f);
            jaw.lineTo(c.x,c.y+a.getHeight()*(.10f+.12f*morphValue));jaw.lineTo(c.x+a.getWidth()*.20f,c.y-a.getHeight()*.15f);
            for(int i=0;i<5;++i){const float x=c.x-a.getWidth()*.14f+i*a.getWidth()*.07f;g.drawLine(x,c.y-.05f*a.getHeight(),x+(i%2?4.f:-4.f),c.y+a.getHeight()*(.12f+.08f*morphValue),.8f+e);}
            g.strokePath(jaw,juce::PathStrokeType(1.f+1.5f*e));
        }else if(crypt&&gt==GeneratorType::Wavetable){ // VAMPIRE: spectral fangs
            for(int i=0;i<7;++i){const float x=a.getX()+a.getWidth()*(.18f+i*.105f);const float h=a.getHeight()*(.08f+.30f*morphValue)*(i%2?.75f:1.f);g.drawLine(x,c.y-h,x,c.y+h,.8f+1.1f*e);}
        }else if((crypt&&gt==GeneratorType::FM)||(!crypt&&gt==GeneratorType::ChamberIII)){ // FM constellations
            for(int i=0;i<5;++i){const float ang=phase*(.35f+.12f*i)+i*1.2566f;const float rad=a.getWidth()*(.08f+.18f*morphValue)*(1.f+.12f*i);g.fillEllipse(c.x+std::cos(ang)*rad-2,c.y+std::sin(ang)*rad-2,4,4);}
        }else if(crypt&&gt==GeneratorType::Noise){ // GHOUL cloud
            for(int i=0;i<22;++i){const float x=a.getX()+std::fmod(i*.618f+phase*.03f,1.f)*a.getWidth();const float y=a.getY()+std::fmod(i*.414f+morphValue*.17f,1.f)*a.getHeight();const float s=1.5f+5.f*e*(.3f+.7f*morphValue);g.fillEllipse(x,y,s,s);}
        }else if(crypt&&gt==GeneratorType::ChamberII){ // CORPSE decomposition
            for(int i=0;i<12;++i){const float x=a.getX()+a.getWidth()*(i+.5f)/12.f;const float split=(i%2?1.f:-1.f)*morphValue*a.getWidth()*.04f;const float h=a.getHeight()*(.12f+.35f*std::abs(std::sin(i*.63f+phase)));g.drawLine(x,c.y-h*.5f,x+split,c.y+h*.5f,.8f+e);}
        }else if(crypt&&gt==GeneratorType::ChamberVIII){ // ABYSS pressure rings
            for(int i=0;i<4;++i){const float s=(.18f+i*.17f)*(1.f+.55f*morphValue)*juce::jmin(a.getWidth(),a.getHeight());g.drawEllipse(c.x-s*.5f,c.y-s*.5f,s,s,.7f+e*(i+1)*.25f);}
        }else if(crypt&&gt==GeneratorType::ChamberIX){ // POLTERGEIST arcs
            for(int i=0;i<6;++i){const float ang=i*1.047f+phase*(.4f+morphValue);const float rad=a.getWidth()*(.12f+.23f*morphValue);g.drawLine(c.x,c.y,c.x+std::cos(ang)*rad,c.y+std::sin(ang)*rad,.7f+1.8f*e);}
        }else if(crypt&&gt==GeneratorType::ChamberX){ // VORTEX spiral
            juce::Path p;for(int i=0;i<72;++i){const float t=i/71.f,ang=t*(8.f+14.f*morphValue)+phase*.25f,rad=t*a.getWidth()*.34f*(.55f+.45f*e);const float x=c.x+std::cos(ang)*rad,y=c.y+std::sin(ang)*rad;if(i==0)p.startNewSubPath(x,y);else p.lineTo(x,y);}g.strokePath(p,juce::PathStrokeType(.9f+1.3f*e));
        }else if(!crypt&&gt==GeneratorType::PM){ // WITCH phase warp
            for(int i=0;i<3;++i){auto rr=a.reduced(20.f+i*18.f);g.drawEllipse(rr.translated(std::sin(phase+i)*6.f*morphValue,0.f),.8f+e);}
        }else if(!crypt&&gt==GeneratorType::Vector){ // SHAPESHIFTER
            juce::Path d;d.startNewSubPath(c.x,c.y-a.getHeight()*.28f);d.lineTo(c.x+a.getWidth()*(.18f+.10f*morphValue),c.y);d.lineTo(c.x,c.y+a.getHeight()*.28f);d.lineTo(c.x-a.getWidth()*(.18f+.10f*morphValue),c.y);d.closeSubPath();g.strokePath(d,juce::PathStrokeType(1.f+e));
        }else if(!crypt&&gt==GeneratorType::ChamberI){ // BELL GLASS
            for(int i=0;i<3;++i){const float s=a.getHeight()*(.22f+i*.18f)*(1.f+.15f*morphValue);g.drawEllipse(c.x-s*.5f,c.y-s*.5f,s,s,.7f+e);}
        }else if(!crypt&&gt==GeneratorType::ChamberIV){ // PRISM grains/shards
            for(int i=0;i<18;++i){const float ang=i*.77f+phase*.12f;const float rad=a.getWidth()*(.05f+.27f*morphValue)*std::fmod(i*.61f,1.f);juce::Path shard;const float x=c.x+std::cos(ang)*rad,y=c.y+std::sin(ang)*rad;shard.addTriangle(x,y,x+3.f+7.f*morphValue,y-5.f,x-2.f,y+5.f);g.fillPath(shard);}
        }else if(!crypt&&gt==GeneratorType::ChamberVI){ // CHOIR mouths/formants
            for(int i=0;i<5;++i){const float x=c.x+(i-2)*a.getWidth()*.10f;const float w=16.f+20.f*morphValue;g.drawEllipse(x-w*.5f,c.y-6.f,w,12.f,.8f+e);}
        }else if(!crypt&&gt==GeneratorType::ChamberVIII){ // MIRROR fracture
            for(int i=0;i<9;++i){const float x=a.getX()+a.getWidth()*(i+.5f)/9.f;const float off=std::sin(i*1.9f+phase)*morphValue*12.f;g.drawLine(x,c.y-a.getHeight()*.24f,x+off,c.y+a.getHeight()*.24f,.8f+e);}
        }else if(!crypt&&gt==GeneratorType::ChamberIX){ // AURORA field
            for(int i=0;i<4;++i){juce::Path p;for(int x=0;x<40;++x){const float xx=a.getX()+a.getWidth()*x/39.f;const float yy=c.y+(i-1.5f)*11.f+std::sin(x*.22f+phase+i)*a.getHeight()*.06f*(.4f+morphValue);if(x==0)p.startNewSubPath(xx,yy);else p.lineTo(xx,yy);}g.strokePath(p,juce::PathStrokeType(.8f+e));}
        }else if(!crypt&&gt==GeneratorType::ChamberX){ // SIREN aperture/jet
            const float spread=6.f+26.f*morphValue;for(int i=-1;i<=1;++i)g.drawLine(a.getX()+12,c.y+i*5.f,a.getRight()-12,c.y+i*spread,.8f+1.1f*e);
        }else{
            g.drawEllipse(a.reduced(a.getWidth()*.28f),1.f+e);
        }
    }

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
    juce::ComboBox creature; juce::Slider morph; juce::TextButton power;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> typeA;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> morphA;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> enabledA;
};
} // namespace horrorcastle

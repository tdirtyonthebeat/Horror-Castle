#pragma once
#include <JuceHeader.h>
#include "../Core/HorrorCastleProcessor.h"
#include "../HorrorCastle/CastleParameters.h"
#include "../HorrorCastle/SynthesisFamilyContract.h"
#include "../HorrorCastle/PerformanceCreatureRoster.h"
#include <memory>

namespace horrorcastle {

// Performance oscillator: summon one creature, transform it, and witness
// real audio/physics telemetry. Hidden compatibility slots remain Laboratory-only.
class CreaturePedestalComponent final : public juce::Component, private juce::Timer
{
public:
    CreaturePedestalComponent(HorrorCastleProcessor& p,const char* room,int slot)
      : processor(p), state(p.getParameterState()), scene(room), index(slot)
    {
        const bool crypt=scene=="crypt";
        accent=crypt?juce::Colour(0xffc65b55):juce::Colour(0xffaa7ac8);

        if(auto* choice=dynamic_cast<juce::AudioParameterChoice*>(state.getParameter(param::id(scene.toRawUTF8(),index,"type")))){
            auto addRoster=[&](const auto& roster){
                for(const int i:roster){
                    const auto& law=synthesis_contract::get(static_cast<GeneratorType>(i),crypt);
                    creature.addItem(choice->choices[i]+"  //  "+law.family,i+1);
                }
            };
            if(crypt)addRoster(performance_roster::crypt); else addRoster(performance_roster::tower);
        }
        creature.setTextWhenNothingSelected("SUMMON CREATURE");
        creature.setColour(juce::ComboBox::backgroundColourId,juce::Colour(0xff07090c));
        creature.setColour(juce::ComboBox::textColourId,juce::Colour(0xffddd1bd));
        creature.setColour(juce::ComboBox::outlineColourId,accent.withAlpha(.55f));
        addAndMakeVisible(creature);
        creature.onChange=[this]{
            if(syncingType)return;
            if(auto* p=state.getParameter(param::id(scene.toRawUTF8(),index,"type"))){
                const int type=creature.getSelectedId()-1;
                if(type>=0){p->beginChangeGesture();p->setValueNotifyingHost(p->convertTo0to1((float)type));p->endChangeGesture();}
            }
            signalFocus();
        };

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
        enabledA=std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(state,param::id(scene.toRawUTF8(),index,"enabled"),power);
        power.onClick=[this]{power.setButtonText(power.getToggleState()?"ON":"OFF");signalFocus();};
        startTimerHz(30);
    }

    std::function<void(bool,int,int)> onFocus;
    void setFocused(bool shouldFocus){focused=shouldFocus;repaint();}

    void paint(juce::Graphics& g) override
    {
        auto r=getLocalBounds().toFloat();
        const bool crypt=scene=="crypt";
        const int type=currentType();
        const auto& law=synthesis_contract::get(static_cast<GeneratorType>(type),crypt);
        const bool powered=read("enabled")>.5f;
        const float e=juce::jlimit(0.f,1.f,energy);
        const float m=juce::jlimit(0.f,1.f,(float)morph.getValue());

        // One large cinematic chamber per oscillator: title -> creature -> living
        // manifestation -> selector -> one transformation control.
        juce::ColourGradient chamber(juce::Colour(0xff050609),r.getCentreX(),r.getY(),
                                     juce::Colour(0xff0d0a10),r.getCentreX(),r.getBottom(),false);
        chamber.addColour(.46,accent.withAlpha(.045f+.085f*e));
        g.setGradientFill(chamber);g.fillRoundedRectangle(r,16.f);
        g.setColour(juce::Colours::black.withAlpha(.62f));g.fillRoundedRectangle(r.reduced(8.f),13.f);
        g.setColour(accent.withAlpha((focused?.70f:.28f)+.24f*e));g.drawRoundedRectangle(r.reduced(.7f),16.f,focused?2.2f:1.0f);
        g.setColour(juce::Colour(0xff8c7355).withAlpha(.24f));g.drawRoundedRectangle(r.reduced(5.f),12.f,.8f);

        // Header hierarchy.
        g.setColour(accent.withAlpha(powered?.96f:.42f));g.setFont(juce::Font(juce::FontOptions(10.f)).boldened());
        g.drawText(crypt?"OSCILLATOR A  •  CRYPT":"OSCILLATOR B  •  TOWER",18,12,getWidth()-112,18,juce::Justification::centredLeft);
        g.setColour(juce::Colour(0xfff2e7d5).withAlpha(powered?1.f:.48f));g.setFont(juce::Font(juce::FontOptions(24.f)).boldened());
        g.drawFittedText(law.creature,18,36,getWidth()-36,34,juce::Justification::centredLeft,1);
        g.setColour(juce::Colour(0xffc1b7a8).withAlpha(powered?.90f:.42f));g.setFont(juce::FontOptions(10.f));
        g.drawFittedText(law.family+"  //  "+law.synthesisMethod,18,72,getWidth()-36,18,juce::Justification::centredLeft,1);

        // Transformation law as a quiet engraved caption.
        auto lawBox=juce::Rectangle<float>(18.f,94.f,(float)getWidth()-36.f,31.f);
        g.setColour(juce::Colour(0xff111216));g.fillRoundedRectangle(lawBox,5.f);
        g.setColour(accent.withAlpha(.22f));g.drawRoundedRectangle(lawBox,5.f,.8f);
        g.setColour(juce::Colour(0xffaaa092));g.setFont(juce::FontOptions(8.2f));
        g.drawFittedText(juce::String("MORPH TRAJECTORY  //  ")+law.morphTrajectory,lawBox.reduced(8,3).toNearestInt(),juce::Justification::centredLeft,2);

        // Gothic manifestation window. Audio energy controls illumination while
        // MORPH controls creature-specific geometry.
        auto altar=juce::Rectangle<float>(22.f,136.f,(float)getWidth()-44.f,190.f);
        juce::Path arch;
        arch.startNewSubPath(altar.getX(),altar.getBottom());
        arch.lineTo(altar.getX(),altar.getY()+50.f);
        arch.quadraticTo(altar.getCentreX(),altar.getY()-34.f,altar.getRight(),altar.getY()+50.f);
        arch.lineTo(altar.getRight(),altar.getBottom());arch.closeSubPath();
        g.setColour(juce::Colour(0xff020305));g.fillPath(arch);
        g.setColour(accent.withAlpha(.16f+.20f*e));g.strokePath(arch,juce::PathStrokeType(1.f+e));
        g.setColour(accent.withAlpha(.018f+.13f*e));g.fillEllipse(altar.reduced(28.f-10.f*e));
        drawManifestation(g,altar.reduced(18.f,12.f),type,crypt,m);
        drawBehavior(g,altar.reduced(18.f,12.f),type,crypt);

        // Live state reads like an instrument, not telemetry debug text.
        const float behavior=juce::jlimit(0.f,1.f,behaviorMeter(type,crypt));
        g.setColour(juce::Colour(0xff8d857a));g.setFont(juce::FontOptions(8.f));
        g.drawText(powered?(e>.025f?juce::String("MANIFESTING  //  ")+behaviorName(type,crypt,behavior):juce::String("SUMMONED  //  WAITING")):juce::String("DORMANT"),
                   20,331,getWidth()-40,15,juce::Justification::centred);
        auto meter=juce::Rectangle<float>(30.f,350.f,(float)getWidth()-60.f,5.f);
        g.setColour(juce::Colour(0xff17181b));g.fillRoundedRectangle(meter,2.f);
        g.setColour(accent.withAlpha(.32f+.58f*e));g.fillRoundedRectangle(meter.withWidth(meter.getWidth()*e),2.f);

        g.setColour(juce::Colour(0xffb8ad9d));g.setFont(juce::FontOptions(8.2f));
        g.drawText("CHOOSE CREATURE",20,366,getWidth()-40,14,juce::Justification::centredLeft);
        g.drawText("TRANSFORM",20,getHeight()-116,getWidth()-40,14,juce::Justification::centred);

        // Power lamp.
        const auto lamp=juce::Rectangle<float>((float)getWidth()-28.f,16.f,10.f,10.f);
        g.setColour(accent.withAlpha(powered?(.30f+.70f*e):.08f));g.fillEllipse(lamp.expanded(powered?e*3.f:0.f));
    }

    void resized() override
    {
        power.setBounds(getWidth()-92,10,68,28);
        creature.setBounds(20,383,getWidth()-40,34);
        const int d=96;
        morph.setBounds((getWidth()-d)/2,getHeight()-100,d,d);
    }

private:
    bool containsItemId(int id) const {for(int i=0;i<creature.getNumItems();++i)if(creature.getItemId(i)==id)return true;return false;}
    float read(const char* leaf) const {if(auto* p=state.getRawParameterValue(param::id(scene.toRawUTF8(),index,leaf)))return p->load();return 0.f;}
    int currentType() const {return juce::jlimit(0,17,(int)std::lround(read("type")));}
    void signalFocus(){if(onFocus)onFocus(scene=="crypt",currentType(),index-1);}

    void drawManifestation(juce::Graphics& g,juce::Rectangle<float> a,int type,bool crypt,float m) const
    {
        const auto gt=static_cast<GeneratorType>(type); const auto c=a.getCentre(); const float e=juce::jlimit(0.f,1.f,energy);
        g.setColour(accent.withAlpha(.10f+.42f*e));
        if(crypt&&gt==GeneratorType::VA){
            juce::Path jaw; jaw.startNewSubPath(c.x-a.getWidth()*.20f,c.y-a.getHeight()*.15f); jaw.lineTo(c.x,c.y+a.getHeight()*(.10f+.12f*m)); jaw.lineTo(c.x+a.getWidth()*.20f,c.y-a.getHeight()*.15f); g.strokePath(jaw,juce::PathStrokeType(1.f+1.5f*e));
        }else if(crypt&&gt==GeneratorType::Wavetable){
            for(int i=0;i<7;++i){const float x=a.getX()+a.getWidth()*(.18f+i*.105f),h=a.getHeight()*(.08f+.30f*m)*(i%2?.75f:1.f);g.drawLine(x,c.y-h,x,c.y+h,.8f+1.1f*e);}
        }else if((crypt&&gt==GeneratorType::FM)||(!crypt&&gt==GeneratorType::ChamberIII)){
            for(int i=0;i<5;++i){const float ang=phase*(.35f+.12f*i)+i*1.2566f,rad=a.getWidth()*(.08f+.18f*m)*(1.f+.12f*i);g.fillEllipse(c.x+std::cos(ang)*rad-2,c.y+std::sin(ang)*rad-2,4,4);}
        }else if(crypt&&gt==GeneratorType::Noise){
            for(int i=0;i<22;++i){const float x=a.getX()+std::fmod(i*.618f+phase*.03f,1.f)*a.getWidth(),y=a.getY()+std::fmod(i*.414f+m*.17f,1.f)*a.getHeight(),s=1.5f+5.f*e*(.3f+.7f*m);g.fillEllipse(x,y,s,s);}
        }else if(crypt&&gt==GeneratorType::ChamberII){
            for(int i=0;i<12;++i){const float x=a.getX()+a.getWidth()*(i+.5f)/12.f,split=(i%2?1.f:-1.f)*m*a.getWidth()*.04f,h=a.getHeight()*(.12f+.35f*std::abs(std::sin(i*.63f+phase)));g.drawLine(x,c.y-h*.5f,x+split,c.y+h*.5f,.8f+e);}
        }else if(crypt&&gt==GeneratorType::ChamberVIII){
            for(int i=0;i<4;++i){const float s=(.18f+i*.17f)*(1.f+.55f*m)*juce::jmin(a.getWidth(),a.getHeight());g.drawEllipse(c.x-s*.5f,c.y-s*.5f,s,s,.7f+e*(i+1)*.25f);}
        }else if(crypt&&gt==GeneratorType::ChamberIX){
            for(int i=0;i<6;++i){const float ang=i*1.047f+phase*(.4f+m),rad=a.getWidth()*(.12f+.23f*m);g.drawLine(c.x,c.y,c.x+std::cos(ang)*rad,c.y+std::sin(ang)*rad,.7f+1.8f*e);}
        }else if(crypt&&gt==GeneratorType::ChamberX){
            juce::Path p; for(int i=0;i<72;++i){const float t=i/71.f,ang=t*(8.f+14.f*m)+phase*.25f,rad=t*a.getWidth()*.34f*(.55f+.45f*e),x=c.x+std::cos(ang)*rad,y=c.y+std::sin(ang)*rad;if(i==0)p.startNewSubPath(x,y);else p.lineTo(x,y);} g.strokePath(p,juce::PathStrokeType(.9f+1.3f*e));
        }else if(!crypt&&gt==GeneratorType::PM){
            for(int i=0;i<3;++i){auto rr=a.reduced(20.f+i*18.f);g.drawEllipse(rr.translated(std::sin(phase+i)*6.f*m,0.f),.8f+e);}
        }else if(!crypt&&gt==GeneratorType::Vector){
            juce::Path d;d.startNewSubPath(c.x,c.y-a.getHeight()*.28f);d.lineTo(c.x+a.getWidth()*(.18f+.10f*m),c.y);d.lineTo(c.x,c.y+a.getHeight()*.28f);d.lineTo(c.x-a.getWidth()*(.18f+.10f*m),c.y);d.closeSubPath();g.strokePath(d,juce::PathStrokeType(1.f+e));
        }else if(!crypt&&gt==GeneratorType::ChamberI){
            for(int i=0;i<3;++i){const float s=a.getHeight()*(.22f+i*.18f)*(1.f+.15f*m);g.drawEllipse(c.x-s*.5f,c.y-s*.5f,s,s,.7f+e);}
        }else if(!crypt&&gt==GeneratorType::ChamberIV){
            for(int i=0;i<18;++i){const float ang=i*.77f+phase*.12f,rad=a.getWidth()*(.05f+.27f*m)*std::fmod(i*.61f,1.f),x=c.x+std::cos(ang)*rad,y=c.y+std::sin(ang)*rad;juce::Path shard;shard.addTriangle(x,y,x+3.f+7.f*m,y-5.f,x-2.f,y+5.f);g.fillPath(shard);}
        }else if(!crypt&&gt==GeneratorType::ChamberVI){
            for(int i=0;i<5;++i){const float x=c.x+(i-2)*a.getWidth()*.10f,w=16.f+20.f*m;g.drawEllipse(x-w*.5f,c.y-6.f,w,12.f,.8f+e);}
        }else if(!crypt&&gt==GeneratorType::ChamberVIII){
            for(int i=0;i<9;++i){const float x=a.getX()+a.getWidth()*(i+.5f)/9.f,off=std::sin(i*1.9f+phase)*m*12.f;g.drawLine(x,c.y-a.getHeight()*.24f,x+off,c.y+a.getHeight()*.24f,.8f+e);}
        }else if(!crypt&&gt==GeneratorType::ChamberIX){
            for(int i=0;i<4;++i){juce::Path p;for(int x=0;x<40;++x){const float xx=a.getX()+a.getWidth()*x/39.f,yy=c.y+(i-1.5f)*11.f+std::sin(x*.22f+phase+i)*a.getHeight()*.06f*(.4f+m);if(x==0)p.startNewSubPath(xx,yy);else p.lineTo(xx,yy);}g.strokePath(p,juce::PathStrokeType(.8f+e));}
        }else if(!crypt&&gt==GeneratorType::ChamberX){
            const float spread=6.f+26.f*m;for(int i=-1;i<=1;++i)g.drawLine(a.getX()+12,c.y+i*5.f,a.getRight()-12,c.y+i*spread,.8f+1.1f*e);
        }else g.drawEllipse(a.reduced(a.getWidth()*.28f),1.f+e);
    }

    juce::String behaviorName(int type,bool crypt,float b) const
    {
        if(crypt&&type==17)return b>.55f?"COLLAPSING":b>.18f?"TURBULENT":"SPINNING";
        if(crypt&&type==16)return b>.55f?"ARCING":b>.18f?"CHARGED":"HAUNTING";
        if(!crypt&&type==17)return b>.55f?"OVERBLOWN":b>.18f?"LOCKED":"BREATHING";
        if(!crypt&&type==16)return b>.55f?"RADIANT":b>.18f?"ALIGNED":"DRIFTING";
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
        const float b=juce::jlimit(0.f,1.f,behaviorMeter(type,crypt));
        const auto gt=static_cast<GeneratorType>(type); g.setColour(accent.withAlpha(.12f+.55f*energy));
        if(crypt&&gt==GeneratorType::ChamberX){
            juce::Path p;const auto c=a.getCentre();for(int i=0;i<54;++i){const float t=i/53.f,ang=t*(8.f+b*10.f),rad=t*a.getWidth()*.36f*(.55f+.45f*energy),x=c.x+std::cos(ang)*rad,y=c.y+std::sin(ang)*rad;if(i==0)p.startNewSubPath(x,y);else p.lineTo(x,y);}g.strokePath(p,juce::PathStrokeType(1.f+b));
        }else if(crypt&&gt==GeneratorType::ChamberIX){
            for(int i=0;i<5;++i){const float x=a.getX()+a.getWidth()*(.12f+i*.19f),y=a.getCentreY()+std::sin(phase*5.f+i)*a.getHeight()*.22f*b;g.drawLine(a.getCentreX(),a.getCentreY(),x,y,.7f+1.5f*b);}
        }else if(!crypt&&gt==GeneratorType::ChamberX){
            for(int i=0;i<3;++i){const float y=a.getCentreY()+(i-1)*10.f;g.drawLine(a.getX()+8,y,a.getRight()-8,y+std::sin(phase*4.f+i)*12.f*b,.8f+b);}
        }else if((crypt&&gt==GeneratorType::ChamberII)||(!crypt&&gt==GeneratorType::ChamberVIII)){
            for(int i=0;i<9;++i){const float h=a.getHeight()*(.08f+energy*.58f)*std::abs(std::sin(phase*2.f+i*.77f)),x=a.getX()+8+i*(a.getWidth()-16)/8.f;g.drawLine(x,a.getCentreY()-h*.5f,x,a.getCentreY()+h*.5f,.7f+b);}
        }else{
            const float pulse=.72f+.28f*std::sin(phase*(2.f+4.f*energy));auto e=a.withSizeKeepingCentre(a.getWidth()*(.28f+.42f*energy)*pulse,a.getHeight()*(.28f+.42f*energy)*pulse);g.drawEllipse(e,1.f+energy);
        }
    }

    void timerCallback() override
    {
        const bool crypt=scene=="crypt"; const int type=currentType(),wantedId=type+1;
        if(creature.getSelectedId()!=wantedId){
            syncingType=true;
            if(!containsItemId(wantedId)){
                juce::String name="Legacy Engine";
                if(auto* choice=dynamic_cast<juce::AudioParameterChoice*>(state.getParameter(param::id(scene.toRawUTF8(),index,"type"))))
                    if(type<choice->choices.size())name=choice->choices[type];
                // Current JUCE ComboBox has no removeItem API. Keep any previously
                // surfaced legacy item; there are at most 18 stable engine IDs.
                creature.addItem(name+"  //  LEGACY • OPEN LABORATORY",wantedId);
            }
            creature.setSelectedId(wantedId,juce::dontSendNotification); syncingType=false;
        }
        power.setButtonText(read("enabled")>.5f?"ON":"OFF");
        const float target=processor.getCreatureEnergy(crypt,index-1); energy+=.32f*(target-energy);
        phase=std::fmod(phase+.035f+.12f*energy,juce::MathConstants<float>::twoPi); repaint();
    }

    HorrorCastleProcessor& processor;
    juce::AudioProcessorValueTreeState& state;
    juce::String scene; int index=1; float energy=0.f,phase=0.f; bool focused=false,syncingType=false; juce::Colour accent;
    juce::ComboBox creature; juce::Slider morph; juce::TextButton power;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> morphA;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> enabledA;
};
} // namespace horrorcastle

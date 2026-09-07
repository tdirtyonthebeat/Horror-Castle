#pragma once
#include <JuceHeader.h>
#include "../Core/HorrorCastleProcessor.h"
#include "../HorrorCastle/SynthesisFamilyContract.h"
#include "../HorrorCastle/CastleParameters.h"
#include <array>
#include <cmath>

namespace horrorcastle {

// One visual language for the Necromancer: waveform = body, spectrum = aura,
// stereo = apparition width. It deliberately exposes no extra synthesis knobs.
class SoulGlassComponent final : public juce::Component, private juce::Timer
{
public:
    explicit SoulGlassComponent(HorrorCastleProcessor& p) : processor(p)
    {
        setInterceptsMouseClicks(false,false);
        startTimerHz(30);
    }

    void paint(juce::Graphics& g) override
    {
        auto r=getLocalBounds().toFloat();
        g.setColour(juce::Colour(0xff030507).withAlpha(.96f));g.fillRoundedRectangle(r,5.f);
        g.setColour(juce::Colour(0xff7f6648).withAlpha(.42f));g.drawRoundedRectangle(r.reduced(.5f),5.f,1.f);
        auto title=r.removeFromTop(22.f).reduced(8,0);
        g.setColour(juce::Colour(0xffcbbca4));g.setFont(juce::FontOptions(10.f));
        g.drawText("SOUL GLASS // BODY • AURA • APPARITION",title.toNearestInt(),juce::Justification::centredLeft);

        auto scope=r.reduced(8,5); auto spectrum=scope.removeFromBottom(scope.getHeight()*.40f);
        g.setColour(juce::Colour(0xff6f737a).withAlpha(.14f));
        for(int i=1;i<4;++i)g.drawHorizontalLine((int)(scope.getY()+scope.getHeight()*i/4.f),scope.getX(),scope.getRight());

        std::array<float,512> wave{}; std::array<float,64> bins{}; float width=0.f;
        processor.copySoulGlass(wave,bins,width);

        juce::Path p; const float mid=scope.getCentreY(),amp=scope.getHeight()*.42f;
        for(size_t i=0;i<wave.size();++i){
            const float x=scope.getX()+scope.getWidth()*(float)i/(float)(wave.size()-1);
            const float y=mid-wave[i]*amp;
            if(i==0)p.startNewSubPath(x,y);else p.lineTo(x,y);
        }
        g.setColour(juce::Colour(0xffc25d58).withAlpha(.82f));g.strokePath(p,juce::PathStrokeType(1.25f));

        const float bw=spectrum.getWidth()/(float)bins.size();
        for(size_t i=0;i<bins.size();++i){
            const float h=juce::jlimit(0.f,1.f,bins[i])*spectrum.getHeight();
            g.setColour(juce::Colour(0xffa779c1).withAlpha(.18f+.55f*bins[i]));
            g.fillRect(spectrum.getX()+i*bw,spectrum.getBottom()-h,std::max(1.f,bw-.7f),h);
        }
        const float cx=scope.getRight()-24.f,cy=scope.getY()+18.f;
        g.setColour(juce::Colour(0xffb58bd0).withAlpha(.22f));g.fillEllipse(cx-18.f*width,cy-5.f,36.f*width,10.f);
        g.setColour(juce::Colour(0xffd3c5ad).withAlpha(.70f));g.setFont(juce::FontOptions(8.f));
        g.drawText("WIDTH",juce::Rectangle<float>(cx-24,cy+7,48,10).toNearestInt(),juce::Justification::centred);
    }
private:
    void timerCallback() override { repaint(); }
    HorrorCastleProcessor& processor;
};

class CreaturePortraitComponent final : public juce::Component, private juce::Timer
{
public:
    explicit CreaturePortraitComponent(juce::AudioProcessorValueTreeState& s):state(s){setInterceptsMouseClicks(false,false);startTimerHz(10);}
    void paint(juce::Graphics& g) override
    {
        auto r=getLocalBounds().toFloat();
        g.setColour(juce::Colour(0xff050608));g.fillRoundedRectangle(r,6.f);
        const bool crypt=dominantCrypt(); const int type=dominantType(crypt);
        const auto& c=synthesis_contract::get(static_cast<GeneratorType>(type),crypt);
        const auto accent=crypt?juce::Colour(0xffc45a55):juce::Colour(0xffa979c5);
        auto portrait=r.removeFromLeft(r.getWidth()*.38f).reduced(12);
        drawCreature(g,portrait,accent,type,crypt);
        auto copy=r.reduced(8,8);
        g.setColour(accent);g.setFont(juce::Font(juce::FontOptions(15.f)).boldened());g.drawFittedText(c.creature,copy.removeFromTop(22).toNearestInt(),juce::Justification::centredLeft,1);
        g.setColour(juce::Colour(0xffc7bcad));g.setFont(juce::FontOptions(9.5f));g.drawFittedText(c.family,copy.removeFromTop(17).toNearestInt(),juce::Justification::centredLeft,1);
        g.setColour(juce::Colour(0xff9d9588));g.setFont(juce::FontOptions(8.7f));
        g.drawFittedText(juce::String("SUMMON // ")+c.excitation+"\nFORM // "+c.synthesisMethod+"\nMORPH // "+c.morphTrajectory,
                         copy.toNearestInt(),juce::Justification::topLeft,4);
    }
private:
    float read(const juce::String& id,float fallback=0.f) const {if(auto* p=state.getRawParameterValue(id))return p->load();return fallback;}
    bool dominantCrypt() const {float c=0,t=0;for(int i=1;i<=3;++i){c+=read(param::id("crypt",i,"level"))*(read(param::id("crypt",i,"enabled"))>.5f);t+=read(param::id("tower",i,"level"))*(read(param::id("tower",i,"enabled"))>.5f);}return c>=t;}
    int dominantType(bool crypt) const {const char* s=crypt?"crypt":"tower";int best=0;float level=-1;for(int i=1;i<=3;++i){float l=read(param::id(s,i,"level"))*(read(param::id(s,i,"enabled"))>.5f);if(l>level){level=l;best=(int)std::lround(read(param::id(s,i,"type")));}}return juce::jlimit(0,17,best);}
    static void drawCreature(juce::Graphics& g,juce::Rectangle<float> r,juce::Colour accent,int type,bool crypt){
        const auto c=r.getCentre();g.setColour(accent.withAlpha(.08f));g.fillEllipse(r);
        juce::Path silhouette; silhouette.startNewSubPath(c.x,c.y-r.getHeight()*.38f);
        const float horns=4.f+(type%6)*2.f;
        silhouette.lineTo(c.x-horns,c.y-r.getHeight()*.20f);silhouette.lineTo(c.x-r.getWidth()*.24f,c.y-r.getHeight()*.34f);
        silhouette.lineTo(c.x-r.getWidth()*.18f,c.y-r.getHeight()*.05f);silhouette.quadraticTo(c.x-r.getWidth()*.30f,c.y+r.getHeight()*.18f,c.x-r.getWidth()*.36f,c.y+r.getHeight()*.40f);
        silhouette.lineTo(c.x+r.getWidth()*.36f,c.y+r.getHeight()*.40f);silhouette.quadraticTo(c.x+r.getWidth()*.30f,c.y+r.getHeight()*.18f,c.x+r.getWidth()*.18f,c.y-r.getHeight()*.05f);
        silhouette.lineTo(c.x+r.getWidth()*.24f,c.y-r.getHeight()*.34f);silhouette.lineTo(c.x+horns,c.y-r.getHeight()*.20f);silhouette.closeSubPath();
        g.setColour(juce::Colour(0xff15171a));g.fillPath(silhouette);g.setColour(accent.withAlpha(.62f));g.strokePath(silhouette,juce::PathStrokeType(1.f));
        g.setColour(crypt?juce::Colour(0xffd14e48):juce::Colour(0xffb885d6));g.fillEllipse(c.x-10,c.y-8,4,3);g.fillEllipse(c.x+6,c.y-8,4,3);
    }
    void timerCallback() override { repaint(); }
    juce::AudioProcessorValueTreeState& state;
};

} // namespace horrorcastle

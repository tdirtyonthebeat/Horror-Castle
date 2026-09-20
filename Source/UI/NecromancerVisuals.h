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
    explicit SoulGlassComponent(HorrorCastleProcessor& p) : processor(p), state(p.getParameterState())
    {
        setInterceptsMouseClicks(false,false);
        startTimerHz(30);
    }
    void focusCreature(bool crypt,int type){focusCrypt=crypt;focusType=juce::jlimit(0,17,type);focusValid=true;repaint();}

    void paint(juce::Graphics& g) override
    {
        auto r=getLocalBounds().toFloat();
        juce::ColourGradient glass(juce::Colour(0xff020305),r.getCentreX(),r.getY(),
                                   juce::Colour(0xff0b0910),r.getCentreX(),r.getBottom(),false);
        g.setGradientFill(glass);g.fillRoundedRectangle(r,10.f);
        g.setColour(juce::Colour(0xff8c7352).withAlpha(.34f));g.drawRoundedRectangle(r.reduced(.5f),10.f,1.f);
        g.setColour(juce::Colour(0xffa979c5).withAlpha(.10f));g.drawRoundedRectangle(r.reduced(5.f),7.f,.8f);
        auto title=r.removeFromTop(34.f).reduced(12,0);
        g.setColour(juce::Colour(0xffeee1cb));g.setFont(juce::Font(juce::FontOptions(13.f)).boldened());
        g.drawText("SOUL GLASS",title.removeFromLeft(116).toNearestInt(),juce::Justification::centredLeft);
        g.setColour(juce::Colour(0xff8f877b));g.setFont(juce::FontOptions(8.2f));
        g.drawText("LIVE BODY  •  SPECTRAL AURA  •  APPARITION WIDTH",title.toNearestInt(),juce::Justification::centredLeft);

        auto scope=r.reduced(12,7); auto spectrum=scope.removeFromBottom(scope.getHeight()*.42f);
        g.setColour(juce::Colour(0xff6f737a).withAlpha(.10f));
        for(int i=1;i<4;++i)g.drawHorizontalLine((int)(scope.getY()+scope.getHeight()*i/4.f),scope.getX(),scope.getRight());
        g.setColour(juce::Colour(0xff8c7352).withAlpha(.16f));
        g.drawVerticalLine((int)scope.getCentreX(),scope.getY(),scope.getBottom());

        std::array<float,512> wave{}; std::array<float,64> bins{}; float width=0.f;
        processor.copySoulGlass(wave,bins,width);
        // Real spectrum is intentionally calculated on the message thread, never
        // the audio thread. A Hann-windowed 512-sample DFT is plenty for this
        // teaching display and keeps synthesis real-time safe.
        float spectralPeak=1.0e-6f;
        for(size_t k=0;k<bins.size();++k){double re=0.0,im=0.0;const double omega=juce::MathConstants<double>::twoPi*(double)(k+1)/(double)wave.size();for(size_t n=0;n<wave.size();++n){const double win=.5-.5*std::cos(juce::MathConstants<double>::twoPi*(double)n/(double)(wave.size()-1));const double a=omega*(double)n;re+=wave[n]*win*std::cos(a);im-=wave[n]*win*std::sin(a);}bins[k]=(float)std::sqrt(re*re+im*im);spectralPeak=std::max(spectralPeak,bins[k]);}
        for(auto& b:bins)b=juce::jlimit(0.f,1.f,b/spectralPeak);
        const auto identity=focusedIdentity();
        const auto& contract=synthesis_contract::get(static_cast<GeneratorType>(identity.second),identity.first);

        juce::Path p; const float mid=scope.getCentreY(),amp=scope.getHeight()*.42f;
        for(size_t i=0;i<wave.size();++i){
            const float x=scope.getX()+scope.getWidth()*(float)i/(float)(wave.size()-1);
            const float y=mid-wave[i]*amp;
            if(i==0)p.startNewSubPath(x,y);else p.lineTo(x,y);
        }
        const auto waveAccent=identity.first?juce::Colour(0xffd15f59):juce::Colour(0xffb985dc);
        g.setColour(waveAccent.withAlpha(.16f));g.strokePath(p,juce::PathStrokeType(3.2f));
        g.setColour(waveAccent.withAlpha(.90f));g.strokePath(p,juce::PathStrokeType(1.15f));

        const float bw=spectrum.getWidth()/(float)bins.size();
        for(size_t i=0;i<bins.size();++i){
            const float h=juce::jlimit(0.f,1.f,bins[i])*spectrum.getHeight();
            const auto barColour=identity.first?juce::Colour(0xffc75d57):juce::Colour(0xffaa79c8);
            g.setColour(barColour.withAlpha(.10f+.58f*bins[i]));
            g.fillRoundedRectangle(spectrum.getX()+i*bw,spectrum.getBottom()-h,std::max(1.f,bw-.9f),h,1.2f);
        }
        const float cx=scope.getRight()-24.f,cy=scope.getY()+18.f;
        g.setColour(waveAccent.withAlpha(.16f+.18f*width));g.fillEllipse(cx-22.f*width,cy-6.f,44.f*width,12.f);
        g.setColour(juce::Colour(0xffd3c5ad).withAlpha(.70f));g.setFont(juce::FontOptions(8.f));
        g.drawText("WIDTH",juce::Rectangle<float>(cx-28,cy+8,56,10).toNearestInt(),juce::Justification::centred);

        // FAMILY LENS: the same audio telemetry is interpreted according to the
        // synthesis law. No fake animation: every glyph is anchored to waveform,
        // spectral energy, stereo width, MORPH or live ecology telemetry.
        drawFamilyLens(g,spectrum,wave,bins,width,identity.first,identity.second,contract);
    }
private:
    float read(const juce::String& id,float fallback=0.f) const {if(auto* p=state.getRawParameterValue(id))return p->load();return fallback;}
    std::pair<bool,int> focusedIdentity() const {
        // Follow the last creature selector the Necromancer touched. This makes
        // auditioning deterministic; fall back to the loudest active creature.
        if(focusValid)return {focusCrypt,focusType};
        return dominantIdentity();
    }
    std::pair<bool,int> dominantIdentity() const {
        bool bestCrypt=true; int bestType=0; float best=-1.f;
        for(bool crypt:{true,false}){const char* s=crypt?"crypt":"tower";for(int i=1;i<=3;++i){
            const float level=read(param::id(s,i,"level"))*(read(param::id(s,i,"enabled"))>.5f);
            if(level>best){best=level;bestCrypt=crypt;bestType=juce::jlimit(0,17,(int)std::lround(read(param::id(s,i,"type"))));}
        }}
        return {bestCrypt,bestType};
    }
    static float bandEnergy(const std::array<float,64>& b,int a,int z){float e=0.f;for(int i=a;i<z;++i)e+=b[(size_t)i];return e/std::max(1,z-a);}
    void drawFamilyLens(juce::Graphics& g,juce::Rectangle<float> r,const std::array<float,512>& wave,
                        const std::array<float,64>& bins,float width,bool crypt,int type,const SynthesisFamilyContract& contract) const {
        const auto accent=crypt?juce::Colour(0xffc65b55):juce::Colour(0xffaa7ac8);
        const float low=bandEnergy(bins,0,16),mid=bandEnergy(bins,16,40),high=bandEnergy(bins,40,64);
        const float morph=read(param::id(crypt?"crypt":"tower",1,"shape"),.5f);
        auto lens=r.reduced(3); lens.removeFromTop(3);
        g.setColour(accent.withAlpha(.76f));g.setFont(juce::FontOptions(7.8f));
        g.drawFittedText(juce::String("LENS // ")+contract.family+" // "+contract.spectralMotion,
                         lens.removeFromTop(12).toNearestInt(),juce::Justification::centredLeft,1);

        const auto gt=static_cast<GeneratorType>(type);
        if(gt==GeneratorType::FM||gt==GeneratorType::PM||(!crypt&&gt==GeneratorType::ChamberIII)){
            // Operator/sideband constellation.
            const auto c=lens.getCentre(); const float rad=7.f+18.f*juce::jlimit(0.f,1.f,high+mid);
            for(int i=0;i<6;++i){const float a=juce::MathConstants<float>::twoPi*i/6.f+morph;const float rr=rad*(.45f+.55f*bins[(size_t)(8+i*7)]);g.setColour(accent.withAlpha(.25f+.5f*bins[(size_t)(8+i*7)]));g.fillEllipse(c.x+std::cos(a)*rr-2,c.y+std::sin(a)*rr-2,4,4);}
            g.setColour(accent.withAlpha(.65f));g.drawEllipse(c.x-rad,c.y-rad,2*rad,2*rad,.7f);
        } else if((!crypt&&gt==GeneratorType::ChamberIV)){
            // Grain cloud: density comes from high-frequency/temporal activity.
            for(int i=0;i<18;++i){const float e=bins[(size_t)((i*11)%64)];const float x=lens.getX()+std::fmod(i*.6180339f,1.f)*lens.getWidth();const float y=lens.getY()+std::fmod(i*.4142135f+e,1.f)*lens.getHeight();g.setColour(accent.withAlpha(.12f+.62f*e));g.fillEllipse(x,y,1.5f+4.f*e,1.5f+4.f*e);}
        } else if((crypt&&gt==GeneratorType::ChamberII)||(!crypt&&gt==GeneratorType::ChamberVIII)){
            // Spectral partial pillars / reflected frames.
            for(int i=0;i<12;++i){const float e=bins[(size_t)(3+i*5)];const float x=lens.getX()+i*lens.getWidth()/12.f;g.setColour(accent.withAlpha(.16f+.58f*e));g.drawLine(x,lens.getCentreY()-e*22.f,x,lens.getCentreY()+e*22.f,.8f);}
        } else if((crypt&&gt==GeneratorType::ChamberX)||(!crypt&&gt==GeneratorType::ChamberX)){
            // Physical flow/jet: ecology pressure/event bends stream lines.
            const float ecology=crypt?processor.getEcologyMeter(CastleEngine::VortexEvent):processor.getEcologyMeter(CastleEngine::SirenPressure);
            for(int i=0;i<3;++i){juce::Path p;for(int x=0;x<48;++x){const float xx=lens.getX()+lens.getWidth()*x/47.f;const float yy=lens.getCentreY()+(i-1)*8.f+std::sin(x*.28f+i+ecology*5.f)*ecology*10.f;if(x==0)p.startNewSubPath(xx,yy);else p.lineTo(xx,yy);}g.setColour(accent.withAlpha(.25f+.45f*ecology));g.strokePath(p,juce::PathStrokeType(.8f));}
        } else {
            // Universal harmonic/body map for VA, additive, modal and hybrids.
            const float vals[3]={low,mid,high};for(int i=0;i<3;++i){auto b=lens.removeFromLeft(lens.getWidth()/(3-i));b=b.reduced(3);const float h=b.getHeight()*juce::jlimit(0.f,1.f,vals[i]);g.setColour(accent.withAlpha(.18f+.55f*vals[i]));g.fillRoundedRectangle(b.withTop(b.getBottom()-h),2.f);}
        }
        juce::ignoreUnused(wave,width);
    }
    void timerCallback() override { repaint(); }
    HorrorCastleProcessor& processor;
    juce::AudioProcessorValueTreeState& state;
    bool focusValid=false,focusCrypt=true; int focusType=0;
};

class CreaturePortraitComponent final : public juce::Component, private juce::Timer
{
public:
    explicit CreaturePortraitComponent(juce::AudioProcessorValueTreeState& s):state(s){setInterceptsMouseClicks(false,false);startTimerHz(10);}
    void focusCreature(bool crypt,int type){focusCrypt=crypt;focusType=juce::jlimit(0,17,type);focusValid=true;repaint();}
    void paint(juce::Graphics& g) override
    {
        auto r=getLocalBounds().toFloat();
        juce::ColourGradient card(juce::Colour(0xff030406),r.getX(),r.getY(),juce::Colour(0xff0c090d),r.getRight(),r.getBottom(),false);
        g.setGradientFill(card);g.fillRoundedRectangle(r,10.f);
        const bool crypt=focusValid?focusCrypt:dominantCrypt(); const int type=focusValid?focusType:dominantType(crypt);
        const auto& c=synthesis_contract::get(static_cast<GeneratorType>(type),crypt);
        const auto accent=crypt?juce::Colour(0xffc45a55):juce::Colour(0xffa979c5);
        g.setColour(accent.withAlpha(.26f));g.drawRoundedRectangle(r.reduced(.5f),10.f,1.f);
        auto portrait=r.removeFromLeft(r.getWidth()*.44f).reduced(14);
        drawCreature(g,portrait,accent,type,crypt);
        auto copy=r.reduced(12,12);
        g.setColour(juce::Colour(0xff8f877b));g.setFont(juce::FontOptions(8.f));
        g.drawText("CURRENT MANIFESTATION",copy.removeFromTop(16).toNearestInt(),juce::Justification::centredLeft);
        g.setColour(juce::Colour(0xfff0e4d1));g.setFont(juce::Font(juce::FontOptions(21.f)).boldened());
        g.drawFittedText(c.creature,copy.removeFromTop(32).toNearestInt(),juce::Justification::centredLeft,1);
        g.setColour(accent);g.setFont(juce::Font(juce::FontOptions(10.f)).boldened());
        g.drawFittedText(c.family,copy.removeFromTop(20).toNearestInt(),juce::Justification::centredLeft,1);
        g.setColour(juce::Colour(0xffb1a89b));g.setFont(juce::FontOptions(8.8f));
        g.drawFittedText(juce::String("EXCITATION  //  ")+c.excitation+"\nMETHOD  //  "+c.synthesisMethod+"\nMORPH  //  "+c.morphTrajectory,
                         copy.toNearestInt(),juce::Justification::topLeft,5);
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
    bool focusValid=false,focusCrypt=true; int focusType=0;
};

} // namespace horrorcastle

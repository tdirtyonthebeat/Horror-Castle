#pragma once
#include <JuceHeader.h>
#include "../HorrorCastle/CastleParameters.h"

namespace horrorcastle {

// Performance chrome is deliberately architectural, not a second control layer.
// It frames two independent summoning chambers and leaves the Castle artwork,
// creature manifestations and Soul Glass as the visual hierarchy.
class LivingCastleChrome final : public juce::Component, private juce::Timer
{
public:
    explicit LivingCastleChrome(juce::AudioProcessorValueTreeState& state) : apvts(state)
    {
        setInterceptsMouseClicks(false,false);
        startTimerHz(24);
    }

    void paint(juce::Graphics& g) override
    {
        using namespace juce;
        const auto r=getLocalBounds().toFloat();
        const float w=r.getWidth(),h=r.getHeight(),cx=r.getCentreX();

        ColourGradient bg(Colour(0xff030406),0,0,Colour(0xff0b0810),w,h,false);
        bg.addColour(.42,Colour(0xff08090d));
        g.setGradientFill(bg);g.fillRect(r);

        // Architectural vignette keeps attention on the creature chambers.
        ColourGradient vignette(Colours::transparentBlack,cx,h*.42f,Colour(0xff000000).withAlpha(.72f),cx,h*.96f,true);
        g.setGradientFill(vignette);g.fillRect(r);

        drawSanctum(g,{42.f,88.f,w*.36f,430.f},Colour(0xffc65750),true);
        drawSanctum(g,{w*.64f-42.f,88.f,w*.36f,430.f},Colour(0xffaa79c9),false);

        // Central ritual axis: one quiet focal spine rather than a panel wall.
        Path spine;
        spine.startNewSubPath(cx,104.f);
        spine.lineTo(cx-10.f,132.f);spine.lineTo(cx,160.f);spine.lineTo(cx+10.f,132.f);spine.closeSubPath();
        for(int y=172;y<514;y+=38){spine.startNewSubPath(cx-5.f,(float)y);spine.lineTo(cx,(float)y+10.f);spine.lineTo(cx+5.f,(float)y);}
        g.setColour(Colour(0xffb58b58).withAlpha(.26f));g.strokePath(spine,PathStrokeType(1.1f));
        g.setColour(Colour(0xff6d536f).withAlpha(.12f));g.fillEllipse(cx-54.f,244.f,108.f,108.f);
        g.setColour(Colour(0xffb58b58).withAlpha(.20f));g.drawEllipse(cx-42.f,256.f,84.f,84.f,.9f);
        g.drawEllipse(cx-27.f,271.f,54.f,54.f,.7f);

        // Witness gallery below: portrait -> Soul Glass -> Grimoire. These are
        // visual destinations, not controls, so the chrome only gives them depth.
        auto lower=Rectangle<float>(34.f,724.f,w-68.f,h-744.f);
        g.setColour(Colour(0xff030405).withAlpha(.62f));g.fillRoundedRectangle(lower,16.f);
        g.setColour(Colour(0xff8b6c49).withAlpha(.24f));g.drawRoundedRectangle(lower,16.f,1.f);
        g.setColour(Colour(0xffd6c8af).withAlpha(.50f));g.setFont(Font(FontOptions(9.5f)).boldened());
        g.drawText("WITNESS CHAMBER",lower.withHeight(24.f).reduced(14,0).toNearestInt(),Justification::centredLeft);

        // Subtle reactive torch halos. These use actual oscillator enable/level
        // state so the room visually wakes with the instrument.
        const float cryptLife=life("crypt"),towerLife=life("tower");
        drawTorch(g,{66.f,126.f,28.f,66.f},Colour(0xffc65750),cryptLife);
        drawTorch(g,{w-94.f,126.f,28.f,66.f},Colour(0xffaa79c9),towerLife);

        // Slow mist sits behind controls and reacts modestly to activity.
        for(int i=0;i<4;++i){
            const float travel=w+360.f;
            const float x=std::fmod(phase*(7.f+i*2.3f)+i*337.f,travel)-180.f;
            const float y=176.f+i*178.f;
            const float lifeMix=.5f*(cryptLife+towerLife);
            g.setColour(Colour(0xffb7c2ca).withAlpha(.008f+.012f*lifeMix));
            g.fillEllipse(x,y,360.f,56.f+18.f*lifeMix);
        }
    }

private:
    juce::AudioProcessorValueTreeState& apvts;
    float phase=0.f;

    float read(const juce::String& id,float fallback=0.f) const
    {
        if(auto* p=apvts.getRawParameterValue(id))return p->load();
        return fallback;
    }

    float life(const char* scene) const
    {
        const float on=read(horrorcastle::param::id(scene,1,"enabled"),0.f)>.5f?1.f:0.f;
        return juce::jlimit(0.f,1.f,on*read(horrorcastle::param::id(scene,1,"level"),0.f));
    }

    static void drawSanctum(juce::Graphics& g,juce::Rectangle<float> r,juce::Colour accent,bool crypt)
    {
        using namespace juce;
        g.setColour(Colour(0xff050608).withAlpha(.68f));g.fillRoundedRectangle(r,18.f);
        g.setColour(accent.withAlpha(.22f));g.drawRoundedRectangle(r,18.f,1.2f);

        // Gothic arch.
        auto inner=r.reduced(18.f);
        Path arch;
        arch.startNewSubPath(inner.getX(),inner.getBottom());
        arch.lineTo(inner.getX(),inner.getY()+84.f);
        arch.quadraticTo(inner.getCentreX(),inner.getY()-26.f,inner.getRight(),inner.getY()+84.f);
        arch.lineTo(inner.getRight(),inner.getBottom());
        g.setColour(Colour(0xff121216).withAlpha(.84f));g.fillPath(arch);
        g.setColour(accent.withAlpha(.18f));g.strokePath(arch,PathStrokeType(1.f));

        g.setColour(accent.withAlpha(.52f));g.setFont(Font(FontOptions(10.f)).boldened());
        g.drawText(crypt?"CRYPT SANCTUM // OSCILLATOR A":"TOWER SANCTUM // OSCILLATOR B",
                   r.withHeight(26.f).reduced(18,0).toNearestInt(),Justification::centredLeft);

        // Stone scoring, intentionally sparse.
        g.setColour(Colour(0xff2a272b).withAlpha(.34f));
        for(float y=r.getY()+46.f;y<r.getBottom()-14.f;y+=34.f)g.drawLine(r.getX()+10.f,y,r.getRight()-10.f,y,.35f);
    }

    static void drawTorch(juce::Graphics& g,juce::Rectangle<float> r,juce::Colour accent,float life)
    {
        using namespace juce;
        g.setColour(Colour(0xff161316));g.fillRoundedRectangle(r.withTrimmedTop(22.f),3.f);
        const auto c=Point<float>(r.getCentreX(),r.getY()+18.f);
        g.setColour(accent.withAlpha(.05f+.16f*life));g.fillEllipse(c.x-24.f-life*8.f,c.y-24.f-life*8.f,48.f+life*16.f,48.f+life*16.f);
        g.setColour(accent.withAlpha(.35f+.55f*life));
        Path flame;flame.startNewSubPath(c.x,c.y-13.f-life*7.f);flame.quadraticTo(c.x-11.f,c.y+4.f,c.x,c.y+12.f);flame.quadraticTo(c.x+11.f,c.y+4.f,c.x,c.y-13.f-life*7.f);g.fillPath(flame);
    }

    void timerCallback() override
    {
        phase=std::fmod(phase+.55f,100000.f);
        repaint();
    }
};

} // namespace horrorcastle

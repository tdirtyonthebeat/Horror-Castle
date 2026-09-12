#pragma once
#include <JuceHeader.h>
#include "../Core/HorrorCastleProcessor.h"
#include "Theme/CastleTheme.h"
#include "Theme/CastleGraphics.h"
#include <array>

namespace horrorcastle {

// Production UI for the Creature Routing Matrix. The meters are read-only
// telemetry sourced from the real audio engine; no duplicate visual simulation.
class NervousSystemComponent final : public juce::Component, private juce::Timer
{
public:
    explicit NervousSystemComponent(HorrorCastleProcessor& p)
        : processor(p), apvts(p.getParameterState())
    {
        title.setText("THE NERVOUS SYSTEM", juce::dontSendNotification);
        title.setJustificationType(juce::Justification::centredLeft);
        title.setFont(juce::Font(juce::FontOptions(14.0f)).boldened());
        title.setColour(juce::Label::textColourId, theme::gold());
        addAndMakeVisible(title);

        subtitle.setText("PHYSICAL SYSTEMS HAUNTING OTHER PHYSICAL SYSTEMS", juce::dontSendNotification);
        subtitle.setJustificationType(juce::Justification::centredLeft);
        subtitle.setFont(juce::FontOptions(8.5f));
        subtitle.setColour(juce::Label::textColourId, theme::parchment().withAlpha(.58f));
        addAndMakeVisible(subtitle);

        enabled.setButtonText("AWAKEN");
        enabled.setColour(juce::ToggleButton::textColourId, theme::bone());
        enabledAttachment=std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts,"ecology.enabled",enabled);
        addAndMakeVisible(enabled);

        depth.setSliderStyle(juce::Slider::LinearHorizontal);
        depth.setTextBoxStyle(juce::Slider::TextBoxRight,false,48,18);
        depth.setColour(juce::Slider::trackColourId,theme::cryptEmber());
        depth.setColour(juce::Slider::thumbColourId,theme::gold());
        depth.setColour(juce::Slider::textBoxTextColourId,theme::bone());
        depth.setColour(juce::Slider::textBoxBackgroundColourId,juce::Colour(0xff050608));
        depthAttachment=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts,"ecology.depth",depth);
        addAndMakeVisible(depth);

        depthLabel.setText("ECOLOGY DEPTH",juce::dontSendNotification);
        depthLabel.setFont(juce::FontOptions(8.5f));
        depthLabel.setColour(juce::Label::textColourId,theme::bone().withAlpha(.75f));
        addAndMakeVisible(depthLabel);
        startTimerHz(30);
    }

    ~NervousSystemComponent() override { stopTimer(); }

    void resized() override
    {
        auto r=getLocalBounds().reduced(18,12);
        auto head=r.removeFromTop(38);
        title.setBounds(head.removeFromTop(20)); subtitle.setBounds(head);
        auto controls=r.removeFromTop(34);
        enabled.setBounds(controls.removeFromLeft(105));
        depthLabel.setBounds(controls.removeFromLeft(94));
        depth.setBounds(controls.removeFromLeft(210));
    }

    void paint(juce::Graphics& g) override
    {
        auto r=getLocalBounds().toFloat();
        CastleGraphics::drawStonePanel(g,r.reduced(1.0f),theme::gold(),7.0f);
        CastleGraphics::drawRuneBorder(g,r.reduced(4.0f),theme::gold(),.17f);
        g.setColour(juce::Colour(0xff030507).withAlpha(.78f));
        g.fillRoundedRectangle(r.reduced(10.0f).withTrimmedTop(74.0f),5.0f);

        const float y0=95.0f;
        drawRoute(g,{18.0f,y0,getWidth()-36.0f,42.0f},"VORTEX  COLLAPSE EVENT", "SIREN  PLENUM PRESSURE", meters[0],meters[1],theme::cryptEmber(),theme::towerGlow());
        drawRoute(g,{18.0f,y0+48.0f,getWidth()-36.0f,42.0f},"POLTERGEIST  INSTABILITY", "AURORA  FIELD", meters[2],meters[3],theme::cryptRed(),theme::towerPurple());
    }

private:
    HorrorCastleProcessor& processor;
    juce::AudioProcessorValueTreeState& apvts;
    juce::Label title,subtitle,depthLabel;
    juce::ToggleButton enabled;
    juce::Slider depth;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> enabledAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> depthAttachment;
    std::array<float,4> meters{};

    void timerCallback() override
    {
        for(int i=0;i<4;++i){
            const float target=juce::jlimit(0.f,1.f,processor.getEcologyMeter(i));
            meters[(size_t)i]+=0.22f*(target-meters[(size_t)i]);
        }
        repaint();
    }

    static void meter(juce::Graphics& g,juce::Rectangle<float> area,float v,juce::Colour c)
    {
        v=juce::jlimit(0.f,1.f,v);
        g.setColour(juce::Colour(0xff05070a));g.fillRoundedRectangle(area,2.0f);
        g.setColour(c.withAlpha(.16f));g.fillRoundedRectangle(area.withWidth(area.getWidth()*v),2.0f);
        g.setColour(c.withAlpha(.88f));g.fillRect(area.getX(),area.getBottom()-2.0f,area.getWidth()*v,2.0f);
        g.setColour(theme::gold().withAlpha(.18f));g.drawRoundedRectangle(area,2.0f,.8f);
    }

    static void drawRoute(juce::Graphics& g,juce::Rectangle<float> area,const juce::String& src,const juce::String& dst,float a,float b,juce::Colour ca,juce::Colour cb)
    {
        auto top=area.removeFromTop(15.0f);
        g.setFont(juce::Font(juce::FontOptions(8.4f)).boldened());
        g.setColour(ca.withAlpha(.90f));g.drawText(src,top.removeFromLeft(area.getWidth()*.47f).toNearestInt(),juce::Justification::centredLeft);
        g.setColour(theme::gold().withAlpha(.72f));g.drawText("→",top.removeFromLeft(area.getWidth()*.06f).toNearestInt(),juce::Justification::centred);
        g.setColour(cb.withAlpha(.90f));g.drawText(dst,top.toNearestInt(),juce::Justification::centredRight);
        auto bars=area.reduced(0,5);auto left=bars.removeFromLeft(bars.getWidth()*.47f);bars.removeFromLeft(bars.getWidth()*.11f);auto right=bars;
        meter(g,left,a,ca);meter(g,right,b,cb);
    }
};

} // namespace horrorcastle

#include "SirenEngine.h"
#include <cmath>

namespace horrorcastle {

float SirenEngine::renderSample(VoiceState& s,float fundamentalHz,float aperture,float aether,float expression,float velocity,double sr,float externalEvent,float eventCoupling) noexcept {
    if(sr<=1.0||fundamentalHz<=0.0f) return 0.0f;
    aperture=juce::jlimit(0.0f,1.0f,aperture); aether=juce::jlimit(0.0f,1.0f,aether); expression=juce::jlimit(0.0f,1.0f,expression); velocity=juce::jlimit(0.0f,1.0f,velocity);
    externalEvent=CreatureStateBus::sanitize(externalEvent);
    eventCoupling=std::isfinite(eventCoupling)?juce::jlimit(-1.0f,1.0f,eventCoupling):0.0f;

    // Aperture now crosses three audible regimes: closed/breathy, edge-locked,
    // and overblown. Aether controls how strongly jets phase-lock through the plenum.
    const float apertureCurve=aperture*aperture;
    const float breath=(0.08f+0.92f*expression)*(0.25f+0.75f*velocity);
    const float overblow=juce::jlimit(0.0f,1.0f,(aperture-.62f)/.38f);
    const float targetPlenum=breath*(0.30f+0.72f*apertureCurve)*(1.0f+0.32f*overblow);
    s.ecologicalImpulse=juce::jlimit(-1.0f,1.0f,s.ecologicalImpulse*0.960f+externalEvent*eventCoupling*0.14f);
    const float disturbedTarget=juce::jlimit(0.0f,1.35f,targetPlenum+s.ecologicalImpulse*(0.14f+0.30f*aether));
    s.plenum += (0.0007f+0.0055f*(1.0f-aether))*(disturbedTarget-s.plenum);
    s.plenum=juce::jlimit(0.0f,1.35f,s.plenum);
    const float fieldRate=(0.05f+0.42f*aether*aether)/(float)sr;
    s.field += fieldRate*(std::sin((s.plenum+s.ecologicalImpulse*0.38f)*4.1f)-s.field*0.42f);
    s.field=juce::jlimit(-1.0f,1.0f,s.field);
    constexpr std::array<float,Jets> ratios{1.0f,1.503f,2.019f,3.011f};
    float sum=0.0f, motionMean=0.0f;
    for(int i=0;i<Jets;++i){
        auto& j=s.jets[(size_t)i];
        const float edgeBias=(float)i/(float)(Jets-1)-0.5f;
        const float speed=s.plenum*(0.62f+0.58f*apertureCurve)*(1.0f+edgeBias*0.14f*aether);
        const float modeJump=1.0f+overblow*(0.48f+0.12f*(float)(i&1));
        const float edgeFreq=fundamentalHz*ratios[(size_t)i]*modeJump*(0.78f+0.35f*aperture+0.055f*s.field*(float)(i+1)+0.020f*s.ecologicalImpulse*(float)(i+1));
        if(edgeFreq<sr*0.46){ j.phase+=edgeFreq/(float)sr; j.phase-=std::floor(j.phase); }
        const float carrier=std::sin(juce::MathConstants<float>::twoPi*j.phase);
        const float airy=std::sin(juce::MathConstants<float>::twoPi*j.phase*0.5f + j.edgeMemory*1.7f);
        const float jetTarget=std::tanh(carrier*(1.2f+4.8f*speed)+j.edgeMemory*(0.28f+0.72f*aether)+s.ecologicalImpulse*(0.16f+0.11f*(float)i));
        j.displacement += (0.012f+0.070f*speed)*(jetTarget-j.displacement);
        const float edgeOffset=(aperture-0.5f)*0.62f;
        const float edge=std::tanh((j.displacement-edgeOffset)*(1.8f+7.2f*speed));
        j.edgeMemory=(0.986f-0.010f*overblow)*j.edgeMemory+(0.014f+0.010f*overblow)*edge;
        const float brightness=0.12f+0.32f*apertureCurve+0.12f*aether*(float)(i+1)/(float)Jets;
        const float breathNoise=airy*(1.0f-aperture)*(.025f+.055f*expression);
        sum += edge*(0.11f+brightness)
             + carrier*j.displacement*(0.04f+0.13f*aether)
             + breathNoise
             + std::tanh(edge*carrier*3.0f)*overblow*0.08f;
        motionMean += std::abs(j.displacement);
    }
    motionMean*=0.25f;
    const float choirLock=std::tanh(sum*s.field*(0.10f+0.30f*aether));
    const float out=sum*0.38f + s.field*(0.04f+0.18f*aether) + choirLock;
    if(!std::isfinite(out)){ s=VoiceState{}; return 0.0f; }

    auto& bus=s.creatureState;
    bus.set(CreatureStateBus::Signal::Energy, juce::jlimit(0.0f,1.0f,std::abs(out)*0.72f+motionMean*0.28f));
    bus.set(CreatureStateBus::Signal::Pressure, juce::jlimit(0.0f,1.0f,s.plenum/1.35f));
    bus.set(CreatureStateBus::Signal::Motion, motionMean);
    bus.set(CreatureStateBus::Signal::Instability, juce::jlimit(0.0f,1.0f,std::abs(s.ecologicalImpulse)*0.62f+std::abs(s.field)*0.20f+overblow*0.22f));
    bus.set(CreatureStateBus::Signal::Event, juce::jlimit(0.0f,1.0f,std::abs(s.ecologicalImpulse)+overblow*0.15f));
    bus.set(CreatureStateBus::Signal::Field, juce::jlimit(0.0f,1.0f,std::abs(s.field)));

    return juce::jlimit(-1.0f,1.0f,std::tanh(out*1.50f));
}

} // namespace horrorcastle

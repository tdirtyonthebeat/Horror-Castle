#include "SirenEngine.h"
#include <cmath>

namespace horrorcastle {

float SirenEngine::renderSample(VoiceState& s,float fundamentalHz,float aperture,float aether,float expression,float velocity,double sr,float externalEvent,float eventCoupling) noexcept {
    if(sr<=1.0||fundamentalHz<=0.0f) return 0.0f;
    aperture=juce::jlimit(0.0f,1.0f,aperture); aether=juce::jlimit(0.0f,1.0f,aether); expression=juce::jlimit(0.0f,1.0f,expression); velocity=juce::jlimit(0.0f,1.0f,velocity);
    externalEvent=CreatureStateBus::sanitize(externalEvent);
    eventCoupling=std::isfinite(eventCoupling)?juce::jlimit(-1.0f,1.0f,eventCoupling):0.0f;
    const float breath=(0.10f+0.90f*expression)*(0.32f+0.68f*velocity);
    const float targetPlenum=breath*(0.42f+0.58f*aperture);
    s.ecologicalImpulse=juce::jlimit(-1.0f,1.0f,s.ecologicalImpulse*0.965f+externalEvent*eventCoupling*0.12f);
    const float disturbedTarget=juce::jlimit(0.0f,1.2f,targetPlenum+s.ecologicalImpulse*(0.16f+0.24f*aether));
    s.plenum += (0.0009f+0.0045f*(1.0f-aether))*(disturbedTarget-s.plenum);
    s.plenum=juce::jlimit(0.0f,1.25f,s.plenum);
    const float fieldRate=(0.07f+0.31f*aether)/(float)sr;
    s.field += fieldRate*(std::sin((s.plenum+s.ecologicalImpulse*0.35f)*3.7f)-s.field*0.45f);
    s.field=juce::jlimit(-1.0f,1.0f,s.field);
    constexpr std::array<float,Jets> ratios{1.0f,1.497f,2.013f,2.996f};
    float sum=0.0f, motionMean=0.0f;
    for(int i=0;i<Jets;++i){
        auto& j=s.jets[(size_t)i];
        const float edgeBias=(float)i/(float)(Jets-1)-0.5f;
        const float speed=s.plenum*(0.72f+0.46f*aperture)*(1.0f+edgeBias*0.10f*aether);
        const float edgeFreq=fundamentalHz*ratios[(size_t)i]*(0.82f+0.31f*aperture+0.045f*s.field*(float)(i+1)+0.018f*s.ecologicalImpulse*(float)(i+1));
        if(edgeFreq<sr*0.46){ j.phase+=edgeFreq/(float)sr; j.phase-=std::floor(j.phase); }
        const float carrier=std::sin(juce::MathConstants<float>::twoPi*j.phase);
        const float jetTarget=std::tanh(carrier*(1.5f+4.0f*speed)+j.edgeMemory*(0.35f+0.50f*aether)+s.ecologicalImpulse*(0.18f+0.09f*(float)i));
        j.displacement += (0.018f+0.055f*speed)*(jetTarget-j.displacement);
        const float edge=std::tanh((j.displacement-(aperture-0.5f)*0.52f)*(2.2f+5.8f*speed));
        j.edgeMemory=0.982f*j.edgeMemory+0.018f*edge;
        const float brightness=0.18f+0.24f*aperture+0.10f*aether*(float)(i+1)/(float)Jets;
        sum += edge*(0.13f+brightness) + carrier*j.displacement*(0.05f+0.10f*aether);
        motionMean += std::abs(j.displacement);
    }
    motionMean*=0.25f;
    const float out=sum*0.42f + s.field*(0.05f+0.15f*aether);
    if(!std::isfinite(out)){ s=VoiceState{}; return 0.0f; }

    auto& bus=s.creatureState;
    bus.set(CreatureStateBus::Signal::Energy, juce::jlimit(0.0f,1.0f,std::abs(out)*0.75f+motionMean*0.25f));
    bus.set(CreatureStateBus::Signal::Pressure, juce::jlimit(0.0f,1.0f,s.plenum/1.25f));
    bus.set(CreatureStateBus::Signal::Motion, motionMean);
    bus.set(CreatureStateBus::Signal::Instability, juce::jlimit(0.0f,1.0f,std::abs(s.ecologicalImpulse)*0.70f+std::abs(s.field)*0.20f+aether*0.10f));
    bus.set(CreatureStateBus::Signal::Event, juce::jlimit(0.0f,1.0f,std::abs(s.ecologicalImpulse)));
    bus.set(CreatureStateBus::Signal::Field, juce::jlimit(0.0f,1.0f,std::abs(s.field)));

    return juce::jlimit(-1.0f,1.0f,std::tanh(out*1.45f));
}

} // namespace horrorcastle

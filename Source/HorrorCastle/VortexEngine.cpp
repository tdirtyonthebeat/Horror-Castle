#include "VortexEngine.h"
#include <cmath>

namespace horrorcastle {
namespace {
float noise(uint32_t& s) noexcept { s^=s<<13; s^=s>>17; s^=s<<5; return (float)s/2147483648.0f-1.0f; }
}

float VortexEngine::renderSample(VoiceState& s,float fundamentalHz,float turbulence,float dread,float expression,float velocity,double sr) noexcept {
    if(sr<=1.0||fundamentalHz<=0.0f) return 0.0f;
    turbulence=juce::jlimit(0.0f,1.0f,turbulence); dread=juce::jlimit(0.0f,1.0f,dread); expression=juce::jlimit(0.0f,1.0f,expression); velocity=juce::jlimit(0.0f,1.0f,velocity);
    const float drive=(0.12f+0.88f*expression)*(0.25f+0.75f*velocity);
    const float hiss=noise(s.rng)*(0.004f+0.055f*turbulence)*drive;
    const float memory=0.965f+0.030f*dread;
    float sum=0.0f, pressureMean=0.0f, motionMean=0.0f, vortexMean=0.0f;
    constexpr std::array<float,Cells> ratios{0.47f,0.73f,1.11f,1.61f};
    for(int i=0;i<Cells;++i){
        auto& c=s.cells[(size_t)i];
        const auto& prev=s.cells[(size_t)((i+Cells-1)%Cells)];
        const float neighbor=prev.pressure-c.pressure;
        const float chaotic=std::tanh((c.flow*2.6f+c.vortex*1.9f+neighbor*1.3f)*(0.65f+1.75f*turbulence));
        const float forcing=hiss + chaotic*(0.002f+0.018f*turbulence) + s.cavity*(0.001f+0.010f*dread);
        c.flow=std::tanh(c.flow*memory + forcing - c.pressure*(0.012f+0.020f*(1.0f-dread)));
        c.pressure=std::tanh(c.pressure*(0.972f+0.020f*dread)+c.flow*(0.018f+0.035f*drive));
        c.vortex=std::tanh(c.vortex*(0.91f+0.07f*dread)+(c.flow-prev.flow)*(0.12f+0.48f*turbulence));
        const float shed=fundamentalHz*ratios[(size_t)i]*(0.86f+0.30f*turbulence+0.10f*c.vortex);
        if(shed<sr*0.46){ c.phase+=shed/(float)sr; c.phase-=std::floor(c.phase); }
        const float swirl=std::sin(juce::MathConstants<float>::twoPi*c.phase + c.pressure*(1.0f+3.5f*dread));
        sum += swirl*(0.10f+0.22f*std::abs(c.vortex)+0.10f*turbulence) + c.flow*0.05f;
        pressureMean += c.pressure;
        motionMean += std::abs(c.flow);
        vortexMean += std::abs(c.vortex);
    }
    pressureMean*=0.25f;
    motionMean*=0.25f;
    vortexMean*=0.25f;
    const float collapseDrive=std::max(0.0f,std::abs(pressureMean)-(.58f-.24f*dread));
    s.collapse=0.985f*s.collapse+collapseDrive*(0.08f+0.22f*turbulence);
    s.cavity=std::tanh(s.cavity*(0.975f+0.018f*dread)+pressureMean*(0.018f+0.045f*dread)-s.collapse*pressureMean*0.16f);
    const float burst=noise(s.rng)*s.collapse*(0.02f+0.08f*dread);
    const float out=sum*0.42f+s.cavity*(0.20f+0.32f*dread)+burst;
    if(!std::isfinite(out)){ s=VoiceState{}; return 0.0f; }

    auto& bus=s.creatureState;
    bus.set(CreatureStateBus::Signal::Energy, juce::jlimit(0.0f,1.0f,std::abs(out)*0.75f+motionMean*0.25f));
    bus.set(CreatureStateBus::Signal::Pressure, std::abs(pressureMean));
    bus.set(CreatureStateBus::Signal::Motion, motionMean);
    bus.set(CreatureStateBus::Signal::Instability, juce::jlimit(0.0f,1.0f,vortexMean*0.55f+turbulence*0.30f+s.collapse*0.50f));
    bus.set(CreatureStateBus::Signal::Event, juce::jlimit(0.0f,1.0f,collapseDrive*3.5f+s.collapse*0.45f));
    bus.set(CreatureStateBus::Signal::Field, juce::jlimit(0.0f,1.0f,std::abs(s.cavity)));

    return juce::jlimit(-1.0f,1.0f,std::tanh(out*1.55f));
}

} // namespace horrorcastle

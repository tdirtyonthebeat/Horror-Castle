#include "VortexEngine.h"
#include <cmath>

namespace horrorcastle {
namespace {
float noise(uint32_t& s) noexcept { s^=s<<13; s^=s>>17; s^=s<<5; return (float)s/2147483648.0f-1.0f; }
}

float VortexEngine::renderSample(VoiceState& s,float fundamentalHz,float turbulence,float dread,float expression,float velocity,double sr) noexcept {
    if(sr<=1.0||fundamentalHz<=0.0f) return 0.0f;
    turbulence=juce::jlimit(0.0f,1.0f,turbulence); dread=juce::jlimit(0.0f,1.0f,dread); expression=juce::jlimit(0.0f,1.0f,expression); velocity=juce::jlimit(0.0f,1.0f,velocity);

    // Turbulence now changes the *topology* of the fluid interaction: low values
    // are coherent rotating cells, high values shed asymmetric vortices and burst.
    const float drive=(0.10f+0.90f*expression)*(0.20f+0.80f*velocity);
    const float turbulence2=turbulence*turbulence;
    const float hiss=noise(s.rng)*(0.002f+0.075f*turbulence2)*drive;
    const float memory=0.952f+0.040f*dread;
    float sum=0.0f, pressureMean=0.0f, motionMean=0.0f, vortexMean=0.0f;
    constexpr std::array<float,Cells> ratios{0.43f,0.69f,1.17f,1.79f};
    for(int i=0;i<Cells;++i){
        auto& c=s.cells[(size_t)i];
        const auto& prev=s.cells[(size_t)((i+Cells-1)%Cells)];
        const auto& next=s.cells[(size_t)((i+1)%Cells)];
        const float gradient=prev.pressure-next.pressure;
        const float shear=c.flow-0.5f*(prev.flow+next.flow);
        const float chaotic=std::tanh((c.flow*2.2f+c.vortex*2.5f+gradient*1.5f+shear*1.1f)*(0.48f+2.65f*turbulence2));
        const float forcing=hiss
                          + chaotic*(0.0015f+0.026f*turbulence2)
                          + s.cavity*(0.001f+0.014f*dread)
                          + gradient*(0.002f+0.008f*turbulence);
        c.flow=std::tanh(c.flow*memory + forcing - c.pressure*(0.010f+0.025f*(1.0f-dread)));
        c.pressure=std::tanh(c.pressure*(0.967f+0.025f*dread)+c.flow*(0.016f+0.043f*drive));
        c.vortex=std::tanh(c.vortex*(0.88f+0.10f*dread)+(c.flow-prev.flow)*(0.10f+0.66f*turbulence2)+gradient*0.05f);
        const float shedding=1.0f+(0.22f*turbulence2*((i&1)?-1.0f:1.0f)) + 0.12f*c.vortex;
        const float shed=fundamentalHz*ratios[(size_t)i]*juce::jlimit(0.55f,1.55f,shedding);
        if(shed<sr*0.46){ c.phase+=shed/(float)sr; c.phase-=std::floor(c.phase); }
        const float swirl=std::sin(juce::MathConstants<float>::twoPi*c.phase + c.pressure*(0.7f+5.0f*dread));
        const float rasp=std::sin(juce::MathConstants<float>::twoPi*c.phase*2.0f+c.vortex*2.4f);
        sum += swirl*(0.08f+0.28f*std::abs(c.vortex)+0.08f*turbulence)
             + rasp*(0.015f+0.10f*turbulence2)
             + c.flow*0.055f;
        pressureMean += c.pressure;
        motionMean += std::abs(c.flow);
        vortexMean += std::abs(c.vortex);
    }
    pressureMean*=0.25f;
    motionMean*=0.25f;
    vortexMean*=0.25f;
    const float threshold=.64f-.31f*dread-.08f*turbulence2;
    const float collapseDrive=std::max(0.0f,std::abs(pressureMean)-threshold);
    s.collapse=0.982f*s.collapse+collapseDrive*(0.07f+0.34f*turbulence2);
    s.cavity=std::tanh(s.cavity*(0.969f+0.024f*dread)+pressureMean*(0.014f+0.060f*dread)-s.collapse*pressureMean*(0.12f+0.14f*dread));
    const float burst=noise(s.rng)*s.collapse*(0.015f+0.13f*dread+0.06f*turbulence2);
    const float implosion=std::tanh(-pressureMean*s.collapse*(0.20f+0.65f*dread));
    const float out=sum*0.38f+s.cavity*(0.18f+0.38f*dread)+burst+implosion;
    if(!std::isfinite(out)){ s=VoiceState{}; return 0.0f; }

    auto& bus=s.creatureState;
    bus.set(CreatureStateBus::Signal::Energy, juce::jlimit(0.0f,1.0f,std::abs(out)*0.70f+motionMean*0.30f));
    bus.set(CreatureStateBus::Signal::Pressure, std::abs(pressureMean));
    bus.set(CreatureStateBus::Signal::Motion, motionMean);
    bus.set(CreatureStateBus::Signal::Instability, juce::jlimit(0.0f,1.0f,vortexMean*0.50f+turbulence2*0.35f+s.collapse*0.60f));
    bus.set(CreatureStateBus::Signal::Event, juce::jlimit(0.0f,1.0f,collapseDrive*4.0f+s.collapse*0.55f));
    bus.set(CreatureStateBus::Signal::Field, juce::jlimit(0.0f,1.0f,std::abs(s.cavity)));

    return juce::jlimit(-1.0f,1.0f,std::tanh(out*1.65f));
}

} // namespace horrorcastle

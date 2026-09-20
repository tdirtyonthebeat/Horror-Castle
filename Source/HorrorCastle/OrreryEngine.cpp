#include "OrreryEngine.h"
#include <cmath>

namespace horrorcastle {
namespace {
float wrap01(float x) noexcept { x-=std::floor(x); return x<0.0f?x+1.0f:x; }
}

float OrreryEngine::renderSample(VoiceState& s,float fundamentalHz,float orbitDepth,float aether,float expression,float velocity,double sr) noexcept {
    if(sr<=1.0||fundamentalHz<=0.0f) return 0.0f;
    orbitDepth=juce::jlimit(0.0f,1.0f,orbitDepth); aether=juce::jlimit(0.0f,1.0f,aether); expression=juce::jlimit(0.0f,1.0f,expression); velocity=juce::jlimit(0.0f,1.0f,velocity);
    constexpr float T=juce::MathConstants<float>::twoPi;
    constexpr std::array<float,Bodies> ratios{1.0f,1.61803399f,2.41421356f,3.14159265f,4.23606798f};
    constexpr std::array<float,Bodies> orbitRates{0.031f,0.047f,0.071f,0.109f,0.167f};

    float rawField=0.0f;
    for(int i=0;i<Bodies;++i){
        auto& b=s.bodies[(size_t)i];
        b.orbitPhase=wrap01(b.orbitPhase+(orbitRates[(size_t)i]*(0.35f+1.65f*aether))/(float)sr);
        const float orbital=std::sin(T*b.orbitPhase+s.barycenter*(0.17f+0.09f*(float)i));
        const float neighbor=(i==0?s.bodies[Bodies-1].memory:s.bodies[(size_t)i-1].memory);
        const float perturb=(0.0015f+0.014f*orbitDepth)*orbital+(0.0008f+0.008f*aether)*neighbor;
        const float frequency=fundamentalHz*ratios[(size_t)i]*(1.0f+perturb);
        if(frequency<sr*0.46f){
            b.phase=wrap01(b.phase+frequency/(float)sr);
            const float tone=std::sin(T*b.phase+s.field*(0.06f+0.20f*aether));
            b.memory+= (0.0025f+0.012f*expression)*(tone-b.memory);
            rawField+=tone/(1.0f+0.28f*(float)i);
        }
    }

    rawField/=3.2f;
    s.field+= (0.0015f+0.009f*aether)*(rawField-s.field);
    s.barycenter=wrap01(s.barycenter+(0.007f+0.043f*orbitDepth)/(float)sr);
    const float gravityWarp=std::tanh((rawField+s.field*(0.25f+0.65f*orbitDepth))*(1.0f+0.85f*expression));
    const float output=gravityWarp*(0.45f+0.55f*velocity)+s.field*(0.10f+0.24f*aether);
    return juce::jlimit(-1.0f,1.0f,std::tanh(output*1.25f));
}

} // namespace horrorcastle

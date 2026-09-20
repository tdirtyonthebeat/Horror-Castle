#include "MarrowEngine.h"
#include <cmath>

namespace horrorcastle {
namespace {
float noise(uint32_t& s) noexcept { s^=s<<13; s^=s>>17; s^=s<<5; return (float)s/2147483648.0f-1.0f; }
float reson(MarrowEngine::ResonatorState& st,float in,float f,float decay,double sr) noexcept {
    if(f<=0.0f||f>=sr*0.46||decay<=0.0f) return 0.0f;
    const float r=std::exp(-1.0f/(decay*(float)sr));
    const float w=juce::MathConstants<float>::twoPi*f/(float)sr;
    float y=2.0f*r*std::cos(w)*st.y1-r*r*st.y2+in*(1.0f-r);
    if(!std::isfinite(y)) y=0.0f; st.y2=st.y1; st.y1=std::abs(y)<1e-15f?0.0f:y; return y;
}
}

float MarrowEngine::renderSample(VoiceState& s,float fundamentalHz,float viscosity,float dread,float expression,float velocity,double sr) noexcept {
    if(sr<=1.0||fundamentalHz<=0.0f) return 0.0f;
    viscosity=juce::jlimit(0.0f,1.0f,viscosity); dread=juce::jlimit(0.0f,1.0f,dread); expression=juce::jlimit(0.0f,1.0f,expression); velocity=juce::jlimit(0.0f,1.0f,velocity);
    s.bowPhase+=fundamentalHz*(0.37f+0.13f*dread)/(float)sr; s.bowPhase-=std::floor(s.bowPhase);
    const float bow=std::sin(juce::MathConstants<float>::twoPi*s.bowPhase);
    const float relative=bow-s.bowMemory;
    const float pressure=0.18f+0.82f*expression;
    const float friction=std::tanh(relative*(4.0f+14.0f*pressure)*(1.1f-0.45f*viscosity));
    s.bowMemory+= (0.004f+0.025f*(1.0f-viscosity))*(friction-s.bowMemory);
    const float scrape=noise(s.rng)*(0.004f+0.035f*pressure)*(0.3f+0.7f*velocity);
    const float drive=(friction*0.035f+scrape)+s.fluidFeedback*(0.015f+0.12f*dread);

    constexpr std::array<float,StringModes> ratios{1.0f,2.003f,3.01f,4.025f,5.061f};
    float stringSum=0.0f;
    for(int i=0;i<StringModes;++i){
        const float n=(float)(i+1); const float stretch=1.0f+0.012f*viscosity*n*n;
        const float decay=(0.08f+1.2f*(1.0f-viscosity)+1.5f*expression)/(1.0f+0.18f*(float)i);
        stringSum+=reson(s.strings[(size_t)i],drive/(std::pow(n,0.35f)),fundamentalHz*ratios[(size_t)i]*stretch,decay,sr);
    }

    constexpr std::array<float,FluidModes> fluidRatios{0.47f,0.83f,1.37f};
    float fluidSum=0.0f;
    for(int i=0;i<FluidModes;++i){
        const float f=fundamentalHz*fluidRatios[(size_t)i]*(0.82f+0.32f*dread);
        const float decay=(0.16f+1.8f*viscosity+1.0f*dread)/(1.0f+0.22f*(float)i);
        const float input=stringSum*(0.010f+0.095f*dread)/(float)(i+1)+drive*0.18f;
        fluidSum+=reson(s.fluid[(size_t)i],input,f,decay,sr);
    }
    s.stringFeedback=std::tanh(stringSum*5.5f); s.fluidFeedback=std::tanh(fluidSum*6.0f);
    const float body=stringSum*(7.0f+4.0f*expression)+fluidSum*(6.0f+5.0f*viscosity)+s.stringFeedback*s.fluidFeedback*(0.15f+0.35f*dread);
    return juce::jlimit(-1.0f,1.0f,std::tanh(body*1.25f));
}

} // namespace horrorcastle

#include "CastleEngine.h"
#include <algorithm>
#include <cmath>

namespace horrorcastle {
namespace {
static float hz(float n){return 440.f*std::pow(2.f,(n-69.f)/12.f);}
}

void CastleEngine::scene(Voice&v,const SceneArchitecture&s,float&l,float&r,float mod,const std::array<float, CurseMatrix::Destinations>& hx,bool isCrypt){
auto&g=s.voice.generators; const float character=juce::jlimit(0.f,1.f,s.character + (isCrypt?hx[11]:hx[12])*.48f);
float& wander=isCrypt?v.cryptWander:v.towerWander;
wander=std::fmod(wander+(isCrypt?(.045f+.025f*character):(.11f+.035f*character))/(float)sr,1.f);
const float fixedCents=isCrypt?v.cryptDetune:v.towerDetune;
const float movingCents=std::sin(juce::MathConstants<float>::twoPi*wander)*(isCrypt?(2.0f+8.5f*character):(.08f+.42f*character));
const float driftCents=(fixedCents*character)+movingCents;
float f=hz(v.pitchNote + pitchBendSemitones + hx[9]*12.f)*std::pow(2.f,(driftCents + (globalUnison-1)*fixedCents*.08f)/1200.f);
if(isCrypt){
    v.cryptSubPhase=std::fmod(v.cryptSubPhase+(f*.5f)/(float)sr,1.f);
    v.cryptAbyssPhase=std::fmod(v.cryptAbyssPhase+(f*.25f)/(float)sr,1.f);
}else{
    const float bellA=f*2.41421356f;
    const float bellB=f*3.73205081f;
    if(bellA<sr*.46f)v.towerBellPhaseA=std::fmod(v.towerBellPhaseA+bellA/(float)sr,1.f);
    if(bellB<sr*.46f)v.towerBellPhaseB=std::fmod(v.towerBellPhaseB+bellB/(float)sr,1.f);
}
const float shapeMod=isCrypt?hx[3]:hx[4]; const float fmDepthMod=hx[5]*.75f;
auto modShape=[&](const GeneratorSlot& gen){float sh=gen.shape+shapeMod;if(gen.type==GeneratorType::FM||gen.type==GeneratorType::PM||(!isCrypt&&gen.type==GeneratorType::ChamberIII))sh+=fmDepthMod;return juce::jlimit(0.f,1.f,sh);};
float shapeA=modShape(g[0]),shapeB=modShape(g[1]),shapeC=modShape(g[2]); float sceneCut=isCrypt?hx[1]:hx[2];float sceneDrive=hx[6];
float da=f/sr,db=f*std::pow(2.f,g[1].tune/12.f)/sr,dc=f*std::pow(2.f,g[2].tune/12.f)/sr;v.pa=std::fmod(v.pa+da,1.f);v.pb=std::fmod(v.pb+db,1.f);v.pc=std::fmod(v.pc+dc,1.f);
const float T=juce::MathConstants<float>::twoPi;
auto advanceAux=[&](float& phase,float frequency,float phaseOffset=0.f){
    if(frequency<=0.f||frequency>=sr*.46f)return 0.f;
    phase=std::fmod(phase+frequency/(float)sr,1.f);
    return std::sin(T*phase+phaseOffset);
};
auto signatureOsc=[&](int slot,const GeneratorSlot& gen,float phase,float sh,float freq){
    // Indices 0..7 remain shared. Indices 8..11 keep their historical chamber
    // meanings. Chamber V is appended at index 12 for WRAITH / RELIQUARY.
    if(gen.type>=GeneratorType::ChamberI){
        float& aux1=isCrypt?v.cryptAux1[(size_t)slot]:v.towerAux1[(size_t)slot];
        float& aux2=isCrypt?v.cryptAux2[(size_t)slot]:v.towerAux2[(size_t)slot];
        const float base=std::sin(T*phase);
        const float pressure=juce::jlimit(0.f,1.f,std::max(channelPressure,v.polyPressure));
        const float expression=juce::jlimit(0.f,1.f,v.velocity*.22f+modWheel*.43f+pressure*.35f);

        if(isCrypt){
            switch(gen.type){
                case GeneratorType::ChamberI: {
                    const float sub=advanceAux(aux1,freq*.5f);
                    const float abyss=advanceAux(aux2,freq*.25f,.23f);
                    const float wound=(freq*3.f<sr*.46f)?std::sin(T*phase*3.f+sh*1.9f):0.f;
                    return std::tanh((base*.18f+sub*.68f+abyss*.36f+wound*.10f)*(1.20f+character*1.55f));
                }
                case GeneratorType::ChamberII: {
                    const float position=juce::jlimit(0.f,1.f,corpsePosition*.46f+sh*.54f);
                    const float rot=juce::jlimit(0.f,1.f,corpseRot+character*.36f);
                    return spectralCorpse.renderSample(v.cryptCorpse[(size_t)slot], freq, position, rot, corpseFormant, corpseInharmonic, sr);
                }
                case GeneratorType::ChamberIII:
                    return boneResonator.renderSample(v.cryptBone[(size_t)slot],freq,sh,character,expression,v.velocity,sr);
                case GeneratorType::ChamberIV: {
                    const float slow=advanceAux(aux1,freq*.75f);
                    const float fast=advanceAux(aux2,freq*1.25f,.5f);
                    const float ring=base*slow;
                    const float turn=std::sin(T*phase+fast*(1.0f+sh*4.5f));
                    return std::tanh((ring*(.75f-.30f*sh)+turn*(.28f+.58f*sh))*(1.35f+character));
                }
                case GeneratorType::ChamberV:
                    return wraith.renderSample(v.cryptWraith[(size_t)slot],freq,sh,character,expression,v.velocity,sr);
                default: break;
            }
        }else{
            switch(gen.type){
                case GeneratorType::ChamberI: {
                    const float bellA=advanceAux(aux1,freq*2.41421356f,.11f);
                    const float bellB=advanceAux(aux2,freq*3.73205081f,.53f);
                    return std::tanh(base*.24f+bellA*(.48f+.18f*sh)+bellB*(.31f+.22f*character));
                }
                case GeneratorType::ChamberII: {
                    const float spireA=advanceAux(aux1,freq*5.071f,.29f);
                    const float spireB=advanceAux(aux2,freq*9.173f,1.07f);
                    const float crown=(freq*13.127f<sr*.46f)?std::sin(T*phase*13.127f+sh*2.2f):0.f;
                    const float shoulder=(freq*3.019f<sr*.46f)?std::sin(T*phase*3.019f+.41f):0.f;
                    return std::tanh(base*.045f+shoulder*.13f+spireA*(.52f+.22f*sh)+spireB*(.34f+.12f*character)+crown*(.22f+.18f*character));
                }
                case GeneratorType::ChamberIII: {
                    const float astral=advanceAux(aux1,freq*1.61803399f,.37f);
                    const float orbit=advanceAux(aux2,freq*.70710678f,.83f);
                    const float index=1.25f+sh*8.75f;
                    return std::sin(T*phase+astral*index+orbit*(.35f+1.65f*character));
                }
                case GeneratorType::ChamberIV: {
                    const float prismA=advanceAux(aux1,freq*2.071f,.21f);
                    const float prismB=advanceAux(aux2,freq*5.173f,.93f);
                    const float refraction=base*(.36f-.18f*sh)+prismA*(.34f+.28f*sh)+prismB*(.18f+.24f*character);
                    return std::sin(refraction*juce::MathConstants<float>::pi*(1.15f+sh*1.85f));
                }
                case GeneratorType::ChamberV:
                    return reliquary.renderSample(v.towerReliquary[(size_t)slot],freq,sh,character,expression,v.velocity,sr);
                default: break;
            }
        }
    }

    if(gen.type==GeneratorType::FM){
        auto& fmState=isCrypt?v.cryptRitualFM[(size_t)slot]:v.towerRitualFM[(size_t)slot];
        const float pressure=juce::jlimit(0.f,1.f,std::max(channelPressure,v.polyPressure));
        const float expression=juce::jlimit(0.f,1.f,v.velocity*.22f+modWheel*.43f+pressure*.35f);
        return ritualFM.renderSample(fmState,freq,sh,character,expression,isCrypt,sr);
    }

    float y=osc(gen.type,phase,sh,freq);
    if(isCrypt){
        const float scar=(freq*3.f<sr*.46f)?std::sin(T*phase*3.f+sh*1.7f)*(.04f+.18f*character):0.f;
        const float asym=y*std::abs(y)*(.10f+.32f*character);
        return std::tanh((y+scar-asym)*(1.f+1.15f*character));
    }
    float glass=0.f;
    if(freq*2.f<sr*.46f)glass+=.12f*std::sin(T*phase*2.f+sh*.8f);
    if(freq*5.f<sr*.46f)glass+=.05f*std::sin(T*phase*5.f+sh*2.1f);
    return std::tanh(y*.82f+glass*(.22f+.38f*character));
};
const float fA=f;
const float fB=f*std::pow(2.f,g[1].tune/12.f);
const float fC=f*std::pow(2.f,g[2].tune/12.f);
float x=0;
if(g[0].enabled)x+=signatureOsc(0,g[0],v.pa,shapeA,fA)*g[0].level;
if(g[1].enabled)x+=signatureOsc(1,g[1],v.pb,shapeB,fB)*g[1].level;
if(g[2].enabled)x+=signatureOsc(2,g[2],v.pc,shapeC,fC)*g[2].level;
x*=.42f;
if(s.voice.noise.enabled)x+=rnd()*s.voice.noise.level*.2f;
FilterCell fa=s.voice.filters[0],fb=s.voice.filters[1];fa.cutoff=juce::jlimit(.002f,.48f,fa.cutoff+sceneCut*.20f);fb.cutoff=juce::jlimit(.002f,.48f,fb.cutoff+sceneCut*.16f);fa.drive=juce::jlimit(0.f,1.f,fa.drive+sceneDrive);fb.drive=juce::jlimit(0.f,1.f,fb.drive+sceneDrive);
float& za=isCrypt?v.cfa:v.tfa;float& zb=isCrypt?v.cfb:v.tfb;float a=filter(x,za,fa,mod),b=filter(x,zb,fb,mod*.7f);
if(s.voice.filterRoute==Route::Parallel)x=.5f*(a+b);else if(s.voice.filterRoute==Route::Crossfeed)x=.65f*a+.35f*b;else if(s.voice.filterRoute==Route::Split)x=.78f*a+.22f*b;else x=filter(a,zb,fb,mod*.7f);
float stereoSide=0.f;
if(isCrypt){
    const float sub=std::sin(T*v.cryptSubPhase);
    const float abyss=std::sin(T*v.cryptAbyssPhase);
    const float underbody=sub*(.06f+.30f*character)+abyss*(.015f+.13f*character);
    const float cutoff=7200.f-5700.f*character;
    const float alpha=1.f-std::exp(-T*cutoff/(float)sr);
    v.cryptBody+=alpha*(x-v.cryptBody);
    const float body=.38f*x+.62f*v.cryptBody;
    x=std::tanh((body+underbody)*(1.f+.95f*character));
}else{
    const float bellA=(f*2.41421356f<sr*.46f)?std::sin(T*v.towerBellPhaseA):0.f;
    const float bellB=(f*3.73205081f<sr*.46f)?std::sin(T*v.towerBellPhaseB+.37f):0.f;
    const float celestial=bellA*(.08f+.30f*character)+bellB*(.03f+.17f*character);
    const float alpha=1.f-std::exp(-T*2500.f/(float)sr);
    v.towerBody+=alpha*(x-v.towerBody);
    const float air=x-v.towerBody;
    x=std::tanh(x*(.78f-.20f*character)+air*(.14f+.48f*character)+celestial);
    stereoSide=(bellA-bellB)*(.015f+.11f*character);
}
const float chamberEnvelope=isCrypt?std::pow(juce::jlimit(0.f,1.f,v.amp.value),.78f):v.amp.value*(1.f+.22f*character*v.iron.value);
x*=chamberEnvelope*v.velocity*s.voice.master;
float levelSum=0.f,weightedPan=0.f,spread=0.f;for(const auto& gen:g){if(gen.enabled){levelSum+=gen.level;weightedPan+=gen.pan*gen.level;spread+=gen.spread*gen.level;}}if(levelSum>1.0e-5f){weightedPan/=levelSum;spread/=levelSum;}
float pan=juce::jlimit(-1.f,1.f,weightedPan+s.sceneBalance);
const float unisonWidth=(globalUnison-1)/7.f;stereoSide+=x*spread*(.06f+.18f*unisonWidth)*std::sin(T*wander+.7f);
const float left=x*(.5f-.5f*pan)+stereoSide*(.5f+.25f*character);
const float right=x*(.5f+.5f*pan)-stereoSide*(.5f+.25f*character);
l+=left;r+=right;}

} // namespace horrorcastle

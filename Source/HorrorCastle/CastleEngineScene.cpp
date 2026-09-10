#include "CastleEngine.h"
#include "SynthesisFamilyContract.h"
#include <algorithm>
#include <cmath>

namespace horrorcastle {
namespace { static float hz(float n){return 440.f*std::pow(2.f,(n-69.f)/12.f);} }

void CastleEngine::scene(Voice&v,const SceneArchitecture&s,float&l,float&r,float mod,const std::array<float, CurseMatrix::Destinations>& hx,bool isCrypt){
auto&g=s.voice.generators; const float character=juce::jlimit(0.f,1.f,s.character + (isCrypt?hx[11]:hx[12])*.48f);
float& wander=isCrypt?v.cryptWander:v.towerWander; wander=std::fmod(wander+(isCrypt?(.045f+.025f*character):(.11f+.035f*character))/(float)sr,1.f);
const float fixedCents=isCrypt?v.cryptDetune:v.towerDetune; const float movingCents=std::sin(juce::MathConstants<float>::twoPi*wander)*(isCrypt?(2.0f+8.5f*character):(.08f+.42f*character)); const float driftCents=(fixedCents*character)+movingCents;
float f=hz(v.pitchNote + pitchBendSemitones + hx[9]*12.f)*std::pow(2.f,(driftCents + (globalUnison-1)*fixedCents*.08f)/1200.f);
if(isCrypt){
    // Legacy room phases are retained in voice state for preset compatibility,
    // but Performance room coloration no longer advances or injects pitched
    // sub/bell oscillators. Pitched resonances belong to explicit creatures only.
}
const float shapeMod=isCrypt?hx[3]:hx[4],fmDepthMod=hx[5]*.75f;
auto modShape=[&](const GeneratorSlot& gen){float sh=gen.shape+shapeMod;if(gen.type==GeneratorType::FM||gen.type==GeneratorType::PM||(!isCrypt&&gen.type==GeneratorType::ChamberIII))sh+=fmDepthMod;return juce::jlimit(0.f,1.f,sh);};
float shapeA=modShape(g[0]),shapeB=modShape(g[1]),shapeC=modShape(g[2]); float sceneCut=isCrypt?hx[1]:hx[2],sceneDrive=hx[6];
float da=f/sr,db=f*std::pow(2.f,g[1].tune/12.f)/sr,dc=f*std::pow(2.f,g[2].tune/12.f)/sr;v.pa=std::fmod(v.pa+da,1.f);v.pb=std::fmod(v.pb+db,1.f);v.pc=std::fmod(v.pc+dc,1.f);
const float T=juce::MathConstants<float>::twoPi;
auto advanceAux=[&](float& phase,float frequency,float phaseOffset=0.f){ if(frequency<=0.f||frequency>=sr*.46f)return 0.f; phase=std::fmod(phase+frequency/(float)sr,1.f); return std::sin(T*phase+phaseOffset); };
auto signatureOsc=[&](int slot,const GeneratorSlot& gen,float phase,float sh,float freq){
    if(gen.type>=GeneratorType::ChamberI){
        float& aux1=isCrypt?v.cryptAux1[(size_t)slot]:v.towerAux1[(size_t)slot]; float& aux2=isCrypt?v.cryptAux2[(size_t)slot]:v.towerAux2[(size_t)slot]; const float base=std::sin(T*phase);
        const float pressure=juce::jlimit(0.f,1.f,std::max(channelPressure,v.polyPressure)); const float expression=juce::jlimit(0.f,1.f,v.velocity*.22f+modWheel*.43f+pressure*.35f);
        auto mixMacro=[](float local,float global,float localWeight){return juce::jlimit(0.f,1.f,local*localWeight+global*(1.f-localWeight));};
        auto expressive=[&](float macro){return juce::jlimit(0.f,1.f,expression*(.58f+.70f*macro)+pressure*.12f*macro);};
        if(isCrypt){
            switch(gen.type){
                case GeneratorType::ChamberI:{const float sub=advanceAux(aux1,freq*.5f),abyssTone=advanceAux(aux2,freq*.25f,.23f),wound=(freq*3.f<sr*.46f)?std::sin(T*phase*3.f+sh*1.9f):0.f;return std::tanh((base*.18f+sub*.68f+abyssTone*.36f+wound*.10f)*(1.20f+character*1.55f));}
                case GeneratorType::ChamberII:{const float position=juce::jlimit(0.f,1.f,corpsePosition*.46f+sh*.54f),rot=juce::jlimit(0.f,1.f,corpseRot+character*.36f);return spectralCorpse.renderSample(v.cryptCorpse[(size_t)slot],freq,position,rot,corpseFormant,corpseInharmonic,sr);}
                case GeneratorType::ChamberIII:return boneResonator.renderSample(v.cryptBone[(size_t)slot],freq,sh,character,expression,v.velocity,sr);
                case GeneratorType::ChamberIV:{const float slow=advanceAux(aux1,freq*.75f),fast=advanceAux(aux2,freq*1.25f,.5f),ring=base*slow,turn=std::sin(T*phase+fast*(1.0f+sh*4.5f));return std::tanh((ring*(.75f-.30f*sh)+turn*(.28f+.58f*sh))*(1.35f+character));}
                case GeneratorType::ChamberV:return wraith.renderSample(v.cryptWraith[(size_t)slot],freq,sh,character,expression,v.velocity,sr);
                case GeneratorType::ChamberVI:return coffin.renderSample(v.cryptCoffin[(size_t)slot],freq,sh,character,expression,v.velocity,sr);
                case GeneratorType::ChamberVII:return marrow.renderSample(v.cryptMarrow[(size_t)slot],freq,sh,character,expression,v.velocity,sr);
                case GeneratorType::ChamberVIII:{
                    const float depth=mixMacro(sh,living.abyssDepth,.52f);
                    const float dread=mixMacro(character,living.abyssDread,.38f);
                    return abyss.renderSample(v.cryptAbyss[(size_t)slot],freq,depth,dread,expressive(living.abyssPressure),v.velocity,sr);
                }
                case GeneratorType::ChamberIX:{
                    auto& st=v.cryptPoltergeist[(size_t)slot];
                    const float charge=mixMacro(sh,living.poltergeistCharge,.48f);
                    const float instability=mixMacro(character,living.poltergeistInstability,.34f);
                    const float y=poltergeist.renderSample(st,freq,charge,instability,expressive(living.poltergeistArc),v.velocity,sr);
                    v.ecologySnapshot[PoltergeistCreature]=PoltergeistEngine::stateBus(st);return y;
                }
                case GeneratorType::ChamberX:{
                    auto& st=v.cryptVortex[(size_t)slot];
                    const float turbulence=mixMacro(sh,living.vortexTurbulence,.48f);
                    const float collapse=mixMacro(character,living.vortexCollapse,.32f);
                    const float y=vortex.renderSample(st,freq,turbulence,collapse,expressive(living.vortexPressure),v.velocity,sr);
                    v.ecologySnapshot[VortexCreature]=VortexEngine::stateBus(st);return y;
                }
                default:break;
            }
        }else{
            switch(gen.type){
                case GeneratorType::ChamberI:{const float bellA=advanceAux(aux1,freq*2.41421356f,.11f),bellB=advanceAux(aux2,freq*3.73205081f,.53f);return std::tanh(base*.24f+bellA*(.48f+.18f*sh)+bellB*(.31f+.22f*character));}
                case GeneratorType::ChamberII:{const float spireA=advanceAux(aux1,freq*5.071f,.29f),spireB=advanceAux(aux2,freq*9.173f,1.07f),crown=(freq*13.127f<sr*.46f)?std::sin(T*phase*13.127f+sh*2.2f):0.f,shoulder=(freq*3.019f<sr*.46f)?std::sin(T*phase*3.019f+.41f):0.f;return std::tanh(base*.045f+shoulder*.13f+spireA*(.52f+.22f*sh)+spireB*(.34f+.12f*character)+crown*(.22f+.18f*character));}
                case GeneratorType::ChamberIII:{const float astral=advanceAux(aux1,freq*1.61803399f,.37f),orbit=advanceAux(aux2,freq*.70710678f,.83f),index=1.25f+sh*8.75f;return std::sin(T*phase+astral*index+orbit*(.35f+1.65f*character));}
                case GeneratorType::ChamberIV:{
                    // PRISM is the Castle's oscillator-granular creature. Three
                    // continuously re-seeded micro-grains overlap with different
                    // pitch ratios; MORPH changes grain density/window sharpness.
                    const float grainA=advanceAux(aux1,freq*(1.31f+2.20f*sh),.21f);
                    const float grainB=advanceAux(aux2,freq*(2.07f+5.10f*sh),.93f);
                    const float wA=std::pow(.5f+.5f*std::sin(T*aux1),1.0f+7.0f*sh);
                    const float wB=std::pow(.5f+.5f*std::sin(T*aux2+2.1f),1.0f+9.0f*sh);
                    const float wC=std::pow(.5f+.5f*std::sin(T*phase+4.2f),1.0f+5.0f*sh);
                    const float cloud=grainA*wA+grainB*wB+base*wC;
                    const float refraction=cloud*(.52f+.28f*sh)+std::sin(T*phase*3.0f+.4f)*(.08f+.16f*character);
                    return std::tanh(refraction*(1.05f+1.65f*sh));
                }
                case GeneratorType::ChamberV:return reliquary.renderSample(v.towerReliquary[(size_t)slot],freq,sh,character,expression,v.velocity,sr);
                case GeneratorType::ChamberVI:return choir.renderSample(v.towerChoir[(size_t)slot],freq,sh,character,expression,v.velocity,sr);
                case GeneratorType::ChamberVII:return orrery.renderSample(v.towerOrrery[(size_t)slot],freq,sh,character,expression,v.velocity,sr);
                case GeneratorType::ChamberVIII:{
                    const float reflection=mixMacro(sh,living.mirrorReflection,.50f);
                    const float fracture=mixMacro(character,living.mirrorFracture,.34f);
                    return mirror.renderSample(v.towerMirror[(size_t)slot],freq,reflection,fracture,expressive(living.mirrorSmear),v.velocity,sr);
                }
                case GeneratorType::ChamberIX:{
                    auto& st=v.towerAurora[(size_t)slot];
                    const float ext=v.ecologyInbox[AuroraCreature].get(CreatureStateBus::Signal::Field);
                    const float field=mixMacro(sh,living.auroraField,.46f);
                    const float instability=mixMacro(character,living.auroraInstability,.34f);
                    const float y=aurora.renderSample(st,freq,field,instability,expressive(living.auroraRadiance),v.velocity,sr,ext,ecologyEnabled?ecologyDepth:0.f);
                    v.ecologySnapshot[AuroraCreature]=AuroraEngine::stateBus(st);return y;
                }
                case GeneratorType::ChamberX:{
                    auto& st=v.towerSiren[(size_t)slot];
                    const float ext=v.ecologyInbox[SirenCreature].get(CreatureStateBus::Signal::Pressure);
                    const float aperture=mixMacro(sh,living.sirenAperture,.48f);
                    const float edge=mixMacro(character,living.sirenEdge,.30f);
                    const float y=siren.renderSample(st,freq,aperture,edge,expressive(living.sirenBreath),v.velocity,sr,ext,ecologyEnabled?ecologyDepth:0.f);
                    v.ecologySnapshot[SirenCreature]=SirenEngine::stateBus(st);return y;
                }
                default:break;
            }
        }
    }
    if(gen.type==GeneratorType::FM){auto& fmState=isCrypt?v.cryptRitualFM[(size_t)slot]:v.towerRitualFM[(size_t)slot];const float pressure=juce::jlimit(0.f,1.f,std::max(channelPressure,v.polyPressure));const float expression=juce::jlimit(0.f,1.f,v.velocity*.22f+modWheel*.43f+pressure*.35f);return ritualFM.renderSample(fmState,freq,sh,character,expression,isCrypt,sr);}
    float y=osc(gen.type,phase,sh,freq); if(isCrypt){const float scar=(freq*3.f<sr*.46f)?std::sin(T*phase*3.f+sh*1.7f)*(.04f+.18f*character):0.f,asym=y*std::abs(y)*(.10f+.32f*character);return std::tanh((y+scar-asym)*(1.f+1.15f*character));}
    float glass=0.f;if(freq*2.f<sr*.46f)glass+=.12f*std::sin(T*phase*2.f+sh*.8f);if(freq*5.f<sr*.46f)glass+=.05f*std::sin(T*phase*5.f+sh*2.1f);return std::tanh(y*.82f+glass*(.22f+.38f*character));
};
// Engine-aware articulation is intentionally generator-local.  The Castle's
// species must not all inherit the same temporal fingerprint just because they
// share a MIDI gate.  IRON is the transient envelope; AMP is the sustained gate.
auto articulationFor=[&](GeneratorType type,float sh){
    const auto& contract=synthesis_contract::get(type,isCrypt);
    const float e=juce::jlimit(0.f,1.f,v.amp.value);
    const float hit=juce::jlimit(0.f,1.f,v.iron.value);
    const float body=std::pow(e,contract.articulationPower);
    const float transient=contract.transientBoost*hit*(1.f-.35f*e);
    const float morphAccent=.90f+.18f*std::pow(juce::jlimit(0.f,1.f,sh),contract.morphCurve);
    return body*juce::jlimit(.22f,1.60f,(contract.sustainBias+transient)*morphAccent);
};
// CREATURE CONTRACTS: TYPE is the monster; MORPH is its transformation arc.
// Each contract owns a different spectral/nonlinear motion.  These are deliberately
// parameter-free so choosing a generator is itself the large audible decision.
auto creatureContract=[&](GeneratorType type,float y,float phase,float sh,float freq){
    const float hit=juce::jlimit(0.f,1.f,v.iron.value);
    const float slow=std::sin(T*wander);
    switch(type){
        case GeneratorType::VA:        return std::tanh(y*(1.05f+1.25f*sh)-y*std::abs(y)*(.08f+.34f*sh));                 // WEREWOLF: growl/transform
        case GeneratorType::Wavetable: return std::tanh(y*(.82f+.38f*sh)+(freq*3.f<sr*.46f?std::sin(T*phase*3.f)*.18f*sh:0.f)); // VAMPIRE: elegant harmonic bite
        case GeneratorType::FM:        return std::tanh(y*(.78f+1.55f*hit)+slow*.07f*sh);                                // SORCERER: metallic spell strike
        case GeneratorType::PM:        return std::sin(y*(1.35f+2.4f*sh))* (.78f+.18f*hit);                             // WITCH: warped phase magic
        case GeneratorType::Vector:    return std::tanh(y*(.88f+.45f*sh)+slow*.12f*(1.f-sh));                           // SHAPESHIFTER: continuous mutation
        case GeneratorType::Chip:      return juce::jlimit(-1.f,1.f,y*(.70f+.48f*hit));                                // GREMLIN: brittle digital snap
        case GeneratorType::Noise:     return std::tanh(y*(.72f+1.45f*sh))* (.62f+.32f*hit);                            // GHOUL: breath/grit
        case GeneratorType::Resonator: return std::tanh(y*(1.05f+.70f*hit))*(.82f+.12f*slow);                           // SKELETON: struck/rattling
        default: break;
    }
    if(isCrypt){
        switch(type){
            case GeneratorType::ChamberI:    return std::tanh(y*(1.35f+.75f*sh));                                      // UNDERCRYPT: buried giant
            case GeneratorType::ChamberII:   return y*(.82f+.16f*slow);                                                // CORPSE: spectral dead body
            case GeneratorType::ChamberIII:  return std::tanh(y*(1.15f+1.15f*hit));                                    // BONE: hard skeletal strike
            case GeneratorType::ChamberIV:   return std::tanh(y+slow*.18f*y*y);                                        // ROTATOR: mechanical creature
            case GeneratorType::ChamberV:    return y*(.70f+.28f*(1.f-hit))+.035f*slow;                                // WRAITH: ghost/breath
            case GeneratorType::ChamberVI:   return std::tanh(y*(1.55f+.65f*hit));                                     // COFFIN: blunt wooden body
            case GeneratorType::ChamberVII:  return std::tanh(y*(.95f+1.55f*hit)-.18f*y*y);                             // MARROW: wet exciter
            case GeneratorType::ChamberVIII: return std::tanh(y*(1.10f+.90f*sh))*(.88f+.08f*slow);                     // ABYSS: pressure monster
            case GeneratorType::ChamberIX:   return std::tanh(y*(.85f+1.85f*hit)+.05f*slow);                            // POLTERGEIST: electrical ghost
            case GeneratorType::ChamberX:    return std::tanh(y*(.92f+.65f*sh))*(.82f+.14f*slow);                       // VORTEX: rotating fluid beast
            default: break;
        }
    }else{
        switch(type){
            case GeneratorType::ChamberI:    return std::tanh(y*(1.0f+1.0f*hit));                                      // BELL GLASS: crystalline strike
            case GeneratorType::ChamberII:   return std::tanh(y*(.72f+1.0f*sh));                                       // SPIRE: razor spectral crown
            case GeneratorType::ChamberIII:  return std::sin(y*(1.2f+2.8f*sh));                                       // ASTRAL FM: arcane orbit
            case GeneratorType::ChamberIV:   return std::tanh(y*(.88f+.9f*sh)+.10f*slow);                              // PRISM: refracted creature
            case GeneratorType::ChamberV:    return y*(.84f+.13f*slow);                                                // RELIQUARY: ancient resonant vessel
            case GeneratorType::ChamberVI:   return std::tanh(y*(.76f+.42f*(1.f-hit)));                                // CHOIR: undead collective
            case GeneratorType::ChamberVII:  return std::tanh(y*(.92f+.58f*sh))*(.90f+.07f*slow);                       // ORRERY: clockwork occultist
            case GeneratorType::ChamberVIII: return std::sin(y*(1.0f+1.9f*sh))*(.86f+.10f*hit);                         // MIRROR: fractured apparition
            case GeneratorType::ChamberIX:   return y*(.72f+.25f*(1.f-hit))+.025f*slow;                                // AURORA: luminous spirit
            case GeneratorType::ChamberX:    return std::tanh(y*(1.05f+1.35f*sh))*(.78f+.18f*hit);                      // SIREN: overblown predator
            default: break;
        }
    }
    return y;
};
float familyStereoSide=0.f;
auto renderSlot=[&](int slot,const GeneratorSlot& gen,float phase,float sh,float freq){
    if(!gen.enabled||gen.level<=0.f)return 0.f;
    const auto& contract=synthesis_contract::get(gen.type,isCrypt);
    const float familyMorph=std::pow(juce::jlimit(0.f,1.f,sh),contract.morphCurve);

    float y=signatureOsc(slot,gen,phase,familyMorph,freq);
    y=creatureContract(gen.type,y,phase,familyMorph,freq);

    // The contract owns clarity and nonlinearity, so family identity survives
    // shared Castle processing without adding another user control.
    float& memory=isCrypt?v.cryptCreatureMemory[(size_t)slot]:v.towerCreatureMemory[(size_t)slot];
    const float alpha=.08f+.18f*(1.f-familyMorph);
    memory+=alpha*(y-memory);
    const float edge=y-memory;
    y=std::tanh((y+edge*contract.clarity*(1.0f+1.35f*familyMorph))
                *(1.0f+.18f*contract.nonlinearDrive*familyMorph));

    // Creature-local DC cleanup keeps aggressive asymmetry/folding crisp without
    // shaving musical bass. The ~15 Hz pole is below the useful synthesis body.
    float& dc=isCrypt?v.cryptCreatureDC[(size_t)slot]:v.towerCreatureDC[(size_t)slot];
    dc+=.002f*(y-dc);
    y-=dc;

    // Stereo is also a declared family law. Keep it subtle here: the later
    // room-pan stage remains in charge of placement, while this creates motion
    // characteristic of the synthesis family itself.
    const float stereoPhase=T*(wander*(.37f+.63f*contract.stereoMotion)+phase*.17f);
    familyStereoSide+=y*std::sin(stereoPhase+slot*.91f)*contract.stereoMotion*.11f*gen.level;

    const float art=articulationFor(gen.type,familyMorph);
    const float rendered=y*art*gen.level;
    auto& peak=isCrypt?v.cryptCreatureBlockPeak[(size_t)slot]:v.towerCreatureBlockPeak[(size_t)slot];
    peak=std::max(peak,std::abs(rendered));
    return rendered;
};
const float fA=f,fB=f*std::pow(2.f,g[1].tune/12.f),fC=f*std::pow(2.f,g[2].tune/12.f); float x=0;
x+=renderSlot(0,g[0],v.pa,shapeA,fA);x+=renderSlot(1,g[1],v.pb,shapeB,fB);x+=renderSlot(2,g[2],v.pc,shapeC,fC);
x*=.42f;if(s.voice.noise.enabled)x+=rnd()*s.voice.noise.level*.2f*juce::jlimit(0.f,1.f,v.amp.value);

// Preserve a small amount of each summoned creature's native body through the
// shared Castle filters. This prevents downstream subtractive plumbing from
// collapsing radically different synthesis families toward the same timbre.
const float nativeBody=x;
float identityPreserve=0.f,identityWeight=0.f;
for(const auto& gen:g)if(gen.enabled&&gen.level>0.f){
    const auto& law=synthesis_contract::get(gen.type,isCrypt);
    identityPreserve+=(.035f+.28f*law.clarity+.05f*law.stereoMotion)*gen.level;
    identityWeight+=gen.level;
}
identityPreserve=identityWeight>1.0e-5f?juce::jlimit(.035f,.20f,identityPreserve/identityWeight):.05f;

FilterCell fa=s.voice.filters[0],fb=s.voice.filters[1];fa.cutoff=juce::jlimit(.002f,.48f,fa.cutoff+sceneCut*.20f);fb.cutoff=juce::jlimit(.002f,.48f,fb.cutoff+sceneCut*.16f);fa.drive=juce::jlimit(0.f,1.f,fa.drive+sceneDrive);fb.drive=juce::jlimit(0.f,1.f,fb.drive+sceneDrive);
float& za=isCrypt?v.cfa:v.tfa;float& zb=isCrypt?v.cfb:v.tfb;float a=filter(x,za,fa,mod),b=filter(x,zb,fb,mod*.7f);if(s.voice.filterRoute==Route::Parallel)x=.5f*(a+b);else if(s.voice.filterRoute==Route::Crossfeed)x=.65f*a+.35f*b;else if(s.voice.filterRoute==Route::Split)x=.78f*a+.22f*b;else x=filter(a,zb,fb,mod*.7f);
x=x*(1.f-identityPreserve)+nativeBody*identityPreserve;

// The room itself is part of the Castle, but it must not repaint every creature
// with the same brush. Preserve more of highly distinctive families around the
// CRYPT/TOWER body stage, while still keeping each room's overall character.
const float preRoomIdentity=x;
float roomPreserve=0.f,roomWeight=0.f;
for(const auto& gen:g)if(gen.enabled&&gen.level>0.f){
    const auto& law=synthesis_contract::get(gen.type,isCrypt);
    roomPreserve+=(.08f+.16f*law.clarity+.07f*law.nonlinearDrive+.05f*law.stereoMotion)*gen.level;
    roomWeight+=gen.level;
}
roomPreserve=roomWeight>1.0e-5f?juce::jlimit(.10f,.30f,roomPreserve/roomWeight):.12f;

float stereoSide=familyStereoSide;
if(isCrypt){
    // Room character must colour, never generate a pitched identity of its own.
    // The previous global sub/quarter-tone underbody made unrelated creatures
    // share an audible drone. Keep only a slow body response and saturation.
    const float cutoff=7200.f-5700.f*character;
    const float alpha=1.f-std::exp(-T*cutoff/(float)sr);
    v.cryptBody+=alpha*(x-v.cryptBody);
    const float body=.38f*x+.62f*v.cryptBody;
    x=std::tanh(body*(1.f+.72f*character));
}else{
    // Likewise, TOWER no longer injects fixed bell partials into every engine.
    // "Bell" now belongs only to Bell Glass (and other explicit modal creatures).
    const float alpha=1.f-std::exp(-T*2500.f/(float)sr);
    v.towerBody+=alpha*(x-v.towerBody);
    const float air=x-v.towerBody;
    x=std::tanh(x*(.86f-.14f*character)+air*(.10f+.34f*character));
}
x=x*(1.f-roomPreserve)+preRoomIdentity*roomPreserve;
// Generator-local articulation already owns the amplitude contour. Keep only
// velocity and room gain here so downstream processing cannot homogenize species.
x*=v.velocity*s.voice.master;
float levelSum=0.f,weightedPan=0.f,spread=0.f;for(const auto& gen:g){if(gen.enabled){levelSum+=gen.level;weightedPan+=gen.pan*gen.level;spread+=gen.spread*gen.level;}}if(levelSum>1.0e-5f){weightedPan/=levelSum;spread/=levelSum;}float pan=juce::jlimit(-1.f,1.f,weightedPan+s.sceneBalance);const float unisonWidth=(globalUnison-1)/7.f;stereoSide+=x*spread*(.06f+.18f*unisonWidth)*std::sin(T*wander+.7f);const float left=x*(.5f-.5f*pan)+stereoSide*(.5f+.25f*character),right=x*(.5f+.5f*pan)-stereoSide*(.5f+.25f*character);l+=left;r+=right;}

} // namespace horrorcastle

#include "CastleEngine.h"
#include <algorithm>

namespace horrorcastle {
static float hz(float n){return 440.f*std::pow(2.f,(n-69.f)/12.f);} static float clip(float x){return std::tanh(x);}
static float polyBlep(float phase, float dt){
    dt=juce::jlimit(1.0e-6f,.5f,dt);
    if(phase<dt){const float x=phase/dt;return x+x-x*x-1.f;}
    if(phase>1.f-dt){const float x=(phase-1.f)/dt;return x*x+x+x+1.f;}
    return 0.f;
}
static GeneratorType gt(int v){ return static_cast<GeneratorType>(juce::jlimit(0,11,v)); }
static FilterType ft(int v){ return static_cast<FilterType>(juce::jlimit(0,8,v)); }
static Route rt(int v){ return static_cast<Route>(juce::jlimit(0,3,v)); }

CastleEngine::CastleEngine() = default;
void CastleEngine::prepare(double s,int bs){
    sr=s; blockSize=bs;
    delay.setMaximumDelayInSamples((int)std::ceil(s*1.25));
    delay.prepare({s, static_cast<uint32_t>(juce::jmax(1,bs)), 2});
    grave.prepare(s);
    possession.prepare(s);
    rituals.prepare(s);
    ritual.prepare(s);
    reset();
}
void CastleEngine::reset(){
    for(auto&v:voices)v=Voice{};
    delay.reset(); grave.reset(); possession.reset(); rituals.reset(); hex.reset(); ritual.reset();
    for(auto& a : hexLaneTelemetry) a.store(0.f, std::memory_order_relaxed);
    for(auto& a : hexDestinationTelemetry) a.store(0.f, std::memory_order_relaxed);
    modWheel=0.f; channelPressure=0.f; pitchBendSemitones=0.f;
}
CastleEngine::Voice* CastleEngine::steal(){for(auto&v:voices)if(!v.active)return &v;return &*std::min_element(voices.begin(),voices.end(),[](auto&a,auto&b){return a.amp.value<b.amp.value;});}
void CastleEngine::on(int n,int vel){auto*v=steal();*v=Voice{};v->active=true;v->note=n;v->pitchNote=globalGlide>.001f?lastPlayedNote:(float)n;lastPlayedNote=(float)n;v->velocity=vel/127.f;
v->cryptDetune=rnd()*11.0f; v->towerDetune=rnd()*0.8f; v->cryptWander=(rnd()+1.f)*.5f; v->towerWander=(rnd()+1.f)*.5f;
v->amp.attack=.004f;v->amp.decay=.32f;v->amp.sustain=.76f;v->amp.release=.18f;v->iron.attack=.006f;v->iron.decay=.24f;v->iron.sustain=.15f;v->iron.release=.22f;v->amp.on();v->iron.on();}
void CastleEngine::off(int n){for(auto&v:voices)if(v.active&&v.note==n){v.releasing=true;v.amp.off();v.iron.off();}}
float CastleEngine::rnd(){rng^=rng<<13;rng^=rng>>17;rng^=rng<<5;return float(rng)/2147483648.f-1.f;}
float CastleEngine::osc(GeneratorType t,float p,float shape,float f){
const float T=juce::MathConstants<float>::twoPi; const float dt=juce::jlimit(1.0e-6f,.49f,f/(float)sr);
switch(t){
case GeneratorType::VA:{
    float saw=2.f*p-1.f-polyBlep(p,dt);
    const float width=juce::jlimit(.12f,.88f,.66f-.32f*shape);
    float pulse=p<width?1.f:-1.f; pulse+=polyBlep(p,dt);
    float edge=p-width; if(edge<0.f)edge+=1.f; pulse-=polyBlep(edge,dt);
    return saw*(1.f-shape)+pulse*shape;
}
case GeneratorType::Wavetable:return .72f*std::sin(T*p)+.28f*std::sin(T*2*p+shape*5.f);
case GeneratorType::FM:return std::sin(T*p+shape*3.f*std::sin(T*p*2.01f));
case GeneratorType::PM:return std::sin(T*p+shape*2.f*std::sin(T*p*3.f));
case GeneratorType::Vector:return (1.f-shape)*std::sin(T*p)+shape*std::sin(T*2.f*p);
case GeneratorType::Chip:{float q=p<.5f?1.f:-1.f;q+=polyBlep(p,dt);float e=p-.5f;if(e<0)e+=1.f;q-=polyBlep(e,dt);return std::round(q*7.f)/7.f;}
case GeneratorType::Noise:return rnd();
case GeneratorType::Resonator:{float y=std::sin(T*p);if(f*2.03f<sr*.46f)y+=.4f*std::sin(T*p*2.03f);if(f*3.97f<sr*.46f)y+=.2f*std::sin(T*p*3.97f);return y;}
default:return std::sin(T*p);
}}
float CastleEngine::filter(float x,float&z,const FilterCell&f,float mod){if(!f.enabled)return x;float c=juce::jlimit(.002f,.48f,f.cutoff+mod),q=juce::jlimit(.05f,1.f,f.resonance),a=std::exp(-juce::MathConstants<float>::twoPi*c),lp=(1-a)*x+a*z;z=lp;float y=lp;if(f.type==FilterType::HighPass)y=x-lp;else if(f.type==FilterType::BandPass)y=.5f*(x-lp);else if(f.type==FilterType::Notch)y=x-.7f*lp;else if(f.type==FilterType::Comb){float comb=x+.72f*z;z=x;y=.55f*comb;}else if(f.type==FilterType::Shaper)y=std::tanh(lp*(1+f.drive*8.f));else y=clip(lp*(1+f.drive*3.f+q*.35f));return y;}
void CastleEngine::setParameters(const juce::AudioProcessorValueTreeState& apvts){
    auto get=[&](const juce::String& id,float fallback){if(auto* p=apvts.getRawParameterValue(id))return p->load();return fallback;};
    auto getb=[&](const juce::String& id,bool fallback){return get(id,fallback?1.f:0.f)>.5f;};
    auto choice=[&](const juce::String& id,int maxIndex,int fallback){
        return juce::jlimit(0, maxIndex, (int)std::lround(get(id, (float)fallback)));
    };
    auto loadScene=[&](SceneArchitecture& s,const char* name){
        for(int i=0;i<3;++i){auto& g=s.voice.generators[i];g.type=gt(choice(param::id(name,i+1,"type"),11,(int)g.type));g.level=get(param::id(name,i+1,"level"),g.level);g.pan=get(param::id(name,i+1,"pan"),g.pan);g.tune=get(param::id(name,i+1,"tune"),g.tune);g.shape=get(param::id(name,i+1,"shape"),g.shape);g.spread=get(param::id(name,i+1,"spread"),g.spread);g.enabled=getb(param::id(name,i+1,"enabled"),g.enabled);}
        s.voice.noise.enabled=getb(param::noise(name,"enabled"),s.voice.noise.enabled);s.voice.noise.level=get(param::noise(name,"level"),s.voice.noise.level);
        for(int i=0;i<2;++i){auto& f=s.voice.filters[i];f.type=ft(choice(param::fid(name,i+1,"type"),8,(int)f.type));f.cutoff=get(param::fid(name,i+1,"cutoff"),f.cutoff);f.resonance=get(param::fid(name,i+1,"resonance"),f.resonance);f.drive=get(param::fid(name,i+1,"drive"),f.drive);f.enabled=getb(param::fid(name,i+1,"enabled"),f.enabled);}
        s.voice.filterRoute=rt(choice(param::route(name),3,(int)s.voice.filterRoute));s.voice.master=get(param::scene(name,"master"),s.voice.master);s.sceneBalance=get(param::scene(name,"balance"),s.sceneBalance);s.character=get(param::scene(name,"character"),s.character);s.crossSceneFM=getb(param::scene(name,"crossfm"),s.crossSceneFM);s.crossSceneRing=getb(param::scene(name,"crossring"),s.crossSceneRing);
    };
    loadScene(patch.crypt,"crypt");loadScene(patch.tower,"tower");
    hex.load(apvts);
    patch.ritualMix=get("ritual.mix",patch.ritualMix);
    patch.hexAmount=get("global.hex",patch.hexAmount);
    patch.graveDepth=get("grave.reverb",patch.graveDepth);
    master=get("grave.output",.78f);
    globalGlide=get("global.glide",0.f);
    globalUnison=juce::jlimit(1,8,(int)std::lround(get("global.unison",1.f)));
    corpsePosition=get("corpse.position",.34f);
    corpseRot=get("corpse.rot",.22f);
    corpseFormant=get("corpse.formant",0.f);
    corpseInharmonic=get("corpse.inharmonic",.08f);

    ritualParams.mode = static_cast<RitualMode>(choice("ritual.mode", 4, 0));
    ritualParams.mix = get("ritual.mix", 0.f);
    ritualParams.depth = get("ritual.depth", .35f);
    ritualParams.drive = get("ritual.drive", .12f);
    ritualParams.width = get("ritual.width", .55f);
    ritualParams.feedback = get("ritual.feedback", .28f);
    ritual.setParameters(ritualParams);

    possessionParams.bloodFeed = get("possession.bloodFeed", 0.f);
    possessionParams.aetherLeak = get("possession.aetherLeak", 0.f);
    possessionParams.soulExchange = get("possession.soulExchange", 0.f);
    possessionParams.haunt = get("possession.haunt", 0.f);
    possession.setParameters(possessionParams);

    ritualsParams.enabled = getb("rituals.enabled", false);
    ritualsParams.pattern = choice("rituals.pattern", 7, 0);
    ritualsParams.rate = choice("rituals.rate", 3, 2);
    ritualsParams.bpm = get("rituals.bpm", 120.f);
    ritualsParams.gate = get("rituals.gate", .62f);
    ritualsParams.probability = get("rituals.probability", 1.f);
    ritualsParams.swing = get("rituals.swing", 0.f);
    ritualsParams.octaves = juce::jlimit(1,4,(int)std::lround(get("rituals.octaves",1.f)));
    rituals.setParameters(ritualsParams);

    delayMix=get("grave.delay",.18f);delayFeedback=get("grave.feedback",.28f);delayTimeSamples=juce::jlimit(1.0f, (float) delay.getMaximumDelayInSamples(), (float) sr * (0.06f + 0.62f * get("grave.delay", 0.18f)));
    graveTone=get("grave.cutoff",.32f);
    const float graveTone01=juce::jlimit(0.f,1.f,(graveTone-.02f)/.46f);
    grave.setParameters(get("grave.reverb",.32f), graveTone01, get("ritual.width",.55f));
}
void CastleEngine::scene(Voice&v,const SceneArchitecture&s,float&l,float&r,float mod,const std::array<float, CurseMatrix::Destinations>& hx,bool isCrypt){
auto&g=s.voice.generators; const float character=juce::jlimit(0.f,1.f,s.character + (isCrypt?hx[11]:hx[12])*.48f);
float& wander=isCrypt?v.cryptWander:v.towerWander;
wander=std::fmod(wander+(isCrypt?(.045f+.025f*character):(.11f+.035f*character))/(float)sr,1.f);
const float fixedCents=isCrypt?v.cryptDetune:v.towerDetune;
const float movingCents=std::sin(juce::MathConstants<float>::twoPi*wander)*(isCrypt?(2.0f+8.5f*character):(.08f+.42f*character));
const float driftCents=(fixedCents*character)+movingCents;
float f=hz(v.pitchNote + pitchBendSemitones + hx[9]*12.f)*std::pow(2.f,(driftCents + (globalUnison-1)*fixedCents*.08f)/1200.f);
// Independent chamber identity clocks. These are not simple harmonics of the
// user-generator phase state, so their character remains audible across VA,
// wavetable, FM, Vector, Chip and Resonator source choices.
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
    // Indices 0..7 remain the compatible shared generator vocabulary.
    // Indices 8..11 are interpreted by the chamber, giving CRYPT and TOWER
    // genuinely exclusive synthesis families without breaking older patches.
    if(gen.type>=GeneratorType::ChamberI){
        float& aux1=isCrypt?v.cryptAux1[(size_t)slot]:v.towerAux1[(size_t)slot];
        float& aux2=isCrypt?v.cryptAux2[(size_t)slot]:v.towerAux2[(size_t)slot];
        const float base=std::sin(T*phase);

        if(isCrypt){
            switch(gen.type){
                case GeneratorType::ChamberI: { // UNDERCRYPT
                    const float sub=advanceAux(aux1,freq*.5f);
                    const float abyss=advanceAux(aux2,freq*.25f,.23f);
                    const float wound=(freq*3.f<sr*.46f)?std::sin(T*phase*3.f+sh*1.9f):0.f;
                    return std::tanh((base*.18f+sub*.68f+abyss*.36f+wound*.10f)
                                     *(1.20f+character*1.55f));
                }
                case GeneratorType::ChamberII: { // CORPSE — spectral frame resynthesis
                    const float position=juce::jlimit(0.f,1.f,corpsePosition*.46f+sh*.54f);
                    const float rot=juce::jlimit(0.f,1.f,corpseRot+character*.36f);
                    return spectralCorpse.renderSample(v.cryptCorpse[(size_t)slot], freq, position, rot,
                                                       corpseFormant, corpseInharmonic, sr);
                }
                case GeneratorType::ChamberIII: { // BONE RESONATOR
                    const float boneA=advanceAux(aux1,freq*2.702f,.17f);
                    const float boneB=advanceAux(aux2,freq*4.113f,.71f);
                    const float knock=std::sin(T*phase*(1.f+sh*.48f));
                    return std::tanh(base*.30f+knock*.24f+boneA*(.34f+.18f*sh)
                                     +boneB*(.20f+.18f*character));
                }
                case GeneratorType::ChamberIV: { // ROTATOR
                    const float slow=advanceAux(aux1,freq*.75f);
                    const float fast=advanceAux(aux2,freq*1.25f,.5f);
                    const float ring=base*slow;
                    const float turn=std::sin(T*phase+fast*(1.0f+sh*4.5f));
                    return std::tanh((ring*(.75f-.30f*sh)+turn*(.28f+.58f*sh))
                                     *(1.35f+character));
                }
                default: break;
            }
        }else{
            switch(gen.type){
                case GeneratorType::ChamberI: { // BELL GLASS
                    const float bellA=advanceAux(aux1,freq*2.41421356f,.11f);
                    const float bellB=advanceAux(aux2,freq*3.73205081f,.53f);
                    return std::tanh(base*.24f+bellA*(.48f+.18f*sh)+bellB*(.31f+.22f*character));
                }
                case GeneratorType::ChamberII: { // SPECTRAL SPIRE
                    // RC2: move the Spire decisively above CORPSE. The original
                    // 3x/4x/7x cluster was distinct but the nonlinear CORPSE path
                    // could still measure brighter. Spire now minimizes its
                    // fundamental and concentrates energy in sparse upper partials.
                    const float spireA=advanceAux(aux1,freq*5.071f,.29f);
                    const float spireB=advanceAux(aux2,freq*9.173f,1.07f);
                    const float crown=(freq*13.127f<sr*.46f)
                        ? std::sin(T*phase*13.127f+sh*2.2f) : 0.f;
                    const float shoulder=(freq*3.019f<sr*.46f)
                        ? std::sin(T*phase*3.019f+.41f) : 0.f;
                    return std::tanh(base*.045f
                                     +shoulder*.13f
                                     +spireA*(.52f+.22f*sh)
                                     +spireB*(.34f+.12f*character)
                                     +crown*(.22f+.18f*character));
                }
                case GeneratorType::ChamberIII: { // ASTRAL FM
                    const float astral=advanceAux(aux1,freq*1.61803399f,.37f);
                    const float orbit=advanceAux(aux2,freq*.70710678f,.83f);
                    const float index=1.25f+sh*8.75f;
                    return std::sin(T*phase+astral*index+orbit*(.35f+1.65f*character));
                }
                case GeneratorType::ChamberIV: { // PRISM
                    const float prismA=advanceAux(aux1,freq*2.071f,.21f);
                    const float prismB=advanceAux(aux2,freq*5.173f,.93f);
                    const float refraction=base*(.36f-.18f*sh)
                                          +prismA*(.34f+.28f*sh)
                                          +prismB*(.18f+.24f*character);
                    return std::sin(refraction*juce::MathConstants<float>::pi*(1.15f+sh*1.85f));
                }
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
        const float crush=std::tanh((y+scar-asym)*(1.f+1.15f*character));
        return crush;
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
    // DREAD grows a true low-frequency underbody: one octave and two octaves
    // below the played pitch. This is intentionally not just EQ or saturation.
    const float sub=std::sin(T*v.cryptSubPhase);
    const float abyss=std::sin(T*v.cryptAbyssPhase);
    const float underbody=sub*(.06f+.30f*character)+abyss*(.015f+.13f*character);

    // The chamber ceiling closes as DREAD rises. Keep CRYPT narrow and physical.
    const float cutoff=7200.f-5700.f*character;
    const float alpha=1.f-std::exp(-T*cutoff/(float)sr);
    v.cryptBody+=alpha*(x-v.cryptBody);
    const float body=.38f*x+.62f*v.cryptBody;
    x=std::tanh((body+underbody)*(1.f+.95f*character));
}else{
    // AETHER opens upward instead: independent irrational-ish ratios create a
    // bell/chime spectrum which cannot collapse into CRYPT's octave underbody.
    const float bellA=(f*2.41421356f<sr*.46f)?std::sin(T*v.towerBellPhaseA):0.f;
    const float bellB=(f*3.73205081f<sr*.46f)?std::sin(T*v.towerBellPhaseB+.37f):0.f;
    const float celestial=bellA*(.08f+.30f*character)+bellB*(.03f+.17f*character);

    const float alpha=1.f-std::exp(-T*2500.f/(float)sr);
    v.towerBody+=alpha*(x-v.towerBody);
    const float air=x-v.towerBody;
    // Reduce low body as AETHER rises and replace it with air + inharmonics.
    x=std::tanh(x*(.78f-.20f*character)+air*(.14f+.48f*character)+celestial);
    // Opposite-polarity chime energy creates a wide spectral tower while CRYPT
    // remains centered/monolithic. Width is intentionally tied to AETHER.
    stereoSide=(bellA-bellB)*(.015f+.11f*character);
}
const float chamberEnvelope=isCrypt
    ? std::pow(juce::jlimit(0.f,1.f,v.amp.value),.78f)
    : v.amp.value*(1.f+.22f*character*v.iron.value);
x*=chamberEnvelope*v.velocity*s.voice.master;
float levelSum=0.f,weightedPan=0.f,spread=0.f;for(const auto& gen:g){if(gen.enabled){levelSum+=gen.level;weightedPan+=gen.pan*gen.level;spread+=gen.spread*gen.level;}}if(levelSum>1.0e-5f){weightedPan/=levelSum;spread/=levelSum;}
float pan=juce::jlimit(-1.f,1.f,weightedPan+s.sceneBalance);
const float unisonWidth=(globalUnison-1)/7.f;stereoSide+=x*spread*(.06f+.18f*unisonWidth)*std::sin(T*wander+.7f);
const float left=x*(.5f-.5f*pan)+stereoSide*(.5f+.25f*character);
const float right=x*(.5f+.5f*pan)-stereoSide*(.5f+.25f*character);
l+=left;r+=right;}

void CastleEngine::render(juce::AudioBuffer<float>& b, juce::MidiBuffer& m)
{
    b.clear();
    juce::MidiBuffer performanceMidi;
    rituals.process(m, performanceMidi, b.getNumSamples());
    auto midiIt = performanceMidi.begin();
    const auto midiEnd = performanceMidi.end();

    auto* L = b.getWritePointer(0);
    auto* R = b.getNumChannels() > 1 ? b.getWritePointer(1) : nullptr;
    delay.setDelay(delayTimeSamples);

    std::array<float, CurseMatrix::Lanes> lanePeak{};
    std::array<float, CurseMatrix::Destinations> destinationPeak{};

    for (int n = 0; n < b.getNumSamples(); ++n)
    {
        while (midiIt != midiEnd && (*midiIt).samplePosition <= n)
        {
            const auto meta = *midiIt;
            const auto msg = meta.getMessage();
            if (msg.isNoteOn()) on(msg.getNoteNumber(), msg.getVelocity());
            else if (msg.isNoteOff()) off(msg.getNoteNumber());
            else if (msg.isPitchWheel())
                pitchBendSemitones = ((float) msg.getPitchWheelValue() - 8192.0f) / 8192.0f * 2.0f;
            else if (msg.isController() && msg.getControllerNumber() == 1)
                modWheel = juce::jlimit(0.0f, 1.0f, msg.getControllerValue() / 127.0f);
            else if (msg.isChannelPressure())
                channelPressure = juce::jlimit(0.0f, 1.0f, msg.getChannelPressureValue() / 127.0f);
            else if (msg.isAftertouch())
            {
                const int note = msg.getNoteNumber();
                const float pressure = juce::jlimit(0.0f, 1.0f, msg.getAfterTouchValue() / 127.0f);
                for (auto& voice : voices)
                    if (voice.active && voice.note == note) voice.polyPressure = pressure;
            }
            else if (msg.isAllNotesOff() || msg.isAllSoundOff()) reset();
            ++midiIt;
        }
        float cryptL = 0.f, cryptR = 0.f, towerL = 0.f, towerR = 0.f;
        std::array<float, CurseMatrix::Destinations> busHex{};
        int activeVoices = 0;

        for (auto& v : voices)
        {
            if (!v.active) continue;
            ++activeVoices;

            v.amp.next(sr); v.iron.next(sr);
            if(globalGlide>.001f){const float glideSec=.004f+globalGlide*.72f;const float g=1.f-std::exp(-1.f/(glideSec*(float)sr));v.pitchNote+=((float)v.note-v.pitchNote)*g;}else v.pitchNote=(float)v.note;
            const float mod = (v.iron.value - .5f) * .22f;
            float cl=0.f, cr=0.f, tl=0.f, tr=0.f;
            const float key = juce::jlimit(-1.f, 1.f, (v.note - 60) / 36.f);
            const float random = rnd();
            const float blood = v.amp.value;
            const float wraith = v.iron.value;

            const float pressure = juce::jlimit(0.f, 1.f, std::max(channelPressure, v.polyPressure));
            auto hx = hex.evaluate(blood, wraith, v.velocity, key, random, modWheel, pressure, 1.f / (float)sr);
            const float hexScale = juce::jlimit(0.f, 1.f, patch.hexAmount);
            for (int d = 0; d < CurseMatrix::Destinations; ++d)
            {
                hx[(size_t)d] *= hexScale;
                busHex[(size_t)d] += hx[(size_t)d];
                destinationPeak[(size_t)d] = std::max(destinationPeak[(size_t)d], std::abs(hx[(size_t)d]));
            }
            for (int i = 0; i < CurseMatrix::Lanes; ++i)
                lanePeak[(size_t)i] = std::max(lanePeak[(size_t)i], std::abs(hex.getLaneValue(i) * hexScale));

            scene(v, patch.crypt, cl, cr, mod, hx, true);
            scene(v, patch.tower, tl, tr, -mod, hx, false);

            if(patch.crypt.crossSceneFM||patch.tower.crossSceneFM){const float ncL=std::sin(cl*juce::MathConstants<float>::pi+tl*1.35f);const float ncR=std::sin(cr*juce::MathConstants<float>::pi+tr*1.35f);const float ntL=std::sin(tl*juce::MathConstants<float>::pi-cl*1.05f);const float ntR=std::sin(tr*juce::MathConstants<float>::pi-cr*1.05f);cl=.58f*cl+.42f*ncL;cr=.58f*cr+.42f*ncR;tl=.58f*tl+.42f*ntL;tr=.58f*tr+.42f*ntR;}
            if(patch.crypt.crossSceneRing||patch.tower.crossSceneRing){const float ringL=std::tanh(cl*tl*5.f),ringR=std::tanh(cr*tr*5.f);cl=.72f*cl+.28f*ringL;cr=.72f*cr+.28f*ringR;tl=.72f*tl-.28f*ringL;tr=.72f*tr-.28f*ringR;}

            cryptL += cl; cryptR += cr;
            towerL += tl; towerR += tr;

            if (!v.amp.value && v.releasing) v.active = false;
        }

        if (activeVoices > 1)
            for (auto& x : busHex) x /= (float)activeVoices;

        // RITUALS carries a small performance imprint beyond note order: its
        // circular balance and curse pulse animate the pre-Ritual relationship.
        const float ritualBalance=rituals.getBalanceMod();
        const float ritualCurse=rituals.getCursePulse();
        cryptL*=1.f-ritualBalance*.18f;cryptR*=1.f-ritualBalance*.18f;
        towerL*=1.f+ritualBalance*.18f;towerR*=1.f+ritualBalance*.18f;

        // POSSESSION infects the chambers before they enter the Ritual bus.
        float possessedCL=cryptL, possessedCR=cryptR, possessedTL=towerL, possessedTR=towerR;
        possession.processSample(cryptL, cryptR, towerL, towerR,
                                 busHex[17], busHex[18], busHex[19], busHex[20]+ritualCurse*.12f,
                                 possessedCL, possessedCR, possessedTL, possessedTR);

        const float ritualAccent = rituals.getIntensityMod() * .18f + std::abs(ritualCurse)*.08f;
        float ritualL = 0.f, ritualR = 0.f;
        ritual.processSample(possessedCL, possessedCR, possessedTL, possessedTR,
                             busHex[7], busHex[13], busHex[14] + ritualAccent, busHex[15],
                             ritualL, ritualR);

        const float graveMod = busHex[8];
        const float effectiveDelayFeedback = juce::jlimit(0.f,.95f,delayFeedback + busHex[21]*.22f);
        const float dl = delay.popSample(0), dr = delay.popSample(1);
        delay.pushSample(0, ritualL + dr * effectiveDelayFeedback);
        delay.pushSample(1, ritualR + dl * effectiveDelayFeedback);
        const float outL = ritualL + dl * delayMix;
        const float outR = ritualR + dr * delayMix;

        const float graveTone01=juce::jlimit(0.f,1.f,(graveTone-.02f)/.46f + busHex[16]*.32f);
        grave.setParameters(juce::jlimit(0.f,1.f,patch.graveDepth+graveMod*.28f), graveTone01, ritualParams.width);
        float graveWetL=0.f,graveWetR=0.f;
        grave.processSample(outL,outR,graveWetL,graveWetR);
        const float graveMix=juce::jlimit(0.f,1.f,patch.graveDepth+graveMod*.45f);
        const float graveDry=1.f-.32f*graveMix;
        L[n]=clip((outL*graveDry+graveWetL*graveMix)*master);
        if(R)R[n]=clip((outR*graveDry+graveWetR*graveMix)*master);
    }

    for (int i = 0; i < CurseMatrix::Lanes; ++i)
        hexLaneTelemetry[(size_t)i].store(juce::jlimit(0.f,1.f,lanePeak[(size_t)i]), std::memory_order_relaxed);
    for (int d = 0; d < CurseMatrix::Destinations; ++d)
        hexDestinationTelemetry[(size_t)d].store(juce::jlimit(0.f,1.f,destinationPeak[(size_t)d]), std::memory_order_relaxed);
}
}

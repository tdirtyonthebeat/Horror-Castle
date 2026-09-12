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
static GeneratorType gt(int v){ return static_cast<GeneratorType>(juce::jlimit(0,17,v)); }
static FilterType ft(int v){ return static_cast<FilterType>(juce::jlimit(0,8,v)); }
static Route rt(int v){ return static_cast<Route>(juce::jlimit(0,3,v)); }

CastleEngine::CastleEngine() = default;
void CastleEngine::prepare(double s,int bs){
    sr=s; blockSize=bs;
    delay.setMaximumDelayInSamples((int)std::ceil(s*1.25));
    delay.prepare({s, static_cast<uint32_t>(juce::jmax(1,bs)), 2});
    grave.prepare(s); possession.prepare(s); rituals.prepare(s); ritual.prepare(s); reset();
}
void CastleEngine::configureEcology(Voice& v) noexcept {
    v.ecology.clear();
    CreatureRoutingMatrix::Route r;
    r.sourceCreature=VortexCreature; r.sourceSignal=CreatureStateBus::Signal::Event; r.destinationCreature=SirenCreature; r.destinationSignal=CreatureStateBus::Signal::Pressure; r.amount=ecologyDepth; r.smoothing=1.f; r.enabled=ecologyEnabled; v.ecology.setRoute(0,r);
    r.sourceCreature=PoltergeistCreature; r.sourceSignal=CreatureStateBus::Signal::Instability; r.destinationCreature=AuroraCreature; r.destinationSignal=CreatureStateBus::Signal::Field; r.amount=.72f*ecologyDepth; r.smoothing=.075f; r.enabled=ecologyEnabled; v.ecology.setRoute(1,r);
    r.sourceSignal=CreatureStateBus::Signal::Event; r.amount=.48f*ecologyDepth; r.smoothing=1.f; v.ecology.setRoute(2,r);
}
void CastleEngine::reset(){
    for(auto&v:voices){v=Voice{};configureEcology(v);}
    delay.reset(); grave.reset(); possession.reset(); rituals.reset(); hex.reset(); ritual.reset();
    for(auto& a : hexLaneTelemetry) a.store(0.f, std::memory_order_relaxed);
    for(auto& a : hexDestinationTelemetry) a.store(0.f, std::memory_order_relaxed);
    modWheel=0.f; channelPressure=0.f; pitchBendSemitones=0.f;
}
CastleEngine::Voice* CastleEngine::steal(){for(auto&v:voices)if(!v.active)return &v;return &*std::min_element(voices.begin(),voices.end(),[](auto&a,auto&b){return a.amp.value<b.amp.value;});}
void CastleEngine::on(int n,int vel){auto*v=steal();*v=Voice{};configureEcology(*v);v->active=true;v->note=n;v->pitchNote=globalGlide>.001f?lastPlayedNote:(float)n;lastPlayedNote=(float)n;v->velocity=vel/127.f;
v->cryptDetune=rnd()*11.0f; v->towerDetune=rnd()*0.8f; v->cryptWander=(rnd()+1.f)*.5f; v->towerWander=(rnd()+1.f)*.5f;
v->amp.attack=.004f;v->amp.decay=.32f;v->amp.sustain=.76f;v->amp.release=.18f;v->iron.attack=.006f;v->iron.decay=.24f;v->iron.sustain=.15f;v->iron.release=.22f;v->amp.on();v->iron.on();}
void CastleEngine::off(int n){for(auto&v:voices)if(v.active&&v.note==n){v.releasing=true;v.amp.off();v.iron.off();}}
float CastleEngine::rnd(){rng^=rng<<13;rng^=rng>>17;rng^=rng<<5;return float(rng)/2147483648.f-1.f;}
float CastleEngine::osc(GeneratorType t,float p,float shape,float f){
const float T=juce::MathConstants<float>::twoPi; const float dt=juce::jlimit(1.0e-6f,.49f,f/(float)sr);
switch(t){
case GeneratorType::VA:{
    // Production VA: band-limited saw/pulse with equal-power morphing and
    // conservative headroom. The old linear blend got noticeably thinner at
    // mid MORPH and exaggerated the shared distortion stage.
    const float saw=2.f*p-1.f-polyBlep(p,dt);
    const float width=juce::jlimit(.14f,.86f,.68f-.36f*shape);
    float pulse=p<width?1.f:-1.f;
    pulse+=polyBlep(p,dt);
    float e=p-width;if(e<0.f)e+=1.f;
    pulse-=polyBlep(e,dt);
    const float a=std::cos(shape*juce::MathConstants<float>::halfPi);
    const float b=std::sin(shape*juce::MathConstants<float>::halfPi);
    return juce::jlimit(-1.f,1.f,(saw*a+pulse*b)*.78f);
}
case GeneratorType::Wavetable:{
    // Band-limited harmonic-frame wavetable. MORPH changes spectral frame while
    // amplitude stays intentionally stable, closer to a polished table synth than
    // a waveshaper. Harmonics simply disappear before Nyquist instead of folding.
    const float h1=std::sin(T*p);
    const float h2=(f*2.f<sr*.45f)?std::sin(T*2.f*p+.19f):0.f;
    const float h3=(f*3.f<sr*.45f)?std::sin(T*3.f*p+.73f):0.f;
    const float h4=(f*4.f<sr*.45f)?std::sin(T*4.f*p+1.17f):0.f;
    const float h5=(f*5.f<sr*.45f)?std::sin(T*5.f*p+.41f):0.f;
    const float h7=(f*7.f<sr*.45f)?std::sin(T*7.f*p+1.43f):0.f;
    const float frameA=.92f*h1+.18f*h3;
    const float frameB=.58f*h1+.28f*h2+.20f*h4+.10f*h5;
    const float frameC=.44f*h1+.20f*h3+.16f*h5+.12f*h7;
    const float q=shape*2.f;
    const float y=q<1.f?frameA+(frameB-frameA)*q:frameB+(frameC-frameB)*(q-1.f);
    return y*.78f;
}
case GeneratorType::FM:{
    // Metallic, index-driven family with an intentionally non-integer ratio.
    const float ratio=1.37f+2.91f*shape;
    const float index=.25f+7.75f*shape*shape;
    const float mod=std::sin(T*p*ratio+.31f);
    return std::sin(T*p+index*mod);
}
case GeneratorType::PM:{
    // Clean phase modulation with a Nyquist-aware index. Keeping the modulator
    // ratio discrete-ish and limiting sideband span prevents the fizzy alias
    // cloud that previously made PM resemble several other bright engines.
    const float ratio=2.f+std::floor(shape*3.999f); // 2:1 .. 5:1
    const float rawIndex=.18f+3.35f*shape*shape;
    const float room=juce::jlimit(.12f,1.f,(float)(sr*.44f/std::max(1.f,f)-1.f)/std::max(1.f,ratio*4.f));
    const float index=rawIndex*room;
    const float mod=std::sin(T*p*ratio);
    return std::sin(T*p+index*mod)*.82f;
}
case GeneratorType::Vector:{
    // Four-corner vector path: sine -> triangle -> saw-ish -> hollow octave.
    const float sine=std::sin(T*p);
    const float tri=(2.f/juce::MathConstants<float>::pi)*std::asin(std::sin(T*p));
    const float saw=2.f*p-1.f-polyBlep(p,dt);
    const float octave=(f*2.f<sr*.46f)?std::sin(T*2.f*p+.65f):0.f;
    const float q=shape*3.f; const int region=juce::jlimit(0,2,(int)q); const float t=q-(float)region;
    const float a=std::cos(t*juce::MathConstants<float>::halfPi),b=std::sin(t*juce::MathConstants<float>::halfPi);
    if(region==0)return (sine*a+tri*b)*.76f;
    if(region==1)return (tri*a+saw*b)*.76f;
    return (saw*a+(sine*.35f+octave*.65f)*b)*.76f;
}
case GeneratorType::Chip:{
    // Duty-cycle and bit-depth are coupled into one deliberately digital macro.
    const float width=.12f+.70f*shape;
    float q=p<width?1.f:-1.f; q+=polyBlep(p,dt); float e=p-width;if(e<0)e+=1.f;q-=polyBlep(e,dt);
    const float steps=2.f+std::floor((1.f-shape)*14.f);
    const float staircase=std::round((2.f*p-1.f)*steps)/steps;
    return juce::jlimit(-1.f,1.f,q*(.72f-.28f*shape)+staircase*(.18f+.42f*shape));
}
case GeneratorType::Noise:{
    // Pitched-noise window makes this family obviously textural even at neutral settings.
    const float gate=.30f+.70f*std::abs(std::sin(T*p*(1.f+7.f*shape)));
    return rnd()*gate;
}
case GeneratorType::Resonator:{
    float y=.72f*std::sin(T*p);
    if(f*2.03f<sr*.46f)y+=(.18f+.30f*shape)*std::sin(T*p*2.03f+.17f);
    if(f*3.97f<sr*.46f)y+=(.10f+.25f*shape)*std::sin(T*p*3.97f+.73f);
    if(f*6.91f<sr*.46f)y+=.14f*shape*std::sin(T*p*6.91f+1.21f);
    return std::tanh(y*(1.0f+.55f*shape));
}
default:return std::sin(T*p);
}}
float CastleEngine::filter(float x,float&z,const FilterCell&f,float mod){if(!f.enabled)return x;float c=juce::jlimit(.002f,.48f,f.cutoff+mod),q=juce::jlimit(.05f,1.f,f.resonance),a=std::exp(-juce::MathConstants<float>::twoPi*c),lp=(1-a)*x+a*z;z=lp;float y=lp;if(f.type==FilterType::HighPass)y=x-lp;else if(f.type==FilterType::BandPass)y=.5f*(x-lp);else if(f.type==FilterType::Notch)y=x-.7f*lp;else if(f.type==FilterType::Comb){float comb=x+.72f*z;z=x;y=.55f*comb;}else if(f.type==FilterType::Shaper)y=std::tanh(lp*(1+f.drive*8.f));else y=clip(lp*(1+f.drive*3.f+q*.35f));return y;}
void CastleEngine::setParameters(const juce::AudioProcessorValueTreeState& apvts){
    auto get=[&](const juce::String& id,float fallback){if(auto* p=apvts.getRawParameterValue(id))return p->load();return fallback;};
    auto getb=[&](const juce::String& id,bool fallback){return get(id,fallback?1.f:0.f)>.5f;};
    auto choice=[&](const juce::String& id,int maxIndex,int fallback){ return juce::jlimit(0, maxIndex, (int)std::lround(get(id, (float)fallback))); };
    auto loadScene=[&](SceneArchitecture& s,const char* name){
        for(int i=0;i<3;++i){auto& g=s.voice.generators[i];g.type=gt(choice(param::id(name,i+1,"type"),17,(int)g.type));g.level=get(param::id(name,i+1,"level"),g.level);g.pan=get(param::id(name,i+1,"pan"),g.pan);g.tune=get(param::id(name,i+1,"tune"),g.tune);g.shape=get(param::id(name,i+1,"shape"),g.shape);g.spread=get(param::id(name,i+1,"spread"),g.spread);g.enabled=getb(param::id(name,i+1,"enabled"),g.enabled);}
        s.voice.noise.enabled=getb(param::noise(name,"enabled"),s.voice.noise.enabled);s.voice.noise.level=get(param::noise(name,"level"),s.voice.noise.level);
        for(int i=0;i<2;++i){auto& f=s.voice.filters[i];f.type=ft(choice(param::fid(name,i+1,"type"),8,(int)f.type));f.cutoff=get(param::fid(name,i+1,"cutoff"),f.cutoff);f.resonance=get(param::fid(name,i+1,"resonance"),f.resonance);f.drive=get(param::fid(name,i+1,"drive"),f.drive);f.enabled=getb(param::fid(name,i+1,"enabled"),f.enabled);}
        s.voice.filterRoute=rt(choice(param::route(name),3,(int)s.voice.filterRoute));s.voice.master=get(param::scene(name,"master"),s.voice.master);s.sceneBalance=get(param::scene(name,"balance"),s.sceneBalance);s.character=get(param::scene(name,"character"),s.character);s.crossSceneFM=getb(param::scene(name,"crossfm"),s.crossSceneFM);s.crossSceneRing=getb(param::scene(name,"crossring"),s.crossSceneRing);
    };
    loadScene(patch.crypt,"crypt");loadScene(patch.tower,"tower"); hex.load(apvts);
    patch.ritualMix=get("ritual.mix",patch.ritualMix); patch.hexAmount=get("global.hex",patch.hexAmount); patch.graveDepth=get("grave.reverb",patch.graveDepth);
    master=get("grave.output",.78f); globalGlide=get("global.glide",0.f); globalUnison=juce::jlimit(1,8,(int)std::lround(get("global.unison",1.f)));
    corpsePosition=get("corpse.position",.34f); corpseRot=get("corpse.rot",.22f); corpseFormant=get("corpse.formant",0.f); corpseInharmonic=get("corpse.inharmonic",.08f);

    living.abyssDepth=get("living.abyss.depth",.58f); living.abyssPressure=get("living.abyss.pressure",.46f); living.abyssDread=get("living.abyss.dread",.38f);
    living.poltergeistCharge=get("living.poltergeist.charge",.70f); living.poltergeistArc=get("living.poltergeist.arc",.48f); living.poltergeistInstability=get("living.poltergeist.instability",.42f);
    living.vortexTurbulence=get("living.vortex.turbulence",.82f); living.vortexPressure=get("living.vortex.pressure",.52f); living.vortexCollapse=get("living.vortex.collapse",.46f);
    living.mirrorReflection=get("living.mirror.reflection",.58f); living.mirrorSmear=get("living.mirror.smear",.40f); living.mirrorFracture=get("living.mirror.fracture",.34f);
    living.auroraField=get("living.aurora.field",.70f); living.auroraRadiance=get("living.aurora.radiance",.56f); living.auroraInstability=get("living.aurora.instability",.36f);
    living.sirenAperture=get("living.siren.aperture",.82f); living.sirenBreath=get("living.siren.breath",.58f); living.sirenEdge=get("living.siren.edge",.44f);

    ecologyEnabled=getb("ecology.enabled",false); ecologyDepth=juce::jlimit(0.f,1.f,get("ecology.depth",.65f)); for(auto& v:voices) configureEcology(v);
    ritualParams.mode=static_cast<RitualMode>(choice("ritual.mode",4,0));ritualParams.mix=get("ritual.mix",0.f);ritualParams.depth=get("ritual.depth",.35f);ritualParams.drive=get("ritual.drive",.12f);ritualParams.width=get("ritual.width",.55f);ritualParams.feedback=get("ritual.feedback",.28f);ritual.setParameters(ritualParams);
    possessionParams.bloodFeed=get("possession.bloodFeed",0.f);possessionParams.aetherLeak=get("possession.aetherLeak",0.f);possessionParams.soulExchange=get("possession.soulExchange",0.f);possessionParams.haunt=get("possession.haunt",0.f);possession.setParameters(possessionParams);
    ritualsParams.enabled=getb("rituals.enabled",false);ritualsParams.pattern=choice("rituals.pattern",7,0);ritualsParams.rate=choice("rituals.rate",3,2);ritualsParams.bpm=get("rituals.bpm",120.f);ritualsParams.gate=get("rituals.gate",.62f);ritualsParams.probability=get("rituals.probability",1.f);ritualsParams.swing=get("rituals.swing",0.f);ritualsParams.octaves=juce::jlimit(1,4,(int)std::lround(get("rituals.octaves",1.f)));rituals.setParameters(ritualsParams);
    delayMix=get("grave.delay",.18f);delayFeedback=get("grave.feedback",.28f);delayTimeSamples=juce::jlimit(1.0f,(float)delay.getMaximumDelayInSamples(),(float)sr*(.06f+.62f*get("grave.delay",.18f)));
    graveTone=get("grave.cutoff",.32f);const float graveTone01=juce::jlimit(0.f,1.f,(graveTone-.02f)/.46f);grave.setParameters(get("grave.reverb",.32f),graveTone01,get("ritual.width",.55f));
}

} // namespace horrorcastle

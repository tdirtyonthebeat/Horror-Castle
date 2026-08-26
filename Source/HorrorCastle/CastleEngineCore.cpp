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
static GeneratorType gt(int v){ return static_cast<GeneratorType>(juce::jlimit(0,12,v)); }
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
        for(int i=0;i<3;++i){auto& g=s.voice.generators[i];g.type=gt(choice(param::id(name,i+1,"type"),12,(int)g.type));g.level=get(param::id(name,i+1,"level"),g.level);g.pan=get(param::id(name,i+1,"pan"),g.pan);g.tune=get(param::id(name,i+1,"tune"),g.tune);g.shape=get(param::id(name,i+1,"shape"),g.shape);g.spread=get(param::id(name,i+1,"spread"),g.spread);g.enabled=getb(param::id(name,i+1,"enabled"),g.enabled);}
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

} // namespace horrorcastle

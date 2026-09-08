#include <JuceHeader.h>
#include "../Source/HorrorCastle/CastleEngine.h"
#include "../Source/HorrorCastle/CastleParameters.h"
#include "../Source/HorrorCastle/Grimoire.h"
#include "../Source/HorrorCastle/SynthesisFamilyContract.h"
#include <cmath>
#include <iostream>
#include <functional>
#include <vector>

namespace {
using namespace horrorcastle;

class HarnessProcessor final : public juce::AudioProcessor
{
public:
    HarnessProcessor()
        : juce::AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
          state(*this, nullptr, juce::Identifier("HorrorCastleSignatureHarness"), param::createLayout()) {}
    void prepareToPlay(double,int) override {} void releaseResources() override {}
    void processBlock(juce::AudioBuffer<float>& b,juce::MidiBuffer&) override { b.clear(); }
    juce::AudioProcessorEditor* createEditor() override { return nullptr; } bool hasEditor() const override { return false; }
    const juce::String getName() const override { return "Horror Castle Signature Harness"; }
    bool acceptsMidi() const override { return true; } bool producesMidi() const override { return false; } bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; } int getCurrentProgram() override { return 0; } void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; } void changeProgramName(int,const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override {} void setStateInformation(const void*,int) override {}
    juce::AudioProcessorValueTreeState state;
};

struct Render { std::vector<float> left, right; };

void setParam(HarnessProcessor& h,const juce::String& id,float denormalised)
{
    if(auto* p=h.state.getParameter(id))
        p->setValueNotifyingHost(p->convertTo0to1(denormalised));
}

void neutral(HarnessProcessor& h)
{
    setParam(h,"grave.delay",0.f); setParam(h,"grave.feedback",0.f); setParam(h,"grave.reverb",0.f);
    setParam(h,"ritual.mix",0.f); setParam(h,"global.hex",1.f); setParam(h,"rituals.enabled",0.f);
    setParam(h,"possession.bloodFeed",0.f); setParam(h,"possession.aetherLeak",0.f); setParam(h,"possession.soulExchange",0.f); setParam(h,"possession.haunt",0.f);
}

Render renderAt(double sampleRate,int block,double seconds,const std::function<void(HarnessProcessor&)>& configure)
{
    HarnessProcessor h; neutral(h); configure(h);
    CastleEngine engine; engine.prepare(sampleRate,block); engine.setParameters(h.state);
    const int total=(int)std::ceil(seconds*sampleRate); Render out; out.left.reserve((size_t)total); out.right.reserve((size_t)total);
    juce::AudioBuffer<float> audio(2,block);
    for(int pos=0;pos<total;pos+=block){
        juce::MidiBuffer midi;
        if(pos==0)midi.addEvent(juce::MidiMessage::noteOn(1,60,(juce::uint8)104),0);
        const int off=(int)(seconds*.72*sampleRate); if(off>=pos&&off<pos+block)midi.addEvent(juce::MidiMessage::noteOff(1,60),off-pos);
        engine.render(audio,midi);
        const int n=juce::jmin(block,total-pos);for(int i=0;i<n;++i){out.left.push_back(audio.getSample(0,i));out.right.push_back(audio.getSample(1,i));}
    }
    return out;
}
Render render(double seconds,const std::function<void(HarnessProcessor&)>& configure){return renderAt(48000.0,256,seconds,configure);}

float rms(const Render& r){double s=0;for(float x:r.left)s+=x*x;return r.left.empty()?0.f:(float)std::sqrt(s/(double)r.left.size());}
float brightnessProxy(const Render& r){
    if(r.left.size()<2)return 0.f;
    double d=0,s=0;
    for(size_t i=1;i<r.left.size();++i){const double x=r.left[i];const double dx=x-r.left[i-1];d+=dx*dx;s+=x*x;}
    return s>1.0e-12?(float)std::sqrt(d/s):0.f;
}
float lowBodyProxy(const Render& r){
    if(r.left.empty())return 0.f;
    constexpr float sr=48000.f, cutoff=420.f;
    const float a=1.f-std::exp(-juce::MathConstants<float>::twoPi*cutoff/sr);
    float z=0.f;double low=0,total=0;
    for(float x:r.left){z+=a*(x-z);low+=z*z;total+=x*x;}
    return total>1.0e-12?(float)std::sqrt(low/total):0.f;
}
float difference(const Render& a,const Render& b){const size_t n=std::min(a.left.size(),b.left.size());double s=0;for(size_t i=0;i<n;++i){const double d=a.left[i]-b.left[i];s+=d*d;}const float d=n?(float)std::sqrt(s/(double)n):0.f;return d/std::max(.0001f,std::max(rms(a),rms(b)));}
float waveformCorrelation(const Render& a,const Render& b){
    const size_t n=std::min(a.left.size(),b.left.size());if(n<2)return 1.f;
    double ab=0,aa=0,bb=0;for(size_t i=0;i<n;++i){const double x=a.left[i],y=b.left[i];ab+=x*y;aa+=x*x;bb+=y*y;}
    return (aa>1.0e-12&&bb>1.0e-12)?(float)std::abs(ab/std::sqrt(aa*bb)):1.f;
}
float crestProxy(const Render& r){float peak=0.f;for(float x:r.left)peak=std::max(peak,std::abs(x));return peak/std::max(.0001f,rms(r));}
float transientProxy(const Render& r){if(r.left.empty())return 0.f;const size_t n=std::min<size_t>(r.left.size(),2400);double a=0,b=0;for(size_t i=0;i<n;++i)a+=r.left[i]*r.left[i];for(size_t i=n;i<std::min(r.left.size(),n*4);++i)b+=r.left[i]*r.left[i];const float ar=n?(float)std::sqrt(a/n):0.f;const size_t bn=std::min(r.left.size(),n*4)-n;const float br=bn?(float)std::sqrt(b/bn):0.f;return ar/std::max(.0001f,br);}
float stereoProxy(const Render& r){const size_t n=std::min(r.left.size(),r.right.size());double side=0,mid=0;for(size_t i=0;i<n;++i){const double l=r.left[i],rr=r.right[i];side+=(l-rr)*(l-rr);mid+=(l+rr)*(l+rr);}return mid>1.0e-12?(float)std::sqrt(side/mid):0.f;}
float temporalFluxProxy(const Render& r){if(r.left.size()<1024)return 0.f;constexpr size_t hop=512;double prev=0,flux=0;int frames=0;for(size_t p=0;p+hop<=r.left.size();p+=hop){double e=0;for(size_t i=p;i<p+hop;++i)e+=r.left[i]*r.left[i];e=std::sqrt(e/hop);if(frames>0)flux+=std::abs(e-prev);prev=e;++frames;}return frames>1?(float)(flux/(frames-1))/std::max(.0001f,rms(r)):0.f;}
float contractDistance(const SynthesisFamilyContract& a,const SynthesisFamilyContract& b){
    const float d0=a.morphCurve-b.morphCurve,d1=a.articulationPower-b.articulationPower,d2=a.transientBoost-b.transientBoost;
    const float d3=a.sustainBias-b.sustainBias,d4=a.clarity-b.clarity,d5=a.nonlinearDrive-b.nonlinearDrive,d6=a.stereoMotion-b.stereoMotion;
    return std::sqrt(d0*d0+d1*d1+d2*d2+d3*d3+d4*d4+d5*d5+d6*d6);
}

bool finite(const Render& r){for(float x:r.left)if(!std::isfinite(x)||std::abs(x)>1.01f)return false;for(float x:r.right)if(!std::isfinite(x)||std::abs(x)>1.01f)return false;return true;}
float tailRms(const Render& r,size_t count=4800){if(r.left.empty())return 0.f;const size_t begin=r.left.size()>count?r.left.size()-count:0;double s=0;size_t n=0;for(size_t i=begin;i<r.left.size();++i){s+=r.left[i]*r.left[i];++n;}return n?(float)std::sqrt(s/(double)n):0.f;}
void setPossession(HarnessProcessor& h,const char* id,float amount){setParam(h,juce::String("possession.")+id,amount);}
void hexDestination(HarnessProcessor& h,int destination,float amount=.72f){setParam(h,"hex.curse1.source",6.f);setParam(h,"hex.curse1.curse",0.f);setParam(h,"hex.curse1.destination",(float)destination);setParam(h,"hex.curse1.amount",amount);}

void sceneOnly(HarnessProcessor& h,bool crypt){setParam(h,"crypt.master",crypt?.86f:0.f);setParam(h,"tower.master",crypt?0.f:.86f);setParam(h,crypt?"crypt.character":"tower.character",.82f);}
void ritual(HarnessProcessor& h,int mode){setParam(h,"ritual.mode",(float)mode);setParam(h,"ritual.mix",.82f);setParam(h,"ritual.depth",.58f);setParam(h,"ritual.drive",.46f);setParam(h,"ritual.feedback",.42f);}
void curse(HarnessProcessor& h,int curseKind){setParam(h,"hex.curse1.source",6.f);setParam(h,"hex.curse1.curse",(float)curseKind);setParam(h,"hex.curse1.destination",9.f);setParam(h,"hex.curse1.amount",.22f);}

void exclusiveEngine(HarnessProcessor& h,bool crypt,int typeIndex)
{
    sceneOnly(h,crypt);
    const juce::String scene=crypt?"crypt":"tower";
    setParam(h,scene+".character",.88f);
    for(int i=1;i<=3;++i){
        setParam(h,scene+".g"+juce::String(i)+".enabled",i==1?1.f:0.f);
        setParam(h,scene+".g"+juce::String(i)+".level",i==1?.86f:0.f);
        setParam(h,scene+".g"+juce::String(i)+".tune",0.f);
    }
    setParam(h,scene+".g1.type",(float)typeIndex);
    setParam(h,scene+".g1.shape",.58f);
}

bool writeWav(const juce::File& file,const Render& r)
{
    file.deleteFile(); juce::WavAudioFormat format;
    std::unique_ptr<juce::AudioFormatWriter> writer(format.createWriterFor(new juce::FileOutputStream(file),48000.0,2,16,{},0));
    if(!writer)return false; juce::AudioBuffer<float> b(2,(int)r.left.size());
    b.copyFrom(0,0,r.left.data(),(int)r.left.size());b.copyFrom(1,0,r.right.data(),(int)r.right.size());
    return writer->writeFromAudioSampleBuffer(b,0,b.getNumSamples());
}

}

int main(int argc,char* argv[])
{
    juce::ScopedJuceInitialiser_GUI init;
    int failures=0; auto check=[&](bool ok,const char* name){std::cout<<(ok?"PASS  ":"FAIL  ")<<name<<"\n";if(!ok)++failures;};

    auto crypt=render(1.1,[](auto& h){sceneOnly(h,true);});
    auto tower=render(1.1,[](auto& h){sceneOnly(h,false);});
    check(finite(crypt)&&finite(tower),"CRYPT/TOWER output remains finite");
    const float chamberDifference=difference(crypt,tower);
    const float cryptBrightness=brightnessProxy(crypt),towerBrightness=brightnessProxy(tower);
    const float cryptLow=lowBodyProxy(crypt),towerLow=lowBodyProxy(tower);
    std::cout<<"INFO  chamber difference="<<chamberDifference
             <<" brightness(C/T)="<<cryptBrightness<<"/"<<towerBrightness
             <<" low-body(C/T)="<<cryptLow<<"/"<<towerLow<<"\n";
    check(chamberDifference>.35f,"CRYPT and TOWER strongly differ");
    check(towerBrightness>cryptBrightness*1.08f,"TOWER is spectrally brighter than CRYPT");
    check(cryptLow>towerLow*1.08f,"CRYPT carries more low-body energy than TOWER");


    auto undercrypt=render(1.1,[](auto& h){exclusiveEngine(h,true,8);});
    auto corpse=render(1.1,[](auto& h){exclusiveEngine(h,true,9);});
    auto bone=render(1.1,[](auto& h){exclusiveEngine(h,true,10);});
    auto rotator=render(1.1,[](auto& h){exclusiveEngine(h,true,11);});
    auto bellGlass=render(1.1,[](auto& h){exclusiveEngine(h,false,8);});
    auto spectralSpire=render(1.1,[](auto& h){exclusiveEngine(h,false,9);});
    auto astralFM=render(1.1,[](auto& h){exclusiveEngine(h,false,10);});
    auto prism=render(1.1,[](auto& h){exclusiveEngine(h,false,11);});

    check(finite(undercrypt)&&finite(corpse)&&finite(bone)&&finite(rotator),"all CRYPT-exclusive engines remain finite");
    check(finite(bellGlass)&&finite(spectralSpire)&&finite(astralFM)&&finite(prism),"all TOWER-exclusive engines remain finite");
    check(difference(undercrypt,corpse)>.06f&&difference(undercrypt,bone)>.06f&&difference(undercrypt,rotator)>.06f,
          "CRYPT-exclusive engines have distinct identities");
    check(difference(bellGlass,spectralSpire)>.06f&&difference(bellGlass,astralFM)>.06f&&difference(bellGlass,prism)>.06f,
          "TOWER-exclusive engines have distinct identities");
    check(lowBodyProxy(undercrypt)>lowBodyProxy(bellGlass)*1.10f,"UNDERCRYPT lives below BELL GLASS");
    const float corpseBrightness=brightnessProxy(corpse);
    const float spireBrightness=brightnessProxy(spectralSpire);
    std::cout<<"INFO  exclusive brightness(CORPSE/SPIRE)="<<corpseBrightness<<"/"<<spireBrightness
             <<" ratio="<<(corpseBrightness>1.0e-9f?spireBrightness/corpseBrightness:0.f)<<"\n";
    check(spireBrightness>corpseBrightness*1.10f,"SPECTRAL SPIRE lives above CORPSE");

    // Synthesis Family Contract gate: every generator must declare all seven
    // behavioral laws, and the numeric implementation vectors must occupy
    // distinct regions before the audio test even runs.
    for(bool isCrypt : {true,false})
    {
        float nearestContract=1000.f;
        bool declarationsComplete=true;
        for(int type=0;type<18;++type)
        {
            const auto& a=synthesis_contract::get(static_cast<GeneratorType>(type),isCrypt);
            declarationsComplete=declarationsComplete
                && a.excitation[0] && a.synthesisMethod[0] && a.spectralMotion[0]
                && a.articulationLaw[0] && a.nonlinearity[0] && a.stereoLaw[0]
                && a.morphTrajectory[0];
            for(int other=type+1;other<18;++other)
                nearestContract=std::min(nearestContract,contractDistance(
                    a,synthesis_contract::get(static_cast<GeneratorType>(other),isCrypt)));
        }
        std::cout<<"INFO  "<<(isCrypt?"CRYPT":"TOWER")<<" nearest synthesis-contract region="<<nearestContract<<"\n";
        check(declarationsComplete,isCrypt?"all CRYPT generators declare seven synthesis laws":"all TOWER generators declare seven synthesis laws");
        check(nearestContract>.025f,isCrypt?"CRYPT synthesis contracts occupy distinct regions":"TOWER synthesis contracts occupy distinct regions");
    }

    // Full generator fingerprint gate: every selectable generator must produce a
    // materially different neutral render, and its single MORPH control must move it.
    for(bool isCrypt : {true,false})
    {
        std::vector<Render> fingerprints;
        fingerprints.reserve(18);
        bool allFinite=true, allMorph=true;
        float nearest=1000.f,maxCorrelation=0.f; int corrA=-1,corrB=-1;
        for(int type=0;type<18;++type)
        {
            auto base=render(.9,[isCrypt,type](auto& h){exclusiveEngine(h,isCrypt,type);setParam(h,isCrypt?"crypt.g1.shape":"tower.g1.shape",.24f);});
            auto moved=render(.9,[isCrypt,type](auto& h){exclusiveEngine(h,isCrypt,type);setParam(h,isCrypt?"crypt.g1.shape":"tower.g1.shape",.86f);});
            allFinite=allFinite&&finite(base)&&finite(moved)&&rms(base)>.00015f;
            allMorph=allMorph&&difference(base,moved)>.006f;
            fingerprints.push_back(std::move(base));
        }
        for(size_t i=0;i<fingerprints.size();++i)
            for(size_t j=i+1;j<fingerprints.size();++j){
                nearest=std::min(nearest,difference(fingerprints[i],fingerprints[j]));
                const float corr=waveformCorrelation(fingerprints[i],fingerprints[j]);
                if(corr>maxCorrelation){maxCorrelation=corr;corrA=(int)i;corrB=(int)j;}
            }
        std::cout<<"INFO  "<<(isCrypt?"CRYPT":"TOWER")<<" nearest generator fingerprint distance="<<nearest
                 <<" max correlation="<<maxCorrelation<<" pair="<<corrA<<"/"<<corrB<<"\n";
        check(allFinite,isCrypt?"all 18 CRYPT generators are audible and finite":"all 18 TOWER generators are audible and finite");
        check(allMorph,isCrypt?"MORPH audibly changes every CRYPT generator":"MORPH audibly changes every TOWER generator");
        check(nearest>.018f,isCrypt?"every CRYPT generator has a distinct fingerprint":"every TOWER generator has a distinct fingerprint");
        check(maxCorrelation<.9985f,isCrypt?"no CRYPT creature collapses into a near-identical waveform":"no TOWER creature collapses into a near-identical waveform");
        float crestMin=1000.f,crestMax=0.f,transientMin=1000.f,transientMax=0.f,stereoMin=1000.f,stereoMax=0.f,fluxMin=1000.f,fluxMax=0.f;
        for(const auto& fp:fingerprints){
            const float cr=crestProxy(fp),tr=transientProxy(fp),st=stereoProxy(fp),fl=temporalFluxProxy(fp);
            crestMin=std::min(crestMin,cr);crestMax=std::max(crestMax,cr);
            transientMin=std::min(transientMin,tr);transientMax=std::max(transientMax,tr);
            stereoMin=std::min(stereoMin,st);stereoMax=std::max(stereoMax,st);
            fluxMin=std::min(fluxMin,fl);fluxMax=std::max(fluxMax,fl);
        }
        std::cout<<"INFO  "<<(isCrypt?"CRYPT":"TOWER")<<" behavioral-region spread crest="<<(crestMax-crestMin)
                 <<" transient="<<(transientMax-transientMin)<<" stereo="<<(stereoMax-stereoMin)<<" flux="<<(fluxMax-fluxMin)<<"\n";
        check(crestMax-crestMin>.10f,isCrypt?"CRYPT creatures differ dynamically":"TOWER creatures differ dynamically");
        check(transientMax-transientMin>.05f,isCrypt?"CRYPT creatures differ in articulation":"TOWER creatures differ in articulation");
        check(stereoMax-stereoMin>.002f,isCrypt?"CRYPT creatures differ in stereo law":"TOWER creatures differ in stereo law");
        check(fluxMax-fluxMin>.002f,isCrypt?"CRYPT creatures differ in temporal motion":"TOWER creatures differ in temporal motion");
    }

    // Named synthesis-family spot checks keep the Castle honest: these are
    // intentionally different techniques, not merely themed oscillator labels.
    auto vaLow=render(.9,[](auto& h){exclusiveEngine(h,true,0);setParam(h,"crypt.g1.shape",.15f);});
    auto vaHigh=render(.9,[](auto& h){exclusiveEngine(h,true,0);setParam(h,"crypt.g1.shape",.88f);});
    auto wavetable=render(.9,[](auto& h){exclusiveEngine(h,true,1);setParam(h,"crypt.g1.shape",.72f);});
    auto fmFamily=render(.9,[](auto& h){exclusiveEngine(h,true,2);setParam(h,"crypt.g1.shape",.72f);});
    auto additive=render(.9,[](auto& h){exclusiveEngine(h,false,8);setParam(h,"tower.g1.shape",.72f);});
    auto granular=render(.9,[](auto& h){exclusiveEngine(h,false,11);setParam(h,"tower.g1.shape",.82f);});
    auto resynthesis=render(.9,[](auto& h){exclusiveEngine(h,true,9);setParam(h,"crypt.g1.shape",.72f);});
    check(difference(vaLow,vaHigh)>.02f,"VA subtractive MORPH changes waveform character");
    check(difference(wavetable,fmFamily)>.08f,"Wavetable and FM families remain clearly distinct");
    check(difference(additive,granular)>.08f,"Additive and granular families remain clearly distinct");
    check(difference(granular,resynthesis)>.08f,"Granular and spectral-resynthesis families remain clearly distinct");

    auto corpsePositionSweep=render(1.1,[](auto& h){exclusiveEngine(h,true,9);setParam(h,"corpse.position",.96f);});
    auto corpseRotSweep=render(1.1,[](auto& h){exclusiveEngine(h,true,9);setParam(h,"corpse.rot",.92f);});
    auto corpseFormantSweep=render(1.1,[](auto& h){exclusiveEngine(h,true,9);setParam(h,"corpse.formant",.82f);});
    auto corpseInharmonicSweep=render(1.1,[](auto& h){exclusiveEngine(h,true,9);setParam(h,"corpse.inharmonic",.86f);});
    check(finite(corpsePositionSweep)&&finite(corpseRotSweep)&&finite(corpseFormantSweep)&&finite(corpseInharmonicSweep),
          "Spectral Corpse parameter extremes remain finite");
    check(difference(corpse,corpsePositionSweep)>.008f,"CORPSE Position audibly moves spectral frames");
    check(difference(corpse,corpseRotSweep)>.008f,"CORPSE ROT audibly decomposes the spectrum");
    check(difference(corpse,corpseFormantSweep)>.008f,"CORPSE Formant audibly remaps spectral shape");
    check(difference(corpse,corpseInharmonicSweep)>.008f,"CORPSE Inharmonicity audibly stretches partials");

    auto bind=render(1.1,[](auto& h){ritual(h,0);});
    auto sacrifice=render(1.1,[](auto& h){ritual(h,1);});
    auto summon=render(1.1,[](auto& h){ritual(h,2);});
    check(difference(bind,sacrifice)>.10f,"BIND differs from SACRIFICE");
    check(difference(bind,summon)>.10f,"BIND differs from SUMMON");

    auto clean=render(1.1,[](auto& h){curse(h,0);});
    auto corrupt=render(1.1,[](auto& h){curse(h,1);});
    auto haunt=render(1.1,[](auto& h){curse(h,2);});
    auto possession=render(1.1,[](auto& h){curse(h,3);});
    auto decay=render(1.1,[](auto& h){curse(h,4);});
    auto madness=render(1.1,[](auto& h){curse(h,5);});
    auto blood=render(1.1,[](auto& h){curse(h,6);});
    check(difference(clean,corrupt)>.015f,"CLEAN Curse differs from CORRUPT");
    check(difference(clean,haunt)>.015f,"CLEAN Curse differs from HAUNT");
    check(difference(clean,possession)>.015f,"CLEAN Curse differs from POSSESSION");
    check(difference(clean,decay)>.015f,"CLEAN Curse differs from DECAY");
    check(difference(clean,madness)>.015f,"CLEAN Curse differs from MADNESS");
    check(difference(clean,blood)>.015f,"CLEAN Curse differs from BLOOD");

    auto noPossession=render(1.25,[](auto&){});
    auto bloodFeed=render(1.25,[](auto& h){setPossession(h,"bloodFeed",.82f);});
    auto aetherLeak=render(1.25,[](auto& h){setPossession(h,"aetherLeak",.82f);});
    auto soulExchange=render(1.25,[](auto& h){setPossession(h,"soulExchange",.82f);});
    auto haunted=render(1.25,[](auto& h){setPossession(h,"haunt",.82f);});
    check(finite(bloodFeed)&&finite(aetherLeak)&&finite(soulExchange)&&finite(haunted),"Possession Matrix remains finite");
    check(difference(noPossession,bloodFeed)>.04f&&difference(noPossession,aetherLeak)>.04f&&difference(noPossession,soulExchange)>.04f&&difference(noPossession,haunted)>.02f,"Possession modes audibly affect the castle");

    auto hexDread=render(1.1,[](auto& h){sceneOnly(h,true);hexDestination(h,11);});
    auto hexAether=render(1.1,[](auto& h){sceneOnly(h,false);hexDestination(h,12);});
    auto hexSoul=render(1.1,[](auto& h){hexDestination(h,19);});
    check(difference(crypt,hexDread)>.015f,"HEX 2.0 reaches CRYPT DREAD");
    check(difference(tower,hexAether)>.015f,"HEX 2.0 reaches TOWER AETHER");
    check(difference(noPossession,hexSoul)>.015f,"HEX 2.0 reaches SOUL EXCHANGE");

    auto ritualPattern=render(1.8,[](auto& h){setParam(h,"rituals.enabled",1.f);setParam(h,"rituals.pattern",5.f);setParam(h,"rituals.rate",3.f);setParam(h,"rituals.gate",.42f);});
    check(finite(ritualPattern),"RITUALS performance engine remains finite");
    check(difference(noPossession,ritualPattern)>.08f,"RITUALS changes note performance");
    check(tailRms(ritualPattern)<.08f,"RITUALS releases notes without a stuck voice");

    bool factoriesFinite=true;
    HarnessProcessor catalogHarness; Grimoire catalog(catalogHarness.state);
    const int factoryCount=(int)catalog.getFactorySpells().size();
    for(int preset=0;preset<factoryCount;++preset){
        auto factory=render(1.15,[preset](auto& h){Grimoire g(h.state);g.loadFactory(preset);});
        factoriesFinite = factoriesFinite && finite(factory);
    }
    std::cout<<"INFO  Grimoire factory spells tested="<<factoryCount<<"\n";
    check(factoriesFinite&&factoryCount>=40,"all Grimoire factory spells remain finite");

    auto sr44=renderAt(44100.0,64,1.0,[](auto& h){setPossession(h,"soulExchange",.55f);});
    auto sr96=renderAt(96000.0,1024,1.0,[](auto& h){setPossession(h,"soulExchange",.55f);});
    check(finite(sr44)&&finite(sr96),"44.1/96 kHz and 64/1024-sample buffers remain finite");

    {
        HarnessProcessor a;setParam(a,"possession.soulExchange",.731f);setParam(a,"rituals.pattern",7.f);
        auto state=a.state.copyState();state.setProperty("stateVersion",Grimoire::CurrentStateVersion,nullptr);
        auto xml=state.createXml();auto round=xml?juce::ValueTree::fromXml(*xml):juce::ValueTree{};
        HarnessProcessor b;if(round.isValid())b.state.replaceState(round);
        const auto* p1=b.state.getRawParameterValue("possession.soulExchange");const auto* p2=b.state.getRawParameterValue("rituals.pattern");
        check(p1&&p2&&std::abs(p1->load()-.731f)<.002f&&std::abs(p2->load()-7.f)<.01f,"v1 state round-trip preserves new parameters");
    }

    auto stress=render(1.5,[](auto& h){setParam(h,"ritual.mode",4.f);setParam(h,"ritual.mix",1.f);setParam(h,"ritual.depth",1.f);setParam(h,"ritual.drive",1.f);setParam(h,"grave.reverb",1.f);setParam(h,"grave.feedback",.92f);setParam(h,"grave.output",1.f);setParam(h,"possession.bloodFeed",1.f);setParam(h,"possession.aetherLeak",1.f);setParam(h,"possession.soulExchange",1.f);setParam(h,"possession.haunt",1.f);});
    check(finite(stress),"extreme Ritual/Grave settings remain finite");

    if(argc>=3&&juce::String(argv[1])=="--render-dir"){
        juce::File dir{juce::String(argv[2])};dir.createDirectory();
        struct Item{const char* name;Render audio;};
        std::vector<Item> items;
        items.push_back({"01-crypt.wav",crypt});items.push_back({"02-tower.wav",tower});
        items.push_back({"02a-crypt-dread-max.wav",render(1.1,[](auto& h){sceneOnly(h,true);setParam(h,"crypt.character",1.f);})});
        items.push_back({"02b-tower-aether-max.wav",render(1.1,[](auto& h){sceneOnly(h,false);setParam(h,"tower.character",1.f);})});
        items.push_back({"15-crypt-undercrypt.wav",undercrypt});
        items.push_back({"16-crypt-corpse.wav",corpse});
        items.push_back({"17-crypt-bone-resonator.wav",bone});
        items.push_back({"18-crypt-rotator.wav",rotator});
        items.push_back({"19-tower-bell-glass.wav",bellGlass});
        items.push_back({"20-tower-spectral-spire.wav",spectralSpire});
        items.push_back({"21-tower-astral-fm.wav",astralFM});
        items.push_back({"22-tower-prism.wav",prism});
        items.push_back({"03-bind.wav",bind});items.push_back({"04-sacrifice.wav",sacrifice});items.push_back({"05-summon.wav",summon});
        items.push_back({"06-possess.wav",render(1.1,[](auto& h){ritual(h,3);})});items.push_back({"07-devour.wav",render(1.1,[](auto& h){ritual(h,4);})});
        items.push_back({"08-curse-clean.wav",clean});items.push_back({"09-curse-corrupt.wav",corrupt});items.push_back({"10-curse-haunt.wav",haunt});items.push_back({"11-curse-possession.wav",possession});items.push_back({"12-curse-decay.wav",decay});items.push_back({"13-curse-madness.wav",madness});items.push_back({"14-curse-blood.wav",blood});
        items.push_back({"23-possession-blood-feed.wav",bloodFeed});items.push_back({"24-possession-aether-leak.wav",aetherLeak});items.push_back({"25-possession-soul-exchange.wav",soulExchange});items.push_back({"26-possession-haunt.wav",haunted});
        items.push_back({"27-rituals-possession.wav",ritualPattern});items.push_back({"28-hex-dread.wav",hexDread});items.push_back({"29-hex-aether.wav",hexAether});items.push_back({"30-hex-soul-exchange.wav",hexSoul});
        items.push_back({"31-corpse-position.wav",corpsePositionSweep});
        items.push_back({"32-corpse-rot.wav",corpseRotSweep});
        items.push_back({"33-corpse-formant.wav",corpseFormantSweep});
        items.push_back({"34-corpse-inharmonic.wav",corpseInharmonicSweep});
        for(auto& item:items)check(writeWav(dir.getChildFile(item.name),item.audio),item.name);
        std::cout<<"Rendered signature references to: "<<dir.getFullPathName()<<"\n";
    }

    std::cout<<(failures==0?"\nHorror Castle signature check passed.\n":"\nHorror Castle signature check FAILED.\n");
    return failures==0?0:1;
}

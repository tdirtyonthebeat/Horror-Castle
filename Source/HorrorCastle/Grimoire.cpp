#include "Grimoire.h"
#include "FactoryPresetManifest.h"
#include "CastleParameters.h"
#include <cmath>

namespace horrorcastle {

std::vector<Grimoire::SpellInfo> Grimoire::getFactorySpells() const
{
    std::vector<SpellInfo> out;
    const auto manifest=FactoryPresetManifest::create(); out.reserve(manifest.size());
    for(const auto& p:manifest) out.push_back({p.name,p.subtitle,p.description,p.category,p.tags,p.sigil});
    return out;
}

juce::StringArray Grimoire::getFactoryNames() const { juce::StringArray names; for(const auto& s:getFactorySpells()) names.add(s.name); return names; }
void Grimoire::set(const juce::String& id,float actual){if(auto*p=apvts.getParameter(id)){p->beginChangeGesture();p->setValueNotifyingHost(p->convertTo0to1(actual));p->endChangeGesture();}}
void Grimoire::setHex(int lane,int source,int curse,int destination,float amount){const auto p=juce::String("hex.curse")+juce::String(juce::jlimit(1,8,lane))+".";set(p+"source",(float)source);set(p+"curse",(float)curse);set(p+"destination",(float)destination);set(p+"amount",amount);}

void Grimoire::commonReset()
{
    // Factory spells always begin from the complete INIT contract. No generator,
    // filter, modulation, effect, ecology, or Living Engine physics value is allowed
    // to survive from the previously loaded spell.
    for (const auto* sceneName : {"crypt", "tower"})
    {
        const bool isCrypt = juce::String(sceneName) == "crypt";
        for (int g = 1; g <= 3; ++g)
        {
            const auto p=[&](const char* control){return param::id(sceneName,g,control);};
            set(p("type"),(float)(14+g));
            set(p("level"),g==1?(isCrypt?.72f:.58f):0.f);
            set(p("pan"),0.f); set(p("tune"),0.f);
            set(p("shape"),g==1?.58f:(g==2?.70f:.82f));
            set(p("spread"),0.f); set(p("enabled"),1.f);
        }
        set(param::noise(sceneName,"enabled"),0.f); set(param::noise(sceneName,"level"),.08f);
        for(int f=1;f<=2;++f)
        {
            set(param::fid(sceneName,f,"type"),0.f);
            set(param::fid(sceneName,f,"cutoff"),f==1?.42f:.46f);
            set(param::fid(sceneName,f,"resonance"),.08f);
            set(param::fid(sceneName,f,"drive"),.02f);
            set(param::fid(sceneName,f,"enabled"),1.f);
        }
        set(param::route(sceneName),1.f);
        set(param::scene(sceneName,"master"),isCrypt?.76f:.62f);
        set(param::scene(sceneName,"balance"),isCrypt?-.28f:.28f);
        set(param::scene(sceneName,"character"),isCrypt?.38f:.32f);
        set(param::scene(sceneName,"crossfm"),0.f); set(param::scene(sceneName,"crossring"),0.f);
    }

    set("global.glide",0.f); set("global.unison",1.f); set("global.hex",0.f);
    set("ritual.mode",0.f);set("ritual.mix",0.f);set("ritual.depth",.35f);set("ritual.drive",.12f);set("ritual.width",.55f);set("ritual.feedback",.28f);
    set("grave.reverb",.10f);set("grave.delay",.04f);set("grave.feedback",.18f);set("grave.cutoff",.38f);set("grave.output",.82f);
    set("corpse.position",.34f);set("corpse.rot",.22f);set("corpse.formant",0.f);set("corpse.inharmonic",.08f);
    set("possession.bloodFeed",0.f);set("possession.aetherLeak",0.f);set("possession.soulExchange",0.f);set("possession.haunt",0.f);
    set("ecology.enabled",0.f);set("ecology.depth",.65f);
    set("living.abyss.depth",.58f);set("living.abyss.pressure",.46f);set("living.abyss.dread",.38f);
    set("living.poltergeist.charge",.70f);set("living.poltergeist.arc",.48f);set("living.poltergeist.instability",.42f);
    set("living.vortex.turbulence",.82f);set("living.vortex.pressure",.52f);set("living.vortex.collapse",.46f);
    set("living.mirror.reflection",.58f);set("living.mirror.smear",.40f);set("living.mirror.fracture",.34f);
    set("living.aurora.field",.70f);set("living.aurora.radiance",.56f);set("living.aurora.instability",.36f);
    set("living.siren.aperture",.82f);set("living.siren.breath",.58f);set("living.siren.edge",.44f);
    set("rituals.enabled",0.f);set("rituals.pattern",0.f);set("rituals.rate",2.f);set("rituals.bpm",120.f);set("rituals.gate",.62f);set("rituals.probability",1.f);set("rituals.swing",0.f);set("rituals.octaves",1.f);
    for(int i=1;i<=8;++i)
    {
        const auto p=juce::String("hex.curse")+juce::String(i)+".";
        set(p+"source",0.f);set(p+"curse",0.f);set(p+"destination",0.f);set(p+"amount",0.f);
        set(p+"decay",.25f);set(p+"corruptBias",.22f);set(p+"hauntRate",1.70f);
        set(p+"possessionFold",.35f);set(p+"decayTime",1.50f);set(p+"madnessSteps",8.f);set(p+"bloodHunger",.65f);
    }
}

static float seeded01(std::uint32_t& s){s=s*1664525u+1013904223u;return float((s>>8)&0x00ffffffu)/float(0x00ffffffu);}
void Grimoire::loadGeneratedFactory(int archetype,std::uint32_t seed)
{
    auto u=[&](){return seeded01(seed);}; auto vary=[&](float center,float radius){return juce::jlimit(0.f,1.f,center+(u()*2.f-1.f)*radius);};
    const float grave=vary(.34f,.16f),character=vary(.78f,.18f);
    set("crypt.character",character);set("tower.character",vary(.76f,.18f));set("grave.reverb",grave);set("grave.delay",vary(.16f,.12f));set("grave.feedback",vary(.25f,.16f));
    switch(archetype){
      case 0:set("crypt.g1.type",u()>.5f?17.f:15.f);set("crypt.g1.level",.78f);set("crypt.g1.shape",vary(.78f,.18f));set("crypt.g2.type",u()>.55f?14.f:13.f);set("crypt.g2.level",.28f);set("crypt.g2.tune",-12.f);set("tower.master",vary(.16f,.10f));set("crypt.f1.cutoff",vary(.075f,.035f));set("grave.reverb",vary(.18f,.10f));break;
      case 1:set("crypt.g1.type",u()>.5f?12.f:9.f);set("tower.g1.type",u()>.45f?16.f:17.f);set("crypt.g1.level",.48f);set("tower.g1.level",.64f);set("tower.g1.shape",vary(.72f,.20f));set("global.glide",vary(.16f,.12f));set("grave.reverb",vary(.28f,.12f));setHex(1,7,2,12,vary(.38f,.18f));break;
      case 2:set("crypt.g1.type",u()>.5f?9.f:12.f);set("tower.g1.type",u()>.5f?13.f:15.f);set("tower.g2.type",u()>.5f?14.f:16.f);set("crypt.g1.level",.38f);set("tower.g1.level",.48f);set("tower.g2.level",.32f);set("global.unison",4.f+std::floor(u()*3.f));set("grave.reverb",vary(.64f,.16f));set("grave.delay",vary(.28f,.14f));break;
      case 3:set("crypt.g1.type",u()>.5f?15.f:17.f);set("crypt.g2.type",u()>.5f?13.f:14.f);set("tower.g1.type",u()>.5f?16.f:14.f);set("crypt.g1.level",.58f);set("crypt.g2.level",.30f);set("tower.g1.level",.30f);set("grave.reverb",vary(.52f,.18f));setHex(1,5,2,11,vary(.30f,.20f));break;
      case 4:set("crypt.g1.type",u()>.5f?10.f:14.f);set("tower.g1.type",u()>.5f?13.f:15.f);set("crypt.g1.level",.48f);set("tower.g1.level",.52f);set("grave.reverb",vary(.28f,.12f));set("grave.delay",vary(.12f,.08f));break;
      case 5:set("crypt.g1.type",u()>.5f?10.f:13.f);set("crypt.g2.type",u()>.5f?16.f:17.f);set("crypt.g1.level",.72f);set("crypt.g2.level",.34f);set("tower.master",.12f);set("grave.reverb",vary(.18f,.10f));set("ritual.drive",vary(.52f,.20f));break;
      case 6:set("crypt.g1.type",u()>.5f?10.f:14.f);set("tower.g1.type",u()>.5f?14.f:13.f);set("rituals.enabled",1.f);set("rituals.pattern",std::floor(u()*8.f));set("rituals.rate",std::floor(u()*4.f));set("rituals.probability",vary(.82f,.16f));set("ritual.mix",vary(.50f,.18f));break;
      case 7:set("crypt.g1.type",u()>.5f?17.f:12.f);set("tower.g1.type",u()>.5f?16.f:17.f);set("possession.haunt",vary(.62f,.22f));set("ritual.mode",u()>.5f?4.f:2.f);set("ritual.mix",vary(.58f,.18f));set("grave.reverb",vary(.68f,.18f));set("grave.delay",vary(.38f,.16f));break;
      default:set("crypt.g1.type",u()>.5f?17.f:16.f);set("crypt.g2.type",u()>.5f?16.f:15.f);set("tower.g1.type",u()>.5f?17.f:16.f);set("tower.g2.type",u()>.5f?16.f:15.f);set("crypt.g1.level",.52f);set("crypt.g2.level",.30f);set("tower.g1.level",.48f);set("tower.g2.level",.30f);set("ecology.enabled",1.f);set("ecology.depth",vary(.80f,.18f));set("grave.reverb",vary(.46f,.14f));break;
    }
}

bool Grimoire::loadFactory(int index)
{
    const auto manifest=FactoryPresetManifest::create(); if(index<0||index>=(int)manifest.size())return false; commonReset();
    if(index>=FactoryPresetManifest::LegacyPresetCount){const auto&p=manifest[(size_t)index];loadGeneratedFactory(p.archetype,p.seed);return true;}
    switch(index){
      case 0:set("crypt.character",1.f);set("tower.master",.18f);set("crypt.g1.type",8.f);set("crypt.g2.type",9.f);set("crypt.f1.cutoff",.10f);set("crypt.f2.cutoff",.16f);set("possession.haunt",.32f);set("grave.reverb",.48f);break;
      case 1:set("crypt.master",.20f);set("tower.character",1.f);set("tower.g1.type",8.f);set("tower.g2.type",9.f);set("tower.f1.cutoff",.10f);set("tower.f2.cutoff",.46f);set("grave.reverb",.62f);set("grave.cutoff",.44f);break;
      case 2:set("crypt.g1.type",10.f);set("crypt.g2.type",8.f);set("crypt.character",.88f);set("tower.master",.32f);set("rituals.enabled",1.f);set("rituals.pattern",0.f);set("rituals.rate",2.f);set("rituals.gate",.52f);set("possession.bloodFeed",.42f);break;
      case 3:set("crypt.g1.type",9.f);set("tower.g1.type",8.f);set("tower.g2.type",10.f);set("possession.soulExchange",.46f);set("ritual.mode",3.f);set("ritual.mix",.58f);set("grave.reverb",.55f);break;
      case 4:set("crypt.master",.35f);set("tower.g1.type",10.f);set("tower.g2.type",11.f);set("tower.character",.94f);set("ritual.mode",2.f);set("ritual.mix",.70f);set("ritual.feedback",.62f);set("possession.aetherLeak",.48f);break;
      case 5:set("crypt.g1.type",8.f);set("crypt.g2.type",11.f);set("tower.master",.12f);set("possession.haunt",.72f);set("grave.delay",.42f);set("grave.feedback",.58f);set("grave.reverb",.40f);break;
      case 6:set("crypt.master",.14f);set("tower.g1.type",8.f);set("tower.g2.type",9.f);set("tower.g3.type",11.f);set("tower.character",1.f);set("rituals.enabled",1.f);set("rituals.pattern",4.f);set("rituals.rate",3.f);set("rituals.probability",.72f);set("grave.reverb",.68f);break;
      case 7:set("crypt.g1.type",10.f);set("tower.g1.type",8.f);set("tower.g2.type",10.f);set("possession.soulExchange",.72f);set("possession.haunt",.34f);set("ritual.mode",3.f);set("ritual.mix",.68f);set("rituals.enabled",1.f);set("rituals.pattern",5.f);set("rituals.rate",2.f);break;
      case 8:set("crypt.g1.type",8.f);set("crypt.g2.type",8.f);set("crypt.character",1.f);set("tower.master",.08f);set("crypt.f1.cutoff",.065f);set("grave.reverb",.22f);setHex(1,1,6,11,.45f);break;
      case 9:set("crypt.g1.type",9.f);set("crypt.g2.type",8.f);set("crypt.character",.92f);set("tower.master",.16f);setHex(1,6,5,11,.36f);set("grave.reverb",.36f);break;
      case 10:set("crypt.g1.type",10.f);set("crypt.g2.type",10.f);set("crypt.g3.type",8.f);set("tower.master",.12f);set("crypt.f1.resonance",.64f);set("crypt.f2.resonance",.48f);break;
      case 11:set("crypt.g1.type",11.f);set("crypt.g2.type",9.f);set("tower.master",.06f);set("crypt.balance",-.12f);set("grave.cutoff",.10f);set("grave.reverb",.20f);break;
      case 12:set("crypt.g1.type",8.f);set("tower.master",.14f);setHex(1,2,2,1,.62f);set("grave.delay",.18f);break;
      case 13:set("crypt.master",.16f);set("tower.g1.type",9.f);set("tower.g2.type",8.f);set("tower.g3.type",11.f);set("tower.character",1.f);set("grave.reverb",.64f);set("grave.cutoff",.43f);setHex(1,2,2,12,.34f);break;
      case 14:set("crypt.master",.12f);set("tower.g1.type",8.f);set("tower.g2.type",9.f);set("tower.character",.92f);set("grave.reverb",.52f);set("grave.delay",.22f);break;
      case 15:set("crypt.master",.10f);set("tower.g1.type",10.f);set("tower.g2.type",8.f);set("tower.character",.95f);set("possession.aetherLeak",.18f);set("ritual.mix",.34f);break;
      case 16:set("crypt.master",.08f);set("tower.g1.type",9.f);set("tower.g2.type",11.f);setHex(1,5,2,12,.52f);set("grave.reverb",.48f);break;
      case 17:set("crypt.master",.18f);set("tower.g1.type",11.f);set("tower.g2.type",11.f);set("ritual.mode",0.f);set("ritual.mix",.44f);set("ritual.width",.82f);break;
      case 18:set("crypt.g1.type",9.f);set("tower.g1.type",10.f);set("possession.soulExchange",.82f);set("ritual.mode",3.f);set("ritual.mix",.52f);break;
      case 19:set("crypt.g1.type",9.f);set("tower.g1.type",9.f);set("possession.bloodFeed",.62f);set("possession.haunt",.72f);set("grave.reverb",.42f);break;
      case 20:set("crypt.g1.type",10.f);set("tower.g1.type",8.f);set("possession.soulExchange",.48f);set("ritual.mode",3.f);set("ritual.width",.90f);set("grave.reverb",.50f);break;
      case 21:set("crypt.g1.type",8.f);set("tower.g1.type",10.f);set("possession.bloodFeed",.66f);set("possession.aetherLeak",.68f);set("possession.soulExchange",.42f);set("ritual.mix",.46f);break;
      case 22:set("crypt.g1.type",11.f);set("tower.g1.type",9.f);set("possession.haunt",.58f);set("ritual.mode",4.f);set("ritual.mix",.62f);set("ritual.drive",.52f);break;
      case 23:set("crypt.character",.96f);set("grave.feedback",.62f);setHex(1,1,6,11,.54f);setHex(2,1,6,21,.34f);break;
      case 24:set("ritual.drive",.55f);set("ritual.mix",.46f);setHex(1,6,5,9,.12f);setHex(2,6,5,14,.48f);break;
      case 25:set("tower.character",.92f);set("grave.cutoff",.20f);setHex(1,2,2,12,.42f);setHex(2,2,2,16,-.22f);break;
      case 26:set("crypt.g1.type",9.f);set("tower.g1.type",10.f);set("possession.soulExchange",.52f);setHex(1,5,4,6,.58f);setHex(2,5,4,19,-.35f);break;
      case 27:set("ritual.mix",.58f);set("ritual.depth",.62f);set("ritual.feedback",.44f);setHex(1,6,3,13,.58f);setHex(2,6,3,15,.22f);break;
      case 28:set("ritual.mode",1.f);set("ritual.mix",.72f);set("ritual.drive",.62f);set("rituals.enabled",1.f);set("rituals.pattern",6.f);set("rituals.rate",2.f);set("rituals.gate",.38f);set("grave.reverb",.38f);break;
      case 29:set("crypt.g1.type",10.f);set("tower.g1.type",8.f);set("rituals.enabled",1.f);set("rituals.pattern",3.f);set("rituals.rate",1.f);set("rituals.swing",.34f);set("ritual.mix",.42f);set("grave.reverb",.56f);break;
      case 30:set("rituals.enabled",1.f);set("rituals.pattern",3.f);set("rituals.rate",2.f);set("rituals.probability",.76f);setHex(1,5,2,7,.24f);set("grave.delay",.28f);break;
      case 31:set("rituals.enabled",1.f);set("rituals.pattern",1.f);set("rituals.rate",2.f);set("rituals.octaves",3.f);set("possession.aetherLeak",.44f);set("ritual.mode",2.f);set("ritual.mix",.54f);set("grave.reverb",.62f);break;
      case 32:set("crypt.g1.type",9.f);set("crypt.g2.type",9.f);set("tower.master",.14f);set("corpse.position",.62f);set("corpse.rot",.18f);set("corpse.formant",.48f);set("corpse.inharmonic",.06f);set("grave.reverb",.56f);break;
      case 33:set("crypt.g1.type",9.f);set("crypt.g2.type",8.f);set("tower.master",.08f);set("corpse.position",.86f);set("corpse.rot",.28f);set("corpse.formant",-.12f);set("corpse.inharmonic",.04f);set("grave.reverb",.64f);set("grave.cutoff",.18f);break;
      case 34:set("crypt.g1.type",9.f);set("tower.master",.10f);set("corpse.position",.44f);set("corpse.rot",.82f);set("corpse.formant",.18f);set("corpse.inharmonic",.42f);set("grave.delay",.12f);break;
      case 35:set("crypt.g1.type",9.f);set("crypt.g2.type",10.f);set("tower.master",.12f);set("corpse.position",.52f);set("corpse.rot",.94f);set("corpse.formant",0.f);set("corpse.inharmonic",.14f);set("ritual.mode",0.f);set("ritual.mix",.44f);break;
      case 36:set("crypt.g1.type",9.f);set("crypt.g2.type",10.f);set("tower.master",.10f);set("corpse.position",.38f);set("corpse.rot",.34f);set("corpse.formant",-.72f);set("corpse.inharmonic",.09f);set("grave.reverb",.46f);break;
      case 37:set("crypt.g1.type",9.f);set("tower.master",.06f);set("corpse.position",.70f);set("corpse.rot",.26f);set("corpse.formant",.88f);set("corpse.inharmonic",.12f);set("crypt.f1.cutoff",.12f);break;
      case 38:set("crypt.g1.type",9.f);set("tower.g1.type",10.f);set("corpse.position",.58f);set("corpse.rot",.48f);set("corpse.formant",.16f);set("corpse.inharmonic",.88f);set("possession.soulExchange",.34f);set("ritual.mode",3.f);set("ritual.mix",.32f);break;
      case 39:set("crypt.g1.type",9.f);set("crypt.g2.type",9.f);set("tower.master",.20f);set("corpse.position",.50f);set("corpse.rot",.50f);set("corpse.formant",.22f);set("corpse.inharmonic",.32f);set("grave.reverb",.50f);set("grave.delay",.18f);break;
      case 40:set("crypt.g1.type",17.f);set("crypt.g1.level",.72f);set("crypt.g1.shape",.94f);set("crypt.character",.92f);set("tower.g1.type",17.f);set("tower.g1.level",.68f);set("tower.g1.shape",.52f);set("tower.character",.84f);set("ecology.enabled",1.f);set("ecology.depth",.90f);set("grave.reverb",.34f);set("grave.delay",.10f);break;
      case 41:set("crypt.g1.type",16.f);set("crypt.g1.level",.70f);set("crypt.g1.shape",.86f);set("crypt.character",.88f);set("tower.g1.type",16.f);set("tower.g1.level",.66f);set("tower.g1.shape",.72f);set("tower.character",.90f);set("ecology.enabled",1.f);set("ecology.depth",.82f);set("grave.reverb",.42f);break;
      case 42:set("crypt.g1.type",17.f);set("crypt.g2.type",16.f);set("tower.g1.type",17.f);set("tower.g2.type",16.f);set("ecology.enabled",1.f);set("ecology.depth",.76f);set("ritual.mode",3.f);set("ritual.mix",.30f);set("grave.reverb",.48f);break;
      case 43:set("crypt.g1.type",15.f);set("crypt.g2.type",17.f);set("tower.g1.type",15.f);set("tower.g2.type",17.f);set("ecology.enabled",1.f);set("ecology.depth",.62f);set("grave.reverb",.56f);set("grave.delay",.22f);break;
      default:return false;
    } return true;
}

juce::File Grimoire::getUserDirectory() const{auto dir=juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile("SleepFighterStudios").getChildFile("Horror Castle").getChildFile("Grimoire");dir.createDirectory();return dir;}
juce::File Grimoire::saveUserPreset(const juce::String& requestedName) const{auto safe=juce::File::createLegalFileName(requestedName.trim().isEmpty()?"Untitled Spell":requestedName.trim());auto file=getUserDirectory().getNonexistentChildFile(safe,".hcg",false);auto state=apvts.copyState();state.setProperty("stateVersion",CurrentStateVersion,nullptr);state.setProperty("productVersion","1.3.0",nullptr);if(auto xml=state.createXml())xml->writeTo(file);return file;}
bool Grimoire::loadUserPreset(const juce::File& file){if(!file.existsAsFile())return false;if(auto xml=juce::XmlDocument::parse(file)){auto state=juce::ValueTree::fromXml(*xml);migrateState(state);if(state.isValid()&&state.hasType(apvts.state.getType())){apvts.replaceState(state);return true;}}return false;}
void Grimoire::migrateState(juce::ValueTree& state){if(!state.isValid())return;const int version=(int)state.getProperty("stateVersion",13);if(version<CurrentStateVersion)state.setProperty("stateVersion",CurrentStateVersion,nullptr);state.setProperty("productVersion","1.3.0",nullptr);}

} // namespace horrorcastle

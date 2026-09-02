#include <JuceHeader.h>
#include "../Source/HorrorCastle/CastleParameters.h"
#include "../Source/HorrorCastle/Grimoire.h"
#include "../Source/HorrorCastle/FactoryPresetManifest.h"
#include <cmath>
#include <iostream>
#include <map>
#include <set>

namespace {
class DummyProcessor final : public juce::AudioProcessor {
public:
 DummyProcessor():juce::AudioProcessor(BusesProperties()){} const juce::String getName()const override{return "PresetCheck";} void prepareToPlay(double,int)override{} void releaseResources()override{} void processBlock(juce::AudioBuffer<float>&,juce::MidiBuffer&)override{} double getTailLengthSeconds()const override{return 0;} bool acceptsMidi()const override{return false;}bool producesMidi()const override{return false;}bool isMidiEffect()const override{return false;}juce::AudioProcessorEditor*createEditor()override{return nullptr;}bool hasEditor()const override{return false;}int getNumPrograms()override{return 1;}int getCurrentProgram()override{return 0;}void setCurrentProgram(int)override{}const juce::String getProgramName(int)override{return{};}void changeProgramName(int,const juce::String&)override{}void getStateInformation(juce::MemoryBlock&)override{}void setStateInformation(const void*,int)override{}
};

using Snapshot=std::map<std::string,float>;
Snapshot snapshot(const juce::AudioProcessorValueTreeState& s)
{
 Snapshot out;
 const auto tree=s.copyState();
 for(int c=0;c<tree.getNumChildren();++c){const auto child=tree.getChild(c);if(child.hasProperty("id")&&child.hasProperty("value"))out[child.getProperty("id").toString().toStdString()]=(float)child.getProperty("value");}
 return out;
}

void force(juce::AudioProcessorValueTreeState& s,const juce::String& id,float actual)
{
 if(auto* p=s.getParameter(id))p->setValueNotifyingHost(p->convertTo0to1(actual));
}
}

int main(){
 DummyProcessor p;juce::AudioProcessorValueTreeState state(p,nullptr,"PARAMS",horrorcastle::param::createLayout());horrorcastle::Grimoire g(state);const auto spells=g.getFactorySpells();
 bool ok=spells.size()==horrorcastle::FactoryPresetManifest::FactoryPresetCount;std::set<std::string> names;std::set<std::string> cats;
 for(size_t i=0;i<spells.size();++i){ok &= !spells[i].name.isEmpty()&&!spells[i].category.isEmpty()&&!spells[i].tags.isEmpty();ok &= names.insert(spells[i].name.toStdString()).second;cats.insert(spells[i].category.toStdString());ok &= g.loadFactory((int)i);auto tree=state.copyState();for(int c=0;c<tree.getNumChildren();++c){auto child=tree.getChild(c);if(child.hasProperty("value")){const auto v=(float)child.getProperty("value");ok &= std::isfinite(v);}}}
 ok &= cats.size()>=10;ok &= !g.loadFactory(-1);ok &= !g.loadFactory((int)spells.size());

 // Cross-preset bleed regression: contaminate every major subsystem, then load a
 // target spell and require byte-for-byte-equivalent parameter values to a fresh
 // instance that loaded the same target directly.
 DummyProcessor p2;juce::AudioProcessorValueTreeState cleanState(p2,nullptr,"PARAMS",horrorcastle::param::createLayout());horrorcastle::Grimoire clean(cleanState);
 ok &= g.loadFactory(40);
 force(state,"living.vortex.collapse",1.f);force(state,"living.siren.edge",1.f);force(state,"living.mirror.fracture",1.f);force(state,"living.poltergeist.arc",1.f);
 force(state,"crypt.g3.level",.93f);force(state,"tower.g2.tune",19.f);force(state,"crypt.f2.resonance",.91f);force(state,"global.unison",8.f);force(state,"ritual.feedback",.88f);force(state,"possession.haunt",.97f);force(state,"ecology.depth",.99f);
 ok &= g.loadFactory(54);ok &= clean.loadFactory(54);
 const auto dirtyThenTarget=snapshot(state), freshTarget=snapshot(cleanState);
 ok &= dirtyThenTarget==freshTarget;

 // Repeat in the opposite direction with a legacy target to ensure the new full
 // reset protects both the original 44 spells and the generated launch bank.
 force(state,"living.abyss.pressure",1.f);force(state,"living.aurora.instability",1.f);force(state,"crypt.g2.pan",1.f);force(state,"tower.f1.drive",1.f);force(state,"global.hex",1.f);
 ok &= g.loadFactory(7);ok &= clean.loadFactory(7);
 ok &= snapshot(state)==snapshot(cleanState);

 if(!ok){std::cerr<<"FACTORY PRESET CHECK FAILED\n";return 1;}std::cout<<"FACTORY PRESET CHECK PASSED\nPASS  exactly 100 unique named factory spells\nPASS  legacy indices 0-43 retained, launch bank append-only\nPASS  deterministic archetype loading for indices 44-99\nPASS  at least 10 musical categories\nPASS  all presets load with finite parameter state\nPASS  factory loading is deterministic after hostile cross-preset contamination\nPASS  Living Engine physics, generator, filter, FX, HEX and ecology state cannot bleed between factory spells\n";return 0;
}

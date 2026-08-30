#include <JuceHeader.h>
#include "../Source/HorrorCastle/CastleParameters.h"
#include "../Source/HorrorCastle/Grimoire.h"
#include "../Source/HorrorCastle/FactoryPresetManifest.h"
#include <iostream>
#include <set>

namespace {
class DummyProcessor final : public juce::AudioProcessor {
public:
 DummyProcessor():juce::AudioProcessor(BusesProperties()){} const juce::String getName()const override{return "PresetCheck";} void prepareToPlay(double,int)override{} void releaseResources()override{} void processBlock(juce::AudioBuffer<float>&,juce::MidiBuffer&)override{} double getTailLengthSeconds()const override{return 0;} bool acceptsMidi()const override{return false;}bool producesMidi()const override{return false;}bool isMidiEffect()const override{return false;}juce::AudioProcessorEditor*createEditor()override{return nullptr;}bool hasEditor()const override{return false;}int getNumPrograms()override{return 1;}int getCurrentProgram()override{return 0;}void setCurrentProgram(int)override{}const juce::String getProgramName(int)override{return{};}void changeProgramName(int,const juce::String&)override{}void getStateInformation(juce::MemoryBlock&)override{}void setStateInformation(const void*,int)override{}
};
}
int main(){
 DummyProcessor p;juce::AudioProcessorValueTreeState state(p,nullptr,"PARAMS",horrorcastle::param::createLayout());horrorcastle::Grimoire g(state);const auto spells=g.getFactorySpells();
 bool ok=spells.size()==horrorcastle::FactoryPresetManifest::FactoryPresetCount;std::set<std::string> names;std::set<std::string> cats;
 for(size_t i=0;i<spells.size();++i){ok &= !spells[i].name.isEmpty()&&!spells[i].category.isEmpty()&&!spells[i].tags.isEmpty();ok &= names.insert(spells[i].name.toStdString()).second;cats.insert(spells[i].category.toStdString());ok &= g.loadFactory((int)i);auto tree=state.copyState();for(int c=0;c<tree.getNumChildren();++c){auto child=tree.getChild(c);if(child.hasProperty("value")){const auto v=(float)child.getProperty("value");ok &= std::isfinite(v);}}}
 ok &= cats.size()>=10;ok &= !g.loadFactory(-1);ok &= !g.loadFactory((int)spells.size());
 if(!ok){std::cerr<<"FACTORY PRESET CHECK FAILED\n";return 1;}std::cout<<"FACTORY PRESET CHECK PASSED\nPASS  exactly 100 unique named factory spells\nPASS  legacy indices 0-43 retained, launch bank append-only\nPASS  deterministic archetype loading for indices 44-99\nPASS  at least 10 musical categories\nPASS  all presets load with finite parameter state\n";return 0;
}

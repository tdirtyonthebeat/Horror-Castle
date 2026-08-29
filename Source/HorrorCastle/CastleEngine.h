#pragma once
#include <JuceHeader.h>
#include "HorrorCastleArchitecture.h"
#include "CastleParameters.h"
#include "CurseMatrix.h"
#include "RitualEngine.h"
#include "GraveChamber.h"
#include "PossessionEngine.h"
#include "RitualsEngine.h"
#include "SpectralCorpseEngine.h"
#include "RitualFMEngine.h"
#include "BoneResonatorEngine.h"
#include "WraithBreathEngine.h"
#include "ReliquaryEngine.h"
#include "CoffinBodyEngine.h"
#include "ChoirBodyEngine.h"
#include "MarrowEngine.h"
#include "OrreryEngine.h"
#include "AbyssWaveguideEngine.h"
#include "MirrorSpectralEngine.h"
#include "PoltergeistEngine.h"
#include "AuroraEngine.h"
#include "VortexEngine.h"
#include "SirenEngine.h"
#include "CreatureRoutingMatrix.h"
#include <array>
#include <cmath>
#include <atomic>
namespace horrorcastle {
class CastleEngine {
public:
 static constexpr int MaxVoices=32;
 CastleEngine(); void prepare(double,int); void reset(); void setParameters(const juce::AudioProcessorValueTreeState&); void render(juce::AudioBuffer<float>&,juce::MidiBuffer&);
 float getHexLaneActivity(int index) const noexcept { return (index >= 0 && index < CurseMatrix::Lanes) ? hexLaneTelemetry[(size_t)index].load(std::memory_order_relaxed) : 0.f; }
 float getHexDestinationValue(int index) const noexcept { return (index >= 0 && index < CurseMatrix::Destinations) ? hexDestinationTelemetry[(size_t)index].load(std::memory_order_relaxed) : 0.f; }
private:
 enum CreatureEndpoint : std::uint8_t { VortexCreature=0, SirenCreature=1, PoltergeistCreature=2, AuroraCreature=3 };
 struct Env { enum Stage{Off,Attack,Decay,Sustain,Release}; Stage stage=Off; float value=0; float attack=.008f,decay=.35f,sustain=.72f,release=.25f; void on(){stage=Attack;} void off(){stage=Release;} float next(double sr){ if(stage==Attack){value+=1.f/(attack*sr);if(value>=1){value=1;stage=Decay;}} else if(stage==Decay){value+=(sustain-value)/(decay*sr);if(std::abs(value-sustain)<1e-4f){value=sustain;stage=Sustain;}} else if(stage==Release){value-=value/(release*sr);if(value<1e-5f){value=0;stage=Off;}} return value;} };
 struct Voice {
  bool active=false,releasing=false; int note=0; float pitchNote=60.f; float velocity=0;
  float pa=0,pb=0,pc=0;
  float cfa=0,cfb=0,tfa=0,tfb=0;
  float cryptBody=0,towerBody=0;
  float cryptWander=0,towerWander=0;
  float cryptDetune=0,towerDetune=0;
  float cryptSubPhase=0, cryptAbyssPhase=0;
  float towerBellPhaseA=0, towerBellPhaseB=0;
  std::array<float,3> cryptAux1{}, cryptAux2{};
  std::array<SpectralCorpseEngine::VoiceState,3> cryptCorpse{};
  std::array<RitualFMEngine::VoiceState,3> cryptRitualFM{};
  std::array<BoneResonatorEngine::VoiceState,3> cryptBone{};
  std::array<WraithBreathEngine::VoiceState,3> cryptWraith{};
  std::array<CoffinBodyEngine::VoiceState,3> cryptCoffin{};
  std::array<MarrowEngine::VoiceState,3> cryptMarrow{};
  std::array<AbyssWaveguideEngine::VoiceState,3> cryptAbyss{};
  std::array<PoltergeistEngine::VoiceState,3> cryptPoltergeist{};
  std::array<VortexEngine::VoiceState,3> cryptVortex{};
  std::array<float,3> towerAux1{}, towerAux2{};
  std::array<RitualFMEngine::VoiceState,3> towerRitualFM{};
  std::array<ReliquaryEngine::VoiceState,3> towerReliquary{};
  std::array<ChoirBodyEngine::VoiceState,3> towerChoir{};
  std::array<OrreryEngine::VoiceState,3> towerOrrery{};
  std::array<MirrorSpectralEngine::VoiceState,3> towerMirror{};
  std::array<AuroraEngine::VoiceState,3> towerAurora{};
  std::array<SirenEngine::VoiceState,3> towerSiren{};
  CreatureRoutingMatrix ecology;
  CreatureRoutingMatrix::StateArray ecologySnapshot{}, ecologyInbox{};
  float polyPressure=0.f;
  Env amp,iron;
 };
 std::array<Voice,MaxVoices> voices; double sr=44100; int blockSize=512; float master=.72f; CastlePatchArchitecture patch{};
 CurseMatrix hex;
 RitualEngine ritual; RitualEngine::Parameters ritualParams{};
 std::array<std::atomic<float>, CurseMatrix::Lanes> hexLaneTelemetry{};
 std::array<std::atomic<float>, CurseMatrix::Destinations> hexDestinationTelemetry{};
 juce::dsp::DelayLine<float> delay { 48000 };
 float delayMix=.18f; float delayFeedback=.28f; float delayTimeSamples=13230.f;
 GraveChamber grave; float graveTone=.32f;
 PossessionEngine possession; PossessionEngine::Parameters possessionParams{};
 RitualsEngine rituals; RitualsEngine::Parameters ritualsParams{};
 SpectralCorpseEngine spectralCorpse;
 RitualFMEngine ritualFM;
 BoneResonatorEngine boneResonator;
 WraithBreathEngine wraith;
 ReliquaryEngine reliquary;
 CoffinBodyEngine coffin;
 ChoirBodyEngine choir;
 MarrowEngine marrow;
 OrreryEngine orrery;
 AbyssWaveguideEngine abyss;
 MirrorSpectralEngine mirror;
 PoltergeistEngine poltergeist;
 AuroraEngine aurora;
 VortexEngine vortex;
 SirenEngine siren;
 float corpsePosition=.34f, corpseRot=.22f, corpseFormant=0.f, corpseInharmonic=.08f;
 bool ecologyEnabled=false; float ecologyDepth=.65f;
 float modWheel=0.f, channelPressure=0.f, pitchBendSemitones=0.f;
 float globalGlide=0.f; int globalUnison=1; float lastPlayedNote=60.f;
 uint32_t rng=0xA341316Cu;
 void configureEcology(Voice&) noexcept;
 void on(int,int);void off(int);Voice* steal();float osc(GeneratorType,float,float,float);float rnd();float filter(float,float&,const FilterCell&,float);void scene(Voice&,const SceneArchitecture&,float&,float&,float,const std::array<float, CurseMatrix::Destinations>&,bool);
};}

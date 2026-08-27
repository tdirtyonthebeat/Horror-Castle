#pragma once
#include <array>
#include <cstdint>

namespace horrorcastle {

enum class GeneratorType : uint8_t { VA, Wavetable, FM, PM, Vector, Chip, Noise, Resonator, ChamberI, ChamberII, ChamberIII, ChamberIV, ChamberV, ChamberVI, ChamberVII };
enum class FilterType : uint8_t { LowPass, HighPass, BandPass, Notch, Comb, Formant, Diode, K35, Shaper };
enum class Route : uint8_t { Serial, Parallel, Split, Crossfeed };

enum class ModSource : uint8_t {
    Velocity, KeyTrack, ModWheel, Aftertouch, PitchEnvelope,
    BloodEnvelope, IronEnvelope, WraithLFO, DreadLFO, Random, Gate
};

enum class ModDestination : uint8_t {
    Pitch, LevelA, LevelB, LevelC, CutoffA, CutoffB,
    ResonanceA, ResonanceB, Drive, Pan, FMDepth, WavetablePosition
};

struct GeneratorSlot {
    GeneratorType type = GeneratorType::VA;
    float level = 0.8f;
    float pan = 0.0f;
    float tune = 0.0f;
    float spread = 0.0f;
    float shape = 0.5f;
    bool enabled = true;
};

struct FilterCell {
    FilterType type = FilterType::LowPass;
    float cutoff = 0.65f;
    float resonance = 0.15f;
    float drive = 0.0f;
    bool enabled = true;
};

struct ModRoute {
    ModSource source;
    ModDestination destination;
    float amount = 0.0f;
    bool bipolar = false;
};

struct VoiceArchitecture {
    std::array<GeneratorSlot, 3> generators{};
    GeneratorSlot noise;
    std::array<FilterCell, 2> filters{};
    Route filterRoute = Route::Serial;
    std::array<ModRoute, 12> modulation{};
    float master = 0.8f;
    float glide = 0.0f;
    int unison = 1;
};

struct SceneArchitecture {
    VoiceArchitecture voice;
    bool crossSceneFM = false;
    bool crossSceneRing = false;
    float sceneBalance = 0.0f;
    float character = 0.5f;
};

struct CastlePatchArchitecture {
    SceneArchitecture crypt;
    SceneArchitecture tower;
    float ritualMix = 0.0f;
    float ritualDepth = 0.35f;
    float hexAmount = 1.0f;
    float graveDepth = 0.35f;
};

} // namespace horrorcastle

#pragma once
#include <JuceHeader.h>
#include <array>

namespace horrorcastle {

// CHOIR is a synthetic congregation, not a sample player. Four independently
// drifting vocal-tract bodies receive a harmonic exciter and exchange a small
// amount of energy so expression can change the ensemble's physical alignment.
class ChoirBodyEngine
{
public:
    static constexpr int Mouths = 4;
    static constexpr int Formants = 3;

    struct ResonatorState
    {
        float y1 = 0.0f;
        float y2 = 0.0f;
    };

    struct MouthState
    {
        std::array<ResonatorState, Formants> formants {};
        float driftPhase = 0.0f;
        float breath = 0.0f;
        float feedback = 0.0f;
    };

    struct VoiceState
    {
        std::array<MouthState, Mouths> mouths {};
        float exciterPhase = 0.0f;
        float congregation = 0.0f;
        uint32_t rng = 0xC401F00Du;
    };

    float renderSample(VoiceState& state,
                       float fundamentalHz,
                       float vowel,
                       float aether,
                       float expression,
                       float velocity,
                       double sampleRate) noexcept;
};

} // namespace horrorcastle

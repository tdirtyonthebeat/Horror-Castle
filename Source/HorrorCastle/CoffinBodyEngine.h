#pragma once
#include <JuceHeader.h>
#include <array>
#include <cstdint>

namespace horrorcastle {

// COFFIN is a nonlinear wooden enclosure: a vibrating lid and body exchange
// energy with enclosed-air and sympathetic-string modes. Controls change the
// virtual geometry and coupling rather than acting as ordinary EQ parameters.
class CoffinBodyEngine
{
public:
    static constexpr int LidModes = 5;
    static constexpr int CavityModes = 4;
    static constexpr int StringModes = 3;

    struct ResonatorState
    {
        float y1 = 0.0f;
        float y2 = 0.0f;
    };

    struct VoiceState
    {
        std::array<ResonatorState, LidModes> lid {};
        std::array<ResonatorState, CavityModes> cavity {};
        std::array<ResonatorState, StringModes> strings {};
        float lidFeedback = 0.0f;
        float cavityFeedback = 0.0f;
        float stringFeedback = 0.0f;
        float strike = 1.0f;
        float scrape = 0.0f;
        float creakPhase = 0.0f;
        uint32_t rng = 0xC0FF1A5Eu;
    };

    float renderSample(VoiceState& state,
                       float fundamentalHz,
                       float lidOpen,
                       float dread,
                       float expression,
                       float velocity,
                       double sampleRate) noexcept;
};

} // namespace horrorcastle

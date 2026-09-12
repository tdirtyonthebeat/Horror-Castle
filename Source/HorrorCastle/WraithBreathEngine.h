#pragma once
#include <JuceHeader.h>
#include <array>
#include <cstdint>

namespace horrorcastle {

// WRAITH is a breath-excited coupled body rather than a conventional oscillator.
// A noisy air column continuously excites a stretched membrane bank; membrane
// energy feeds back into odd air-column resonances so pressure changes the object
// itself, not merely its level.
class WraithBreathEngine
{
public:
    static constexpr int MembraneModes = 6;
    static constexpr int AirModes = 3;

    struct ResonatorState
    {
        float y1 = 0.0f;
        float y2 = 0.0f;
    };

    struct VoiceState
    {
        std::array<ResonatorState, MembraneModes> membrane {};
        std::array<ResonatorState, AirModes> air {};
        float breathLowpass = 0.0f;
        float membraneFeedback = 0.0f;
        float airFeedback = 0.0f;
        float pressure = 0.0f;
        float flutterPhase = 0.0f;
        uint32_t rng = 0x2468ACE1u;
    };

    float renderSample(VoiceState& state,
                       float fundamentalHz,
                       float veil,
                       float haunt,
                       float expression,
                       float velocity,
                       double sampleRate) noexcept;
};

} // namespace horrorcastle

#pragma once
#include <JuceHeader.h>
#include <array>
#include <cstdint>

namespace horrorcastle {

// MARROW: bowed stick/slip excitation coupled into string-like modes and a
// viscous internal body. Pressure changes the friction regime itself rather
// than acting as a post-amplitude control.
class MarrowEngine
{
public:
    static constexpr int StringModes = 5;
    static constexpr int FluidModes = 3;

    struct ResonatorState { float y1=0.0f, y2=0.0f; };
    struct VoiceState {
        std::array<ResonatorState,StringModes> strings{};
        std::array<ResonatorState,FluidModes> fluid{};
        float bowPhase=0.0f;
        float bowMemory=0.0f;
        float stringFeedback=0.0f;
        float fluidFeedback=0.0f;
        uint32_t rng=0x4D415252u;
    };

    float renderSample(VoiceState& state,
                       float fundamentalHz,
                       float viscosity,
                       float dread,
                       float expression,
                       float velocity,
                       double sampleRate) noexcept;
};

} // namespace horrorcastle

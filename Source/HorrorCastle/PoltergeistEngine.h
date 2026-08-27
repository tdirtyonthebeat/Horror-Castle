#pragma once
#include <JuceHeader.h>
#include <array>
#include <cstdint>

namespace horrorcastle {

// POLTERGEIST: four charged plates whose electrostatic attraction/repulsion
// bends their resonant frequencies. Large potential differences cross a
// bounded discharge threshold and create short arc impulses.
class PoltergeistEngine
{
public:
    static constexpr int Plates = 4;

    struct PlateState {
        float phase = 0.0f;
        float charge = 0.0f;
        float memory = 0.0f;
    };

    struct VoiceState {
        std::array<PlateState, Plates> plates{};
        float field = 0.0f;
        float arcEnvelope = 0.0f;
        uint32_t rng = 0x504F4C54u;
    };

    float renderSample(VoiceState& state,
                       float fundamentalHz,
                       float charge,
                       float dread,
                       float expression,
                       float velocity,
                       double sampleRate) noexcept;
};

} // namespace horrorcastle

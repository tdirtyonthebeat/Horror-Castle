#pragma once
#include <JuceHeader.h>
#include "CreatureStateBus.h"
#include <array>

namespace horrorcastle {

// AURORA: five charged resonant rings in a shared electrostatic field.
// Charge is redistributed continuously between rings, smoothly bending
// frequency and phase relationships rather than crossing a discharge boundary.
class AuroraEngine
{
public:
    static constexpr int Rings = 5;

    struct RingState {
        float phase = 0.0f;
        float fieldPhase = 0.0f;
        float charge = 0.0f;
        float memory = 0.0f;
    };

    struct VoiceState {
        std::array<RingState, Rings> rings{};
        float sharedField = 0.0f;
        float halo = 0.0f;
        float ecologicalField = 0.0f;
        CreatureStateBus::State creatureState{};
    };

    float renderSample(VoiceState& state,
                       float fundamentalHz,
                       float fieldStrength,
                       float aether,
                       float expression,
                       float velocity,
                       double sampleRate,
                       float externalField=0.0f,
                       float fieldCoupling=0.0f) noexcept;

    static const CreatureStateBus::State& stateBus(const VoiceState& state) noexcept { return state.creatureState; }
};

} // namespace horrorcastle

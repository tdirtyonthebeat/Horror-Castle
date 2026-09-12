#pragma once
#include <JuceHeader.h>
#include "CreatureStateBus.h"
#include <array>

namespace horrorcastle {

class SirenEngine
{
public:
    static constexpr int Jets = 4;
    struct JetState { float phase=0.0f; float displacement=0.0f; float edgeMemory=0.0f; };
    struct VoiceState {
        std::array<JetState,Jets> jets{};
        float plenum=0.0f;
        float field=0.0f;
        float ecologicalImpulse=0.0f;
        CreatureStateBus::State creatureState{};
    };

    float renderSample(VoiceState& state,
                       float fundamentalHz,
                       float aperture,
                       float aether,
                       float expression,
                       float velocity,
                       double sampleRate,
                       float externalEvent=0.0f,
                       float eventCoupling=0.0f) noexcept;

    static const CreatureStateBus::State& stateBus(const VoiceState& state) noexcept { return state.creatureState; }
};

} // namespace horrorcastle

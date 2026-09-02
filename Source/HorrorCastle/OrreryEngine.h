#pragma once
#include <JuceHeader.h>
#include <array>

namespace horrorcastle {

class OrreryEngine
{
public:
    static constexpr int Bodies = 5;
    struct BodyState { float phase=0.0f; float orbitPhase=0.0f; float memory=0.0f; };
    struct VoiceState { std::array<BodyState,Bodies> bodies{}; float field=0.0f; float barycenter=0.0f; };

    float renderSample(VoiceState& state,
                       float fundamentalHz,
                       float orbitDepth,
                       float aether,
                       float expression,
                       float velocity,
                       double sampleRate) noexcept;
};

} // namespace horrorcastle

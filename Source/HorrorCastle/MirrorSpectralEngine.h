#pragma once
#include <JuceHeader.h>
#include <array>

namespace horrorcastle {

class MirrorSpectralEngine
{
public:
    static constexpr int Rays = 7;
    struct RayState { float phase = 0.0f; float memory = 0.0f; };
    struct VoiceState { std::array<RayState, Rays> rays {}; float field = 0.0f; float precession = 0.0f; };

    float renderSample(VoiceState& state,
                       float fundamentalHz,
                       float reflection,
                       float aether,
                       float expression,
                       float velocity,
                       double sampleRate) noexcept;
};

} // namespace horrorcastle

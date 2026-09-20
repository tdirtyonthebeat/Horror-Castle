#pragma once
#include <JuceHeader.h>
#include <array>

namespace horrorcastle {

// RELIQUARY is TOWER Chamber V: a sympathetic glass/cavity body.
// Sparse upper resonances excite three linked cavities; AETHER changes how
// strongly those cavities exchange energy, while APERTURE changes where the
// body is excited/read and therefore which modes survive.
class ReliquaryEngine
{
public:
    static constexpr int GlassModes = 8;
    static constexpr int CavityModes = 3;

    struct ResonatorState
    {
        float y1 = 0.0f;
        float y2 = 0.0f;
    };

    struct VoiceState
    {
        std::array<ResonatorState, GlassModes> glass {};
        std::array<ResonatorState, CavityModes> cavity {};
        float glassFeedback = 0.0f;
        float cavityFeedback = 0.0f;
        float shimmerPhase = 0.0f;
        float exciterPhase = 0.0f;
    };

    float renderSample(VoiceState& state,
                       float fundamentalHz,
                       float aperture,
                       float aether,
                       float expression,
                       float velocity,
                       double sampleRate) noexcept;
};

} // namespace horrorcastle

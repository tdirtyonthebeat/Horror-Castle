#pragma once
#include <JuceHeader.h>
#include <array>

namespace horrorcastle {

// Living Engines v1.3: an independent, continuously morphable four-operator
// FM topology. It intentionally avoids fixed DX-style algorithm tables. Shape
// moves through chain -> branching -> dual-carrier networks while chamber
// identity and performance expression continuously alter ratios and index.
class RitualFMEngine
{
public:
    struct VoiceState
    {
        std::array<float, 4> phase {};
        std::array<float, 4> previous {};
        float motionPhase = 0.0f;
    };

    float renderSample(VoiceState& state,
                       float fundamentalHz,
                       float topology,
                       float character,
                       float expression,
                       bool crypt,
                       double sampleRate) noexcept;
};

} // namespace horrorcastle

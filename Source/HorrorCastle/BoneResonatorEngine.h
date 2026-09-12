#pragma once
#include <JuceHeader.h>
#include <array>
#include <cstdint>

namespace horrorcastle {

// Independent modal resonator for CRYPT's Bone family.
// The design uses a bank of damped second-order modes with continuously
// stretched partial spacing, position-dependent excitation/readout, and
// performance-controlled damping. It is intentionally not a port of any
// reference synth implementation.
class BoneResonatorEngine
{
public:
    static constexpr int MaxModes = 10;

    struct ModeState
    {
        float y1 = 0.0f;
        float y2 = 0.0f;
    };

    struct VoiceState
    {
        std::array<ModeState, MaxModes> modes {};
        float strike = 1.0f;
        float drivePhase = 0.0f;
        uint32_t rng = 0x13579BDFu;
    };

    float renderSample(VoiceState& state,
                       float fundamentalHz,
                       float material,
                       float character,
                       float expression,
                       float velocity,
                       double sampleRate) noexcept;
};

} // namespace horrorcastle

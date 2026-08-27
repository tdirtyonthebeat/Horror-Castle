#pragma once
#include <JuceHeader.h>
#include <array>
#include <cstdint>

namespace horrorcastle {

class VortexEngine
{
public:
    static constexpr int Cells = 4;
    struct CellState { float phase=0.0f; float pressure=0.0f; float flow=0.0f; float vortex=0.0f; };
    struct VoiceState {
        std::array<CellState,Cells> cells{};
        float cavity=0.0f;
        float collapse=0.0f;
        uint32_t rng=0x564F5254u;
    };

    float renderSample(VoiceState& state,
                       float fundamentalHz,
                       float turbulence,
                       float dread,
                       float expression,
                       float velocity,
                       double sampleRate) noexcept;
};

} // namespace horrorcastle

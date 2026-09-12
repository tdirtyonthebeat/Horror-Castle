#pragma once
#include <JuceHeader.h>
#include <array>
#include <cstdint>

namespace horrorcastle {

class AbyssWaveguideEngine
{
public:
    static constexpr int Tunnels = 4;
    static constexpr int MaxDelay = 2048;

    struct TunnelState
    {
        std::array<float, MaxDelay> buffer {};
        int writeIndex = 0;
        float wall = 0.0f;
    };

    struct VoiceState
    {
        std::array<TunnelState, Tunnels> tunnels {};
        float impulse = 1.0f;
        float junction = 0.0f;
        float rumble = 0.0f;
        uint32_t rng = 0xAB155001u;
    };

    float renderSample(VoiceState& state,
                       float fundamentalHz,
                       float depth,
                       float dread,
                       float expression,
                       float velocity,
                       double sampleRate) noexcept;
};

} // namespace horrorcastle

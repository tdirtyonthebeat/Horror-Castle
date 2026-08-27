#include "AbyssWaveguideEngine.h"
#include <cmath>

namespace horrorcastle {
namespace {
float noise(uint32_t& s) noexcept
{
    s ^= s << 13; s ^= s >> 17; s ^= s << 5;
    return (float) s / 2147483648.0f - 1.0f;
}
}

float AbyssWaveguideEngine::renderSample(VoiceState& s,
                                         float fundamentalHz,
                                         float depth,
                                         float dread,
                                         float expression,
                                         float velocity,
                                         double sr) noexcept
{
    if (sr <= 1.0 || fundamentalHz <= 0.0f) return 0.0f;
    depth = juce::jlimit(0.0f, 1.0f, depth);
    dread = juce::jlimit(0.0f, 1.0f, dread);
    expression = juce::jlimit(0.0f, 1.0f, expression);
    velocity = juce::jlimit(0.0f, 1.0f, velocity);

    constexpr std::array<float, Tunnels> lengthRatios { 0.53f, 0.79f, 1.17f, 1.61f };
    std::array<float, Tunnels> exits {};
    const float basePeriod = (float) sr / fundamentalHz;
    const float lengthScale = 0.72f + 0.96f * depth;

    for (int i = 0; i < Tunnels; ++i)
    {
        auto& t = s.tunnels[(size_t) i];
        const int delaySamples = juce::jlimit(8, MaxDelay - 1,
            (int) std::lround(basePeriod * lengthRatios[(size_t) i] * lengthScale));
        int readIndex = t.writeIndex - delaySamples;
        if (readIndex < 0) readIndex += MaxDelay;
        exits[(size_t) i] = t.buffer[(size_t) readIndex];
    }

    float mean = 0.0f;
    for (const auto x : exits) mean += x;
    mean *= 0.25f;
    s.junction += (0.015f + 0.055f * (1.0f - dread)) * (mean - s.junction);

    const float n = noise(s.rng);
    s.rumble += (0.0015f + 0.005f * (1.0f - dread)) * (n - s.rumble);
    const float onset = s.impulse * (0.06f + 0.18f * velocity);
    s.impulse *= 0.9992f - 0.00035f * depth;
    const float breath = s.rumble * (0.006f + 0.028f * expression);
    const float feedback = juce::jlimit(0.0f, 0.965f,
        0.72f + 0.15f * dread + 0.055f * expression);

    float wallSum = 0.0f;
    for (int i = 0; i < Tunnels; ++i)
    {
        auto& t = s.tunnels[(size_t) i];
        const float wallRate = 0.015f + 0.16f * (1.0f - dread) / (float) (i + 1);
        t.wall += wallRate * (exits[(size_t) i] - t.wall);
        wallSum += t.wall;

        const float scattered = 2.0f * mean - exits[(size_t) i];
        const float neighbor = exits[(size_t) ((i + 1) % Tunnels)];
        const float coupling = (0.04f + 0.15f * depth) * neighbor;
        const float excitation = (onset / (float) (i + 1)) + breath;
        const float next = excitation
                         + scattered * feedback * (0.82f - 0.08f * (float) i)
                         + coupling * (0.35f + 0.45f * expression)
                         + s.junction * (0.025f + 0.085f * dread);

        t.buffer[(size_t) t.writeIndex] = std::tanh(next);
        if (++t.writeIndex >= MaxDelay) t.writeIndex = 0;
    }

    wallSum *= 0.25f;
    const float cave = mean * (0.48f + 0.30f * expression)
                     + wallSum * (0.52f + 0.36f * dread)
                     + s.junction * (0.20f + 0.30f * depth);
    return juce::jlimit(-1.0f, 1.0f, std::tanh(cave * 1.9f));
}

} // namespace horrorcastle

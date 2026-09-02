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

    // ABYSS must feel like changing the dimensions and pressure of an underground
    // network, not like turning a generic tone knob. Depth stretches the tunnels,
    // Dread hardens the walls and raises pressure memory, expression pushes air.
    constexpr std::array<float, Tunnels> lengthRatios { 0.41f, 0.71f, 1.23f, 1.93f };
    std::array<float, Tunnels> exits {};
    const float basePeriod = (float) sr / fundamentalHz;
    const float lengthScale = 0.58f + 1.42f * depth * depth;

    for (int i = 0; i < Tunnels; ++i)
    {
        auto& t = s.tunnels[(size_t) i];
        const float asym = 1.0f + ((i & 1) ? 0.11f : -0.07f) * dread;
        const int delaySamples = juce::jlimit(8, MaxDelay - 1,
            (int) std::lround(basePeriod * lengthRatios[(size_t) i] * lengthScale * asym));
        int readIndex = t.writeIndex - delaySamples;
        if (readIndex < 0) readIndex += MaxDelay;
        exits[(size_t) i] = t.buffer[(size_t) readIndex];
    }

    float mean = 0.0f;
    for (const auto x : exits) mean += x;
    mean *= 0.25f;
    const float junctionRate = 0.006f + 0.070f * (1.0f - dread);
    s.junction += junctionRate * (mean - s.junction);

    const float n = noise(s.rng);
    s.rumble += (0.0007f + 0.004f * (1.0f - depth)) * (n - s.rumble);
    const float onset = s.impulse * (0.05f + 0.24f * velocity) * (0.55f + 0.45f * expression);
    s.impulse *= 0.99945f - 0.00045f * depth;
    const float breath = s.rumble * (0.004f + 0.050f * expression) * (0.6f + 0.8f * depth);
    const float feedback = juce::jlimit(0.0f, 0.982f,
        0.68f + 0.21f * dread + 0.060f * expression + 0.020f * depth);

    float wallSum = 0.0f;
    float pressure = 0.0f;
    for (int i = 0; i < Tunnels; ++i)
    {
        auto& t = s.tunnels[(size_t) i];
        const float wallRate = 0.006f + 0.18f * (1.0f - dread) / (float) (i + 1);
        t.wall += wallRate * (exits[(size_t) i] - t.wall);
        wallSum += t.wall;
        pressure += std::abs(exits[(size_t)i] - t.wall);

        const float scattered = 2.0f * mean - exits[(size_t) i];
        const float neighbor = exits[(size_t) ((i + 1) % Tunnels)];
        const float opposite = exits[(size_t) ((i + 2) % Tunnels)];
        const float coupling = (0.025f + 0.24f * depth * depth) * neighbor
                             - (0.01f + 0.07f * dread) * opposite;
        const float excitation = onset / (float) (i + 1) + breath;
        const float compression = std::tanh((s.junction + mean) * (0.6f + 2.8f * dread));
        const float next = excitation
                         + scattered * feedback * (0.86f - 0.07f * (float) i)
                         + coupling * (0.32f + 0.58f * expression)
                         + compression * (0.02f + 0.14f * dread);

        t.buffer[(size_t) t.writeIndex] = std::tanh(next * (1.0f + 0.32f * dread));
        if (++t.writeIndex >= MaxDelay) t.writeIndex = 0;
    }

    wallSum *= 0.25f;
    pressure *= 0.25f;
    const float subPressure = std::tanh((mean - wallSum) * (1.1f + 2.2f * depth));
    const float cave = mean * (0.38f + 0.32f * expression)
                     + wallSum * (0.48f + 0.48f * dread)
                     + s.junction * (0.16f + 0.42f * depth)
                     + subPressure * (0.08f + 0.30f * depth * dread)
                     + s.rumble * (0.03f + 0.12f * pressure);
    const float out = std::tanh(cave * (1.55f + 0.70f * dread));
    if (!std::isfinite(out)) { s = VoiceState{}; return 0.0f; }
    return juce::jlimit(-1.0f, 1.0f, out);
}

} // namespace horrorcastle

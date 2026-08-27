#include "MirrorSpectralEngine.h"
#include <cmath>

namespace horrorcastle {
namespace {
float wrap01(float x) noexcept { x -= std::floor(x); return x < 0.0f ? x + 1.0f : x; }
}

float MirrorSpectralEngine::renderSample(VoiceState& s,
                                         float fundamentalHz,
                                         float reflection,
                                         float aether,
                                         float expression,
                                         float velocity,
                                         double sr) noexcept
{
    if (sr <= 1.0 || fundamentalHz <= 0.0f) return 0.0f;
    reflection = juce::jlimit(0.0f, 1.0f, reflection);
    aether = juce::jlimit(0.0f, 1.0f, aether);
    expression = juce::jlimit(0.0f, 1.0f, expression);
    velocity = juce::jlimit(0.0f, 1.0f, velocity);

    constexpr float T = juce::MathConstants<float>::twoPi;
    constexpr std::array<float, Rays> baseRatios { 1.0f, 1.497f, 2.071f, 2.731f, 3.887f, 5.173f, 7.391f };
    constexpr std::array<float, Rays> phaseSeeds { 0.03f, 0.19f, 0.37f, 0.52f, 0.68f, 0.81f, 0.94f };

    s.precession = wrap01(s.precession + (0.018f + 0.115f * aether) / (float) sr);
    const float pivot = 1.35f + 4.65f * reflection;
    float sum = 0.0f;
    float recursive = s.field;

    for (int i = 0; i < Rays; ++i)
    {
        const float original = baseRatios[(size_t) i];
        const float reflected = (pivot * pivot) / juce::jmax(0.35f, original);
        const float logRatio = (1.0f - reflection) * std::log(original)
                             + reflection * std::log(reflected);
        const float drift = 1.0f + (0.0007f + 0.0032f * aether)
                                  * std::sin(T * (s.precession * (0.43f + 0.11f * (float) i)
                                                + phaseSeeds[(size_t) i]));
        const float frequency = fundamentalHz * std::exp(logRatio) * drift;
        if (frequency <= 0.0f || frequency >= sr * 0.46) continue;

        auto& ray = s.rays[(size_t) i];
        ray.phase = wrap01(ray.phase + frequency / (float) sr);
        const float recursionDepth = (0.08f + 0.42f * aether) * (0.25f + 0.75f * expression);
        const float phaseWarp = recursive * recursionDepth * (0.65f + 0.18f * (float) i);
        const float y = std::sin(T * ray.phase + phaseWarp + T * phaseSeeds[(size_t) i] * reflection);
        ray.memory += (0.006f + 0.022f * (1.0f - reflection)) * (y - ray.memory);

        const float polarity = (i & 1) ? -1.0f : 1.0f;
        const float weight = polarity / std::sqrt((float) i + 1.0f);
        const float contribution = y * weight + ray.memory * weight * (0.12f + 0.34f * reflection);
        sum += contribution;
        recursive = std::tanh(recursive * 0.72f + contribution * (0.18f + 0.24f * aether));
    }

    s.field = std::tanh(sum * (0.23f + 0.10f * reflection) + recursive * (0.28f + 0.28f * aether));
    const float dryAnchor = std::sin(T * s.rays[0].phase) * (0.10f + 0.12f * velocity);
    const float body = dryAnchor + sum * (0.19f + 0.10f * velocity) + s.field * (0.36f + 0.30f * expression);
    return juce::jlimit(-1.0f, 1.0f, std::tanh(body * 1.25f));
}

} // namespace horrorcastle

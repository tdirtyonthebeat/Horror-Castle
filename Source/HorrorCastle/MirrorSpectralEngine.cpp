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

    // Reflection bends the spectrum around a moving pivot. Aether controls how
    // strongly the reflected rays recurse and smear into one another.
    s.precession = wrap01(s.precession + (0.010f + 0.19f * aether * aether) / (float) sr);
    const float pivot = 1.12f + 6.4f * reflection * reflection;
    const float fracture = reflection * reflection * (0.25f + 0.75f * aether);
    float sum = 0.0f;
    float recursive = s.field;

    for (int i = 0; i < Rays; ++i)
    {
        const float original = baseRatios[(size_t) i];
        const float reflected = (pivot * pivot) / juce::jmax(0.28f, original);
        const float blend = std::pow(reflection, 0.72f + 0.08f * (float)i);
        const float logRatio = (1.0f - blend) * std::log(original)
                             + blend * std::log(reflected);
        const float shard = 1.0f + fracture * 0.018f * ((i & 1) ? -1.0f : 1.0f) * (float)(i + 1);
        const float drift = 1.0f + (0.0004f + 0.006f * aether)
                                  * std::sin(T * (s.precession * (0.37f + 0.13f * (float) i)
                                                + phaseSeeds[(size_t) i]));
        const float frequency = fundamentalHz * std::exp(logRatio) * drift * shard;
        if (frequency <= 0.0f || frequency >= sr * 0.46) continue;

        auto& ray = s.rays[(size_t) i];
        ray.phase = wrap01(ray.phase + frequency / (float) sr);
        const float recursionDepth = (0.04f + 0.62f * aether * aether) * (0.22f + 0.78f * expression);
        const float phaseWarp = recursive * recursionDepth * (0.52f + 0.23f * (float) i);
        const float reflectedPhase = T * phaseSeeds[(size_t) i] * reflection * (1.0f + 0.35f * fracture);
        const float y = std::sin(T * ray.phase + phaseWarp + reflectedPhase);
        const float memoryRate = 0.003f + 0.031f * (1.0f - reflection) * (1.0f - 0.55f * aether);
        ray.memory += memoryRate * (y - ray.memory);

        const float polarity = (i & 1) ? -1.0f : 1.0f;
        const float weight = polarity / std::sqrt((float) i + 1.0f);
        const float ghost = std::sin(T * ray.phase * 0.5f + recursive * (0.8f + 2.5f * reflection));
        const float contribution = y * weight
                                 + ray.memory * weight * (0.10f + 0.48f * reflection)
                                 + ghost * weight * fracture * 0.12f;
        sum += contribution;
        recursive = std::tanh(recursive * (0.58f + 0.18f * aether)
                            + contribution * (0.14f + 0.36f * aether));
    }

    s.field = std::tanh(sum * (0.20f + 0.14f * reflection)
                      + recursive * (0.22f + 0.40f * aether));
    const float dryAnchor = std::sin(T * s.rays[0].phase) * (0.12f + 0.10f * velocity) * (1.0f - 0.55f * reflection);
    const float shimmer = std::sin(T * s.precession + s.field * 3.0f) * s.field * (0.04f + 0.16f * aether);
    const float body = dryAnchor
                     + sum * (0.15f + 0.13f * velocity)
                     + s.field * (0.30f + 0.40f * expression)
                     + shimmer;
    const float out = std::tanh(body * (1.05f + 0.55f * reflection));
    if (!std::isfinite(out)) { s = VoiceState{}; return 0.0f; }
    return juce::jlimit(-1.0f, 1.0f, out);
}

} // namespace horrorcastle

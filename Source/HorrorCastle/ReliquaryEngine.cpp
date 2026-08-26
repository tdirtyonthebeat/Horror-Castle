#include "ReliquaryEngine.h"
#include <cmath>

namespace horrorcastle {
namespace {
float runResonator(ReliquaryEngine::ResonatorState& state,
                   float input,
                   float frequency,
                   float decaySeconds,
                   double sampleRate) noexcept
{
    if (frequency <= 0.0f || frequency >= sampleRate * 0.46 || decaySeconds <= 0.0f)
        return 0.0f;

    const float radius = std::exp(-1.0f / (decaySeconds * (float) sampleRate));
    const float omega = juce::MathConstants<float>::twoPi * frequency / (float) sampleRate;
    float y = 2.0f * radius * std::cos(omega) * state.y1
            - radius * radius * state.y2
            + input * (1.0f - radius);

    if (!std::isfinite(y)) y = 0.0f;
    state.y2 = state.y1;
    state.y1 = std::abs(y) < 1.0e-15f ? 0.0f : y;
    return y;
}

float wrap01(float x) noexcept
{
    x -= std::floor(x);
    return x < 0.0f ? x + 1.0f : x;
}
}

float ReliquaryEngine::renderSample(VoiceState& state,
                                    float fundamentalHz,
                                    float aperture,
                                    float aether,
                                    float expression,
                                    float velocity,
                                    double sampleRate) noexcept
{
    if (sampleRate <= 1.0 || fundamentalHz <= 0.0f)
        return 0.0f;

    aperture = juce::jlimit(0.0f, 1.0f, aperture);
    aether = juce::jlimit(0.0f, 1.0f, aether);
    expression = juce::jlimit(0.0f, 1.0f, expression);
    velocity = juce::jlimit(0.0f, 1.0f, velocity);

    constexpr float T = juce::MathConstants<float>::twoPi;
    constexpr float pi = juce::MathConstants<float>::pi;

    state.exciterPhase = wrap01(state.exciterPhase + fundamentalHz / (float) sampleRate);
    state.shimmerPhase = wrap01(state.shimmerPhase
        + (0.07f + 0.23f * aether) / (float) sampleRate);

    const float shimmer = std::sin(T * state.shimmerPhase);
    const float fundamental = std::sin(T * state.exciterPhase);
    const float overtone = std::sin(T * state.exciterPhase * (2.41421356f + 0.06f * aperture));
    const float exciter = (fundamental * (0.10f + 0.10f * velocity)
                         + overtone * (0.05f + 0.13f * expression))
                        * (0.45f + 0.55f * velocity);

    constexpr std::array<float, GlassModes> glassRatios {
        1.000f, 1.618034f, 2.414214f, 3.141593f,
        4.236068f, 5.372281f, 7.071068f, 9.173f
    };

    float glassSum = 0.0f;
    const float inputPosition = 0.10f + 0.38f * aperture;
    const float outputPosition = 0.83f - 0.31f * aperture + 0.04f * shimmer;
    const float coupling = (0.018f + 0.145f * aether)
                         * (0.40f + 0.60f * expression);

    for (int i = 0; i < GlassModes; ++i)
    {
        const float n = (float) (i + 1);
        const float drift = 1.0f + 0.0025f * aether
                                  * std::sin(T * state.shimmerPhase * (0.31f + 0.13f * n));
        const float frequency = fundamentalHz * glassRatios[(size_t) i] * drift;
        const float decaySeconds = (0.15f + 2.30f * aether + 0.70f * expression)
                                 / (1.0f + 0.16f * (float) i);
        const float inShape = std::sin(pi * n * inputPosition);
        const float outShape = std::sin(pi * n * outputPosition);
        const float geometry = inShape * outShape / std::pow(n, 0.42f);
        const float input = exciter * geometry
                          + state.cavityFeedback * coupling / (1.0f + 0.18f * (float) i);
        glassSum += runResonator(state.glass[(size_t) i], input,
                                 frequency, decaySeconds, sampleRate);
    }

    constexpr std::array<float, CavityModes> cavityRatios { 0.503f, 1.307f, 2.071f };
    float cavitySum = 0.0f;
    for (int i = 0; i < CavityModes; ++i)
    {
        const float frequency = fundamentalHz * cavityRatios[(size_t) i]
                              * (1.0f + 0.015f * aperture * (float) (i + 1));
        const float decaySeconds = (0.11f + 1.15f * aether + 0.38f * expression)
                                 / (1.0f + 0.22f * (float) i);
        const float input = exciter * (0.24f / (float) (i + 1))
                          + state.glassFeedback * coupling
                            * (0.78f / (float) (i + 1));
        cavitySum += runResonator(state.cavity[(size_t) i], input,
                                  frequency, decaySeconds, sampleRate);
    }

    state.glassFeedback = std::tanh(glassSum * 5.5f);
    state.cavityFeedback = std::tanh(cavitySum * 7.0f);

    // Keep the sympathetic system dynamic rather than pinning it against the
    // output limiter; the surrounding TOWER chamber still contributes air.
    const float glassGain = 3.2f + 2.2f * aperture + 1.5f * aether;
    const float cavityGain = 1.8f + 1.6f * (1.0f - aperture) + 1.2f * aether;
    const float air = (glassSum - cavitySum) * shimmer * (0.18f * aether);
    const float body = glassSum * glassGain + cavitySum * cavityGain + air;

    return juce::jlimit(-1.0f, 1.0f, std::tanh(body * 0.85f));
}

} // namespace horrorcastle

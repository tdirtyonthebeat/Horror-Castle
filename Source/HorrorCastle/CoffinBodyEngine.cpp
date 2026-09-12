#include "CoffinBodyEngine.h"
#include <cmath>

namespace horrorcastle {
namespace {
float nextNoise(uint32_t& state) noexcept
{
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return (float) state / 2147483648.0f - 1.0f;
}

float runResonator(CoffinBodyEngine::ResonatorState& state,
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

float CoffinBodyEngine::renderSample(VoiceState& state,
                                     float fundamentalHz,
                                     float lidOpen,
                                     float dread,
                                     float expression,
                                     float velocity,
                                     double sampleRate) noexcept
{
    if (sampleRate <= 1.0 || fundamentalHz <= 0.0f)
        return 0.0f;

    lidOpen = juce::jlimit(0.0f, 1.0f, lidOpen);
    dread = juce::jlimit(0.0f, 1.0f, dread);
    expression = juce::jlimit(0.0f, 1.0f, expression);
    velocity = juce::jlimit(0.0f, 1.0f, velocity);

    constexpr float T = juce::MathConstants<float>::twoPi;

    // Initial mallet/knuckle strike, followed by expression-driven scraping.
    const float strikeSeconds = 0.0035f + 0.010f * (1.0f - lidOpen);
    state.strike *= std::exp(-1.0f / (strikeSeconds * (float) sampleRate));
    const float noise = nextNoise(state.rng);
    state.scrape += (0.018f + 0.055f * expression) * (noise - state.scrape);

    state.creakPhase = wrap01(state.creakPhase
        + (0.21f + 0.43f * dread + 0.16f * expression) / (float) sampleRate);
    const float creak = std::sin(T * state.creakPhase);

    const float strike = noise * state.strike * (0.32f + 0.68f * velocity);
    const float scrape = state.scrape * expression * (0.010f + 0.045f * dread)
                       + creak * expression * 0.004f;
    const float exciter = strike + scrape;

    // LID rule: opening the lid lowers structural coupling to the body and lets
    // the plate ring brighter/longer. Closed-lid geometry is heavier and darker.
    constexpr std::array<float, LidModes> lidRatios {
        1.000f, 1.867f, 2.731f, 3.589f, 4.913f
    };
    const float lidDetune = 1.0f + 0.035f * lidOpen;
    const float woodCoupling = (0.035f + 0.170f * (1.0f - lidOpen))
                             * (0.55f + 0.45f * expression);

    float lidSum = 0.0f;
    for (int i = 0; i < LidModes; ++i)
    {
        const float n = (float) (i + 1);
        const float frequency = fundamentalHz * lidRatios[(size_t) i]
                              * lidDetune * (1.0f + 0.0020f * dread * n);
        const float decaySeconds = (0.11f + 1.05f * lidOpen + 0.70f * expression)
                                 / (1.0f + 0.18f * (float) i);
        const float input = exciter * (0.85f / std::pow(n, 0.35f))
                          + state.cavityFeedback * woodCoupling
                            / (1.0f + 0.22f * (float) i)
                          + state.stringFeedback * woodCoupling * 0.28f;
        lidSum += runResonator(state.lid[(size_t) i], input,
                               frequency, decaySeconds, sampleRate);
    }

    // BODY/AIR rule: DREAD makes the enclosure acoustically larger. The cavity
    // resonances drop and decay longer, giving a changing internal volume.
    constexpr std::array<float, CavityModes> cavityRatios {
        0.503f, 0.817f, 1.283f, 1.997f
    };
    float cavitySum = 0.0f;
    const float volumeScale = 1.0f - 0.28f * dread;
    for (int i = 0; i < CavityModes; ++i)
    {
        const float frequency = fundamentalHz * cavityRatios[(size_t) i]
                              * volumeScale * (1.0f + 0.020f * lidOpen * (float) i);
        const float decaySeconds = (0.16f + 1.70f * dread + 0.42f * expression)
                                 / (1.0f + 0.20f * (float) i);
        const float vent = 0.25f + 0.75f * lidOpen;
        const float input = exciter * (0.18f + 0.10f * (float) i) * (1.0f - 0.45f * vent)
                          + state.lidFeedback * woodCoupling * (0.85f - 0.12f * (float) i);
        cavitySum += runResonator(state.cavity[(size_t) i], input,
                                  frequency, decaySeconds, sampleRate);
    }

    // STRINGS rule: three sympathetic modes are not direct oscillators. They
    // only wake when the wooden structure injects enough energy into them.
    constexpr std::array<float, StringModes> stringRatios { 1.000f, 2.003f, 3.011f };
    float stringSum = 0.0f;
    const float sympathetic = (0.018f + 0.110f * dread)
                            * (0.30f + 0.70f * expression);
    for (int i = 0; i < StringModes; ++i)
    {
        const float frequency = fundamentalHz * stringRatios[(size_t) i]
                              * (1.0f + 0.004f * lidOpen * (float) (i + 1));
        const float decaySeconds = 0.35f + 1.65f * expression + 0.55f * lidOpen;
        const float input = state.lidFeedback * sympathetic
                          + state.cavityFeedback * sympathetic * 0.45f;
        stringSum += runResonator(state.strings[(size_t) i], input,
                                  frequency, decaySeconds, sampleRate);
    }

    state.lidFeedback = std::tanh(lidSum * 5.0f);
    state.cavityFeedback = std::tanh(cavitySum * 6.5f);
    state.stringFeedback = std::tanh(stringSum * 5.5f);

    // RATTLE rule: when lid/body differential energy exceeds a moving threshold,
    // a soft nonlinear boundary chatters instead of simply clipping the signal.
    const float differential = state.lidFeedback - state.cavityFeedback;
    const float threshold = 0.18f - 0.09f * dread + 0.05f * lidOpen;
    const float excess = std::max(0.0f, std::abs(differential) - threshold);
    const float rattle = std::tanh(excess * (6.0f + 8.0f * dread))
                       * (differential >= 0.0f ? 1.0f : -1.0f)
                       * (0.10f + 0.24f * expression);

    const float body = lidSum * (5.0f + 2.5f * lidOpen)
                     + cavitySum * (5.8f + 3.4f * dread)
                     + stringSum * (4.0f + 2.2f * expression)
                     + rattle;

    return juce::jlimit(-1.0f, 1.0f, std::tanh(body * 1.10f));
}

} // namespace horrorcastle

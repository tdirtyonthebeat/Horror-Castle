#include "ChoirBodyEngine.h"
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

float runResonator(ChoirBodyEngine::ResonatorState& state,
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

float ChoirBodyEngine::renderSample(VoiceState& state,
                                    float fundamentalHz,
                                    float vowel,
                                    float aether,
                                    float expression,
                                    float velocity,
                                    double sampleRate) noexcept
{
    if (sampleRate <= 1.0 || fundamentalHz <= 0.0f)
        return 0.0f;

    vowel = juce::jlimit(0.0f, 1.0f, vowel);
    aether = juce::jlimit(0.0f, 1.0f, aether);
    expression = juce::jlimit(0.0f, 1.0f, expression);
    velocity = juce::jlimit(0.0f, 1.0f, velocity);

    constexpr float T = juce::MathConstants<float>::twoPi;
    state.exciterPhase = wrap01(state.exciterPhase + fundamentalHz / (float) sampleRate);

    // A bright but restrained harmonic glottal source. The vocal character is
    // created by the tract bodies below rather than baked into the oscillator.
    float exciter = 0.60f * std::sin(T * state.exciterPhase);
    if (fundamentalHz * 2.0f < sampleRate * 0.46)
        exciter += 0.24f * std::sin(T * state.exciterPhase * 2.0f);
    if (fundamentalHz * 3.0f < sampleRate * 0.46)
        exciter += 0.13f * std::sin(T * state.exciterPhase * 3.0f);
    if (fundamentalHz * 5.0f < sampleRate * 0.46)
        exciter += 0.06f * std::sin(T * state.exciterPhase * 5.0f);
    exciter *= 0.18f + 0.22f * velocity + 0.18f * expression;

    // Interpolate between two tract geometries. This is intentionally not tied
    // to named human vowels; it is a continuous supernatural mouth shape.
    constexpr std::array<float, Formants> lowGeometry { 430.0f, 1180.0f, 2550.0f };
    constexpr std::array<float, Formants> highGeometry { 820.0f, 1780.0f, 3120.0f };

    float ensemble = 0.0f;
    float nextCongregation = 0.0f;

    for (int m = 0; m < Mouths; ++m)
    {
        auto& mouth = state.mouths[(size_t) m];
        const float mouthIndex = (float) m / (float) (Mouths - 1);
        const float driftHz = 0.07f + 0.055f * (float) m + 0.08f * aether;
        mouth.driftPhase = wrap01(mouth.driftPhase + driftHz / (float) sampleRate);
        const float drift = std::sin(T * mouth.driftPhase + 1.7f * (float) m);

        const float noise = nextNoise(state.rng);
        const float breathAlpha = 0.008f + 0.025f * expression;
        mouth.breath += breathAlpha * (noise - mouth.breath);

        // AETHER changes whether mouths cluster into one spectral body or pull
        // apart into separate drifting tracts. Expression strengthens the shared
        // congregation field, so pressure changes ensemble alignment.
        const float separation = (0.003f + 0.018f * aether) * (mouthIndex - 0.5f);
        const float driftAmount = (0.004f + 0.020f * aether) * drift;
        const float alignment = state.congregation
                              * (0.010f + 0.055f * expression)
                              * (1.0f - 0.45f * aether);

        float mouthSum = 0.0f;
        for (int f = 0; f < Formants; ++f)
        {
            const float baseFormant = lowGeometry[(size_t) f]
                                    + (highGeometry[(size_t) f] - lowGeometry[(size_t) f]) * vowel;
            float formantHz = baseFormant * (1.0f + separation + driftAmount + alignment);
            formantHz *= 1.0f + 0.006f * (float) f * (mouthIndex - 0.5f);

            const float decaySeconds = (0.018f + 0.055f * expression + 0.040f * (1.0f - aether))
                                     / (1.0f + 0.18f * (float) f);
            const float input = exciter * (0.85f / (1.0f + 0.45f * (float) f))
                              + mouth.breath * expression * (0.025f + 0.015f * (float) f)
                              + state.congregation * (0.008f + 0.016f * aether);
            mouthSum += runResonator(mouth.formants[(size_t) f], input,
                                     formantHz, decaySeconds, sampleRate);
        }

        mouth.feedback = std::tanh(mouthSum * 7.0f);
        nextCongregation += mouth.feedback / (float) Mouths;

        const float mouthGain = 3.5f + 1.4f * expression;
        ensemble += mouthSum * mouthGain / (float) Mouths;
    }

    state.congregation += 0.035f * (std::tanh(nextCongregation * 1.6f) - state.congregation);

    // A small antiphase shimmer prevents four near-identical tracts from
    // collapsing into a static comb-like tone while retaining a coherent body.
    const float spectralPull = state.congregation * (0.10f + 0.22f * aether);
    const float body = ensemble + spectralPull;
    return juce::jlimit(-1.0f, 1.0f, std::tanh(body * 1.25f));
}

} // namespace horrorcastle

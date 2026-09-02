#include "BoneResonatorEngine.h"
#include <cmath>

namespace horrorcastle {
namespace {
float nextNoise(uint32_t& state) noexcept
{
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return (float)state / 2147483648.0f - 1.0f;
}

float wrap01(float x) noexcept
{
    x -= std::floor(x);
    return x < 0.0f ? x + 1.0f : x;
}
}

float BoneResonatorEngine::renderSample(VoiceState& state,
                                        float fundamentalHz,
                                        float material,
                                        float character,
                                        float expression,
                                        float velocity,
                                        double sampleRate) noexcept
{
    if (sampleRate <= 1.0 || fundamentalHz <= 0.0f)
        return 0.0f;

    constexpr float pi = juce::MathConstants<float>::pi;
    constexpr float twoPi = juce::MathConstants<float>::twoPi;

    material = juce::jlimit(0.0f, 1.0f, material);
    character = juce::jlimit(0.0f, 1.0f, character);
    expression = juce::jlimit(0.0f, 1.0f, expression);
    velocity = juce::jlimit(0.0f, 1.0f, velocity);

    // Shape/material moves the spectrum from nearly harmonic bone/string modes
    // toward progressively stiffer, stretched partials while preserving the
    // played fundamental.
    const float stiffness = 0.0010f + 0.090f * material * material;

    // DREAD and live pressure alter where the virtual object is struck and read.
    // Moving through modal nodes changes timbre without changing oscillator type.
    const float inputPosition = 0.12f + 0.33f * character;
    const float outputPosition = 0.78f - 0.23f * expression;

    state.drivePhase = wrap01(state.drivePhase + fundamentalHz / (float)sampleRate);
    const float fundamental = std::sin(twoPi * state.drivePhase);
    const float noise = nextNoise(state.rng);

    // A short noisy strike starts each fresh VoiceState. Expression adds a very
    // small continuous bow/pressure feed so held notes can keep breathing.
    const float strikeSeconds = 0.0025f + 0.0075f * (1.0f - material);
    const float strikeDecay = std::exp(-1.0f / (strikeSeconds * (float)sampleRate));
    const float strike = state.strike;
    state.strike *= strikeDecay;

    const float bow = expression * (0.0030f + 0.0120f * character);
    const float excitation = (noise * 0.82f + fundamental * 0.18f)
                               * strike * (0.35f + 0.65f * velocity)
                           + fundamental * bow;

    float sum = 0.0f;
    int activeModes = 0;

    for (int modeIndex = 0; modeIndex < MaxModes; ++modeIndex)
    {
        const float n = (float)(modeIndex + 1);
        const float ratio = n * std::sqrt((1.0f + stiffness * n * n)
                                         / (1.0f + stiffness));
        const float frequency = fundamentalHz * ratio;
        if (frequency >= sampleRate * 0.46)
            continue;

        // Higher modes lose energy faster. DREAD and performance expression
        // lengthen the body while remaining safely inside the unit circle.
        const float decaySeconds = (0.10f + 2.40f * character + 1.60f * expression)
                                 / (1.0f + 0.15f * (float)modeIndex);
        const float radius = std::exp(-1.0f / (decaySeconds * (float)sampleRate));
        const float omega = twoPi * frequency / (float)sampleRate;

        const float inputShape = std::sin(pi * n * inputPosition);
        const float outputShape = std::sin(pi * n * outputPosition);
        const float coupling = inputShape * outputShape / std::pow(n, 0.55f);

        auto& mode = state.modes[(size_t)modeIndex];
        float y = 2.0f * radius * std::cos(omega) * mode.y1
                - radius * radius * mode.y2
                + excitation * coupling * (1.0f - radius);

        if (!std::isfinite(y))
            y = 0.0f;

        mode.y2 = mode.y1;
        mode.y1 = std::abs(y) < 1.0e-15f ? 0.0f : y;
        sum += y;
        ++activeModes;
    }

    if (activeModes > 0)
        sum /= std::sqrt((float)activeModes);

    // Modal banks are naturally low-level because each resonator is energy
    // conserving. Restore playable level here, then bound the output explicitly.
    const float bodyGain = 16.0f + 8.0f * character;
    return juce::jlimit(-1.0f, 1.0f, std::tanh(sum * bodyGain));
}

} // namespace horrorcastle

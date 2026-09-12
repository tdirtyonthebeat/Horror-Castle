#include "WraithBreathEngine.h"
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

float runResonator(WraithBreathEngine::ResonatorState& state,
                   float input,
                   float frequency,
                   float decaySeconds,
                   double sampleRate) noexcept
{
    if (frequency <= 0.0f || frequency >= sampleRate * 0.46 || decaySeconds <= 0.0f)
        return 0.0f;

    const float radius = std::exp(-1.0f / (decaySeconds * (float)sampleRate));
    const float omega = juce::MathConstants<float>::twoPi * frequency / (float)sampleRate;
    float y = 2.0f * radius * std::cos(omega) * state.y1
            - radius * radius * state.y2
            + input * (1.0f - radius);

    if (!std::isfinite(y))
        y = 0.0f;

    state.y2 = state.y1;
    state.y1 = std::abs(y) < 1.0e-15f ? 0.0f : y;
    return y;
}
}

float WraithBreathEngine::renderSample(VoiceState& state,
                                       float fundamentalHz,
                                       float veil,
                                       float haunt,
                                       float expression,
                                       float velocity,
                                       double sampleRate) noexcept
{
    if (sampleRate <= 1.0 || fundamentalHz <= 0.0f)
        return 0.0f;

    veil = juce::jlimit(0.0f, 1.0f, veil);
    haunt = juce::jlimit(0.0f, 1.0f, haunt);
    expression = juce::jlimit(0.0f, 1.0f, expression);
    velocity = juce::jlimit(0.0f, 1.0f, velocity);

    // WRAITH rule 1: there is no impulse-only excitation. A breath source keeps
    // the body alive for as long as the player continues to feed it energy.
    const float noise = nextNoise(state.rng);
    const float breathAlpha = 0.015f + 0.080f * (1.0f - veil);
    state.breathLowpass += breathAlpha * (noise - state.breathLowpass);

    const float targetPressure = 0.25f + 0.75f * expression;
    state.pressure += 0.0006f * (targetPressure - state.pressure);

    state.flutterPhase += (0.17f + 0.31f * haunt) / (float)sampleRate;
    state.flutterPhase -= std::floor(state.flutterPhase);
    const float flutter = std::sin(juce::MathConstants<float>::twoPi * state.flutterPhase);

    const float breath = state.breathLowpass * (0.020f + 0.100f * expression + 0.025f * velocity)
                       + flutter * (0.008f * expression);

    // WRAITH rule 2: the membrane and air column exchange energy. HAUNT controls
    // the strength of that coupling, while expression changes how hard the
    // player drives the coupled system.
    const float coupling = (0.025f + 0.190f * haunt)
                         * (0.45f + 0.55f * expression);

    constexpr std::array<float, MembraneModes> membraneRatios {
        1.000f, 1.593f, 2.135f, 2.296f, 2.653f, 2.918f
    };
    constexpr std::array<float, AirModes> airRatios { 1.000f, 2.970f, 5.110f };

    float membraneSum = 0.0f;
    const float stiffness = 0.005f + 0.055f * veil * veil;
    const float inputPosition = 0.22f + 0.22f * veil;

    for (int i = 0; i < MembraneModes; ++i)
    {
        const float n = (float)(i + 1);
        const float stretched = membraneRatios[(size_t)i]
                              * std::sqrt((1.0f + stiffness * n * n)
                                        / (1.0f + stiffness));
        const float slowDrift = 1.0f + 0.003f * haunt
                                      * std::sin(state.flutterPhase
                                               * juce::MathConstants<float>::twoPi
                                               * n * 0.37f);
        const float frequency = fundamentalHz * stretched * slowDrift;
        const float decaySeconds = (0.09f + 1.15f * (1.0f - veil)
                                          + 1.65f * haunt
                                          + 1.40f * expression)
                                 / (1.0f + 0.13f * (float)i);
        const float modeShape = std::sin(juce::MathConstants<float>::pi * n * inputPosition)
                              / std::pow(n, 0.35f);
        const float input = breath * modeShape
                          + state.airFeedback * coupling / (1.0f + 0.25f * (float)i);
        membraneSum += runResonator(state.membrane[(size_t)i], input,
                                    frequency, decaySeconds, sampleRate);
    }

    float airSum = 0.0f;
    for (int i = 0; i < AirModes; ++i)
    {
        const float frequency = fundamentalHz * airRatios[(size_t)i]
                              * (1.0f + 0.012f * haunt * (float)(i + 1));
        const float decaySeconds = (0.045f + 0.28f * expression + 0.55f * haunt)
                                 / (1.0f + 0.25f * (float)i);
        const float input = breath * (0.80f / (float)(i + 1))
                          + state.membraneFeedback * coupling
                            * (0.70f / (float)(i + 1));
        airSum += runResonator(state.air[(size_t)i], input,
                              frequency, decaySeconds, sampleRate);
    }

    // WRAITH rule 3: feedback is energy-limited at the body boundaries. This
    // allows eerie self-reinforcement without permitting the coupled modes to
    // turn into an unbounded numerical oscillator.
    state.membraneFeedback = std::tanh(membraneSum * 5.0f);
    state.airFeedback = std::tanh(airSum * 7.0f);

    const float membraneGain = 9.0f * (0.65f + 0.35f * veil);
    const float airGain = 11.0f * (0.35f + 0.65f * haunt);
    const float body = membraneSum * membraneGain
                     + airSum * airGain
                     + breath * 0.80f;

    return juce::jlimit(-1.0f, 1.0f, std::tanh(body * 1.5f));
}

} // namespace horrorcastle

#include "RitualFMEngine.h"
#include <cmath>

namespace horrorcastle {
namespace {
float wrap01(float x) noexcept
{
    x -= std::floor(x);
    return x < 0.0f ? x + 1.0f : x;
}

float lerp(float a, float b, float t) noexcept { return a + (b - a) * t; }
}

float RitualFMEngine::renderSample(VoiceState& state,
                                   float fundamentalHz,
                                   float topology,
                                   float character,
                                   float expression,
                                   bool crypt,
                                   double sampleRate) noexcept
{
    if (sampleRate <= 1.0 || fundamentalHz <= 0.0f)
        return 0.0f;

    constexpr float twoPi = juce::MathConstants<float>::twoPi;
    topology = juce::jlimit(0.0f, 1.0f, topology);
    character = juce::jlimit(0.0f, 1.0f, character);
    expression = juce::jlimit(0.0f, 1.0f, expression);

    // CRYPT prefers weighty/subharmonic relationships; TOWER uses irrational
    // upper ratios. The interpolation is deliberately smooth so the same FM
    // generator acquires different chamber identities without preset breaks.
    constexpr std::array<float, 4> cryptRatios { 1.0f, 0.5000f, 1.5030f, 2.0110f };
    constexpr std::array<float, 4> towerRatios { 1.0f, 1.41421356f, 2.41421356f, 3.73205081f };
    const float chamber = crypt ? 0.0f : 1.0f;

    state.motionPhase = wrap01(state.motionPhase
                               + (0.035f + 0.115f * character + 0.075f * expression)
                                     / (float) sampleRate);
    const float motion = std::sin(twoPi * state.motionPhase);

    std::array<float, 4> phi {};
    for (size_t i = 0; i < phi.size(); ++i)
    {
        float ratio = lerp(cryptRatios[i], towerRatios[i], chamber);
        // Each operator gets a tiny correlated motion offset. It is small enough
        // to keep pitch stable but prevents long held notes from freezing.
        const float cents = motion * (0.35f + 1.65f * character) * (float)(i + 1);
        ratio *= std::pow(2.0f, cents / 1200.0f);
        const float frequency = fundamentalHz * ratio;
        if (frequency < sampleRate * 0.47)
            state.phase[i] = wrap01(state.phase[i] + frequency / (float) sampleRate);
        phi[i] = twoPi * state.phase[i];
    }

    // Performance gestures increase modulation index rather than merely volume.
    // This makes mod wheel/pressure open timbre and complexity in a playable way.
    const float requestedIndex = 0.30f + topology * 2.0f + character * 3.2f + expression * 4.2f;
    // Sideband headroom: high notes automatically back the index down instead of
    // spraying aliases above Nyquist. This keeps the FM family glassy and precise
    // across the keyboard rather than only sounding clean in the middle register.
    const float highestRatio = crypt ? 2.0110f : 3.73205081f;
    const float sidebandRoom = juce::jlimit(0.10f, 1.0f,
        (float)((sampleRate * 0.44 / std::max(1.0f, fundamentalHz) - highestRatio) / 8.0f));
    const float index = requestedIndex * sidebandRoom;
    const float feedback = juce::jlimit(0.0f, 0.78f,
                                        0.03f + character * 0.34f + expression * 0.18f);

    const float op4 = std::sin(phi[3] + state.previous[3] * feedback * 2.6f);
    const float op3 = std::sin(phi[2] + op4 * index * 0.68f);
    const float op2 = std::sin(phi[1] + op3 * index * 0.82f);

    // Topology A: a deep serial chain with a single carrier.
    const float chain = std::sin(phi[0] + op2 * index);

    // Topology B: operator four branches into two independent modulators.
    const float branchA = std::sin(phi[1] + op4 * index * 0.56f);
    const float branchB = std::sin(phi[2] - op4 * index * 0.43f);
    const float branch = std::sin(phi[0] + (branchA + branchB) * index * 0.54f);

    // Topology C: two carriers with separate modulators; especially effective
    // for TOWER's glassy ratios and wide harmonic motion.
    const float dualA = std::sin(phi[0] + std::sin(phi[2]) * index * 0.72f);
    const float dualB = std::sin(phi[1] + op4 * index * 0.76f);
    const float dual = dualA * 0.64f + dualB * 0.36f;

    // Triangular crossfades make Shape a true continuous graph morph rather
    // than a disguised algorithm selector.
    const float livingTopology = juce::jlimit(0.0f, 1.0f,
                                               topology + motion * (0.018f + 0.055f * expression));
    const float chainW = juce::jlimit(0.0f, 1.0f, 1.0f - livingTopology * 2.0f);
    const float branchW = 1.0f - std::abs(livingTopology - 0.5f) * 2.0f;
    const float dualW = juce::jlimit(0.0f, 1.0f, livingTopology * 2.0f - 1.0f);

    state.previous[0] = chain;
    state.previous[1] = branchA;
    state.previous[2] = branchB;
    state.previous[3] = op4;

    float out = chain * chainW + branch * branchW + dual * dualW;
    if (crypt)
    {
        // Dense without blanket saturation: retain operator detail, then add only
        // a little low-ratio body. Headroom is fixed so velocity/expression do
        // not turn into accidental loudness jumps.
        out = out * (0.76f + character * 0.10f) + 0.055f * std::sin(phi[1]);
    }
    else
    {
        // TOWER stays airy and precise; the high-ratio operator is audible only
        // as a controlled glint, not as a permanent bell pasted onto every note.
        const float glint = std::sin(phi[3] + op3 * 0.35f) * (0.025f + 0.055f * character);
        out = out * (0.72f + character * 0.12f) + glint;
    }

    return juce::jlimit(-1.0f, 1.0f, out);
}

} // namespace horrorcastle

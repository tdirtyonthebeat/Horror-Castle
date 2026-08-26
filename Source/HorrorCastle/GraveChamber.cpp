#include "GraveChamber.h"
#include <cmath>

namespace horrorcastle {

void GraveChamber::prepare(double sampleRate)
{
    sr = juce::jmax(1000.0, sampleRate);

    // Uneven line lengths avoid obvious common repeats while keeping the
    // chamber dense enough to read as architecture rather than a multitap delay.
    constexpr std::array<double, Lines> seconds { 0.0371, 0.0437, 0.0533, 0.0611 };
    for (int i = 0; i < Lines; ++i)
    {
        const int samples = juce::jmax(8, (int)std::lround(seconds[(size_t)i] * sr));
        buffers[(size_t)i].assign((size_t)samples, 0.0f);
        positions[(size_t)i] = 0;
        damping[(size_t)i] = 0.0f;
    }
}

void GraveChamber::reset()
{
    for (auto& b : buffers)
        std::fill(b.begin(), b.end(), 0.0f);
    positions.fill(0);
    damping.fill(0.0f);
}

void GraveChamber::setParameters(float size01, float tone01, float width01) noexcept
{
    const float size = juce::jlimit(0.0f, 1.0f, size01);
    const float tone = juce::jlimit(0.0f, 1.0f, tone01);
    width = juce::jlimit(0.0f, 1.0f, width01);

    // Stable below unity, but long enough at maximum size to bloom behind notes.
    feedback = 0.72f + size * 0.245f;

    // TONE is intentionally logarithmic: the lower half remains genuinely dark.
    const float cutoffHz = 480.0f * std::pow(18.0f, tone);
    dampAlpha = 1.0f - std::exp(-juce::MathConstants<float>::twoPi
                                * cutoffHz / (float)sr);
    dampAlpha = juce::jlimit(0.002f, 0.92f, dampAlpha);
}

void GraveChamber::processSample(float inputL, float inputR,
                                 float& wetL, float& wetR) noexcept
{
    std::array<float, Lines> r{};
    for (int i = 0; i < Lines; ++i)
    {
        const auto& b = buffers[(size_t)i];
        if (!b.empty())
            r[(size_t)i] = b[(size_t)positions[(size_t)i]];
    }

    // Darken each returning path before it re-enters the feedback matrix.
    for (int i = 0; i < Lines; ++i)
        damping[(size_t)i] += dampAlpha * (r[(size_t)i] - damping[(size_t)i]);

    // Four-way orthogonal sign mixing. Each path receives all the others but
    // with alternating polarity, producing a fast buildup without one dominant echo.
    const float d0 = damping[0], d1 = damping[1], d2 = damping[2], d3 = damping[3];
    const std::array<float, Lines> mixed {
        0.5f * ( d0 + d1 + d2 + d3),
        0.5f * ( d0 - d1 + d2 - d3),
        0.5f * ( d0 + d1 - d2 - d3),
        0.5f * ( d0 - d1 - d2 + d3)
    };

    const float midIn = 0.5f * (inputL + inputR);
    const float sideIn = 0.5f * (inputL - inputR);
    const std::array<float, Lines> inject {
        midIn + sideIn * 0.55f,
       -midIn + sideIn * 0.20f,
        midIn - sideIn * 0.20f,
       -midIn - sideIn * 0.55f
    };

    for (int i = 0; i < Lines; ++i)
    {
        auto& b = buffers[(size_t)i];
        if (b.empty()) continue;

        const float write = std::tanh(inject[(size_t)i] * 0.22f
                                      + mixed[(size_t)i] * feedback);
        b[(size_t)positions[(size_t)i]] = write;
        if (++positions[(size_t)i] >= (int)b.size())
            positions[(size_t)i] = 0;
    }

    // Different sign taps create stereo decorrelation without a chorus LFO.
    float l = 0.42f * (r[0] - r[1] + r[2] - r[3]);
    float rr = 0.42f * (r[0] + r[1] - r[2] - r[3]);
    const float mid = 0.5f * (l + rr);
    const float side = 0.5f * (l - rr) * (0.30f + 1.70f * width);
    wetL = mid + side;
    wetR = mid - side;
}

} // namespace horrorcastle

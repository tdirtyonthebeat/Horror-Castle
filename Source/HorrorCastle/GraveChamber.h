#pragma once
#include <JuceHeader.h>
#include <array>
#include <vector>

namespace horrorcastle {

// GRAVE is Horror Castle's post-Ritual space: a compact four-line feedback
// chamber with dark damping and deliberately uneven decay paths. It is built
// from standard feedback-delay-network principles but voiced for this synth.
class GraveChamber
{
public:
    void prepare(double sampleRate);
    void reset();
    void setParameters(float size01, float tone01, float width01) noexcept;
    void processSample(float inputL, float inputR, float& wetL, float& wetR) noexcept;

private:
    static constexpr int Lines = 4;
    double sr = 44100.0;
    std::array<std::vector<float>, Lines> buffers;
    std::array<int, Lines> positions{};
    std::array<float, Lines> damping{};
    float feedback = 0.84f;
    float dampAlpha = 0.22f;
    float width = 0.75f;
};

} // namespace horrorcastle

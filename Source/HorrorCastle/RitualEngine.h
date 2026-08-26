#pragma once
#include <JuceHeader.h>
#include <cstdint>

namespace horrorcastle {

enum class RitualMode : uint8_t { Bind = 0, Sacrifice, Summon, Possess, Devour };

class RitualEngine {
public:
    struct Parameters {
        RitualMode mode = RitualMode::Bind;
        float mix = 0.0f;
        float depth = 0.35f;
        float drive = 0.12f;
        float width = 0.55f;
        float feedback = 0.28f;
    };

    void prepare(double sampleRate);
    void reset();
    void setParameters(const Parameters& newParameters) noexcept { params = newParameters; }

    void processSample(float cryptL, float cryptR,
                       float towerL, float towerR,
                       float mixMod, float depthMod, float driveMod, float feedbackMod,
                       float& outputL, float& outputR) noexcept;

private:
    Parameters params {};
    double sr = 44100.0;
    float summonL = 0.0f;       // resonator y[n-1], left
    float summonR = 0.0f;       // resonator y[n-1], right
    float summonPrevL = 0.0f;   // resonator y[n-2], left
    float summonPrevR = 0.0f;   // resonator y[n-2], right

    // SACRIFICE DC-blocker state. Ring multiplication of similar tones creates
    // a large DC component plus sum/difference sidebands; removing the DC makes
    // the mode audibly live in a different spectral world from BIND.
    float sacrificeInL = 0.0f;
    float sacrificeInR = 0.0f;
    float sacrificeOutL = 0.0f;
    float sacrificeOutR = 0.0f;

    static float clamp(float x) noexcept;
    static float fold(float x) noexcept;
    static void applyWidth(float& l, float& r, float width) noexcept;
};

} // namespace horrorcastle

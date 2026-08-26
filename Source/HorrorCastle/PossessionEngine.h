#pragma once
#include <JuceHeader.h>
#include <vector>

namespace horrorcastle {

class PossessionEngine {
public:
    struct Parameters {
        float bloodFeed = 0.0f;
        float aetherLeak = 0.0f;
        float soulExchange = 0.0f;
        float haunt = 0.0f;
    };

    void prepare(double sampleRate);
    void reset();
    void setParameters(const Parameters& p) noexcept { params = p; }
    void processSample(float cryptL, float cryptR, float towerL, float towerR,
                       float bloodMod, float aetherMod, float soulMod, float hauntMod,
                       float& outCryptL, float& outCryptR,
                       float& outTowerL, float& outTowerR) noexcept;

private:
    Parameters params{};
    double sr = 44100.0;
    float cryptEnv = 0.0f;
    float towerLpL = 0.0f, towerLpR = 0.0f;
    std::vector<float> cryptDelayL, cryptDelayR, towerDelayL, towerDelayR;
    int writePos = 0;
    static float clamp(float x) noexcept { return std::tanh(x); }
};

} // namespace horrorcastle

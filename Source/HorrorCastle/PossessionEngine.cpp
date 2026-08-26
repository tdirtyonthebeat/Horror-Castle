#include "PossessionEngine.h"
#include <cmath>

namespace horrorcastle {

void PossessionEngine::prepare(double sampleRate)
{
    sr = juce::jmax(1000.0, sampleRate);
    const int size = juce::jmax(64, (int)std::ceil(sr * 0.22));
    cryptDelayL.assign((size_t)size, 0.f); cryptDelayR.assign((size_t)size, 0.f);
    towerDelayL.assign((size_t)size, 0.f); towerDelayR.assign((size_t)size, 0.f);
    reset();
}

void PossessionEngine::reset()
{
    cryptEnv = towerLpL = towerLpR = 0.f; writePos = 0;
    std::fill(cryptDelayL.begin(), cryptDelayL.end(), 0.f);
    std::fill(cryptDelayR.begin(), cryptDelayR.end(), 0.f);
    std::fill(towerDelayL.begin(), towerDelayL.end(), 0.f);
    std::fill(towerDelayR.begin(), towerDelayR.end(), 0.f);
}

void PossessionEngine::processSample(float cL, float cR, float tL, float tR,
                                     float bloodMod, float aetherMod, float soulMod, float hauntMod,
                                     float& ocL, float& ocR, float& otL, float& otR) noexcept
{
    const float blood = juce::jlimit(0.f, 1.f, params.bloodFeed + bloodMod * .55f);
    const float aether = juce::jlimit(0.f, 1.f, params.aetherLeak + aetherMod * .55f);
    const float soul = juce::jlimit(0.f, 1.f, params.soulExchange + soulMod * .55f);
    const float haunt = juce::jlimit(0.f, 1.f, params.haunt + hauntMod * .55f);

    const float envIn = .5f * (std::abs(cL) + std::abs(cR));
    const float envA = 1.f - std::exp(-juce::MathConstants<float>::twoPi * 18.f / (float)sr);
    cryptEnv += envA * (envIn - cryptEnv);

    // BLOOD FEED: CRYPT's physical envelope drives TOWER's intensity and edge.
    const float bloodGain = 1.f + blood * cryptEnv * 2.8f;
    tL = std::tanh(tL * bloodGain + cL * cryptEnv * blood * .12f);
    tR = std::tanh(tR * bloodGain + cR * cryptEnv * blood * .12f);

    // AETHER LEAK: only TOWER's high-frequency residue contaminates CRYPT.
    const float airA = 1.f - std::exp(-juce::MathConstants<float>::twoPi * 1700.f / (float)sr);
    towerLpL += airA * (tL - towerLpL); towerLpR += airA * (tR - towerLpR);
    const float airL = tL - towerLpL, airR = tR - towerLpR;
    cL = std::tanh(cL + airL * aether * 1.15f);
    cR = std::tanh(cR + airR * aether * 1.15f);

    // SOUL EXCHANGE: bounded bidirectional cross-phase transformation.
    if (soul > 1.0e-5f) {
        const float phase = juce::MathConstants<float>::pi * (0.35f + soul * 2.25f);
        const float ncL = std::sin(cL * juce::MathConstants<float>::pi + tL * phase);
        const float ncR = std::sin(cR * juce::MathConstants<float>::pi + tR * phase);
        const float ntL = std::sin(tL * juce::MathConstants<float>::pi - cL * phase * .83f);
        const float ntR = std::sin(tR * juce::MathConstants<float>::pi - cR * phase * .83f);
        cL = cL * (1.f - soul * .68f) + ncL * soul * .68f;
        cR = cR * (1.f - soul * .68f) + ncR * soul * .68f;
        tL = tL * (1.f - soul * .68f) + ntL * soul * .68f;
        tR = tR * (1.f - soul * .68f) + ntR * soul * .68f;
    }

    // HAUNT: each chamber hears a delayed imprint of the other's past behavior.
    if (!cryptDelayL.empty()) {
        const int size = (int)cryptDelayL.size();
        const int cDelay = juce::jlimit(1, size - 1, (int)(sr * .073));
        const int tDelay = juce::jlimit(1, size - 1, (int)(sr * .109));
        const int rc = (writePos - cDelay + size) % size;
        const int rt = (writePos - tDelay + size) % size;
        const float ghostCL = cryptDelayL[(size_t)rc], ghostCR = cryptDelayR[(size_t)rc];
        const float ghostTL = towerDelayL[(size_t)rt], ghostTR = towerDelayR[(size_t)rt];
        cryptDelayL[(size_t)writePos] = cL; cryptDelayR[(size_t)writePos] = cR;
        towerDelayL[(size_t)writePos] = tL; towerDelayR[(size_t)writePos] = tR;
        if (++writePos >= size) writePos = 0;
        cL += ghostTL * haunt * .52f; cR += ghostTR * haunt * .52f;
        tL -= ghostCL * haunt * .39f; tR -= ghostCR * haunt * .39f;
    }

    ocL = clamp(cL); ocR = clamp(cR); otL = clamp(tL); otR = clamp(tR);
}

} // namespace horrorcastle

#include "RitualEngine.h"
#include <cmath>

namespace horrorcastle {

void RitualEngine::prepare(double sampleRate)
{
    sr = juce::jmax(1000.0, sampleRate);
    reset();
}

void RitualEngine::reset()
{
    summonL = 0.0f;
    summonR = 0.0f;
    summonPrevL = 0.0f;
    summonPrevR = 0.0f;
    sacrificeInL = 0.0f;
    sacrificeInR = 0.0f;
    sacrificeOutL = 0.0f;
    sacrificeOutR = 0.0f;
}

float RitualEngine::clamp(float x) noexcept
{
    return juce::jlimit(-1.0f, 1.0f, x);
}

float RitualEngine::fold(float x) noexcept
{
    // Sine folding is bounded and gives DEVOUR a bright, metallic collapse.
    return std::sin(x);
}

void RitualEngine::applyWidth(float& l, float& r, float width) noexcept
{
    const float mid = 0.5f * (l + r);
    float side = 0.5f * (l - r);
    side *= 0.35f + 1.65f * juce::jlimit(0.0f, 1.0f, width);
    l = mid + side;
    r = mid - side;
}

void RitualEngine::processSample(float cryptL, float cryptR,
                                 float towerL, float towerR,
                                 float mixMod, float depthMod, float driveMod, float feedbackMod,
                                 float& outputL, float& outputR) noexcept
{
    const float mix = juce::jlimit(0.0f, 1.0f, params.mix + mixMod * 0.32f);
    const float depth = juce::jlimit(0.0f, 1.0f, params.depth + depthMod * 0.42f);
    const float drive = juce::jlimit(0.0f, 1.0f, params.drive + driveMod * 0.35f);
    const float feedback = juce::jlimit(0.0f, 0.92f, params.feedback + feedbackMod * 0.18f);

    const float dryL = 0.5f * (cryptL + towerL);
    const float dryR = 0.5f * (cryptR + towerR);
    float wetL = dryL;
    float wetR = dryR;

    switch (params.mode)
    {
        case RitualMode::Bind:
        {
            // BIND is no longer just a crossfade. It fuses both chambers with
            // sign-preserving cross-products, creating a warm "glued" harmonic
            // body even when CRYPT and TOWER start from very similar material.
            auto bindChannel = [&](float crypt, float tower)
            {
                const float sum = 0.5f * (crypt + tower);
                const float cross =
                    0.5f * (crypt * std::abs(tower) + tower * std::abs(crypt));

                const float bond = 0.45f + depth * 1.55f;
                const float heat = 1.0f + drive * 3.0f;
                const float fused = std::tanh((sum + cross * bond) * heat);

                // Feedback acts as a small coherence push rather than a delay.
                const float coherence =
                    std::tanh((sum + fused * feedback * 0.65f) * (1.0f + drive));

                return clamp(sum * (0.42f - depth * 0.18f)
                             + fused * (0.38f + depth * 0.38f)
                             + coherence * 0.20f);
            };

            wetL = bindChannel(cryptL, towerL);
            wetR = bindChannel(cryptR, towerR);
            break;
        }

        case RitualMode::Sacrifice:
        {
            // SACRIFICE is intentionally spectral rather than saturating.
            // Multiplying CRYPT x TOWER creates sum/difference sidebands. When
            // the chambers are similar, that includes a strong octave component
            // plus DC. A dedicated DC blocker removes the static component so
            // SACRIFICE cannot collapse into the warm distortion character of BIND.
            auto sacrificeChannel = [&](float crypt, float tower, float dry,
                                        float& previousInput, float& previousOutput)
            {
                const float ringGain = 4.0f + drive * 16.0f;
                const float ring = std::tanh((crypt * tower) * ringGain);

                // One-pole DC blocker. 0.995 is stable and low enough to remove
                // ring-product DC without erasing musical low-frequency sidebands.
                const float sideband = ring - previousInput + 0.995f * previousOutput;
                previousInput = ring;
                previousOutput = sideband;

                // DEPTH moves from mostly octave/sideband content into a brighter,
                // folded metallic spectrum. Difference energy keeps unlike scenes alive.
                const float difference = crypt - tower;
                const float foldDrive = 1.0f + depth * 4.5f + drive * 2.5f;
                const float metal = std::sin(sideband * juce::MathConstants<float>::pi * foldDrive);
                const float cross = std::tanh(difference * (1.0f + feedback * 5.0f));

                const float sacrificial = metal * (0.82f + depth * 0.18f)
                                        + cross * feedback * 0.30f;

                // Keep only a trace of the original fundamental in the wet signal.
                const float remnant = dry * (0.10f * (1.0f - depth));
                return clamp(remnant + sacrificial);
            };

            wetL = sacrificeChannel(cryptL, towerL, dryL,
                                    sacrificeInL, sacrificeOutL);
            wetR = sacrificeChannel(cryptR, towerR, dryR,
                                    sacrificeInR, sacrificeOutR);
            break;
        }

        case RitualMode::Summon:
        {
            // SUMMON is a tuned regenerative resonator. DEPTH sweeps the entity's
            // resonant pitch, FURY controls excitation, and FEEDBACK controls how
            // long the summoned tone persists. A slight R-channel detune prevents
            // the resonance from collapsing into a narrow mono whistle.
            const float baseHz = 72.0f * std::pow(2.0f, depth * 5.0f); // 72..2304 Hz
            const float radius = juce::jlimit(0.72f, 0.992f,
                                              0.78f + feedback * 0.22f);
            const float exciteGain = 0.18f + drive * 1.55f;

            auto resonator = [&](float input, float frequency,
                                 float& y1, float& y2)
            {
                const float omega =
                    juce::MathConstants<float>::twoPi
                    * juce::jlimit(30.0f, (float) sr * 0.45f, frequency)
                    / (float) sr;

                const float a1 = 2.0f * radius * std::cos(omega);
                const float a2 = radius * radius;
                const float excite = std::tanh(input * (1.0f + drive * 4.0f));

                float y = excite * exciteGain + a1 * y1 - a2 * y2;

                // Soft limiting keeps extreme feedback playable.
                y = std::tanh(y * (0.85f + drive * 0.70f));

                y2 = y1;
                y1 = y;
                return y;
            };

            const float summonedL =
                resonator(dryL, baseHz, summonL, summonPrevL);
            const float summonedR =
                resonator(dryR, baseHz * 1.011f, summonR, summonPrevR);

            wetL = clamp(dryL * 0.15f + summonedL * 0.85f);
            wetR = clamp(dryR * 0.15f + summonedR * 0.85f);
            break;
        }

        case RitualMode::Possess:
        {
            // Cross-phase possession: each scene bends the transfer function of
            // the other. It is intentionally different from oscillator FM.
            const float phase = juce::MathConstants<float>::pi * (0.8f + drive * 2.2f);
            const float cross = juce::MathConstants<float>::pi * depth * 1.75f;
            const float pCryptL = std::sin(cryptL * phase + towerL * cross);
            const float pTowerL = std::sin(towerL * phase + cryptL * cross);
            const float pCryptR = std::sin(cryptR * phase + towerR * cross);
            const float pTowerR = std::sin(towerR * phase + cryptR * cross);
            wetL = 0.5f * (pCryptL + pTowerL);
            wetR = 0.5f * (pCryptR + pTowerR);
            break;
        }

        case RitualMode::Devour:
        {
            // Soft saturation feeds a bounded wavefolder. Depth controls how much
            // of the original scene identity survives the DEVOUR stage.
            const float gain = 1.0f + drive * 9.0f;
            const float satL = std::tanh(dryL * gain);
            const float satR = std::tanh(dryR * gain);
            const float foldGain = 1.0f + depth * 7.0f;
            const float foldedL = fold(dryL * gain * foldGain);
            const float foldedR = fold(dryR * gain * foldGain);
            wetL = satL * (1.0f - depth) + foldedL * depth;
            wetR = satR * (1.0f - depth) + foldedR * depth;
            break;
        }
    }

    applyWidth(wetL, wetR, params.width);

    outputL = clamp(dryL * (1.0f - mix) + wetL * mix);
    outputR = clamp(dryR * (1.0f - mix) + wetR * mix);
}

} // namespace horrorcastle

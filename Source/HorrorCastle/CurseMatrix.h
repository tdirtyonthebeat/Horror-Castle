#pragma once
#include <JuceHeader.h>
#include <array>
#include <cmath>

namespace horrorcastle {

struct CurseLane
{
    int source = 0;
    int curse = 0;
    int destination = 0;
    float amount = 0.f;

    // Legacy v0.9 state retained for backwards-compatible preset loading.
    float legacyDecay = .25f;

    // v0.10 Curse-specific character parameters.
    float corruptBias = .22f;
    float hauntRate = 1.70f;
    float possessionFold = .35f;
    float decayTime = 1.50f;
    float madnessSteps = 8.f;
    float bloodHunger = .65f;
};

class CurseMatrix
{
public:
    static constexpr int Lanes = 8;
    static constexpr int Destinations = 22;
    static constexpr int CurseDepthDestination = 10;

    void load(const juce::AudioProcessorValueTreeState& apvts)
    {
        for (int i = 0; i < Lanes; ++i)
        {
            const auto p = juce::String("hex.curse") + juce::String(i + 1) + ".";
            lane(i).source = choice(apvts, p + "source", 8);
            lane(i).curse = choice(apvts, p + "curse", 6);
            lane(i).destination = choice(apvts, p + "destination", 21);
            lane(i).amount = value(apvts, p + "amount", 0.f);
            lane(i).legacyDecay = value(apvts, p + "decay", .25f);
            lane(i).corruptBias = value(apvts, p + "corruptBias", .22f);
            lane(i).hauntRate = value(apvts, p + "hauntRate", 1.70f);
            lane(i).possessionFold = value(apvts, p + "possessionFold", .35f);
            lane(i).decayTime = value(apvts, p + "decayTime", 1.50f);
            lane(i).madnessSteps = value(apvts, p + "madnessSteps", 8.f);
            lane(i).bloodHunger = value(apvts, p + "bloodHunger", .65f);
        }
    }

    void reset()
    {
        pulse = 0.f;
        laneValues.fill(0.f);
        hauntPhase.fill(0.f);
    }

    std::array<float, Destinations> evaluate(float blood, float wraith, float velocity,
                                              float key, float random, float modWheel,
                                              float aftertouch, float dt)
    {
        pulse += dt * 1.7f;
        while (pulse > 1.f) pulse -= 1.f;

        std::array<float, Destinations> out{};
        std::array<float, Lanes> depthMod{};

        // First pass: each lane has its own Haunt clock and Curse character.
        for (int i = 0; i < Lanes; ++i)
        {
            const auto& l = lanes[(size_t)i];
            if (l.source == 0 || l.destination == 0 || std::abs(l.amount) < 1.0e-6f)
            {
                laneValues[(size_t)i] = 0.f;
                continue;
            }

            hauntPhase[(size_t)i] += dt * juce::jlimit(.05f, 8.f, l.hauntRate);
            while (hauntPhase[(size_t)i] > 1.f) hauntPhase[(size_t)i] -= 1.f;

            float s = sourceValue(l.source, blood, wraith, velocity, key, random, modWheel, aftertouch, pulse);
            s = applyCurse(l, s, blood, wraith, hauntPhase[(size_t)i]);

            // Low, bounded memory makes the lane feel alive but cannot self-oscillate.
            s = juce::jlimit(-1.0f, 1.0f, s + laneValues[(size_t)i] * 0.08f);
            laneValues[(size_t)i] = s * l.amount;
        }

        // Curse Depth remains feed-forward: earlier lanes can possess later lanes,
        // but later lanes cannot feed backward into their ancestors.
        for (int i = 0; i < Lanes; ++i)
        {
            if (lanes[(size_t)i].destination != CurseDepthDestination)
                continue;

            for (int j = i + 1; j < Lanes; ++j)
                depthMod[(size_t)j] += laneValues[(size_t)i] * 0.35f;
        }

        // Accumulate ordinary destinations after second-order Curse depth is applied.
        for (int i = 0; i < Lanes; ++i)
        {
            const auto& l = lanes[(size_t)i];
            if (l.destination <= 0 || l.destination >= Destinations || l.destination == CurseDepthDestination)
                continue;

            const float depth = juce::jlimit(0.0f, 2.0f, 1.0f + depthMod[(size_t)i]);
            out[(size_t)l.destination] += laneValues[(size_t)i] * depth;
        }

        float totalDepth = 0.f;
        for (float d : depthMod) totalDepth += d;
        out[(size_t)CurseDepthDestination] = juce::jlimit(-1.0f, 1.0f, totalDepth);

        for (auto& x : out)
            x = juce::jlimit(-1.0f, 1.0f, x);

        return out;
    }

    float getLaneValue(int index) const noexcept
    {
        return (index >= 0 && index < Lanes) ? laneValues[(size_t)index] : 0.f;
    }

private:
    std::array<CurseLane, Lanes> lanes{};
    std::array<float, Lanes> laneValues{};
    std::array<float, Lanes> hauntPhase{};
    float pulse = 0.f;

    CurseLane& lane(int i) { return lanes[(size_t)i]; }

    static float value(const juce::AudioProcessorValueTreeState& a, const juce::String& id, float fallback)
    {
        if (auto* p = a.getRawParameterValue(id)) return p->load();
        return fallback;
    }

    // AudioParameterChoice raw values are already zero-based choice indices.
    static int choice(const juce::AudioProcessorValueTreeState& a, const juce::String& id, int max)
    {
        return juce::jlimit(0, max, (int)std::lround(value(a, id, 0.f)));
    }

    static float sourceValue(int source, float blood, float wraith, float velocity,
                             float key, float random, float modWheel, float aftertouch,
                             float pulse)
    {
        switch (source)
        {
            case 1: return blood * 2.f - 1.f;
            case 2: return wraith * 2.f - 1.f;
            case 3: return velocity * 2.f - 1.f;
            case 4: return key;
            case 5: return random;
            case 6: return std::sin(juce::MathConstants<float>::twoPi * pulse);
            case 7: return juce::jlimit(-1.f, 1.f, modWheel * 2.f - 1.f);
            case 8: return juce::jlimit(-1.f, 1.f, aftertouch * 2.f - 1.f);
            default:return 0.f;
        }
    }

    static float applyCurse(const CurseLane& lane, float x, float blood, float wraith, float hauntPhase)
    {
        x = juce::jlimit(-1.0f, 1.0f, x);

        switch (lane.curse)
        {
            case 1: // CORRUPT — asymmetric, polarity-dependent breakage.
            {
                const float bias = juce::jlimit(-1.f, 1.f, lane.corruptBias);
                const float skew = juce::jlimit(-1.5f, 1.5f,
                                                x + bias * (0.55f + 0.45f * x * x));
                const float positive = std::tanh(skew * (2.8f + std::abs(bias) * 2.2f));
                const float negative = 0.48f * std::tanh(skew * (5.2f + std::abs(bias) * 2.8f));
                return juce::jlimit(-1.f, 1.f, skew >= 0.f ? positive : negative);
            }

            case 2: // HAUNT — slow amplitude eclipse plus independent ghost motion.
            {
                const float rate = juce::jlimit(.05f, 8.f, lane.hauntRate);
                const float phase = juce::MathConstants<float>::twoPi * hauntPhase;
                const float eclipse = 0.28f + 0.72f * (0.5f + 0.5f * std::sin(phase * 0.37f + 1.2f));
                const float ghost = std::sin(phase + x * juce::MathConstants<float>::halfPi);
                const float ghostMix = 0.30f + 0.35f * (rate / 8.f);
                return juce::jlimit(-1.f, 1.f,
                                    x * eclipse * (1.f - ghostMix) + ghost * ghostMix);
            }

            case 3: // POSSESSION — true wavefolding of the modulation source.
            {
                const float fold = juce::jlimit(0.f, 1.f, lane.possessionFold);
                const float cycles = 1.0f + fold * 4.5f;
                const float possessed = std::sin(juce::MathConstants<float>::pi * x * cycles
                                                 + fold * 0.65f);
                return juce::jlimit(-1.f, 1.f, possessed);
            }

            case 4: // DECAY — turns smooth motion into dying, narrow peaks.
            {
                const float seconds = juce::jlimit(.05f, 8.f, lane.decayTime);
                const float fastness = 1.f - ((seconds - .05f) / 7.95f);
                const float exponent = 1.25f + fastness * 6.75f;
                const float shaped = std::pow(std::abs(x), exponent);
                const float life = 0.12f + 0.88f * juce::jlimit(0.f, 1.f, blood);
                return std::copysign(shaped * life, x);
            }

            case 5: // MADNESS — hard staircase / sample-and-hold-like contour.
            {
                const int steps = juce::jlimit(2, 32, (int)std::lround(lane.madnessSteps));
                const float unipolar = (x + 1.f) * .5f;
                const float quantized = std::round(unipolar * (float)(steps - 1)) / (float)(steps - 1);
                const float stepped = quantized * 2.f - 1.f;
                return juce::jlimit(-1.f, 1.f, stepped * 1.08f);
            }

            case 6: // BLOOD — envelope-hungry pumping with Wraith contamination.
            {
                const float hunger = juce::jlimit(0.f, 1.f, lane.bloodHunger);
                const float envelope = std::pow(juce::jlimit(0.0001f, 1.f, blood),
                                                0.30f + hunger * 2.70f);
                const float wraithSignal = wraith * 2.f - 1.f;
                const float fed = x * (0.18f + 1.82f * envelope)
                                + wraithSignal * hunger * 0.48f;
                return std::tanh(fed * (1.35f + hunger * 2.65f));
            }

            default: // CLEAN — transparent source.
                return x;
        }
    }
};

} // namespace horrorcastle

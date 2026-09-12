#include "AuroraEngine.h"
#include <cmath>

namespace horrorcastle {

float AuroraEngine::renderSample(VoiceState& s,
                                 float fundamentalHz,
                                 float fieldStrength,
                                 float aether,
                                 float expression,
                                 float velocity,
                                 double sr,
                                 float externalField,
                                 float fieldCoupling) noexcept
{
    if (sr <= 1.0 || fundamentalHz <= 0.0f) return 0.0f;
    fieldStrength = juce::jlimit(0.0f, 1.0f, fieldStrength);
    aether = juce::jlimit(0.0f, 1.0f, aether);
    expression = juce::jlimit(0.0f, 1.0f, expression);
    velocity = juce::jlimit(0.0f, 1.0f, velocity);
    externalField = CreatureStateBus::sanitize(externalField);
    fieldCoupling = std::isfinite(fieldCoupling) ? juce::jlimit(-1.0f, 1.0f, fieldCoupling) : 0.0f;

    const float ecologicalTarget = juce::jlimit(-1.0f, 1.0f, externalField * fieldCoupling);
    s.ecologicalField += (0.003f + 0.024f * aether * aether) * (ecologicalTarget - s.ecologicalField);
    const float fieldCurve = fieldStrength * fieldStrength;
    const float effectiveField = juce::jlimit(0.0f, 1.15f, fieldCurve + std::max(0.0f, s.ecologicalField) * 0.62f);

    constexpr std::array<float, Rings> ratios { 1.0f, 1.41421356f, 1.61803399f, 2.23606798f, 2.71828183f };

    float fieldSum = 0.0f;
    float chargeMotion = 0.0f;
    for (int i = 0; i < Rings; ++i)
    {
        auto& r = s.rings[(size_t)i];
        const float ringBias = 1.0f + 0.16f * (float)i;
        const float fieldRate = (0.018f + 0.10f * aether * aether) * ringBias * (1.0f + 0.36f * std::abs(s.ecologicalField));
        r.fieldPhase += fieldRate / (float)sr;
        r.fieldPhase -= std::floor(r.fieldPhase);

        const float phaseOffset = 0.71f * (float)i + s.ecologicalField * (0.21f + 0.12f * (float)i);
        const float polarity = (i & 1) ? -1.0f : 1.0f;
        const float targetCharge = std::sin(juce::MathConstants<float>::twoPi * r.fieldPhase + phaseOffset)
                                 * effectiveField * (0.22f + 0.78f * expression)
                                 * (1.0f + polarity * 0.12f * aether);
        const float slew = 0.00018f + 0.0034f * aether * aether + 0.0015f * std::abs(s.ecologicalField);
        chargeMotion += std::abs(targetCharge - r.charge);
        r.charge += slew * (targetCharge - r.charge);
        fieldSum += r.charge;
    }

    fieldSum /= (float)Rings;
    chargeMotion /= (float)Rings;
    s.sharedField += (0.0010f + 0.019f * aether * aether) * (fieldSum + s.ecologicalField * 0.24f - s.sharedField);

    float body = 0.0f;
    float luminous = 0.0f;
    float localInstability = 0.0f;
    for (int i = 0; i < Rings; ++i)
    {
        auto& r = s.rings[(size_t)i];
        const auto& left = s.rings[(size_t)((i + Rings - 1) % Rings)];
        const auto& right = s.rings[(size_t)((i + 1) % Rings)];
        const float localField = 0.5f * (left.charge + right.charge) - r.charge;
        localInstability += std::abs(localField);
        const float alignment = right.charge - left.charge;
        const float bend = juce::jlimit(-0.24f, 0.24f,
            effectiveField * (0.060f * localField + 0.048f * s.sharedField)
            + aether * 0.026f * alignment
            + s.ecologicalField * 0.030f * ((i & 1) ? -1.0f : 1.0f));

        const float f = fundamentalHz * ratios[(size_t)i] * (1.0f + bend);
        if (f > 0.0f && f < (float)sr * 0.46f)
        {
            r.phase += f / (float)sr;
            r.phase -= std::floor(r.phase);
        }

        const float phase = juce::MathConstants<float>::twoPi * r.phase;
        const float ring = std::sin(phase + r.charge * (0.28f + 1.70f * aether) + s.ecologicalField * 0.28f);
        const float upper = (f * 2.01f < (float)sr * 0.46f)
            ? std::sin(phase * 2.01f + r.charge * 1.1f) : 0.0f;
        r.memory += (0.0012f + 0.012f * (1.0f - aether)) * (ring - r.memory);
        body += (ring * (0.68f + 0.10f * fieldCurve) + r.memory * 0.26f + upper * 0.08f * effectiveField)
              / std::sqrt(1.0f + (float)i);
        luminous += std::sin(phase * 0.5f + s.sharedField * 2.6f) * r.charge;
    }

    localInstability /= (float)Rings;
    s.halo += (0.0007f + 0.009f * aether * aether) * (luminous - s.halo);
    const float corona = std::tanh(s.halo * body * (0.06f + 0.26f * effectiveField));
    const float aligned = body * (0.15f + 0.16f * effectiveField)
                        + s.halo * (0.04f + 0.22f * aether)
                        + s.sharedField * body * (0.03f + 0.18f * expression)
                        + s.ecologicalField * s.halo * 0.09f
                        + corona;
    const float out = juce::jlimit(-1.0f, 1.0f, std::tanh(aligned * (1.06f + 0.42f * velocity + 0.18f * effectiveField)));

    auto& bus = s.creatureState;
    bus.set(CreatureStateBus::Signal::Energy, std::abs(body) * 0.17f + std::abs(s.halo) * 0.24f);
    bus.set(CreatureStateBus::Signal::Pressure, std::abs(s.sharedField) * 0.68f + std::abs(s.ecologicalField) * 0.28f);
    bus.set(CreatureStateBus::Signal::Motion, chargeMotion * 1.8f);
    bus.set(CreatureStateBus::Signal::Instability, localInstability * 1.45f + std::abs(s.ecologicalField) * 0.40f);
    bus.set(CreatureStateBus::Signal::Event, 0.0f);
    bus.set(CreatureStateBus::Signal::Field, std::abs(s.sharedField) + std::abs(s.ecologicalField) * 0.68f);

    if (!std::isfinite(out)) { s = VoiceState{}; return 0.0f; }
    return out;
}

} // namespace horrorcastle

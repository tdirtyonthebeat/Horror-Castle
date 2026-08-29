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
    s.ecologicalField += (0.004f + 0.018f * aether) * (ecologicalTarget - s.ecologicalField);
    const float effectiveField = juce::jlimit(0.0f, 1.0f, fieldStrength + std::max(0.0f, s.ecologicalField) * 0.55f);

    constexpr std::array<float, Rings> ratios { 1.0f, 1.41421356f, 1.61803399f, 2.23606798f, 2.71828183f };

    float fieldSum = 0.0f;
    float chargeMotion = 0.0f;
    for (int i = 0; i < Rings; ++i)
    {
        auto& r = s.rings[(size_t)i];
        const float fieldRate = (0.031f + 0.067f * aether) * (1.0f + 0.11f * (float)i) * (1.0f + 0.28f * std::abs(s.ecologicalField));
        r.fieldPhase += fieldRate / (float)sr;
        r.fieldPhase -= std::floor(r.fieldPhase);

        const float phaseOffset = 0.71f * (float)i + s.ecologicalField * (0.18f + 0.09f * (float)i);
        const float targetCharge = std::sin(juce::MathConstants<float>::twoPi * r.fieldPhase + phaseOffset)
                                 * effectiveField * (0.30f + 0.70f * expression);
        const float slew = 0.00025f + 0.0028f * aether + 0.0012f * std::abs(s.ecologicalField);
        chargeMotion += std::abs(targetCharge - r.charge);
        r.charge += slew * (targetCharge - r.charge);
        fieldSum += r.charge;
    }

    fieldSum /= (float)Rings;
    chargeMotion /= (float)Rings;
    s.sharedField += (0.0015f + 0.014f * aether) * (fieldSum + s.ecologicalField * 0.18f - s.sharedField);

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
        const float bend = juce::jlimit(-0.18f, 0.18f,
            effectiveField * (0.052f * localField + 0.035f * s.sharedField)
            + aether * 0.018f * (right.charge - left.charge)
            + s.ecologicalField * 0.022f * ((i & 1) ? -1.0f : 1.0f));

        const float f = fundamentalHz * ratios[(size_t)i] * (1.0f + bend);
        if (f > 0.0f && f < (float)sr * 0.46f)
        {
            r.phase += f / (float)sr;
            r.phase -= std::floor(r.phase);
        }

        const float phase = juce::MathConstants<float>::twoPi * r.phase;
        const float ring = std::sin(phase + r.charge * (0.35f + 1.25f * aether) + s.ecologicalField * 0.21f);
        r.memory += (0.0018f + 0.009f * (1.0f - aether)) * (ring - r.memory);
        body += (ring * 0.78f + r.memory * 0.22f) / std::sqrt(1.0f + (float)i);
        luminous += std::sin(phase * 0.5f + s.sharedField * 2.0f) * r.charge;
    }

    localInstability /= (float)Rings;
    s.halo += (0.001f + 0.006f * aether) * (luminous - s.halo);
    const float aligned = body * (0.19f + 0.12f * effectiveField)
                        + s.halo * (0.05f + 0.16f * aether)
                        + s.sharedField * body * (0.04f + 0.14f * expression)
                        + s.ecologicalField * s.halo * 0.06f;
    const float out = juce::jlimit(-1.0f, 1.0f, std::tanh(aligned * (1.10f + 0.35f * velocity)));

    auto& bus = s.creatureState;
    bus.set(CreatureStateBus::Signal::Energy, std::abs(body) * 0.18f + std::abs(s.halo) * 0.20f);
    bus.set(CreatureStateBus::Signal::Pressure, std::abs(s.sharedField) * 0.65f + std::abs(s.ecologicalField) * 0.25f);
    bus.set(CreatureStateBus::Signal::Motion, chargeMotion * 1.6f);
    bus.set(CreatureStateBus::Signal::Instability, localInstability * 1.3f + std::abs(s.ecologicalField) * 0.35f);
    bus.set(CreatureStateBus::Signal::Event, 0.0f);
    bus.set(CreatureStateBus::Signal::Field, std::abs(s.sharedField) + std::abs(s.ecologicalField) * 0.6f);

    if (!std::isfinite(out)) { s = VoiceState{}; return 0.0f; }
    return out;
}

} // namespace horrorcastle

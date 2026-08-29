#include "PoltergeistEngine.h"
#include <cmath>

namespace horrorcastle {
namespace {
float noise(uint32_t& s) noexcept
{
    s ^= s << 13; s ^= s >> 17; s ^= s << 5;
    return (float)s / 2147483648.0f - 1.0f;
}
}

float PoltergeistEngine::renderSample(VoiceState& s,
                                      float fundamentalHz,
                                      float charge,
                                      float dread,
                                      float expression,
                                      float velocity,
                                      double sr) noexcept
{
    if (sr <= 1.0 || fundamentalHz <= 0.0f) return 0.0f;
    charge = juce::jlimit(0.0f, 1.0f, charge);
    dread = juce::jlimit(0.0f, 1.0f, dread);
    expression = juce::jlimit(0.0f, 1.0f, expression);
    velocity = juce::jlimit(0.0f, 1.0f, velocity);

    constexpr std::array<float, Plates> ratios { 1.0f, 1.487f, 2.071f, 2.913f };
    const float chargingRate = (0.00035f + 0.0022f * expression) * (0.45f + 0.55f * velocity);
    const float leak = 0.00008f + 0.00045f * (1.0f - dread);

    float meanCharge = 0.0f;
    float maxPotential = 0.0f;
    float motionMean = 0.0f;
    for (int i = 0; i < Plates; ++i)
    {
        auto& p = s.plates[(size_t)i];
        const float polarity = (i & 1) ? -1.0f : 1.0f;
        const float target = polarity * charge * (0.28f + 0.72f * expression);
        p.charge += chargingRate * (target - p.charge);
        p.charge *= (1.0f - leak);
        p.charge = juce::jlimit(-1.0f, 1.0f, p.charge);
        meanCharge += p.charge;
        motionMean += std::abs(target - p.charge);

        const auto& n = s.plates[(size_t)((i + 1) % Plates)];
        maxPotential = std::max(maxPotential, std::abs(p.charge - n.charge));
    }
    meanCharge /= (float)Plates;
    motionMean /= (float)Plates;
    s.field += (0.002f + 0.012f * charge) * (meanCharge - s.field);

    const float dischargeThreshold = 0.78f - 0.28f * dread - 0.10f * expression;
    const float priorArc = s.arcEnvelope;
    if (maxPotential > dischargeThreshold)
    {
        const float excess = maxPotential - dischargeThreshold;
        s.arcEnvelope = std::max(s.arcEnvelope, juce::jlimit(0.0f, 0.85f, excess * (0.30f + 0.55f * dread)));
    }
    s.arcEnvelope *= 0.991f - 0.002f * dread;

    float body = 0.0f;
    for (int i = 0; i < Plates; ++i)
    {
        auto& p = s.plates[(size_t)i];
        const auto& n = s.plates[(size_t)((i + 1) % Plates)];
        const float electrostatic = p.charge * n.charge;
        const float repulsion = (p.charge - n.charge) * charge;
        const float bend = juce::jlimit(-0.22f, 0.22f,
            -0.075f * electrostatic + 0.040f * repulsion + 0.018f * s.field * dread);

        const float f = fundamentalHz * ratios[(size_t)i] * (1.0f + bend);
        if (f > 0.0f && f < (float)sr * 0.46f)
        {
            p.phase += f / (float)sr;
            p.phase -= std::floor(p.phase);
        }

        const float phase = juce::MathConstants<float>::twoPi * p.phase;
        const float plate = std::sin(phase)
                          + (0.12f + 0.18f * charge) * std::sin(phase * 2.0f + p.charge * 1.7f);
        p.memory += (0.004f + 0.018f * (1.0f - dread)) * (plate - p.memory);
        body += (plate * 0.72f + p.memory * 0.28f) / (1.0f + 0.30f * (float)i);
    }

    const float arc = noise(s.rng) * s.arcEnvelope * (0.20f + 0.45f * velocity);
    const float stressed = body * (0.20f + 0.16f * charge)
                         + arc
                         + std::tanh(body * s.field * (0.08f + 0.30f * dread));
    const float out = juce::jlimit(-1.0f, 1.0f, std::tanh(stressed * 1.35f));

    auto& bus = s.creatureState;
    const float chargeEnergy = juce::jlimit(0.0f, 1.0f, maxPotential * 0.72f + std::abs(s.field) * 0.28f);
    const float instability = juce::jlimit(0.0f, 1.0f, std::max(0.0f, maxPotential - dischargeThreshold) * 2.2f + s.arcEnvelope * 0.45f);
    const float arcEvent = juce::jlimit(0.0f, 1.0f, std::max(0.0f, s.arcEnvelope - priorArc) * 8.0f);
    bus.set(CreatureStateBus::Signal::Energy, chargeEnergy);
    bus.set(CreatureStateBus::Signal::Pressure, maxPotential * 0.5f);
    bus.set(CreatureStateBus::Signal::Motion, motionMean * 2.0f);
    bus.set(CreatureStateBus::Signal::Instability, instability);
    bus.set(CreatureStateBus::Signal::Event, arcEvent);
    bus.set(CreatureStateBus::Signal::Field, std::abs(s.field) + chargeEnergy * 0.25f);

    if (!std::isfinite(out)) { s = VoiceState{}; return 0.0f; }
    return out;
}

} // namespace horrorcastle

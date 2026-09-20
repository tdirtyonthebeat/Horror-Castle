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
    const float chargeCurve = charge * charge;
    const float chargingRate = (0.00022f + 0.0032f * expression) * (0.38f + 0.62f * velocity) * (0.7f + 0.7f * chargeCurve);
    const float leak = 0.00005f + 0.00062f * (1.0f - dread);

    float meanCharge = 0.0f;
    float maxPotential = 0.0f;
    float motionMean = 0.0f;
    for (int i = 0; i < Plates; ++i)
    {
        auto& p = s.plates[(size_t)i];
        const float polarity = (i & 1) ? -1.0f : 1.0f;
        const float plateBias = 0.86f + 0.14f * (float)i / (float)(Plates - 1);
        const float target = polarity * chargeCurve * plateBias * (0.22f + 0.78f * expression);
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
    s.field += (0.0012f + 0.017f * chargeCurve) * (meanCharge - s.field);

    // Dread lowers the arc threshold and lengthens arc persistence. High charge
    // therefore stops sounding like "more brightness" and enters a discharge regime.
    const float dischargeThreshold = 0.86f - 0.36f * dread - 0.16f * expression - 0.08f * chargeCurve;
    const float priorArc = s.arcEnvelope;
    if (maxPotential > dischargeThreshold)
    {
        const float excess = maxPotential - dischargeThreshold;
        const float ignition = excess * (0.34f + 0.78f * dread) * (0.7f + 0.6f * chargeCurve);
        s.arcEnvelope = std::max(s.arcEnvelope, juce::jlimit(0.0f, 0.95f, ignition));
    }
    s.arcEnvelope *= 0.994f - 0.0045f * (1.0f - dread);

    float body = 0.0f;
    for (int i = 0; i < Plates; ++i)
    {
        auto& p = s.plates[(size_t)i];
        const auto& n = s.plates[(size_t)((i + 1) % Plates)];
        const float electrostatic = p.charge * n.charge;
        const float repulsion = (p.charge - n.charge) * chargeCurve;
        const float bend = juce::jlimit(-0.28f, 0.28f,
            -0.095f * electrostatic + 0.060f * repulsion + 0.026f * s.field * dread);

        const float f = fundamentalHz * ratios[(size_t)i] * (1.0f + bend);
        if (f > 0.0f && f < (float)sr * 0.46f)
        {
            p.phase += f / (float)sr;
            p.phase -= std::floor(p.phase);
        }

        const float phase = juce::MathConstants<float>::twoPi * p.phase;
        const float plate = std::sin(phase)
                          + (0.08f + 0.26f * chargeCurve) * std::sin(phase * 2.0f + p.charge * 2.1f)
                          + (0.02f + 0.10f * dread) * std::sin(phase * 3.01f + s.field * 1.6f);
        p.memory += (0.002f + 0.022f * (1.0f - dread)) * (plate - p.memory);
        body += (plate * 0.66f + p.memory * 0.34f) / (1.0f + 0.28f * (float)i);
    }

    const float arcNoise = noise(s.rng);
    const float arc = arcNoise * s.arcEnvelope * (0.18f + 0.52f * velocity);
    const float sparkTone = std::sin(juce::MathConstants<float>::twoPi * s.plates[0].phase * 7.13f)
                          * s.arcEnvelope * (0.03f + 0.16f * chargeCurve);
    const float stressed = body * (0.16f + 0.22f * chargeCurve)
                         + arc
                         + sparkTone
                         + std::tanh(body * s.field * (0.06f + 0.42f * dread));
    const float out = juce::jlimit(-1.0f, 1.0f, std::tanh(stressed * 1.42f));

    auto& bus = s.creatureState;
    const float chargeEnergy = juce::jlimit(0.0f, 1.0f, maxPotential * 0.70f + std::abs(s.field) * 0.30f);
    const float instability = juce::jlimit(0.0f, 1.0f, std::max(0.0f, maxPotential - dischargeThreshold) * 2.6f + s.arcEnvelope * 0.55f);
    const float arcEvent = juce::jlimit(0.0f, 1.0f, std::max(0.0f, s.arcEnvelope - priorArc) * 9.0f);
    bus.set(CreatureStateBus::Signal::Energy, chargeEnergy);
    bus.set(CreatureStateBus::Signal::Pressure, maxPotential * 0.55f);
    bus.set(CreatureStateBus::Signal::Motion, motionMean * 2.0f);
    bus.set(CreatureStateBus::Signal::Instability, instability);
    bus.set(CreatureStateBus::Signal::Event, arcEvent);
    bus.set(CreatureStateBus::Signal::Field, std::abs(s.field) + chargeEnergy * 0.28f);

    if (!std::isfinite(out)) { s = VoiceState{}; return 0.0f; }
    return out;
}

} // namespace horrorcastle

#include "CurseChain.h"
#include <cstdlib>

namespace HorrorCastle
{
    CurseChain::CurseChain()
    {
        reset();
    }

    void CurseChain::reset()
    {
        for (auto& lane : lanes)
            lane = {};
        laneValues.fill(0.0f);
        destinations.fill(0.0f);
        chainEnergy = 0.0f;
        velocityHeld = keyHeld = randomHeld = 0.0f;
    }

    void CurseChain::setSampleRate(float sr)
    {
        sampleRate = std::max(1000.0f, sr);
    }

    void CurseChain::noteOn(float velocity, float key01, float random01)
    {
        velocityHeld = std::clamp(velocity, 0.0f, 1.0f);
        keyHeld = std::clamp(key01, 0.0f, 1.0f);
        randomHeld = std::clamp(random01, 0.0f, 1.0f);

        for (auto& lane : lanes)
        {
            lane.phase = 0.0f;
            lane.decay = 1.0f;
            lane.pulsePhase = 0.0f;
            lane.heldRandom = randomHeld * 2.0f - 1.0f;
        }
    }

    void CurseChain::noteOff()
    {
        // Decay curses naturally; no hard reset so tails can remain audible.
    }

    float CurseChain::clampBipolar(float v)
    {
        return std::clamp(v, -1.0f, 1.0f);
    }

    float CurseChain::applyCurse(CurseType curse, float x, CurseLane& lane,
                                 float blood, float wraith, float dt)
    {
        x = clampBipolar(x);

        switch (curse)
        {
            case CurseType::Clean:
                return x;

            case CurseType::Corrupt:
            {
                // Asymmetric soft saturation: positive values corrupt faster.
                const float asym = x + 0.22f * x * x;
                return std::tanh(asym * 2.15f);
            }

            case CurseType::Haunt:
            {
                // Slow moving phase wobble creates an unstable spectral drift.
                lane.phase += dt * (0.37f + 0.83f * std::abs(x));
                if (lane.phase > 6.28318530718f)
                    lane.phase -= 6.28318530718f;
                return clampBipolar(x + 0.22f * std::sin(lane.phase));
            }

            case CurseType::Possession:
            {
                // Fold around the center, then restore sign.
                const float folded = std::abs(x) * (1.0f - 0.35f * std::abs(x));
                return std::copysign(folded, x);
            }

            case CurseType::Decay:
            {
                lane.decay = std::max(0.0f, lane.decay - dt * (0.18f + 0.42f * blood));
                return x * lane.decay;
            }

            case CurseType::Madness:
            {
                // Quantized steps with a little moving hysteresis.
                const float steps = 7.0f;
                const float q = std::round(((x + 1.0f) * 0.5f) * steps) / steps;
                return clampBipolar(q * 2.0f - 1.0f);
            }

            case CurseType::Blood:
            {
                // Envelope-driven multiplication with a wraith-dependent bias.
                const float gain = 0.15f + 1.85f * blood;
                const float bias = 0.20f * (wraith * 2.0f - 1.0f);
                return clampBipolar(x * gain + bias);
            }
        }

        return x;
    }

    void CurseChain::clearDestinations()
    {
        destinations.fill(0.0f);
    }

    void CurseChain::process(float blood, float wraith, float velocity,
                             float key01, float random01, float dt)
    {
        clearDestinations();

        blood = std::clamp(blood, 0.0f, 1.0f);
        wraith = std::clamp(wraith, 0.0f, 1.0f);
        velocity = std::clamp(velocity, 0.0f, 1.0f);
        key01 = std::clamp(key01, 0.0f, 1.0f);
        random01 = std::clamp(random01, 0.0f, 1.0f);

        // First pass: evaluate every lane independently.
        for (int i = 0; i < maxLanes; ++i)
        {
            auto& lane = lanes[(size_t)i];

            if (!lane.enabled || lane.destination == ModDestination::None)
            {
                laneValues[(size_t)i] = 0.0f;
                continue;
            }

            float source = 0.0f;

            switch (lane.source)
            {
                case ModSource::None:     source = 0.0f; break;
                case ModSource::Blood:    source = blood * 2.0f - 1.0f; break;
                case ModSource::Wraith:   source = wraith * 2.0f - 1.0f; break;
                case ModSource::Velocity: source = velocity * 2.0f - 1.0f; break;
                case ModSource::Key:      source = key01 * 2.0f - 1.0f; break;
                case ModSource::Random:   source = random01 * 2.0f - 1.0f; break;
                case ModSource::Pulse:
                {
                    lane.pulsePhase += dt * 1.7f;
                    if (lane.pulsePhase > 1.0f)
                        lane.pulsePhase -= 1.0f;
                    source = lane.pulsePhase < 0.5f ? 1.0f : -1.0f;
                    break;
                }
            }

            float cursed = applyCurse(lane.curse, source, lane, blood, wraith, dt);

            // Small internal feedback: a lane's prior value slightly affects
            // its next value, creating the "living patch" behavior without
            // allowing runaway feedback.
            cursed += 0.10f * laneValues[(size_t)i];
            cursed = clampBipolar(cursed);

            laneValues[(size_t)i] = cursed * lane.amount;
        }

        // Second pass: controlled chain coupling.
        // Any lane targeting CurseDepth contributes to the depth of later lanes.
        float depthMod[maxLanes] {};
        for (int i = 0; i < maxLanes; ++i)
        {
            const auto& lane = lanes[(size_t)i];
            if (!lane.enabled || lane.destination != ModDestination::CurseDepth)
                continue;

            for (int j = i + 1; j < maxLanes; ++j)
                depthMod[j] += laneValues[(size_t)i] * 0.35f;
        }

        // Final destination accumulation.
        for (int i = 0; i < maxLanes; ++i)
        {
            const auto& lane = lanes[(size_t)i];
            if (!lane.enabled || lane.destination == ModDestination::None ||
                lane.destination == ModDestination::CurseDepth)
                continue;

            const float effective = laneValues[(size_t)i] *
                                     std::clamp(1.0f + depthMod[i], 0.0f, 2.0f);

            destinations[(size_t)lane.destination] += effective;
        }

        // Keep the total sane when multiple lanes attack the same destination.
        for (auto& value : destinations)
            value = clampBipolar(value);

        chainEnergy = 0.995f * chainEnergy + 0.005f *
                      std::min(1.0f, std::abs(destinations[(size_t)ModDestination::FMDepth])
                                      + std::abs(destinations[(size_t)ModDestination::FilterDrive]));
    }

    float CurseChain::getDestination(ModDestination d) const noexcept
    {
        const auto idx = static_cast<size_t>(d);
        return idx < destinations.size() ? destinations[idx] : 0.0f;
    }

    float CurseChain::getLaneValue(int index) const noexcept
    {
        return (index >= 0 && index < maxLanes) ? laneValues[(size_t)index] : 0.0f;
    }

    const CurseLane& CurseChain::getLane(int index) const noexcept
    {
        static const CurseLane empty {};
        return (index >= 0 && index < maxLanes) ? lanes[(size_t)index] : empty;
    }

    void CurseChain::setLane(int index, const CurseLane& lane)
    {
        if (index >= 0 && index < maxLanes)
            lanes[(size_t)index] = lane;
    }
}

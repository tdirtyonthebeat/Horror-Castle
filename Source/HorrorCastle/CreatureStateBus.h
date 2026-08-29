#pragma once
#include <JuceHeader.h>
#include <array>
#include <cstddef>
#include <cstdint>

namespace horrorcastle {

// Stable, normalized abstraction boundary for cross-creature physical coupling.
// Producers publish physical meaning, never implementation-specific state.
class CreatureStateBus
{
public:
    enum class Signal : std::uint8_t { Energy, Pressure, Motion, Instability, Event, Field, Count };
    static constexpr std::size_t SignalCount = static_cast<std::size_t>(Signal::Count);

    struct State {
        std::array<float, SignalCount> value{};
        float get(Signal signal) const noexcept { return value[static_cast<std::size_t>(signal)]; }
        void set(Signal signal, float v) noexcept { value[static_cast<std::size_t>(signal)] = juce::jlimit(0.0f, 1.0f, v); }
        void clear() noexcept { value.fill(0.0f); }
    };

    struct Coupling {
        Signal source = Signal::Energy;
        Signal destination = Signal::Energy;
        float amount = 0.0f; // bipolar routing depth [-1, +1]
    };

    static float sanitize(float v) noexcept
    {
        return std::isfinite(v) ? juce::jlimit(0.0f, 1.0f, v) : 0.0f;
    }

    static float apply(float destinationValue, float sourceValue, float amount) noexcept
    {
        destinationValue = sanitize(destinationValue);
        sourceValue = sanitize(sourceValue);
        amount = std::isfinite(amount) ? juce::jlimit(-1.0f, 1.0f, amount) : 0.0f;
        return juce::jlimit(0.0f, 1.0f, destinationValue + sourceValue * amount);
    }

    // One-pole smoothing for continuous physical state. EVENT should normally bypass
    // this and be treated as a short bounded pulse by the producer/consumer.
    static float smooth(float previous, float target, float coefficient) noexcept
    {
        previous = sanitize(previous);
        target = sanitize(target);
        coefficient = std::isfinite(coefficient) ? juce::jlimit(0.0f, 1.0f, coefficient) : 0.0f;
        return previous + coefficient * (target - previous);
    }
};

} // namespace horrorcastle

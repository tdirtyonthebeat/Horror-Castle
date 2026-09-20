#pragma once
#include "CreatureStateBus.h"
#include <array>
#include <cstddef>
#include <cstdint>

namespace horrorcastle {

// Fixed-capacity, allocation-free routing layer for inter-creature ecology.
// Routing always reads from an immutable source snapshot and writes to a fresh
// destination inbox, so cycles cannot recurse within the same processing frame.
class CreatureRoutingMatrix
{
public:
    static constexpr std::size_t MaxCreatures = 16;
    static constexpr std::size_t MaxRoutes = 32;

    struct Route {
        std::uint8_t sourceCreature = 0;
        CreatureStateBus::Signal sourceSignal = CreatureStateBus::Signal::Energy;
        std::uint8_t destinationCreature = 0;
        CreatureStateBus::Signal destinationSignal = CreatureStateBus::Signal::Energy;
        float amount = 0.0f;
        float smoothing = 1.0f;
        bool enabled = false;
    };

    using StateArray = std::array<CreatureStateBus::State, MaxCreatures>;

    void clear() noexcept
    {
        routes.fill(Route{});
        routeMemory.fill(0.0f);
    }

    bool setRoute(std::size_t index, Route route) noexcept
    {
        if (index >= MaxRoutes || route.sourceCreature >= MaxCreatures || route.destinationCreature >= MaxCreatures)
            return false;
        route.amount = std::isfinite(route.amount) ? juce::jlimit(-1.0f, 1.0f, route.amount) : 0.0f;
        route.smoothing = std::isfinite(route.smoothing) ? juce::jlimit(0.0f, 1.0f, route.smoothing) : 1.0f;
        routes[index] = route;
        if (!route.enabled) routeMemory[index] = 0.0f;
        return true;
    }

    const Route& getRoute(std::size_t index) const noexcept { return routes[index]; }

    void process(const StateArray& sourceSnapshot, StateArray& destinationInbox) noexcept
    {
        for (auto& state : destinationInbox) state.clear();

        for (std::size_t i = 0; i < MaxRoutes; ++i)
        {
            const auto& route = routes[i];
            if (!route.enabled || route.amount == 0.0f) continue;

            const float source = sourceSnapshot[route.sourceCreature].get(route.sourceSignal);
            float routed = source;
            if (route.sourceSignal != CreatureStateBus::Signal::Event)
            {
                routeMemory[i] = CreatureStateBus::smooth(routeMemory[i], source, route.smoothing);
                routed = routeMemory[i];
            }
            else
            {
                routeMemory[i] = source; // EVENT remains a pulse; no artificial tail.
            }

            auto& target = destinationInbox[route.destinationCreature];
            const float current = target.get(route.destinationSignal);
            target.set(route.destinationSignal, CreatureStateBus::apply(current, routed, route.amount));
        }
    }

private:
    std::array<Route, MaxRoutes> routes{};
    std::array<float, MaxRoutes> routeMemory{};
};

} // namespace horrorcastle

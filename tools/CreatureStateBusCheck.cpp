#include <JuceHeader.h>
#include "../Source/HorrorCastle/CreatureStateBus.h"
#include "../Source/HorrorCastle/CreatureRoutingMatrix.h"
#include <cmath>
#include <iostream>
#include <limits>

using horrorcastle::CreatureStateBus;
using horrorcastle::CreatureRoutingMatrix;

namespace {
bool near(float a, float b, float eps = 1.0e-5f) { return std::abs(a - b) <= eps; }
int fail(const char* message) { std::cerr << "CreatureStateBusCheck: " << message << '\n'; return 1; }
}

int main()
{
    CreatureStateBus::State state;
    state.set(CreatureStateBus::Signal::Pressure, 1.7f);
    state.set(CreatureStateBus::Signal::Motion, -0.4f);
    if (!near(state.get(CreatureStateBus::Signal::Pressure), 1.0f)) return fail("state upper bound failed");
    if (!near(state.get(CreatureStateBus::Signal::Motion), 0.0f)) return fail("state lower bound failed");

    if (!near(CreatureStateBus::apply(0.25f, 0.5f, 0.5f), 0.5f)) return fail("positive coupling failed");
    if (!near(CreatureStateBus::apply(0.25f, 0.5f, -0.5f), 0.0f)) return fail("negative coupling failed");
    if (!near(CreatureStateBus::apply(0.9f, 1.0f, 1.0f), 1.0f)) return fail("coupling bound failed");

    const float nan = std::numeric_limits<float>::quiet_NaN();
    if (!near(CreatureStateBus::sanitize(nan), 0.0f)) return fail("non-finite sanitize failed");
    if (!near(CreatureStateBus::apply(nan, nan, nan), 0.0f)) return fail("non-finite coupling failed");

    float smoothed = 0.0f;
    for (int i = 0; i < 32; ++i) smoothed = CreatureStateBus::smooth(smoothed, 1.0f, 0.125f);
    if (!(smoothed > 0.9f && smoothed < 1.0f)) return fail("smoothing convergence failed");

    CreatureRoutingMatrix matrix;
    CreatureRoutingMatrix::Route continuous;
    continuous.sourceCreature = 0;
    continuous.sourceSignal = CreatureStateBus::Signal::Pressure;
    continuous.destinationCreature = 1;
    continuous.destinationSignal = CreatureStateBus::Signal::Field;
    continuous.amount = 0.75f;
    continuous.smoothing = 0.5f;
    continuous.enabled = true;
    if (!matrix.setRoute(0, continuous)) return fail("continuous route rejected");

    CreatureRoutingMatrix::Route eventRoute;
    eventRoute.sourceCreature = 0;
    eventRoute.sourceSignal = CreatureStateBus::Signal::Event;
    eventRoute.destinationCreature = 1;
    eventRoute.destinationSignal = CreatureStateBus::Signal::Pressure;
    eventRoute.amount = 0.8f;
    eventRoute.enabled = true;
    if (!matrix.setRoute(1, eventRoute)) return fail("event route rejected");

    CreatureRoutingMatrix::Route cycleBack;
    cycleBack.sourceCreature = 1;
    cycleBack.sourceSignal = CreatureStateBus::Signal::Field;
    cycleBack.destinationCreature = 0;
    cycleBack.destinationSignal = CreatureStateBus::Signal::Motion;
    cycleBack.amount = 1.0f;
    cycleBack.enabled = true;
    if (!matrix.setRoute(2, cycleBack)) return fail("cycle route rejected");

    CreatureRoutingMatrix::StateArray sources{}, inbox{};
    sources[0].set(CreatureStateBus::Signal::Pressure, 1.0f);
    sources[0].set(CreatureStateBus::Signal::Event, 0.5f);
    sources[1].set(CreatureStateBus::Signal::Field, 0.25f);
    matrix.process(sources, inbox);

    if (!near(inbox[1].get(CreatureStateBus::Signal::Field), 0.375f)) return fail("continuous routing/smoothing failed");
    if (!near(inbox[1].get(CreatureStateBus::Signal::Pressure), 0.4f)) return fail("EVENT routing failed");
    if (!near(inbox[0].get(CreatureStateBus::Signal::Motion), 0.25f)) return fail("snapshot cycle semantics failed");

    sources = inbox;
    matrix.process(sources, inbox);
    if (!(inbox[0].get(CreatureStateBus::Signal::Motion) >= 0.0f && inbox[0].get(CreatureStateBus::Signal::Motion) <= 1.0f)) return fail("cycle bound failed");

    CreatureRoutingMatrix::Route invalid;
    invalid.sourceCreature = static_cast<std::uint8_t>(CreatureRoutingMatrix::MaxCreatures);
    if (matrix.setRoute(3, invalid)) return fail("invalid creature index accepted");

    std::cout << "CreatureStateBusCheck passed: normalized state, bipolar coupling, finite safety, smoothing, routing matrix, EVENT semantics, cycle-safe snapshots.\n";
    return 0;
}

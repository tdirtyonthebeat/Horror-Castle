#include <JuceHeader.h>
#include "../Source/HorrorCastle/CreatureStateBus.h"
#include <cmath>
#include <iostream>
#include <limits>

using horrorcastle::CreatureStateBus;

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

    std::cout << "CreatureStateBusCheck passed: normalized state, bipolar coupling, bounds, finite safety, smoothing.\n";
    return 0;
}

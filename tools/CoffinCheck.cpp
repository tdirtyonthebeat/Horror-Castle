#include <JuceHeader.h>
#include "../Source/HorrorCastle/CoffinBodyEngine.h"
#include <cmath>
#include <iostream>
#include <vector>

namespace {
using horrorcastle::CoffinBodyEngine;

std::vector<float> render(float lidOpen, float dread, float expression, float velocity = 0.82f)
{
    constexpr double sampleRate = 48000.0;
    constexpr int samples = 48000;
    CoffinBodyEngine engine;
    CoffinBodyEngine::VoiceState state;
    std::vector<float> out;
    out.reserve(samples);
    for (int i = 0; i < samples; ++i)
        out.push_back(engine.renderSample(state, 110.0f, lidOpen, dread,
                                          expression, velocity, sampleRate));
    return out;
}

bool finite(const std::vector<float>& x)
{
    for (const auto s : x)
        if (!std::isfinite(s) || std::abs(s) > 1.001f)
            return false;
    return true;
}

float rms(const std::vector<float>& x)
{
    double sum = 0.0;
    for (const auto s : x) sum += (double) s * (double) s;
    return x.empty() ? 0.0f : (float) std::sqrt(sum / (double) x.size());
}

float difference(const std::vector<float>& a, const std::vector<float>& b)
{
    const auto n = std::min(a.size(), b.size());
    double sum = 0.0;
    for (size_t i = 0; i < n; ++i)
    {
        const double d = (double) a[i] - (double) b[i];
        sum += d * d;
    }
    const float absolute = n ? (float) std::sqrt(sum / (double) n) : 0.0f;
    return absolute / std::max(0.0001f, std::max(rms(a), rms(b)));
}
}

int main()
{
    int failures = 0;
    auto check = [&](bool ok, const char* name)
    {
        std::cout << (ok ? "PASS  " : "FAIL  ") << name << '\n';
        if (!ok) ++failures;
    };

    const auto closed = render(0.05f, 0.55f, 0.45f);
    const auto open = render(0.95f, 0.55f, 0.45f);
    const float lidDifference = difference(closed, open);
    std::cout << "INFO  COFFIN lid difference=" << lidDifference
              << " rms(closed/open)=" << rms(closed) << "/" << rms(open) << '\n';
    check(finite(closed) && finite(open)
              && rms(closed) > 0.001f && rms(open) > 0.001f,
          "COFFIN remains bounded and audible");
    check(lidDifference > 0.15f,
          "COFFIN LID changes enclosure geometry and plate coupling");

    const auto shallow = render(0.35f, 0.05f, 0.45f);
    const auto abyss = render(0.35f, 0.95f, 0.45f);
    const float dreadDifference = difference(shallow, abyss);
    std::cout << "INFO  COFFIN DREAD difference=" << dreadDifference << '\n';
    check(finite(abyss) && dreadDifference > 0.15f,
          "COFFIN DREAD changes cavity volume and sympathetic coupling");

    const auto still = render(0.42f, 0.72f, 0.03f);
    const auto pressed = render(0.42f, 0.72f, 1.00f);
    const float expressionDifference = difference(still, pressed);
    std::cout << "INFO  COFFIN expression difference=" << expressionDifference << '\n';
    check(finite(pressed) && expressionDifference > 0.15f,
          "COFFIN expression changes scrape, string wake-up, and rattle behavior");

    const auto extreme = render(0.0f, 1.0f, 1.0f, 1.0f);
    check(finite(extreme), "COFFIN extreme coupling remains finite");

    std::cout << (failures == 0
        ? "\nHorror Castle COFFIN check passed.\n"
        : "\nHorror Castle COFFIN check FAILED.\n");
    return failures == 0 ? 0 : 1;
}

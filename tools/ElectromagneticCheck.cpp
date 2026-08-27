#include <JuceHeader.h>
#include "../Source/HorrorCastle/PoltergeistEngine.h"
#include "../Source/HorrorCastle/AuroraEngine.h"
#include <cmath>
#include <iostream>
#include <vector>

namespace {
using horrorcastle::PoltergeistEngine;
using horrorcastle::AuroraEngine;
constexpr double sampleRate = 48000.0;
constexpr int samples = 24000;

std::vector<float> renderPoltergeist(float charge, float dread, float expression)
{
    PoltergeistEngine e; PoltergeistEngine::VoiceState s; std::vector<float> out; out.reserve(samples);
    for (int i = 0; i < samples; ++i)
        out.push_back(e.renderSample(s, 110.0f, charge, dread, expression, 0.82f, sampleRate));
    return out;
}

std::vector<float> renderAurora(float field, float aether, float expression)
{
    AuroraEngine e; AuroraEngine::VoiceState s; std::vector<float> out; out.reserve(samples);
    for (int i = 0; i < samples; ++i)
        out.push_back(e.renderSample(s, 110.0f, field, aether, expression, 0.82f, sampleRate));
    return out;
}

bool finite(const std::vector<float>& x)
{
    for (float v : x) if (!std::isfinite(v) || std::abs(v) > 1.001f) return false;
    return true;
}

float rms(const std::vector<float>& x)
{
    double z = 0.0; for (float v : x) z += (double)v * v;
    return x.empty() ? 0.0f : (float)std::sqrt(z / x.size());
}

float diff(const std::vector<float>& a, const std::vector<float>& b)
{
    const auto n = std::min(a.size(), b.size()); double z = 0.0;
    for (size_t i = 0; i < n; ++i) { const double d = (double)a[i] - b[i]; z += d * d; }
    const float d = n ? (float)std::sqrt(z / n) : 0.0f;
    return d / std::max(0.0001f, std::max(rms(a), rms(b)));
}
}

int main()
{
    int failures = 0;
    auto check = [&](bool ok, const char* name) {
        std::cout << (ok ? "PASS  " : "FAIL  ") << name << '\n';
        if (!ok) ++failures;
    };

    auto lowCharge = renderPoltergeist(0.05f, 0.60f, 0.60f);
    auto highCharge = renderPoltergeist(0.95f, 0.60f, 0.60f);
    check(finite(lowCharge) && finite(highCharge) && rms(lowCharge) > 0.001f && rms(highCharge) > 0.001f,
          "POLTERGEIST remains bounded and audible");
    std::cout << "INFO  POLTERGEIST charge difference=" << diff(lowCharge, highCharge) << '\n';
    check(diff(lowCharge, highCharge) > 0.12f, "POLTERGEIST CHARGE changes electrostatic plate interaction");

    auto lowDread = renderPoltergeist(0.65f, 0.05f, 0.60f);
    auto highDread = renderPoltergeist(0.65f, 0.95f, 0.60f);
    check(diff(lowDread, highDread) > 0.10f, "POLTERGEIST DREAD changes leakage and discharge threshold");

    auto touch = renderPoltergeist(0.65f, 0.60f, 0.05f);
    auto pressed = renderPoltergeist(0.65f, 0.60f, 1.00f);
    check(diff(touch, pressed) > 0.10f, "POLTERGEIST expression changes charging regime");

    auto weakField = renderAurora(0.05f, 0.60f, 0.60f);
    auto strongField = renderAurora(0.95f, 0.60f, 0.60f);
    check(finite(weakField) && finite(strongField) && rms(weakField) > 0.001f && rms(strongField) > 0.001f,
          "AURORA remains bounded and audible");
    std::cout << "INFO  AURORA field difference=" << diff(weakField, strongField) << '\n';
    check(diff(weakField, strongField) > 0.10f, "AURORA FIELD changes charged-ring frequency bending");

    auto grounded = renderAurora(0.65f, 0.05f, 0.60f);
    auto radiant = renderAurora(0.65f, 0.95f, 0.60f);
    check(diff(grounded, radiant) > 0.08f, "AURORA AETHER changes shared electrostatic field motion");

    auto faint = renderAurora(0.65f, 0.60f, 0.05f);
    auto expressive = renderAurora(0.65f, 0.60f, 1.00f);
    check(diff(faint, expressive) > 0.08f, "AURORA expression changes charge redistribution");

    std::cout << (failures ? "\nHorror Castle Electromagnetic check FAILED.\n"
                           : "\nHorror Castle Electromagnetic check passed.\n");
    return failures ? 1 : 0;
}

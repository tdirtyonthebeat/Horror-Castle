#include <JuceHeader.h>
#include "../Source/HorrorCastle/RitualFMEngine.h"
#include <cmath>
#include <iostream>
#include <vector>

namespace {
using horrorcastle::RitualFMEngine;

std::vector<float> render(float topology, float character, float expression, bool crypt)
{
    constexpr double sampleRate = 48000.0;
    constexpr int samples = 48000;
    RitualFMEngine engine;
    RitualFMEngine::VoiceState state;
    std::vector<float> out;
    out.reserve(samples);

    for (int i = 0; i < samples; ++i)
        out.push_back(engine.renderSample(state, 110.0f, topology, character,
                                          expression, crypt, sampleRate));
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
    for (const auto s : x) sum += (double)s * (double)s;
    return x.empty() ? 0.0f : (float)std::sqrt(sum / (double)x.size());
}

float difference(const std::vector<float>& a, const std::vector<float>& b)
{
    const auto n = std::min(a.size(), b.size());
    double sum = 0.0;
    for (size_t i = 0; i < n; ++i)
    {
        const auto d = (double)a[i] - (double)b[i];
        sum += d * d;
    }
    const auto absolute = n ? (float)std::sqrt(sum / (double)n) : 0.0f;
    return absolute / std::max(0.0001f, std::max(rms(a), rms(b)));
}

float motion(const std::vector<float>& x)
{
    if (x.size() < 2) return 0.0f;
    double sum = 0.0;
    for (size_t i = 1; i < x.size(); ++i)
    {
        const auto d = (double)x[i] - (double)x[i - 1];
        sum += d * d;
    }
    return (float)std::sqrt(sum / (double)(x.size() - 1));
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

    const auto chain = render(0.04f, 0.78f, 0.15f, true);
    const auto branch = render(0.50f, 0.78f, 0.15f, true);
    const auto dual = render(0.96f, 0.78f, 0.15f, true);
    check(finite(chain) && finite(branch) && finite(dual),
          "Ritual FM topology extremes remain finite");

    const auto chainBranch = difference(chain, branch);
    const auto branchDual = difference(branch, dual);
    std::cout << "INFO  topology difference chain/branch=" << chainBranch
              << " branch/dual=" << branchDual << '\n';
    check(chainBranch > 0.08f && branchDual > 0.08f,
          "Ritual FM graph morph spans distinct spectra");

    const auto crypt = render(0.58f, 0.90f, 0.30f, true);
    const auto tower = render(0.58f, 0.90f, 0.30f, false);
    const auto chamberDifference = difference(crypt, tower);
    std::cout << "INFO  Ritual FM CRYPT/TOWER difference=" << chamberDifference << '\n';
    check(finite(crypt) && finite(tower) && chamberDifference > 0.10f,
          "Ritual FM preserves chamber identity");

    const auto restrained = render(0.52f, 0.72f, 0.02f, true);
    const auto expressive = render(0.52f, 0.72f, 1.00f, true);
    const auto expressionDifference = difference(restrained, expressive);
    std::cout << "INFO  expression difference=" << expressionDifference
              << " motion(restraint/expression)=" << motion(restrained)
              << "/" << motion(expressive) << '\n';
    check(finite(expressive) && expressionDifference > 0.08f,
          "performance expression changes Ritual FM timbre");

    std::cout << (failures == 0
        ? "\nHorror Castle Living Engines check passed.\n"
        : "\nHorror Castle Living Engines check FAILED.\n");
    return failures == 0 ? 0 : 1;
}

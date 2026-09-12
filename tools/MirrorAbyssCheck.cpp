#include <JuceHeader.h>
#include "../Source/HorrorCastle/MirrorSpectralEngine.h"
#include "../Source/HorrorCastle/AbyssWaveguideEngine.h"
#include <cmath>
#include <iostream>
#include <vector>

namespace {
using horrorcastle::MirrorSpectralEngine;
using horrorcastle::AbyssWaveguideEngine;
constexpr double sampleRate = 48000.0;
constexpr int samples = 48000;

std::vector<float> renderMirror(float reflection, float aether, float expression)
{
    MirrorSpectralEngine e; MirrorSpectralEngine::VoiceState s; std::vector<float> out; out.reserve(samples);
    for (int i = 0; i < samples; ++i) out.push_back(e.renderSample(s, 110.0f, reflection, aether, expression, 0.82f, sampleRate));
    return out;
}

std::vector<float> renderAbyss(float depth, float dread, float expression)
{
    AbyssWaveguideEngine e; AbyssWaveguideEngine::VoiceState s; std::vector<float> out; out.reserve(samples);
    for (int i = 0; i < samples; ++i) out.push_back(e.renderSample(s, 110.0f, depth, dread, expression, 0.82f, sampleRate));
    return out;
}

bool finite(const std::vector<float>& x)
{
    for (float v : x) if (!std::isfinite(v) || std::abs(v) > 1.001f) return false;
    return true;
}
float rms(const std::vector<float>& x)
{
    double z = 0.0; for (float v : x) z += (double) v * v;
    return x.empty() ? 0.0f : (float) std::sqrt(z / (double) x.size());
}
float diff(const std::vector<float>& a, const std::vector<float>& b)
{
    const auto n = std::min(a.size(), b.size()); double z = 0.0;
    for (size_t i = 0; i < n; ++i) { const double d = (double) a[i] - b[i]; z += d * d; }
    const float d = n ? (float) std::sqrt(z / (double) n) : 0.0f;
    return d / std::max(0.0001f, std::max(rms(a), rms(b)));
}
}

int main()
{
    int failures = 0;
    auto check = [&](bool ok, const char* name) { std::cout << (ok ? "PASS  " : "FAIL  ") << name << '\n'; if (!ok) ++failures; };

    const auto mirrorPlain = renderMirror(0.05f, 0.60f, 0.50f);
    const auto mirrorReflected = renderMirror(0.95f, 0.60f, 0.50f);
    check(finite(mirrorPlain) && finite(mirrorReflected) && rms(mirrorPlain) > 0.001f && rms(mirrorReflected) > 0.001f,
          "MIRROR remains bounded and audible");
    std::cout << "INFO  MIRROR REFLECTION difference=" << diff(mirrorPlain, mirrorReflected) << '\n';
    check(diff(mirrorPlain, mirrorReflected) > 0.20f, "MIRROR REFLECTION changes spectral geometry");

    const auto mirrorStill = renderMirror(0.55f, 0.05f, 0.50f);
    const auto mirrorAether = renderMirror(0.55f, 0.95f, 0.50f);
    check(diff(mirrorStill, mirrorAether) > 0.10f, "MIRROR AETHER changes recursive precession field");
    const auto mirrorSoft = renderMirror(0.55f, 0.60f, 0.03f);
    const auto mirrorPressed = renderMirror(0.55f, 0.60f, 1.00f);
    check(diff(mirrorSoft, mirrorPressed) > 0.06f, "MIRROR expression changes recursive phase coupling");

    const auto abyssShallow = renderAbyss(0.05f, 0.60f, 0.50f);
    const auto abyssDeep = renderAbyss(0.95f, 0.60f, 0.50f);
    check(finite(abyssShallow) && finite(abyssDeep) && rms(abyssShallow) > 0.001f && rms(abyssDeep) > 0.001f,
          "ABYSS remains bounded and audible");
    std::cout << "INFO  ABYSS DEPTH difference=" << diff(abyssShallow, abyssDeep) << '\n';
    check(diff(abyssShallow, abyssDeep) > 0.12f, "ABYSS DEPTH changes tunnel lengths and propagation");

    const auto abyssDry = renderAbyss(0.55f, 0.05f, 0.50f);
    const auto abyssDread = renderAbyss(0.55f, 0.95f, 0.50f);
    check(diff(abyssDry, abyssDread) > 0.12f, "ABYSS DREAD changes wall absorption and junction feedback");
    const auto abyssWhisper = renderAbyss(0.55f, 0.60f, 0.03f);
    const auto abyssPressure = renderAbyss(0.55f, 0.60f, 1.00f);
    check(diff(abyssWhisper, abyssPressure) > 0.12f, "ABYSS expression changes excitation and tunnel coupling");

    std::cout << (failures ? "\nHorror Castle MIRROR / ABYSS check FAILED.\n" : "\nHorror Castle MIRROR / ABYSS check passed.\n");
    return failures ? 1 : 0;
}

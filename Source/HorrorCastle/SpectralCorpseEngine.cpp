#include "SpectralCorpseEngine.h"
#include <cmath>

namespace horrorcastle {

SpectralCorpseEngine::SpectralCorpseEngine() { loadDefaultCorpseModel(); }

void SpectralCorpseEngine::setModel(const Model& newModel) noexcept
{
    if (newModel.valid()) model = newModel;
}

void SpectralCorpseEngine::loadDefaultCorpseModel() noexcept
{
    model = {};
    model.frameCount = 12;
    model.analysedFundamentalHz = 110.0f;

    // A deliberately low/wooden synthetic reference corpse. This is not a
    // sample: it is an independent harmonic model used until the user imports
    // material through the forthcoming model-loader UI.
    for (int f = 0; f < model.frameCount; ++f)
    {
        const float t = (float) f / (float) (model.frameCount - 1);
        for (int h = 0; h < Partials; ++h)
        {
            const float n = (float) (h + 1);
            const float base = std::exp(-n * (0.16f + 0.10f * t));
            const float odd = ((h & 1) == 0) ? 1.0f : (0.50f + 0.25f * t);
            const float hollow = 1.0f - 0.42f * std::exp(-0.5f * std::pow((n - (5.0f + t * 3.5f)) / 1.7f, 2.0f));
            const float wound = 0.78f + 0.22f * std::sin(n * 1.71f + t * 7.0f);
            model.frames[(size_t) f].magnitude[(size_t) h] = base * odd * hollow * wound;
        }
        model.frames[(size_t) f].magnitude[0] = 1.0f;
    }
}

SpectralCorpseEngine::Model SpectralCorpseEngine::analysePeriodicAudio(const juce::AudioBuffer<float>& source,
                                                                         double sourceSampleRate,
                                                                         float fundamentalHz) const
{
    Model result{};
    if (source.getNumSamples() < AnalysisSize || source.getNumChannels() < 1 ||
        sourceSampleRate <= 0.0 || fundamentalHz <= 1.0f)
        return result;

    result.analysedFundamentalHz = fundamentalHz;
    const int usable = source.getNumSamples() - AnalysisSize;
    result.frameCount = juce::jlimit(1, MaxFrames, 1 + usable / juce::jmax(1, AnalysisSize / 4));

    juce::dsp::FFT fft(AnalysisOrder);
    juce::dsp::WindowingFunction<float> window((size_t) AnalysisSize,
                                                juce::dsp::WindowingFunction<float>::hann,
                                                true);
    std::array<float, AnalysisSize * 2> fftData{};

    for (int frameIndex = 0; frameIndex < result.frameCount; ++frameIndex)
    {
        fftData.fill(0.0f);
        const float normPos = result.frameCount > 1 ? (float) frameIndex / (float) (result.frameCount - 1) : 0.0f;
        const int start = juce::jlimit(0, usable, (int) std::lround(normPos * (float) usable));

        for (int i = 0; i < AnalysisSize; ++i)
        {
            float mono = 0.0f;
            for (int ch = 0; ch < source.getNumChannels(); ++ch)
                mono += source.getSample(ch, start + i);
            fftData[(size_t) i] = mono / (float) source.getNumChannels();
        }

        window.multiplyWithWindowingTable(fftData.data(), (size_t) AnalysisSize);
        fft.performRealOnlyForwardTransform(fftData.data());

        float peak = 1.0e-9f;
        for (int h = 0; h < Partials; ++h)
        {
            const float targetHz = fundamentalHz * (float) (h + 1);
            const int bin = (int) std::lround(targetHz * (float) AnalysisSize / (float) sourceSampleRate);
            if (bin <= 0 || bin >= AnalysisSize / 2) break;

            // Small three-bin harvest is tolerant of imperfect pitch estimates.
            float mag = 0.0f;
            for (int k = -1; k <= 1; ++k)
            {
                const int b = juce::jlimit(1, AnalysisSize / 2 - 1, bin + k);
                const float re = fftData[(size_t) (2 * b)];
                const float im = fftData[(size_t) (2 * b + 1)];
                mag = std::max(mag, std::sqrt(re * re + im * im));
            }
            result.frames[(size_t) frameIndex].magnitude[(size_t) h] = mag;
            peak = std::max(peak, mag);
        }

        for (auto& magnitude : result.frames[(size_t) frameIndex].magnitude)
            magnitude = std::pow(juce::jlimit(0.0f, 1.0f, magnitude / peak), 0.72f);
    }
    return result;
}

float SpectralCorpseEngine::readMagnitude(const Frame& frame, float harmonicIndex) noexcept
{
    const float x = juce::jlimit(1.0f, (float) Partials, harmonicIndex) - 1.0f;
    const int a = (int) x;
    const int b = juce::jmin(Partials - 1, a + 1);
    const float frac = x - (float) a;
    return frame.magnitude[(size_t) a] + (frame.magnitude[(size_t) b] - frame.magnitude[(size_t) a]) * frac;
}

float SpectralCorpseEngine::renderSample(VoiceState& state,
                                         float fundamentalHz,
                                         float position,
                                         float rot,
                                         float formant,
                                         float inharmonicity,
                                         double sampleRate) const noexcept
{
    if (!model.valid() || fundamentalHz <= 0.0f || sampleRate <= 0.0) return 0.0f;

    position = juce::jlimit(0.0f, 1.0f, position);
    rot = juce::jlimit(0.0f, 1.0f, rot);
    formant = juce::jlimit(-1.0f, 1.0f, formant);
    inharmonicity = juce::jlimit(0.0f, 1.0f, inharmonicity);

    const float framePos = position * (float) (model.frameCount - 1);
    const int frameA = (int) framePos;
    const int frameB = juce::jmin(model.frameCount - 1, frameA + 1);
    const float frameMix = framePos - (float) frameA;
    const float formantRatio = std::pow(2.0f, formant * 1.25f);
    const float twoPi = juce::MathConstants<float>::twoPi;

    float output = 0.0f;
    float normalizer = 0.0f;
    for (int h = 0; h < Partials; ++h)
    {
        const float harmonic = (float) (h + 1);
        const float sourceHarmonic = harmonic / formantRatio;
        const float ma = readMagnitude(model.frames[(size_t) frameA], sourceHarmonic);
        const float mb = readMagnitude(model.frames[(size_t) frameB], sourceHarmonic);
        float magnitude = ma + (mb - ma) * frameMix;

        // ROT is spectral decomposition rather than simple filtering: upper
        // partials decay nonlinearly and alternate partial families disappear
        // at different rates as the corpse breaks apart.
        const float family = ((h % 3) == 1) ? (1.0f - 0.58f * rot) : 1.0f;
        magnitude *= std::exp(-rot * harmonic * 0.115f) * family;

        const float stretch = 1.0f + inharmonicity * 0.0035f * std::pow(harmonic - 1.0f, 1.28f);
        const float partialHz = fundamentalHz * harmonic * stretch;
        if (partialHz >= (float) sampleRate * 0.46f) break;

        float& phase = state.phase[(size_t) h];
        phase += partialHz / (float) sampleRate;
        phase -= std::floor(phase);
        output += std::sin(twoPi * phase) * magnitude;
        normalizer += magnitude;
    }

    if (normalizer > 1.0e-5f) output /= std::sqrt(normalizer);
    // A short spectral-body integrator keeps frame movement organic and avoids
    // the brittle quality of naked additive banks without turning this into a filter synth.
    const float bodyAlpha = 1.0f - std::exp(-twoPi * (1800.0f - 1100.0f * rot) / (float) sampleRate);
    state.body += bodyAlpha * (output - state.body);
    return std::tanh((0.62f * output + 0.52f * state.body) * (1.15f + 0.65f * rot));
}

} // namespace horrorcastle

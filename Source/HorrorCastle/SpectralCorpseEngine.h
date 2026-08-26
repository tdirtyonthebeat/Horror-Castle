#pragma once
#include <JuceHeader.h>
#include <array>

namespace horrorcastle {

class SpectralCorpseEngine {
public:
    static constexpr int MaxFrames = 32;
    static constexpr int Partials = 24;
    static constexpr int AnalysisOrder = 10;
    static constexpr int AnalysisSize = 1 << AnalysisOrder;

    struct Frame {
        std::array<float, Partials> magnitude{};
    };

    struct Model {
        std::array<Frame, MaxFrames> frames{};
        int frameCount = 0;
        float analysedFundamentalHz = 110.0f;
        bool valid() const noexcept { return frameCount > 0; }
    };

    struct VoiceState {
        std::array<float, Partials> phase{};
        float body = 0.0f;
        void reset() noexcept { phase.fill(0.0f); body = 0.0f; }
    };

    SpectralCorpseEngine();

    // Offline/non-audio-thread analysis entry point. The caller supplies the
    // approximate fundamental of the source so partial energy can be harvested
    // into stable harmonic bins instead of copied as raw FFT frames.
    Model analysePeriodicAudio(const juce::AudioBuffer<float>& source,
                               double sourceSampleRate,
                               float fundamentalHz) const;

    void setModel(const Model& newModel) noexcept;
    const Model& getModel() const noexcept { return model; }
    void loadDefaultCorpseModel() noexcept;

    float renderSample(VoiceState& state,
                       float fundamentalHz,
                       float position,
                       float rot,
                       float formant,
                       float inharmonicity,
                       double sampleRate) const noexcept;

private:
    Model model{};
    static float readMagnitude(const Frame& frame, float harmonicIndex) noexcept;
};

} // namespace horrorcastle

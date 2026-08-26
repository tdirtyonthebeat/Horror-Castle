#pragma once
#include <JuceHeader.h>
#include <array>

namespace horrorcastle {

class RitualsEngine {
public:
    enum Pattern { Procession, Ascension, Descent, Circle, Seance, Possession, Sacrifice, Chaos };
    struct Parameters {
        bool enabled = false;
        int pattern = Procession;
        int rate = 2; // 0=1/4, 1=1/8, 2=1/16, 3=1/32
        float bpm = 120.f;
        float gate = .62f;
        float probability = 1.f;
        float swing = 0.f;
        int octaves = 1;
    };

    void prepare(double sampleRate);
    void reset();
    void setParameters(const Parameters& p) noexcept { params = p; }
    void process(const juce::MidiBuffer& input, juce::MidiBuffer& output, int numSamples);
    float getCursePulse() const noexcept { return cursePulse; }
    float getBalanceMod() const noexcept { return balanceMod; }
    float getIntensityMod() const noexcept { return intensityMod; }

private:
    struct Held { int note = -1; juce::uint8 velocity = 100; };
    Parameters params{};
    double sr = 44100.0;
    std::array<Held, 16> held{};
    int heldCount = 0;
    int step = 0;
    int currentNote = -1;
    int noteOffCountdown = -1;
    double samplesToNext = 0.0;
    uint32_t rng = 0x19f3a65bu;
    float cursePulse = 0.f, balanceMod = 0.f, intensityMod = 0.f;

    float random01() noexcept;
    void addHeld(int note, juce::uint8 velocity);
    void removeHeld(int note);
    Held chooseNote();
    double stepSamples() const noexcept;
};

} // namespace horrorcastle

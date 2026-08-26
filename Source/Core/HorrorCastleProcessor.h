#pragma once

#include <JuceHeader.h>
#include "../HorrorCastle/CastleEngine.h"
#include "../HorrorCastle/Grimoire.h"
#include "../HorrorCastle/CastleParameters.h"

namespace horrorcastle {

class HorrorCastleProcessor final : public juce::AudioProcessor
{
public:
    HorrorCastleProcessor();
    ~HorrorCastleProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout&) const override;
#endif

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 8.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    juce::AudioProcessorValueTreeState& getParameterState() noexcept { return parameters; }
    const juce::AudioProcessorValueTreeState& getParameterState() const noexcept { return parameters; }

    float getHexLaneActivity(int index) const noexcept { return engine.getHexLaneActivity(index); }
    float getHexDestinationValue(int index) const noexcept { return engine.getHexDestinationValue(index); }

    juce::StringArray getFactoryPresetNames() const { return grimoire.getFactoryNames(); }
    std::vector<Grimoire::SpellInfo> getFactoryPresetSpells() const { return grimoire.getFactorySpells(); }
    bool loadFactoryPreset(int index) { return grimoire.loadFactory(index); }
    juce::File saveUserPreset(const juce::String& name) const { return grimoire.saveUserPreset(name); }
    bool loadUserPreset(const juce::File& file) { return grimoire.loadUserPreset(file); }
    juce::File getUserPresetDirectory() const { return grimoire.getUserDirectory(); }

private:
    juce::AudioProcessorValueTreeState parameters;
    CastleEngine engine;
    Grimoire grimoire;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HorrorCastleProcessor)
};

} // namespace horrorcastle

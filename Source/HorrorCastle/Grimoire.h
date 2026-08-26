#pragma once
#include <JuceHeader.h>
#include <vector>

namespace horrorcastle {

class Grimoire {
public:
    static constexpr int CurrentStateVersion = 120;
    struct SpellInfo {
        juce::String name, subtitle, description, category, tags;
        int sigil = 0;
    };

    explicit Grimoire(juce::AudioProcessorValueTreeState& state) : apvts(state) {}

    std::vector<SpellInfo> getFactorySpells() const;
    juce::StringArray getFactoryNames() const;
    bool loadFactory(int index);
    juce::File getUserDirectory() const;
    juce::File saveUserPreset(const juce::String& requestedName) const;
    bool loadUserPreset(const juce::File& file);
    static void migrateState(juce::ValueTree& state);

private:
    juce::AudioProcessorValueTreeState& apvts;
    void set(const juce::String& id, float actual);
    void commonReset();
    void setHex(int lane, int source, int curse, int destination, float amount);
};

} // namespace horrorcastle

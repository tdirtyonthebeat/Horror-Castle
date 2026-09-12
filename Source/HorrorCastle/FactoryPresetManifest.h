#pragma once
#include <JuceHeader.h>
#include <array>
#include <cstdint>
#include <vector>

namespace horrorcastle {

struct FactoryPresetDefinition {
    juce::String name, subtitle, description, category, tags;
    int sigil = 0;
    int archetype = 0;
    std::uint32_t seed = 0;
};

class FactoryPresetManifest {
public:
    static constexpr int LegacyPresetCount = 44;
    static constexpr int FactoryPresetCount = 100;

    static std::vector<FactoryPresetDefinition> create();
};

} // namespace horrorcastle

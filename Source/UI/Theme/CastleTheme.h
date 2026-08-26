#pragma once
#include <JuceHeader.h>

namespace horrorcastle::theme {

inline juce::Colour stoneBlack()   { return juce::Colour(0xff080a0d); }
inline juce::Colour stoneRaised()  { return juce::Colour(0xff111419); }
inline juce::Colour iron()         { return juce::Colour(0xff2a2a2e); }
inline juce::Colour bone()         { return juce::Colour(0xffd8ccba); }
inline juce::Colour parchment()    { return juce::Colour(0xffc8b28e); }
inline juce::Colour gold()         { return juce::Colour(0xffb99455); }
inline juce::Colour cryptRed()     { return juce::Colour(0xff9f3040); }
inline juce::Colour cryptEmber()   { return juce::Colour(0xffdf5c46); }
inline juce::Colour towerPurple()  { return juce::Colour(0xff76509f); }
inline juce::Colour towerGlow()    { return juce::Colour(0xffb06fe3); }
inline juce::Colour graveViolet()  { return juce::Colour(0xff6c477f); }
inline juce::Colour fog()          { return juce::Colour(0xff8d99a7); }

inline juce::Colour accentFor(const juce::Component& c)
{
    const auto key = c.getProperties().getWithDefault("castleAccent", "gold").toString();
    if (key == "crypt") return cryptEmber();
    if (key == "tower") return towerGlow();
    if (key == "grave") return towerPurple();
    if (key == "hex") return juce::Colour(0xffc04462);
    if (key == "ritual") return juce::Colour(0xffc66a4c);
    if (key == "grimoire") return juce::Colour(0xff9c6bc3);
    return gold();
}

} // namespace horrorcastle::theme

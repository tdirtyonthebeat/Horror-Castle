#pragma once
#include <JuceHeader.h>

namespace horrorcastle::CastleGraphics {
void drawStonePanel(juce::Graphics&, juce::Rectangle<float>, juce::Colour accent, float corner=10.0f);
void drawGothicArch(juce::Graphics&, juce::Rectangle<float>, juce::Colour accent);
void drawRuneBorder(juce::Graphics&, juce::Rectangle<float>, juce::Colour accent, float alpha=0.28f);
void drawSigil(juce::Graphics&, juce::Point<float> centre, float radius, juce::Colour accent, float phase=0.0f);
void drawTorch(juce::Graphics&, juce::Point<float> base, juce::Colour flame, float pulse=0.0f);
}

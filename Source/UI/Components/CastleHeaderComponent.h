#pragma once
#include <JuceHeader.h>

namespace horrorcastle {

class CastleHeaderComponent : public juce::Component
{
public:
    CastleHeaderComponent();
    void paint(juce::Graphics&) override;

private:
    juce::Image headerImage;
};

} // namespace horrorcastle

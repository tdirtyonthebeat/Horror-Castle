#pragma once
#include <JuceHeader.h>
#include "../HorrorCastle/CurseChain.h"
#include <array>

namespace horrorcastle {

class HexMatrixComponent : public juce::Component, private juce::Timer
{
public:
    explicit HexMatrixComponent(juce::AudioProcessorValueTreeState& state);
    void setLane(int index, const HorrorCastle::CurseLane& lane);
    void setLaneValue(int index, float value);
    void setDestinationValue(HorrorCastle::ModDestination d, float value);
    void paint(juce::Graphics&) override;
    void resized() override;
    std::function<void(int)> onLaneSelected;
    int getSelectedLane() const noexcept { return selectedLane; }
    float getLaneValue(int index) const noexcept { return (index >= 0 && index < (int) laneValues.size()) ? laneValues[(size_t) index] : 0.0f; }

private:
    struct Node { juce::Point<float> pos; float radius = 20.0f; float activity = 0.0f; bool selected = false; };
    struct LaneWidgets {
        std::unique_ptr<juce::ComboBox> source, curse, destination;
        std::unique_ptr<juce::Slider> amount;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> sourceA, curseA, destinationA;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> amountA;
    };

    juce::AudioProcessorValueTreeState& apvts;
    std::array<LaneWidgets, HorrorCastle::CurseChain::maxLanes> widgets;
    std::array<HorrorCastle::CurseLane, HorrorCastle::CurseChain::maxLanes> lanes{};
    std::array<float, HorrorCastle::CurseChain::maxLanes> laneValues{};
    std::array<float, 22> destinationValues{};
    std::array<Node, HorrorCastle::CurseChain::maxLanes> sourceNodes{}, curseNodes{}, destinationNodes{};
    int selectedLane = -1;
    float animation = 0.0f;
    juce::Image skinFrame;

    void setupWidgets();
    void timerCallback() override;
    void layoutNodes();
    void drawConnection(juce::Graphics&, const Node&, const Node&, float activity, bool cursed, bool secondary);
    void drawNode(juce::Graphics&, const Node&, const juce::String&, const juce::String&, bool cursed);
    juce::String sourceName(HorrorCastle::ModSource) const;
    juce::String curseName(HorrorCastle::CurseType) const;
    juce::String destinationName(HorrorCastle::ModDestination) const;
    juce::String curseGlyph(HorrorCastle::CurseType) const;
    void mouseDown(const juce::MouseEvent&) override;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HexMatrixComponent)
};

} // namespace horrorcastle

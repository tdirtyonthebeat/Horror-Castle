#pragma once
#include <JuceHeader.h>
#include "../../HorrorCastle/Grimoire.h"
#include <array>
#include <vector>

namespace horrorcastle {
class HorrorCastleProcessor;

class GrimoireComponent : public juce::Component
{
public:
    explicit GrimoireComponent(HorrorCastleProcessor&);
    ~GrimoireComponent() override;
    void paint(juce::Graphics&) override;
    void resized() override;
    std::function<void()> onClose;
    std::function<void(const juce::String&)> onStatus;

private:
    HorrorCastleProcessor& processor;
    std::vector<Grimoire::SpellInfo> spells;
    std::vector<int> filtered;
    std::array<juce::TextButton,8> spellButtons;
    std::array<bool,64> favourites{};
    juce::ComboBox category;
    juce::TextEditor search;
    juce::TextButton closeButton{"CLOSE"}, previousButton{"◀"}, nextButton{"▶"};
    juce::TextButton summonButton{"RE-SUMMON"}, favouriteButton{"☆ FAVORITE"};
    juce::TextButton saveButton{"SAVE USER SPELL"}, loadButton{"OPEN USER SPELL"};
    juce::Label title, subtitle, description, tags, pageLabel;
    std::unique_ptr<juce::FileChooser> chooser;
    int page=0, selected=-1;

    void applyFilter();
    void refreshButtons();
    void selectFactoryIndex(int);
    void updateDetail();
    int pageCount() const;
};
}

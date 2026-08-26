#include "CurseInspectorComponent.h"
#include "Theme/CastleTheme.h"
#include "Theme/CastleGraphics.h"
#include <BinaryData.h>
#include <cmath>

namespace horrorcastle {
using namespace juce;

static void setupInspectorLabel(Label& l, float size, Colour colour, Justification j = Justification::centredLeft)
{
    l.setFont(Font(FontOptions(size)));
    l.setColour(Label::textColourId, colour);
    l.setJustificationType(j);
}

CurseInspectorComponent::CurseInspectorComponent(AudioProcessorValueTreeState& state) : apvts(state)
{
    skinFrame=ImageFileFormat::loadFrom(BinaryData::inspector_frame_png,BinaryData::inspector_frame_pngSize);
    setOpaque(false);

    setupInspectorLabel(title, 17.0f, Colour(0xffe4cfaa));
    setupInspectorLabel(routeLabel, 10.0f, Colour(0xffaa8e82));
    setupInspectorLabel(behaviorTitle, 10.0f, Colour(0xffd4a957));
    setupInspectorLabel(behaviorBody, 10.0f, Colour(0xffd7c7b4));
    setupInspectorLabel(chainLabel, 9.0f, Colour(0xffd4a957));
    setupInspectorLabel(intensityLabel, 9.0f, Colour(0xff9f837b), Justification::centred);
    setupInspectorLabel(characterLabel, 9.0f, Colour(0xffd4a957), Justification::centred);

    behaviorTitle.setText("CURSE BEHAVIOR", dontSendNotification);
    intensityLabel.setText("INTENSITY", dontSendNotification);
    characterLabel.setText("CHARACTER", dontSendNotification);

    addAndMakeVisible(title);
    addAndMakeVisible(routeLabel);
    addAndMakeVisible(behaviorTitle);
    addAndMakeVisible(behaviorBody);
    addAndMakeVisible(chainLabel);
    addAndMakeVisible(intensityLabel);
    addAndMakeVisible(characterLabel);

    const StringArray curses { "Clean", "Corrupt", "Haunt", "Possession", "Decay", "Madness", "Blood" };
    for (int i = 0; i < curses.size(); ++i)
        curseChoice.addItem(curses[i], i + 1);
    curseChoice.setColour(ComboBox::backgroundColourId, Colour(0xff120d13));
    curseChoice.setColour(ComboBox::outlineColourId, Colour(0xff6a3040));
    curseChoice.setColour(ComboBox::textColourId, Colour(0xffe1d0b7));
    addAndMakeVisible(curseChoice);

    for (auto* slider : { &intensity, &character })
    {
        slider->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        slider->setTextBoxStyle(Slider::TextBoxBelow, false, 68, 17);
        slider->setColour(Slider::rotarySliderFillColourId, Colour(0xff8e2940));
        slider->setColour(Slider::rotarySliderOutlineColourId, Colour(0xff33202a));
        slider->setColour(Slider::thumbColourId, Colour(0xffd4a957));
        slider->setColour(Slider::textBoxTextColourId, Colour(0xffdfceb6));
        slider->setColour(Slider::textBoxBackgroundColourId, Colour(0xff100b10));
        slider->setColour(Slider::textBoxOutlineColourId, Colour(0xff3d2530));
        addAndMakeVisible(*slider);
    }

    intensity.setRange(-1.0, 1.0, 0.001);
    intensity.setDoubleClickReturnValue(true, 0.0);

    curseChoice.onChange = [this]
    {
        rebuildCharacterAttachment();
        refreshText();
        repaint();
    };

    selectLane(0);
    startTimerHz(30);
}

CurseInspectorComponent::~CurseInspectorComponent()
{
    stopTimer();
}

String CurseInspectorComponent::parameterPrefix() const
{
    return String("hex.curse") + String(selectedLane + 1) + ".";
}

int CurseInspectorComponent::readChoice(const String& id, int fallback) const
{
    if (auto* p = apvts.getRawParameterValue(id))
        return (int) std::lround(p->load());
    return fallback;
}

float CurseInspectorComponent::readFloat(const String& id, float fallback) const
{
    if (auto* p = apvts.getRawParameterValue(id))
        return p->load();
    return fallback;
}

String CurseInspectorComponent::sourceName(int i) const
{
    static const StringArray names { "OFF", "BLOOD", "WRAITH", "VELOCITY", "KEY", "RANDOM", "PULSE" };
    return isPositiveAndBelow(i, names.size()) ? names[i] : "OFF";
}

String CurseInspectorComponent::destinationName(int i) const
{
    static const StringArray names { "OFF", "CRYPT CUTOFF", "TOWER CUTOFF", "CRYPT SHAPE", "TOWER SHAPE", "FM DEPTH", "FILTER DRIVE", "RITUAL MIX", "GRAVE MIX", "PITCH", "CURSE DEPTH", "CRYPT DREAD", "TOWER AETHER", "RITUAL DEPTH", "RITUAL FURY", "RITUAL FEEDBACK", "GRAVE TONE", "BLOOD FEED", "AETHER LEAK", "SOUL EXCHANGE", "HAUNT", "GRAVE FEEDBACK" };
    return isPositiveAndBelow(i, names.size()) ? names[i] : "OFF";
}

String CurseInspectorComponent::curseName(int i) const
{
    static const StringArray names { "CLEAN", "CORRUPT", "HAUNT", "POSSESSION", "DECAY", "MADNESS", "BLOOD" };
    return isPositiveAndBelow(i, names.size()) ? names[i] : "CLEAN";
}

String CurseInspectorComponent::curseGlyph(int i) const
{
    static const StringArray glyphs { "O", "X", "~", "+", "\\", "Y", "D" };
    return isPositiveAndBelow(i, glyphs.size()) ? glyphs[i] : "O";
}

String CurseInspectorComponent::behavior(int i) const
{
    switch (i)
    {
        case 1: return "Asymmetric polarity breakage. CORRUPT BIAS decides which side of the modulation fractures first and how hard it clips.";
        case 2: return "Ghost motion plus amplitude eclipse. HAUNT RATE controls the independent apparition clock for this lane.";
        case 3: return "Wavefolded possession. POSSESSION FOLD increases the number of folds imposed on the modulation source.";
        case 4: return "Dying modulation peaks. DECAY TIME controls how sharply the source collapses while Blood determines how much life remains.";
        case 5: return "Hard staircase instability. MADNESS STEPS chooses how many discrete ritual states the modulation may occupy.";
        case 6: return "Envelope-hungry pumping. BLOOD HUNGER increases dependence on note amplitude and Wraith contamination.";
        default:return "Uncursed routing. The modulation source reaches its destination without transformation or secondary character.";
    }
}

String CurseInspectorComponent::characterParameter(int curseIndex) const
{
    switch (curseIndex)
    {
        case 1: return "corruptBias";
        case 2: return "hauntRate";
        case 3: return "possessionFold";
        case 4: return "decayTime";
        case 5: return "madnessSteps";
        case 6: return "bloodHunger";
        default:return {};
    }
}

String CurseInspectorComponent::characterCaption(int curseIndex) const
{
    switch (curseIndex)
    {
        case 1: return "CORRUPT BIAS";
        case 2: return "HAUNT RATE";
        case 3: return "POSSESSION FOLD";
        case 4: return "DECAY TIME";
        case 5: return "MADNESS STEPS";
        case 6: return "BLOOD HUNGER";
        default:return "NO CURSE PROPERTY";
    }
}

Colour CurseInspectorComponent::curseColour(int i) const
{
    switch (i)
    {
        case 1: return Colour(0xffb52c46);
        case 2: return Colour(0xff8655a8);
        case 3: return Colour(0xffb36b45);
        case 4: return Colour(0xff657b76);
        case 5: return Colour(0xffb85d89);
        case 6: return Colour(0xffa71432);
        default:return Colour(0xff6f6055);
    }
}

void CurseInspectorComponent::configureCharacterSlider(int curseIndex)
{
    character.setTextValueSuffix("");
    character.setNumDecimalPlacesToDisplay(2);
    character.setEnabled(curseIndex != 0);
    character.setAlpha(curseIndex == 0 ? 0.30f : 1.0f);

    switch (curseIndex)
    {
        case 1:
            character.setRange(-1.0, 1.0, 0.001);
            character.setDoubleClickReturnValue(true, 0.22);
            break;
        case 2:
            character.setRange(0.05, 8.0, 0.01);
            character.setTextValueSuffix(" Hz");
            character.setDoubleClickReturnValue(true, 1.70);
            break;
        case 3:
            character.setRange(0.0, 1.0, 0.001);
            character.setDoubleClickReturnValue(true, 0.35);
            break;
        case 4:
            character.setRange(0.05, 8.0, 0.01);
            character.setTextValueSuffix(" s");
            character.setDoubleClickReturnValue(true, 1.50);
            break;
        case 5:
            character.setRange(2.0, 32.0, 1.0);
            character.setNumDecimalPlacesToDisplay(0);
            character.setTextValueSuffix(" steps");
            character.setDoubleClickReturnValue(true, 8.0);
            break;
        case 6:
            character.setRange(0.0, 1.0, 0.001);
            character.setDoubleClickReturnValue(true, 0.65);
            break;
        default:
            character.setRange(0.0, 1.0, 0.001);
            character.setValue(0.0, dontSendNotification);
            break;
    }
}

void CurseInspectorComponent::selectLane(int laneIndex)
{
    selectedLane = jlimit(0, 7, laneIndex);
    rebuildAttachments();
    refreshText();
    repaint();
}

void CurseInspectorComponent::rebuildAttachments()
{
    curseAttachment.reset();
    intensityAttachment.reset();
    characterAttachment.reset();
    attachedCharacterCurse = -1;

    const auto p = parameterPrefix();
    curseAttachment = std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, p + "curse", curseChoice);
    intensityAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(apvts, p + "amount", intensity);
    rebuildCharacterAttachment();
}

void CurseInspectorComponent::rebuildCharacterAttachment()
{
    characterAttachment.reset();
    if (selectedLane < 0) return;

    const int curse = readChoice(parameterPrefix() + "curse");
    attachedCharacterCurse = curse;
    configureCharacterSlider(curse);
    characterLabel.setText(characterCaption(curse), dontSendNotification);

    const auto parameter = characterParameter(curse);
    if (parameter.isNotEmpty())
        characterAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
            apvts, parameterPrefix() + parameter, character);
}

void CurseInspectorComponent::setLiveActivity(float value)
{
    activity = jlimit(0.0f, 1.0f, std::abs(value));
}

void CurseInspectorComponent::setChainInfluence(float value)
{
    chainInfluence = jlimit(-1.0f, 1.0f, value);
}

void CurseInspectorComponent::refreshText()
{
    if (selectedLane < 0) return;
    const auto p = parameterPrefix();
    const int source = readChoice(p + "source");
    const int curse = readChoice(p + "curse");
    const int destination = readChoice(p + "destination");

    title.setText("CURSE " + String(selectedLane + 1) + " // " + curseName(curse), dontSendNotification);
    routeLabel.setText(sourceName(source) + "  >  " + curseName(curse) + "  >  " + destinationName(destination), dontSendNotification);
    behaviorBody.setText(behavior(curse), dontSendNotification);
    chainLabel.setText("CHAIN INFLUENCE  " + String(chainInfluence >= 0.0f ? "+" : "") + String(chainInfluence, 2), dontSendNotification);
    characterLabel.setText(characterCaption(curse), dontSendNotification);
}

void CurseInspectorComponent::timerCallback()
{
    animation += 0.055f;
    if (selectedLane >= 0)
    {
        const int curse = readChoice(parameterPrefix() + "curse");
        if (curse != attachedCharacterCurse)
            rebuildCharacterAttachment();
    }
    refreshText();
    repaint();
}

void CurseInspectorComponent::drawMeter(Graphics& g, Rectangle<float> area, float value, const String& caption)
{
    g.setColour(Colour(0xff20151d));
    g.fillRoundedRectangle(area, 4.0f);
    auto fill = area.reduced(2.0f);
    fill.setWidth(fill.getWidth() * jlimit(0.0f, 1.0f, std::abs(value)));
    g.setColour(value >= 0.0f ? Colour(0xffd4a957) : Colour(0xff8f2941));
    g.fillRoundedRectangle(fill, 3.0f);
    g.setColour(Colour(0xffa78d7e));
    g.setFont(Font(FontOptions(8.5f)));
    g.drawText(caption, area.toNearestInt(), Justification::centred);
}

void CurseInspectorComponent::drawSigil(Graphics& g, Rectangle<float> area)
{
    const auto p = parameterPrefix();
    const int curse = selectedLane >= 0 ? readChoice(p + "curse") : 0;
    const auto c = curseColour(curse);
    const auto centre = area.getCentre();
    const float r = jmin(area.getWidth(), area.getHeight()) * 0.31f;
    const float pulse = 1.0f + 0.10f * activity + 0.025f * std::sin(animation * 1.8f);

    g.setColour(c.withAlpha(0.14f + 0.35f * activity));
    g.fillEllipse(centre.x - r * 1.38f * pulse, centre.y - r * 1.38f * pulse,
                  r * 2.76f * pulse, r * 2.76f * pulse);

    g.setColour(Colour(0xff0c080d));
    g.fillEllipse(centre.x-r, centre.y-r, r*2, r*2);

    g.setColour(c.withAlpha(0.85f));
    g.drawEllipse(centre.x-r, centre.y-r, r*2, r*2, 2.0f);
    g.drawEllipse(centre.x-r*0.73f, centre.y-r*0.73f, r*1.46f, r*1.46f, 1.0f);

    Path rune;
    for (int i = 0; i < 6; ++i)
    {
        const float a = -MathConstants<float>::halfPi + i * MathConstants<float>::twoPi / 6.0f;
        auto pt = centre + Point<float>(std::cos(a), std::sin(a)) * r * 0.83f;
        if (i == 0) rune.startNewSubPath(pt); else rune.lineTo(pt);
    }
    rune.closeSubPath();
    g.setColour(c.withAlpha(0.46f + activity * 0.35f));
    g.strokePath(rune, PathStrokeType(1.2f));

    g.setColour(Colour(0xffead8b9));
    g.setFont(Font(FontOptions(38.0f)).boldened());
    g.drawText(curseGlyph(curse), area.toNearestInt(), Justification::centred);

    if (std::abs(chainInfluence) > 0.005f)
    {
        g.setColour(Colour(0xffd4a957).withAlpha(0.9f));
        const float chainRadius = r + 7.0f;
        const float startAngle = -MathConstants<float>::halfPi;
        const float endAngle = startAngle + MathConstants<float>::twoPi * std::abs(chainInfluence);
        Path chainArc;
        chainArc.addCentredArc(centre.x, centre.y, chainRadius, chainRadius,
                              0.0f, startAngle, endAngle, true);
        g.strokePath(chainArc, PathStrokeType(3.0f));
    }
}

void CurseInspectorComponent::paint(Graphics& g)
{
    auto outer = getLocalBounds().toFloat();
    CastleGraphics::drawStonePanel(g, outer.reduced(3.0f), theme::towerPurple(), 5.0f);
    if(skinFrame.isValid()){
        g.setOpacity(.97f);g.drawImageWithin(skinFrame,0,0,getWidth(),getHeight(),RectanglePlacement::stretchToFit);g.setOpacity(1.0f);
    }else CastleGraphics::drawRuneBorder(g, outer.reduced(4.0f), theme::towerPurple(), .30f);
    drawSigil(g, { 18.0f, 86.0f, 142.0f, 142.0f });
    drawMeter(g, { 170.0f, 94.0f, (float)getWidth()-194.0f, 16.0f }, chainInfluence, "CURSE CHAIN");
    drawMeter(g, { 170.0f, 116.0f, (float)getWidth()-194.0f, 16.0f }, activity, "LIVE ACTIVITY");
}

void CurseInspectorComponent::resized()
{
    const int w = getWidth();

    title.setBounds(14, 10, w - 28, 24);
    routeLabel.setBounds(14, 34, w - 28, 18);
    curseChoice.setBounds(20, 54, w - 40, 24);

    chainLabel.setBounds(170, 136, w - 194, 18);

    intensityLabel.setBounds(166, 158, 98, 17);
    intensity.setBounds(166, 175, 98, 72);

    characterLabel.setBounds(270, 158, 110, 17);
    character.setBounds(276, 175, 98, 72);

    behaviorTitle.setBounds(18, 258, w - 36, 18);
    behaviorBody.setBounds(18, 278, w - 36, getHeight() - 294);
}
} // namespace horrorcastle

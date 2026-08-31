#pragma once
#include <JuceHeader.h>

namespace horrorcastle {
namespace param {
inline juce::String id(const char* scene, int slot, const char* control) { return juce::String(scene) + ".g" + juce::String(slot) + "." + control; }
inline juce::String fid(const char* scene, int cell, const char* control) { return juce::String(scene) + ".f" + juce::String(cell) + "." + control; }
inline juce::String noise(const char* scene, const char* control) { return juce::String(scene) + ".noise." + control; }
inline juce::String scene(const char* scene, const char* control) { return juce::String(scene) + "." + control; }
inline juce::String route(const char* scene) { return juce::String(scene) + ".route"; }

inline juce::AudioProcessorValueTreeState::ParameterLayout createLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout l;
    auto addFloat = [&l](juce::String id, juce::String name, float lo, float hi, float def) { l.add(std::make_unique<juce::AudioParameterFloat>(id, name, juce::NormalisableRange<float>(lo, hi), def)); };
    auto addChoice = [&l](juce::String id, juce::String name, std::initializer_list<juce::String> choices, int def) { juce::StringArray a; for (auto& c : choices) a.add(c); l.add(std::make_unique<juce::AudioParameterChoice>(id, name, a, def)); };
    auto addBool = [&l](juce::String id, juce::String name, bool def) { l.add(std::make_unique<juce::AudioParameterBool>(id, name, def)); };

    // INIT is intentionally an engine-audition patch rather than a finished sound.
    // One primary Living Engine is audible per room, the remaining two are parked at
    // zero level but preselected to the other Living Engine species for instant A/B.
    const char* scenes[] = {"crypt", "tower"};
    const char* genNames[] = {"Generator I", "Generator II", "Generator III"};
    for (auto* s : scenes) {
        const bool sceneIsCrypt = juce::String(s) == "crypt";
        for (int g = 0; g < 3; ++g) {
            auto p = [&](const char* c) { return id(s, g + 1, c); };
            const int defaultType = 15 + g; // ABYSS/MIRROR, POLTERGEIST/AURORA, VORTEX/SIREN.
            if (sceneIsCrypt)
                addChoice(p("type"), juce::String(s) + " " + genNames[g] + " Type",
                          {"VA", "Wavetable", "FM", "PM", "Vector", "Chip", "Noise", "Resonator", "Undercrypt", "Corpse", "Bone Resonator", "Rotator", "Wraith", "Coffin", "Marrow", "Abyss", "Poltergeist", "Vortex"}, defaultType);
            else
                addChoice(p("type"), juce::String(s) + " " + genNames[g] + " Type",
                          {"VA", "Wavetable", "FM", "PM", "Vector", "Chip", "Noise", "Resonator", "Bell Glass", "Spectral Spire", "Astral FM", "Prism", "Reliquary", "Choir", "Orrery", "Mirror", "Aurora", "Siren"}, defaultType);
            const float defaultLevel = (g == 0) ? (sceneIsCrypt ? .72f : .58f) : 0.f;
            addFloat(p("level"), juce::String(s) + " " + genNames[g] + " Level", 0.f, 1.f, defaultLevel);
            addFloat(p("pan"), juce::String(s) + " " + genNames[g] + " Pan", -1.f, 1.f, 0.f);
            addFloat(p("tune"), juce::String(s) + " " + genNames[g] + " Tune", -24.f, 24.f, 0.f);
            const float defaultShape = (g == 0) ? .58f : (g == 1 ? .70f : .82f);
            addFloat(p("shape"), juce::String(s) + " " + genNames[g] + " Shape", 0.f, 1.f, defaultShape);
            addFloat(p("spread"), juce::String(s) + " " + genNames[g] + " Spread", 0.f, 1.f, 0.f);
            addBool(p("enabled"), juce::String(s) + " " + genNames[g] + " Enabled", true);
        }
        addBool(noise(s, "enabled"), juce::String(s) + " Noise", false);
        addFloat(noise(s, "level"), juce::String(s) + " Noise Level", 0.f, 1.f, .08f);
        for (int f = 0; f < 2; ++f) {
            auto p = [&](const char* c) { return fid(s, f + 1, c); };
            // Keep INIT deliberately broad-band. Heavy filter coloration was masking
            // the species differences before the player even touched a control.
            const int defaultFilterType = 0;
            addChoice(p("type"), juce::String(s) + " Filter " + juce::String(f + 1) + " Type", {"Low Pass", "High Pass", "Band Pass", "Notch", "Comb", "Formant", "Diode", "K35", "Shaper"}, defaultFilterType);
            const float defaultCutoff = (f == 0) ? .42f : .46f;
            const float defaultRes = .08f;
            const float defaultDrive = .02f;
            addFloat(p("cutoff"), juce::String(s) + " Filter " + juce::String(f + 1) + " Cutoff", 0.002f, .48f, defaultCutoff);
            addFloat(p("resonance"), juce::String(s) + " Filter " + juce::String(f + 1) + " Resonance", 0.f, 1.f, defaultRes);
            addFloat(p("drive"), juce::String(s) + " Filter " + juce::String(f + 1) + " Drive", 0.f, 1.f, defaultDrive);
            addBool(p("enabled"), juce::String(s) + " Filter " + juce::String(f + 1) + " Enabled", true);
        }
        addChoice(route(s), juce::String(s) + " Filter Routing", {"Serial", "Parallel", "Split", "Crossfeed"}, 1);
        addFloat(scene(s, "master"), juce::String(s) + " Master", 0.f, 1.f, sceneIsCrypt ? .76f : .62f);
        addFloat(scene(s, "balance"), juce::String(s) + " Balance", -1.f, 1.f, sceneIsCrypt ? -.28f : .28f);
        addFloat(scene(s, "character"), juce::String(s) + " Character", 0.f, 1.f, sceneIsCrypt ? .38f : .32f);
        addBool(scene(s, "crossfm"), juce::String(s) + " Cross FM", false);
        addBool(scene(s, "crossring"), juce::String(s) + " Cross Ring", false);
    }

    addChoice("ritual.mode", "Ritual Mode", {"Bind", "Sacrifice", "Summon", "Possess", "Devour"}, 0);
    addFloat("ritual.mix", "Ritual Mix", 0.f, 1.f, .0f); addFloat("ritual.depth", "Ritual Depth", 0.f, 1.f, .35f); addFloat("ritual.drive", "Ritual Fury", 0.f, 1.f, .12f); addFloat("ritual.width", "Ritual Width", 0.f, 1.f, .55f); addFloat("ritual.feedback", "Ritual Feedback", 0.f, .92f, .28f);
    addFloat("grave.reverb", "Grave Reverb", 0.f, 1.f, .10f); addFloat("grave.delay", "Grave Delay", 0.f, 1.f, .04f); addFloat("grave.feedback", "Grave Feedback", 0.f, .95f, .18f); addFloat("grave.cutoff", "Grave Tone", .02f, .48f, .38f); addFloat("grave.output", "Grave Output", 0.f, 1.f, .82f);
    addFloat("global.glide", "Castle Glide", 0.f, 1.f, 0.f); addFloat("global.unison", "Castle Unison", 1.f, 8.f, 1.f); addFloat("global.hex", "Hex Amount", 0.f, 1.f, 0.f);
    addFloat("corpse.position", "Corpse Position", 0.f, 1.f, .34f); addFloat("corpse.rot", "Corpse Rot", 0.f, 1.f, .22f); addFloat("corpse.formant", "Corpse Formant", -1.f, 1.f, 0.f); addFloat("corpse.inharmonic", "Corpse Inharmonicity", 0.f, 1.f, .08f);
    addFloat("possession.bloodFeed", "Blood Feed", 0.f, 1.f, 0.f); addFloat("possession.aetherLeak", "Aether Leak", 0.f, 1.f, 0.f); addFloat("possession.soulExchange", "Soul Exchange", 0.f, 1.f, 0.f); addFloat("possession.haunt", "Haunt", 0.f, 1.f, 0.f);
    addBool("ecology.enabled", "Nervous System", false); addFloat("ecology.depth", "Ecology Depth", 0.f, 1.f, .65f);
    addBool("rituals.enabled", "Rituals Enabled", false); addChoice("rituals.pattern", "Rituals Pattern", {"Procession", "Ascension", "Descent", "Circle", "Seance", "Possession", "Sacrifice", "Chaos"}, 0); addChoice("rituals.rate", "Rituals Rate", {"1/4", "1/8", "1/16", "1/32"}, 2); addFloat("rituals.bpm", "Rituals BPM", 30.f, 240.f, 120.f); addFloat("rituals.gate", "Rituals Gate", .05f, .98f, .62f); addFloat("rituals.probability", "Rituals Probability", 0.f, 1.f, 1.f); addFloat("rituals.swing", "Rituals Swing", 0.f, 1.f, 0.f); addFloat("rituals.octaves", "Rituals Octaves", 1.f, 4.f, 1.f);

    const char* curseSources[] = {"Off", "Blood", "Wraith", "Velocity", "Key", "Random", "Pulse", "Mod Wheel", "Aftertouch"};
    const char* curseKinds[] = {"Clean", "Corrupt", "Haunt", "Possession", "Decay", "Madness", "Blood"};
    const char* curseDests[] = {"Off", "Crypt Cutoff", "Tower Cutoff", "Crypt Shape", "Tower Shape", "FM Depth", "Filter Drive", "Ritual Mix", "Grave Mix", "Pitch", "Curse Depth", "Crypt Dread", "Tower Aether", "Ritual Depth", "Ritual Fury", "Ritual Feedback", "Grave Tone", "Blood Feed", "Aether Leak", "Soul Exchange", "Haunt", "Grave Feedback"};
    for (int i = 1; i <= 8; ++i) {
        auto prefix = juce::String("hex.curse") + juce::String(i) + ".";
        addChoice(prefix + "source", "Hex Curse " + juce::String(i) + " Source", {curseSources[0], curseSources[1], curseSources[2], curseSources[3], curseSources[4], curseSources[5], curseSources[6], curseSources[7], curseSources[8]}, 0);
        addChoice(prefix + "curse", "Hex Curse " + juce::String(i) + " Curse", {curseKinds[0], curseKinds[1], curseKinds[2], curseKinds[3], curseKinds[4], curseKinds[5], curseKinds[6]}, 0);
        addChoice(prefix + "destination", "Hex Curse " + juce::String(i) + " Destination", {curseDests[0], curseDests[1], curseDests[2], curseDests[3], curseDests[4], curseDests[5], curseDests[6], curseDests[7], curseDests[8], curseDests[9], curseDests[10], curseDests[11], curseDests[12], curseDests[13], curseDests[14], curseDests[15], curseDests[16], curseDests[17], curseDests[18], curseDests[19], curseDests[20], curseDests[21]}, 0);
        addFloat(prefix + "amount", "Hex Curse " + juce::String(i) + " Amount", -1.f, 1.f, 0.f); addFloat(prefix + "decay", "Hex Curse " + juce::String(i) + " Legacy Decay", 0.f, 1.f, .25f);
        addFloat(prefix + "corruptBias", "Hex Curse " + juce::String(i) + " Corrupt Bias", -1.f, 1.f, .22f); addFloat(prefix + "hauntRate", "Hex Curse " + juce::String(i) + " Haunt Rate", .05f, 8.f, 1.70f); addFloat(prefix + "possessionFold", "Hex Curse " + juce::String(i) + " Possession Fold", 0.f, 1.f, .35f); addFloat(prefix + "decayTime", "Hex Curse " + juce::String(i) + " Decay Time", .05f, 8.f, 1.50f); addFloat(prefix + "madnessSteps", "Hex Curse " + juce::String(i) + " Madness Steps", 2.f, 32.f, 8.f); addFloat(prefix + "bloodHunger", "Hex Curse " + juce::String(i) + " Blood Hunger", 0.f, 1.f, .65f);
    }
    return l;
}
} // namespace param
} // namespace horrorcastle

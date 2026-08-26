#include "Grimoire.h"

namespace horrorcastle {

std::vector<Grimoire::SpellInfo> Grimoire::getFactorySpells() const
{
    using S=SpellInfo;
    return {
        S{"The Thing Below","Something moved under the chapel","Subterranean Undercrypt and Corpse mass, breathing through Haunt and a cavernous Grave.","CRYPT","DARK • BASS • EVOLVING",0},
        S{"Glass Cathedral","Hymns through broken windows","Bell Glass and Spectral Spire suspended in a cold, luminous Grave.","TOWER","BRIGHT • GLASS • ATMOS",1},
        S{"Bone Procession","The dead keep time","Bone Resonator marching beneath Procession Rituals and Blood Feed.","RITUALS","RHYTHMIC • BONE • DARK",2},
        S{"Unholy Choir","Many mouths, one breath","Corpse, Bell Glass and Astral material exchanging souls through Possess.","POSSESSION","CHOIR • POSSESSED • WIDE",3},
        S{"Dead Star","A light that should not exist","Astral FM and Prism pulled into Summon feedback with leaking Aether.","TOWER","ASTRAL • METALLIC • SPACE",4},
        S{"Basement Door","It opens from the other side","Undercrypt and Rotator with a long cross-chamber haunting and delayed Grave.","ATMOSPHERE","DRONE • HAUNTED • LOW",5},
        S{"Bell Tower 3AM","Thirteen chimes, no bell","Bell Glass, Spectral Spire and Prism driven by a Seance pattern.","RITUALS","BELLS • SEANCE • NIGHT",6},
        S{"Possessed Music Box","It plays after the key stops","Bone Resonator and Astral FM twisted by Soul Exchange and Possession Rituals.","POSSESSION","MUSIC BOX • CREEPY • MOTION",7},

        S{"Buried Alive","Earth over the lid","Undercrypt pressure, low-pass stone and Blood modulation for crushing basses.","CRYPT","BASS • HEAVY • BLOOD",8},
        S{"Catacomb Heart","A pulse beneath the floor","Corpse and Undercrypt with Pulse-to-Dread HEX movement.","HEXED","PULSE • DARK • MODULATED",9},
        S{"Bone Orchard","Branches made of ribs","Multiple Bone Resonators with dry Grave reflections and slow Dread.","CRYPT","RESONANT • ORGANIC • DRY",10},
        S{"Black Coffin","No air inside","Narrow Rotator/Corpse body with oppressive Grave damping.","CRYPT","MONO • CLOSED • DREAD",11},
        S{"Crypt Lanterns","Lights in the tunnels","Undercrypt under a flickering Wraith-to-cutoff Curse.","HEXED","WRAITH • FILTER • FLICKER",12},

        S{"Wailing Spires","Echoes in stone","A towering evolving texture of upper inharmonics and distant spectral motion.","TOWER","ATMOS • EVOLVING • BRIGHT",13},
        S{"Moonlit Belfry","Cold iron under moonlight","Bell Glass and Spectral Spire with a clear, ringing Grave.","TOWER","BELL • COLD • WIDE",14},
        S{"Astral Bell","Orbiting metal","Astral FM with a restrained Prism halo and Aether emphasis.","TOWER","FM • BELL • ORBIT",15},
        S{"The Watcher","Something above the parapet","Sparse Spectral Spire with random HEX pressure on Aether.","HEXED","SPARSE • RANDOM • TENSION",16},
        S{"Prism Chapel","Light bends wrong here","Prism-heavy harmonic refraction with bright Ritual Bind.","TOWER","PRISM • HARMONIC • AIR",17},

        S{"Body Exchange","Neither voice returns intact","Strong Soul Exchange between Corpse and Astral FM.","POSSESSION","CROSS-FM • MORPH • DARK",18},
        S{"Taken","The room changes when it enters","Blood Feed and Haunt push both chambers into unstable motion.","POSSESSION","HAUNT • BLOOD • MOVING",19},
        S{"Two Souls","One note, two memories","Balanced chambers with moderate Soul Exchange and wide Ritual Possess.","POSSESSION","DUAL • WIDE • POSSESS",20},
        S{"Unholy Communion","The chambers feed each other","Blood Feed and Aether Leak form a dense bidirectional infection.","POSSESSION","DENSE • INFECTED • WIDE",21},
        S{"The Visitor","It was never invited","A subtle Haunt imprint that grows into Devour at the Ritual Bus.","POSSESSION","CINEMATIC • HAUNT • DEVOUR",22},

        S{"Blood Sigil","The seal drinks first","Blood Curse drives Grave feedback and CRYPT Dread.","HEXED","BLOOD • FEEDBACK • DREAD",23},
        S{"Mad King's Staircase","Every step is the wrong height","Madness quantizes pitch and Ritual Fury for broken ceremonial motion.","HEXED","MADNESS • PITCH • STAIRS",24},
        S{"Haunting Hour","The clock has no hands","Wraith/Haunt modulation drifts Tower Aether and Grave Tone.","HEXED","WRAITH • HAUNT • CLOCK",25},
        S{"Rotting Signal","Transmission from below","Decay Curse collapses filter drive and Soul Exchange in uneven breaths.","HEXED","DECAY • SIGNAL • DIRTY",26},
        S{"The Witching","Pulse becomes ritual","Pulse-driven Possession Curse moves Ritual Depth and feedback.","HEXED","PULSE • RITUAL • CURSED",27},

        S{"Black Mass","Circle closes at midnight","Sacrifice Rituals with high Fury, gated movement and Grave weight.","RITUALS","SACRIFICE • RHYTHMIC • HEAVY",28},
        S{"Grave Waltz","Three steps with the lost","Circle pattern, moderate swing, Bell Glass and Bone Resonator dance together.","RITUALS","WALTZ • SWING • GHOSTLY",29},
        S{"Circle of Ravens","They return to the same stone","Circle Rituals with random Curse movement and short Grave echoes.","RITUALS","CIRCLE • RANDOM • RAVENS",30},
        S{"Ascension of the Dead","They climb past the bell tower","Ascending Rituals drive CRYPT into TOWER with growing Aether Leak.","RITUALS","ASCENDING • CINEMATIC • LARGE",31},

        S{"Choir of Ash","A voice reconstructed from soot","Spectral Corpse frame motion with a lifted formant and slow Grave bloom.","CORPSE","RESYNTH • CHOIR • EVOLVING",32},
        S{"The Last Breath","One spectrum before silence","A restrained Corpse model with late-frame movement, soft ROT and long dark air.","CORPSE","BREATH • DARK • MORPH",33},
        S{"Mummified Tape","The recording decayed but kept singing","High ROT and mild inharmonic stretch create brittle spectral tape remains.","CORPSE","DEGRADED • TAPE • SPECTRAL",34},
        S{"Rot Cathedral","Every harmonic loses a stone","Corpse ROT strips harmonic families while Ritual Bind holds the body together.","CORPSE","ROT • ARCHITECTURAL • DARK",35},
        S{"Hollow Saint","A formant where a face should be","Down-shifted Corpse formants and sparse Bone Resonator reinforcement.","CORPSE","FORMANT • HOLLOW • VOICE",36},
        S{"Formant Crypt","The mouth is larger than the room","Extreme spectral formant remapping with slow frame travel and restrained Grave.","CORPSE","FORMANT • RESYNTH • LOW",37},
        S{"Broken Anatomy","The partials no longer agree","Inharmonic Corpse stretching, Soul Exchange and a small Possess blend.","CORPSE","INHARMONIC • POSSESSED • TENSE",38},
        S{"Spectral Reliquary","A body preserved as frames","Balanced Position/ROT/Formant movement showing the complete Spectral Corpse engine.","CORPSE","SHOWCASE • MORPH • CINEMATIC",39},
    };
}

juce::StringArray Grimoire::getFactoryNames() const
{
    juce::StringArray names;
    for (const auto& spell : getFactorySpells()) names.add(spell.name);
    return names;
}

void Grimoire::set(const juce::String& id, float actual)
{
    if (auto* p = apvts.getParameter(id)) {
        p->beginChangeGesture();
        p->setValueNotifyingHost(p->convertTo0to1(actual));
        p->endChangeGesture();
    }
}

void Grimoire::setHex(int lane,int source,int curse,int destination,float amount)
{
    const auto p=juce::String("hex.curse")+juce::String(juce::jlimit(1,8,lane))+".";
    set(p+"source",(float)source); set(p+"curse",(float)curse); set(p+"destination",(float)destination); set(p+"amount",amount);
}

void Grimoire::commonReset()
{
    set("crypt.master", .78f); set("tower.master", .74f); set("global.hex", 1.f);
    set("crypt.character",.72f); set("tower.character",.70f);
    set("ritual.mode",0.f); set("ritual.mix", .28f); set("ritual.depth", .42f); set("ritual.drive", .20f); set("ritual.width",.55f);
    set("ritual.feedback", .28f); set("grave.reverb", .32f); set("grave.delay", .14f);
    set("grave.feedback", .24f); set("grave.cutoff", .28f); set("grave.output", .78f);
    set("corpse.position",.34f);set("corpse.rot",.22f);set("corpse.formant",0.f);set("corpse.inharmonic",.08f);
    set("possession.bloodFeed", 0.f); set("possession.aetherLeak", 0.f);
    set("possession.soulExchange", 0.f); set("possession.haunt", 0.f);
    set("rituals.enabled", 0.f); set("rituals.pattern",0.f); set("rituals.rate",2.f); set("rituals.bpm",120.f);
    set("rituals.gate",.62f); set("rituals.probability",1.f); set("rituals.swing",0.f); set("rituals.octaves",1.f);
    for (int i=1;i<=8;++i) {
        const auto p=juce::String("hex.curse")+juce::String(i)+".";
        set(p+"source",0.f); set(p+"curse",0.f); set(p+"destination",0.f); set(p+"amount",0.f);
    }
}

bool Grimoire::loadFactory(int index)
{
    if(index<0||index>=(int)getFactorySpells().size()) return false;
    commonReset();
    switch(index) {
        case 0: set("crypt.character",1.f);set("tower.master",.18f);set("crypt.g1.type",8.f);set("crypt.g2.type",9.f);set("crypt.f1.cutoff",.10f);set("crypt.f2.cutoff",.16f);set("possession.haunt",.32f);set("grave.reverb",.48f);break;
        case 1: set("crypt.master",.20f);set("tower.character",1.f);set("tower.g1.type",8.f);set("tower.g2.type",9.f);set("tower.f1.cutoff",.10f);set("tower.f2.cutoff",.46f);set("grave.reverb",.62f);set("grave.cutoff",.44f);break;
        case 2: set("crypt.g1.type",10.f);set("crypt.g2.type",8.f);set("crypt.character",.88f);set("tower.master",.32f);set("rituals.enabled",1.f);set("rituals.pattern",0.f);set("rituals.rate",2.f);set("rituals.gate",.52f);set("possession.bloodFeed",.42f);break;
        case 3: set("crypt.g1.type",9.f);set("tower.g1.type",8.f);set("tower.g2.type",10.f);set("possession.soulExchange",.46f);set("ritual.mode",3.f);set("ritual.mix",.58f);set("grave.reverb",.55f);break;
        case 4: set("crypt.master",.35f);set("tower.g1.type",10.f);set("tower.g2.type",11.f);set("tower.character",.94f);set("ritual.mode",2.f);set("ritual.mix",.70f);set("ritual.feedback",.62f);set("possession.aetherLeak",.48f);break;
        case 5: set("crypt.g1.type",8.f);set("crypt.g2.type",11.f);set("tower.master",.12f);set("possession.haunt",.72f);set("grave.delay",.42f);set("grave.feedback",.58f);set("grave.reverb",.40f);break;
        case 6: set("crypt.master",.14f);set("tower.g1.type",8.f);set("tower.g2.type",9.f);set("tower.g3.type",11.f);set("tower.character",1.f);set("rituals.enabled",1.f);set("rituals.pattern",4.f);set("rituals.rate",3.f);set("rituals.probability",.72f);set("grave.reverb",.68f);break;
        case 7: set("crypt.g1.type",10.f);set("tower.g1.type",8.f);set("tower.g2.type",10.f);set("possession.soulExchange",.72f);set("possession.haunt",.34f);set("ritual.mode",3.f);set("ritual.mix",.68f);set("rituals.enabled",1.f);set("rituals.pattern",5.f);set("rituals.rate",2.f);break;

        case 8: set("crypt.g1.type",8.f);set("crypt.g2.type",8.f);set("crypt.character",1.f);set("tower.master",.08f);set("crypt.f1.cutoff",.065f);set("grave.reverb",.22f);setHex(1,1,6,11,.45f);break;
        case 9: set("crypt.g1.type",9.f);set("crypt.g2.type",8.f);set("crypt.character",.92f);set("tower.master",.16f);setHex(1,6,5,11,.36f);set("grave.reverb",.36f);break;
        case 10:set("crypt.g1.type",10.f);set("crypt.g2.type",10.f);set("crypt.g3.type",8.f);set("tower.master",.12f);set("crypt.f1.resonance",.64f);set("crypt.f2.resonance",.48f);break;
        case 11:set("crypt.g1.type",11.f);set("crypt.g2.type",9.f);set("tower.master",.06f);set("crypt.balance",-.12f);set("grave.cutoff",.10f);set("grave.reverb",.20f);break;
        case 12:set("crypt.g1.type",8.f);set("tower.master",.14f);setHex(1,2,2,1,.62f);set("grave.delay",.18f);break;

        case 13:set("crypt.master",.16f);set("tower.g1.type",9.f);set("tower.g2.type",8.f);set("tower.g3.type",11.f);set("tower.character",1.f);set("grave.reverb",.64f);set("grave.cutoff",.43f);setHex(1,2,2,12,.34f);break;
        case 14:set("crypt.master",.12f);set("tower.g1.type",8.f);set("tower.g2.type",9.f);set("tower.character",.92f);set("grave.reverb",.52f);set("grave.delay",.22f);break;
        case 15:set("crypt.master",.10f);set("tower.g1.type",10.f);set("tower.g2.type",8.f);set("tower.character",.95f);set("possession.aetherLeak",.18f);set("ritual.mix",.34f);break;
        case 16:set("crypt.master",.08f);set("tower.g1.type",9.f);set("tower.g2.type",11.f);setHex(1,5,2,12,.52f);set("grave.reverb",.48f);break;
        case 17:set("crypt.master",.18f);set("tower.g1.type",11.f);set("tower.g2.type",11.f);set("ritual.mode",0.f);set("ritual.mix",.44f);set("ritual.width",.82f);break;

        case 18:set("crypt.g1.type",9.f);set("tower.g1.type",10.f);set("possession.soulExchange",.82f);set("ritual.mode",3.f);set("ritual.mix",.52f);break;
        case 19:set("crypt.g1.type",9.f);set("tower.g1.type",9.f);set("possession.bloodFeed",.62f);set("possession.haunt",.72f);set("grave.reverb",.42f);break;
        case 20:set("crypt.g1.type",10.f);set("tower.g1.type",8.f);set("possession.soulExchange",.48f);set("ritual.mode",3.f);set("ritual.width",.90f);set("grave.reverb",.50f);break;
        case 21:set("crypt.g1.type",8.f);set("tower.g1.type",10.f);set("possession.bloodFeed",.66f);set("possession.aetherLeak",.68f);set("possession.soulExchange",.42f);set("ritual.mix",.46f);break;
        case 22:set("crypt.g1.type",11.f);set("tower.g1.type",9.f);set("possession.haunt",.58f);set("ritual.mode",4.f);set("ritual.mix",.62f);set("ritual.drive",.52f);break;

        case 23:set("crypt.character",.96f);set("grave.feedback",.62f);setHex(1,1,6,11,.54f);setHex(2,1,6,21,.34f);break;
        case 24:set("ritual.drive",.55f);set("ritual.mix",.46f);setHex(1,6,5,9,.12f);setHex(2,6,5,14,.48f);break;
        case 25:set("tower.character",.92f);set("grave.cutoff",.20f);setHex(1,2,2,12,.42f);setHex(2,2,2,16,-.22f);break;
        case 26:set("crypt.g1.type",9.f);set("tower.g1.type",10.f);set("possession.soulExchange",.52f);setHex(1,5,4,6,.58f);setHex(2,5,4,19,-.35f);break;
        case 27:set("ritual.mix",.58f);set("ritual.depth",.62f);set("ritual.feedback",.44f);setHex(1,6,3,13,.58f);setHex(2,6,3,15,.22f);break;

        case 28:set("ritual.mode",1.f);set("ritual.mix",.72f);set("ritual.drive",.62f);set("rituals.enabled",1.f);set("rituals.pattern",6.f);set("rituals.rate",2.f);set("rituals.gate",.38f);set("grave.reverb",.38f);break;
        case 29:set("crypt.g1.type",10.f);set("tower.g1.type",8.f);set("rituals.enabled",1.f);set("rituals.pattern",3.f);set("rituals.rate",1.f);set("rituals.swing",.34f);set("ritual.mix",.42f);set("grave.reverb",.56f);break;
        case 30:set("rituals.enabled",1.f);set("rituals.pattern",3.f);set("rituals.rate",2.f);set("rituals.probability",.76f);setHex(1,5,2,7,.24f);set("grave.delay",.28f);break;
        case 31:set("rituals.enabled",1.f);set("rituals.pattern",1.f);set("rituals.rate",2.f);set("rituals.octaves",3.f);set("possession.aetherLeak",.44f);set("ritual.mode",2.f);set("ritual.mix",.54f);set("grave.reverb",.62f);break;

        case 32:set("crypt.g1.type",9.f);set("crypt.g2.type",9.f);set("tower.master",.14f);set("corpse.position",.62f);set("corpse.rot",.18f);set("corpse.formant",.48f);set("corpse.inharmonic",.06f);set("grave.reverb",.56f);break;
        case 33:set("crypt.g1.type",9.f);set("crypt.g2.type",8.f);set("tower.master",.08f);set("corpse.position",.86f);set("corpse.rot",.28f);set("corpse.formant",-.12f);set("corpse.inharmonic",.04f);set("grave.reverb",.64f);set("grave.cutoff",.18f);break;
        case 34:set("crypt.g1.type",9.f);set("tower.master",.10f);set("corpse.position",.44f);set("corpse.rot",.82f);set("corpse.formant",.18f);set("corpse.inharmonic",.42f);set("grave.delay",.12f);break;
        case 35:set("crypt.g1.type",9.f);set("crypt.g2.type",10.f);set("tower.master",.12f);set("corpse.position",.52f);set("corpse.rot",.94f);set("corpse.formant",0.f);set("corpse.inharmonic",.14f);set("ritual.mode",0.f);set("ritual.mix",.44f);break;
        case 36:set("crypt.g1.type",9.f);set("crypt.g2.type",10.f);set("tower.master",.10f);set("corpse.position",.38f);set("corpse.rot",.34f);set("corpse.formant",-.72f);set("corpse.inharmonic",.09f);set("grave.reverb",.46f);break;
        case 37:set("crypt.g1.type",9.f);set("tower.master",.06f);set("corpse.position",.70f);set("corpse.rot",.26f);set("corpse.formant",.88f);set("corpse.inharmonic",.12f);set("crypt.f1.cutoff",.12f);break;
        case 38:set("crypt.g1.type",9.f);set("tower.g1.type",10.f);set("corpse.position",.58f);set("corpse.rot",.48f);set("corpse.formant",.16f);set("corpse.inharmonic",.88f);set("possession.soulExchange",.34f);set("ritual.mode",3.f);set("ritual.mix",.32f);break;
        case 39:set("crypt.g1.type",9.f);set("crypt.g2.type",9.f);set("tower.master",.20f);set("corpse.position",.50f);set("corpse.rot",.50f);set("corpse.formant",.22f);set("corpse.inharmonic",.32f);set("grave.reverb",.50f);set("grave.delay",.18f);break;
        default:return false;
    }
    return true;
}

juce::File Grimoire::getUserDirectory() const
{
    auto dir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
        .getChildFile("SleepFighterStudios").getChildFile("Horror Castle").getChildFile("Grimoire");
    dir.createDirectory(); return dir;
}

juce::File Grimoire::saveUserPreset(const juce::String& requestedName) const
{
    auto safe = juce::File::createLegalFileName(requestedName.trim().isEmpty() ? "Untitled Spell" : requestedName.trim());
    auto file = getUserDirectory().getNonexistentChildFile(safe, ".hcg", false);
    auto state = apvts.copyState(); state.setProperty("stateVersion", CurrentStateVersion, nullptr); state.setProperty("productVersion", "1.2.0", nullptr);
    if (auto xml = state.createXml()) xml->writeTo(file);
    return file;
}

bool Grimoire::loadUserPreset(const juce::File& file)
{
    if (!file.existsAsFile()) return false;
    if (auto xml = juce::XmlDocument::parse(file)) {
        auto state = juce::ValueTree::fromXml(*xml); migrateState(state);
        if (state.isValid() && state.hasType(apvts.state.getType())) { apvts.replaceState(state); return true; }
    }
    return false;
}

void Grimoire::migrateState(juce::ValueTree& state)
{
    if (!state.isValid()) return;
    const int version = (int)state.getProperty("stateVersion", 13);
    if (version < CurrentStateVersion) state.setProperty("stateVersion", CurrentStateVersion, nullptr);
    state.setProperty("productVersion", "1.2.0", nullptr);
}

} // namespace horrorcastle

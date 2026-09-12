#include "FactoryPresetManifest.h"

namespace horrorcastle {

std::vector<FactoryPresetDefinition> FactoryPresetManifest::create()
{
    using P=FactoryPresetDefinition;
    std::vector<P> out;
    out.reserve(FactoryPresetCount);
    auto add=[&](const char* n,const char* sub,const char* desc,const char* cat,const char* tags,int sigil,int archetype,std::uint32_t seed){out.push_back(P{n,sub,desc,cat,tags,sigil,archetype,seed});};

    // The first 44 names are compatibility identities: indices must never move.
    add("The Thing Below","Something moved under the chapel","Subterranean Undercrypt and Corpse mass, breathing through Haunt and a cavernous Grave.","CRYPT","DARK • BASS • EVOLVING",0,0,0);
    add("Glass Cathedral","Hymns through broken windows","Bell Glass and Spectral Spire suspended in a cold, luminous Grave.","TOWER","BRIGHT • GLASS • ATMOS",1,1,0);
    add("Bone Procession","The dead keep time","Bone Resonator marching beneath Procession Rituals and Blood Feed.","RITUALS","RHYTHMIC • BONE • DARK",2,2,0);
    add("Unholy Choir","Many mouths, one breath","Corpse, Bell Glass and Astral material exchanging souls through Possess.","POSSESSION","CHOIR • POSSESSED • WIDE",3,3,0);
    add("Dead Star","A light that should not exist","Astral FM and Prism pulled into Summon feedback with leaking Aether.","TOWER","ASTRAL • METALLIC • SPACE",4,4,0);
    add("Basement Door","It opens from the other side","Undercrypt and Rotator with a long cross-chamber haunting and delayed Grave.","ATMOSPHERE","DRONE • HAUNTED • LOW",5,5,0);
    add("Bell Tower 3AM","Thirteen chimes, no bell","Bell Glass, Spectral Spire and Prism driven by a Seance pattern.","RITUALS","BELLS • SEANCE • NIGHT",6,6,0);
    add("Possessed Music Box","It plays after the key stops","Bone Resonator and Astral FM twisted by Soul Exchange and Possession Rituals.","POSSESSION","MUSIC BOX • CREEPY • MOTION",7,7,0);
    const char* legacyNames[]={"Buried Alive","Catacomb Heart","Bone Orchard","Black Coffin","Crypt Lanterns","Wailing Spires","Moonlit Belfry","Astral Bell","The Watcher","Prism Chapel","Body Exchange","Taken","Two Souls","Unholy Communion","The Visitor","Blood Sigil","Mad King's Staircase","Haunting Hour","Rotting Signal","The Witching","Black Mass","Grave Waltz","Circle of Ravens","Ascension of the Dead","Choir of Ash","The Last Breath","Mummified Tape","Rot Cathedral","Hollow Saint","Formant Crypt","Broken Anatomy","Spectral Reliquary","Black Weather","Static Halo","The Castle Breathes","Mirror in the Flood"};
    const char* legacyCats[]={"CRYPT","HEXED","CRYPT","CRYPT","HEXED","TOWER","TOWER","TOWER","HEXED","TOWER","POSSESSION","POSSESSION","POSSESSION","POSSESSION","POSSESSION","HEXED","HEXED","HEXED","HEXED","HEXED","RITUALS","RITUALS","RITUALS","RITUALS","CORPSE","CORPSE","CORPSE","CORPSE","CORPSE","CORPSE","CORPSE","CORPSE","LIVING ECOLOGY","LIVING ECOLOGY","LIVING ECOLOGY","LIVING ECOLOGY"};
    for(int i=0;i<36;++i) add(legacyNames[i],"Factory spell","Original Horror Castle factory identity.",legacyCats[i],"LEGACY • CURATED",8+i,8+i,0);

    // Launch-bank additions: 56 curated identities generated from 10 deterministic archetypes.
    struct N { const char* name; const char* category; const char* tags; int archetype; };
    const N fresh[]={
      {"Grave Current","BASS","BASS • VORTEX • SUB",0},{"Coffin Pressure","BASS","BASS • COFFIN • HEAVY",0},{"Marrow Substation","BASS","BASS • MARROW • ELECTRIC",0},{"Abyssal Weight","BASS","BASS • ABYSS • DEEP",0},{"Blood Cellar","BASS","BASS • BLOOD • DIRTY",0},{"Vortex Jaw","BASS","BASS • FLUID • AGGRESSIVE",0},{"Stone Lung","BASS","BASS • BREATH • DARK",0},{"Crypt Engine","BASS","BASS • MACHINE • LOW",0},{"Burial Voltage","BASS","BASS • POLTERGEIST • PUNCH",0},{"Underfloor Titan","BASS","BASS • CINEMATIC • MASSIVE",0},{"Iron Hunger","BASS","BASS • DRIVE • MONO",0},{"Black Reservoir","BASS","BASS • WATER • EVOLVING",0},
      {"Wraith Needle","LEAD","LEAD • WRAITH • EXPRESSIVE",1},{"Aurora Knife","LEAD","LEAD • AURORA • BRIGHT",1},{"Siren Crown","LEAD","LEAD • SIREN • AIR",1},{"Spectral Fang","LEAD","LEAD • CORPSE • SHARP",1},{"Reliquary Voice","LEAD","LEAD • RELIQUARY • RESONANT",1},{"Astral Heretic","LEAD","LEAD • FM • CELESTIAL",1},{"Lightning Saint","LEAD","LEAD • ELECTRIC • AFTERTOUCH",1},{"Glass Exorcist","LEAD","LEAD • GLASS • WIDE",1},
      {"Ashen Heaven","PAD","PAD • CHOIR • WIDE",2},{"Aurora Veil","PAD","PAD • AURORA • EVOLVING",2},{"Mirror Chapel","PAD","PAD • MIRROR • GLASS",2},{"Wraith Communion","PAD","PAD • WRAITH • HAUNTED",2},{"Reliquary Cloud","PAD","PAD • RELIQUARY • AIR",2},{"Corpse Bloom","PAD","PAD • CORPSE • MORPH",2},{"Dead Moon Choir","PAD","PAD • CHOIR • DARK",2},{"Frozen Seance","PAD","PAD • SEANCE • COLD",2},{"Halo of Dust","PAD","PAD • SPECTRAL • SOFT",2},{"Cathedral Weather","PAD","PAD • ECOLOGY • CINEMATIC",2},
      {"Bottomless Room","DRONE","DRONE • ABYSS • LOW",3},{"Pressure Ghost","DRONE","DRONE • VORTEX • HAUNTED",3},{"Static Crypt","DRONE","DRONE • POLTERGEIST • ELECTRIC",3},{"Orrery Below","DRONE","DRONE • ORRERY • MOTION",3},{"Coffin Ocean","DRONE","DRONE • COFFIN • FLUID",3},{"Black Aurora","DRONE","DRONE • AURORA • DARK",3},
      {"Bone Keys","KEYS","KEYS • BONE • ORGANIC",4},{"Glass Teeth","KEYS","PLUCK • GLASS • BRIGHT",4},{"Mirror Harpsichord","KEYS","KEYS • MIRROR • PLUCK",4},{"Reliquary Bells","KEYS","BELLS • RELIQUARY • RESONANT",4},{"Marrow Pluck","KEYS","PLUCK • MARROW • DRY",4},{"Dead Toy Piano","KEYS","KEYS • CREEPY • SHORT",4},
      {"Rib Strike","PERCUSSION","PERCUSSION • BONE • IMPACT",5},{"Coffin Slam","PERCUSSION","PERCUSSION • COFFIN • HEAVY",5},{"Arc Snap","PERCUSSION","PERCUSSION • POLTERGEIST • ELECTRIC",5},{"Vortex Kick","PERCUSSION","PERCUSSION • VORTEX • LOW",5},
      {"Raven Engine","RITUALS","SEQUENCE • RAVEN • RHYTHMIC",6},{"Blood Procession","RITUALS","SEQUENCE • BLOOD • DARK",6},{"Possession Clock","RITUALS","SEQUENCE • POSSESSION • MOTION",6},{"Orrery Rite","RITUALS","SEQUENCE • ORRERY • CELESTIAL",6},
      {"Door Behind You","CINEMATIC FX","FX • HAUNT • RISER",7},{"Arc in the Walls","CINEMATIC FX","FX • ELECTRIC • TENSION",7},{"Flooded Chapel","CINEMATIC FX","FX • FLUID • SPACE",7},{"The Ceiling Breathes","CINEMATIC FX","FX • BREATH • HORROR",7},
      {"Living Storm","LIVING ECOLOGY","ECOLOGY • VORTEX • SIREN",8},{"Electric Séance","LIVING ECOLOGY","ECOLOGY • POLTERGEIST • AURORA",8},{"Haunted Climate","LIVING ECOLOGY","ECOLOGY • DUAL • EXPRESSIVE",8},{"Nervous Cathedral","LIVING ECOLOGY","ECOLOGY • SHOWCASE • CINEMATIC",8}
    };
    int sigil=44; std::uint32_t seed=0xC4571E01u;
    for(const auto& p:fresh){seed=seed*1664525u+1013904223u;add(p.name,"Launch collection","A curated v1.3 launch spell with deterministic variation and performance-ready gain staging.",p.category,p.tags,sigil++,p.archetype,seed);}
    jassert((int)out.size()==FactoryPresetCount);
    return out;
}

} // namespace horrorcastle

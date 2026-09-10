#pragma once
#include <array>

namespace horrorcastle::performance_roster {

// Public Performance mode intentionally exposes fewer creatures than the
// compatibility engine. Each choice must represent a clearly different
// synthesis region. Hidden legacy engines remain available to old presets and
// the Laboratory, but the Necromancer sees only this curated spellbook.
inline constexpr std::array<int,8> crypt{{0,1,2,6,9,15,16,17}};
inline constexpr std::array<int,9> tower{{3,4,8,10,11,13,15,16,17}};

} // namespace horrorcastle::performance_roster

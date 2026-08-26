#include "HorrorCastleArchitecture.h"

namespace horrorcastle {
// Deliberately small translation unit: the architecture is data-oriented so
// the audio implementation can be swapped without changing the patch model.
static_assert(static_cast<int>(GeneratorType::Resonator) == 7);
static_assert(static_cast<int>(FilterType::Shaper) == 8);
}

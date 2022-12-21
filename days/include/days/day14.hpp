#include <cstddef>

#include "days/day.hpp"
#include "offset_grid.hpp"
#include "owning_span.hpp"

namespace day14 {
constexpr inline u32 const MAX_POINTS{880};
constexpr inline u32 const MAX_WIDTH{60};
constexpr inline u32 const MAX_HEIGHT{185};
using cave_type = offset_grid<char, owning_span<char, static_cast<u32>(MAX_WIDTH * MAX_HEIGHT)>>;
} // namespace day14

using Day14 = Day<14, day14::cave_type, u32>;

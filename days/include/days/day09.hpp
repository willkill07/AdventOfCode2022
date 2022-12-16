#include "days/day.hpp"
#include "offset_grid.hpp"
#include "owning_span.hpp"

namespace day09 {
constexpr u32 const MAX_GRID_POINTS{98304};

using grid_type = offset_grid<u16, owning_span<u16, day09::MAX_GRID_POINTS>>;
} // namespace day09

using Day09 = Day<9, day09::grid_type, i64>;

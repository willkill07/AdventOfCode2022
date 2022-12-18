#include "days/day.hpp"
#include "owning_span.hpp"

namespace day18 {
constexpr u32 const MAX_DIM{24};
using grid_type = owning_span<u8, MAX_DIM * MAX_DIM * MAX_DIM>;
} // namespace day18

using Day18 = Day<18, day18::grid_type, u32>;

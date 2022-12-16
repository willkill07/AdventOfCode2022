#include "days/day.hpp"
#include "owning_span.hpp"

namespace day06 {
constexpr u32 const MAX_LEN{4096};
using buffer_t = owning_span<i8, MAX_LEN>;
} // namespace day06

using Day06 = Day<6, day06::buffer_t, i64>;

#include "days/day.hpp"
#include "owning_span.hpp"

namespace day07 {
constexpr u32 const MAX_DIRS{512};
constexpr u32 const MAX_DEPTH{10};
} // namespace day07

using Day07 = Day<7, owning_span<u32, day07::MAX_DIRS>, u32>;

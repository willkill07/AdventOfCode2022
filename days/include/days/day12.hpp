#include "days/day.hpp"
#include "owning_span.hpp"

namespace day12 {

constexpr u32 const MAX_SIZE{8192};

struct map {
  owning_span<char, MAX_SIZE> grid;
  u32 start;
  u32 stop;
  u32 width;
  u32 height;
};

} // namespace day12

using Day12 = Day<12, day12::map, u32>;

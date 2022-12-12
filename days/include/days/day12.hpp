#include <vector>

#include "days/day.hpp"

namespace day12 {

struct map {
  std::vector<char> grid;
  u32 start;
  u32 stop;
  u32 width;
  u32 height;
};

} // namespace day12

using Day12 = Day<12, day12::map, u32>;

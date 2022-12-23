#include <cstddef>

#include "days/day.hpp"
#include "owning_span.hpp"

namespace day23 {
  using grid_type = owning_span<u8, static_cast<usize>((73u + 55u) * (73u + 55u))>;

  struct result {
    grid_type grid;
    i32 xmin, xmax, ymin, ymax;
    i32 count;
  };
}

using Day23 = Day<23, day23::result, i32>;

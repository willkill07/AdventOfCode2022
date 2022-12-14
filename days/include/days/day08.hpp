#include <string_view>

#include "days/day.hpp"
#include "owning_span.hpp"

namespace day08 {

  constexpr u32 const GRID_SIZE{99};

  using visible_t = owning_span<u32, GRID_SIZE * GRID_SIZE>;
  using grid_t = owning_span<std::string_view, GRID_SIZE>;

  struct result {
    visible_t visible;
    grid_t grid;
    u32 size;
  };
}

using Day08 = Day<8, day08::result, i64>;

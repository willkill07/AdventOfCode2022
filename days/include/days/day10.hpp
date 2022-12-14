#include <array>

#include "days/day.hpp"
#include "owning_span.hpp"

namespace day10 {
  using xstate_t = owning_span<i32, 240>;
}

using Day10 = Day<10, day10::xstate_t, i32, std::string_view>;

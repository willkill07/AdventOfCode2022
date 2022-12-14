#include <string_view>
#include "days/day.hpp"
#include "owning_span.hpp"

namespace day03 {
  using list_t = owning_span<std::string_view, 300>;
}

using Day03 = Day<3, day03::list_t, int>;

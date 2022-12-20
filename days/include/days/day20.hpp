#include <array>

#include "days/day.hpp"
#include "owning_span.hpp"

namespace day20 {
  struct node {
    i64 value;
    u32 prev;
    u32 next;
  };

  using list_type = owning_span<node, 5000>;

  struct result {
    list_type list;
    u32 zero_index;
  };
}

using Day20 = Day<20, day20::result, i64>;

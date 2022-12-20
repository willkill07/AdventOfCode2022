#include "days/day.hpp"
#include "owning_span.hpp"

namespace day20 {

  constexpr u32 const MAXN{5000u};

  template <typename T>
  using list_type = owning_span<T, MAXN>;

  struct result {
    list_type<i64> numbers;
    unsigned zero_index;
  };
}

using Day20 = Day<20, day20::result, i64>;

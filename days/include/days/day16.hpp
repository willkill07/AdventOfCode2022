#include "days/day.hpp"
#include "owning_span.hpp"

namespace day16 {
  using T = i32;

  constexpr T const MAX_VALVES{55};
  constexpr T const MAX_WORKING_VALVES{16};

  using matrix_t = owning_span<T, MAX_VALVES * MAX_VALVES>;
  using vector_t = owning_span<T, MAX_VALVES>;

  using compact_matrix_t = owning_span<T, MAX_WORKING_VALVES * MAX_WORKING_VALVES>;
  using compact_vector_t = owning_span<T, MAX_WORKING_VALVES>;

  struct result_t {
    compact_vector_t flow;
    compact_matrix_t dist;
  };

}

using Day16 = Day<16, typename day16::result_t, i64>;

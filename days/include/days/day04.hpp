#include "days/day.hpp"
#include "owning_span.hpp"

namespace day04 {

constexpr u32 const MAX_RANGES{1000};

struct interval {
  u32 lo, hi;

  [[gnu::always_inline]] [[nodiscard]] inline bool encloses(interval const &other) const noexcept {
    return lo >= other.lo and hi <= other.hi;
  }
};

struct range {
  interval a, b;

  [[gnu::always_inline]] [[nodiscard]] inline bool encloses() const noexcept {
    return a.encloses(b) or b.encloses(a);
  }

  [[gnu::always_inline]] [[nodiscard]] inline bool overlaps() const noexcept {
    return (b.lo <= a.hi and a.lo <= b.hi);
  }
};

} // namespace day04

using Day04 = Day<4, owning_span<day04::range, day04::MAX_RANGES>, u32>;

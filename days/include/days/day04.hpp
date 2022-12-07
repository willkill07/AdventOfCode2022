#include <vector>

#include "days/day.hpp"

namespace day04 {
struct interval {
  u32 lo, hi;

  [[gnu::always_inline]] inline bool encloses(interval const &other) const noexcept {
    return lo >= other.lo and hi <= other.hi;
  }
};

struct range {
  interval a, b;

  [[gnu::always_inline]] inline bool encloses() const noexcept {
    return a.encloses(b) or b.encloses(a);
  }

  [[gnu::always_inline]] inline bool overlaps() const noexcept {
    return (b.lo <= a.hi and a.lo <= b.hi);
  }
};

} // namespace day04

using Day04 = Day<4, std::vector<day04::range>, u32>;

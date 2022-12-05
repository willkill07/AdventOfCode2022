#include <vector>

#include "Day.hpp"

namespace day04 {
struct interval {
  u32 lo, hi;

  inline bool between(u32 val) const noexcept {
    return lo <= val and val <= hi;
  }

  inline bool overlaps(interval const& other) const noexcept {
    return between(other.lo) or between(other.hi);
  }

  inline bool encloses(interval const& other) const noexcept {
    return between(other.lo) and between(other.hi);
  }
};

struct range {
  interval a, b;

  inline bool encloses() const noexcept {
    return a.encloses(b) or b.encloses(a);
  }

  inline bool overlaps() const noexcept {
    return a.overlaps(b) or b.overlaps(a);
  }

};

} // namespace day04

using Day04 = Day<4, std::vector<day04::range>, u32>;

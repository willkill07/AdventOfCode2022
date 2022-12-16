#include <array>

#include "days/day.hpp"
#include "owning_span.hpp"
#include "point2d.hpp"

namespace day15 {

struct entry {
  point2d b;
  point2d s;

  constexpr inline entry(point2d const &bp, point2d const &sp) noexcept
      : b{bp},
        s{sp} {
  }

  constexpr inline entry() noexcept = default;
  constexpr inline entry(entry const &) noexcept = default;
  constexpr inline entry(entry &&) noexcept = default;
  constexpr inline entry &operator=(entry const &) noexcept = default;
  constexpr inline entry &operator=(entry &&) noexcept = default;
};

constexpr static inline u32 const MAX_ENTRIES{36};
using result_type = owning_span<entry, MAX_ENTRIES>;
} // namespace day15

using Day15 = Day<15, day15::result_type, i64>;

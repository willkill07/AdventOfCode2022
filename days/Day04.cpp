#include <numeric>

#include "Day.hpp"
#include "Day04.hpp"

template <std::unsigned_integral T>
[[gnu::always_inline]] T
str_to(std::string_view s) noexcept {
  return std::accumulate(std::begin(s) + 1, std::end(s), static_cast<T>(s.front() - '0'), [](T acc, char c) {
    return T{10} * acc + T(c - '0');
  });
}

PARSE_IMPL(Day04, buffer) {
  std::vector<day04::range> ranges;
  std::string_view view{buffer.data(), buffer.size()};
  for (sz off{0}; off < std::size(view); ++off) {
    sz const len_a{view.substr(off).find_first_of('-')};
    auto const a{str_to<u32>(view.substr(off, len_a))};
    off += len_a + 1;
    sz const len_b{view.substr(off).find_first_of(',')};
    auto const b{str_to<u32>(view.substr(off, len_b))};
    off += len_b + 1;
    sz const len_c{view.substr(off).find_first_of('-')};
    auto const c{str_to<u32>(view.substr(off, len_c))};
    off += len_c + 1;
    sz const len_d{view.substr(off).find_first_of('\n')};
    auto const d{str_to<u32>(view.substr(off, len_d))};
    off += len_d;
    ranges.push_back({{a, b}, {c, d}});
  }
  return ranges;
}

SOLVE_IMPL(Day04, Part2, ranges, part1_answer) {
  return std::accumulate(std::begin(ranges), std::end(ranges), 0u, [](u32 sum, day04::range const &r) {
    if constexpr (Part2) {
      return sum + ((r.a.lo <= r.b.lo and r.b.lo <= r.a.hi) or (r.a.lo <= r.b.hi and r.b.hi <= r.a.hi) or
                    (r.b.lo <= r.a.lo and r.a.lo <= r.b.hi) or (r.b.lo <= r.a.hi and r.a.hi <= r.b.hi));
    } else {
      return sum + ((r.a.lo <= r.b.lo and r.b.hi <= r.a.hi) or (r.b.lo <= r.a.lo and r.a.hi <= r.b.hi));
    }
  });
}

INSTANTIATE(Day04);
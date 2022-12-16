#include "days/day15.hpp"
#include "owning_span.hpp"
#include "parsing.hpp"
#include "types.hpp"

#include <fmt/core.h>
#include <iterator>
#include <limits>
#include <math.h>

namespace {

struct mark {
  i32 value;
  bool start;
  constexpr auto operator<=>(mark const &) const noexcept = default;
};

} // namespace

PARSE_IMPL(Day15, view) {
  usize off{0};
  day15::result_type entries(0);
  while (off < std::size(view)) {
    i32 sx, sy, bx, by;
    off += parse<"Sensor at x=\0, y=\1: closest beacon is at x=\2, y=\3\n">(view.substr(off), sx, sy, bx, by);
    entries.push({{sx, sy}, {bx, by}});
  }
  return entries;
}

PART1_IMPL(Day15, data) {
  i32 const target_row = (data.size() <= 15 ? 10 : 2'000'000);
  owning_span<mark, day15::MAX_ENTRIES * 2> marks;
  for (auto &&[s, b] : data) {
    i32 const units_away{s.manhattan(b) - std::abs(target_row - s.y)};
    if (units_away >= 0) {
      marks.push({s.x - units_away, true});
      marks.push({s.x + (units_away + 1), false});
    }
  }
  std::sort(std::begin(marks), std::end(marks));
  i32 answer{-1}, count{0}, prev{std::numeric_limits<i32>::min()};
  for (auto &&[x, start] : marks) {
    // if we still have at least one active range, add the difference
    if (count > 0) {
      answer += (x - prev);
    }
    // update count according to type of mark
    count += (start - (not start));
    prev = x;
  }
  return answer;
}

namespace {

constexpr inline point2d
rotate_left(point2d const &p) noexcept {
  return {p.x + p.y, p.y - p.x};
}

constexpr inline point2d
rotate_right(point2d const &p) noexcept {
  return {(p.x - p.y) / 2, (p.y + p.x) / 2};
}

struct bounding_box {
  point2d left, right;
  constexpr inline auto operator<=>(bounding_box const &) const noexcept = default;
};

template <std::forward_iterator Iter>
constexpr inline u32
filter_unique(Iter begin, Iter end) noexcept {
  std::sort(begin, end);
  return static_cast<u32>(std::distance(begin, std::unique(begin, end)));
}

} // namespace

PART2_IMPL(Day15, data, part1_answer) {
  auto const N{std::size(data)};
  i32 const limit = (N <= 15 ? 20 : 4'000'000);

  owning_span<bounding_box, day15::MAX_ENTRIES> bounds;

  for (auto &&[s, b] : data) {
    i32 const spread{s.manhattan(b) + 1};
    // align the box to the x-axis (expands by sqrt(2))
    bounds.push({rotate_left({s.x - spread, s.y}), rotate_left({s.x + spread, s.y})});
  }

  // build candidate coordinates
  owning_span<i32, day15::MAX_ENTRIES> xlocs, ylocs;
  for (u32 i{0}; i < N - 1; ++i) {
    for (u32 j{i + 1}; j < N; ++j) {
      if (auto const x0{bounds[i].left.x}; x0 == bounds[j].right.x) {
        xlocs.push(x0);
      } else if (auto const x1{bounds[i].right.x}; x1 == bounds[j].left.x) {
        xlocs.push(x1);
      }
      if (auto const y0{bounds[i].left.y}; y0 == bounds[j].right.y) {
        ylocs.push(y0);
      } else if (auto const y1{bounds[i].right.y}; y1 == bounds[j].left.y) {
        ylocs.push(y1);
      }
    }
  }

  // reduce to unique space -- for my input there was only one candidate!
  xlocs.resize(filter_unique(std::begin(xlocs), std::end(xlocs)));
  ylocs.resize(filter_unique(std::begin(ylocs), std::end(ylocs)));

  for (auto &&x : xlocs) {
    for (auto &&y : ylocs) {
      // re-align to normal coordinate system -- restricts by factor of sqrt(2)
      point2d const v{rotate_right({x, y})};
      if (v.x >= 0 and v.x <= limit and v.y >= 0 and v.y <= limit) {
        bool candidate{true};
        for (auto &&[s, b] : data) {
          // only a valid candidate IFF outside of the sensor region
          candidate = candidate and (s.manhattan(v) > s.manhattan(b));
        }
        if (candidate) {
          return (4'000'000l * v.x) + v.y;
        }
      }
    }
  }
  return {};
}

INSTANTIATE_TEST(Day15,
                 R"(
Sensor at x=2, y=18: closest beacon is at x=-2, y=15
Sensor at x=9, y=16: closest beacon is at x=10, y=16
Sensor at x=13, y=2: closest beacon is at x=15, y=3
Sensor at x=12, y=14: closest beacon is at x=10, y=16
Sensor at x=10, y=20: closest beacon is at x=10, y=16
Sensor at x=14, y=17: closest beacon is at x=10, y=16
Sensor at x=8, y=7: closest beacon is at x=2, y=10
Sensor at x=2, y=0: closest beacon is at x=2, y=10
Sensor at x=0, y=11: closest beacon is at x=2, y=10
Sensor at x=20, y=14: closest beacon is at x=25, y=17
Sensor at x=17, y=20: closest beacon is at x=21, y=22
Sensor at x=16, y=7: closest beacon is at x=15, y=3
Sensor at x=14, y=3: closest beacon is at x=15, y=3
Sensor at x=20, y=1: closest beacon is at x=15, y=3
)"sv.substr(1),
                 26,
                 56000011)

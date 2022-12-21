#include <algorithm>

#include "days/day14.hpp"
#include "owning_span.hpp"
#include "point2d.hpp"

PARSE_IMPL(Day14, view) {
  owning_span<point2d, day14::MAX_POINTS> points;

  // drop point location is set as the bounds
  i32 xmin{500}, xmax{500}, ymin{0}, ymax{0};

  u32 off{0};
  while (off < std::size(view)) {
    i32 x{view[off++] - '0'};
    while (view[off] != ',') {
      x = x * 10 + (view[off++] - '0');
    }
    xmin = std::min(xmin, x);
    xmax = std::max(xmax, x);
    ++off;
    i32 y{view[off++] - '0'};
    while (view[off] != ' ' and view[off] != '\n') {
      y = y * 10 + (view[off++] - '0');
    }
    ymin = std::min(ymin, y);
    ymax = std::max(ymax, y);
    points.push({x, y});
    if (view[off] == ' ') {
      off += 4;
    } else {
      points.push({0, 0});
      ++off;
    }
  }

  day14::cave_type cave{xmin - 1, xmax + 1, ymin, ymax + 2};
  std::fill(std::begin(cave), std::end(cave), '.');

  point2d prev{points[0]};
  for (u32 idx{1}; idx < std::size(points); ++idx) {
    point2d const dest{points[idx]};
    if (prev.x == dest.x or prev.y == dest.y) {
      for (point2d const delta{(dest - prev).sgn()}; prev != dest; prev += delta) {
        cave(prev) = '#';
      }
      cave(dest) = '#';
    }
    prev = dest;
  }
  return cave;
}

using path_t = owning_span<point2d, day14::MAX_HEIGHT>;

constexpr inline bool
drop(day14::cave_type &cave, path_t &path) noexcept {
  if (path.size() == 0) {
    return false;
  }
  while (true) {
    auto const [x, y] = path.top();
    if (y >= cave.ymax() - 1) {
      return false;
    } else if (cave(x, y + 1) == '.') {
      path.push({x, y + 1});
    } else if (cave(x - 1, y + 1) == '.') {
      path.push({x - 1, y + 1});
    } else if (cave(x + 1, y + 1) == '.') {
      path.push({x + 1, y + 1});
    } else {
      cave(x, y) = 'o';
      path.pop();
      return true;
    }
  }
}

PART1_IMPL(Day14, data) {

  auto cave{data};

  path_t path;
  path.push({500, 0});

  u32 placed{0};
  while (drop(cave, path)) {
    ++placed;
  }

  return placed;
}

PART2_IMPL(Day14, data, part1_answer) {
  u32 placed{1};

  auto cave{data};
  cave(500, 0) = 'o';

  auto const xmin = cave.xmin();
  auto const xmax = cave.xmax();
  auto const ymax = cave.ymax();

  for (i32 y{1}; y < ymax; ++y) {
    // left edge
    if (cave(xmin, y) == '.' and (cave(xmin, y - 1) == 'o' or cave(xmin + 1, y - 1) == 'o')) {
      ++placed;
      cave(xmin, y) = 'o';
    }
    // middle
    for (i32 x{xmin + 1}; x < xmax; ++x) {
      if (cave(x, y) == '.' and (cave(x - 1, y - 1) == 'o' or cave(x, y - 1) == 'o' or cave(x + 1, y - 1) == 'o')) {
        ++placed;
        cave(x, y) = 'o';
      }
    }
    // right edge
    if (cave(xmax, y) == '.' and (cave(xmax - 1, y - 1) == 'o' or cave(xmax, y - 1) == 'o')) {
      ++placed;
      cave(xmax, y) = 'o';
    }
  }

  auto const left = ymax - (501 - xmin);
  auto const right = ymax - (xmax - 499);
  auto const left_tri = (left * (left + 1) / 2);
  auto const right_tri = (right * (right + 1) / 2);

  return placed + as<u32>(left_tri + right_tri);
}

INSTANTIATE_TEST(Day14,
                 R"(
498,4 -> 498,6 -> 496,6
503,4 -> 502,4 -> 502,9 -> 494,9
)"sv.substr(1),
                 24,
                 93)

#include <doctest/doctest.h>

#include "days/day09.hpp"
#include "parsing.hpp"
#include "point2d.hpp"

constexpr usize const tracked_p1 = 1;
constexpr usize const tracked_p2 = 9;
constexpr usize const max_tracked = std::max(tracked_p1, tracked_p2);

PARSE_IMPL(Day09, view) {

  // keep track of a bounding box
  i32 x_min{0}, x_max{0}, y_min{0}, y_max{0};
  point2d location = point2d::origin();

  // build the command list
  std::vector<std::pair<point2d, i32>> steps;
  usize off{0};
  while (off < std::size(view)) {
    char d;
    i32 distance;
    off += parse<"\0 \1\n">(view.substr(off), d, distance);
    // char -> point
    auto direction = [](char dir) {
      switch (dir) {
      case 'U':
        return point2d::up();
      case 'D':
        return point2d::down();
      case 'L':
        return point2d::left();
      case 'R':
        return point2d::right();
      default:
        __builtin_unreachable();
      }
    }(d);
    steps.push_back({direction, distance});
    location += direction * distance;
    x_min = std::min(x_min, location.x);
    x_max = std::max(x_max, location.x);
    y_min = std::min(y_min, location.y);
    y_max = std::max(y_max, location.y);
  }

  // a visited grid is faster than std::unordered_set
  offset_grid<u16> grid(x_min, x_max, y_min, y_max);

  // visit the origin
  grid(0, 0) = (1 << tracked_p1) | (1 << tracked_p2);

  // make the chain of size 10
  std::array<point2d, 1 + max_tracked> chain;

  for (auto &&[dir, distance] : steps) {
    for (i32 move{0}; move < distance; ++move) {
      chain[0] += dir;
      // always simulate chain of size 10
      for (usize k{1}; k <= max_tracked; ++k) {
        point2d &curr = chain[k];
        point2d const delta{chain[k - 1] - curr};
        // prev has moved too far and need to adjust
        if (std::abs(delta.y) > 1 or std::abs(delta.x) > 1) {
          curr += delta.sgn();
          // mark the location as visited only if interesting
          if (k == tracked_p1 or k == tracked_p2) {
            grid(curr) |= (1 << k);
          }
          continue;
        }
        break;
      }
    }
  }
  
  return grid;
}

SOLVE_IMPL(Day09, Part2, grid, part1_answer) {
  constexpr u16 const mask{1 << (Part2 ? tracked_p2 : tracked_p1)};
  return std::count_if(std::begin(grid), std::end(grid), [](u16 val) {
    return val & mask;
  });
}

INSTANTIATE(Day09);

INSTANTIATE_TEST(Day09,
                 R"(
R 4
U 4
L 3
D 1
R 4
D 1
L 5
R 2
)"sv.substr(1),
                 13u,
                 1u)

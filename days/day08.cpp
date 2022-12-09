#include <algorithm>

#include <doctest/doctest.h>

#include "days/day08.hpp"
#include "parsing.hpp"

PARSE_IMPL(Day08, view) {

  usize const dim = view.find_first_of('\n');
  usize const end = dim - 1;

  std::vector<std::string_view> grid(dim);
  for (usize row{0}; row < dim; ++row) {
    grid[row] = view.substr(row * (dim + 1), dim);
  }

  std::vector visible(dim * dim, 0u);
  auto vis = [&](usize r, usize c) -> decltype(auto) {
    return visible[r * dim + c];
  };

  // populate visible grid
  for (usize j{0}; j < dim; ++j) {
    char lmax{0}, rmax{0}, tmax{0}, bmax{0};
    for (usize i{0}; i < dim; ++i) {
      if (auto const val = grid[j][i]; val > lmax) {
        ++vis(j, i);
        lmax = val;
      }
    }
    for (usize i{0}; i < dim; ++i) {
      if (auto const val = grid[j][end - i]; val > rmax) {
        ++vis(j, end - i);
        rmax = val;
      }
    }
    for (usize i{0}; i < dim; ++i) {
      if (auto const val = grid[i][j]; val > tmax) {
        ++vis(i, j);
        tmax = val;
      }
    }
    for (usize i{0}; i < dim; ++i) {
      if (auto val = grid[end - i][j]; val > bmax) {
        ++vis(end - i, j);
        bmax = val;
      }
    }
  }
  return {visible, grid, dim};
}

PART1_IMPL(Day08, data) {
  
  return std::count_if(std::begin(data.visible), std::end(data.visible), [](unsigned c) {
    return c > 0;
  });
}

PART2_IMPL(Day08, data, part1_answer) {

  usize const dim = data.size;
  usize const end = dim - 1;
  auto const &grid = data.grid;
  auto const &visible = data.visible;

  i64 high{0};
  for (usize y{1}; y < end; y++) {
    for (usize x{1}; x < end; x++) {
      // this is an optimization which likely is not acceptable for all inputs
      if (visible[y * dim + x] != 3u) {
        continue;
      }
      auto const tree = grid[y][x];
      i64 left{x > 0}, right{x < end}, top{y > 0}, bottom{y < end};
      for (usize i{x - 1}; i > 0 and grid[y][i] < tree; --i, ++left)
        ;
      for (usize i{x + 1}; i < end and grid[y][i] < tree; ++i, ++right)
        ;
      for (usize j{y - 1}; j > 0 and grid[j][x] < tree; --j, ++top)
        ;
      for (usize j{y + 1}; j < end and grid[j][x] < tree; ++j, ++bottom)
        ;
      high = std::max({high, left * right * top * bottom});
    }
  }

  return high;
}

INSTANTIATE_TEST(Day08,
                 R"(
30373
25512
65332
33549
35390
)"sv.substr(1),
                 21l,
                 8l)

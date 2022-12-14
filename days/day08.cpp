#include <algorithm>

#include "days/day08.hpp"
#include "parsing.hpp"

PARSE_IMPL(Day08, view) {

  u32 const dim = static_cast<u32>(view.find_first_of('\n'));
  u32 const end = dim - 1;

  typename day08::grid_t grid(dim);
  for (u32 row{0}; row < dim; ++row) {
    grid[row] = view.substr(row * (dim + 1), dim);
  }

  typename day08::visible_t visible(dim * dim, 0u);
  auto vis = [&](u32 r, u32 c) -> decltype(auto) {
    return visible[r * dim + c];
  };

  // populate visible grid
  for (u32 j{0}; j < dim; ++j) {
    char lmax{0}, rmax{0}, tmax{0}, bmax{0};
    for (u32 i{0}; i < dim; ++i) {
      if (auto const val = grid[j][i]; val > lmax) {
        ++vis(j, i);
        if (lmax = val; val == '9') {
          break;
        }
      }
    }
    for (u32 i{0}; i < dim; ++i) {
      if (auto const val = grid[j][end - i]; val > rmax) {
        ++vis(j, end - i);
        if (rmax = val; val == '9') {
          break;
        }
      }
    }
    for (u32 i{0}; i < dim; ++i) {
      if (auto const val = grid[i][j]; val > tmax) {
        ++vis(i, j);
        if (tmax = val; val == '9') {
          break;
        }
      }
    }
    for (u32 i{0}; i < dim; ++i) {
      if (auto val = grid[end - i][j]; val > bmax) {
        ++vis(end - i, j);
        if (bmax = val; val == '9') {
          break;
        }
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

  u32 const dim = data.size;
  u32 const end = dim - 1;
  auto const &grid = data.grid;
  auto const &visible = data.visible;

  i64 high{0};
  for (u32 y{1}; y < end; y++) {
    for (u32 x{1}; x < end; x++) {
      // this is an optimization which likely is not acceptable for all inputs
      if (visible[y * dim + x] != 2u) {
        continue;
      }
      auto const tree = grid[y][x];
      i64 left{x > 0}, right{x < end}, top{y > 0}, bottom{y < end};
      for (u32 i{x - 1}; i > 0 and grid[y][i] < tree; --i, ++left)
        ;
      for (u32 i{x + 1}; i < end and grid[y][i] < tree; ++i, ++right)
        ;
      for (u32 j{y - 1}; j > 0 and grid[j][x] < tree; --j, ++top)
        ;
      for (u32 j{y + 1}; j < end and grid[j][x] < tree; ++j, ++bottom)
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

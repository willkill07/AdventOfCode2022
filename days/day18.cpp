#include "days/day18.hpp"
#include "owning_span.hpp"
#include "parsing.hpp"

namespace {

constexpr inline u32
index(i32 x, i32 y, i32 z) noexcept {
  using day18::MAX_DIM;
  return MAX_DIM * MAX_DIM * static_cast<u32>(z) + MAX_DIM * static_cast<u32>(y) + static_cast<u32>(x);
}

constexpr inline u32
pack(i32 x, i32 y, i32 z) noexcept {
  return static_cast<u32>((x << 16) | (y << 8) | z);
}

constexpr inline auto
unpack(u32 v) noexcept {
  return std::tuple(static_cast<i32>((v >> 16) & 0xFF), static_cast<i32>((v >> 8) & 0xFF), static_cast<i32>(v & 0xFF));
}

constexpr auto const DELTAS = std::array{std::array{-1, 0, 0},
                                         std::array{1, 0, 0},
                                         std::array{0, -1, 0},
                                         std::array{0, 1, 0},
                                         std::array{0, 0, -1},
                                         std::array{0, 0, 1}};

} // namespace

PARSE_IMPL(Day18, view) {
  using day18::MAX_DIM;
  day18::grid_type grid(MAX_DIM * MAX_DIM * MAX_DIM, 0u);
  for (u32 off{0}; off < std::size(view);) {
    i32 x, y, z;
    off += parse<"\0,\1,\2\n">(view.substr(off), x, y, z);
    // offset placement by 1 in each dimension
    grid[index(x + 1, y + 1, z + 1)] = 1u;
  }
  return grid;
}

PART1_IMPL(Day18, grid) {
  using day18::MAX_DIM;
  constexpr i32 const lower{1};
  constexpr i32 const limit{static_cast<i32>(MAX_DIM) - 1};

  u32 voxels{0}, shared{0};

  for (i32 z{lower}; z < limit; ++z) {
    for (i32 y{lower}; y < limit; ++y) {
      for (i32 x{lower}; x < limit; ++x) {
        if (grid[index(x, y, z)]) {
          ++voxels;
          for (auto [dx, dy, dz] : DELTAS) {
            if (grid[index(x + dx, y + dy, z + dz)]) {
              ++shared;
            }
          }
        }
      }
    }
  }
  return (6 * voxels) - shared;
}

PART2_IMPL(Day18, grid, part1_answer) {
  using day18::MAX_DIM;
  owning_span<bool, MAX_DIM * MAX_DIM * MAX_DIM> seen(MAX_DIM * MAX_DIM * MAX_DIM, false);
  owning_span<u32, MAX_DIM * MAX_DIM * MAX_DIM> frontier;
  auto front = std::cbegin(frontier);
  auto back = std::begin(frontier);
  constexpr i32 const limit{static_cast<i32>(MAX_DIM) - 1};

  *back++ = pack(0, 0, 0);
  seen[index(0, 0, 0)] = true;

  u32 face{0};

  while (front != back) {
    auto const [x, y, z] = unpack(*front++);
    for (auto [dx, dy, dz] : DELTAS) {
      bool const xbounds{dx == 0 or (dx < 0 and x > 0) or (dx > 0 and x < limit)};
      bool const ybounds{dy == 0 or (dy < 0 and y > 0) or (dy > 0 and y < limit)};
      bool const zbounds{dz == 0 or (dz < 0 and z > 0) or (dz > 0 and z < limit)};
      u32 const idx{index(x + dx, y + dy, z + dz)};
      if (xbounds and ybounds and zbounds and not seen[idx]) {
        if (not grid[idx]) {
          // neighbor is an air voxel -- visit
          *back++ = pack(x + dx, y + dy, z + dz);
          seen[idx] = true;
        } else {
          // neighbor is lava! increment the face count in that direction
          ++face;
        }
      }
    }
  }
  return face;
}

INSTANTIATE_TEST(Day18,
                 R"(
2,2,2
1,2,2
3,2,2
2,1,2
2,3,2
2,2,1
2,2,3
2,2,4
2,2,6
1,2,5
3,2,5
2,1,5
2,3,5
)"sv.substr(1),
                 64,
                 58)

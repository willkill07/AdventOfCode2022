#include "days/day23.hpp"
#include "owning_span.hpp"
#include "parsing.hpp"

constexpr u8 const Elf{0b1000'0000};
constexpr u8 const Stay{0b0001'0000};
constexpr u8 const North{0b0000'0001};
constexpr u8 const South{0b0000'0010};
constexpr u8 const West{0b0000'0100};
constexpr u8 const East{0b0000'1000};

constexpr i32 MaxDim{128};

constexpr std::array const constants{North, South, West, East};

constexpr std::array const checks{
    std::array{std::pair{-1, -1}, std::pair{0, -1}, std::pair{1, -1}}, // north
    std::array{std::pair{-1, 1}, std::pair{0, 1}, std::pair{1, 1}},    // south
    std::array{std::pair{-1, -1}, std::pair{-1, 0}, std::pair{-1, 1}}, // west
    std::array{std::pair{1, -1}, std::pair{1, 0}, std::pair{1, 1}},    // east
};
constexpr std::array const all_neighbors{std::pair{-1, -1},
                                         std::pair{0, -1},
                                         std::pair{1, -1},
                                         std::pair{-1, 0},
                                         std::pair{1, 0},
                                         std::pair{-1, 1},
                                         std::pair{0, 1},
                                         std::pair{1, 1}};

[[nodiscard]] constexpr inline bool
all_empty(auto const &grid, auto const &deltas, i32 x, i32 y) {
  for (auto [dx, dy] : deltas) {
    if (grid[as<u32>((y + dy) * MaxDim + x + dx)] & Elf) {
      return false;
    }
  }
  return true;
}

constexpr inline void
mark_candidates(day23::grid_type &grid, i32 x, i32 y, i32 step) noexcept {
  // no elf, nothing to do
  if (not(grid[as<u32>(y * MaxDim + x)] & Elf)) {
    return;
  }
  if (all_empty(grid, all_neighbors, x, y)) {
    grid[as<u32>(y * MaxDim + x)] |= Stay;
    return;
  }
  // for each direction in the "correct" order
  for (i32 i{0}; i < 4; ++i) {
    u32 const check_index{as<u32>(i + step) & 0x3u};
    if (all_empty(grid, checks[check_index], x, y)) {
      auto const [dx, dy] = checks[check_index][1];
      grid[as<u32>(y + dy) * MaxDim + as<u32>(x + dx)] |= constants[check_index];
      return;
    }
  }
  grid[as<u32>(y * MaxDim + x)] |= Stay;
}

constexpr inline bool
move_candidates(day23::grid_type &grid, i32 x, i32 y) noexcept {
  if (auto &val = grid[as<u32>(y * MaxDim + x)]; val == 0) {
    return false;
  } else if (std::has_single_bit(val)) {
    bool const moved{val != Stay};
    val = Elf;
    return moved;
  } else {
    // multiple elves requested to move here. put them back
    for (u32 i{0}; i < 4; ++i) {
      if (val & constants[i]) {
        // if we had an elf coming from that direction, push the elf back to where it was
        auto const [dx, dy] = checks[i][1];
        grid[as<u32>(y - dy) * MaxDim + as<u32>(x - dx)] = Elf;
      }
    }
    // then clear the cell
    grid[as<u32>(y * MaxDim + x)] = 0;
    return false;
  }
}

constexpr inline auto
update_bounds(day23::grid_type &grid, i32 xmin, i32 xmax, i32 ymin, i32 ymax) noexcept {
  i32 new_xmin{xmin}, new_xmax{xmax}, new_ymin{ymin}, new_ymax{ymax};
  for (i32 x{xmin}; x <= xmax; ++x) {
    if (grid[as<u32>((ymin - 1) * MaxDim + x)]) {
      new_ymin = ymin - 1;
      break;
    }
  }
  for (i32 x{xmin}; x <= xmax; ++x) {
    if (grid[as<u32>((ymax + 1) * MaxDim + x)]) {
      new_ymax = ymax + 1;
      break;
    }
  }
  for (i32 y{ymin}; y <= ymax; ++y) {
    if (grid[as<u32>(y * MaxDim + xmin - 1)]) {
      new_xmin = xmin - 1;
      break;
    }
  }
  for (i32 y{ymin}; y <= ymax; ++y) {
    if (grid[as<u32>(y * MaxDim + xmax + 1)]) {
      new_xmax = xmax + 1;
      break;
    }
  }
  return std::tuple(new_xmin, new_xmax, new_ymin, new_ymax);
}

constexpr inline auto
update(day23::grid_type &grid, i32 xmin, i32 xmax, i32 ymin, i32 ymax, i32 step) noexcept {
  for (i32 y{ymin}; y <= ymax; ++y) {
    for (i32 x{xmin}; x <= xmax; ++x) {
      mark_candidates(grid, x, y, step);
    }
  }

  for (i32 y{ymin}; y <= ymax; ++y) {
    for (i32 x{xmin}; x <= xmax; ++x) {
      grid[as<u32>(y * MaxDim + x)] &= ~Elf;
    }
  }

  bool moved_any{false};
  for (i32 y{ymin - 1}; y <= ymax + 1; ++y) {
    for (i32 x{xmin - 1}; x <= xmax + 1; ++x) {
      if (move_candidates(grid, x, y)) {
        moved_any = true;
      }
    }
  }
  return std::tuple_cat(std::tuple(moved_any), update_bounds(grid, xmin, xmax, ymin, ymax));
}

PARSE_IMPL(Day23, view) {
  day23::grid_type grid(MaxDim * MaxDim, 0u);
  i32 const width{static_cast<i32>(view.find_first_of('\n'))};
  i32 const height{static_cast<i32>(std::size(view)) / (width + 1)};
  i32 const xmin{(MaxDim - width) / 2};
  i32 const xmax{xmin + width - 1};
  i32 const ymin{(MaxDim - height) / 2};
  i32 const ymax{ymin + height - 1};

  i32 num_elves{0};
  for (i32 y{0}; y < height; ++y) {
    for (i32 x{0}; x < width; ++x) {
      if (view[as<u32>(y * (width + 1) + x)] == '#') {
        grid[as<u32>((y + ymin) * MaxDim + xmin + x)] = Elf;
        ++num_elves;
      }
    }
  }

  return {grid, xmin, xmax, ymin, ymax, num_elves};
}

SOLVE_IMPL(Day23, Part2, data, part1_answer) {

  i32 xmin{data.xmin}, xmax{data.xmax}, ymin{data.ymin}, ymax{data.ymax};
  auto grid{data.grid};

  for (i32 step{0}; Part2 or step < 10; ++step) {
    bool moved{false};
    std::tie(moved, xmin, xmax, ymin, ymax) = update(grid, xmin, xmax, ymin, ymax, step);
    if constexpr (Part2) {
      if (not moved) {
        return step + 1;
      }
    }
  }

  return (ymax - ymin + 1) * (xmax - xmin + 1) - data.count;
}

INSTANTIATE(Day23);

INSTANTIATE_TEST(Day23,
                 R"(
....#..
..###.#
#...#.#
.#...##
#.###..
##.#.##
.#..#..
)"sv.substr(1),
                 110,
                 20)

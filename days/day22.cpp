#include <utility>

#include "days/day22.hpp"
#include "owning_span.hpp"
#include "parsing.hpp"

using namespace day22;

constexpr inline u32
gcd(u32 a, u32 b) noexcept {
  while (a != b) {
    if (a > b) {
      a -= b;
    } else {
      b -= a;
    }
  }
  return a;
}

constexpr inline direction
adjust(direction curr, direction shift) noexcept {
  u32 const num{std::to_underlying(direction::num_dirs)};
  u32 const shift_amount{shift == direction::right ? 1u : 3u};
  u32 const index{(std::to_underlying(curr) + shift_amount) % num};
  return direction{index};
}

inline auto
step(owning_span<side, 6> const &sides,
     u32 const side_size,
     u32 const side,
     u32 const x,
     u32 const y,
     direction const dir) noexcept {

  u32 const max_index{side_size - 1};

  // staying within the same side
  if (dir == direction::left and x > 0) {
    bool const res{sides[side].valid(x - 1, y)};
    return std::tuple(res, side, x - res, y, dir);
  } else if (dir == direction::right and x < max_index) {
    bool const res{sides[side].valid(x + 1, y)};
    return std::tuple(res, side, x + res, y, dir);
  } else if (dir == direction::up and y > 0) {
    bool const res{sides[side].valid(x, y - 1)};
    return std::tuple(res, side, x, y - res, dir);
  } else if (dir == direction::down and y < max_index) {
    bool const res{sides[side].valid(x, y + 1)};
    return std::tuple(res, side, x, y + res, dir);
  }

  u32 const side_offset{[](u32 max_idx, direction d, u32 x, u32 y) noexcept {
    switch (d) {
    case direction::right:
      return y;
    case direction::down:
      return max_idx - x;
    case direction::left:
      return max_idx - y;
    case direction::up:
      return x;
    default:
      __builtin_unreachable();
    }
  }(max_index, dir, x, y)};

  direction const new_dir{sides[side].neighbors[std::to_underlying(dir)].dir};

  auto const [new_x, new_y] = [](u32 max_idx, direction d, u32 offset) noexcept {
    switch (d) {
    case direction::right:
      return std::tuple(0u, offset);
    case direction::down:
      return std::tuple(max_idx - offset, 0u);
    case direction::left:
      return std::tuple(0u, max_idx - offset);
    case direction::up:
      return std::tuple(offset, 0u);
    default:
      __builtin_unreachable();
    }
  }(max_index, new_dir, side_offset);

  u32 const new_side{sides[side].neighbors[std::to_underlying(dir)].id};

  if (sides[new_side].valid(new_x, new_y)) {
    return std::tuple(true, new_side, new_x, new_y, new_dir);
  } else {
    return std::tuple(false, side, x, y, dir);
  }
}

#include <fmt/core.h>

PARSE_IMPL(Day22, view) {
  auto const [height, width, side_len] = [&] {
    u32 w{as<u32>(view.find_first_of('\n'))}, h{1}, e{as<u32>(view.find_first_of('\n'))};
    for (u32 off{w + 1}; view[off] != '\n';) {
      u32 const len{as<u32>(view.find_first_of('\n', off)) - off};
      w = std::max(w, len);
      e = gcd(e, len);
      off += len + 1;
      ++h;
    }
    return std::tuple(h, w, e);
  }();

  owning_span<side, 6> sides;
  u32 skip{1}, global_row{0}, len{0};
  u32 off{0};
  for (; view[off] != '\n'; off += len + 1, ++global_row) {
    len = as<u32>(view.find_first_of('\n', off)) - off;
    if (--skip != 0) {
      continue;
    }
    skip = side_len;
    u32 const N{std::size(sides)};

    // walk across columns of interest
    for (u32 global_col{0}; global_col < len; global_col += side_len) {
      if (view[off + global_col] == ' ') {
        continue;
      }
      // if we have a "corner", make a side
      u32 const new_id{std::size(sides)};
      sides.push({view.substr(off + global_col), {}, len + 1, global_row, global_col});

      // establish left <-> right relationships
      if (new_id - N > 0) {
        // when we've inserted at least two faces in the same row
        if (global_col - sides[new_id - 1].global_col == side_len) {
          // and they are exactly side_len away, create the relationships
          sides[new_id].neighbors[std::to_underlying(direction::left)] = {new_id - 1, direction::left};
          sides[new_id - 1].neighbors[std::to_underlying(direction::right)] = {new_id, direction::right};
        }
      }

      // establish top <-> bottom relationships
      for (u32 id{N}; id-- > 0;) {
        // visit each preceding ID
        if (auto const diff{(global_row - sides[id].global_row)}; diff > side_len) {
          // early exit when delta of rows becomes greater than side length
          break;
        } else if (diff == side_len) {
          if (global_col == sides[id].global_col) {
            // when vertically adjacent and columns align, we are connected
            sides[id].neighbors[std::to_underlying(direction::down)] = {new_id, direction::down};
            sides[new_id].neighbors[std::to_underlying(direction::up)] = {id, direction::up};
          }
        }
      }
    }
  }

  ++off;
  owning_span<command, 2'048> cmds;
  u32 dist{as<u32>(view[off++] - '0')};
  while ('0' <= view[off] and view[off] <= '9') {
    dist = dist * 10 + as<u32>(view[off++] - '0');
  }
  cmds.push({direction::right, dist});
  while (off + 1 < std::size(view)) {
    // read single character for direction
    direction const dir{view[off++] == 'R' ? direction::right : direction::left};
    // read number
    dist = as<u32>(view[off++] - '0');
    while ('0' <= view[off] and view[off] <= '9') {
      dist = dist * 10 + as<u32>(view[off++] - '0');
    }
    cmds.push({dir, dist});
  }

  return {sides, cmds, side_len};
}

SOLVE_IMPL(Day22, Part2, data, part1_answer) {
  auto sides{data.sides};
  if constexpr (not Part2) {
    if (data.side_len != 50) {
      //   0
      // 123
      //   45
      sides[0].neighbors[std::to_underlying(direction::right)] = {0, direction::right};
      sides[0].neighbors[std::to_underlying(direction::left)] = {0, direction::left};
      sides[0].neighbors[std::to_underlying(direction::up)] = {4, direction::up};
      sides[1].neighbors[std::to_underlying(direction::down)] = {1, direction::down};
      sides[1].neighbors[std::to_underlying(direction::left)] = {3, direction::left};
      sides[1].neighbors[std::to_underlying(direction::up)] = {1, direction::up};
      sides[2].neighbors[std::to_underlying(direction::down)] = {2, direction::down};
      sides[2].neighbors[std::to_underlying(direction::up)] = {2, direction::up};
      sides[3].neighbors[std::to_underlying(direction::right)] = {1, direction::right};
      sides[4].neighbors[std::to_underlying(direction::left)] = {5, direction::left};
      sides[4].neighbors[std::to_underlying(direction::down)] = {0, direction::down};
      sides[5].neighbors[std::to_underlying(direction::right)] = {4, direction::right};
      sides[5].neighbors[std::to_underlying(direction::up)] = {5, direction::up};
      sides[5].neighbors[std::to_underlying(direction::down)] = {5, direction::down};
    } else {
      //  01
      //  2
      // 34
      // 5
      sides[0].neighbors[std::to_underlying(direction::left)] = {1, direction::left};
      sides[0].neighbors[std::to_underlying(direction::up)] = {4, direction::up};
      sides[1].neighbors[std::to_underlying(direction::right)] = {0, direction::right};
      sides[1].neighbors[std::to_underlying(direction::down)] = {1, direction::down};
      sides[1].neighbors[std::to_underlying(direction::up)] = {1, direction::up};
      sides[2].neighbors[std::to_underlying(direction::right)] = {2, direction::right};
      sides[2].neighbors[std::to_underlying(direction::left)] = {2, direction::left};
      sides[3].neighbors[std::to_underlying(direction::left)] = {4, direction::left};
      sides[3].neighbors[std::to_underlying(direction::up)] = {5, direction::up};
      sides[4].neighbors[std::to_underlying(direction::right)] = {3, direction::right};
      sides[4].neighbors[std::to_underlying(direction::down)] = {0, direction::down};
      sides[5].neighbors[std::to_underlying(direction::right)] = {5, direction::right};
      sides[5].neighbors[std::to_underlying(direction::down)] = {3, direction::down};
      sides[5].neighbors[std::to_underlying(direction::left)] = {5, direction::left};
    }
  } else {
    if (data.side_len != 50) {
      //   0
      // 123
      //   45
      sides[0].neighbors[std::to_underlying(direction::right)] = {5, direction::left};
      sides[0].neighbors[std::to_underlying(direction::up)] = {1, direction::down};
      sides[0].neighbors[std::to_underlying(direction::left)] = {2, direction::down};
      sides[1].neighbors[std::to_underlying(direction::left)] = {5, direction::up};
      sides[1].neighbors[std::to_underlying(direction::up)] = {0, direction::down};
      sides[1].neighbors[std::to_underlying(direction::down)] = {4, direction::up};
      sides[2].neighbors[std::to_underlying(direction::up)] = {0, direction::right};
      sides[2].neighbors[std::to_underlying(direction::down)] = {4, direction::left};
      sides[3].neighbors[std::to_underlying(direction::right)] = {5, direction::down};
      sides[4].neighbors[std::to_underlying(direction::down)] = {1, direction::right};
      sides[4].neighbors[std::to_underlying(direction::left)] = {1, direction::up};
      sides[5].neighbors[std::to_underlying(direction::right)] = {0, direction::left};
      sides[5].neighbors[std::to_underlying(direction::down)] = {1, direction::right};
      sides[5].neighbors[std::to_underlying(direction::up)] = {3, direction::left};
    } else {
      //  01
      //  2
      // 34
      // 5
      sides[0].neighbors[std::to_underlying(direction::left)] = {3, direction::right};
      sides[0].neighbors[std::to_underlying(direction::up)] = {5, direction::right};
      sides[1].neighbors[std::to_underlying(direction::right)] = {4, direction::left};
      sides[1].neighbors[std::to_underlying(direction::down)] = {2, direction::left};
      sides[1].neighbors[std::to_underlying(direction::up)] = {5, direction::up};
      sides[2].neighbors[std::to_underlying(direction::left)] = {3, direction::down};
      sides[2].neighbors[std::to_underlying(direction::right)] = {1, direction::up};
      sides[3].neighbors[std::to_underlying(direction::left)] = {0, direction::right};
      sides[3].neighbors[std::to_underlying(direction::up)] = {2, direction::right};
      sides[4].neighbors[std::to_underlying(direction::right)] = {1, direction::left};
      sides[4].neighbors[std::to_underlying(direction::down)] = {5, direction::left};
      sides[5].neighbors[std::to_underlying(direction::left)] = {0, direction::down};
      sides[5].neighbors[std::to_underlying(direction::right)] = {4, direction::up};
      sides[5].neighbors[std::to_underlying(direction::down)] = {1, direction::down};
    }
  }

  u32 const size{data.side_len};

  direction dir{direction::up};
  u32 side{0}, x{0}, y{0};

  for (auto const [rotate, amount] : data.commands) {
    dir = adjust(dir, rotate);

    for (u32 i{0}; i < amount; ++i) {
      bool moved{false};
      std::tie(moved, side, x, y, dir) = step(sides, size, side, x, y, dir);
      if (not moved) {
        break;
      }
    }
  }

  return sides[side].get_uid(x, y, dir);
}

INSTANTIATE(Day22);

INSTANTIATE_TEST(Day22,
                 R"(
        ...#
        .#..
        #...
        ....
...#.......#
........#...
..#....#....
..........#.
        ...#....
        .....#..
        .#......
        ......#.

10R5L5R10L4R5L5
)"sv.substr(1),
                 6032,
                 5031)

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

inline std::tuple<u32, u32, u32, direction>
step(std::array<side, 6> &sides, u32 const side_size, u32 side, u32 x, u32 y, direction dir) noexcept {

  // staying within the same side
  if (dir == direction::left and x > 0) {
    return {side, x - 1, y, dir};
  } else if (dir == direction::right and x + 1 < side_size) {
    return {side, x + 1, y, dir};
  } else if (dir == direction::up and y > 0) {
    return {side, x, y - 1, dir};
  } else if (dir == direction::down and y + 1 < side_size) {
    return {side, x, y + 1, dir};
  }

  u32 const new_side{sides[side].neighbors[std::to_underlying(dir)].id};

  // if we have no side, return same location
  if (new_side == 0xFFFFFFFF) {
    return {side, x, y, dir};
  }

  u32 const side_offset{(dir == direction::left or dir == direction::right) ? y : x};

  direction const incoming{sides[side].neighbors[std::to_underlying(dir)].dir};

  direction const new_dir{[] (direction d) {
    switch (d) {
      case direction::right: return direction::left;
      case direction::down: return direction::up;
      case direction::left: return direction::right;
      case direction::up: return direction::down;
      default: __builtin_unreachable();
    }
  }(incoming)};

  switch (dir) {
    case direction::right:
      switch (new_dir) {
        case direction::right: return {new_side, 0, side_offset, new_dir};
        case direction::down: return {new_side, side_offset, 0, new_dir};
        case direction::left: return {new_side, 0, (side_size - 1) - side_offset, new_dir};
        case direction::up: return {new_side, (side_size - 1) - side_offset, 0, new_dir};
        default: __builtin_unreachable();
      }
    case direction::down:
      switch (new_dir) {
        case direction::right: return {new_side, 0, (side_size - 1) - side_offset, new_dir};
        case direction::down: return {new_side, side_offset, 0, new_dir};
        case direction::left: return {new_side, 0, side_offset, new_dir};
        case direction::up: return {new_side, (side_size - 1) - side_offset, 0, new_dir};
        default: __builtin_unreachable();
      }
    case direction::left:
      switch (new_dir) {
        case direction::right: return {new_side, 0, (side_size - 1) - side_offset, new_dir};
        case direction::down: return {new_side, (side_size - 1) - side_offset, 0, new_dir};
        case direction::left: return {new_side, 0, side_offset, new_dir};
        case direction::up: return {new_side, side_offset, 0, new_dir};
        default: __builtin_unreachable();
      }
    case direction::up:
      switch (new_dir) {
        case direction::right: return {new_side, 0, side_offset, new_dir};
        case direction::down: return {new_side, (side_size - 1) - side_offset, 0, new_dir};
        case direction::left: return {new_side, 0, (side_size - 1) - side_offset, new_dir};
        case direction::up: return {new_side, side_offset, 0, new_dir};
        default: __builtin_unreachable();
      }
    default: __builtin_unreachable();
  }
  
}

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
          sides[new_id].neighbors[std::to_underlying(direction::left)] = {new_id - 1, direction::right};
          sides[new_id - 1].neighbors[std::to_underlying(direction::right)] = {new_id, direction::left};
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
            sides[id].neighbors[std::to_underlying(direction::down)] = {new_id, direction::up};
            sides[new_id].neighbors[std::to_underlying(direction::up)] = {id, direction::down};
          }
        }
      }
    }
  }

  ++off;
  owning_span<command, 2'000> cmds;
  while (view[off] != '\n') {
    // TODO: parse command by reading a single character, then constructing a number
    // finally, push into owning span
  }

  return {sides, cmds};
}

PART1_IMPL(Day22, data) {
  return {};
}

PART2_IMPL(Day22, data, part1_answer) {
  return {};
}

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
                 24000,
                 45000)

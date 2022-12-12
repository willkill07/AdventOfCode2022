#include <doctest/doctest.h>

#include "days/day12.hpp"

PARSE_IMPL(Day12, view) {
  u32 const width = static_cast<u32>(view.find_first_of('\n') + 1);
  u32 const height = static_cast<u32>(view.size() / width);
  u32 const start = static_cast<u32>(view.find_first_of('S'));
  u32 const stop = static_cast<u32>(view.find_first_of('E', start));
  std::vector<char> grid(std::begin(view), std::end(view));
  grid[start] = 'a';
  grid[stop] = 'z';
  return {std::move(grid), start, stop, width, height};
}

namespace {

struct pair {
  u32 distance{0};
  u32 index{0};
};

u32 part2_answer{-1u};

[[gnu::always_inline]] inline void
enqueue(auto &queued, auto &grid, auto &back, u32 curr, u32 next, u32 dist) noexcept {
  // only enqueue if not seen and we can move there
  if (not queued[next] and (grid[curr] - grid[next]) <= 1) {
    // mark as queued/seen
    queued[next] = 1;
    // push back
    *back++ = {dist, next};
  }
}

} // namespace

SOLVE_IMPL(Day12, Part2, data, part1_answer) {

  if constexpr (Part2) {
    return part2_answer;
  }

  auto const &grid{data.grid};
  u32 const stop{data.stop};
  u32 const start{data.start};
  u32 const width{data.width};

  // single memory allocation for queued + frontier
  std::vector<u8> queued(std::size(grid), 0u);
  std::vector<pair> frontier(std::size(grid));

  // model a queue as two pointers
  pair const *front = std::data(frontier);
  pair *back = std::data(frontier);

  // start from the end -- work back toward
  *back++ = {0u, stop};

  while (front != back) {
    // pop front
    auto [dist, curr] = *front++;
    if (grid[curr] == 'a') {
      part2_answer = std::min(part2_answer, dist);
      if (curr == start) {
        return dist;
      }
    }
    // walk one step
    ++dist;

    // left -- check detects overflow for first row or looks for newline
    if (u32 const next{curr - 1}; next < curr and grid[next] != '\n') {
      enqueue(queued, grid, back, curr, next, dist);
    }
    // right -- check looks for newline to right
    if (u32 const next{curr + 1}; grid[next] != '\n') {
      enqueue(queued, grid, back, curr, next, dist);
    }
    // up -- check detects overflow
    if (u32 const next{curr - width}; next < curr) {
      enqueue(queued, grid, back, curr, next, dist);
    }
    // down -- checks boundary/size
    if (u32 const next{curr + width}; next < std::size(grid)) {
      enqueue(queued, grid, back, curr, next, dist);
    }
  }

  return -1u;
}

INSTANTIATE(Day12);

INSTANTIATE_TEST(Day12,
                 R"(
Sabqponm
abcryxxl
accszExk
acctuvwj
abdefghi
)"sv.substr(1),
                 31u,
                 29u)

#include <algorithm>
#include <numeric>
#include <vector>

#include <cstdio>

#include "Day05.hpp"
#include "Parsing.hpp"

PARSE_IMPL(Day05, buffer) {
  std::string_view view{buffer.data(), buffer.size()};
  sz const num_stacks{(view.find_first_of('\n') + 1) / 4};
  std::vector<std::vector<char>> stacks{num_stacks};

  sz off{0};
  while (not('0' <= view[off + 1] and view[off + 1] <= '9')) {
    for (sz stack{0}; stack < num_stacks; ++stack) {
      char const c{view[off + 4 * stack + 1]};
      if (c != ' ') {
        stacks[stack].push_back(c);
      }
    }
    off += num_stacks * 4;
  }
  for (auto &s : stacks) {
    std::reverse(std::begin(s), std::end(s));
  }
  off += num_stacks * 4 + 1;

  std::vector<day05::command> cmds;
  while (off < std::size(view)) {
    u32 count{0}, src{0}, dst{0}, len;
    // TODO: fix error with read<> and replace
    sscanf(view.substr(off).data(), "move %u from %u to %u\n%n", &count, &src, &dst, &len);
    off += static_cast<u32>(len);
    cmds.push_back({count, src, dst});
  }
  return {stacks, cmds};
}

SOLVE_IMPL(Day05, Part2, initial_state, part1_answer) {
  auto stacks = initial_state.stacks;
  for (auto const &[count, src, dst] : initial_state.commands) {
    auto &the_src = stacks[src - 1];
    auto &the_dst = stacks[dst - 1];
    if constexpr (Part2) {
      std::copy_n(std::end(the_src) - count, count, std::back_inserter(the_dst));
    } else {
      std::copy_n(std::rbegin(the_src), count, std::back_inserter(the_dst));
    }
    the_src.resize(std::size(the_src) - count);
  }
  return std::accumulate(std::begin(stacks), std::end(stacks), std::string{}, [](std::string res, auto const &stack) {
    return res + stack.back();
  });
}

INSTANTIATE(Day05);

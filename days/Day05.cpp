#include <algorithm>
#include <vector>

#include "Day05.hpp"
#include "Parsing.hpp"

PARSE_IMPL(Day05, view) {
  // determine number of stacks by looking at line length
  sz const num_stacks{(view.find_first_of('\n') + 1) / 4};
  // create stacks
  std::vector<day05::stack_t> stacks{num_stacks};
  sz off{0};
  // exit condition for parsing; line is ' 1   2   3  ...'
  while (not('0' <= view[off + 1] and view[off + 1] <= '9')) {
    // line is of form:   '    [A]     [B] ...'
    // offsets of values:   1   5   9   13 ...   (4s + 1)
    for (sz stack{0}; stack < num_stacks; ++stack) {
      if (char const c{view[off + 4 * stack + 1]}; c != ' ') {
        stacks[stack].push_back(c);
      }
    }
    // advance line
    off += num_stacks * 4;
  }
  // reverse all stacks since we parsed top->bottom
  for (auto &s : stacks) {
    std::reverse(std::begin(s), std::end(s));
  }
  // advance line + empty newline
  off += num_stacks * 4 + 1;
  // parse command section
  std::vector<day05::command> cmds;
  while (off < std::size(view)) {
    u32 count{0}, src{0}, dst{0};
    off += read<"move \0 from \1 to \2\n">(view.substr(off), count, src, dst);
    cmds.push_back({count, src, dst});
  }
  return {stacks, cmds};
}

SOLVE_IMPL(Day05, Part2, initial_state, part1_answer) {
  // need to make a copy from read-only input
  auto stacks = initial_state.stacks;
  // walk through all commands
  for (auto const& command : initial_state.commands) {
    command.execute<Part2>(stacks);
  }
  // accumulate into string
  std::string result(std::size(stacks), '\0');
  std::transform(std::begin(stacks), std::end(stacks), std::begin(result), [](auto const &stack) {
    return stack.back();
  });
  return result;
}

INSTANTIATE(Day05);

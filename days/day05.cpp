#include "days/day05.hpp"
#include "owning_span.hpp"
#include "parsing.hpp"

namespace day05 {
template <bool Bulk>
[[gnu::always_inline]] inline void
command::execute(stacks_t &stacks) const noexcept {
  stack_t &src = stacks[from];
  stack_t &dst = stacks[to];
  if constexpr (auto elems = src.top(count); Bulk) {
    dst.push(std::begin(elems), count);
  } else {
    dst.push(std::rbegin(elems), count);
  }
  src.pop(count);
}

} // namespace day05

PARSE_IMPL(Day05, view) {

  day05::state state;

  // determine number of stacks by looking at line length
  u32 const num_stacks{static_cast<u32>(view.find_first_of('\n') + 1) / 4};

  // create stacks
  state.stacks.resize(num_stacks);

  usize off{0};
  // exit condition for parsing; line is ' 1   2   3  ...'
  while (not('0' <= view[off + 1] and view[off + 1] <= '9')) {
    // line is of form:   '    [A]     [B] ...'
    // offsets of values:   1   5   9   13 ...   (4s + 1)
    for (u32 idx{0}; auto &s : state.stacks) {
      if (char const c{view[off + (idx << 2) + 1]}; c != ' ') {
        s.push(c);
      }
      ++idx;
    }
    // advance line
    off += num_stacks << 2;
  }

  // reverse all stacks since we parsed top->bottom
  for (auto &s : state.stacks) {
    std::reverse(std::begin(s), std::end(s));
  }

  // advance line + empty newline
  off += num_stacks * 4 + 1;

  // parse command section
  while (off < std::size(view)) {
    u32 count{0}, src{0}, dst{0};
    off += parse<"move \0 from \1 to \2\n">(view.substr(off), count, src, dst);
    state.commands.push({count, src - 1, dst - 1});
  }
  return state;
}

SOLVE_IMPL(Day05, Part2, state, part1_answer) {

  day05::commands_t const &commands{state.commands};
  day05::stacks_t stacks{state.stacks};

  // simulate
  for (auto &&command : commands) {
    command.execute<Part2>(stacks);
  }

  // accumulate into buffer
  owning_span<char, day05::MAX_STACKS> result;
  result.resize(0);
  for (auto const &s : stacks) {
    result.push(s.top());
  }

  return std::string{std::begin(result), std::end(result)};
}

INSTANTIATE(Day05);

INSTANTIATE_TEST(Day05,
                 R"(
    [D]    
[N] [C]    
[Z] [M] [P]
 1   2   3 

move 1 from 2 to 1
move 3 from 1 to 3
move 2 from 2 to 1
move 1 from 1 to 2
)"sv.substr(1),
                 "CMZ"sv,
                 "MCD"sv)

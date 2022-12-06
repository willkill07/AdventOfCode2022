#include <algorithm>
#include <vector>

#include "Day.hpp"

namespace day05 {

using stack_t = std::vector<char>;

struct command {
  u32 count, from, to;

  template <bool Bulk>
  [[gnu::always_inline]] inline void execute(std::vector<stack_t> &stacks) const noexcept {
    stack_t &src{stacks[from - 1]};
    stack_t &dst{stacks[to - 1]};
    auto const new_src_end{std::end(src) - count};
    auto const src_begin = [&] {
      if constexpr (Bulk) {
        // start copy "count" down from top (as one big move)
        return new_src_end;
      } else {
        // start copy from the top->down (use reverse iterator)
        return std::rbegin(src);
      };
    }();
    std::copy_n(src_begin, count, std::back_inserter(dst));
    src.erase(new_src_end, std::end(src));
  }
};

struct state {
  std::vector<stack_t> stacks;
  std::vector<command> commands;
};
} // namespace day05

using Day05 = Day<5, day05::state, std::string>;

#include "days/day.hpp"
#include "owning_span.hpp"

namespace day05 {

constexpr u32 const MAX_STACK_HEIGHT{60};
constexpr u32 const MAX_STACKS{9};
constexpr u32 const MAX_COMMANDS{512};

using stack_t = owning_span<char, MAX_STACK_HEIGHT>;
using stacks_t = owning_span<stack_t, MAX_STACKS>;

struct command {
  u32 count, from, to;

  template <bool Bulk>
  [[gnu::always_inline]] inline void execute(stacks_t &stacks) const noexcept;
};

using commands_t = owning_span<command, MAX_COMMANDS>;

struct state {
  stacks_t stacks;
  commands_t commands;
};

} // namespace day05

using Day05 = Day<5, day05::state, std::string>;

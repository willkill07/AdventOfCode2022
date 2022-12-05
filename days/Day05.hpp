#include <vector>

#include "Day.hpp"

namespace day05 {

  struct command {
    u32 count, from, to;
  };

  struct state {
    std::vector<std::vector<char>> stacks;
    std::vector<command> commands;
  };
}


using Day05 = Day<5, day05::state, std::string>;

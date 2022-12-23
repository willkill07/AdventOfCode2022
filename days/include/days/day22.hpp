#include <array>
#include <utility>

#include "days/day.hpp"
#include "owning_span.hpp"

namespace day22 {

enum class direction : u32 { right = 0, down = 1, left = 2, up = 3, none = 0xFFFFFFFF };

struct neighbor {
  u32 id{0xFFFFFFFF};
  direction dir{direction::none};
};

struct side {
  std::string_view buffer;
  std::array<neighbor, 4> neighbors{};
  u32 stride{0};
  u32 global_row{0};
  u32 global_col{0};

  [[nodiscard]] inline constexpr bool connected(direction d) const noexcept {
    return neighbors[std::to_underlying(d)].id != 0xFFFFFFFF;
  }
};

struct command {
    direction rotate;
    u32 distance;
};

struct result {
    owning_span<side, 6> sides;
    owning_span<command, 2'000U> commands;
};

}

using Day22 = Day<22, day22::result, u32>;

#include <utility>

#include "days/day.hpp"
#include "owning_span.hpp"

namespace day22 {

enum class direction : u32 { right = 0, down = 1, left = 2, up = 3, num_dirs = 4, none = 0xFFFFFFFF };

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

  [[nodiscard]] constexpr inline bool valid(u32 x, u32 y) const noexcept {
    return buffer[y * stride + x] != '#';
  }

  [[nodiscard]] constexpr inline u32 get_uid(u32 x, u32 y, direction d) const noexcept {
    return std::to_underlying(d) + 4 * (global_col + x + 1) + 1000 * (global_row + y + 1);
  }
};

struct command {
  direction rotate;
  u32 distance;
};

struct result {
  owning_span<side, 6> sides;
  owning_span<command, 2'048U> commands;
  u32 side_len;
};

} // namespace day22

using Day22 = Day<22, day22::result, u32>;

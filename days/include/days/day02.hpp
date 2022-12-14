#include <array>

#include "days/day.hpp"

namespace day02 {

constexpr const i32 options{3};
using lookup_table_t = std::array<u32, options * options>;

} // namespace day02

using Day02 = Day<2, day02::lookup_table_t, int>;

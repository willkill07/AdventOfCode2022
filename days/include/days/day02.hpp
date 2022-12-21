#include <array>

#include "days/day.hpp"

namespace day02 {

constexpr const u32 options{3};
constexpr const u32 product{options * options};
using lookup_table_t = std::array<u32, product>;

} // namespace day02

using Day02 = Day<2, day02::lookup_table_t, u32>;

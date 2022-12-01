#include "Day01.hpp"

template <>
typename Day01::parse_result_t Day01::parse(FILE *f) const noexcept {
  (void)f;
  return std::vector{1, 2, 3, 4, 5};
}

template <>
template <bool Part2>
std::conditional_t<Part2, Day01::part1_result_t, Day01::part2_result_t>
Day01::solve(const_bool<Part2>, parse_result_t const &data,
             [[maybe_unused]] std::optional<part1_result_t> part1_answer)
    const noexcept {
  if constexpr (Part2) {
    return *part1_answer + 1;
  } else {
    return static_cast<int>(data.size());
  }
}

INSTANTIATE(Day01);

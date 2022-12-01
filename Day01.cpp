#include <algorithm>
#include <numeric>
#include <utility>

#include <cctype>
#include <cstdio>

#include <sys/mman.h>

#include "Day01.hpp"

template <>
typename Day01::parse_result_t Day01::parse(FILE *f) const noexcept {
  fseek(f, 0L, SEEK_END);
  i64 len = ftell(f);
  rewind(f);

  char* addr = reinterpret_cast<char*>(mmap(NULL, static_cast<sz>(len), PROT_READ, MAP_PRIVATE, fileno(f), 0));

  std::vector<int> elves;
  int curr_calories{0};
  int curr_sum{0};
  bool saw_space{false};

  for (i64 i{0}; i < len; ++i) {
    char const curr = addr[i];
    if (std::isdigit(curr)) {
      curr_calories *= 10;
      curr_calories += (curr - '0');
      saw_space = false;
    } else if (saw_space) {
      elves.push_back(curr_sum);
      curr_sum = 0;
      saw_space = false;
    } else {
      curr_sum += curr_calories;
      curr_calories = 0;
      saw_space = true;
    }
  }
  munmap(addr, static_cast<sz>(len));
  std::nth_element(std::begin(elves), std::end(elves) - 3, std::end(elves));
  return elves;
}

template <>
template <bool Part2>
std::conditional_t<Part2, Day01::part1_result_t, Day01::part2_result_t>
Day01::solve(const_bool<Part2>, parse_result_t const &data,
             [[maybe_unused]] std::optional<part1_result_t> part1_answer)
    const noexcept {
  if constexpr (Part2) {
    return std::accumulate(std::end(data) - 3, std::end(data), 0);
  } else {
    return *std::max_element(std::end(data) - 3, std::end(data));
  }
}

INSTANTIATE(Day01);

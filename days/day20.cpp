#include <algorithm>
#include <cstring>
#include <numeric>

#include "days/day20.hpp"
#include "owning_span.hpp"
#include "parsing.hpp"

namespace {

template <typename T>
constexpr inline T
fast_mod(T val, unsigned N) {
  if (N == 0) {
    __builtin_unreachable();
  }
  if (N == 5000) {
    return val % 5000u;
  } else if (N == 4999) {
    return val % 4999u;
  } else {
    return val % N;
  }
}

template <unsigned Steps>
inline i64
run(day20::list_type<i64> const &val, unsigned zero_index) noexcept {
  unsigned const N{std::size(val)};

  // populate id array with [0, 1, 2, 3, ... ]
  day20::list_type<unsigned> id(N);
  std::iota(std::begin(id), std::end(id), 0u);

  // could use C++ standard library:
  //    std::distance(std::begin(haystack), std::find(std::begin(haystack), std::end(haystack), needle))
  // but it was slower and generated less optimal code
  auto const index_of = [](auto const needle, auto const &haystack, auto const n) {
    for (unsigned i{0}; i < n; ++i) {
      if (haystack[i] == needle) {
        return i;
      }
    }
    return n;
  };

  unsigned *base{std::begin(id)};
  // for each step
  for (unsigned step{0}; step < Steps; ++step) {
    // for each value
    for (unsigned i{0}; i < N; ++i) {
      i64 const value{val[i]};
      // find the front of the range
      unsigned const front{index_of(i, id, N)};
      // find the tail of the range
      unsigned tail = fast_mod(as<unsigned>(front + N + fast_mod(value, N - 1)), N);
      if (tail > front) {
        // we will have to shift left one
        if (value < 0) {
          // decrement the end of the range if our value is negative
          --tail;
        }
        unsigned *const begin{base + front + 1};
        unsigned const count{tail - front};
        ::memmove(begin - 1, begin, count * sizeof(unsigned));
      } else if (tail < front) {
        // we will have to shift right one
        if (value > 0) {
          // increment the beginning of the range if our value is positive
          ++tail;
        }
        unsigned *const begin{base + tail};
        unsigned const count{front - tail};
        ::memmove(begin + 1, begin, count * sizeof(unsigned));
      }
      id[tail] = i;
    }
  }
  unsigned const zero{index_of(zero_index, id, N)};
  return val[id[fast_mod(zero + 1000u, N)]] + val[id[fast_mod(zero + 2000u, N)]] + val[id[fast_mod(zero + 3000u, N)]];
}

} // namespace

PARSE_IMPL(Day20, view) {
  typename day20::list_type<i64> numbers;
  u32 zero_index{0xFFFFFFFFU};
  u32 idx{0};
  for (usize off{0}; off < std::size(view);) {
    i32 val;
    off += parse<"\0\n">(view.substr(off), val);
    if (val == 0) {
      zero_index = idx;
    }
    numbers.push(val);
    ++idx;
  }
  return {numbers, zero_index};
}

SOLVE_IMPL(Day20, Part2, state, part1_answer) {
  auto const &nums{state.numbers};
  unsigned const zero_index{state.zero_index};

  if constexpr (not Part2) {
    return run<1>(nums, zero_index);
  } else {
    owning_span<i64, day20::MAXN> numbers(std::size(nums));
    std::transform(std::begin(nums), std::end(nums), std::begin(numbers), [](i32 val) {
      return 811'589'153L * val;
    });
    return run<10>(numbers, zero_index);
  }
}

INSTANTIATE(Day20);

INSTANTIATE_TEST(Day20,
                 R"(
1
2
-3
3
-2
0
4
)"sv.substr(1),
                 3L,
                 1623178306L)

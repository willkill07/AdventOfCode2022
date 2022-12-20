#include <algorithm>

#include "days/day20.hpp"
#include "owning_span.hpp"
#include "parsing.hpp"

namespace {

template <u32 Den>
constexpr inline i32
fast_rem(i64 val) noexcept {
  return val % Den;
}

constexpr inline i32
fast_rem(i64 val, u32 den) {
  if (den == 4999) [[likely]] {
    return fast_rem<4999>(val);
  } else if (den == 6) {
    return fast_rem<6>(val);
  } else {
    return static_cast<i32>(val % den);
  }
}

constexpr inline void
unhook(u32 id, typename day20::list_type &list) noexcept {
  u32 const next{list[id].next}, prev{list[id].prev};
  list[prev].next = next;
  list[next].prev = prev;
}

constexpr inline void
hook_after(u32 id, typename day20::list_type &list, u32 prev) noexcept {
  __builtin_assume(id != prev);
  u32 const next = list[prev].next;
  list[id].prev = prev;
  list[id].next = next;
  list[next].prev = id;
  list[prev].next = id;
}

constexpr inline u32
prev(u32 from, typename day20::list_type &list, u32 amount) noexcept {
  for (u32 i{0}; i < amount; ++i) {
    from = list[from].prev;
  }
  return from;
}

constexpr inline u32
next(u32 from, typename day20::list_type &list, u32 amount) noexcept {
  for (u32 i{0}; i < amount; ++i) {
    from = list[from].next;
  }
  return from;
}

} // namespace

PARSE_IMPL(Day20, view) {
  typename day20::list_type numbers;
  u32 zero_index;
  u32 idx{0};
  for (usize off{0}; off < std::size(view);) {
    i64 val;
    off += parse<"\0\n">(view.substr(off), val);
    if (val == 0) {
      zero_index = idx;
    }
    numbers.push({val, idx - 1, idx + 1});
    ++idx;
  }
  std::begin(numbers)->prev = std::size(numbers) - 1;
  std::rbegin(numbers)->next = 0;
  return {numbers, zero_index};
}

SOLVE_IMPL(Day20, Part2, state, part1_answer) {
  auto const &nums{state.list};
  u32 const zero_index{state.zero_index};
  day20::list_type list(std::size(nums));
  std::transform(std::begin(nums), std::end(nums), std::begin(list), [](day20::node n) {
    if constexpr (Part2) {
      n.value *= 811'589'153;
    }
    return n;
  });

  u32 const size{std::size(list)};

  for (u32 iters{0}; iters < (Part2 ? 10 : 1); ++iters) {
    for (u32 idx{0}; idx < size; ++idx) {
      auto const value{list[idx].value};
      u32 new_loc{-1u};
      if (i32 const skip{fast_rem(value, size - 1)}; skip < 0) {
        new_loc = prev(idx, list, static_cast<u32>(1 - skip));
      } else {
        new_loc = next(idx, list, static_cast<u32>(skip));
      }
      if (new_loc != idx) {
        unhook(idx, list);
        hook_after(idx, list, new_loc);
      }
    }
  }

  u32 const one{next(zero_index, list, 1000)};
  u32 const two{next(one, list, 1000)};
  u32 const three{next(two, list, 1000)};

  return list[one].value + list[two].value + list[three].value;
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
                 3l,
                 1623178306l)

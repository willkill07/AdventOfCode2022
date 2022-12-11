#include <array>
#include <vector>

#include "days/day.hpp"

namespace day11 {

template <auto Mod, typename T>
constexpr inline bool fast_evenly_divisible(T value) noexcept {
  return (value % Mod) == 0;
}

static constexpr std::array<bool (*) (u64) noexcept, 24> mod_lookup {{
  nullptr,
  nullptr,
  &fast_evenly_divisible<2>,
  &fast_evenly_divisible<3>,
  nullptr,
  &fast_evenly_divisible<5>,
  nullptr,
  &fast_evenly_divisible<7>,
  nullptr,
  nullptr,
  nullptr,
  &fast_evenly_divisible<11>,
  nullptr,
  &fast_evenly_divisible<13>,
  nullptr,
  nullptr,
  nullptr,
  &fast_evenly_divisible<17>,
  nullptr,
  &fast_evenly_divisible<19>,
  nullptr,
  nullptr,
  nullptr,
  &fast_evenly_divisible<23>
}};

enum class op { plus = '+', multiplies = '*' };

template <typename T>
class operation {
  T m_rhs;
  op m_op;
  bool m_rhs_is_lhs{false};

public:
  constexpr operation() = delete;

  constexpr inline operation(T rhs, op o) noexcept
      : m_rhs{rhs},
        m_op{o},
        m_rhs_is_lhs(rhs == 0) {
  }

  [[nodiscard]] constexpr inline T inspect(T old) const noexcept {
    auto const lhs = old;
    auto const rhs = (m_rhs_is_lhs ? lhs : m_rhs);
    switch (m_op) {
    case op::plus:
      return lhs + rhs;
    case op::multiplies:
      return lhs * rhs;
    }
  }
};

template <typename T>
class test {
  T div;
  u32 if_true;
  u32 if_false;

public:
  constexpr inline test(T div_amount, u32 throw_if_true, u32 throw_if_false)
      : div{div_amount},
        if_true{throw_if_true},
        if_false{throw_if_false} {
  }

  constexpr test() = delete;

  [[nodiscard]] constexpr inline u32 evaluate(T value) const noexcept {
    return mod_lookup[div](value) ? if_true : if_false;
  }
};

template <typename T>
class monkey {
  std::vector<T> m_items;
  operation<T> m_op;
  test<T> m_test;
  u64 m_inspection_count{0};

public:
  constexpr inline monkey(std::vector<T> &&items, operation<T> const &op, test<T> const &test)
      : m_items{std::move(items)},
        m_op{op},
        m_test{test} {
  }

  constexpr monkey() = delete;

  template <bool Part2>
  constexpr inline void throw_items(std::vector<monkey> &all_monkeys) noexcept {
    m_inspection_count += std::size(m_items);
    for (T item : m_items) {
      T worry_level{m_op.inspect(item)};
      if constexpr (Part2) {
        worry_level %= (T{2} * T{3} * T{5} * T{7} * T{11} * T{13} * T{17} * T{19} * T{23});
      } else {
        worry_level /= T{3};
      }
      all_monkeys[m_test.evaluate(worry_level)].catch_item(worry_level);
    }
    m_items.clear();
  }

  constexpr inline void catch_item(T item) noexcept {
    m_items.push_back(item);
  }

  constexpr inline u64 count() noexcept {
    return m_inspection_count;
  }
};

} // namespace day11

using Day11 = Day<11, std::vector<day11::monkey<u64>>, u64>;

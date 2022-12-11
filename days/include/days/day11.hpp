#include <array>
#include <span>
#include <vector>

#include "days/day.hpp"

namespace day11 {

constexpr static inline u32 MAX_MONKEYS = 8;
constexpr static inline u32 MAX_ITEMS = 40;

template <auto Mod, typename T>
[[gnu::always_inline]] constexpr inline bool
fast_evenly_divisible(T value) noexcept {
  return (value % Mod) == 0;
}

enum class op { plus, multiplies };

template <typename T>
class operation {
  T m_rhs{};
  op m_op{};
  bool m_rhs_is_lhs{false};

public:
  constexpr operation() noexcept = default;

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
    default:
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
  constexpr test() noexcept = default;

  constexpr inline test(T div_amount, u32 throw_if_true, u32 throw_if_false)
      : div{div_amount},
        if_true{throw_if_true},
        if_false{throw_if_false} {
  }

  [[nodiscard]] constexpr inline u32 evaluate(T value) const noexcept {
    bool const cond = [](T v, u64 d) noexcept {
      switch (d) {
      case 3:
        return fast_evenly_divisible<3>(v);
      case 5:
        return fast_evenly_divisible<5>(v);
      case 7:
        return fast_evenly_divisible<7>(v);
      case 11:
        return fast_evenly_divisible<11>(v);
      case 13:
        return fast_evenly_divisible<13>(v);
      case 17:
        return fast_evenly_divisible<17>(v);
      case 19:
        return fast_evenly_divisible<19>(v);
      case 23:
        return fast_evenly_divisible<23>(v);
      default:
        return fast_evenly_divisible<2>(v);
      }
    }(value, div);
    return cond ? if_true : if_false;
  }
};

template <typename T>
constinit T static inline bigmod = T{2} * T{3} * T{5} * T{7} * T{11} * T{13} * T{17} * T{19} * T{23};

template <typename T>
class monkey {
  std::array<T, MAX_ITEMS> m_items{};
  u32 m_num_items{0};
  operation<T> m_op{};
  test<T> m_test{};
  u64 m_inspection_count{0};

public:
  constexpr monkey() noexcept = default;

  constexpr inline monkey(std::array<T, MAX_ITEMS> &&items, u32 num_items, operation<T> const &op, test<T> const &test)
      : m_items{std::move(items)},
        m_num_items{num_items},
        m_op{op},
        m_test{test} {
  }

  constexpr inline std::span<T const> items() const noexcept {
    return std::span(m_items.data(), m_num_items);
  }

  template <bool Part2>
  constexpr inline void throw_items(std::span<monkey> all_monkeys) noexcept {
    m_inspection_count += m_num_items;
    for (T item : items()) {
      T worry_level{m_op.inspect(item)};
      if constexpr (Part2) {
        worry_level %= bigmod<T>;
      } else {
        worry_level /= T{3};
      }
      all_monkeys[m_test.evaluate(worry_level)].catch_item(worry_level);
    }
    m_num_items = 0;
  }

  constexpr inline void catch_item(T item) noexcept {
    m_items[m_num_items++] = item;
  }

  constexpr inline u64 count() noexcept {
    return m_inspection_count;
  }
};

struct result_type {
  std::array<monkey<u64>, MAX_MONKEYS> max_monkeys;
  u32 num_monkeys;

  result_type() noexcept = delete;

  constexpr inline result_type(std::array<monkey<u64>, MAX_MONKEYS> &&m, u32 n) noexcept
      : max_monkeys{std::move(m)},
        num_monkeys{n} {
  }
};

} // namespace day11

using Day11 = Day<11, day11::result_type, u64>;

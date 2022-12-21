#include <utility>

#include "days/day.hpp"
#include "owning_span.hpp"

namespace day11 {

constexpr static inline u32 MAX_MONKEYS = 8;
constexpr static inline u32 MAX_ITEMS = 40;

enum class op_type : u8 { plus, multiplies, squares };

namespace detail {

template <auto Mod, typename T>
[[gnu::always_inline]] constexpr inline bool
fast_evenly_divisible(T value) noexcept {
  return (value % Mod) == 0;
}

template <typename T>
constinit T static inline bigmod = T{2} * T{3} * T{5} * T{7} * T{11} * T{13} * T{17} * T{19} * T{23};

template <op_type Op>
[[nodiscard, gnu::always_inline]] inline constexpr auto
new_value(u32 old, u32 rhs) noexcept {
  if constexpr (Op == op_type::squares) {
    return static_cast<u64>(old) * static_cast<u64>(old);
  } else if constexpr (Op == op_type::plus) {
    return old + rhs;
  } else {
    return old * rhs;
  }
}

template <bool Part2, typename U>
[[nodiscard, gnu::always_inline]] inline constexpr u32
update(U res) noexcept {
  if constexpr (Part2) {
    if (res > bigmod<U>) [[unlikely]] {
      return static_cast<u32>(res % bigmod<U>);
    } else {
      return static_cast<u32>(res);
    }
  } else {
    return static_cast<u32>(res / U{3});
  }
}

} // namespace detail

class operation {
  u32 m_rhs{};
  op_type m_op{};

public:
  constexpr operation() noexcept = default;

  constexpr inline operation(u32 rhs, op_type op) noexcept
      : m_rhs(rhs),
        m_op(op) {
  }

  constexpr static inline operation power() noexcept {
    return {0, op_type::squares};
  }

  constexpr static inline operation add(u32 rhs) noexcept {
    return {rhs, op_type::plus};
  }

  constexpr static inline operation mul(u32 rhs) noexcept {
    return {rhs, op_type::multiplies};
  }

  template <bool Part2>
  [[nodiscard, gnu::always_inline]] constexpr inline u32 new_worry_level(u32 old) const noexcept {
    if (m_op == op_type::plus) [[likely]] {
      return detail::update<Part2>(detail::new_value<op_type::plus>(old, m_rhs));
    } else if (m_op == op_type::multiplies) {
      return detail::update<Part2>(detail::new_value<op_type::multiplies>(old, m_rhs));
    } else {
      return detail::update<Part2>(detail::new_value<op_type::squares>(old, m_rhs));
    }
  }
};

using items_type = owning_span<u32, MAX_ITEMS>;

class monkey {
  u64 m_inspection_count{0};
  items_type m_items;
  operation m_op{};
  u8 div_amount;
  u8 if_true;
  u8 if_false;

public:
  constexpr monkey() noexcept = default;

  constexpr inline monkey(items_type const& items, operation const &op, u8 div, u8 throw_if_true, u8 throw_if_false)

      : m_items{items},
        m_op{op},
        div_amount{div},
        if_true{throw_if_true},
        if_false{throw_if_false} {
  }

  [[nodiscard]] constexpr inline std::span<u32 const> items() const noexcept {
    return m_items;
  }

  constexpr inline void update_throws() noexcept {
    m_inspection_count += std::size(m_items);
    m_items.resize(0);
  }

  constexpr inline void catch_item(u32 item) noexcept {
    m_items.push(item);
  }

  constexpr inline u64 count() noexcept {
    return m_inspection_count;
  }

  constexpr inline operation const &get_op() noexcept {
    return m_op;
  }

  [[nodiscard]] constexpr inline u32 get_destination(u32 value) const noexcept {
    bool const cond = [](auto v, auto d) noexcept {
      switch (d) {
      case 3:
        return detail::fast_evenly_divisible<3>(v);
      case 5:
        return detail::fast_evenly_divisible<5>(v);
      case 7:
        return detail::fast_evenly_divisible<7>(v);
      case 11:
        return detail::fast_evenly_divisible<11>(v);
      case 13:
        return detail::fast_evenly_divisible<13>(v);
      case 17:
        return detail::fast_evenly_divisible<17>(v);
      case 19:
        return detail::fast_evenly_divisible<19>(v);
      case 23:
        return detail::fast_evenly_divisible<23>(v);
      default:
        return detail::fast_evenly_divisible<2>(v);
      }
    }(value, div_amount);
    return cond ? if_true : if_false;
  }
};

} // namespace day11

using Day11 = Day<11, owning_span<day11::monkey, day11::MAX_MONKEYS>, u64>;

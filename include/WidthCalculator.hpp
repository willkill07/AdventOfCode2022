#include <array>
#include <concepts>
#include <string_view>

#include "MetaUtils.hpp"
#include "Types.hpp"

template <sz Columns>
class width_calculator {
  std::array<bool, Columns> mask{};
  std::array<sz, Columns> total{};

  struct column_state {
    sz sum{0};
    sz count{0};

    inline void add(sz width) noexcept {
      ++count;
      sum += width;
    }

    inline sz get_adjustment(sz growth_amount, sz id) const noexcept {
      return (growth_amount * (id + 1) / count) - (growth_amount * id / count);
    }

    inline sz real_width() const noexcept {
      return (count == 0) ? 0lu : (count - 1) * 3 + sum;
    }
  };

public:
  template <typename T>
  width_calculator(std::array<T, Columns> const &initial_values, std::array<bool, Columns> mask) noexcept
      : mask{mask} {
    for (sz idx{0}; idx < Columns; ++idx) {
      if (mask[idx]) {
        total[idx] = std::string_view{initial_values[idx]}.size();
      } else {
        total[idx] = 0;
      }
    }
  }

  template <sz CurrLen, sz Offset>
  inline column_state get_column() noexcept {
    column_state state;
    static_for<CurrLen>([&]<sz i> {
      if (mask[Offset + i]) {
        state.add(total[Offset + i]);
      }
    });
    return state;
  }

  template <std::array Array, typename T>
  void update(std::array<T, Array.size()> const &col_vals) noexcept
    requires std::constructible_from<std::string_view, T>
  {
    static_assert(sum(Array) == Columns, "Invalid grouping array specified");
    constexpr auto const Offsets = exclusive_sum(Array);
    // walk through each "size" in grouping array
    static_for<Array.size()>([&]<sz Idx>() {
      constexpr sz const curr_len{Array[Idx]};
      column_state const col{get_column<curr_len, Offsets[Idx]>()};
      sz const required{std::size(std::string_view{col_vals[Idx]})};
      sz const current{col.real_width()};
      // if we need to expand
      if (current < required) {
        sz const growth_amount{required - current};
        sz curr{0};
        static_for<curr_len>([&]<sz I>() {
          // we are not hiding the underlying
          if (mask[Offsets[Idx] + I]) {
            total[Offsets[Idx] + I] += col.get_adjustment(growth_amount, curr++);
          }
        });
      }
    });
  }

  template <std::array Array>
  std::array<sz, Array.size()> get(std::array<bool, Array.size()> const &curr_mask) noexcept {
    static_assert(sum(Array) == Columns, "Invalid grouping array specified");
    constexpr auto const Offsets = exclusive_sum(Array);
    std::array<sz, Array.size()> arr;
    static_for<Array.size()>([&]<sz Idx>() {
      arr[Idx] = (curr_mask[Idx] ? get_column<Array[Idx], Offsets[Idx]>().real_width() : 0);
    });
    return arr;
  }
};

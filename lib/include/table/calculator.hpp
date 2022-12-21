#include <array>
#include <concepts>
#include <string_view>

#include "meta/utils.hpp"
#include "types.hpp"

namespace table {

template <usize Columns>
class width_calculator {
  std::array<bool, Columns> const mask;
  std::array<usize, Columns> total;

  struct column_state {
    usize sum{0};
    usize count{0};

    inline void add(usize width) noexcept {
      ++count;
      sum += width;
    }

    [[nodiscard]] inline usize get_adjustment(usize growth_amount, usize id) const noexcept {
      return (growth_amount * (id + 1) / count) - (growth_amount * id / count);
    }

    [[nodiscard]] inline usize real_width() const noexcept {
      return (count == 0) ? 0LU : (count - 1) * 3 + sum;
    }
  };

public:
  template <typename T>
  width_calculator(std::array<T, Columns> const &initial_values, std::array<bool, Columns> visibility_mask) noexcept
      : mask{visibility_mask},
        total{fold<Columns>(
            std::array<usize, Columns>{},
            []<usize Idx>(auto &tot, constant_t<Idx>, auto const &init_vals, auto const &vis_mask) {
              tot[Idx] = (vis_mask[Idx] ? std::string_view{init_vals[Idx]}.size() : 0);
            },
            initial_values,
            visibility_mask)} {
  }

  template <usize CurrLen, usize Offset>
  inline column_state get_column(std::array<bool, Columns> mask_override = all<Columns>(true)) noexcept {
    return fold<CurrLen>(
        column_state{},
        []<usize I>(column_state &state, constant_t<I>, width_calculator<Columns> &self, auto& m) {
          if (self.mask[I + Offset] and m[I + Offset]) {
            state.add(self.total[I + Offset]);
          }
        },
        *this,
        mask_override);
  }

  template <std::array Array, typename T, usize N = Array.size()>
  void update(std::array<T, N> const &col_vals, std::array<bool, Columns> col_update_mask = all<Columns>(true)) noexcept
    requires std::constructible_from<std::string_view, T>
  {
    static_assert(sum(Array) == Columns, "Invalid grouping array specified");
    // walk through each "size" in grouping array
    // need this to be compile-time for the call to get_column below
    static_for<N>(
        []<usize Idx>(constant_t<Idx>, width_calculator<Columns> &self, auto const &cv, auto const &update_mask) {
          constexpr std::array Offsets{exclusive_scan(Array)};
          constexpr usize const CurrLen{Array[Idx]};
          column_state const col{self.get_column<CurrLen, Offsets[Idx]>(update_mask)};
          if (col.count == 0) {
            return;
          }
          usize const required{std::size(std::string_view{cv[Idx]})};
          usize const current{col.real_width()};
          // if we need to expand
          if (current < required) {
            usize const growth_amount{required - current};
            usize curr{0};
            for (usize Jdx{0}; Jdx < CurrLen; ++Jdx) {
              // we are not hiding the underlying
              if (self.mask[Offsets[Idx] + Jdx]) {
                self.total[Offsets[Idx] + Jdx] += col.get_adjustment(growth_amount, curr++);
              }
            }
          }
        },
        *this,
        col_vals,
        col_update_mask);
  }

  template <std::array Array, usize N = Array.size()>
  auto get(std::array<bool, N> const &curr_mask) noexcept {
    static_assert(sum(Array) == Columns, "Invalid grouping array specified");
    static constexpr std::array const Offsets = exclusive_scan(Array);
    return fold<N>(
        std::array<usize, N>{},
        []<usize Idx>(auto &width, constant_t<Idx>, width_calculator<Columns> &self, auto const &cm) {
          width[Idx] = (cm[Idx] ? self.get_column<Array[Idx], Offsets[Idx]>().real_width() : 0);
        },
        *this,
        curr_mask);
  }

  void ensure_at_least(std::array<usize, Columns> const &vals) noexcept {
    for (usize i{0}; i < Columns; ++i) {
      total[i] = std::max(total[i], vals[i]);
    }
  }
};

} // namespace table

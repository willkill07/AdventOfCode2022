#include <array>
#include <compare>
#include <limits>
#include <span>
#include <utility>

#include "days/day.hpp"

namespace day13 {

struct structure;

struct handle {

  using type = i16;

  static constexpr inline void set_data(handle const *data_pointer) {
    handle::data = data_pointer;
  }

  constexpr inline handle(type v) noexcept
      : value{v} {
  }

  constexpr inline handle() noexcept = default;
  constexpr inline handle(handle &&) noexcept = default;
  constexpr inline handle(handle const &) noexcept = default;
  constexpr inline handle &operator=(handle &&) noexcept = default;
  constexpr inline handle &operator=(handle const &) noexcept = default;

  constexpr inline std::partial_ordering operator<=>(handle const &) const noexcept;
  constexpr inline bool operator==(handle const &) const noexcept = default;
  constexpr inline bool operator!=(handle const &) const noexcept = default;

  constexpr inline type get_value() const noexcept;
  constexpr inline std::span<handle const> get_list() const noexcept;

  type value{std::numeric_limits<type>::min()};

private:
  static inline handle const *data = nullptr;
};

static constexpr handle Null = handle{};
static constexpr u32 MAX_CELLS{16384};
static constexpr u32 MAX_LEN{6};
static constexpr u32 MAX_ELEMS{300};

struct structure {

  constexpr inline structure(std::string_view buffer) noexcept;
  constexpr inline std::span<handle const> trials() const noexcept;
  constexpr inline handle const *data() const noexcept;

private:
  // parse functions
  constexpr inline handle List() noexcept;
  constexpr inline handle Elem() noexcept;

  char const *ptr{nullptr};
  char const *end{nullptr};
  std::array<handle, MAX_CELLS> heap;
  std::array<handle, MAX_ELEMS> raw_elems;
  u32 num_elems{0};
  typename handle::type brk{1};
};

} // namespace day13

using Day13 = Day<13, day13::structure, i64>;

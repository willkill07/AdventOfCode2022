#include <algorithm>
#include <type_traits>

#include "days/day13.hpp"

namespace day13 {

// handle
// ============================================================================

constexpr inline typename handle::type
handle::get_value() const noexcept {
  return value;
}

constexpr inline std::span<handle const>
handle::get_list() const noexcept {
  // handle refers to internal heap location
  if (value < 0) {
    // starting is the data pointer plus the offset (subtract since negated)
    handle const *start{handle::data - value};
    // calculate the length of the list
    u32 len{0};
    while (start[len] != Null) {
      ++len;
    }
    // return the span of handles
    return std::span{start, len};
  } else {
    // transform number into list
    return std::span{std::addressof(*this), 1};
  }
}

constexpr inline std::partial_ordering
handle::operator<=>(handle const &b) const noexcept {
  if (i32 const av{value}, bv{b.value}; av >= 0 and bv >= 0) {
    // we have a number
    if (av < bv) {
      return std::partial_ordering::less;
    } else if (av > bv) {
      return std::partial_ordering::greater;
    } else {
      return std::partial_ordering::unordered;
    }
  } else {
    // have at least one list
    std::span const sa{get_list()};
    std::span const sb{b.get_list()};
    auto A = std::begin(sa);
    auto B = std::begin(sb);
    while (true) {
      bool const a_good{A != std::end(sa)};
      bool const b_good{B != std::end(sb)};
      // check if we have more elements in both lists
      if (not a_good or not b_good) {
        if (b_good) {
          return std::partial_ordering::less;
        } else if (a_good) {
          return std::partial_ordering::greater;
        } else {
          return std::partial_ordering::unordered;
        }
      }
      // look at the next pair of values
      auto const res = (*A <=> *B);
      // if we are less or greater, early exit!
      if (res != std::partial_ordering::unordered) {
        return res;
      }
      ++A;
      ++B;
    }
  }
}

// structure
// ============================================================================

constexpr inline structure::structure(std::string_view buffer) noexcept
    : ptr{std::begin(buffer)},
      end{std::end(buffer)} {
  while (ptr < end) {
    raw_elems[num_elems++] = List();
    ++ptr; // expect '\n';
    raw_elems[num_elems++] = List();
    ++ptr; // expect '\n';
    ++ptr; // expect '\n';
  }
}

constexpr inline std::span<handle const>
structure::trials() const noexcept {
  return {std::data(raw_elems), num_elems};
}

constexpr inline handle const *
structure::data() const noexcept {
  return std::data(heap);
}

// parsing
// ============================================================================

// List := '[' (Elem [',' Elem ]* )? ']'
constexpr inline handle
structure::List() noexcept {
  using type = typename handle::type;
  ++ptr; // expect '['
  std::array<handle, MAX_LEN> elems;
  std::make_unsigned_t<type> count{0};
  if (*ptr != ']') {
    elems[count++] = Elem();
    while (*ptr != ']') {
      ++ptr; // expect ','
      elems[count++] = Elem();
    }
  }
  // add null terminator to list
  elems[count++] = Null;
  ++ptr; // expect ']'
  // copy list elements to list location in "heap"
  std::move(std::begin(elems), std::begin(elems) + count, std::begin(heap) + brk);
  // return location of list
  i16 const result{static_cast<i16>(-brk)};
  brk = static_cast<type>(brk + static_cast<type>(count));
  return result;
}

// Elem := List | [0-9][0-9]?
constexpr inline handle
structure::Elem() noexcept {
  using type = typename handle::type;
  if (*ptr == '[') {
    // list
    return List();
  } else if (type const val(static_cast<type>(*ptr++ - '0')); *ptr >= '0' and *ptr <= '9') {
    // two digit number
    return {static_cast<type>((val * 10) + (*ptr++ - '0'))};
  } else {
    // single digit
    return handle{val};
  }
}

} // namespace day13

// solution
// ============================================================================

PARSE_IMPL(Day13, view) {
  return {view};
}

template <day13::handle H>
bool
cmp(day13::handle const &v) noexcept {
  return v < H;
}

SOLVE_IMPL(Day13, Part2, data, part1_answer) {
  // point the static data handle to the correct place
  day13::handle::set_data(data.data());

  std::span const all{data.trials()};

  if constexpr (Part2) {
    i64 const two = 1L + std::count_if(std::begin(all), std::end(all), cmp<2>);
    i64 const six = 2L + std::count_if(std::begin(all), std::end(all), cmp<6>);
    return two * six;
  } else {
    i64 result{0}, trial{1};
    for (u32 idx{0}; idx < std::size(all); idx += 2) {
      if (all[idx] < all[idx + 1]) {
        result += trial;
      }
      ++trial;
    }
    return result;
  }
}

INSTANTIATE(Day13);

INSTANTIATE_TEST(Day13,
                 R"(
[1,1,3,1,1]
[1,1,5,1,1]

[[1],[2,3,4]]
[[1],4]

[9]
[[8,7,6]]

[[4,4],4,4]
[[4,4],4,4,4]

[7,7,7,7]
[7,7,7]

[]
[3]

[[[]]]
[[]]

[1,[2,[3,[4,[5,6,7]]]],8,9]
[1,[2,[3,[4,[5,6,0]]]],8,9]
)"sv.substr(1),
                 13,
                 140)

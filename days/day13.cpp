#include <algorithm>

#include <doctest/doctest.h>

#include "days/day13.hpp"

// handle
// ============================================================================

constexpr inline i32
day13::handle::get_value() const noexcept {
  return value;
}

constexpr inline std::span<day13::handle const>
day13::handle::get_list() const noexcept {
  // handle refers to internal heap location
  if (value < 0) {
    // starting is the data pointer plus the offset (subtract since negated)
    day13::handle const *start{day13::handle::data - value};
    // calculate the length of the list
    u32 len{0};
    while (start[len] != day13::Null) {
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
day13::handle::operator<=>(day13::handle const &b) const noexcept {
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

constexpr inline day13::structure::structure(std::string_view buffer) noexcept
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

constexpr inline std::span<day13::handle const>
day13::structure::trials() const noexcept {
  return {std::data(raw_elems), num_elems};
}

constexpr inline day13::handle const *
day13::structure::data() const noexcept {
  return std::data(heap);
}

// parsing
// ============================================================================

// List := '[' (Elem [',' Elem ]* )? ']'
constexpr inline day13::handle
day13::structure::List() noexcept {
  ++ptr; // expect '['
  std::array<day13::handle, MAX_LEN> elems;
  u32 count{0};
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
  return day13::handle{-std::exchange(brk, brk + static_cast<i32>(count))};
}

// Elem := List | [0-9][0-9]?
constexpr inline day13::handle
day13::structure::Elem() noexcept {
  if (*ptr == '[') {
    // list
    return List();
  } else if (i32 const val{static_cast<i32>(*ptr++ - '0')}; *ptr >= '0' and *ptr <= '9') {
    // two digit number
    return day13::handle{val * 10 + static_cast<i32>(*ptr++ - '0')};
  } else {
    // single digit
    return day13::handle{val};
  }
}

// solution
// ============================================================================

PARSE_IMPL(Day13, view) {
  return {view};
}

SOLVE_IMPL(Day13, Part2, data, part1_answer) {
  // point the static data handle to the correct place
  day13::handle::set_data(data.data());

  std::span const all{data.trials()};
  u32 const size{static_cast<u32>(std::size(all))};

  u32 result{Part2};

  if constexpr (Part2) {
    constexpr day13::handle const TWO{2}, SIX{6};
    std::array<day13::handle, day13::MAX_ELEMS + 2> elems{};
    std::copy(std::begin(all), std::end(all), std::begin(elems));
    elems[size + 0] = TWO;
    elems[size + 1] = SIX;
    std::stable_sort(std::begin(elems), std::begin(elems) + size + 2);
    for (u32 i{1}; auto e : elems) {
      if (e == TWO || e == SIX) {
        result *= i;
      }
      ++i;
    }
  } else {
    u32 trial{1};
    for (u32 idx{0}; idx < size; idx += 2, ++trial) {
      if (all[idx] < all[idx + 1]) {
        result += trial;
      }
    }
  }
  return result;
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

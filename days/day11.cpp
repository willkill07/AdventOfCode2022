#include <algorithm>
#include <doctest/doctest.h>

#include "days/day11.hpp"
#include "parsing.hpp"

PARSE_IMPL(Day11, view) {
  using T = u64;
  std::array<day11::monkey<T>, day11::MAX_MONKEYS> monkeys;

  u32 num_monkeys{0};
  usize off{0};
  while (off < std::size(view)) {
    // skip "Monkey _:"
    off = view.find_first_of('\n', off) + 3;
    // advance to "Starting items: "
    off = view.find_first_of(':', off) + 2;

    std::array<T, 40> values;
    u32 num_items{0};
    // we have a value!
    while (view[off] != ' ') {
      // do this manually for speed
      values[num_items++] = (static_cast<u32>(view[off] - '0') * 10lu + static_cast<u32>(view[off + 1] - '0'));
      off += 4;
    }

    // advance to "  Operation: new = old "
    off = view.find_first_of('=', off) + 6;
    char const op_char{view[off]};
    // advance to number
    off += 2;
    // get rhs of op -- default to lhs == rhs
    T rhs{0};
    if (view[off] != 'o') {
      // lhs != rhs -- parse
      off += parse<"\0\n">(view.substr(off), rhs);
    }
    day11::operation<T> op{rhs, (op_char == '+' ? day11::op::plus : day11::op::multiplies)};

    // advance to " Test: divisible by "
    off = view.find_first_of('y', off) + 2;
    T div;
    off += parse<"\0\n">(view.substr(off), div);
    off = view.find_first_of('y', off) + 2;
    u32 if_true = static_cast<u32>(view[off] - '0');
    off = view.find_first_of('y', off) + 2;
    u32 if_false = static_cast<u32>(view[off] - '0');
    off += 3;

    day11::test<T> test{div, if_true, if_false};

    monkeys[num_monkeys++] = day11::monkey<T>(std::move(values), num_items, op, test);
  }

  return day11::result_type{std::move(monkeys), num_monkeys};
}

SOLVE_IMPL(Day11, Part2, monkeys, part1_answer) {
  auto working_monkeys = monkeys.max_monkeys;
  auto const count = monkeys.num_monkeys;
  std::span all_monkeys(working_monkeys.data(), count);

  constexpr u32 round_limit{Part2 ? 10'000u : 20u};
  for (u32 round{0}; round < round_limit; ++round) {
    for (auto &monkey : all_monkeys) {
      monkey.throw_items<Part2>(all_monkeys);
    }
  }

  std::array<u64, 8> counts;
  counts.fill(0lu);
  std::span throws(counts.data(), count);
  for (usize midx{0}; midx < count; ++midx) {
    throws[midx] = all_monkeys[midx].count();
  }
  std::nth_element(std::begin(throws), std::end(throws) - 2, std::end(throws));

  return throws[count - 1] * throws[count - 2];
}

INSTANTIATE(Day11);

INSTANTIATE_TEST(Day11,
                 R"(
Monkey 0:
  Starting items: 79, 98
  Operation: new = old * 19
  Test: divisible by 23
    If true: throw to monkey 2
    If false: throw to monkey 3

Monkey 1:
  Starting items: 54, 65, 75, 74
  Operation: new = old + 6
  Test: divisible by 19
    If true: throw to monkey 2
    If false: throw to monkey 0

Monkey 2:
  Starting items: 79, 60, 97
  Operation: new = old * old
  Test: divisible by 13
    If true: throw to monkey 1
    If false: throw to monkey 3

Monkey 3:
  Starting items: 74
  Operation: new = old + 3
  Test: divisible by 17
    If true: throw to monkey 0
    If false: throw to monkey 1
)"sv.substr(1),
                 10605lu,
                 2713310158lu)

#include <algorithm>

#include "days/day11.hpp"
#include "owning_span.hpp"
#include "parsing.hpp"

PARSE_IMPL(Day11, view) {

  owning_span<day11::monkey, day11::MAX_MONKEYS> monkeys;

  for (usize off{0}; off < std::size(view);) {
    // skip "Monkey _:"
    off = view.find_first_of('\n', off) + 3;
    // advance to "Starting items: "
    off = view.find_first_of(':', off) + 2;

    day11::items_type items;
    // we have a value!
    while (view[off] != ' ') {
      // do this manually for speed
      items.push(static_cast<u32>(view[off] - '0') * 10u + static_cast<u32>(view[off + 1] - '0'));
      off += 4;
    }

    // advance to "  Operation: new = old "
    off = view.find_first_of('=', off) + 6;
    char const op_char{view[off]};
    // advance to number
    off += 2;
    // get rhs of op -- default to lhs == rhs
    u32 rhs{0};
    if (view[off] != 'o') {
      // lhs != rhs -- parse
      off += parse<"\0\n">(view.substr(off), rhs);
    }
    day11::operation op;
    if (op_char == '+') {
      op = day11::operation::add(rhs);
    } else if (rhs == 0) {
      op = day11::operation::power();
    } else {
      op = day11::operation::mul(rhs);
    }

    // advance to " Test: divisible by "
    off = view.find_first_of('y', off) + 2;
    u8 div;
    off += parse<"\0\n">(view.substr(off), div);
    off = view.find_first_of('y', off) + 2;
    u8 if_true = static_cast<u8>(view[off] - '0');
    off = view.find_first_of('y', off) + 2;
    u8 if_false = static_cast<u8>(view[off] - '0');
    off += 3;

    monkeys.push({std::move(items), op, div, if_true, if_false});
  }

  return monkeys;
}

SOLVE_IMPL(Day11, Part2, original, part1_answer) {
  auto monkeys = original;
  constexpr u32 round_limit{Part2 ? 10'000u : 20u};

  for (u32 round{0}; round < round_limit; ++round) {
    for (auto &monkey : monkeys) {
      for (u32 item : monkey.items()) {
        u32 const worry_level{monkey.get_op().template new_worry_level<Part2>(item)};
        u32 const destination = monkey.get_destination(worry_level);
        monkeys[destination].catch_item(worry_level);
      }
      monkey.update_throws();
    }
  }

  u32 const count{std::size(monkeys)};
  owning_span<u64, day11::MAX_MONKEYS> throws(count, 0lu);
  for (u32 midx{0}; midx < std::size(throws); ++midx) {
    throws[midx] = monkeys[midx].count();
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

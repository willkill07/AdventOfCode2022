#include <algorithm>
#include <limits>
#include <variant>

#include "days/day.hpp"
#include "days/day21.hpp"
#include "parsing.hpp"

namespace {

constexpr inline u32
get_uid(std::string_view s) noexcept {
  u32 res{0};
  for (char c : s) {
    res = (res << 5) + as<u32>(c - 'a');
  }
  return res;
}

constexpr inline i64
evaluate(day21::list_type<day21::expr_node> const &nodes, u32 id) noexcept {
  if (auto const &node{nodes[id]}; std::holds_alternative<day21::value_node>(node)) {
    return std::get<day21::value_node>(node);
  } else if (std::holds_alternative<day21::op_node>(node)) {
    auto const &tree_node{std::get<day21::op_node>(node)};
    i64 const lhs{evaluate(nodes, tree_node.lhs)};
    i64 const rhs{evaluate(nodes, tree_node.rhs)};
    switch (tree_node.op) {
    case '+':
      return lhs + rhs;
    case '-':
      return lhs - rhs;
    case '/':
      return lhs / rhs;
    case '*':
      return lhs * rhs;
    default:
      __builtin_unreachable();
    }
  }
  __builtin_unreachable();
}

constexpr inline void
simplify(day21::list_type<day21::expr_node> &nodes, day21::expr_node &node) noexcept {
  // only simplify if we are an operation
  if (std::holds_alternative<day21::op_node>(node)) {
    auto const &tree{std::get<day21::op_node>(node)};
    // recursively simplify
    simplify(nodes, nodes[tree.lhs]);
    simplify(nodes, nodes[tree.rhs]);
    // constant fold
    if (std::holds_alternative<day21::value_node>(nodes[tree.lhs]) and
        std::holds_alternative<day21::value_node>(nodes[tree.rhs])) {
      auto const lhs{std::get<day21::value_node>(nodes[tree.lhs])};
      auto const rhs{std::get<day21::value_node>(nodes[tree.rhs])};
      switch (tree.op) {
      case '+':
        node = lhs + rhs;
        break;
      case '-':
        node = lhs - rhs;
        break;
      case '/':
        node = lhs / rhs;
        break;
      case '*':
        node = lhs * rhs;
        break;
      default:
        __builtin_unreachable();
      }
    }
  }
}

constexpr inline i64
symbolic_solve(day21::list_type<day21::expr_node> &numbers, u32 idx) noexcept {

  // simplify the tree
  simplify(numbers, numbers[idx]);

  auto const &tree_node = std::get<day21::op_node>(numbers[idx]);
  auto const lhs_idx{tree_node.lhs};
  auto const rhs_idx{tree_node.rhs};
  bool value_on_lhs{std::holds_alternative<day21::value_node>(numbers[lhs_idx])};
  // target value from one side of the '='
  i64 value{std::get<day21::value_node>(numbers[value_on_lhs ? lhs_idx : rhs_idx])};
  // index to start tree traversal for simplifying / solving
  idx = (value_on_lhs ? rhs_idx : lhs_idx);

  // while we have not seen the symbol
  while (not std::holds_alternative<day21::symbol_node>(numbers[idx])) {
    auto const tree{std::get<day21::op_node>(numbers[idx])};
    value_on_lhs = std::holds_alternative<day21::value_node>(numbers[tree.lhs]);
    // grab the paired value with the subexpression
    auto const paired{std::get<day21::value_node>(numbers[value_on_lhs ? tree.lhs : tree.rhs])};
    // and update the value
    switch (tree.op) {
    case '+':
      value = value - paired;
      break;
    case '-':
      value = (value_on_lhs ? -value : value) + paired;
      break;
    case '*':
      value = value / paired;
      break;
    case '/':
      value = value * paired;
      break;
    default:
      __builtin_unreachable();
    }
    // update the index to traverse "down" the tree
    idx = value_on_lhs ? tree.rhs : tree.lhs;
  }
  return value;
}

} // namespace

PARSE_IMPL(Day21, view) {
  day21::list_type<day21::expr_node> parsed;
  day21::list_type<u32> ids;

  // parse data -- O(N)
  for (usize off{0}; off < std::size(view);) {
    std::string_view id;
    off += parse<"\0: ">(view.substr(off), id);
    ids.push(get_uid(id));
    if ('0' <= view[off] and view[off] <= '9') {
      i64 value;
      off += parse<"\0\n">(view.substr(off), value);
      parsed.push(value);
    } else {
      std::string_view lhs, rhs;
      day21::op_node tree_node;
      off += parse<"\0 \1 \2\n">(view.substr(off), lhs, tree_node.op, rhs);
      tree_node.lhs = get_uid(lhs);
      tree_node.rhs = get_uid(rhs);
      parsed.push(tree_node);
    }
  }

  // offsets for "special" nodes
  u32 root_idx{0}, humn_idx{0};

  // cleaned data for processing -- O(N)
  day21::list_type<day21::expr_node> cleaned(std::size(parsed));

  // make temporary lookup table -- O(N lg N)
  {
    day21::list_type<u32> mapping(std::size(ids));
    {
      day21::list_type<u32> lookup(std::size(ids));
      // create [0, 1, 2, ...]
      std::iota(std::begin(lookup), std::end(lookup), 0U);
      // sort by ID
      std::sort(std::begin(lookup), std::end(lookup), [&](u32 i, u32 j) {
        return ids[i] < ids[j];
      });
      // invert
      for (u32 i{0}; u32 j : lookup) {
        mapping[j] = i++;
      }
    }

    u32 const ROOT{get_uid("root")};
    u32 const HUMN{get_uid("humn")};

    // ingest in-order
    for (u32 i{0}; i < std::size(parsed); ++i) {
      if (ids[i] == ROOT) {
        root_idx = mapping[i];
      }
      if (ids[i] == HUMN) {
        humn_idx = mapping[i];
      }
      cleaned[mapping[i]] = parsed[i];
    }
  }

  // sort IDs (can only be doing after cleaning) -- O(N lg N)
  std::sort(std::begin(ids), std::end(ids));

  // fix references within subexpressions -- O(N lg N)
  for (auto &entry : cleaned) {
    if (std::holds_alternative<day21::op_node>(entry)) {
      auto &tree{std::get<day21::op_node>(entry)};
      auto &lhs{tree.lhs};
      auto &rhs{tree.rhs};
      // O(lg N) each
      lhs = as<u32>(std::distance(std::begin(ids), std::lower_bound(std::begin(ids), std::end(ids), lhs)));
      rhs = as<u32>(std::distance(std::begin(ids), std::lower_bound(std::begin(ids), std::end(ids), rhs)));
    }
  }

  return {cleaned, root_idx, humn_idx};
}

SOLVE_IMPL(Day21, Part2, data, part1_answer) {
  if constexpr (not Part2) {
    return evaluate(data.numbers, data.root);
  } else {
    // make a mutable copy
    auto numbers{data.numbers};
    // set the human to be a default expr_node (symbol)
    numbers[data.humn] = day21::symbol_node{};
    // symbolicly solve for "X" from root
    return symbolic_solve(numbers, data.root);
  }
}

INSTANTIATE(Day21);

INSTANTIATE_TEST(Day21,
                 R"(
root: pppw + sjmn
dbpl: 5
cczh: sllz + lgvd
zczc: 2
ptdq: humn - dvpt
dvpt: 3
lfqf: 4
humn: 5
ljgn: 2
sjmn: drzm * dbpl
sllz: 4
pppw: cczh / lfqf
lgvd: ljgn * ptdq
drzm: hmdt - zczc
hmdt: 32
)"sv.substr(1),
                 152L,
                 301L)

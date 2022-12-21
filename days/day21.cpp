#include <algorithm>
#include <limits>

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
evaluate(day21::list_type<day21::parse_type> const &nodes, u32 id) noexcept {
  if (auto const &node{nodes[id]}; node.type == day21::node_type::value) {
    return node.underlying.value;
  } else {
    auto const &tree_node{node.underlying.tree};
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
}

constexpr inline void
simplify(day21::list_type<day21::parse_type> &nodes, day21::parse_type &node) noexcept {
  if (node.type == day21::node_type::tree) {
    auto const &tree{node.underlying.tree};
    simplify(nodes, nodes[tree.lhs]);
    simplify(nodes, nodes[tree.rhs]);
    if (nodes[tree.lhs].type == day21::node_type::value and nodes[tree.rhs].type == day21::node_type::value) {
      node.type = day21::node_type::value;
      auto const lhs{nodes[tree.lhs].underlying.value};
      auto const rhs{nodes[tree.rhs].underlying.value};
      switch (tree.op) {
      case '+':
        node.underlying.value = lhs + rhs;
        break;
      case '-':
        node.underlying.value = lhs - rhs;
        break;
      case '/':
        node.underlying.value = lhs / rhs;
        break;
      case '*':
        node.underlying.value = lhs * rhs;
        break;
      default:
        __builtin_unreachable();
      }
    }
  }
}

constexpr inline i64
symbolic_solve(day21::list_type<day21::parse_type> &numbers, u32 idx) noexcept {
  auto const lhs_idx{numbers[idx].underlying.tree.lhs};
  auto const rhs_idx{numbers[idx].underlying.tree.rhs};
  bool value_on_lhs{numbers[lhs_idx].type == day21::node_type::value};
  i64 value{numbers[value_on_lhs ? lhs_idx : rhs_idx].underlying.value};
  idx = (value_on_lhs ? rhs_idx : lhs_idx);

  while (numbers[idx].type != day21::node_type::symbol) {
    auto const tree{numbers[idx].underlying.tree};
    value_on_lhs = numbers[tree.lhs].type == day21::node_type::value;
    auto const paired{numbers[value_on_lhs ? tree.lhs : tree.rhs].underlying.value};
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
    idx = value_on_lhs ? tree.rhs : tree.lhs;
  }
  return value;
}

} // namespace

PARSE_IMPL(Day21, view) {
  day21::list_type<day21::parse_type> parsed;
  day21::list_type<u32> ids;

  // parse data -- O(N)
  for (usize off{0}; off < std::size(view);) {
    std::string_view id;
    off += parse<"\0: ">(view.substr(off), id);
    ids.push(get_uid(id));
    day21::parse_type rest;
    if ('0' <= view[off] and view[off] <= '9') {
      rest.type = day21::node_type::value;
      off += parse<"\0\n">(view.substr(off), rest.underlying.value);
    } else {
      rest.type = day21::node_type::tree;
      std::string_view lhs, rhs;
      off += parse<"\0 \1 \2\n">(view.substr(off), lhs, rest.underlying.tree.op, rhs);
      rest.underlying.tree.lhs = get_uid(lhs);
      rest.underlying.tree.rhs = get_uid(rhs);
    }
    parsed.push(rest);
  }

  // offsets for "special" nodes
  u32 root_idx{0}, humn_idx{0};

  // cleaned data for processing -- O(N)
  day21::list_type<day21::parse_type> cleaned(std::size(parsed));

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
    if (entry.type == day21::node_type::tree) {
      auto &lhs{entry.underlying.tree.lhs};
      auto &rhs{entry.underlying.tree.rhs};
      // O(lg N) each
      lhs = as<u32>(std::distance(std::begin(ids), std::lower_bound(std::begin(ids), std::end(ids), lhs)));
      rhs = as<u32>(std::distance(std::begin(ids), std::lower_bound(std::begin(ids), std::end(ids), rhs)));
    }
  }

  return {cleaned, root_idx, humn_idx};
}

PART1_IMPL(Day21, data) {
  return evaluate(data.numbers, data.root);
}

PART2_IMPL(Day21, data, part1_answer) {
  auto numbers{data.numbers};
  numbers[data.humn].type = day21::node_type::symbol;
  simplify(numbers, numbers[data.root]);
  return symbolic_solve(numbers, data.root);
}

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

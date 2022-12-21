#include "days/day.hpp"
#include "owning_span.hpp"

namespace day21 {

constexpr u32 const MAXN{2352U};

template <typename T>
using list_type = owning_span<T, MAXN>;

enum class node_type { value, tree, symbol };

struct parse_type {
  union data {
    struct inner {
      u32 lhs, rhs;
      char op;
    } tree;

    i64 value{0};
  } underlying{};

  node_type type{node_type::value};
};

struct result {
  list_type<parse_type> numbers;
  u32 root;
  u32 humn;
};

} // namespace day21

using Day21 = Day<21, day21::result, i64>;

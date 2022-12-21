#include <variant>

#include "days/day.hpp"
#include "owning_span.hpp"

namespace day21 {

constexpr u32 const MAXN{2352U};

template <typename T>
using list_type = owning_span<T, MAXN>;

struct op_node {
  u32 lhs, rhs;
  char op;
};

using value_node = i64;

struct symbol_node{};

using expr_node = std::variant<symbol_node, op_node, value_node>;

struct result {
  list_type<expr_node> numbers;
  u32 root;
  u32 humn;
};

} // namespace day21

using Day21 = Day<21, day21::result, i64>;

#include <array>
#include <functional>
#include <numeric>
#include <string_view>
#include <utility>

#include <unistd.h>

#include <fmt/color.h>
#include <fmt/core.h>

#include "FixedString.hpp"
#include "TabularPrint.hpp"
#include "Types.hpp"

template <typename T>
using FormatFn = fmt::detail::styled_arg<std::remove_cvref_t<T>> (*)(T const &) noexcept;

template <fmt::emphasis... Emphs>
fmt::detail::styled_arg<std::string>
print_plain(std::string const &v) noexcept {
  if (isatty(STDOUT_FILENO)) {
    return fmt::styled(v, (fmt::text_style{} | ... | fmt::text_style{Emphs}));
  } else {
    return fmt::styled(v, fmt::text_style{});
  }
}

template <fmt::color Color, fmt::emphasis... Emphs>
fmt::detail::styled_arg<std::string>
print(std::string const &v) noexcept {
  if (isatty(STDOUT_FILENO)) {
    return fmt::styled(v, (fmt::fg(Color) | ... | fmt::text_style{Emphs}));
  } else {
    return fmt::styled(v, fmt::text_style{});
  }
}

namespace colors {

constexpr auto header_plain = &print_plain<fmt::emphasis::bold, fmt::emphasis::italic>;
constexpr auto plain = &print_plain<>;

constexpr auto red = &print<fmt::color::red>;
constexpr auto yellow = &print<fmt::color::yellow>;
constexpr auto green = &print<fmt::color::green>;
constexpr auto cyan = &print<fmt::color::cyan>;

constexpr auto bold_red = &print<fmt::color::red, fmt::emphasis::bold>;
constexpr auto bold_yellow = &print<fmt::color::yellow, fmt::emphasis::bold>;
constexpr auto bold_green = &print<fmt::color::green, fmt::emphasis::bold>;
constexpr auto bold_cyan = &print<fmt::color::cyan, fmt::emphasis::bold>;

constexpr auto faint_red = &print<fmt::color::red, fmt::emphasis::faint>;
constexpr auto faint_green = &print<fmt::color::green, fmt::emphasis::faint>;
constexpr auto faint_yellow = &print<fmt::color::yellow, fmt::emphasis::faint>;
constexpr auto faint_cyan = &print<fmt::color::cyan, fmt::emphasis::faint>;

constexpr std::array const group{plain, header_plain, header_plain};
constexpr std::array const header{bold_yellow, bold_red, bold_green, bold_cyan, bold_red, bold_green, bold_yellow};
constexpr std::array const content{yellow, red, green, faint_cyan, faint_red, faint_green, faint_yellow};
constexpr std::array const summary{header_plain, cyan, red, green, yellow};

} // namespace colors

template <fixed_string String, sz Width>
void
print_edge_row(std::array<sz, Width> const &widths) {

  static_assert(String.printable_size == Width * 2 + 1, "table format string mismatch for width");

  auto inner = [&]<sz I>() {
    if (widths[I] > 0) {
      fmt::print("{}", String[2 * I]);
      auto const fmt_str = fmt::format("{{:{}^{}}}", String[2 * I + 1], widths[I] + 2);
      fmt::vprint(fmt_str, fmt::make_format_args(""));
    }
  };

  [&]<sz... Is>(std::integer_sequence<sz, Is...>) {
    (inner.template operator()<Is>(), ...);
    fmt::print("{}\n", String[Width * 2]);
  }
  (std::make_integer_sequence<sz, Width>{});
}

template <fixed_string String, typename T, sz Data, sz Width, sz Style>
void
print_data_row(std::array<T, Data> const &data,
               std::array<sz, Width> const &widths,
               std::array<FormatFn<std::string>, Style> const &stylizers) {

  constexpr sz const elements = (String.printable_size - 1) / 2;
  static_assert((String.printable_size & 1) == 1, "table format string must have odd length");
  static_assert(String.printable_size > 2, "table format string must be at least 3");
  static_assert(elements == Width, "table format string mismatch for width");
  static_assert(elements == Data, "table format string mismatch for data");
  static_assert(elements == Style, "table format string mismatch for stylizers");

  auto inner = [&]<sz I>() {
    if (widths[I] > 0) {
      auto const fmt_str = fmt::format("{{: {0}{1}}}", String[2 * I + 1], widths[I]);
      auto const str = fmt::vformat(fmt_str, fmt::make_format_args(data[I]));
      fmt::print("{} {} ", String[2 * I], stylizers[I](str));
    }
  };

  [&]<sz... Is>(std::integer_sequence<sz, Is...>) {
    (inner.template operator()<Is>(), ...);
    fmt::print("{}\n", String[Width * 2]);
  }
  (std::make_integer_sequence<sz, Width>{});
}

constexpr std::array const content_grouping{1lu, 1lu, 1lu, 1lu, 1lu, 1lu, 1lu};
constexpr std::array const group_grouping{1lu, 2lu, 4lu};
constexpr std::array const summary_grouping{3lu, 1lu, 1lu, 1lu, 1lu};

template <typename T, sz N>
consteval T
sum(std::array<T, N> const &arr) noexcept {
  return std::accumulate(std::begin(arr), std::end(arr), T{0});
}

template <sz Columns>
struct width_calculator {
  std::array<bool, Columns> mask{};
  std::array<sz, Columns> total{};

  sz real_width(sz sum, sz count) const noexcept {
    sz const width{(count == 0) ? 0lu : (count - 1) * 3 + sum};
    return width;
  }

public:
  template <typename T>
  width_calculator(std::array<T, Columns> const &initial_values, std::array<bool, Columns> mask) noexcept
      : mask{mask} {
    for (sz idx{0}; idx < Columns; ++idx) {
      if (mask[idx]) {
        total[idx] = std::string_view{initial_values[idx]}.size();
      } else {
        total[idx] = 0;
      }
    }
  }

  template <std::array Array, typename T>
  void update(std::array<T, Array.size()> const &col_vals) noexcept {
    static_assert(sum(Array) == Columns, "Invalid grouping array specified");
    // walk through each "size" in grouping array
    for (sz off{0}, idx{0}; sz const curr_len : Array) {
      sz sum{0}, count{0};

      // we are not hiding this
      for (sz i{0}; i < curr_len; ++i) {
        // we are not hiding the underlying
        if (mask[off + i]) {
          ++count;
          sum += total[off + i];
        }
      }

      sz const required{std::string_view{col_vals[idx]}.size()};
      sz const current{real_width(sum, count)};

      // if we need to expand
      if (current < required) {
        sz const to_add{required - current};
        sz curr{0};
        for (sz i{0}; i < curr_len; ++i) {
          // we are not hiding the underlying
          if (mask[off + i]) {
            sz const new_min_width{(to_add * (curr + 1) / count) - (to_add * curr / count)};
            total[off + i] += new_min_width;
            ++curr;
          }
        }
      }
      off += curr_len;
      ++idx;
    }
  }

  template <std::array Array, sz N = Array.size()>
  std::array<sz, N> get(std::array<bool, N> const &curr_mask) noexcept {
    static_assert(sum(Array) == Columns, "Invalid grouping array specified");
    std::array<sz, N> arr;
    arr.fill(0lu);
    sz off{0};
    sz idx{0};
    // walk through each "size" in grouping array
    for (sz const curr_len : Array) {
      sz sum{0};
      sz count{0};
      // we are keeping the current column "group"
      if (curr_mask[idx]) {
        for (sz i{0}; i < curr_len; ++i) {
          // we are keeping the underlying column
          if (mask[off + i]) {
            ++count;
            sum += total[off + i];
          }
        }
        // real width + padding + separator
        arr[idx] = real_width(sum, count);
      } else {
        arr[idx] = 0;
      }
      off += curr_len;
      ++idx;
    }
    return arr;
  }
};

void
print_table(run_options const &opts, report_data const &entries, timing_data const &sum) {

  using std::literals::string_literals::operator""s;

  constexpr std::array const group_names{"", "Solutions", "Timing (μs)"};
  constexpr std::array const header_names{"AoC++2022", "Part 1", "Part 2", "Parse", "Part 1", "Part 2", "Total"};

  std::array const summary_data{opts.format("Summary"s),
                                opts.format(sum.parsing),
                                opts.format(sum.part1),
                                opts.format(sum.part2),
                                opts.format(sum.total())};

  width_calculator<7> calc{header_names, opts.content_mask()};
  for (auto const &entry : entries) {
    calc.update<content_grouping>(entry);
  }
  calc.update<summary_grouping>(summary_data);
  calc.update<group_grouping>(group_names);

  std::array const group_widths = calc.get<group_grouping>(opts.group_mask());
  std::array const content_widths = calc.get<content_grouping>(opts.content_mask());
  std::array const summary_widths = calc.get<summary_grouping>(opts.summary_mask());

  print_edge_row<"  ╭─┬─╮">(group_widths);
  print_data_row<" ^│^│^│">(group_names, group_widths, colors::group);
  print_edge_row<"╭─┼─┬─┼─┬─┬─┬─┤">(content_widths);
  print_data_row<"│^│^│^│^│^│^│^│">(header_names, content_widths, colors::header);
  print_edge_row<"├─┼─┼─┼─┼─┼─┼─┤">(content_widths);
  if (opts.single.has_value()) {
    u32 const day{opts.single.value()};
    print_data_row<"│^│<│<│>│>│>│>│">(entries[day], content_widths, colors::content);
  } else {
    for (auto const &entry : entries) {
      print_data_row<"│^│<│<│>│>│>│>│">(entry, content_widths, colors::content);
    }
    if (opts.timing) {
      print_edge_row<"├─┴─┴─┼─┼─┼─┼─┤">(content_widths);
      print_data_row<"│^│>│>│>│>│">(summary_data, summary_widths, colors::summary);
      print_edge_row<"╰─┴─┴─┴─┴─╯">(summary_widths);
      return;
    }
  }
  print_edge_row<"╰─┴─┴─┴─┴─┴─┴─╯">(content_widths);
}

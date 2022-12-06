#include <array>
#include <string_view>
#include <utility>

#include <fmt/core.h>

#include "FixedString.hpp"
#include "Formatting.hpp"
#include "MetaUtils.hpp"
#include "Options.hpp"
#include "TabularPrint.hpp"
#include "Types.hpp"
#include "WidthCalculator.hpp"

template <fixed_string String, usize Width>
void
print_edge_row(std::array<usize, Width> const &widths) {
  static_assert(String.printable_size == Width * 2 + 1, "table format string mismatch for width");
  static_for<Width>(
      []<usize I>(std::array<usize, Width> const &w) {
        if (w[I] > 0) {
          // find the left-most edge to print in case of hiding columns
          usize skipped{0};
          while (I > 1 and w[I - 1 - skipped] == 0) {
            ++skipped;
          }
          fmt::print("{}", String[2 * (I - skipped)]);
          auto const fmt_str = fmt::format("{{:{}^{}}}", String[2 * I + 1], w[I] + 2);
          fmt::vprint(fmt_str, fmt::make_format_args(""));
        }
      },
      widths);
  fmt::print("{}\n", String[Width * 2]);
}

template <fixed_string String, typename T, usize Data, usize Width, usize Style>
void
print_data_row(std::array<T, Data> const &data,
               std::array<usize, Width> const &widths,
               std::array<FormatFn<std::string>, Style> const &stylizers) noexcept {

  constexpr usize const elements = (String.printable_size - 1) / 2;
  static_assert((String.printable_size & 1) == 1, "table format string must have odd length");
  static_assert(String.printable_size > 2, "table format string must be at least 3");
  static_assert(elements == Width, "table format string mismatch for width");
  static_assert(elements == Data, "table format string mismatch for data");
  static_assert(elements == Style, "table format string mismatch for stylizers");

  static_for<Width>(
      []<usize I>(auto const &d, auto const &w, auto const &s) {
        if (w[I] > 0) {
          auto const fmt_str = fmt::format("{{: {0}{1}}}", String[2 * I + 1], w[I]);
          auto const str = fmt::vformat(fmt_str, fmt::make_format_args(d[I]));
          fmt::print("{} {} ", String[2 * I], s[I](str));
        }
      },
      data,
      widths,
      stylizers);
  fmt::print("{}\n", String[Width * 2]);
}

template <usize Width>
inline std::array<FormatFn<std::string>, Width>
maybe_plain(bool color_condition, std::array<FormatFn<std::string>, Width> arr) noexcept {
  if (not color_condition) {
    arr.fill(colors::plain);
  }
  return arr;
}

void
print_table(run_options const &opts, report_data const &entries, timing_data const &sum) {

  using std::literals::string_literals::operator""s;

  constexpr std::array const group_grouping{1lu, 2lu, 4lu};
  constexpr std::array const content_grouping{1lu, 1lu, 1lu, 1lu, 1lu, 1lu, 1lu};
  constexpr std::array const summary_grouping{3lu, 1lu, 1lu, 1lu, 1lu};

  constexpr std::array const group_names{"", "Solutions", "Timing (μs)"};
  constexpr std::array const header_names{"AoC++2022", "Part 1", "Part 2", "Parse", "Part 1", "Part 2", "Total"};
  std::array const summary_data{"Summary"s,
                                opts.format(sum.parsing),
                                opts.format(sum.part1),
                                opts.format(sum.part2),
                                opts.format(sum.total())};

  width_calculator<7> calc{header_names, opts.content_mask()};
  calc.update<group_grouping>(group_names);
  for (auto const &entry : entries) {
    calc.update<content_grouping>(entry);
  }
  calc.update<summary_grouping>(summary_data);

  std::array const group_widths = calc.get<group_grouping>(opts.group_mask());
  std::array const content_widths = calc.get<content_grouping>(opts.content_mask());
  std::array const summary_widths = calc.get<summary_grouping>(opts.summary_mask());

  constexpr std::array const group_colors{colors::plain, colors::header_plain, colors::header_plain};

  constexpr std::array const header_colors{colors::bold_yellow,
                                           colors::bold_red,
                                           colors::bold_green,
                                           colors::bold_cyan,
                                           colors::bold_red,
                                           colors::bold_green,
                                           colors::bold_yellow};

  constexpr std::array const content_colors{colors::yellow,
                                            colors::red,
                                            colors::green,
                                            colors::faint_cyan,
                                            colors::faint_red,
                                            colors::faint_green,
                                            colors::faint_yellow};

  constexpr std::array const summary_colors{colors::header_plain,
                                            colors::cyan,
                                            colors::red,
                                            colors::green,
                                            colors::yellow};

  print_edge_row<"  ╭─┬─╮">(group_widths);
  print_data_row<" ^│^│^│">(group_names, group_widths, maybe_plain(opts.colorize, group_colors));
  print_edge_row<"╭─┼─┬─┼─┬─┬─┬─┤">(content_widths);
  print_data_row<"│^│^│^│^│^│^│^│">(header_names, content_widths, maybe_plain(opts.colorize, header_colors));
  print_edge_row<"├─┼─┼─┼─┼─┼─┼─┤">(content_widths);
  if (opts.single.has_value()) {
    u32 const day{opts.single.value()};
    print_data_row<"│^│<│<│>│>│>│>│">(entries[day], content_widths, maybe_plain(opts.colorize, content_colors));
  } else {
    for (auto const &entry : entries) {
      print_data_row<"│^│<│<│>│>│>│>│">(entry, content_widths, maybe_plain(opts.colorize, content_colors));
    }
    if (opts.timing) {
      print_edge_row<"├─┴─┴─┼─┼─┼─┼─┤">(content_widths);
      print_data_row<"│^│>│>│>│>│">(summary_data, summary_widths, maybe_plain(opts.colorize, summary_colors));
      print_edge_row<"╰─┴─┴─┴─┴─╯">(summary_widths);
      return;
    }
  }
  print_edge_row<"╰─┴─┴─┴─┴─┴─┴─╯">(content_widths);
}

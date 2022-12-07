#include <array>
#include <string_view>
#include <utility>

#include <fmt/core.h>

#include "fixed_string.hpp"
#include "meta/utils.hpp"
#include "options.hpp"
#include "table.hpp"
#include "table/calculator.hpp"
#include "table/formatting.hpp"
#include "types.hpp"

namespace colors = table::colors;

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

constexpr std::array const summary_colors{
    colors::header_plain, colors::cyan, colors::red, colors::green, colors::yellow};

constexpr std::array const group_grouping{1lu, 2lu, 4lu};
constexpr std::array const content_grouping{1lu, 1lu, 1lu, 1lu, 1lu, 1lu, 1lu};
constexpr std::array const summary_grouping{3lu, 1lu, 1lu, 1lu, 1lu};

void
print(run_options const &opts, report_data const &entries, timing_data const &sum) {

  using std::literals::string_literals::operator""s;

  constexpr std::array const group_names{"", "Solutions", "Timing (μs)"};
  constexpr std::array const header_names{"AoC++2022", "Part 1", "Part 2", "Parse", "Part 1", "Part 2", "Total"};
  std::array const summary_data{"Summary"s,
                                opts.format(sum.parsing),
                                opts.format(sum.part1),
                                opts.format(sum.part2),
                                opts.format(sum.total())};

  auto const content_mask{opts.content_mask()};
  table::width_calculator<7> calc{header_names, content_mask};
  calc.update<group_grouping>(group_names);
  for (auto const &entry : entries) {
    calc.update<content_grouping>(entry);
  }
  calc.update<summary_grouping>(summary_data);

  std::array const group_widths = calc.get<group_grouping>(opts.group_mask());
  std::array const content_widths = calc.get<content_grouping>(content_mask);
  std::array const summary_widths = calc.get<summary_grouping>(opts.summary_mask());

  table::print_edge_row<"  ╭─┬─╮">(group_widths);
  table::print_data_row<" ^│^│^│">(group_names, group_widths, table::maybe_plain(opts.colorize, group_colors));
  table::print_edge_row<"╭─┼─┬─┼─┬─┬─┬─┤">(content_widths);
  table::print_data_row<"│^│^│^│^│^│^│^│">(header_names,
                                           content_widths,
                                           table::maybe_plain(opts.colorize, header_colors));
  table::print_edge_row<"├─┼─┼─┼─┼─┼─┼─┤">(content_widths);
  if (opts.single.has_value()) {
    u32 const day{opts.single.value()};
    table::print_data_row<"│^│<│<│>│>│>│>│">(entries[day],
                                             content_widths,
                                             table::maybe_plain(opts.colorize, content_colors));
  } else {
    for (auto const &entry : entries) {
      table::print_data_row<"│^│<│<│>│>│>│>│">(entry,
                                               content_widths,
                                               table::maybe_plain(opts.colorize, content_colors));
    }
    if (opts.timing) {
      table::print_edge_row<"├─┴─┴─┼─┼─┼─┼─┤">(content_widths);
      table::print_data_row<"│^│>│>│>│>│">(summary_data,
                                           summary_widths,
                                           table::maybe_plain(opts.colorize, summary_colors));
      table::print_edge_row<"╰─┴─┴─┴─┴─╯">(summary_widths);
      return;
    }
  }
  table::print_edge_row<"╰─┴─┴─┴─┴─┴─┴─╯">(content_widths);
}

#include <string_view>

#include "chart/calculator.hpp"
#include "chart/chart.hpp"
#include "fmt/color.h"
#include "graph.hpp"
#include "options.hpp"

using std::string_view_literals::operator""sv;

void
graph_output(run_options const &options,
             std::vector<timing_data> const &timing,
             std::vector<report_line> const &entries) noexcept {

  chart::calculator<double, 8> all_width, parse_width, part1_width, part2_width, total_width;

  for (auto const &t : timing) {
    all_width.add_sample(t.parsing);
    all_width.add_sample(t.part1);
    all_width.add_sample(t.part2);
    parse_width.add_sample(t.parsing);
    part1_width.add_sample(t.part1);
    part2_width.add_sample(t.part2);
    total_width.add_sample(t.total());
  }

  u32 width{options.graph_width.value_or(run_options::default_graph_width)};
  u32 const space{9u};

  fmt::print("\n");
  fmt::print("{:<6} {:^{}}\n", "", "Overall Statistics", width + space);
  fmt::print("{:<6} ╭{:─^{}}╮\n", "", "", width + space);
  for (unsigned idx{0}; idx < std::size(timing); ++idx) {
    auto const &curr_time = timing[idx];
    auto const &curr_entry = entries[idx];
    std::string_view const title = curr_entry[0];
    constexpr auto const fmtstr = "{0:<6} │ {1:<7} {2}│\n";

    fmt::print(
        fmtstr,
        (options.part1 and options.part2 ? ""sv : title),
        "Parsing",
        fmt::styled(chart::row(all_width.length_for(curr_time.parsing, width)), fmt::fg(fmt::terminal_color::cyan)));
    if (options.part1) {
      fmt::print(
          fmtstr,
          (options.part2 ? title : ""sv),
          "Part 1",
          fmt::styled(chart::row(all_width.length_for(curr_time.part1, width)), fmt::fg(fmt::terminal_color::red)));
    }
    if (options.part2) {
      fmt::print(
          fmtstr,
          "",
          "Part 2",
          fmt::styled(chart::row(all_width.length_for(curr_time.part2, width)), fmt::fg(fmt::terminal_color::green)));
    }
  }
  fmt::print("{:<6} ╰{:─^{}}╯\n", "", "", width + space);

  fmt::print("\n");
  fmt::print("{:<6} {:^{}}\n", "", "Total", width + 1);
  fmt::print("{:<6} ╭{:─^{}}╮\n", "", "", width + 1);
  for (unsigned idx{0}; idx < std::size(timing); ++idx) {
    auto const &curr_time = timing[idx];
    auto const &curr_entry = entries[idx];
    std::string_view const title = curr_entry[0];
    constexpr auto const fmtstr = "{0:<6} │ {1}│\n";
    fmt::print(fmtstr,
               title,
               fmt::styled(chart::row(total_width.length_for(curr_time.total(), width)),
                           fmt::fg(fmt::terminal_color::yellow)));
  }
  fmt::print("{:<6} ╰{:─^{}}╯\n", "", "", width + 1);

  fmt::print("\n");
  fmt::print("{:<6} {:^{}}\n", "", "Parsing", width + 1);
  fmt::print("{:<6} ╭{:─^{}}╮\n", "", "", width + 1);
  for (unsigned idx{0}; idx < std::size(timing); ++idx) {
    auto const &curr_time = timing[idx];
    auto const &curr_entry = entries[idx];
    std::string_view const title = curr_entry[0];
    constexpr auto const fmtstr = "{0:<6} │ {1}│\n";

    fmt::print(
        fmtstr,
        title,
        fmt::styled(chart::row(parse_width.length_for(curr_time.parsing, width)), fmt::fg(fmt::terminal_color::cyan)));
  }
  fmt::print("{:<6} ╰{:─^{}}╯\n", "", "", width + 1);

  if (options.part1) {
    fmt::print("\n");
    fmt::print("{:<6} {:^{}}\n", "", "Part 1", width + 1);
    fmt::print("{:<6} ╭{:─^{}}╮\n", "", "", width + 1);
    for (unsigned idx{0}; idx < std::size(timing); ++idx) {
      auto const &curr_time = timing[idx];
      auto const &curr_entry = entries[idx];
      std::string_view const title = curr_entry[0];
      constexpr auto const fmtstr = "{0:<6} │ {1}│\n";
      fmt::print(
          fmtstr,
          title,
          fmt::styled(chart::row(part1_width.length_for(curr_time.part1, width)), fmt::fg(fmt::terminal_color::red)));
    }
    fmt::print("{:<6} ╰{:─^{}}╯\n", "", "", width + 1);
  }

  if (options.part2) {
    fmt::print("\n");
    fmt::print("{:<6} {:^{}}\n", "", "Part 2", width + 1);
    fmt::print("{:<6} ╭{:─^{}}╮\n", "", "", width + 1);
    for (unsigned idx{0}; idx < std::size(timing); ++idx) {
      auto const &curr_time = timing[idx];
      auto const &curr_entry = entries[idx];
      std::string_view const title = curr_entry[0];
      constexpr auto const fmtstr = "{0:<6} │ {1}│\n";
      fmt::print(
          fmtstr,
          title,
          fmt::styled(chart::row(part2_width.length_for(curr_time.part1, width)), fmt::fg(fmt::terminal_color::green)));
    }
    fmt::print("{:<6} ╰{:─^{}}╯\n", "", "", width + 1);
  }
}

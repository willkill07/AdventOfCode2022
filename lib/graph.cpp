#include <cstdio>
#include <functional>
#include <string_view>

#include <fmt/color.h>

#include "chart/calculator.hpp"
#include "chart/chart.hpp"
#include "graph.hpp"
#include "options.hpp"
#include "table/formatting.hpp"
#include "timing.hpp"

using std::string_literals::operator""s;

static constexpr auto const faint_cyan{fmt::fg(fmt::terminal_color::cyan) | fmt::emphasis::faint};
static constexpr auto const faint_red{fmt::fg(fmt::terminal_color::red) | fmt::emphasis::faint};
static constexpr auto const faint_yellow{fmt::fg(fmt::terminal_color::yellow) | fmt::emphasis::faint};
static constexpr auto const faint_green{fmt::fg(fmt::terminal_color::green) | fmt::emphasis::faint};

inline bool const is_a_tty{as<bool>(usize(STDOUT_FILENO))};

fmt::text_style
style(fmt::text_style s, bool flag) {
  fmt::text_style style;
  if (is_a_tty and flag) {
    return s;
  }
  return style;
}

void
print_header(std::string const &title, unsigned width) noexcept {
  fmt::print("\n{0:<6} {1:^{2}}\n", "", title, width + 2);
  fmt::print("{0:<6} ╭{0:─^{1}}╮\n", "", width + 2);
}

void
print_footer(unsigned width) noexcept {
  fmt::print("{0:<6} ╰{0:─^{1}}╯\n", "", width + 2);
}

void
print_single(std::string const &header,
             unsigned width,
             run_options const &options,
             std::vector<timing_data> const &timing,
             std::vector<report_line> const &entries,
             fmt::text_style const &the_style,
             auto &&value_getter) noexcept {

  chart::calculator<double, 8> calc;
  for (auto const &t : timing) {
    calc.add_sample(value_getter(t));
  }

  print_header(header, width);
  for (unsigned idx{0}; idx < std::size(timing); ++idx) {
    auto const &curr = timing[idx];
    std::string row = chart::row(calc.length_for(value_getter(curr), width));
    auto const styled = fmt::styled(row, style(the_style, options.colorize));
    fmt::print("{0:<6} │ {1} │\n", entries[idx][0], styled);
  }
  print_footer(width);
}

void
graph_output(run_options const &options,
             std::vector<timing_data> const &timing,
             std::vector<report_line> const &entries) noexcept {

  chart::calculator<double, 8> all_width;
  for (auto const &t : timing) {
    all_width.add_sample(t.parsing);
    all_width.add_sample(t.part1);
    all_width.add_sample(t.part2);
  }

  u32 const width{options.graph_width.value_or(run_options::default_graph_width)};

  fmt::print("\n");
  print_header("Overall Statistics"s, width);
  for (unsigned idx{0}; idx < std::size(timing); ++idx) {
    auto const &curr_time = timing[idx];
    auto const &curr_entry = entries[idx];
    std::string_view const title = curr_entry[0];
    constexpr auto const fmtstr = "{0:<6} │ {1} │\n";
    auto const parsing_bar = chart::row(all_width.length_for(curr_time.parsing, width));

    fmt::print(fmtstr, title, fmt::styled(parsing_bar, style(faint_cyan, options.colorize)));
    if (options.part1) {
      auto const part1_bar = chart::row(all_width.length_for(curr_time.part1, width));
      fmt::print(fmtstr, "", fmt::styled(part1_bar, style(faint_red, options.colorize)));
    }
    if (options.part2) {
      auto const part2_bar = chart::row(all_width.length_for(curr_time.part2, width));
      fmt::print(fmtstr, "", fmt::styled(part2_bar, style(faint_green, options.colorize)));
    }
  }
  print_footer(width);

  print_single("Parse", width, options, timing, entries, faint_cyan, [](auto x) {
    return x.parsing;
  });
  if (options.part1) {
    print_single("Part 1", width, options, timing, entries, faint_red, [](auto x) {
      return x.part1;
    });
  }
  if (options.part2) {
    print_single("Part 2", width, options, timing, entries, faint_green, [](auto x) {
      return x.part2;
    });
  }
  print_single("Total", width, options, timing, entries, faint_yellow, [](auto x) {
    return x.total();
  });
}

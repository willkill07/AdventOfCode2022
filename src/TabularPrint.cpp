#include <array>
#include <functional>
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
print_edge(std::array<sz, Width> const &widths, std::array<bool, Width> const &masks) {
  [&]<sz... I>(std::integer_sequence<sz, I...>) {
    (
        [&] {
          if (masks[I] and widths[I] > 0) {
            fmt::print("{}", String[2 * I]);
            auto const fmt_str = fmt::format("{{:{}^{}}}", String[2 * I + 1], widths[I] + 2);
            fmt::vprint(fmt_str, fmt::make_format_args(""));
          }
        }(),
        ...);
  }
  (std::make_integer_sequence<sz, Width>{});
  fmt::print("{}", String[String.printable_size - 1]);
  fmt::print("\n");
}

template <fixed_string String, sz Width, sz Data, sz Mask, typename T>
void
print_data(std::array<sz, Width> const &widths,
           std::array<T, Data> const &data,
           std::array<bool, Mask> const &masks,
           std::array<char, Mask> const &align,
           std::array<FormatFn<std::string>, Width> const &stylizers) {
  static_assert(Data == Width, "data array does not match width array");
  static_assert(Mask == Width, "mask array does not match width array");
  [&]<sz... I>(std::integer_sequence<sz, I...>) {
    (
        [&] {
          if (masks[I] and widths[I] > 0) {
            fmt::print("{}", String[2 * I]);
            auto const fmt_str = fmt::format("{0}{{:{0}{2}{1}}}{0}", String[2 * I + 1], widths[I], align[I]);
            auto const str = fmt::vformat(fmt_str, fmt::make_format_args(data[I]));
            fmt::print("{}", stylizers[I](str));
          }
        }(),
        ...);
  }
  (std::make_integer_sequence<sz, Width>{});
  fmt::print("{}", String[String.printable_size - 1]);
  fmt::print("\n");
}

template <typename T, typename U, size_t Len>
std::array<sz, report_size>
calculate_widths(std::array<T, report_size> const &header_names,
                 report_data const &entries,
                 std::array<U, Len> const &summary_data,
                 std::array<bool, report_size> const &mask) {

  // initialize to header width
  std::array<sz, report_size> max;
  std::transform(std::begin(header_names),
                 std::end(header_names),
                 std::begin(max),
                 std::mem_fn(&std::string_view::size));

  // adjust for all entries
  for (auto const &entry : entries) {
    for (sz i{0}; i < max.size(); ++i) {
      max[i] = std::max(max[i], entry[i].size());
    }
  }

  // adjust for summary width
  auto const summary_offset = summary_data.size() - 4;
  auto const width_offset = max.size() - 4;
  for (sz base{0}; base < 4; ++base) {
    max[base + width_offset] = std::max(max[base + width_offset], summary_data[base + summary_offset].size());
  }

  // correct for padding/mask
  for (sz idx{0}; idx < report_size; ++idx) {
    if (not mask[idx]) {
      max[idx] = 0;
    }
  }

  return max;
}

void
print_table(run_options const &opts, report_data const &entries, timing_data const &sum) {

  using std::literals::string_literals::operator""s;
  using std::literals::string_view_literals::operator""sv;

  std::array const group_names{""sv, "Answers"sv, "Timing (μs)"sv};
  std::array const
      header_names{"AoC++2022"sv, " Part 1 "sv, " Part 2 "sv, "Parse"sv, "Part 1"sv, "Part 2"sv, "Total"sv};

  std::array const summary_data{opts.format("Summary"s),
                                opts.format(sum.parsing),
                                opts.format(sum.part1),
                                opts.format(sum.part2),
                                opts.format(sum.total())};

  std::array const header_mask{true, true, opts.timing};
  std::array const
      content_mask{true, true, opts.part2, opts.timing, opts.timing, opts.part2 && opts.timing, opts.timing};
  std::array const summary_mask{true, opts.timing, opts.timing, opts.timing, opts.timing};

  std::array const max_widths = calculate_widths(header_names, entries, summary_data, content_mask);

  auto width = [&](auto... inds) -> sz {
    auto const sum = (max_widths[static_cast<sz>(inds)] + ...);
    auto const count = ((2 + content_mask[static_cast<sz>(inds)]) + ...) - 2;
    if (count > 0) {
      return sum + static_cast<sz>(count - 1);
    } else {
      return 0;
    }
  };

  std::array const header_widths{width(0), width(1, 2), width(3, 4, 5, 6)};

  std::array const summary_widths{width(0, 1, 2), width(3), width(4), width(5), width(6)};

  std::array const group_align{'^', '^', '^'};
  std::array const header_align{'^', '^', '^', '^', '^', '^', '^'};
  std::array const data_align{'^', '<', '<', '>', '>', '>', '>'};
  std::array const summary_align{'^', '>', '>', '>', '>'};

  print_edge<"  ╭─┬─╮">(header_widths, header_mask);
  print_data<"  │ │ │">(header_widths, group_names, header_mask, group_align, colors::group);
  print_edge<"╭─┼─┬─┼─┬─┬─┬─┤">(max_widths, content_mask);
  print_data<"│ │ │ │ │ │ │ │">(max_widths, header_names, content_mask, header_align, colors::header);
  print_edge<"├─┼─┼─┼─┼─┼─┼─┤">(max_widths, content_mask);
  if (opts.single.has_value()) {
    print_data<"│ │ │ │ │ │ │ │">(max_widths, entries[opts.single.value()], content_mask, data_align, colors::content);
  } else {
    for (auto const &entry : entries) {
      print_data<"│ │ │ │ │ │ │ │">(max_widths, entry, content_mask, data_align, colors::content);
    }
    if (opts.timing) {
      print_edge<"├─┴─┴─┼─┼─┼─┼─┤">(max_widths, content_mask);
      print_data<"│ │ │ │ │ │">(summary_widths, summary_data, summary_mask, summary_align, colors::summary);
      print_edge<"╰─┴─┴─┴─┴─╯">(summary_widths, summary_mask);
      return;
    }
  }
  print_edge<"╰─┴─┴─┴─┴─┴─┴─╯">(max_widths, content_mask);
}

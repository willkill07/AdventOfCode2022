#include "options.hpp"

[[nodiscard]] bool
run_options::validate() const noexcept {
  bool valid{true};
  if (not(part1 or part2)) {
    (void)fprintf(stderr, "Cannot suppress both parts\n");
    valid = false;
  }
  if (not timing) {
    if (precision.has_value()) {
      (void)fprintf(stderr, "Cannot specify precision when not timing\n");
      valid = false;
    }
    if (benchmark) {
      (void)fprintf(stderr, "Cannot benchmark when not timing\n");
      valid = false;
    }
    if (mask) {
      (void)fprintf(stderr, "Cannot mask answers when not timing\n");
      valid = false;
    }
  }
  if (not answers) {
    if (mask) {
      (void)fprintf(stderr, "Cannot mask answers when not printing solutions\n");
      valid = false;
    }
    if (not timing) {
      (void)fprintf(stderr, "Cannot suppress answers and timing\n");
      valid = false;
    }
  }
  if (not graphs and graph_width.has_value()) {
    (void)fprintf(stderr, "Cannot specify graph width when graph output is disabled\n");
    valid = false;
  }
  if (graphs and single.has_value()) {
    (void)fprintf(stderr, "Cannot specify execution of single day with graph output\n");
    valid = false;
  }
  if (visual and single.has_value()) {
    (void)fprintf(stderr, "Cannot specify execution of single day with visual timing\n");
    valid = false;
  }
  return valid;
}

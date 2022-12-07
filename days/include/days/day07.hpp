#include <memory>
#include <vector>

#include "days/day.hpp"

namespace day07 {

struct dir {
  std::string_view name;
  dir *parent{nullptr};
  std::vector<dir *> dirs;
  u32 size{0};

  explicit dir(std::string_view n, dir *p = nullptr) noexcept
      : name{n},
        parent{p} {
  }

  inline ~dir() noexcept {
    for (auto *d : dirs) {
      delete d;
    }
  }
};

} // namespace day07

using Day07 = Day<7, std::unique_ptr<day07::dir>, u32>;

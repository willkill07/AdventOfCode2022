#pragma once

#include <span>
#include <string>

#include "types.hpp"

class file_backed_buffer {
  usize const buffer_length;
  int const file_desc;
  char const *const buffer_address;

public:
  file_backed_buffer(std::string const &filename) noexcept;

  ~file_backed_buffer() noexcept;

  operator bool() const noexcept;

  [[nodiscard]] std::span<char const> get_span() const noexcept;

  [[nodiscard]] std::string_view get_string_view() const noexcept;
};
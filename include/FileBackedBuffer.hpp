#pragma once

#include <span>
#include <string>

#include "Types.hpp"

class file_backed_buffer {
  sz const buffer_length;
  int const file_desc;
  char const *const buffer_address;

public:
  file_backed_buffer(std::string const &filename) noexcept;

  ~file_backed_buffer() noexcept;

  operator bool() const noexcept;

  std::span<char const> get_span() const noexcept;

  std::string_view get_string_view() const noexcept;
};
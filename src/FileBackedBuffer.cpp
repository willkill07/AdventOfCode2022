#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "FileBackedBuffer.hpp"

file_backed_buffer::file_backed_buffer(std::string const& filename) noexcept
    : buffer_length([&]() noexcept -> sz {
        struct stat st;
        int const   res{stat(filename.c_str(), &st)};
        if (res < 0) {
          return sz{0};
        } else {
          return static_cast<sz>(st.st_size);
        }
      }()),
      file_desc([&]() noexcept -> int {
        return open(filename.c_str(), O_RDONLY);
      }()),
      buffer_address([&]() noexcept -> char const* {
        if (file_desc < 0) {
          return nullptr;
        } else {
          return reinterpret_cast<char const*>(mmap(NULL, buffer_length, PROT_READ, MAP_PRIVATE, file_desc, 0));
        }
      }()) {
}

file_backed_buffer::~file_backed_buffer() noexcept {
  if (buffer_address != nullptr) {
    munmap(const_cast<void*>(reinterpret_cast<void const*>(buffer_address)), buffer_length);
  }
  if (file_desc >= 0) {
    (void)close(file_desc);
  }
}

file_backed_buffer::operator bool() const noexcept {
  return (buffer_address != nullptr) and not(file_desc < 0);
}

std::span<char const> file_backed_buffer::get_span() const noexcept {
  return {buffer_address, buffer_length};
}

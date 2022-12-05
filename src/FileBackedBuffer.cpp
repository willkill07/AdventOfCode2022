#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "FileBackedBuffer.hpp"

file_backed_buffer::file_backed_buffer(std::string const &filename) noexcept
    : buffer_length([](std::string const &file) noexcept -> sz {
        struct stat st;
        int const res{stat(file.c_str(), &st)};
        if (res < 0) {
          return sz{0};
        } else {
          return static_cast<sz>(st.st_size);
        }
      }(filename)),
      file_desc([](std::string const &file) noexcept -> int {
        return open(file.c_str(), O_RDONLY);
      }(filename)),
      buffer_address([](int fd, sz len) noexcept -> char const * {
        if (fd < 0) {
          return nullptr;
        } else {
          return reinterpret_cast<char const *>(mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd, 0));
        }
      }(file_desc, buffer_length)) {
}

file_backed_buffer::~file_backed_buffer() noexcept {
  if (buffer_address != nullptr) {
    munmap(const_cast<void *>(reinterpret_cast<void const *>(buffer_address)), buffer_length);
  }
  if (file_desc >= 0) {
    (void)close(file_desc);
  }
}

file_backed_buffer::operator bool() const noexcept {
  return (buffer_address != nullptr) and not(file_desc < 0);
}

std::span<char const>
file_backed_buffer::get_span() const noexcept {
  return {buffer_address, buffer_length};
}

std::string_view
file_backed_buffer::get_string_view() const noexcept {
  return {buffer_address, buffer_length};
}

#pragma once

#include <span>
#include <string_view>
#include <vector>

#include "point2d.hpp"
#include "types.hpp"

template <typename T, typename Storage = std::vector<T>>
struct offset_grid {

  using iterator = typename Storage::iterator;
  using const_iterator = typename Storage::const_iterator;
  using pointer = typename Storage::pointer;
  using const_pointer = typename Storage::const_pointer;
  using reference = typename Storage::reference;
  using const_reference = typename Storage::const_reference;
  using value_type = typename Storage::value_type;
  
  constexpr inline offset_grid(i32 x_min, i32 x_max, i32 y_min, i32 y_max) noexcept 
      : m_width{x_max - x_min + 1},
        m_height{y_max - y_min + 1},
        m_xmin{x_min},
        m_ymin{y_min},
        m_data(static_cast<u32>(m_width) * static_cast<u32>(m_height), 0) {
  }

  constexpr inline reference operator()(i32 x, i32 y) noexcept {
    return m_data[static_cast<u32>(m_width * (y - m_ymin) + (x - m_xmin))];
  }

  constexpr inline const_reference operator()(i32 x, i32 y) const noexcept {
    return m_data[static_cast<u32>(m_width * (y - m_ymin) + (x - m_xmin))];
  }

  constexpr inline reference operator()(point2d const& p) noexcept {
    return (*this)(p.x, p.y);
  }

  constexpr inline const_reference operator()(point2d const& p) const noexcept {
    return (*this)(p.x, p.y);
  }

  constexpr inline iterator begin() noexcept {
    return m_data.begin();
  }

  constexpr inline const_iterator begin() const noexcept {
    return m_data.begin();
  }

  constexpr inline const_iterator cbegin() const noexcept {
    return m_data.cbegin();
  }

  constexpr inline iterator end() noexcept {
    return m_data.end();
  }

  constexpr inline const_iterator end() const noexcept {
    return m_data.end();
  }

  constexpr inline const_iterator cend() const noexcept {
    return m_data.cend();
  }

  constexpr inline usize size() const noexcept {
    return m_data.size();
  }

  constexpr inline pointer data() noexcept {
    return m_data.data();
  }

  constexpr inline const_pointer data() const noexcept {
    return m_data.data();
  }

  constexpr inline i32 xmin() const noexcept {
    return m_xmin;
  }

  constexpr inline i32 xmax() const noexcept {
    return m_xmin + m_width - 1;
  }

  constexpr inline i32 ymin() const noexcept {
    return m_ymin;
  }
  
  constexpr inline i32 ymax() const noexcept {
    return m_ymin + m_height - 1;
  }

  constexpr inline u32 width() const noexcept {
    return static_cast<u32>(m_width);
  }

  constexpr inline u32 height() const noexcept {
    return static_cast<u32>(m_height);
  }
  
  constexpr inline std::string_view row(u32 row_num) const noexcept requires std::same_as<T, char> {
    return std::string_view{data() + row_num * width(), width()};
  }

private:
  i32 m_width, m_height, m_xmin, m_ymin;
  Storage m_data;
};

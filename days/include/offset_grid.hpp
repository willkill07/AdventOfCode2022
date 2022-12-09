#pragma once

#include <vector>

#include "point2d.hpp"
#include "types.hpp"

template <typename T>
struct offset_grid {

  using iterator = typename std::vector<T>::iterator;
  using const_iterator = typename std::vector<T>::const_iterator;
  using pointer = typename std::vector<T>::pointer;
  using const_pointer = typename std::vector<T>::const_pointer;
  using reference = typename std::vector<T>::reference;
  using const_reference = typename std::vector<T>::const_reference;
  using value_type = typename std::vector<T>::value_type;

  offset_grid() = delete;
  
  constexpr offset_grid(i32 x_min, i32 x_max, i32 y_min, i32 y_max)
      : width{x_max - x_min + 1},
        height{y_max - y_min + 1},
        minx{x_min},
        miny{y_min},
        m_data(static_cast<usize>(width) * static_cast<usize>(height)) {
  }

  reference operator()(i32 x, i32 y) noexcept {
    return m_data[static_cast<usize>(width * (y - miny) + (x - minx))];
  }

  const_reference operator()(i32 x, i32 y) const noexcept {
    return m_data[static_cast<usize>(width * (y - miny) + (x - minx))];
  }

  reference operator()(point2d const& p) noexcept {
    return (*this)(p.x, p.y);
  }

  const_reference operator()(point2d const& p) const noexcept {
    return (*this)(p.x, p.y);
  }

  constexpr iterator begin() {
    return m_data.begin();
  }

  constexpr const_iterator begin() const {
    return m_data.begin();
  }

  constexpr const_iterator cbegin() const {
    return m_data.cbegin();
  }

  constexpr iterator end() {
    return m_data.end();
  }

  constexpr const_iterator end() const {
    return m_data.end();
  }

  constexpr const_iterator cend() const {
    return m_data.cend();
  }

  constexpr usize size() const {
    return m_data.size();
  }

  constexpr pointer data() {
    return m_data.data();
  }

  constexpr const_pointer data() const {
    return m_data.data();
  }

private:
  i32 width, height, minx, miny;
  std::vector<T> m_data;
};
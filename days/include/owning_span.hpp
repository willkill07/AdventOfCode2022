#pragma once

#include <algorithm>
#include <array>
#include <span>

#include "types.hpp"

template <typename T, usize N>
struct owning_span {

  using iterator = typename std::array<T, N>::iterator;
  using const_iterator = typename std::array<T, N>::const_iterator;
  using pointer = typename std::array<T, N>::pointer;
  using const_pointer = typename std::array<T, N>::const_pointer;
  using reference = typename std::array<T, N>::reference;
  using const_reference = typename std::array<T, N>::const_reference;
  using value_type = typename std::array<T, N>::value_type;

  constexpr inline owning_span() noexcept = default;

  constexpr inline owning_span(u32 the_size) noexcept
      : m_size{the_size} {
  }

  constexpr inline owning_span(u32 the_size, T const &value) noexcept
      : owning_span{the_size} {
    std::fill(begin(), end(), value);
  }

  constexpr inline auto span() noexcept {
    return std::span{std::data(m_data), m_size};
  }

  constexpr inline auto span() const noexcept {
    return std::span{std::data(m_data), m_size};
  }

  constexpr inline void push(T &&v) noexcept {
    m_data[m_size++] = std::move(v);
  }

  constexpr inline void push(T const &v) noexcept {
    m_data[m_size++] = v;
  }

  template <std::input_iterator I>
  constexpr inline void push(I begin, I end) noexcept {
    std::copy(begin, end, std::end(span()));
    m_size += as<u32>(std::distance(begin, end));
  }

  template <std::input_iterator I>
  constexpr inline void push(I begin, u32 count) noexcept {
    std::copy_n(begin, count, std::end(span()));
    m_size += count;
  }

  constexpr inline T &top() noexcept {
    return m_data[m_size - 1];
  }

  constexpr inline T top() const noexcept {
    return m_data[m_size - 1];
  }

  constexpr inline auto top(u32 count) noexcept {
    return std::span(std::data(m_data) + m_size - count, count);
  }

  constexpr inline auto top(u32 count) const noexcept {
    return std::span(std::data(m_data) + m_size - count, count);
  }

  constexpr inline void pop() noexcept {
    --m_size;
  }

  constexpr inline void pop(u32 count) noexcept {
    m_size -= count;
  }

  constexpr inline T &operator[](u32 idx) noexcept {
    return m_data[idx];
  }

  constexpr inline T const &operator[](u32 idx) const noexcept {
    return m_data[idx];
  }

  constexpr inline void clear() noexcept {
    m_size = 0;
  }

  constexpr inline void resize(u32 sz) noexcept {
    m_size = sz;
  }

  constexpr inline auto size() const noexcept {
    return m_size;
  }

  constexpr inline auto data() noexcept {
    return std::data(m_data);
  }

  constexpr inline auto data() const noexcept {
    return std::data(m_data);
  }

  constexpr inline auto begin() noexcept {
    return std::begin(m_data);
  }

  constexpr inline auto begin() const noexcept {
    return std::begin(m_data);
  }

  constexpr inline auto cbegin() const noexcept {
    return std::begin(m_data);
  }

  constexpr inline auto end() noexcept {
    return std::begin(m_data) + m_size;
  }

  constexpr inline auto end() const noexcept {
    return std::begin(m_data) + m_size;
  }

  constexpr inline auto cend() const noexcept {
    return std::begin(m_data) + m_size;
  }

  constexpr inline auto rbegin() noexcept {
    return std::reverse_iterator(end());
  }

  constexpr inline auto rbegin() const noexcept {
    return std::reverse_iterator(end());
  }

  constexpr inline auto crbegin() const noexcept {
    return std::reverse_iterator(end());
  }

  constexpr inline auto rend() noexcept {
    return std::reverse_iterator(begin());
  }

  constexpr inline auto rend() const noexcept {
    return std::reverse_iterator(begin());
  }

  constexpr inline auto crend() const noexcept {
    return std::reverse_iterator(begin());
  }

private:
  alignas(64) std::array<T, N> m_data;
  u32 m_size{0};
};

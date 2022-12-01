#pragma once

#include <cstdint>
#include <type_traits>

using i32 = std::int32_t;
using u32 = std::uint32_t;
using i64 = std::int64_t;
using u64 = std::uint64_t;
using sz = std::size_t;

template <i32 Value> using const_i32 = std::integral_constant<i32, Value>;
template <u32 Value> using const_u32 = std::integral_constant<u32, Value>;
template <i64 Value> using const_i64 = std::integral_constant<i64, Value>;
template <u64 Value> using const_u64 = std::integral_constant<u64, Value>;
template <sz Value> using const_sz = std::integral_constant<sz, Value>;
template <bool Value> using const_bool = std::integral_constant<bool, Value>;

constexpr inline const_bool<true> true_v {};
constexpr inline const_bool<false> false_v {};

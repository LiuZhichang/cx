/**
 * @file vector2.h
 * @author liuzhichang (lzc3318619633@163.com)
 * @brief 二维向量类
 * @version 0.1
 * @date 2022-05-10
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

#include <cx/common/internal.h>

#include <cstdint>

namespace cx {

template <typename T>
class Vector3;

/**
 * @brief 二维向量类，用于数学计算
 *
 * @tparam T
 */
template <typename T>
class Vector2 {
 public:
  CX_CONSTEXPR Vector2() = default;
  CX_CONSTEXPR Vector2(const T& v);
  CX_CONSTEXPR Vector2(const T& x, const T& y);
  CX_CONSTEXPR Vector2(const Vector2<T>& vec);

  template <typename M, typename N>
  CX_CONSTEXPR Vector2(const M& x, const N& y);

  template <typename M>
  CX_CONSTEXPR Vector2(const Vector2<M>& vec);

  template <typename M>
  CX_CONSTEXPR Vector2(const Vector3<M>& vec);

  template <typename M>
  CX_CONSTEXPR auto add(const Vector2<M>& vec) const;

  template <typename M>
  CX_CONSTEXPR auto sub(const Vector2<M>& vec) const;

  template <typename M>
  CX_CONSTEXPR auto multiply(const Vector2<M>& vec) const;

  template <typename M>
  CX_CONSTEXPR auto divide(const Vector2<M>& vec) const;

  template <typename M>
  CX_CONSTEXPR auto angle(const Vector2<M>& vec) const;

  template <typename M>
  CX_CONSTEXPR auto dot(const Vector2<M>& vec) const;

  template <typename M, typename N = float>
  CX_CONSTEXPR auto lerp(const Vector2<M>& vec, const N& progression) const;

  template <typename M = float>
  CX_CONSTEXPR auto scale(const M& scalar) const;

  template <typename M = float>
  auto rotate(const M& angle) const;

  template <typename M, typename N>
  auto rotate(const M& angle, const Vector2<N>& rotation_axis) const;

  auto normalize() const;

  CX_CONSTEXPR auto length_squared() const;

  auto length() const;

  auto abs() const;

  CX_CONSTEXPR auto min() const;

  CX_CONSTEXPR auto max() const;

  CX_CONSTEXPR auto min_max() const;

  template <typename M>
  CX_CONSTEXPR auto min(const Vector2<M>& vec);

  template <typename M>
  CX_CONSTEXPR auto max(const Vector2<M>& vec);

  template <typename M>
  CX_CONSTEXPR auto distance_squard(const Vector2<M>& vec) const;

  template <typename M>
  auto distance(const Vector2<M>& vec) const;

  template <typename M>
  CX_CONSTEXPR auto distance_vector(const Vector2<M>& vec) const;

  template <typename M>
  CX_CONSTEXPR bool in_triangle(const Vector2<M>& vec1, const Vector2<M>& vec2,
                                const Vector2<M>& vec3) const;

  template <typename M>
  CX_CONSTEXPR Vector2& operator+=(const Vector2<M>& vec);
  template <typename M>
  CX_CONSTEXPR Vector2& operator-=(const Vector2<M>& vec);
  template <typename M>
  CX_CONSTEXPR Vector2& operator*=(const Vector2<M>& vec);
  template <typename M>
  CX_CONSTEXPR Vector2& operator/=(const Vector2<T>& vec);
  CX_CONSTEXPR Vector2& operator+=(const Vector2<T>& vec);
  CX_CONSTEXPR Vector2& operator-=(const Vector2<T>& vec);
  CX_CONSTEXPR Vector2& operator*=(const Vector2<T>& vec);
  CX_CONSTEXPR Vector2& operator/=(const Vector2<T>& vec);

  CX_CONSTEXPR bool operator==(const Vector2<T>& vec);
  CX_CONSTEXPR Vector2<T>& operator=(const Vector2<T>& vec);

  static const Vector2 Zero;
  static const Vector2 One;
  static const Vector2 Infinity;
  static const Vector2 Left;
  static const Vector2 Right;
  static const Vector2 Up;
  static const Vector2 Down;

  T x = 0, y = 0;
  T& w = x;
  T& h = y;
};

using Vector2f = Vector2<float>;
using Vector2d = Vector2<double>;
using Vector2i = Vector2<int32_t>;
using Vector2ui = Vector2<uint32_t>;
using Vector2us = Vector2<uint16_t>;

}  // namespace cx

#include "vector2.inl"
#pragma once

#include <complex>
#include <stdexcept>

#include "vector2.h"

namespace cx {

template <typename T>
CX_CONSTEXPR Vector2<T>::Vector2(const T& v) : x(v), y(v) {}

template <typename T>
CX_CONSTEXPR Vector2<T>::Vector2(const T& x, const T& y) : x(x), y(y) {}

template <typename T>
template <typename M, typename N>
CX_CONSTEXPR Vector2<T>::Vector2(const M& x, const N& y)
    : x(static_cast<T>(x)), y(static_cast<T>(y)) {}

template <typename T>
CX_CONSTEXPR Vector2<T>::Vector2(const Vector2<T>& vec) {
  *this = vec;
}

template <typename T>
template <typename M>
CX_CONSTEXPR Vector2<T>::Vector2(const Vector2<M>& vec)
    : x(static_cast<T>(vec.x)), y(static_cast<T>(vec.y)) {}

template <typename T>
template <typename M>
CX_CONSTEXPR Vector2<T>::Vector2(const Vector3<M>& vec)
    : x(static_cast<T>(vec.x)), y(static_cast<T>(vec.y)) {}

template <typename T>
template <typename M>
CX_CONSTEXPR auto Vector2<T>::add(const Vector2<M>& other) const {
  return Vector2<decltype(x + other.x)>(x + other.x, y + other.y);
}

template <typename T>
template <typename M>
CX_CONSTEXPR auto Vector2<T>::sub(const Vector2<M>& other) const {
  return Vector2<decltype(x - other.x)>(x - other.x, y - other.y);
}

template <typename T>
template <typename M>
CX_CONSTEXPR auto Vector2<T>::multiply(const Vector2<M>& other) const {
  return Vector2<decltype(x * other.x)>(x * other.x, y * other.y);
}

template <typename T>
template <typename M>
CX_CONSTEXPR auto Vector2<T>::divide(const Vector2<M>& other) const {
  return Vector2<decltype(x / other.x)>(x / other.x, y / other.y);
}

template <typename T>
template <typename M>
CX_CONSTEXPR auto Vector2<T>::angle(const Vector2<M>& other) const {
  auto dls = Dot(other) / (length() * other.length());

  if (dls < -1) {
    dls = -1;
  } else if (dls > 1) {
    dls = 1;
  }

  return std::acos(dls);
}
template <typename T>
template <typename M>
CX_CONSTEXPR auto Vector2<T>::dot(const Vector2<M>& other) const {
  return x * other.x + y * other.y;
}

template <typename T>
template <typename M, typename N>
CX_CONSTEXPR auto Vector2<T>::lerp(const Vector2<M>& other,
                                   const N& progression) const {
  auto ta = *this * (1 - progression);
  auto tb = other * progression;
  return ta + tb;
}

template <typename T>
template <typename M>
CX_CONSTEXPR auto Vector2<T>::scale(const M& scalar) const {
  return Vector2<decltype(x * scalar)>(x * scalar, y * scalar);
}

template <typename T>
template <typename M>
auto Vector2<T>::rotate(const M& angle) const {
  return Vector2<decltype(x * angle)>(
      x * std::cos(angle) - y * std::sin(angle),
      x * std::sin(angle) + y * std::cos(angle));
}

template <typename T>
template <typename M, typename N>
auto Vector2<T>::rotate(const M& angle, const Vector2<N>& rotation_axis) const {
  auto x1 = ((x - rotation_axis.x) * std::cos(angle)) -
            ((y - rotation_axis.y) * std::sin(angle) + rotation_axis.x);
  auto y1 = ((x - rotation_axis.x) * std::sin(angle)) +
            ((y - rotation_axis.y) * std::cos(angle) + rotation_axis.y);
  return Vector2<decltype(x1)>(x1, y1);
}

template <typename T>
auto Vector2<T>::normalize() const {
  auto l = length();

  if (l == 0) {
    throw std::logic_error("can't normalize a zero length vector");
  }

  return *this / l;
}

template <typename T>
CX_CONSTEXPR auto Vector2<T>::length_squared() const {
  return x * x + y * y;
}
template <typename T>
auto Vector2<T>::length() const {
  return std::sqrt(length_squared());
}

template <typename T>
CX_CONSTEXPR Vector2<T>& cx::Vector2<T>::operator=(const Vector2<T>& vec) {
  x = vec.x;
  y = vec.y;

  return *this;
}

}  // namespace cx
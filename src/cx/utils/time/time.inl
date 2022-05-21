#pragma once

#include "time.h"

namespace cx::time {

constexpr bool TimePoint::operator==(const TimePoint &rhs) const {
  return value == rhs.value;
}

constexpr bool TimePoint::operator!=(const TimePoint &rhs) const {
  return value != rhs.value;
}

constexpr bool TimePoint::operator<(const TimePoint &rhs) const {
  return value < rhs.value;
}

constexpr bool TimePoint::operator<=(const TimePoint &rhs) const {
  return value <= rhs.value;
}

constexpr bool TimePoint::operator>(const TimePoint &rhs) const {
  return value > rhs.value;
}

constexpr bool TimePoint::operator>=(const TimePoint &rhs) const {
  return value >= rhs.value;
}

constexpr TimePoint TimePoint::operator-() const { return TimePoint(-value); }

constexpr TimePoint operator+(const TimePoint &lhs, const TimePoint &rhs) {
  return lhs.value + rhs.value;
}

constexpr TimePoint operator-(const TimePoint &lhs, const TimePoint &rhs) {
  return lhs.value - rhs.value;
}

constexpr TimePoint operator*(const TimePoint &lhs, float rhs) {
  return lhs.value * rhs;
}

constexpr TimePoint operator*(const TimePoint &lhs, int64_t rhs) {
  return lhs.value * rhs;
}

constexpr TimePoint operator*(float lhs, const TimePoint &rhs) {
  return rhs * lhs;
}

constexpr TimePoint operator*(int64_t lhs, const TimePoint &rhs) {
  return rhs * lhs;
}

constexpr TimePoint operator/(const TimePoint &lhs, float rhs) {
  return lhs.value / rhs;
}

constexpr TimePoint operator/(const TimePoint &lhs, int64_t rhs) {
  return lhs.value / rhs;
}

constexpr double operator/(const TimePoint &lhs, const TimePoint &rhs) {
  return static_cast<double>(lhs.value.count()) /
         static_cast<double>(rhs.value.count());
}

constexpr TimePoint &TimePoint::operator+=(const TimePoint &rhs) {
  return *this = *this + rhs;
}

constexpr TimePoint &TimePoint::operator-=(const TimePoint &rhs) {
  return *this = *this - rhs;
}

constexpr TimePoint &TimePoint::operator*=(float rhs) {
  return *this = *this * rhs;
}

constexpr TimePoint &TimePoint::operator*=(int64_t rhs) {
  return *this = *this * rhs;
}

constexpr TimePoint &TimePoint::operator/=(float rhs) {
  return *this = *this / rhs;
}

constexpr TimePoint &TimePoint::operator/=(int64_t rhs) {
  return *this = *this / rhs;
}
}  // namespace cx::time

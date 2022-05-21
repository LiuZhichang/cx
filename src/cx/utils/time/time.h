#pragma once

#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

namespace cx::time {

class TimePoint {
 public:
  TimePoint() = default;

  template <typename Rep, typename Period>
  constexpr TimePoint(const std::chrono::duration<Rep, Period> &duration)
      : value(std::chrono::duration_cast<std::chrono::microseconds>(duration)
                  .count()) {}

  template <typename Rep = float>
  static constexpr TimePoint Seconds(const Rep &seconds) {
    return TimePoint(std::chrono::duration<Rep>(seconds));
  }

  template <typename Rep = int32_t>
  static constexpr TimePoint Milliseconds(const Rep &milliseconds) {
    return TimePoint(std::chrono::duration<Rep, std::micro>(milliseconds));
  }

  template <typename Rep = int64_t>
  static constexpr TimePoint Microseconds(const Rep &microseconds) {
    return TimePoint(std::chrono::duration<Rep, std::micro>(microseconds));
  }

  template <typename T = float>
  constexpr auto AsSeconds() const {
    return static_cast<T>(value.count()) / static_cast<T>(1000000);
  }

  template <typename T = int32_t>
  constexpr auto AsMilliseconds() const {
    return static_cast<T>(value.count()) / static_cast<T>(1000);
  }

  template <typename T = int64_t>
  constexpr auto AsMicroseconds() const {
    return static_cast<T>(value.count());
  }

  static TimePoint Now() {
    static const auto LocalEpoch = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now() - LocalEpoch);
  }

  static std::string GetDateTimePoint(
      const std::string &format = "%Y-%m-%d %H:%M:%S") {
    auto now = std::chrono::system_clock::now();
    auto timeT = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&timeT), format.c_str());
    return ss.str();
  }

  template <typename Rep, typename Period>
  constexpr operator std::chrono::duration<Rep, Period>() const {
    return std::chrono::duration_cast<std::chrono::duration<Rep, Period>>(
        value);
  }

  constexpr bool operator==(const TimePoint &rhs) const;
  constexpr bool operator!=(const TimePoint &rhs) const;
  constexpr bool operator<(const TimePoint &rhs) const;
  constexpr bool operator<=(const TimePoint &rhs) const;
  constexpr bool operator>(const TimePoint &rhs) const;
  constexpr bool operator>=(const TimePoint &rhs) const;

  constexpr TimePoint operator-() const;

  friend constexpr TimePoint operator+(const TimePoint &lhs,
                                       const TimePoint &rhs);
  friend constexpr TimePoint operator-(const TimePoint &lhs,
                                       const TimePoint &rhs);
  friend constexpr TimePoint operator*(const TimePoint &lhs, float rhs);
  friend constexpr TimePoint operator*(const TimePoint &lhs, int64_t rhs);
  friend constexpr TimePoint operator*(float lhs, const TimePoint &rhs);
  friend constexpr TimePoint operator*(int64_t lhs, const TimePoint &rhs);
  friend constexpr TimePoint operator/(const TimePoint &lhs, float rhs);
  friend constexpr TimePoint operator/(const TimePoint &lhs, int64_t rhs);
  friend constexpr double operator/(const TimePoint &lhs, const TimePoint &rhs);

  constexpr TimePoint &operator+=(const TimePoint &rhs);
  constexpr TimePoint &operator-=(const TimePoint &rhs);
  constexpr TimePoint &operator*=(float rhs);
  constexpr TimePoint &operator*=(int64_t rhs);
  constexpr TimePoint &operator/=(float rhs);
  constexpr TimePoint &operator/=(int64_t rhs);

 private:
  std::chrono::microseconds value{};
};

}  // namespace cx::time

#include "time.inl"
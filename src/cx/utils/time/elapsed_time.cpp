#include "elapsed_time.h"

#include <cmath>

namespace cx::time {

ElapsedTime::ElapsedTime(const TimePoint& time_point)
    : m_start_time(TimePoint::Now()), m_interval(time_point) {}

uint32_t ElapsedTime::elpased() {
  TimePoint now = TimePoint::Now();
  uint32_t elpased =
      static_cast<uint32_t>(std::floor((now - m_start_time) / m_interval));

  return elpased;
}

}  // namespace cx::time
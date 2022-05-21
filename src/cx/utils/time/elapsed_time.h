#pragma once

#include "cx/utils/time/time.h"

namespace cx::time {

class ElapsedTime {
 public:
  explicit ElapsedTime(const TimePoint& time_point = std::chrono::seconds(-1));

  uint32_t elpased();
  const TimePoint& start_time() const { return m_start_time; }
  void set_start_time(const TimePoint& time_point) {
    m_start_time = time_point;
  }

  const TimePoint& interval() const { return m_interval; }
  void set_interval(const TimePoint& time_point) { m_interval = time_point; }

 private:
  TimePoint m_start_time;
  TimePoint m_interval;
};

}  // namespace cx::time
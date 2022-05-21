#pragma once

#include "cx/utils/time/elapsed_time.h"

namespace cx::time {

class Delta {
 public:
  void update() {
    m_current_frame_time = TimePoint::Now();
    m_change = m_current_frame_time - m_last_frame_time;
    m_last_frame_time = m_current_frame_time;
  }

  TimePoint m_current_frame_time;
  TimePoint m_last_frame_time;
  TimePoint m_change;
};

class ChangePerSecond {
 public:
  void update(const TimePoint &time) {
    m_value_temp++;

    if (std::floor(time.AsSeconds()) > std::floor(m_value_time.AsSeconds())) {
      m_value = m_value_temp;
      m_value_temp = 0;
    }

    m_value_time = time;
  }

  uint32_t m_value_temp = 0, m_value = 0;
  TimePoint m_value_time;
};
}  // namespace cx::time
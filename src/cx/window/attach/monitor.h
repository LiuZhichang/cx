#pragma once

#include <cx/maths/vector2.h>

struct GLFWmonitor;
namespace cx {

class Monitor {
 public:
  bool is_primary();

  GLFWmonitor* handle();

  Vector2ui workarea_size();

 private:
  GLFWmonitor* m_monitor;
};

}  // namespace cx
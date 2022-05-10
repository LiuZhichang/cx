#include "monitor.h"

#include <GLFW/glfw3.h>

bool cx::Monitor::is_primary() {
  return (m_monitor == glfwGetPrimaryMonitor());
};

GLFWmonitor* cx::Monitor::handle() { return m_monitor; }
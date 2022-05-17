#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>

namespace cx::graphics {

class Instance;
class LogicalDevice {
 public:
  static const std::vector<const char *> GetExtensions() {
    return s_device_extensions;
  };

 private:
  static const std::vector<const char *> s_device_extensions;
};

}  // namespace cx::graphics
#pragma once

#include <vulkan/vulkan.hpp>

namespace cx::graphics {

class Instance;
class PhysicalDevice;
class Surface {
 public:
  Surface(const Instance* instance, const PhysicalDevice* physical_device);
  ~Surface();

  operator const vk::SurfaceKHR&() const { return m_surface; }

  const vk::SurfaceKHR& handle() const { return m_surface; }

  const vk::SurfaceCapabilitiesKHR& capabilities() const {
    return m_capabilities;
  }

  const vk::SurfaceFormatKHR& format() const { return m_format; }

 private:
  const Instance* m_instance;
  vk::SurfaceKHR m_surface;
  vk::SurfaceCapabilitiesKHR m_capabilities;
  vk::SurfaceFormatKHR m_format;
};

}  // namespace cx::graphics
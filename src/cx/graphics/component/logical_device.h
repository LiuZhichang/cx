#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>

namespace cx::graphics {

enum class QueueType { eGraphics, ePresent, eCompute, eTransfer };

class Instance;
class PhysicalDevice;
class Surface;
class LogicalDevice {
 public:
  LogicalDevice(const Instance *instance, const PhysicalDevice *physical_device,
                const Surface *surface);

  ~LogicalDevice();

  static const std::vector<const char *> GetExtensions() {
    return s_device_extensions;
  };

  const vk::Device &handle() const { return m_device; }

  operator const vk::Device &() const { return m_device; }

  const vk::PhysicalDeviceFeatures &enable_features() const {
    return m_enable_features;
  }

  const vk::Queue &graphics_queue() const { return m_graphics_queue; }

  const vk::Queue &present_queue() const { return m_present_queue; }

  const vk::Queue &compute_queue() const { return m_compute_queue; }

  const vk::Queue &transfer_queue() const { return m_transfer_queue; }

  uint32_t graphics_family() const { return m_graphics_family; }
  uint32_t present_family() const { return m_present_family; }
  uint32_t compute_family() const { return m_compute_family; }
  uint32_t transfer_family() const { return m_transfer_family; }

 private:
  void init_queue(const PhysicalDevice *physical_device,
                  const Surface *surface);
  void init_device(const Instance *instance,
                   const PhysicalDevice *physical_device);

 private:
  static const std::vector<const char *> s_device_extensions;

  vk::Device m_device;
  vk::PhysicalDeviceFeatures m_enable_features;

  vk::QueueFlags m_support_queues;
  vk::Queue m_graphics_queue;
  vk::Queue m_present_queue;
  vk::Queue m_compute_queue;
  vk::Queue m_transfer_queue;

  uint32_t m_graphics_family;
  uint32_t m_present_family;
  uint32_t m_compute_family;
  uint32_t m_transfer_family;
};

}  // namespace cx::graphics
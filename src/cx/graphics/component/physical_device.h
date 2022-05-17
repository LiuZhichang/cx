#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>

namespace cx::graphics {

class Instance;
class PhysicalDevice {
 public:
  explicit PhysicalDevice(const Instance* instance);

  operator const vk::PhysicalDevice&() const { return m_physical_device; }

  const vk::PhysicalDevice& handle() const { return m_physical_device; }

  const vk::PhysicalDeviceProperties& properties() const {
    return m_properties;
  }

  const vk::PhysicalDeviceFeatures& features() const { return m_featuers; }

  const vk::SampleCountFlags samples() const { return m_samples; }

 private:
  vk::PhysicalDevice choose_physical_device(
      const std::vector<vk::PhysicalDevice>& devices);

  vk::SampleCountFlagBits max_usable_sample_count() const;

  uint32_t score_physical_device(const vk::PhysicalDevice& device);

 private:
  vk::PhysicalDevice m_physical_device;
  vk::PhysicalDeviceProperties m_properties;
  vk::PhysicalDeviceFeatures m_featuers;
  vk::PhysicalDeviceMemoryProperties m_memory_properties;
  vk::SampleCountFlagBits m_samples;
};

}  // namespace cx::graphics
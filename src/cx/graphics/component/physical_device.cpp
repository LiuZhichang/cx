#include "physical_device.h"

#include "cx/common/error.h"
#include "cx/common/logger.h"
#include "cx/graphics/component/instance.h"
#include "cx/graphics/component/logical_device.h"

namespace cx::graphics {

static const std::vector<vk::SampleCountFlagBits> s_stage_flag_bits = {
    vk::SampleCountFlagBits::e64, vk::SampleCountFlagBits::e32,
    vk::SampleCountFlagBits::e16, vk::SampleCountFlagBits::e8,
    vk::SampleCountFlagBits::e4,  vk::SampleCountFlagBits::e2};

PhysicalDevice::PhysicalDevice(const Instance* instance) {
  std::vector<vk::PhysicalDevice> devices =
      instance->handle().enumeratePhysicalDevices();
  m_physical_device = choose_physical_device(devices);

  if (!m_physical_device) {
    Error::Output(log::Loggers::core, log::Level::eError,
                  Error::Code::eNoSupportPhysicalDevice);
  }

  m_properties = m_physical_device.getProperties();
  m_featuers = m_physical_device.getFeatures();
  m_memory_properties = m_physical_device.getMemoryProperties();
  m_samples = max_usable_sample_count();

#if defined(CX_DEBUG_MODE)
  LOG_INFO(log::Loggers::engine)
      << "selected the GPU:[" << m_properties.deviceName << ","
      << m_properties.deviceID << "]";
#endif
}

vk::PhysicalDevice PhysicalDevice::choose_physical_device(
    const std::vector<vk::PhysicalDevice>& devices) {
  std::multimap<uint32_t, vk::PhysicalDevice> rank_devices;

  for (const auto& device : devices) {
    rank_devices.emplace(score_physical_device(device), device);
  }

  auto rit = rank_devices.rbegin();
  if (rit->first > 0) {
    return rit->second;
  }

  return nullptr;
}

vk::SampleCountFlagBits PhysicalDevice::max_usable_sample_count() const {
  vk::PhysicalDeviceProperties properties;

  vk::SampleCountFlags color(properties.limits.framebufferColorSampleCounts);
  vk::SampleCountFlags depth(properties.limits.framebufferDepthSampleCounts);
  vk::SampleCountFlags flag = std::min(color, depth);

  for (vk::SampleCountFlagBits bit : s_stage_flag_bits) {
    if (flag & bit) {
      return bit;
    }
  }

  return vk::SampleCountFlagBits::e1;
}

uint32_t PhysicalDevice::score_physical_device(
    const vk::PhysicalDevice& device) {
  uint32_t score = 0;

  std::vector<vk::ExtensionProperties> extensionProperties =
      device.enumerateDeviceExtensionProperties();

  for (const char* currentExtension : LogicalDevice::GetExtensions()) {
    bool extensionFound = false;

    // Checks if the extension is in the available extensions.
    for (const auto& extension : extensionProperties) {
      if (strcmp(currentExtension, extension.extensionName) == 0) {
        extensionFound = true;
        break;
      }
    }

    if (!extensionFound) return 0;
  }

  vk::PhysicalDeviceProperties physicalDeviceProperties =
      device.getProperties();
  vk::PhysicalDeviceFeatures physicalDeviceFeatures = device.getFeatures();

  if (physicalDeviceProperties.deviceType ==
      vk::PhysicalDeviceType::eDiscreteGpu)
    score += 1000;

  score += physicalDeviceProperties.limits.maxImageDimension2D;
  return score;
}
}  // namespace cx::graphics

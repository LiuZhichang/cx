#include "logical_device.h"

#include <optional>

#include "cx/common/error.h"
#include "cx/common/logger.h"
#include "cx/graphics/component/instance.h"
#include "cx/graphics/component/physical_device.h"
#include "cx/graphics/renderpass/surface.h"

namespace cx::graphics {
const std::vector<const char *> LogicalDevice::s_device_extensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME};

LogicalDevice::LogicalDevice(const Instance *instance,
                             const PhysicalDevice *physical_device,
                             const Surface *surface)
    : m_device(VK_NULL_HANDLE),
      m_enable_features{},
      m_graphics_queue(VK_NULL_HANDLE),
      m_present_queue(VK_NULL_HANDLE),
      m_compute_queue(VK_NULL_HANDLE),
      m_transfer_queue(VK_NULL_HANDLE),
      m_graphics_family(0),
      m_present_family(0),
      m_compute_family(0),
      m_transfer_family(0) {
  init_queue(physical_device, surface);
  init_device(instance, physical_device);

#if defined(CX_DEBUG_MODE)
  if (m_device)
    LOG_INFO(log::Loggers::engine)
        << "The logical device is created successfully.";
#endif
}

LogicalDevice::~LogicalDevice() {
  m_device.waitIdle();
  m_device.destroy();
}

void LogicalDevice::init_queue(const PhysicalDevice *physical_device,
                               const Surface *surface) {
  const vk::PhysicalDevice &phy_device = physical_device->handle();
  std::vector<vk::QueueFamilyProperties> properties =
      phy_device.getQueueFamilyProperties();

  std::optional<uint32_t> graphics_family, present_family, compute_family,
      transfer_family;

  size_t deviceQueueFamilyPropertyCount = properties.size();
  for (uint32_t i = 0; i < deviceQueueFamilyPropertyCount; i++) {
    // Check for graphics support.
    if (properties[i].queueFlags & vk::QueueFlagBits::eGraphics) {
      graphics_family = i;
      m_graphics_family = i;
      m_support_queues |= vk::QueueFlagBits::eGraphics;
    }

    vk::Bool32 presentSupport = phy_device.getSurfaceSupportKHR(i, *surface);

    if (properties[i].queueCount > 0 && presentSupport) {
      present_family = i;
      m_present_family = i;
    }

    if (properties[i].queueFlags & vk::QueueFlagBits::eCompute) {
      compute_family = i;
      m_compute_family = i;
      m_support_queues |= vk::QueueFlagBits::eCompute;
    }

    if (properties[i].queueFlags & vk::QueueFlagBits::eTransfer) {
      transfer_family = i;
      m_transfer_family = i;
      m_support_queues |= vk::QueueFlagBits::eTransfer;
    }

    if (graphics_family && present_family && compute_family &&
        transfer_family) {
      break;
    }
  }

  if (!graphics_family)
    Error::Output(log::Loggers::core, log::Level::eError,
                  Error::Code::eNotFoundGraphicsQueue);
}

void LogicalDevice::init_device(const Instance *instance,
                                const PhysicalDevice *physical_device) {
  const vk::PhysicalDeviceFeatures &features = physical_device->features();
  std::vector<vk::DeviceQueueCreateInfo> queue_create_infos;
  float queue_priority = 0.0f;

  if (m_support_queues & vk::QueueFlagBits::eGraphics) {
    vk::DeviceQueueCreateInfo graphics_queue_create_info({}, m_graphics_family,
                                                         1, &queue_priority);
    queue_create_infos.emplace_back(graphics_queue_create_info);
  } else {
    m_graphics_family = 0;
  }

  if (m_support_queues & vk::QueueFlagBits::eCompute &&
      m_compute_family != m_graphics_family) {
    vk::DeviceQueueCreateInfo compute_queue_create_info({}, m_compute_family, 1,
                                                        &queue_priority);
    queue_create_infos.emplace_back(compute_queue_create_info);
  } else {
    m_compute_family = m_graphics_family;
  }

  if (m_support_queues & vk::QueueFlagBits::eTransfer &&
      m_transfer_family != m_graphics_family &&
      m_transfer_family != m_compute_family) {
    vk::DeviceQueueCreateInfo transfer_queue_queue_create_info(
        {}, m_transfer_family, 1, &queue_priority);
    queue_create_infos.emplace_back(transfer_queue_queue_create_info);
  } else {
    m_transfer_family = m_graphics_family;
  }

  if (features.sampleRateShading) {
    m_enable_features.sampleRateShading = VK_TRUE;
  }

  if (features.fillModeNonSolid) {
    m_enable_features.fillModeNonSolid = VK_TRUE;
  }

  if (features.wideLines) {
    m_enable_features.wideLines = VK_TRUE;
  } else {
    LOG_WARN(log::Loggers::engine)
        << "Selected GPU does not support wireframe pipelines!";
  }

  if (features.samplerAnisotropy) {
    m_enable_features.samplerAnisotropy = VK_TRUE;
  } else {
    LOG_WARN(log::Loggers::engine)
        << "Selected GPU does not support sampler anisotropy!";
  }

  if (features.textureCompressionBC) {
    m_enable_features.textureCompressionBC = VK_TRUE;
  } else if (features.textureCompressionASTC_LDR) {
    m_enable_features.textureCompressionASTC_LDR = VK_TRUE;
  } else if (features.textureCompressionETC2) {
    m_enable_features.textureCompressionETC2 = VK_TRUE;
  }

  if (features.vertexPipelineStoresAndAtomics) {
    m_enable_features.vertexPipelineStoresAndAtomics = VK_TRUE;
  } else {
    LOG_WARN(log::Loggers::engine) << "Selected GPU does not support vertex "
                                      "pipeline stores and atomics!";
  }

  if (features.fragmentStoresAndAtomics) {
    m_enable_features.fragmentStoresAndAtomics = VK_TRUE;
  } else {
    LOG_WARN(log::Loggers::engine)
        << "Selected GPU does not support fragment stores and atomics!";
  }

  if (features.shaderStorageImageExtendedFormats) {
    m_enable_features.shaderStorageImageExtendedFormats = VK_TRUE;
  } else {
    LOG_WARN(log::Loggers::engine)
        << "Selected GPU does not support shader storage extended formats!";
  }

  if (features.shaderStorageImageWriteWithoutFormat) {
    m_enable_features.shaderStorageImageWriteWithoutFormat = VK_TRUE;
  } else {
    LOG_WARN(log::Loggers::engine)
        << "Selected GPU does not support shader storage write without format!";
  }

  if (features.geometryShader) {
    m_enable_features.geometryShader = VK_TRUE;
  } else {
    LOG_WARN(log::Loggers::engine)
        << "Selected GPU does not support geometry shaders!";
  }

  if (features.tessellationShader) {
    m_enable_features.tessellationShader = VK_TRUE;
  } else {
    LOG_WARN(log::Loggers::engine)
        << "Selected GPU does not support tessellation shaders!";
  }

  if (features.multiViewport) {
    m_enable_features.multiViewport = VK_TRUE;
  } else {
    LOG_WARN(log::Loggers::engine)
        << "Selected GPU does not support multi viewports!";
  }

  std::vector<const char *> layers;
  if (instance->is_enable_validations()) {
    layers = Instance::GetValidationLayers();
  }

  vk::DeviceCreateInfo device_create_info(
      {}, queue_create_infos, layers, s_device_extensions, &m_enable_features);
  vk::PhysicalDevice phy_device = physical_device->handle();
  m_device = phy_device.createDevice(device_create_info);
}

}  // namespace cx::graphics
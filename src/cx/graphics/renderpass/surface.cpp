#include "surface.h"

#include "cx/common/logger.h"
#include "cx/graphics/component/instance.h"
#include "cx/graphics/component/physical_device.h"
#include "cx/graphics/graphics.h"
#include "cx/window/window.h"

namespace cx::graphics {

Surface::Surface(const Instance* instance,
                 const PhysicalDevice* physical_device)
    : m_instance(instance) {
  auto result = Window::Get()->create_surface(*instance, m_surface);

  Graphics::Check(result);

  vk::PhysicalDevice device = physical_device->handle();
  m_capabilities = device.getSurfaceCapabilitiesKHR(m_surface);

  std::vector<vk::SurfaceFormatKHR> formats =
      device.getSurfaceFormatsKHR(m_surface);

  if (formats.size() == 1 && formats[0].format == vk::Format::eUndefined) {
    m_format.format = vk::Format::eB8G8R8A8Unorm;
    m_format.colorSpace = formats[0].colorSpace;
  } else {
    bool found_B8G8R8A8_UNORM = false;

    for (auto& surfaceFormat : formats) {
      if (surfaceFormat.format == vk::Format::eB8G8R8A8Unorm) {
        m_format.format = surfaceFormat.format;
        m_format.colorSpace = surfaceFormat.colorSpace;
        found_B8G8R8A8_UNORM = true;
        break;
      }
    }

    if (!found_B8G8R8A8_UNORM) {
      m_format.format = formats[0].format;
      m_format.colorSpace = formats[0].colorSpace;
    }
  }

#if defined(CX_DEBUG_MODE)
  if (m_surface) {
    LOG_INFO(log::Loggers::engine) << "The surface is created successfully.";
  }
#endif
}

Surface::~Surface() {
  vk::Instance instance = m_instance->handle();
  instance.destroySurfaceKHR(m_surface);
}

}  // namespace cx::graphics
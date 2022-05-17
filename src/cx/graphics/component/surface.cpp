#include "surface.h"

#include "cx/common/logger.h"
#include "cx/graphics/graphics.h"
#include "cx/window/window.h"
#include "instance.h"
#include "physical_device.h"

namespace cx::graphics {

Surface::Surface(const Instance* instance, const PhysicalDevice* device)
    : m_instance(instance) {
  auto result = Window::Get()->create_surface(instance->handle());

  Graphics::Check(result.second);

  m_surface = result.first;

  m_capabilities = device->handle().getSurfaceCapabilitiesKHR(m_surface);

  std::vector<vk::SurfaceFormatKHR> formats =
      device->handle().getSurfaceFormatsKHR(m_surface);

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
    LOG_INFO(log::Loggers::engine) << "surface create successed";
  } else {
    LOG_INFO(log::Loggers::engine) << "surface create failed";
  }
#endif
}

Surface::~Surface() { m_instance->handle().destroySurfaceKHR(m_surface); }

}  // namespace cx::graphics
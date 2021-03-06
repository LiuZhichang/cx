#include "graphics.h"

#include "cx/common/logger.h"

namespace cx {

using namespace graphics;

Graphics::Graphics()
    : m_instance(std::make_unique<Instance>()),
      m_physical_device(std::make_unique<PhysicalDevice>(m_instance.get())),
      m_surface(
          std::make_unique<Surface>(m_instance.get(), m_physical_device.get())),
      m_logical_device(std::make_unique<LogicalDevice>(
          m_instance.get(), m_physical_device.get(), m_surface.get())) {}

Graphics::~Graphics() {
  vk::Queue graphics_queue = m_logical_device->graphics_queue();

  if (graphics_queue) graphics_queue.waitIdle();

  m_command_pool.clear();
  m_command_buffers.clear();
}

void Graphics::update() {}

const Descriptor* Graphics::attachment(const std::string& name) const {
  if (auto it = m_attachments.find(name); it != m_attachments.end())
    return it->second;
  return nullptr;
}

void Graphics::setup_pipeline_cache() {
  vk::PipelineCacheCreateInfo pipeline_cache_create_info = {};
  vk::Device device = *m_logical_device;
  vk::Result result = device.createPipelineCache(&pipeline_cache_create_info,
                                                 nullptr, &m_pipeline_cache);
  Graphics::Check(result);
}

void Graphics::rebuild_swapchain() {
  vk::Device device = *m_logical_device;
  device.waitIdle();
  VkExtent2D display_extent = {Window::Get()->size().x,
                               Window::Get()->size().y};
  m_swapchain = std::make_unique<Swapchain>(display_extent, m_swapchain.get());
  regenerate_command_buffers();
}

void Graphics::reset_render_stage() { rebuild_swapchain(); 
}

void Graphics::regenerate_command_buffers() {}

const std::shared_ptr<graphics::CommandPool>& Graphics::command_pool(
    const std::thread::id& id) {
  auto it = m_command_pool.find(id);
  if (it != m_command_pool.end()) {
    return it->second;
  }

  return m_command_pool.emplace(id, std::make_shared<graphics::CommandPool>(id))
      .first->second;
}

std::string Graphics::VulkanResultToString(vk::Result result) {
  switch (result) {
    case vk::Result::eSuccess:
      return "Success";
    case vk::Result::eNotReady:
      return "A fence or query has not yet completed";
    case vk::Result::eTimeout:
      return "A wait operation has not completed in the specified time";
    case vk::Result::eEventSet:
      return "An event is signaled";
    case vk::Result::eEventReset:
      return "An event is unsignaled";
    case vk::Result::eIncomplete:
      return "A return array was too small for the result";
    case vk::Result::eErrorOutOfHostMemory:
      return "A host memory allocation has failed";
    case vk::Result::eErrorOutOfDeviceMemory:
      return "A device memory allocation has failed";
    case vk::Result::eErrorInitializationFailed:
      return "Initialization of an object could not be completed for "
             "implementation-specific reasons";
    case vk::Result::eErrorDeviceLost:
      return "The logical or physical device has been lost";
    case vk::Result::eErrorMemoryMapFailed:
      return "Mapping of a memory object has failed";
    case vk::Result::eErrorLayerNotPresent:
      return "A requested layer is not present or could not be loaded";
    case vk::Result::eErrorExtensionNotPresent:
      return "A requested extension is not supported";
    case vk::Result::eErrorFeatureNotPresent:
      return "A requested feature is not supported";
    case vk::Result::eErrorIncompatibleDriver:
      return "The requested version of Vulkan is not supported by the driver "
             "or is otherwise incompatible";
    case vk::Result::eErrorTooManyObjects:
      return "Too many objects of the type have already been created";
    case vk::Result::eErrorFormatNotSupported:
      return "A requested format is not supported on this device";
    case vk::Result::eErrorSurfaceLostKHR:
      return "A surface is no longer available";
    case vk::Result::eSuboptimalKHR:
      return "A swapchain no longer matches the surface properties exactly, "
             "but can still be used";
    case vk::Result::eErrorOutOfDateKHR:
      return "A surface has changed in such a way that it is no longer "
             "compatible with the swapchain";
    case vk::Result::eErrorIncompatibleDisplayKHR:
      return "The display used by a swapchain does not use the same "
             "presentable image layout";
    case vk::Result::eErrorNativeWindowInUseKHR:
      return "The requested window is already connected to a VkSurfaceKHR, or "
             "to some other non-Vulkan API";
    case vk::Result::eErrorValidationFailedEXT:
      return "A validation layer found an error";
    default:
      return "Unknown Vulkan error";
  }
}

void Graphics::Check(vk::Result result) {
  if ((VkResult)result >= 0) return;

  std::string error_msg = VulkanResultToString(result);

  throw std::runtime_error("vulkan error:" + error_msg);
}

}  // namespace cx
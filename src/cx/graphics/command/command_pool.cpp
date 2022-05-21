#include "command_pool.h"

#include "cx/graphics/graphics.h"

namespace cx::graphics {

CommandPool::CommandPool(const std::thread::id& thread_id)

    : m_thread_id(thread_id) {
  vk::Device device = *Graphics::Get()->logical_device();
  uint32_t graphics_family =
      Graphics::Get()->logical_device()->graphics_family();
  vk::CommandPoolCreateInfo create_info({}, graphics_family);
  vk::Result result = device.createCommandPool(&create_info, nullptr, &m_pool);
  Graphics::Check(result);
}
CommandPool::~CommandPool() {
  vk::Device device = *Graphics::Get()->logical_device();
  device.destroyCommandPool(m_pool);
}

}  // namespace cx::graphics
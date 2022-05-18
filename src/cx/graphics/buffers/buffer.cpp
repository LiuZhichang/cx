#include "buffer.h"

#include "cx/graphics/graphics.h"

namespace cx::graphics {

Buffer::Buffer(vk::DeviceSize size, vk::BufferUsageFlags usage,
               vk::MemoryPropertyFlags properties, const void* data)
    : m_size(size), m_buffer(VK_NULL_HANDLE), m_memory(VK_NULL_HANDLE) {
  const LogicalDevice* device = Graphics::Get()->logical_device();

  std::array<uint32_t, 3> queue_family = {device->graphics_family(),
                                          device->present_family(),
                                          device->compute_family()};

  vk::BufferCreateInfo buf_create_info(
      {}, size, usage, vk::SharingMode::eExclusive, queue_family);
}

Buffer::~Buffer() {}

}  // namespace cx::graphics
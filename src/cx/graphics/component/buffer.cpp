#include "buffer.h"

namespace cx::graphics {

Buffer::Buffer(vk::DeviceSize size, vk::BufferUsageFlags usage,
               vk::MemoryPropertyFlags properties, const void* data)
    : m_size(size), m_buffer(VK_NULL_HANDLE), m_memory(VK_NULL_HANDLE) {}

Buffer::~Buffer() {}

}  // namespace cx::graphics
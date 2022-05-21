#include "buffer.h"

#include "cx/common/defer.h"
#include "cx/common/error.h"
#include "cx/common/logger.h"
#include "cx/graphics/graphics.h"

namespace cx::graphics {

Buffer::Buffer(vk::DeviceSize size, vk::BufferUsageFlags usage,
               vk::MemoryPropertyFlags properties, void* data)
    : m_size(size), m_buffer(VK_NULL_HANDLE), m_memory(VK_NULL_HANDLE) {
  const LogicalDevice* logical_device = Graphics::Get()->logical_device();

  std::array<uint32_t, 3> queue_family = {logical_device->graphics_family(),
                                          logical_device->present_family(),
                                          logical_device->compute_family()};

  vk::BufferCreateInfo buf_create_info(
      {}, size, usage, vk::SharingMode::eExclusive, queue_family);

  vk::Device device = *logical_device;
  vk::Result result = device.createBuffer(&buf_create_info, nullptr, &m_buffer);
  Graphics::Check(result);

  vk::MemoryRequirements mem_requires =
      device.getBufferMemoryRequirements(m_buffer);

  uint32_t mem_type_index =
      FindMemoryType(mem_requires.memoryTypeBits, properties);
  vk::MemoryAllocateInfo mem_allocate_info(mem_requires.size, mem_type_index);

  result = device.allocateMemory(&mem_allocate_info, nullptr, &m_memory);
  Graphics::Check(result);

  if (data) {
    void* map;
    map_memory(&data);
    defer { unmap_memory(); };

    if (properties & vk::MemoryPropertyFlagBits::eHostCoherent) {
      vk::MappedMemoryRange map_memory_range(m_memory, 0, size);
      device.flushMappedMemoryRanges(map_memory_range);
    }
  }

  device.bindBufferMemory(m_buffer, m_memory, 0);
}

void Buffer::map_memory(void** data) const {
  vk::Device device = *Graphics::Get()->logical_device();
  Graphics::Check(device.mapMemory(m_memory, 0, m_size, {}, data));
}

void Buffer::unmap_memory() const {
  vk::Device device = *Graphics::Get()->logical_device();
  device.unmapMemory(m_memory);
}

uint32_t Buffer::FindMemoryType(
    uint32_t type_filter, const vk::MemoryPropertyFlags& required_properties) {
  vk::PhysicalDevice physical_device = *Graphics::Get()->physical_device();

  vk::PhysicalDeviceMemoryProperties proerties =
      physical_device.getMemoryProperties();

  for (uint32_t i = 0; i < proerties.memoryTypeCount; ++i) {
    uint32_t memory_type_bits = 1 << i;
    if (type_filter & memory_type_bits &&
        (proerties.memoryTypes[i].propertyFlags & required_properties) ==
            required_properties) {
      return i;
    }
  }

  Error::Output(log::Loggers::engine, log::Level::eError,
                Error::Code::eNotFoundMemoryType);

  return 0;
}

Buffer::~Buffer() {
  vk::Device device = *Graphics::Get()->logical_device();
  device.destroyBuffer(m_buffer);
}

}  // namespace cx::graphics
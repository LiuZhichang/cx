#pragma once

#include <vulkan/vulkan.hpp>

#include "cx/common/internal.h"

namespace cx::graphics {
/**
 * @brief vulkan缓冲区
 *
 */
class Buffer {
 public:
  /**
   * @brief 构造函数
   *
   * @param size 缓冲区大小
   * @param usage 缓冲区使用的阶段(时机/位置)
   * @param properties 内存属性
   * @param data 存放的数据
   */
  Buffer(vk::DeviceSize size, vk::BufferUsageFlags usage,
         vk::MemoryPropertyFlags properties, void* data = nullptr);

  /**
   * @brief 析构函数
   *
   */
  virtual ~Buffer();

  /**
   * @brief 把数据映射到内存
   *
   * @param data 数据
   */
  void map_memory(void** data) const;

  /**
   * @brief 解除内存映射
   *
   */
  void unmap_memory() const;

  const vk::DeviceSize size() const { return m_size; }

  const vk::Buffer handle() const { return m_buffer; }

  CX_STATIC uint32_t FindMemoryType(
      uint32_t type_filter, const vk::MemoryPropertyFlags& required_properties);

  // static void InsertBufferMemoryBarrier(const vk::CommandBuffer&
  // command_buffer, const vk:)
  operator const vk::Buffer&() const { return m_buffer; }

 private:
  vk::DeviceSize m_size;
  vk::Buffer m_buffer;
  vk::DeviceMemory m_memory;
};

}  // namespace cx::graphics
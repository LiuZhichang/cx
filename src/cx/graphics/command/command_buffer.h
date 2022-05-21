#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include "cx/graphics/command/command_pool.h"

namespace cx::graphics {

class CommandBuffer {
 public:
  explicit CommandBuffer(
      bool begin = true,
      vk::QueueFlagBits queue_type = vk::QueueFlagBits::eGraphics,
      vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary);
  ~CommandBuffer();

  void begin(vk::CommandBufferUsageFlags usage =
                 vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

  void end();

  void submit_idle();

  void submit(const vk::Semaphore& wait_semaphore = VK_NULL_HANDLE,
              const vk::Semaphore& signal_semaphore = VK_NULL_HANDLE,
              vk::Fence fence = VK_NULL_HANDLE);

  operator const vk::CommandBuffer&() const { return m_buffer; }

  const vk::CommandBuffer& handle() const { return m_buffer; }

  bool is_running() const { return m_running; }

 private:
  vk::Queue choose_queue() const;

  vk::CommandBuffer m_buffer;
  vk::QueueFlagBits m_queue_type;
  bool m_running;

  std::shared_ptr<CommandPool> m_command_pool;
};

}  // namespace cx::graphics
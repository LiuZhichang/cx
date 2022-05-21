#include "command_buffer.h"

#include "cx/graphics/graphics.h"

namespace cx::graphics {

CommandBuffer::CommandBuffer(bool begin, vk::QueueFlagBits queue_type,
                             vk::CommandBufferLevel level)
    : m_queue_type(queue_type),
      m_command_pool(Graphics::Get()->command_pool()) {
  vk::Device device = *Graphics::Get()->logical_device();

  vk::CommandBufferAllocateInfo allocate_info(*m_command_pool.get(), level, 1);

  vk::Result result = device.allocateCommandBuffers(&allocate_info, &m_buffer);
  Graphics::Check(result);

  if (begin) this->begin();
}
CommandBuffer::~CommandBuffer() {
  vk::Device device = *Graphics::Get()->logical_device();
  device.freeCommandBuffers(*m_command_pool.get(), m_buffer);
}

void CommandBuffer::begin(vk::CommandBufferUsageFlags usage) {
  if (m_running) return;

  vk::CommandBufferBeginInfo begin_info(usage);
  vk::Result result = m_buffer.begin(&begin_info);
  Graphics::Check(result);
  m_running = true;
}

void CommandBuffer::end() {
  if (!m_running) return;
  m_buffer.end();
}

void CommandBuffer::submit_idle() {
  if (m_running) end();
  vk::Device device = *Graphics::Get()->logical_device();
  vk::Queue queue = choose_queue();

  vk::SubmitInfo submit_info{};
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &m_buffer;

  vk::FenceCreateInfo fence_create_info{};

  vk::Fence fence;
  vk::Result result = device.createFence(&fence_create_info, nullptr, &fence);
  Graphics::Check(result);
  result = device.resetFences(1, &fence);
  Graphics::Check(result);
  result = queue.submit(1, &submit_info, fence);
  Graphics::Check(result);
  result = device.waitForFences(1, &fence, VK_TRUE,
                                std::numeric_limits<uint64_t>::max());
  Graphics::Check(result);

  device.destroyFence(fence);
}

void CommandBuffer::submit(const vk::Semaphore& wait_semaphore,
                           const vk::Semaphore& signal_semaphore,
                           vk::Fence fence) {
  if (m_running) end();

  vk::Device device = *Graphics::Get()->logical_device();
  vk::Queue queue = choose_queue();

  vk::SubmitInfo submit_info{};
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &m_buffer;

  if (wait_semaphore) {
    vk::PipelineStageFlags submit_pipeline_stage =
        vk::PipelineStageFlagBits::eColorAttachmentOutput;

    submit_info.pWaitDstStageMask = &submit_pipeline_stage;
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &wait_semaphore;
  }

  if (signal_semaphore) {
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &signal_semaphore;
  }

  if (fence) {
    vk::Result result = device.resetFences(1, &fence);
    Graphics::Check(result);
  }
  vk::Result result = queue.submit(1, &submit_info, fence);
  Graphics::Check(result);
}

vk::Queue CommandBuffer::choose_queue() const {
  const LogicalDevice* device = Graphics::Get()->logical_device();
  switch (m_queue_type) {
    case vk::QueueFlagBits::eGraphics:
      return device->graphics_queue();
    case vk::QueueFlagBits::eCompute:
      return device->compute_queue();
    default:
      return VK_NULL_HANDLE;
  }
}

}  // namespace cx::graphics
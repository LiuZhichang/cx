#pragma once
#include <thread>
#include <vulkan/vulkan.hpp>

namespace cx::graphics {

class CommandPool {
 public:
  CommandPool(const std::thread::id& thread_id = std::this_thread::get_id());
  ~CommandPool();

  operator const vk::CommandPool&() const { return m_pool; }

  const vk::CommandPool& handle() const { return m_pool; }
  const std::thread::id thread_id() const { return m_thread_id; }

 private:
  vk::CommandPool m_pool;

  std::thread::id m_thread_id;
};

}  // namespace cx::graphics
#pragma once

#include <vulkan/vulkan.hpp>

namespace cx::graphics {

/**
 * @brief 交换链 用于与suface交互，传输图像, 在一些特定情况下交换链 需要重构
 *
 */
class Swapchain {
 public:
  /**
   * @brief 构造函数
   *
   * @param extent 图像大小
   * @param old_swapchain 旧的交换链
   */
  explicit Swapchain(const vk::Extent2D& extent,
                     const Swapchain* old_swapchain = nullptr);

  ~Swapchain();

  /**
   * @brief 获取下一张图像
   *
   * @param present_complete_sem 信号量 同步
   * @param fence 栏栅 同步
   * @return vk::Result 操作结果
   */
  vk::Result acquired_next_image(
      const vk::Semaphore& present_complete_sem = VK_NULL_HANDLE,
      vk::Fence fence = VK_NULL_HANDLE);

  operator const vk::SwapchainKHR&() const { return m_swapchain; }

  const vk::SwapchainKHR& handle() const { return m_swapchain; }

 private:
  vk::Extent2D m_extent;
  vk::SwapchainKHR m_swapchain;
  vk::PresentModeKHR m_present_mode;
  std::vector<vk::Image> m_images;
  std::vector<vk::ImageView> m_image_views;
  vk::SurfaceTransformFlagBitsKHR m_pre_transfom;
  vk::CompositeAlphaFlagBitsKHR m_composite_alpha_flag;
  vk::Fence m_image_fence;
  uint32_t m_active_image_index;
};

}  // namespace cx::graphics
#include "swapchain.h"

#include <array>

#include "cx/common/logger.h"
#include "cx/graphics/graphics.h"
#include "cx/graphics/image/image.h"

namespace cx::graphics {

static const std::vector<vk::CompositeAlphaFlagBitsKHR>
    s_composite_alpha_flags = {vk::CompositeAlphaFlagBitsKHR::eOpaque,
                               vk::CompositeAlphaFlagBitsKHR::ePreMultiplied,
                               vk::CompositeAlphaFlagBitsKHR::ePostMultiplied,
                               vk::CompositeAlphaFlagBitsKHR::eInherit};

Swapchain::Swapchain(const vk::Extent2D& extent, const Swapchain* old_swapchain)
    : m_extent(extent),
      m_present_mode(vk::PresentModeKHR::eFifo),
      m_pre_transfom(vk::SurfaceTransformFlagBitsKHR::eIdentity),
      m_composite_alpha_flag(vk::CompositeAlphaFlagBitsKHR::eOpaque),
      m_active_image_index(std::numeric_limits<uint32_t>::max()) {
  const PhysicalDevice* physical_device = Graphics::Get()->physical_device();
  const LogicalDevice* logical_device = Graphics::Get()->logical_device();
  const Surface* surface = Graphics::Get()->surface();

  vk::SurfaceFormatKHR surface_format = surface->format();
  vk::SurfaceCapabilitiesKHR surface_capabilities = surface->capabilities();

  uint32_t graphics_family = logical_device->graphics_family();
  uint32_t present_family = logical_device->present_family();

  std::vector<vk::PresentModeKHR> present_modes =
      physical_device->handle().getSurfacePresentModesKHR(*surface);

  for (const vk::PresentModeKHR& mode : present_modes) {
    if (mode == vk::PresentModeKHR::eMailbox) {
      m_present_mode = mode;
      break;
    }

    if (mode != vk::PresentModeKHR::eMailbox &&
        mode == vk::PresentModeKHR::eImmediate) {
      m_present_mode = mode;
    }
  }

  uint32_t desired_image_count = surface_capabilities.minImageCount + 1;
  if (surface_capabilities.maxImageCount > 0 &&
      desired_image_count > surface_capabilities.maxImageCount) {
    desired_image_count = surface_capabilities.maxImageCount;
  }

  if (surface_capabilities.supportedTransforms &
      vk::SurfaceTransformFlagBitsKHR::eIdentity) {
    m_pre_transfom = vk::SurfaceTransformFlagBitsKHR::eIdentity;
  } else {
    m_pre_transfom = surface_capabilities.currentTransform;
  }

  for (const vk::CompositeAlphaFlagBitsKHR& composite_alpha_flag :
       s_composite_alpha_flags) {
    if (surface_capabilities.supportedCompositeAlpha & composite_alpha_flag) {
      m_composite_alpha_flag = composite_alpha_flag;
      break;
    }
  }

  vk::ImageUsageFlags image_usage_flags =
      vk::ImageUsageFlagBits::eColorAttachment;
  if (surface_capabilities.supportedUsageFlags &
      vk::ImageUsageFlagBits::eTransferSrc)
    image_usage_flags |= vk::ImageUsageFlagBits::eTransferSrc;
  if (surface_capabilities.supportedUsageFlags &
      vk::ImageUsageFlagBits::eTransferDst)
    image_usage_flags |= vk::ImageUsageFlagBits::eTransferDst;

  vk::SwapchainCreateInfoKHR swapchain_create_info{};
  swapchain_create_info.surface = *surface;
  swapchain_create_info.minImageCount = desired_image_count;
  swapchain_create_info.imageFormat = surface_format.format;
  swapchain_create_info.imageColorSpace = surface_format.colorSpace;
  swapchain_create_info.imageExtent = m_extent;
  swapchain_create_info.imageArrayLayers = 1;
  swapchain_create_info.imageUsage = image_usage_flags;
  swapchain_create_info.imageSharingMode = vk::SharingMode::eExclusive;
  swapchain_create_info.preTransform = m_pre_transfom;
  swapchain_create_info.compositeAlpha = m_composite_alpha_flag;
  swapchain_create_info.presentMode = m_present_mode;
  swapchain_create_info.clipped = VK_TRUE;

  if (old_swapchain) swapchain_create_info.oldSwapchain = *old_swapchain;

  if (graphics_family != present_family) {
    std::array<uint32_t, 2> queue_family = {graphics_family, present_family};
    swapchain_create_info.imageSharingMode = vk::SharingMode::eConcurrent;
    swapchain_create_info.queueFamilyIndexCount = 2;
    swapchain_create_info.pQueueFamilyIndices = queue_family.data();
  }

  vk::Device device = logical_device->handle();
  m_swapchain = device.createSwapchainKHR(swapchain_create_info);

#if defined(CX_DEBUG_MODE)
  if (m_swapchain)
    LOG_INFO(log::Loggers::engine) << "The swapchain is created successfully.";
#endif

  m_images = device.getSwapchainImagesKHR(m_swapchain);
  size_t image_count = m_images.size();
  m_image_views.resize(image_count);

  for (int i = 0; i < image_count; ++i) {
    Image::GenerateView(m_images[i], m_image_views[i], vk::ImageViewType::e2D,
                        surface_format.format, vk::ImageAspectFlagBits::eColor,
                        1, 0, 1, 0);
  }

  vk::FenceCreateInfo fence_create_info{};
  m_image_fence = device.createFence(fence_create_info);
}

Swapchain::~Swapchain() {
  const vk::Device& device = Graphics::Get()->logical_device()->handle();
  device.destroySwapchainKHR(m_swapchain);
}

vk::Result Swapchain::acquired_next_image(
    const vk::Semaphore& present_complete_sem, vk::Fence fence) {
  const vk::Device device = *Graphics::Get()->logical_device();
  vk::Result result;
  if (fence) {
    result = device.waitForFences(1, &fence, true,
                                  std::numeric_limits<uint64_t>::max());
    Graphics::Check(result);
  }
  result = device.acquireNextImageKHR(
      m_swapchain, std::numeric_limits<uint64_t>::max(), present_complete_sem,
      {}, &m_active_image_index);
  Graphics::Check(result);

  return result;
}
}  // namespace cx::graphics
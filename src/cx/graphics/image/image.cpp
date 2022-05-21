#include "image.h"

#include "cx/graphics/buffers/buffer.h"
#include "cx/graphics/graphics.h"

namespace cx::graphics {

static constexpr uint32_t g_anisotropy = 16;

Image::Image(vk::Filter filter, vk::SamplerAddressMode sample_address_mode,
             vk::SampleCountFlagBits sample_count, vk::ImageLayout layout,
             vk::ImageUsageFlags usage, vk::Format format, uint32_t mip_level,
             uint32_t array_layers, const vk::Extent3D& extent)
    : m_filter(filter),
      m_sampler_address_mode(sample_address_mode),
      m_sample_count(sample_count),
      m_layout(layout),
      m_usage(usage),
      m_format(format),
      m_extent(extent),
      m_mip_level(mip_level),
      m_array_layers(array_layers) {
  vk::Device device = *Graphics::Get()->logical_device();
}

Image::~Image() {
  vk::Device device = *Graphics::Get()->logical_device();
  device.freeMemory(m_memory);
  device.destroyImageView(m_view);
  device.destroySampler(m_sampler);
  device.destroyImage(m_image);
}

WriteDescriptorSet Image::write_descriptor(
    uint32_t binding, vk::DescriptorType descriptor_type,
    const std::optional<OffsetSize>& offsetSize) const {
  vk::DescriptorImageInfo image_info = {};
  image_info.sampler = m_sampler;
  image_info.imageView = m_view;
  image_info.imageLayout = m_layout;

  vk::WriteDescriptorSet descriptor_write = {};
  descriptor_write.dstSet = VK_NULL_HANDLE;
  descriptor_write.dstBinding = binding;
  descriptor_write.dstArrayElement = 0;
  descriptor_write.descriptorCount = 1;
  descriptor_write.descriptorType = descriptor_type;
  descriptor_write.pImageInfo = &image_info;
  return {descriptor_write, image_info};
}

vk::DescriptorSetLayoutBinding Image::descriptorset_layout(
    uint32_t binding, vk::DescriptorType descriptor_type,
    vk::ShaderStageFlags stage, uint32_t count) {
  vk::DescriptorSetLayoutBinding descriptorset_layout_binding = {};
  descriptorset_layout_binding.binding = binding;
  descriptorset_layout_binding.descriptorType = descriptor_type;
  descriptorset_layout_binding.descriptorCount = 1;
  descriptorset_layout_binding.stageFlags = stage;
  descriptorset_layout_binding.pImmutableSamplers = nullptr;
  return descriptorset_layout_binding;
}

uint32_t Image::GetMipLevels(const vk::Extent3D& extent) {
  return static_cast<uint32_t>(
      std::floor(std::log2(
          std::max(extent.width, std::max(extent.width, extent.height)))) +
      1);
}

vk::Format Image::FindSupportFormat(const std::vector<vk::Format>& formats,
                                    vk::ImageTiling tiling,
                                    vk::FormatFeatureFlags feature) {
  vk::PhysicalDevice device = *Graphics::Get()->physical_device();

  for (const vk::Format& format : formats) {
    vk::FormatProperties properties = device.getFormatProperties(format);
    if (tiling == vk::ImageTiling::eLinear &&
        (properties.linearTilingFeatures & feature) == feature) {
      return format;
    }

    if (tiling == vk::ImageTiling::eOptimal &&
        (properties.optimalTilingFeatures & feature) == feature) {
      return format;
    }
  }

  return vk::Format::eUndefined;
}

bool Image::HasDepth(vk::Format format) {
  CX_STATIC const std::vector<vk::Format> s_depth_formats{
      vk::Format::eD16Unorm,       vk::Format::eX8D24UnormPack32,
      vk::Format::eD32Sfloat,      vk::Format::eD16UnormS8Uint,
      vk::Format::eD24UnormS8Uint, vk::Format::eD32SfloatS8Uint};
  return std::find(s_depth_formats.begin(), s_depth_formats.end(), format) !=
         std::end(s_depth_formats);
}

bool Image::HasStencil(vk::Format format) {
  CX_STATIC const std::vector<vk::Format> s_stencil_formats{
      vk::Format::eS8Uint, vk::Format::eD16UnormS8Uint,
      vk::Format::eD24UnormS8Uint, vk::Format::eD32SfloatS8Uint};

  return std::find(s_stencil_formats.begin(), s_stencil_formats.end(),
                   format) != std::end(s_stencil_formats);
}

void Image::Generate(vk::Image& image, vk::DeviceMemory memory,
                     const vk::Extent3D extent, vk::Format format,
                     vk::SampleCountFlagBits sample_count,
                     vk::ImageTiling tiling, vk::ImageUsageFlags usage,
                     vk::MemoryPropertyFlags properties, uint32_t mip_level,
                     uint32_t array_layers, vk::ImageType type) {
  vk::Device device = *Graphics::Get()->logical_device();

  vk::ImageCreateInfo create_info{};
  create_info.flags = (array_layers == 6)
                          ? vk::ImageCreateFlagBits::eCubeCompatible
                          : vk::ImageCreateFlagBits();
  create_info.imageType = type;
  create_info.format = format;
  create_info.extent = extent;
  create_info.mipLevels = mip_level;
  create_info.arrayLayers = array_layers;
  create_info.samples = sample_count;
  create_info.tiling = tiling;
  create_info.usage = usage;
  create_info.sharingMode = vk::SharingMode::eExclusive;
  create_info.initialLayout = vk::ImageLayout::eUndefined;

  vk::Result result = device.createImage(&create_info, nullptr, &image);
  Graphics::Check(result);

  vk::MemoryRequirements mem_requires;
  mem_requires = device.getImageMemoryRequirements(image);

  uint32_t mem_type_index =
      Buffer::FindMemoryType(mem_requires.memoryTypeBits, properties);

  vk::MemoryAllocateInfo mem_allocate_info(mem_requires.size, mem_type_index);
  result = device.allocateMemory(&mem_allocate_info, nullptr, &memory);
  Graphics::Check(result);

  device.bindImageMemory(image, memory, 0);
}

void Image::GenerateSampler(vk::Sampler& sampler, vk::Filter filter,
                            vk::SamplerAddressMode address_mode,
                            bool anisotropic, uint32_t mip_levels) {
  const PhysicalDevice* phy_device = Graphics::Get()->physical_device();
  vk::Device device = *Graphics::Get()->logical_device();

  float max_anis =
      Graphics::Get()->physical_device()->features().samplerAnisotropy
          ? std::min(g_anisotropy,
                     phy_device->properties().limits.maxSamplerAllocationCount)
          : 1.0f;
  vk::SamplerCreateInfo create_info(
      {}, filter, filter, vk::SamplerMipmapMode::eLinear, address_mode,
      address_mode, address_mode, 0.0f, anisotropic, max_anis, false,
      vk::CompareOp::eNever, 0.0f, mip_levels,
      vk::BorderColor::eFloatOpaqueWhite, false);

  vk::Result result = device.createSampler(&create_info, nullptr, &sampler);
  Graphics::Check(result);
}

void Image::GenerateView(const vk::Image& image, vk::ImageView& view,
                         vk::ImageViewType type, vk::Format format,
                         vk::ImageAspectFlags image_aspect_flags,
                         uint32_t mip_levels, uint32_t base_mip_levels,
                         uint32_t layer_count, uint32_t base_layer_count) {
  vk::Device device = *Graphics::Get()->logical_device();
  vk::ImageViewCreateInfo create_info(
      {}, image, type, format,
      {vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG,
       vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA},
      {});
  create_info.subresourceRange.aspectMask = image_aspect_flags;
  create_info.subresourceRange.baseMipLevel = base_mip_levels;
  create_info.subresourceRange.levelCount = mip_levels;
  create_info.subresourceRange.baseArrayLayer = base_layer_count;
  create_info.subresourceRange.layerCount = layer_count;

  vk::Result result = device.createImageView(&create_info, nullptr, &view);
  Graphics::Check(result);
}

void Image::GenerateMipmaps(const vk::Image& image, const vk::Extent3D& extent,
                            vk::Format format, vk::ImageLayout dst_image_layout,
                            uint32_t mip_levels, uint32_t base_array_layer,
                            uint32_t layer_count) {
  vk::PhysicalDevice device = *Graphics::Get()->physical_device();

  vk::FormatProperties format_properties = device.getFormatProperties(format);

  // Mip-chain generation requires support for blit source and destination
  assert(format_properties.optimalTilingFeatures &
         vk::FormatFeatureFlagBits::eBlitSrc);
  assert(format_properties.optimalTilingFeatures &
         vk::FormatFeatureFlagBits::eBlitDst);

  CommandBuffer command_buffer;
  vk::CommandBuffer cmd_buf = command_buffer;

  for (uint32_t i = 1; i < mip_levels; i++) {
    vk::ImageMemoryBarrier barrier0 = {};
    barrier0.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
    barrier0.dstAccessMask = vk::AccessFlagBits::eTransferRead;
    barrier0.oldLayout = vk::ImageLayout::eTransferDstOptimal;
    barrier0.newLayout = vk::ImageLayout::eTransferSrcOptimal;
    barrier0.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier0.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier0.image = image;
    barrier0.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    barrier0.subresourceRange.baseMipLevel = i - 1;
    barrier0.subresourceRange.levelCount = 1;
    barrier0.subresourceRange.baseArrayLayer = base_array_layer;
    barrier0.subresourceRange.layerCount = layer_count;

    cmd_buf.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                            vk::PipelineStageFlagBits::eTransfer, {}, 0,
                            nullptr, 0, nullptr, 1, &barrier0);

    vk::ImageBlit image_blit = {};
    image_blit.srcOffsets[1] = vk::Offset3D{
        int32_t(extent.width >> (i - 1)), int32_t(extent.height >> (i - 1)), 1};
    image_blit.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
    image_blit.srcSubresource.mipLevel = i - 1;
    image_blit.srcSubresource.baseArrayLayer = base_array_layer;
    image_blit.srcSubresource.layerCount = layer_count;
    image_blit.dstOffsets[1] = vk::Offset3D{int32_t(extent.width >> i),
                                            int32_t(extent.height >> i), 1};
    image_blit.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
    image_blit.dstSubresource.mipLevel = i;
    image_blit.dstSubresource.baseArrayLayer = base_array_layer;
    image_blit.dstSubresource.layerCount = layer_count;

    cmd_buf.blitImage(image, vk::ImageLayout::eTransferSrcOptimal, image,
                      vk::ImageLayout::eTransferDstOptimal, 1, &image_blit,
                      vk::Filter::eLinear);

    vk::ImageMemoryBarrier barrier1 = {};
    barrier1.srcAccessMask = vk::AccessFlagBits::eTransferRead;
    barrier1.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
    barrier1.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
    barrier1.newLayout = dst_image_layout;
    barrier1.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier1.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier1.image = image;
    barrier1.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    barrier1.subresourceRange.baseMipLevel = i - 1;
    barrier1.subresourceRange.levelCount = 1;
    barrier1.subresourceRange.baseArrayLayer = base_array_layer;
    barrier1.subresourceRange.layerCount = layer_count;

    cmd_buf.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                            vk::PipelineStageFlagBits::eFragmentShader, {}, 0,
                            nullptr, 0, nullptr, 1, &barrier1);
  }

  vk::ImageMemoryBarrier barrier = {};
  barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
  barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
  barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
  barrier.newLayout = dst_image_layout;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = image;
  barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
  barrier.subresourceRange.baseMipLevel = mip_levels - 1;
  barrier.subresourceRange.levelCount = 1;
  barrier.subresourceRange.baseArrayLayer = base_array_layer;
  barrier.subresourceRange.layerCount = layer_count;

  cmd_buf.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                          vk::PipelineStageFlagBits::eFragmentShader, {}, 0,
                          nullptr, 0, nullptr, 1, &barrier);

  command_buffer.submit_idle();
}

void Image::TransitionImageLayout(const vk::Image& image, vk::Format format,
                                  vk::ImageLayout src_image_layout,
                                  vk::ImageLayout dst_image_layout,
                                  vk::ImageAspectFlags image_aspect,
                                  uint32_t mip_levels, uint32_t base_mip_level,
                                  uint32_t layer_count,
                                  uint32_t base_array_layer) {
  CommandBuffer command_buffer;
  vk::CommandBuffer cmd_buf = command_buffer;

  vk::ImageMemoryBarrier image_memory_barrier = {};
  image_memory_barrier.oldLayout = src_image_layout;
  image_memory_barrier.newLayout = dst_image_layout;
  image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  image_memory_barrier.image = image;
  image_memory_barrier.subresourceRange.aspectMask = image_aspect;
  image_memory_barrier.subresourceRange.baseMipLevel = base_mip_level;
  image_memory_barrier.subresourceRange.levelCount = mip_levels;
  image_memory_barrier.subresourceRange.baseArrayLayer = base_array_layer;
  image_memory_barrier.subresourceRange.layerCount = layer_count;

  // Source access mask controls actions that have to be finished on the old
  // layout before it will be transitioned to the new layout.
  switch (src_image_layout) {
    case vk::ImageLayout::eUndefined:
      image_memory_barrier.srcAccessMask = vk::AccessFlags();
      break;
    case vk::ImageLayout::ePreinitialized:
      image_memory_barrier.srcAccessMask = vk::AccessFlagBits::eHostWrite;
      break;
    case vk::ImageLayout::eColorAttachmentOptimal:
      image_memory_barrier.srcAccessMask =
          vk::AccessFlagBits::eColorAttachmentWrite;
      break;
    case vk::ImageLayout::eDepthStencilAttachmentOptimal:
      image_memory_barrier.srcAccessMask =
          vk::AccessFlagBits::eDepthStencilAttachmentWrite;
      break;
    case vk::ImageLayout::eTransferSrcOptimal:
      image_memory_barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
      break;
    case vk::ImageLayout::eTransferDstOptimal:
      image_memory_barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
      break;
    case vk::ImageLayout::eShaderReadOnlyOptimal:
      image_memory_barrier.srcAccessMask = vk::AccessFlagBits::eShaderRead;
      break;
    default:
      // throw std::runtime_error("Unsupported image layout transition source");
      break;
  }

  switch (dst_image_layout) {
    case vk::ImageLayout::eTransferDstOptimal:
      image_memory_barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
      break;
    case vk::ImageLayout::eTransferSrcOptimal:
      image_memory_barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;
      break;
    case vk::ImageLayout::eColorAttachmentOptimal:
      image_memory_barrier.dstAccessMask =
          vk::AccessFlagBits::eColorAttachmentWrite;
      break;
    case vk::ImageLayout::eDepthStencilAttachmentOptimal:
      image_memory_barrier.dstAccessMask =
          image_memory_barrier.dstAccessMask |
          vk::AccessFlagBits::eDepthStencilAttachmentWrite;
      break;
    case vk::ImageLayout::eShaderReadOnlyOptimal:
      if (image_memory_barrier.srcAccessMask == vk::AccessFlags()) {
        image_memory_barrier.srcAccessMask =
            vk::AccessFlagBits::eHostWrite | vk::AccessFlagBits::eTransferWrite;
      }

      image_memory_barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
      break;
    default:
      // throw std::runtime_error("Unsupported image layout transition
      // destination");
      break;
  }

  cmd_buf.pipelineBarrier(vk::PipelineStageFlagBits::eAllCommands,
                          vk::PipelineStageFlagBits::eAllCommands, {}, 0,
                          nullptr, 0, nullptr, 1, &image_memory_barrier);
  command_buffer.submit_idle();
}

void Image::InsertImageMemoryBarrier(
    const CommandBuffer& command_buffer, const vk::Image& image,
    vk::AccessFlags src_access_mask, vk::AccessFlags dst_access_mask,
    vk::ImageLayout old_image_layout, vk::ImageLayout new_image_layout,
    vk::PipelineStageFlags src_stage_mask,
    vk::PipelineStageFlags dst_stage_mask, vk::ImageAspectFlags image_aspect,
    uint32_t mip_levels, uint32_t base_mip_level, uint32_t layer_count,
    uint32_t base_array_layer) {
  vk::ImageMemoryBarrier image_memory_barrier = {};
  image_memory_barrier.srcAccessMask = src_access_mask;
  image_memory_barrier.dstAccessMask = dst_access_mask;
  image_memory_barrier.oldLayout = old_image_layout;
  image_memory_barrier.newLayout = new_image_layout;
  image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  image_memory_barrier.image = image;
  image_memory_barrier.subresourceRange.aspectMask = image_aspect;
  image_memory_barrier.subresourceRange.baseMipLevel = base_mip_level;
  image_memory_barrier.subresourceRange.levelCount = mip_levels;
  image_memory_barrier.subresourceRange.baseArrayLayer = base_array_layer;
  image_memory_barrier.subresourceRange.layerCount = layer_count;

  vk::CommandBuffer cmd_buf = command_buffer;
  cmd_buf.pipelineBarrier(src_stage_mask, dst_stage_mask, {}, 0, nullptr, 0,
                          nullptr, 1, &image_memory_barrier);
}

void Image::CopyBufferToImage(const vk::Buffer& buffer, const vk::Image& image,
                              const vk::Extent3D& extent, uint32_t layer_count,
                              uint32_t base_array_layer) {
  CommandBuffer command_buffer;
  vk::CommandBuffer cmd_buf = command_buffer;

  vk::BufferImageCopy region = {};
  region.bufferOffset = 0;
  region.bufferRowLength = 0;
  region.bufferImageHeight = 0;
  region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
  region.imageSubresource.mipLevel = 0;
  region.imageSubresource.baseArrayLayer = base_array_layer;
  region.imageSubresource.layerCount = layer_count;
  region.imageOffset = vk::Offset3D{0, 0, 0};
  region.imageExtent = extent;

  cmd_buf.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal,
                            1, &region);

  command_buffer.submit_idle();
}

bool Image::CopyImage(const vk::Image& src_image, vk::Image& dst_image,
                      vk::DeviceMemory& dst_image_memory, vk::Format src_format,
                      const vk::Extent3D& extent,
                      vk::ImageLayout src_image_layout, uint32_t mip_level,
                      uint32_t array_layer) {
  vk::PhysicalDevice physical_device = *Graphics::Get()->physical_device();
  auto surface = Graphics::Get()->surface();

  // Checks blit swapchain support.
  auto supports_blit = true;
  vk::FormatProperties format_properties;

  format_properties =
      physical_device.getFormatProperties(surface->format().format);

  if (!(format_properties.optimalTilingFeatures &
        vk::FormatFeatureFlagBits::eBlitSrc)) {
    supports_blit = false;
  }

  format_properties = physical_device.getFormatProperties(src_format);

  if (!(format_properties.linearTilingFeatures &
        vk::FormatFeatureFlagBits::eBlitDst)) {
    supports_blit = false;
  }

  Generate(dst_image, dst_image_memory, extent, vk::Format::eR8G8B8A8Unorm,
           vk::SampleCountFlagBits::e1, vk::ImageTiling::eLinear,
           vk::ImageUsageFlagBits::eTransferDst |
               vk::ImageUsageFlagBits::eTransferSrc,
           vk::MemoryPropertyFlagBits::eHostVisible |
               vk::MemoryPropertyFlagBits::eHostCoherent,
           1, 1, vk::ImageType::e2D);

  CommandBuffer command_buffer;

  InsertImageMemoryBarrier(command_buffer, dst_image, vk::AccessFlagBits(),
                           vk::AccessFlagBits::eTransferWrite,
                           vk::ImageLayout::eUndefined,
                           vk::ImageLayout::eTransferDstOptimal,
                           vk::PipelineStageFlagBits::eTransfer,
                           vk::PipelineStageFlagBits::eTransfer,
                           vk::ImageAspectFlagBits::eColor, 1, 0, 1, 0);

  InsertImageMemoryBarrier(
      command_buffer, src_image, vk::AccessFlagBits::eMemoryRead,
      vk::AccessFlagBits::eTransferRead, src_image_layout,
      vk::ImageLayout::eTransferDstOptimal,
      vk::PipelineStageFlagBits::eTransfer,
      vk::PipelineStageFlagBits::eTransfer, vk::ImageAspectFlagBits::eColor, 1,
      mip_level, 1, array_layer);

  vk::CommandBuffer cmd_buf = command_buffer;
  if (supports_blit) {
    vk::Offset3D blit_size = {static_cast<int32_t>(extent.width),
                              static_cast<int32_t>(extent.height),
                              static_cast<int32_t>(extent.depth)};

    vk::ImageBlit image_blit_region = {};
    image_blit_region.srcSubresource.aspectMask =
        vk::ImageAspectFlagBits::eColor;
    image_blit_region.srcSubresource.mipLevel = mip_level;
    image_blit_region.srcSubresource.baseArrayLayer = array_layer;
    image_blit_region.srcSubresource.layerCount = 1;
    image_blit_region.srcOffsets[1] = blit_size;
    image_blit_region.dstSubresource.aspectMask =
        vk::ImageAspectFlagBits::eColor;
    image_blit_region.dstSubresource.mipLevel = 0;
    image_blit_region.dstSubresource.baseArrayLayer = 0;
    image_blit_region.dstSubresource.layerCount = 1;
    image_blit_region.dstOffsets[1] = blit_size;

    cmd_buf.blitImage(src_image, vk::ImageLayout::eTransferSrcOptimal,
                      dst_image, vk::ImageLayout::eTransferDstOptimal, 1,
                      &image_blit_region, vk::Filter::eNearest);
  } else {
    vk::ImageCopy imageCopyRegion = {};
    imageCopyRegion.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
    imageCopyRegion.srcSubresource.mipLevel = mip_level;
    imageCopyRegion.srcSubresource.baseArrayLayer = array_layer;
    imageCopyRegion.srcSubresource.layerCount = 1;
    imageCopyRegion.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
    imageCopyRegion.dstSubresource.mipLevel = 0;
    imageCopyRegion.dstSubresource.baseArrayLayer = 0;
    imageCopyRegion.dstSubresource.layerCount = 1;
    imageCopyRegion.extent = extent;
    cmd_buf.copyImage(src_image, vk::ImageLayout::eTransferSrcOptimal,
                      dst_image, vk::ImageLayout::eTransferDstOptimal, 1,
                      &imageCopyRegion);
  }

  InsertImageMemoryBarrier(
      command_buffer, dst_image, vk::AccessFlagBits::eTransferWrite,
      vk::AccessFlagBits::eMemoryRead, vk::ImageLayout::eTransferDstOptimal,
      vk::ImageLayout::eGeneral, vk::PipelineStageFlagBits::eTransfer,
      vk::PipelineStageFlagBits::eTransfer, vk::ImageAspectFlagBits::eColor, 1,
      0, 1, 0);

  // Transition back the image after the blit is done.
  InsertImageMemoryBarrier(
      command_buffer, src_image, vk::AccessFlagBits::eTransferWrite,
      vk::AccessFlagBits::eMemoryRead, vk::ImageLayout::eTransferSrcOptimal,
      src_image_layout, vk::PipelineStageFlagBits::eTransfer,
      vk::PipelineStageFlagBits::eTransfer, vk::ImageAspectFlagBits::eColor, 1,
      mip_level, 1, array_layer);

  command_buffer.submit_idle();

  return supports_blit;
}

}  // namespace cx::graphics
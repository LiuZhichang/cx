#pragma once

#include <vulkan/vulkan.hpp>

#include "cx/graphics/command/command_buffer.h"
#include "cx/graphics/descriptor/descriptor.h"
#include "cx/maths/vector2.h"

namespace cx::graphics {

/**
 * @brief 图像类，呈现在屏幕上的实体
 *
 */
class Image : public Descriptor {
 public:
  explicit Image(vk::Filter filter, vk::SamplerAddressMode sample_address_mode,
                 vk::SampleCountFlagBits sample_count, vk::ImageLayout layout,
                 vk::ImageUsageFlags usage, vk::Format format,
                 uint32_t mip_level, uint32_t array_layers,
                 const vk::Extent3D& extent);

  ~Image();

  operator const vk::Image&() const { return m_image; }

  const vk::Extent3D extent() const { return m_extent; }

  Vector2ui size() const { return Vector2ui{m_extent.width, m_extent.height}; }
  vk::Format fromat() const { return m_format; }
  vk::ImageUsageFlags usage() const { return m_usage; }

  uint32_t mip_level() const { return m_mip_level; }
  uint32_t array_layers() const { return m_array_layers; }

  vk::SampleCountFlagBits samples_count() { return m_sample_count; }
  vk::SamplerAddressMode sampler_address_mode() const {
    return m_sampler_address_mode;
  }
  const vk::Sampler& sampler() const { return m_sampler; }

  const vk::DeviceMemory& memmory() const { return m_memory; }
  const vk::ImageView& view() const { return m_view; }
  const vk::Image& handle() const { return m_image; }

  WriteDescriptorSet write_descriptor(
      uint32_t binding, vk::DescriptorType descriptor_type,
      const std::optional<OffsetSize>& offsetSize) const override;

  CX_STATIC vk::DescriptorSetLayoutBinding
  descriptorset_layout(uint32_t binding, vk::DescriptorType descriptor_type,
                       vk::ShaderStageFlags stage, uint32_t count);

  CX_STATIC uint32_t GetMipLevels(const vk::Extent3D& extent);

  /**
   * @brief 查找支持的图像格式
   *
   * @param formats 格式集
   * @param tiling 图像平铺模式 线性/优化 平铺
   * @param feature 格式特性标志
   * @return vk::Format 支持的格式
   */
  CX_STATIC vk::Format FindSupportFormat(const std::vector<vk::Format>& formats,
                                         vk::ImageTiling tiling,
                                         vk::FormatFeatureFlags feature);

  CX_STATIC bool HasDepth(vk::Format format);

  CX_STATIC bool HasStencil(vk::Format format);

  CX_STATIC void Generate(vk::Image& image, vk::DeviceMemory memory,
                          const vk::Extent3D extent, vk::Format format,
                          vk::SampleCountFlagBits sample_count,
                          vk::ImageTiling tiling, vk::ImageUsageFlags usage,
                          vk::MemoryPropertyFlags properties,
                          uint32_t mip_level, uint32_t array_layers,
                          vk::ImageType type);

  CX_STATIC void GenerateSampler(vk::Sampler& sampler, vk::Filter filter,
                                 vk::SamplerAddressMode address_mode,
                                 bool anisotropic, uint32_t mip_levels);

  CX_STATIC void GenerateView(const vk::Image& image, vk::ImageView& view,
                              vk::ImageViewType type, vk::Format format,
                              vk::ImageAspectFlags image_aspect_flags,
                              uint32_t mip_levels, uint32_t base_mip_levels,
                              uint32_t layer_count, uint32_t base_layer_count);

  CX_STATIC void GenerateMipmaps(const vk::Image& image,
                                 const vk::Extent3D& extent, vk::Format format,
                                 vk::ImageLayout dst_image_layout,
                                 uint32_t mip_levels, uint32_t base_array_layer,
                                 uint32_t layer_count);

  CX_STATIC void TransitionImageLayout(
      const vk::Image& image, vk::Format format,
      vk::ImageLayout src_image_layout, vk::ImageLayout dst_image_layout,
      vk::ImageAspectFlags image_aspect, uint32_t mip_levels,
      uint32_t base_mip_level, uint32_t layer_count, uint32_t base_array_layer);

  static void InsertImageMemoryBarrier(
      const CommandBuffer& command_buffer, const vk::Image& image,
      vk::AccessFlags src_access_mask, vk::AccessFlags dst_access_mask,
      vk::ImageLayout old_image_layout, vk::ImageLayout new_image_layout,
      vk::PipelineStageFlags src_stage_mask,
      vk::PipelineStageFlags dst_stage_mask, vk::ImageAspectFlags image_aspect,
      uint32_t mip_levels, uint32_t base_mip_level, uint32_t layer_count,
      uint32_t base_array_layer);

  static void CopyBufferToImage(const vk::Buffer& buffer,
                                const vk::Image& image,
                                const vk::Extent3D& extent,
                                uint32_t layer_count,
                                uint32_t base_array_layer);

  static bool CopyImage(const vk::Image& src_image, vk::Image& dst_image,
                        vk::DeviceMemory& dst_image_memory,
                        vk::Format src_format, const vk::Extent3D& extent,
                        vk::ImageLayout src_image_layout, uint32_t mip_level,
                        uint32_t array_layer);

 protected:
  vk::Image m_image;
  vk::ImageView m_view;  // 图像视图，用于呈现图像，放在帧缓冲中
  vk::ImageUsageFlags m_usage;  // 使用阶段
  vk::ImageLayout m_layout;     // 图像布局

  vk::DeviceMemory m_memory;

  vk::Sampler m_sampler;
  vk::SampleCountFlagBits m_sample_count;         // 采样数
  vk::SamplerAddressMode m_sampler_address_mode;  //

  vk::Extent3D m_extent;  // 大小
  vk::Format m_format;    // 图像格式
  vk::Filter m_filter;    //

  uint32_t m_mip_level;
  uint32_t m_array_layers;
};
}  // namespace cx::graphics
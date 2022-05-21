#pragma once

#include <filesystem>
#include <memory>

#include "cx/common/internal.h"
#include "cx/graphics/image/image.h"
#include "cx/maths/vector2.h"
#include "cx/utils/raster/bitmap.h"

namespace cx::graphics {

class Image2D : public Image {
 public:
  typedef std::shared_ptr<Image2D> ptr;
  CX_STATIC ptr Generate();
  CX_STATIC ptr Generate(const std::filesystem::path& path,
                         vk::Filter filter = vk::Filter::eLinear,
                         vk::SamplerAddressMode sampler_address_mode =
                             vk::SamplerAddressMode::eRepeat,
                         bool anisotropic = true, bool mip_map = true);

  explicit Image2D(const std::filesystem::path& path,
                   vk::Filter filter = vk::Filter::eLinear,
                   vk::SamplerAddressMode sampler_address_mode =
                       vk::SamplerAddressMode::eRepeat,
                   bool anisotropic = true, bool mipmap = true,
                   bool load = true);

  explicit Image2D(
      const Vector2ui& extent, vk::Format format = vk::Format::eR8G8B8A8Unorm,
      vk::ImageLayout layout = vk::ImageLayout::eShaderReadOnlyOptimal,
      vk::ImageUsageFlags usage = vk::ImageUsageFlagBits::eColorAttachment |
                                  vk::ImageUsageFlagBits::eStorage,
      vk::Filter filter = vk::Filter::eLinear,
      vk::SamplerAddressMode sampler_address_mode =
          vk::SamplerAddressMode::eClampToEdge,
      vk::SampleCountFlagBits sample_count = vk::SampleCountFlagBits::e1,
      bool anisotropic = false, bool mipmap = false);

  explicit Image2D(
      std::unique_ptr<raster::Bitmap>&& bitmap,
      vk::Format format = vk::Format::eR8G8B8A8Unorm,
      vk::ImageLayout layout = vk::ImageLayout::eShaderReadOnlyOptimal,
      vk::ImageUsageFlags usage = vk::ImageUsageFlagBits::eColorAttachment |
                                  vk::ImageUsageFlagBits::eStorage,
      vk::Filter filter = vk::Filter::eLinear,
      vk::SamplerAddressMode sampler_address_mode =
          vk::SamplerAddressMode::eClampToEdge,
      vk::SampleCountFlagBits sample_count = vk::SampleCountFlagBits::e1,
      bool anisotropic = false, bool mipmap = false);

  void set_pixels(const uint8_t* pixels, uint32_t layer_count,
                  uint32_t base_array_layer);

  std::filesystem::path fila_path() const { return m_filename; }

  bool is_anisotropic() const { return m_anisotropic; }
  bool is_mipmap() const { return m_mipmap; }
  uint32_t components() const { return m_components; }

 private:
  void load_data(std::unique_ptr<raster::Bitmap> bitmap = nullptr);

  std::filesystem::path m_filename;

  bool m_anisotropic;
  bool m_mipmap;

  uint32_t m_components;  // color component
};

}  // namespace cx::graphics
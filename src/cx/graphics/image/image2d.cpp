#include "image2d.h"

#include <cstring>

#include "cx/common/defer.h"
#include "cx/graphics/buffers/buffer.h"

namespace cx::graphics {

Image2D::ptr Image2D::Generate() {}

Image2D::ptr Image2D::Generate(const std::filesystem::path& path,
                               vk::Filter filter,
                               vk::SamplerAddressMode sampler_address_mode,
                               bool anisotropic, bool mip_map) {}

Image2D::Image2D(const std::filesystem::path& path, vk::Filter filter,
                 vk::SamplerAddressMode sampler_address_mode, bool anisotropic,
                 bool mipmap, bool load)
    : Image(filter, sampler_address_mode, vk::SampleCountFlagBits::e1,
            vk::ImageLayout::eShaderReadOnlyOptimal,
            vk::ImageUsageFlagBits::eTransferSrc |
                vk::ImageUsageFlagBits::eTransferDst |
                vk::ImageUsageFlagBits::eSampled,
            vk::Format::eR8G8B8A8Unorm, 1, 1, {0, 0, 1}),
      m_filename(path),
      m_anisotropic(anisotropic),
      m_mipmap(mipmap) {
  if (load) load_data();
}

Image2D::Image2D(const Vector2ui& extent, vk::Format format,
                 vk::ImageLayout layout, vk::ImageUsageFlags usage,
                 vk::Filter filter, vk::SamplerAddressMode sampler_address_mode,
                 vk::SampleCountFlagBits sample_count, bool anisotropic,
                 bool mipmap)
    : Image(filter, sampler_address_mode, sample_count, layout, usage, format,
            1, 1, {0, 0, 1}),
      m_anisotropic(anisotropic),
      m_mipmap(mipmap),
      m_components(4) {
  load_data();
}

Image2D::Image2D(std::unique_ptr<raster::Bitmap>&& bitmap, vk::Format format,
                 vk::ImageLayout layout, vk::ImageUsageFlags usage,
                 vk::Filter filter, vk::SamplerAddressMode sampler_address_mode,
                 vk::SampleCountFlagBits sample_count, bool anisotropic,
                 bool mipmap)
    : Image(filter, sampler_address_mode, sample_count, layout, usage, format,
            1, 1, {bitmap->size().w, bitmap->size().h, 1}),
      m_anisotropic(anisotropic),
      m_mipmap(mipmap),
      m_components(bitmap->bytes_per_pixel()) {
  load_data(std::move(bitmap));
}

void Image2D::set_pixels(const uint8_t* pixels, uint32_t layer_count,
                         uint32_t base_array_layer) {
  Buffer buffer_staging(m_extent.width * m_extent.height * m_array_layers,
                        vk::BufferUsageFlagBits::eTransferSrc,
                        vk::MemoryPropertyFlagBits::eHostVisible |
                            vk::MemoryPropertyFlagBits::eHostCoherent);

  void* data;
  buffer_staging.map_memory(&data);
  std::memcpy(&data, pixels, buffer_staging.size());
  buffer_staging.unmap_memory();

  CopyBufferToImage(buffer_staging, m_image, m_extent, layer_count,
                    base_array_layer);
}

void Image2D::load_data(std::unique_ptr<raster::Bitmap> bitmap) {
  if (!m_filename.empty() && !bitmap) {
    bitmap = std::make_unique<raster::Bitmap>(m_filename);
    m_extent = vk::Extent3D{bitmap->size().w, bitmap->size().h, 1};
    m_components = bitmap->bytes_per_pixel();
  }

  if (m_extent.width == 0 || m_extent.width == 0) return;

  m_mip_level = is_mipmap() ? GetMipLevels(m_extent) : 1;

  Image::Generate(m_image, m_memory, m_extent, m_format, m_sample_count,
                  vk::ImageTiling::eOptimal, m_usage,
                  vk::MemoryPropertyFlagBits::eDeviceLocal, m_mip_level,
                  m_array_layers, vk::ImageType::e2D);
  Image::GenerateSampler(m_sampler, m_filter, m_sampler_address_mode,
                         m_anisotropic, m_mip_level);
  Image::GenerateView(m_image, m_view, vk::ImageViewType::e2D, m_format,
                      vk::ImageAspectFlagBits::eColor, m_mip_level, 0,
                      m_array_layers, 0);

  if (bitmap || m_mipmap) {
    TransitionImageLayout(m_image, m_format, vk::ImageLayout::eUndefined,
                          vk::ImageLayout::eTransferDstOptimal,
                          vk::ImageAspectFlagBits::eColor, m_mip_level, 0,
                          m_array_layers, 0);
  }

  if (bitmap) {
    Buffer buffer_stagin(bitmap->length(),
                         vk::BufferUsageFlagBits::eTransferSrc,
                         vk::MemoryPropertyFlagBits::eHostVisible |
                             vk::MemoryPropertyFlagBits::eHostCoherent);
    uint8_t* data;
    buffer_stagin.map_memory((void**)&data);
    defer { buffer_stagin.unmap_memory(); };

    std::memcpy(data, bitmap->data().get(), buffer_stagin.size());
    CopyBufferToImage(buffer_stagin, m_image, m_extent, m_array_layers, 0);
  }

  if (m_mipmap) {
    GenerateMipmaps(m_image, m_extent, m_format, m_layout, m_mip_level,
                    m_array_layers, 0);

  } else if (bitmap) {
    TransitionImageLayout(
        m_image, m_format, vk::ImageLayout::eTransferDstOptimal, m_layout,
        vk::ImageAspectFlagBits::eColor, m_mip_level, 0, m_array_layers, 0);
  } else {
    TransitionImageLayout(m_image, m_format, vk::ImageLayout::eUndefined,
                          m_layout, vk::ImageAspectFlagBits::eColor,
                          m_mip_level, 0, m_array_layers, 0);
  }
}
}  // namespace cx::graphics
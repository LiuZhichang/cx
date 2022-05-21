#include "bitmap.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

#include <fstream>

#include "cx/utils/fileop/file.h"

namespace cx::raster {

Bitmap::Bitmap(std::filesystem::path filename)
    : m_filename(std::move(filename)) {
  Load(m_filename);
}

Bitmap::Bitmap(const Vector2ui &size, uint32_t bytes_per_pixel)
    : m_data(
          std::make_unique<uint8_t[]>(CalculateLength(size, bytes_per_pixel))),
      m_size(size),
      m_bytes_per_pixel(bytes_per_pixel) {}

Bitmap::Bitmap(std::unique_ptr<uint8_t[]> &&data, const Vector2ui &size,
               uint32_t bytes_per_pixel)
    : m_data(std::move(data)),
      m_size(size),
      m_bytes_per_pixel(bytes_per_pixel) {}

void Bitmap::Load(const std::filesystem::path &filename) {
  Registry()[filename.extension().string()].first(this, filename);

  std::string file_loaded;
  FileOp::read(filename, file_loaded);

  if (!file_loaded.size()) {
    return;
  }

  m_data = std::unique_ptr<uint8_t[]>(stbi_load_from_memory(
      reinterpret_cast<uint8_t *>(file_loaded.data()),
      static_cast<uint32_t>(file_loaded.size()),
      reinterpret_cast<int32_t *>(&m_size.x),
      reinterpret_cast<int32_t *>(&m_size.y),
      reinterpret_cast<int32_t *>(&m_bytes_per_pixel), STBI_rgb_alpha));
  m_bytes_per_pixel = 4;
}

void Bitmap::Write(const std::filesystem::path &filename) const {
  if (auto parentPath = filename.parent_path(); !parentPath.empty())
    std::filesystem::create_directories(parentPath);

  // Registry()[filename.extension().string()].second(this, filename);

  std::ofstream os(filename, std::ios::binary | std::ios::out);
  int32_t len;

  std::unique_ptr<uint8_t[]> png(
      stbi_write_png_to_mem(m_data.get(), m_size.x * m_bytes_per_pixel,
                            m_size.x, m_size.y, m_bytes_per_pixel, &len));
  os.write(reinterpret_cast<char *>(png.get()), len);
}

uint32_t Bitmap::length() const {
  return m_size.x * m_size.y * m_bytes_per_pixel;
}

uint32_t Bitmap::CalculateLength(const Vector2ui &size,
                                 uint32_t bytes_per_pixel) {
  return size.x * size.y * bytes_per_pixel;
}

}  // namespace cx::raster
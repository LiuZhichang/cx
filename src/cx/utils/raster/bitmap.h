#pragma once

#include <filesystem>
#include <functional>
#include <unordered_map>

#include "cx/maths/vector2.h"

namespace cx::raster {

template <typename T>
class RasterFactory {
 public:
  using LoadMethod = std::function<void(T *, const std::filesystem::path &)>;
  using WriteMethod =
      std::function<void(const T *, const std::filesystem::path &)>;
  using RegistryMap =
      std::unordered_map<std::string, std::pair<LoadMethod, WriteMethod>>;

  virtual ~RasterFactory() = default;

  static RegistryMap &Registry() {
    static RegistryMap s_rm;
    return s_rm;
  }

  template <typename RegT>
  class Registrar {
   protected:
    template <typename... Args>
    static bool Register(Args &&...names) {
      for (std::string &&name : {names...})
        RasterFactory::Registry()[name] = std::make_pair(&T::Load, &T::Write);
      return true;
    }
  };
};

class Bitmap : public RasterFactory<Bitmap> {
 public:
  Bitmap() = default;
  explicit Bitmap(std::filesystem::path filename);
  explicit Bitmap(const Vector2ui &size, uint32_t bytes_per_pixel = 4);
  Bitmap(std::unique_ptr<uint8_t[]> &&data, const Vector2ui &size,
         uint32_t bytes_per_pixel = 4);
  ~Bitmap() = default;

  void Load(const std::filesystem::path &filename);
  void Write(const std::filesystem::path &filename) const;

  explicit operator bool() const noexcept { return !m_data; }

  uint32_t length() const;

  const std::filesystem::path &filename() const { return m_filename; }
  void set_filename(const std::filesystem::path &filename) {
    m_filename = filename;
  }

  const std::unique_ptr<uint8_t[]> &data() const { return m_data; }
  std::unique_ptr<uint8_t[]> &data() { return m_data; }
  void set_data(std::unique_ptr<uint8_t[]> &&data) { m_data = std::move(data); }

  const Vector2ui &size() const { return m_size; }
  void set_size(const Vector2ui &size) { m_size = size; }

  uint32_t bytes_per_pixel() const { return m_bytes_per_pixel; }
  void SetBytesPerPixel(uint32_t bytes_per_pixel) {
    m_bytes_per_pixel = bytes_per_pixel;
  }

 private:
  static uint32_t CalculateLength(const Vector2ui &size,
                                  uint32_t bytesPerPixel);

  std::filesystem::path m_filename;
  std::unique_ptr<uint8_t[]> m_data;
  Vector2ui m_size;
  uint32_t m_bytes_per_pixel = 0;
};

}  // namespace cx::raster
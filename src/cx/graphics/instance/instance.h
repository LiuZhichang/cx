#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>

namespace cx {

class GInstance {
 public:
  GInstance();
  GInstance(const char** extensions, uint8_t extensions_count);

  template <typename T = const char*, typename Container = std::vector<T>>
  GInstance(const Container& extensions) {
    static_assert(std::is_same<T, const char*>::value,
                  "type must be [const char*]");

    init(extensions);
  }

  ~GInstance();

  const vk::Instance& get() const { return m_instance; }

  bool isEnableValidations() const;

 private:
  template <typename T = const char*, typename Container = std::vector<T>>
  void init(const Container& extensions);

 private:
  vk::Instance m_instance = VK_NULL_HANDLE;
};

}  // namespace cx
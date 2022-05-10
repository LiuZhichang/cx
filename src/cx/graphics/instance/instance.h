/**
 * @file instance.h
 * @author liuzhichang (lzc3318619633@163.com)
 * @brief vulkan实例类
 * @version 0.1
 * @date 2022-05-10
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

#include <cx/common/common.h>

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

  const vk::Instance& handle() const { return m_instance; }

  bool isEnableValidations() const;

 private:
  template <typename T = const char*, typename Container = std::vector<T>>
  void init(const Container& extensions);

 private:
  vk::Instance m_instance = VK_NULL_HANDLE;
};

}  // namespace cx
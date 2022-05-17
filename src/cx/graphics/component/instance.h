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

namespace cx::graphics {

/**
 * @brief Vulkan 实例
 *
 */
class Instance {
 public:
  /**
   * @brief 无参构造
   *
   */
  Instance();

  /**
   * @brief 构造
   *
   * @param extensions 扩展名称集
   * @param extensions_count 扩展个数
   */
  Instance(const char** extensions, uint8_t extensions_count);

  /**
   * @brief 构造
   *
   * @param extensions 扩展集
   */
  Instance(const std::vector<const char*>& extensions);

  /**
   * @brief 析构函数
   *
   */
  ~Instance();

  /**
   * @brief 获取句柄(instance实例)
   *
   * @return const vk::Instance& 实例引用
   */
  const vk::Instance& handle() const { return m_instance; }

  bool is_enable_validations() const;

  /**
   * @brief 获取指定内存类型的下标
   *
   * @param memoryProperties 内存属性
   * @param typeBits 内存类型
   * @param requirementsMask 掩码
   * @return uint32_t 下标
   */
  static uint32_t FindMemoryTypeIndex(
      vk::PhysicalDeviceMemoryProperties const& memoryProperties,
      uint32_t typeBits, vk::MemoryPropertyFlags requirementsMask);

  operator const vk::Instance&() const { return m_instance; }

  bool is_valid() const { return m_instance; }

 private:
  /**
   * @brief 初始化实例
   *
   * @param extensions 扩展集
   */
  void init(const std::vector<const char*>& extensions);

  /**
   * @brief 检查是否支持验证层
   *
   * @param layers 验证层集
   * @return true 支持
   * @return false 不支持
   */
  bool check_validation_layer_support(std::vector<const char*> const& layers);

 private:
  vk::Instance m_instance;  // vulkan实例
};

}  // namespace cx::graphics
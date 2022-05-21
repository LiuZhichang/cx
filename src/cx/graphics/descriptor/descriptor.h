#pragma once

#include <optional>
#include <vulkan/vulkan.hpp>

namespace cx::graphics {

class OffsetSize {
 public:
  OffsetSize(uint32_t offset, uint32_t size) : m_offset(offset), m_size(size) {}

  uint32_t GetOffset() const { return m_offset; }
  uint32_t GetSize() const { return m_size; }

  bool operator==(const OffsetSize &rhs) const {
    return m_offset == rhs.m_offset && m_size == rhs.m_size;
  }

  bool operator!=(const OffsetSize &rhs) const { return !operator==(rhs); }

 private:
  uint32_t m_offset;
  uint32_t m_size;
};

class WriteDescriptorSet {
 public:
  WriteDescriptorSet(const vk::WriteDescriptorSet &write_descriptor_set,
                     const vk::DescriptorImageInfo &image_info)
      : m_write_descriptor_set(write_descriptor_set),
        m_image_info(std::make_unique<vk::DescriptorImageInfo>(image_info)) {
    m_write_descriptor_set.pImageInfo = m_image_info.get();
  }

  WriteDescriptorSet(const vk::WriteDescriptorSet &write_descriptor_set,
                     const vk::DescriptorBufferInfo &buffer_info)
      : m_write_descriptor_set(write_descriptor_set),
        m_buffer_info(std::make_unique<vk::DescriptorBufferInfo>(buffer_info)) {
    m_write_descriptor_set.pBufferInfo = m_buffer_info.get();
  }

  const vk::WriteDescriptorSet &GetWriteDescriptorSet() const {
    return m_write_descriptor_set;
  }

 private:
  vk::WriteDescriptorSet m_write_descriptor_set;
  std::unique_ptr<vk::DescriptorImageInfo> m_image_info;
  std::unique_ptr<vk::DescriptorBufferInfo> m_buffer_info;
};

class Descriptor {
 public:
  Descriptor() = default;

  virtual ~Descriptor() = default;

  virtual WriteDescriptorSet write_descriptor(
      uint32_t binding, vk::DescriptorType descriptor_type,
      const std::optional<OffsetSize> &offset_size) const = 0;

 private:
};

}  // namespace cx::graphics
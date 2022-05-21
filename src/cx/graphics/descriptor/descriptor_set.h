#pragma once

#include "cx/common/internal.h"
#include "cx/graphics/descriptor/descriptor.h"
#include "cx/graphics/pipeline/pipeline.h"

namespace cx::graphics {

class Descriptor;
class WriteDescriptorSet;
class DescriptorSet {
 public:
  explicit DescriptorSet(const Pipeline &pipeline);
  ~DescriptorSet();

  CX_STATIC void Update(
      const std::vector<vk::WriteDescriptorSet> &descriptorWrites);

  void bind(const CommandBuffer &commandBuffer) const;

  const vk::DescriptorSet &descriptorset() const { return m_descriptorset; }

 private:
  vk::PipelineLayout m_pipeline_layout;
  vk::PipelineBindPoint m_pipeline_bind_point;
  vk::DescriptorPool m_descriptor_pool;
  vk::DescriptorSet m_descriptorset = VK_NULL_HANDLE;
};

}  // namespace cx::graphics
#include "descriptor_set.h"

namespace cx::graphics {

DescriptorSet::DescriptorSet(const Pipeline &pipeline)
    : m_pipeline_bind_point(pipeline.pipeline_bind_point()),
      m_pipeline_layout(pipeline.pipeline_layout()),
      m_descriptor_pool(pipeline.descriptor_pool()) {}
DescriptorSet::~DescriptorSet() {}

void DescriptorSet::Update(
    const std::vector<vk::WriteDescriptorSet> &descriptorWrites) {}

void DescriptorSet::bind(const CommandBuffer &commandBuffer) const {}

}  // namespace cx::graphics
#pragma once

#include "cx/graphics/command/command_buffer.h"

namespace cx::graphics {

class Pipeline {
 public:
  typedef std::pair<uint32_t, uint32_t> Stage;

  Pipeline() = default;
  virtual ~Pipeline() = default;

  void bind(const CommandBuffer& cmd_buffer) const {
    vk::CommandBuffer buffer = cmd_buffer;
    // buffer.bindPipeline();
  }
  virtual bool is_push_descriptor() const = 0;
  virtual const vk::Pipeline& handle() const = 0;
  virtual const vk::DescriptorSetLayout& descriptor_set_layout() const = 0;
  virtual const vk::DescriptorPool& descriptor_pool() const = 0;
  virtual const vk::PipelineBindPoint& pipeline_bind_point() const = 0;
  virtual const vk::PipelineLayout& pipeline_layout() const = 0;

 protected:
  vk::Pipeline m_pipeline;
  vk::PipelineLayout m_pipeline_layout;
  vk::PipelineBindPoint m_pipeline_bind_point;
  vk::DescriptorPool m_descriptor_pool;
  vk::DescriptorSet m_descript_set;
};

}  // namespace cx::graphics
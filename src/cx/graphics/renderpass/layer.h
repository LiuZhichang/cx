/**
 * @file layer.h
 * @author liuzhichang (lzc3318619633@163.com)
 * @brief 图层类
 * @version 0.1
 * @date 2022-05-10
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

#include "cx/graphics/pipeline/pipeline.h"

namespace cx::graphics {

class Layer {
 public:
  explicit Layer(const Pipeline::Stage& stage) : m_stage(std::move(stage)) {}
  virtual ~Layer() = default;

  virtual void on_attach() = 0;
  virtual void on_detach() = 0;

  virtual void render(const CommandBuffer& command_buffer) = 0;

  const Pipeline::Stage& stage() const { return m_stage; }

 private:
  Pipeline::Stage m_stage;
  bool m_enable;
};

}  // namespace cx::graphics
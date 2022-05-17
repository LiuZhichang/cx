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

namespace cx::graphics {

class Layer {
 public:
  virtual void onAttach() = 0;
  virtual void OnDetach() = 0;

 private:
};

}  // namespace cx
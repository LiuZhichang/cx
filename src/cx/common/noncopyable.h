/**
 * @file noncopyable.h
 * @author liuzhichang (lzc3318619633@163.com)
 * @brief
 * @version 0.1
 * @date 2022-05-08
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

namespace cx {

class Noncopyable {
 public:
  Noncopyable() = default;
  ~Noncopyable() = default;
  Noncopyable(const Noncopyable&) = delete;
  Noncopyable& operator=(const Noncopyable&) = delete;
};

}  // namespace cx

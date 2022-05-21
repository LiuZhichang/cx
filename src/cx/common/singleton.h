/**
 * @file singleton.h
 * @author liuzhichang (lzc3318619633@163.com)
 * @brief 单例模板类
 * @version 0.1
 * @date 2022-05-08
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include <memory>

namespace cx {

template <typename T>
class Singleton {
 public:
  typedef T* ptr;

  static ptr Self() { return &self; }

  Singleton(T&&) = delete;
  Singleton(const T&) = delete;
  void operator=(const T&) = delete;

 protected:
  Singleton() = default;
  virtual ~Singleton() = default;

  CX_INLINE static T self;
};

template <class T>
class SingletonPtr {
 public:
  typedef std::shared_ptr<T> ptr;
  static ptr Self() { return m_inst_ptr; }

  SingletonPtr(T&&) = delete;
  SingletonPtr(const T&) = delete;
  void operator=(const T&) = delete;

 protected:
  SingletonPtr() = default;
  virtual ~SingletonPtr() = default;

  CX_INLINE static ptr m_inst_ptr = ptr(new T());
};

}  // namespace cx

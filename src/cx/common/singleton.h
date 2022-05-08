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

#define CX_SINGLETON_FRIEND(cls) friend Singleton<cls>;

namespace cx {

template <typename T>
class Singleton {
 public:
  static T* Self() {
    static T self;
    return &self;
  }

  Singleton(T&&) = delete;
  Singleton(const T&) = delete;
  void operator=(const T&) = delete;

 protected:
  Singleton() = default;
  virtual ~Singleton() = default;
};

template <class T>
class SingletonPtr {
 public:
  static std::shared_ptr<T> Self() {
    static std::shared_ptr<T> ptr = std::make_shared<T>();
    return ptr;
  }

  SingletonPtr(T&&) = delete;
  SingletonPtr(const T&) = delete;
  void operator=(const T&) = delete;

 protected:
  SingletonPtr() = default;
  virtual ~SingletonPtr() = default;
};

}  // namespace cx

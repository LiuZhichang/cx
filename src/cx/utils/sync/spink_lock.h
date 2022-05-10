/**
 * @file spink_lock.h
 * @author liuzhichang (lzc3318619633@163.com)
 * @brief 基于std::atomic_flag的自旋锁
 * @version 0.1
 * @date 2022-05-10
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

#include <cx/common/internal.h>

#include <atomic>

/**
 * @brief 自旋锁
 *
 */
class SpinkLock {
 public:
  SpinkLock() : flag{false} {}

  CX_INLINE void CX_API lock() {
    while (flag.test_and_set(std::memory_order_acquire))
      ;
  }

  CX_INLINE void CX_API unlock() { flag.clear(std::memory_order_release); }

  CX_INLINE bool try_lock() {
    return !flag.test_and_set(std::memory_order_acquire);
  }

 private:
  std::atomic_flag flag;
};
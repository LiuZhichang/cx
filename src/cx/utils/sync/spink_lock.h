#pragma once

#include <cx/common/internal.h>

#include <atomic>

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
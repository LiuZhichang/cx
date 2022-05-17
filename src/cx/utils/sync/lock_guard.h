#pragma once

namespace cx::sync {

template <typename Mutex>
class FakeGuard {
 public:
  FakeGuard(Mutex& mutex) {}

  ~FakeGuard(Mutex& mutex) {}
};

}
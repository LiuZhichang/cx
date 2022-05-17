#pragma once

#include <mutex>

class RWLock {
 public:
  RWLock() : m_wCount(0) {}

  void read_lock() {}

  void write_lock() {}

  void try_lock() {}

  void unlock() {}

 private:
  std::mutex m_rMutex;
  std::mutex m_wMutex;
  std::mutex m_cMutex;
  int m_wCount;
};
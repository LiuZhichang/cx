#pragma once

#include <cstdint>

#include "noncopyable.h"

namespace cx {

template <typename Base>
class ModuleFactory {
 public:
 private:
};

class Module : public Noncopyable {
 public:
  enum class Stage : uint8_t {
    eNone,
    eInvariably,
    ePre,
    eNormal,
    ePost,
    eRender
  };

  virtual void update() = 0;

 private:
};

}  // namespace cx
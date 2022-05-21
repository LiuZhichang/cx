#pragma once

#include "cx/common/internal.h"
#include "cx/common/log/log.h"

namespace cx::log {

class Loggers {
 public:
  CX_INLINE CX_STATIC Logger::ptr engine = CX_LOGGER("engine");
  CX_INLINE CX_STATIC Logger::ptr core = CX_LOGGER("core");
};

};  // namespace cx::log
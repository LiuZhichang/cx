#pragma once

#include "log/log.h"

namespace cx::log {

class Loggers {
 public:
  inline static Logger::ptr engine = CX_LOGGER("engine");
  inline static Logger::ptr core = CX_LOGGER("core");
};

};  // namespace cx::log
#pragma once

#include <cstdint>
#include <string>

namespace cx {
/**
 * @brief 版本类， 用于界定当前版本[用于App/Engine版本]
 *
 */
struct Version {
  Version(uint8_t major, uint8_t minor, uint8_t patch)
      : major(major), minor(minor), patch(patch) {}
  uint8_t major, minor, patch;

  CX_INLINE const uint32_t num() const {
    return ((((uint32_t)(major)) << 22) | (((uint32_t)(minor)) << 12) |
            ((uint32_t)(patch)));
  }

  CX_INLINE std::string to_string() const {
    return std::to_string(major) + "." + std::to_string(major) + "." +
           std::to_string(major);
  }
};
}  // namespace cx
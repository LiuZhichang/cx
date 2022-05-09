/**
 * @file common.h
 * @author liuzhichang (lzc3318619633@163.com)
 * @brief 定义通用结构
 * @version 0.1
 * @date 2022-05-08
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

#include <cstdint>

#include "internal.h"

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
};

CX_INLINE const uint32_t CX_CONSTEXPR BIT(int i) { return (1 << i); }

}  // namespace cx
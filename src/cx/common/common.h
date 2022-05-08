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

namespace cx {

/**
 * @brief 版本类， 用于界定当前版本[用于App/Engine版本]
 *
 */
struct Version {
  Version(uint8_t major, uint8_t minor, uint8_t patch)
      : major(major), minor(minor), patch(patch) {}
  uint8_t major, minor, patch;
};

}  // namespace cx
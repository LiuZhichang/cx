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

CX_INLINE const bool CX_MODULE_USE_RAW_PTR = true;

CX_INLINE const uint32_t CX_CONSTEXPR BIT(int i) { return (1 << i); }

}  // namespace cx
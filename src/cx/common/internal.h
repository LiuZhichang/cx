/**
 * @file internal.h
 * @author liuzhichang (lzc3318619633@163.com)
 * @brief 引擎的宏定义
 * @version 0.1
 * @date 2022-05-08
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#if defined(DEBUG)
#define CX_DEBUG_MODE
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#if !defined(CX_PLATFORM_WINDOWS)
#define CX_PLATFORM_WINDOWS
#endif
#elif defined(linux) || defined(__linux) || defined(__linux__)
#if !defined(CX_PLATFORM_LINUX)
#define CX_PLATFORM_LINUX
#endif
#elif defined(__MACOS__)
#if !defined(CX_PLATFORM_MAC)
#define CX_PLATFORM_MAC
#endif
#endif

#if defined(CX_PLATFORM_WINDOWS)

#endif

#define CX_API
#define CX_C_API extern "C"
#define CX_INLINE inline
#define CX_STATIC static
#define CX_NOEXCEPT noexcept
#define CX_CONSTEXPR constexpr
#define CX_ENTRY_POINT

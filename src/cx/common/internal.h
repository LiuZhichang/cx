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
#else
#error "unknown platform, no support currnet platform."
#endif
#endif

// define compiler macro
#if defined(__clang__)
#define CX_COMPILER_CLANG
#elif defined(__GUNC__)
#define CX_COMPILER_GNUNC
#elif defined(_MSC_VER)
#define CX_COMPILER_MSVC
#else
#error "unknown compiler, no support current compiler."
#endif

#if defined(CX_COMPILER_CLANG) || defined(CX_COMPILER_GNUNC)
#define CX_DLL_INPORT __attribute__((visibility("default")))
#define CX_DLL_EXPORT __attribute__((visibility("default")))
#define CX_DLL_LOCAL __attribute__((visibility("hiden")))

#define CX_DECLARE_CMETAOBJ __attribute__((__constructor__))
#define CX_DECLARE_DMETAOBJ __attribute__((__destructor__))

#elif defined(CX_COMPILER_MSVC)
#define CX_DLL_INPORT _declspec(dllimport))
#define CX_DLL_EXPORT _declspec(dllexport))
#define CX_DLL_LOCAL

#define CX_DECLARE_CMETAOBJ
#define CX_DECLARE_DMETAOBJ
#endif

#define CX_CONSTEXPR constexpr
#define CX_STATIC static
#define CX_STATIC_CONSTEXPR static constexpr
#define CX_NOEXCEPT noexcept
#define CX_INLINE inline
#define CX_C_API extern "C"
#define CX_ENTRY_POINT
#define CX_CORE
#define CX_API

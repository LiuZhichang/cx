/**
 * @file error.h
 * @author liuzhichang (lzc3318619633@163.com)
 * @brief 引擎内部错误码
 * @version 0.1
 * @date 2022-05-10
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

#include <string>

#include "cx/common/internal.h"
#include "cx/common/log/log.h"

namespace cx {

/**
 * @brief 将对应的错误码转为具体释义
 *
 * @param error 错误码
 * @return const char* 具体释义
 */
class Error {
 public:
  /**
   * @brief 错误码定义
   *
   */
  enum class Code : uint8_t {
    eGLFWInitFailed = 0x01,
    eNoSupportVulkan,
    eWindowCreateFailed,
    eNoSupportExtension,
    eInstanceCreateFailed,
    eNoSupportPhysicalDevice,
    eLogicalDeviceCreateFailed,
    eNotFoundGraphicsQueue,
    eSirfaceCreateFailed,
    eSwapchainCreateFailed,
    eNotFoundMemoryType
  };

  CX_STATIC std::string ToString(Code code) {
    switch (code) {
      case Code::eGLFWInitFailed:
        return "GLFW init failed!";
      case Code::eNoSupportVulkan:
        return "Vulkan is not supported in the current environment!";
      case Code::eWindowCreateFailed:
        return "Window create failed!";
      case Code::eNoSupportExtension:
        return "There are unsupported extensions in Instance!";
      case Code::eNoSupportPhysicalDevice:
        return "No Gpu supporting Vulkan was found!";
      case Code::eNotFoundGraphicsQueue:
        return "Graphics queue family not found!";
      case Code::eNotFoundMemoryType:
        return "Failed to find a valid memory type for buffer!";
      default:
        return "unknown error";
        break;
    }
  }

  CX_STATIC void Output(log::Logger::ptr logger, log::Level level, Code code,
                        bool exception = true) {
    std::string err_msg = ToString(code);
    CX_LOG_LEVEL(logger, level) << err_msg;
    if (exception) throw std::runtime_error(err_msg);
  }

  CX_STATIC void Catch(std::exception* exception) {}

 private:
  enum class ExceptionStage {

  };
};

}  // namespace cx
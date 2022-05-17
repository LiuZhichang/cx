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

#include "log/log.h"

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
    eNoSupportPhysicalDevice
  };

  static std::string ToString(Code code) {
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
      default:
        return "unknown error";
        break;
    }
  }

  static void Output(log::Logger::ptr logger, log::Level level, Code code,
                     bool exception = true) {
    std::string err_msg = ToString(code);
    CX_LOG_LEVEL(logger, level) << err_msg;
    if (exception) throw std::runtime_error(err_msg);
  }
};

}  // namespace cx
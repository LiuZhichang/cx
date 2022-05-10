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

namespace cx {

/**
 * @brief 错误码定义
 *
 */
enum class Error {
  eGLFWInitFailed,
  eNoSupportVulkan,
  eWindowCreateFailed,
  eNoSupportExtension
};

/**
 * @brief 将对应的错误码转为具体释义
 *
 * @param error 错误码
 * @return const char* 具体释义
 */
const char* strerror(Error error) {
  switch (error) {
    case Error::eGLFWInitFailed:
      return "glfw init failed!";
    case Error::eNoSupportVulkan:
      return "current env not support vulkan!";
    case Error::eWindowCreateFailed:
      return "window create failed!";
    case Error::eNoSupportExtension:
      return "engine exsist extension not supported!";
    default:
      return "unknown error";
      break;
  }
}

}  // namespace cx
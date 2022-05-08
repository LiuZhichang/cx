#pragma once

namespace cx {

enum class Error { eGLFWInitFailed, eNoSupportVulkan, eWindowCreateFailed };

const char* strerror(Error error) {
  switch (error) {
    case Error::eGLFWInitFailed:
      return "glfw init failed!";
    case Error::eNoSupportVulkan:
      return "current env not support vulkan!";
    case Error::eWindowCreateFailed:
      return "window create failed!";
    default:
      return "unknown error";
      break;
  }
}

}  // namespace cx
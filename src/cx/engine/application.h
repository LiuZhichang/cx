/**
 * @file application.h
 * @author liuzhichang (lzc3318619633@163.com)
 * @brief 应用程序类，用于自定义app
 * @version 0.1
 * @date 2022-05-10
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include <cx/common/common.h>
#include <cx/common/internal.h>
#include <cx/common/noncopyable.h>
#include <cx/engine/engine.h>

#include <string>

namespace cx {

class App : public Noncopyable {
  friend Engine;

 public:
  struct Arguments {
    int argc;
    char** argv;
  };

  explicit App(const std::string name, const Arguments& arg,
               const Version& version = {1, 0, 0})
      : m_name(std::move(name)),
        m_version(version),
        m_arguments(arg),
        m_running(false) {}

  virtual ~App() {}

  virtual CX_API void run() = 0;
  virtual CX_API void update() = 0;

  const std::string& name() const { return m_name; }

  void reset_name(const std::string& name) { m_name = name; }

  const Version& version() const { return m_version; }

  void update_version(const Version& version) { m_version = version; }

  const Arguments& arguments() const { return m_arguments; }

  bool is_running() const { return m_running; }
  void stop() { m_running = false; }

 protected:
  std::string m_name;
  Version m_version;
  Arguments m_arguments;

  bool m_running;
};
}  // namespace cx

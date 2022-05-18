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

#include <string>

#include "cx/common/common.h"
#include "cx/common/internal.h"
#include "cx/common/noncopyable.h"
#include "cx/engine/version.h"

namespace cx {

/**
 * @brief app抽象类，用于实现自定义程序
 *
 */
class App : public Noncopyable {
 public:
  typedef App* ptr;
  /**
   * @brief 程序执行参数
   *
   */
  struct Arguments {
    int argc;
    char** argv;
  };

  /**
   * @brief 应用程序构造函数
   *
   * @param name 程序名称
   * @param arg 参数
   * @param version 版本号
   */
  explicit App(const std::string name, const Arguments& arg,
               const Version& version = {1, 0, 0})
      : m_name(std::move(name)),
        m_version(version),
        m_arguments(arg),
        m_running(false) {}

  virtual ~App() {}

  virtual CX_API void task() = 0;
  virtual CX_API void update() = 0;

  const std::string& name() const { return m_name; }

  void reset_name(const std::string& name) { m_name = name; }

  const Version& version() const { return m_version; }

  void update_version(const Version& version) { m_version = version; }

  const Arguments& arguments() const { return m_arguments; }

  const bool& is_running() const { return m_running; }

  void stop() { m_running = false; }

  void run() {
    task();
    m_running = true;
  }

 protected:
  std::string m_name;
  Version m_version;
  Arguments m_arguments;

 private:
  bool m_running;
};
}  // namespace cx

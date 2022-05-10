/**
 * @file engine.h
 * @author liuzhichang (lzc3318619633@163.com)
 * @brief 引擎类
 * @version 0.1
 * @date 2022-05-10
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

#include <cx/common/common.h>
#include <cx/common/module.h>
#include <cx/common/noncopyable.h>
#include <cx/common/singleton.h>

namespace cx {

class App;
class Engine : public Noncopyable, public Singleton<Engine> {
  CX_SINGLETON_FRIEND(Engine);

 public:
  ~Engine();

  void load(App* app);

  App* app() { return m_app; }

  void run();
  void stop();

  void set_version(const Version& version) { m_version = version; }
  const Version& version() const { return m_version; }

 private:
  Engine();
  void stage_verdict(Module::Stage stage);

 private:
  App* m_app;
  Version m_version;

  std::vector<std::unique_ptr<Module>> m_modules;
  bool m_running;
};

}  // namespace cx
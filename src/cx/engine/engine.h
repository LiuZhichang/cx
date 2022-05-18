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

#include "cx/common/module.h"
#include "cx/common/noncopyable.h"
#include "cx/common/singleton.h"
#include "cx/engine/application.h"
#include "cx/engine/version.h"

namespace cx {

class App;
class Engine : public Noncopyable, public SingletonPtr<Engine> {
  friend SingletonPtr<Engine>;

 public:
  ~Engine();

  /**
   * @brief 加载app 仅加载一次
   *
   * @param app app对象指针即 App::ptr/App* 类型
   */
  void load(App* app);

  /**
   * @brief 获取当前的app对象指针
   *
   * @return App* app对象指针
   */
  App* app() { return m_app; }

  void run();
  void stop();

  void set_version(const Version& version) { m_version = version; }
  const Version& version() const { return m_version; }

 private:
  Engine();
  void init_module();
  void stage_verdict(Module::Stage stage);

 private:
  App* m_app;
  Version m_version;

  std::vector<std::unique_ptr<Module>> m_modules;
  bool m_running;
};

}  // namespace cx
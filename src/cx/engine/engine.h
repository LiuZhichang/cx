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

#include <map>

#include "cx/common/module.h"
#include "cx/common/noncopyable.h"
#include "cx/common/singleton.h"
#include "cx/engine/application.h"
#include "cx/engine/version.h"
#include "cx/utils/time/delta.h"
#include "cx/utils/time/elapsed_time.h"

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

  float fps_limit() const { return m_fps_limit; }
  void set_fps_limit(float fps_limit) { m_fps_limit = fps_limit; }

  const time::TimePoint delta() const { return m_delat_update.m_change; }

  const time::TimePoint delta_render() const { return m_delat_render.m_change; }

  uint32_t upd() const { return m_ups.m_value; }

  uint32_t fps() const { return m_fps.m_value; }

 private:
  Engine();
  void init_module();
  void stage_verdict(Module::Stage stage);

 private:
  typedef std::pair<Module::Stage, std::size_t> StageInfo;

  App* m_app;
  Version m_version;

  std::multimap<StageInfo, std::unique_ptr<Module>> m_modules;
  bool m_running;

  float m_fps_limit;

  time::Delta m_delat_update;
  time::Delta m_delat_render;
  time::ElapsedTime m_elapse_update;
  time::ElapsedTime m_elapse_render;
  time::ChangePerSecond m_ups;
  time::ChangePerSecond m_fps;
};

}  // namespace cx
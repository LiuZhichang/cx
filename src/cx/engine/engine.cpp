#include "engine.h"

#include <cx/common/internal.h>
#include <cx/window/window.h>

#include "application.h"

static auto engine = CX_LOGGER("engine");

cx::Engine::Engine() : m_app(nullptr), m_version{1, 0, 0}, m_running(true) {
  // 记录已经创建的模块
  std::vector<TypeID> created;
  for (;;) {
    bool postponed = false;
    // 遍历注册管理器，获取全部已经注册过的模块信息
    for (const auto &[moduleId, moduleVal] : Module::Registry()) {
      // 如果已经创建则略过
      if (std::find(created.begin(), created.end(), moduleId) != created.end())
        continue;
      bool this_postponed = false;
      for (const auto &requireId : moduleVal.require) {
        if (std::find(created.begin(), created.end(), requireId) ==
            created.end()) {
          this_postponed = true;
          break;
        }
      }
      if (this_postponed) {
        postponed = true;
        continue;
      }
      // 生成模块
      auto &&module = moduleVal.generator();
      m_modules.emplace_back(std::move(module));
      // 记录已经生成的模块
      created.emplace_back(moduleId);
    }
    if (!postponed) break;
  }
}

cx::Engine::~Engine() {
  if (m_app) {
    delete m_app;
    m_app = nullptr;
  }
  Module::Registry().clear();
}

void cx::Engine::load(App *app) {
  if (m_app == app) return;
  if (m_app) {
    delete m_app;
    m_app = nullptr;
#if defined(CX_DEBUG_MODE)
    LOG_WARN(engine) << "旧的app对象是有效的，但是现在需要加载新的app，注意：旧"
                        "的app将会被释放!";
#endif
  }
  m_app = app;
}

void cx::Engine::run() {
  Window *window = Window::Get();
  while (m_running) {
    // 如果窗口关闭 则退出
    if (window->closed()) {
      stop();
      continue;
    }

    if (m_app) {
      if (!m_app->m_running) {
        m_app->run();
        m_app->m_running = true;
      }
      m_app->update();
    }

    window->update();
  }

  m_running = false;
}

void cx::Engine::stop() { m_running = false; }

void cx::Engine::stage_verdict(Module::Stage stage) {}
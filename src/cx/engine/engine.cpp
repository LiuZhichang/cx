#include "engine.h"

#include <cx/common/internal.h>
#include <cx/editor/window.h>
#include <cx/utils/log/log.h>

#include "application.h"

static auto engine = CX_LOGGER("engine");

cx::Engine::Engine() : m_app(nullptr), m_version{1, 0, 0}, m_running(true) {}

cx::Engine::~Engine() {
  if (m_app) delete m_app;
}

void cx::Engine::load(App* app) {
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
  while (m_running) {
    if (m_app) {
      m_app->run();
      m_app->update();

      // Window::Self()->update();
    }

    m_running = false;
  }
}
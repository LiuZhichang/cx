#pragma once

#include <cx/common/defer.h>
#include <cx/common/internal.h>
#include <cx/common/log/log.h>
#include <cx/engine/application.h>
#include <cx/engine/engine.h>

extern cx::App* CX_API CX_CreateApp(int argc, char** argv);

/*∏
 * @brief 入口函数
 */
int main(int argc, char** argv) {
  // 开启日志
  cx::LogManager::EnableEngineLogger();
#if !defined(CX_DEBUG_MODE)
  CX_LOGGER("core")->setLevel(cx::LogLevel::Level::ERROR);
  CX_LOGGER("engine")->setLevel(cx::LogLevel::Level::UNKNOWN);
#else
  auto logger = CX_LOGGER("engine");
  LOG_INFO(logger) << "Entry point";
#endif

  cx::Engine* engine = cx::Engine::Self();

  cx::App* app = CX_CreateApp(argc, argv);
  engine->load(app);
  engine->run();

  return EXIT_SUCCESS;
  ;
}
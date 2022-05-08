#pragma once

#include <cx/common/defer.h>
#include <cx/editor/window.h>
#include <cx/engine/application.h>
#include <cx/engine/engine.h>

extern cx::App* CX_API CX_CreateApp(int argc, char** argv);

/*∏
 * @brief 入口函数
 */
int main(int argc, char** argv) {
  cx::App* app = CX_CreateApp(argc, argv);
  cx::Engine* engine = cx::Engine::Self();
  engine->load(app);

  cx::Window* window = cx::Window::Self();
  window->resize({800, 600});
  window->set_title("Cx Engine");
  window->set_position({0, 0});

  engine->run();

  return EXIT_SUCCESS;
  ;
}
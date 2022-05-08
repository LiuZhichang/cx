#include "myapp.h"

#include <cx/utils/log/log.h>

auto core = CX_LOGGER("core");
auto engine = CX_LOGGER("engine");

int MyApp::counter = 1;

void MyApp::run() {
  { LOG_INFO(core) << "Application run:"; }
}

void MyApp::update() {}

cx::App* CX_CreateApp(int argc, char** argv) {
  cx::Engine::Self()->set_version({1, 1, 1});
  return new MyApp({argc, argv});
}
#include "myapp.h"

#include <cx/common/log/log.h>
#include <cx/window/window.h>

class Test {
 public:
};

cx::App* CX_CreateApp(int argc, char** argv) { return new MyApp({argc, argv}); }

auto core = CX_LOGGER("core");
auto engine = CX_LOGGER("engine");

int MyApp::counter = 1;

MyApp::MyApp(const Arguments& args) : App("CX Engine", args, {1, 0, 0}) {
  LOG_INFO(core) << "Application init";
  initEngine();
}

void MyApp::run() {
  LOG_INFO(engine) << "App Run";
  // initWindow();
}

void MyApp::update() {}

void MyApp::initWindow() {}

void MyApp::initEngine() { cx::Engine::Self()->set_version({1, 1, 1}); }

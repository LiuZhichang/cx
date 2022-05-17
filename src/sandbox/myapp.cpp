#include "myapp.h"

#include <cx/common/log/log.h>
#include <cx/graphics/graphics.h>
#include <cx/window/window.h>

using namespace cx;

cx::App* CX_CreateApp(int argc, char** argv) { return new MyApp({argc, argv}); }

auto applg = CX_LOGGER("app");

int MyApp::counter = 1;

MyApp::MyApp(const Arguments& args) : App("CX Engine", args, {1, 0, 0}) {
  applg->addAppender(log::StdOutLogAppender::Create());
  LOG_INFO(applg) << "Application init";
  initEngine();
  initWindow();
}

void MyApp::task() { LOG_INFO(applg) << "App Do work"; }

void MyApp::update() {}

void MyApp::initWindow() {
  Window::ptr window = Window::Get();
  window->set_title("Cx Engine");
  window->resize(1024, 768);
  LOG_INFO(applg) << window->attr_info();
}

void MyApp::initEngine() { Engine::Self()->set_version({1, 1, 1}); }

#include "myapp.h"

#include <cx/common/logger.h>
#include <cx/graphics/graphics.h>
#include <cx/window/window.h>

using namespace cx;

cx::App* CX_CreateApp(int argc, char** argv) { return new MyApp({argc, argv}); }

MyApp::MyApp(const Arguments& args) : App("CX Engine", args, {1, 0, 0}) {
  LOG_INFO(log::Loggers::core) << "App Start";
  initEngine();
  initWindow();
}

MyApp::~MyApp() { LOG_INFO(log::Loggers::core) << "App Stop"; }

void MyApp::task() { LOG_INFO(log::Loggers::engine) << "App Do work"; }

void MyApp::update() {}

void MyApp::initWindow() {
  auto device = Graphics::Get()->physical_device();
  std::string gpu = device->properties().deviceName;
  std::string title = "Cx Engine\tGPU:" + gpu;
  Window::ptr window = Window::Get();
  window->set_title(title.c_str());
  window->resize(1024, 768);
  LOG_INFO(log::Loggers::engine) << window->attr_info();
}

void MyApp::initEngine() { Engine::Self()->set_version({1, 1, 1}); }

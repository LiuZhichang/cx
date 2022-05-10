#include "instance.h"

#include <cx/common/internal.h>
#include <cx/engine/application.h>
#include <cx/engine/engine.h>
#include <cx/window/window.h>

cx::GInstance::GInstance() {
  auto extensions = std::move(Window::Get()->get_inst_extensions());
  init(extensions);
}

cx::GInstance::GInstance(const char** extensions, uint8_t extensions_count) {
  std::vector<const char*> _extensions(extensions,
                                       extensions + extensions_count);
  init(_extensions);
}

cx::GInstance::~GInstance() {}

template <typename T, typename Container>
void cx::GInstance::init(const Container& extensions) {
  Engine* engine = Engine::Self();
  App* app = Engine::Self()->app();

  const char* name = app->name().c_str();
  uint32_t engine_version = engine->version().num();
  uint32_t app_version = app->version().num();

  vk::ApplicationInfo app_info(name, app_version, name, engine_version,
                               VK_API_VERSION_1_1);

  vk::InstanceCreateInfo inst_create_info();
}

bool cx::GInstance::isEnableValidations() const {
#if defined(CX_DEBUG_MODE)
  return true;
#else
  return false;
#endif
}

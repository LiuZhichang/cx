#pragma once

#include <memory>

#include "component/instance.h"
#include "component/physical_device.h"
#include "component/surface.h"
#include "cx/common/module.h"
#include "cx/window/window.h"

namespace cx {

class Graphics : public Module::Registrar<Graphics> {
  inline static const bool Registered =
      Register(Stage::eRender, Requires<Window>());

 public:
  Graphics();
  ~Graphics();

  void update() override;

  static std::string VulkanResultToString(vk::Result result);
  static void Check(vk::Result result);

 private:
  std::unique_ptr<graphics::Instance> m_instance;
  std::unique_ptr<graphics::PhysicalDevice> m_physical_device;
  std::unique_ptr<graphics::Surface> m_surface;
};

}  // namespace cx
#pragma once

#include <memory>

#include "cx/common/module.h"
#include "cx/graphics/component/instance.h"
#include "cx/graphics/component/logical_device.h"
#include "cx/graphics/component/physical_device.h"
#include "cx/graphics/renderpass/surface.h"
#include "cx/graphics/renderpass/swapchain.h"
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

  const graphics::Instance* instance() const { return m_instance.get(); }
  const graphics::PhysicalDevice* physical_device() const {
    return m_physical_device.get();
  }
  const graphics::LogicalDevice* logical_device() const {
    return m_logical_device.get();
  }
  const graphics::Surface* surface() const { return m_surface.get(); }

  private:
  std::unique_ptr<graphics::Instance> m_instance;
  std::unique_ptr<graphics::PhysicalDevice> m_physical_device;
  std::unique_ptr<graphics::Surface> m_surface;
  std::unique_ptr<graphics::LogicalDevice> m_logical_device;
  std::unique_ptr<graphics::Swapchain> m_swapchain;
};

}  // namespace cx
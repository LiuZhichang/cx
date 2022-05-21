#pragma once

#include <map>
#include <memory>

#include "cx/common/internal.h"
#include "cx/common/module.h"
#include "cx/graphics/command/command_buffer.h"
#include "cx/graphics/component/instance.h"
#include "cx/graphics/component/logical_device.h"
#include "cx/graphics/component/physical_device.h"
#include "cx/graphics/descriptor/descriptor.h"
#include "cx/graphics/renderpass/render.h"
#include "cx/graphics/renderpass/surface.h"
#include "cx/graphics/renderpass/swapchain.h"
#include "cx/window/window.h"

namespace cx {

class CX_CORE Graphics : public Module::Registrar<Graphics> {
  CX_INLINE CX_STATIC const bool Registered =
      Register(Stage::eRender, Requires<Window>());

 public:
  Graphics();
  ~Graphics();

  void update() override;

  CX_STATIC std::string VulkanResultToString(vk::Result result);
  CX_STATIC void Check(vk::Result result);

  const graphics::Instance* instance() const { return m_instance.get(); }
  const graphics::PhysicalDevice* physical_device() const {
    return m_physical_device.get();
  }
  const graphics::LogicalDevice* logical_device() const {
    return m_logical_device.get();
  }
  const graphics::Surface* surface() const { return m_surface.get(); }

  const graphics::Swapchain* swapchain() const { return m_swapchain.get(); }

  const std::shared_ptr<graphics::CommandPool>& command_pool(
      const std::thread::id& id = std::this_thread::get_id());

  void set_renderer(std::unique_ptr<graphics::Render>&& renderer) {
    m_renderer = std::move(renderer);
  }

  const graphics::Descriptor* attachment(const std::string& name) const;

 private:
  void setup_pipeline_cache();
  void rebuild_swapchain();
  void reset_render_stage();
  void regenerate_command_buffers();

 private:
  std::unique_ptr<graphics::Instance> m_instance;
  std::unique_ptr<graphics::PhysicalDevice> m_physical_device;
  std::unique_ptr<graphics::Surface> m_surface;
  std::unique_ptr<graphics::LogicalDevice> m_logical_device;
  std::unique_ptr<graphics::Swapchain> m_swapchain;
  std::unique_ptr<graphics::Render> m_renderer;

  std::map<std::string, const graphics::Descriptor*> m_attachments;
  std::map<const std::thread::id, std::shared_ptr<graphics::CommandPool>>
      m_command_pool;
  std::vector<std::unique_ptr<graphics::CommandBuffer>> m_command_buffers;

  vk::PipelineCache m_pipeline_cache;

  std::vector<vk::Semaphore> m_present_completes;
  std::vector<vk::Semaphore> m_render_completes;
  std::vector<vk::Fence> m_flight_fences;
};

}  // namespace cx
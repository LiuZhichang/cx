#include "ui.h"

#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_vulkan.h>
#include <imgui/imgui.h>

#include "cx/graphics/graphics.h"
#include "cx/window/window.h"

namespace cx::ui {

void check_error(VkResult result) {
  Graphics::Check(static_cast<vk::Result>(result));
}

void EditorUI::init() {
  IMGUI_CHECKVERSION();

  Window* window = Window::Get();

  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  ImGui::StyleColorsDark();
  ImGuiStyle& style = ImGui::GetStyle();

  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
  io.ConfigDockingAlwaysTabBar = true;
  io.ConfigWindowsMoveFromTitleBarOnly = true;

  float content_scale = window->content_scale();
  float font_scale = fmax(1.0f, content_scale);
  io.FontGlobalScale = 1.0f / font_scale;

  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }
}

void EditorUI::setup(Window* window) {
  ImGui_ImplGlfw_InitForVulkan(window->handle(), true);
  ImGui_ImplVulkan_InitInfo init_info = {};
  vk::Instance instance = *Graphics::Get()->instance();
  vk::PhysicalDevice physical_device = *Graphics::Get()->physical_device();
  vk::Device device = *Graphics::Get()->logical_device();
  uint32_t queue_family = Graphics::Get()->logical_device()->graphics_family();
  vk::Queue queue = Graphics::Get()->logical_device()->graphics_queue();

  init_info.Instance = instance;
  init_info.PhysicalDevice = physical_device;
  init_info.Device = device;
  init_info.QueueFamily = queue_family;
  init_info.Queue = queue;
  // init_info.PipelineCache = g_PipelineCache;
  // init_info.DescriptorPool = g_DescriptorPool;
  // init_info.Subpass = 0;
  init_info.MinImageCount = 3;
  init_info.ImageCount = 3;
  init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  init_info.Allocator = nullptr;
  init_info.CheckVkResultFn = check_error;
  // ImGui_ImplVulkan_Init(&init_info, wd->RenderPass);
}

void EditorUI::update() {
  // TODO swapchain rebuild
  //  if(Graphics::Get()->)
}

}  // namespace cx::ui
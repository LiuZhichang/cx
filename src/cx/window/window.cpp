#include "window.h"

#include <GLFW/glfw3.h>

#include "attach/action.h"
#include "cx/common/error.h"
#include "cx/common/logger.h"

namespace cx {

using namespace log;

void Debug_Error(uint32_t code, const char* info) {
  Window::check_error(code);
  LOG_ERROR(Loggers::engine) << "glfw error: " << info << ", code: " << code;
}

void Engine_error(Error::Code error) {
  Error::Output(Loggers::core, log::Level::eError, error);

  throw std::runtime_error(Error::ToString(error));
}

Window::Window() : Window(Attribute()) {}

Window::Window(const Attribute& attribute)
    : m_attribute(attribute), m_desktop_size{2560, 1600} {
  init_Window();
  register_callback();
}

Window::~Window() {
  close();
  ::glfwDestroyWindow(m_whandle);
  ::glfwTerminate();
}

void Window::init_Window() {
  if (::glfwInit() == GLFW_FALSE) {
    Engine_error(Error::Code::eGLFWInitFailed);
  }

  if (::glfwVulkanSupported() == GLFW_FALSE) {
    Engine_error(Error::Code::eNoSupportVulkan);
  }

  ::glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
  ::glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  m_whandle =
      ::glfwCreateWindow(width(), height(), title().c_str(), nullptr, nullptr);

  if (!m_whandle) {
    Engine_error(Error::Code::eWindowCreateFailed);
    ::glfwTerminate();
  }

  ::glfwSetWindowAttrib(m_whandle, GLFW_RESIZABLE, resizeable());

  ::glfwShowWindow(m_whandle);
}

void Window::update_window() {
  set_title(m_attribute.title);
  set_position(m_attribute.pos);
  resize(m_attribute.size);
}

void Window::register_callback() {
  ::glfwSetWindowCloseCallback(m_whandle, window::action::CallBackWindowClose);
  ::glfwSetWindowSizeCallback(m_whandle, window::action::CallBackWindowResize);
  ::glfwSetWindowPosCallback(m_whandle, window::action::CallBackWindowMove);
}

void Window::set_attribute(const Attribute& attribute) {
  m_attribute = attribute;
  update_window();
}

void Window::set_title(const std::string& title) {
  m_attribute.title = title;
  ::glfwSetWindowTitle(m_whandle, title.c_str());
}

void Window::set_width(uint32_t width) {
  m_attribute.size.w = width;
  auto& w = m_attribute.size.w;
  auto& h = m_attribute.size.h;
  ::glfwSetWindowSize(m_whandle, w, h);
}

void Window::set_height(uint32_t height) {
  m_attribute.size.h = height;
  auto& w = m_attribute.size.w;
  auto& h = m_attribute.size.h;
  ::glfwSetWindowSize(m_whandle, w, h);
}

bool Window::closed() { return glfwWindowShouldClose(m_whandle); }

void Window::update() {
  if (!closed()) ::glfwPollEvents();
}

float Window::content_scale() const {
  float x_scale, y_scale;
  glfwGetWindowContentScale(m_whandle, &x_scale, &y_scale);
  return fmaxf(1.0f, fmaxf(x_scale, y_scale));
}

void Window::resize(const Vector2ui& size) {
  if (!resizeable()) return;

  m_attribute.size = size;
  auto& w = m_attribute.size.w;
  auto& h = m_attribute.size.h;

  w = std::min(w, m_desktop_size.w);
  h = std::min(h, m_desktop_size.h);

  ::glfwSetWindowSize(m_whandle, w, h);
}

const void Window::set_position(uint32_t x, uint32_t y) {
  m_attribute.pos = Vector2ui(x, y);

  if (x < 0) {
    m_attribute.pos.x = 0;
  }

  if (y < 0) {
    m_attribute.pos.y = 0;
  }

  ::glfwSetWindowPos(m_whandle, m_attribute.pos.x, m_attribute.pos.y);
}

const void Window::set_position(const Vector2ui& pos) {
  set_position(pos.x, pos.y);
}

void Window::set_resizeable(bool enable) {
  ::glfwSetWindowAttrib(m_whandle, GLFW_RESIZABLE, (int)enable);
  if (enable) {
    m_attribute.status |= Status::eFullScreen;
  } else {
    m_attribute.status ^= Status::eFullScreen;
  }
}

std::string Window::attr_info() {
  std::stringstream ss;
  ss << "\nWidnow Attribute:\n"
     << "\ttitle:" << m_attribute.title << "\n"
     << "\tposition: [" << m_attribute.pos.x << "," << m_attribute.pos.y << "]"
     << "\n"
     << "\tsize: [" << m_attribute.size.w << "," << m_attribute.size.h << "]"
     << "\n"
     << "\tresizeable: " << resizeable() << "\n"
     << "\tfullscreen: " << fullscreen() << "\n"
     << "\tflag bit: " << m_attribute.status.to_bitset() << "\n"
     << "\tflag under value: " << m_attribute.status.underlying_value();
  return ss.str();
}

std::pair<const char**, uint32_t> Window::get_inst_extensions_kv() const {
  std::pair<const char**, uint32_t> result;
  result.first = ::glfwGetRequiredInstanceExtensions(&result.second);
  return result;
}

std::vector<const char*> Window::get_inst_extensions() const {
  uint32_t count = 0;
  const char** extensions = ::glfwGetRequiredInstanceExtensions(&count);

  return std::vector<const char*>(extensions, extensions + count);
}

vk::Result Window::create_surface(
    const vk::Instance& inst, vk::SurfaceKHR& surface,
    const vk::AllocationCallbacks* allocator) const {
  VkSurfaceKHR sf;
  vk::Result vkRes = static_cast<vk::Result>(::glfwCreateWindowSurface(
      static_cast<VkInstance>(inst), m_whandle, VK_NULL_HANDLE, &sf));
  surface = vk::SurfaceKHR(sf);
  return vkRes;
}

const char* Window::result_tostring(uint32_t result) {
  switch (result) {
    case GLFW_TRUE:
      return "Success";
    case GLFW_NOT_INITIALIZED:
      return "GLFW has not been initialized";
    case GLFW_NO_CURRENT_CONTEXT:
      return "No context is current for this thread";
    case GLFW_INVALID_ENUM:
      return "One of the arguments to the function was an invalid enum value";
    case GLFW_INVALID_VALUE:
      return "One of the arguments to the function was an invalid value";
    case GLFW_OUT_OF_MEMORY:
      return "A memory allocation failed";
    case GLFW_API_UNAVAILABLE:
      return "GLFW could not find support for the requested API on the "
             "system";
    case GLFW_VERSION_UNAVAILABLE:
      return "The requested OpenGL or OpenGL ES version is not available";
    case GLFW_PLATFORM_ERROR:
      return "A platform-specific error occurred that does not match any of "
             "the more specific categories";
    case GLFW_FORMAT_UNAVAILABLE:
      return "The requested format is not supported or available";
    case GLFW_NO_WINDOW_CONTEXT:
      return "The specified window does not have an OpenGL or OpenGL ES "
             "context";
    default:
      return "ERROR: UNKNOWN GLFW ERROR";
  }
}

void Window::check_error(uint32_t result) {
  if (result) return;

  const char* errorMsg = result_tostring(result);
  LOG_ERROR(Loggers::engine) << "glfw error:" << errorMsg;
  throw std::runtime_error(errorMsg);
}

}  // namespace cx
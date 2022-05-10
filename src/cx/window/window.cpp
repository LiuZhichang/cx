#include "window.h"

#include <GLFW/glfw3.h>
#include <cx/common/error.h>
#include <cx/common/log/log.h>

static auto engine = CX_LOGGER("engine");
static auto core = CX_LOGGER("core");

void cx::CallBackWindowClose(GLFWwindow* _window) {
  LOG_DEBUG(engine) << "click close button";
  cx::Window* window = cx::Window::Get();
  if (window->onClose) window->onClose();
  window->close();
}

void Debug_Error(uint32_t code, const char* info) {
  cx::Window::check_error(code);
  LOG_ERROR(engine) << "glfw error: " << info << ", code: " << code;
}

void Engine_error(cx::Error error) {
  const char* err_str = strerror(error);
  LOG_ERROR(core) << err_str;
  throw std::runtime_error(err_str);
}

cx::Window::Window() : Window(Attribute()) {}

cx::Window::Window(const Attribute& attribute) : m_attribute(attribute) {
#if defined(CX_DEBUG_MODE)
  LOG_INFO(engine) << "\nWidnow Attribute:\n"
                   << "\ttitle:" << m_attribute.title << "\n"
                   << "\tposition: [" << m_attribute.pos.x << ","
                   << m_attribute.pos.y << "]"
                   << "\n"
                   << "\tsize: [" << m_attribute.size.w << ","
                   << m_attribute.size.h << "]"
                   << "\n"
                   << "\tresizeable: " << resizeable() << "\n"
                   << "\tfullscreen: " << fullscreen() << "\n"
                   << "\tflag bit: " << m_attribute.status.to_bitset() << "\n"
                   << "\tflag under value: "
                   << m_attribute.status.underlying_value();
#endif
  init_Window();
  register_callback();
}

cx::Window::~Window() {
  close();
  ::glfwDestroyWindow(m_whandle);
  ::glfwTerminate();
}

void cx::Window::init_Window() {
  if (::glfwInit() == GLFW_FALSE) {
    Engine_error(Error::eGLFWInitFailed);
  }

  if (::glfwVulkanSupported() == GLFW_FALSE) {
    Engine_error(Error::eNoSupportVulkan);
  }

  ::glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
  ::glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  m_whandle =
      ::glfwCreateWindow(width(), height(), title().c_str(), nullptr, nullptr);

  if (!m_whandle) {
    Engine_error(Error::eWindowCreateFailed);
    ::glfwTerminate();
  }

  ::glfwSetWindowAttrib(m_whandle, GLFW_RESIZABLE, resizeable());

  ::glfwShowWindow(m_whandle);
}

void cx::Window::update_window() {
  set_title(m_attribute.title);
  set_position(m_attribute.pos);
  resize(m_attribute.size);
}

void cx::Window::register_callback() {
  glfwSetWindowCloseCallback(m_whandle, CallBackWindowClose);
}

void cx::Window::set_attribute(const Attribute& attribute) {
  m_attribute = attribute;
}

void cx::Window::set_title(const std::string& title) {
  m_attribute.title = title;
  ::glfwSetWindowTitle(m_whandle, title.c_str());
}

bool cx::Window::closed() { return glfwWindowShouldClose(m_whandle); }

void cx::Window::update() {
  if (!closed()) ::glfwPollEvents();
}

void cx::Window::resize(const Vector2ui& size) {
  if (!resizeable()) return;

  m_attribute.size = size;
  auto& w = m_attribute.size.w;
  auto& h = m_attribute.size.h;

  w = std::min(w, m_desktop_size.w);
  h = std::min(h, m_desktop_size.h);

  ::glfwSetWindowSize(m_whandle, w, h);
}

const void cx::Window::set_position(uint32_t x, uint32_t y) {
  m_attribute.pos = Vector2ui(x, y);

  if (x < 0) {
    m_attribute.pos.x = 0;
  }

  if (y < 0) {
    m_attribute.pos.y = 0;
  }

  ::glfwSetWindowPos(m_whandle, m_attribute.pos.x, m_attribute.pos.y);
}

const void cx::Window::set_position(const Vector2ui& pos) {
  set_position(pos.x, pos.y);
}

void cx::Window::set_resizeable(bool enable) {
  ::glfwSetWindowAttrib(m_whandle, GLFW_RESIZABLE, (int)enable);
  if (enable) {
    m_attribute.status |= Status::eFullScreen;
  } else {
    m_attribute.status ^= Status::eFullScreen;
  }
}

std::pair<const char**, uint32_t> cx::Window::get_inst_extensions_kv() const {
  std::pair<const char**, uint32_t> result;
  result.first = ::glfwGetRequiredInstanceExtensions(&result.second);
  return result;
}

std::vector<const char*>&& cx::Window::get_inst_extensions() const {
  uint32_t count = 0;
  const char** extensions = ::glfwGetRequiredInstanceExtensions(&count);

  return std::move(std::vector<const char*>(extensions, extensions + count));
}

std::pair<vk::SurfaceKHR, vk::Result> cx::Window::create_surface(
    const vk::Instance& inst, const vk::AllocationCallbacks* allocator) const {
  VkSurfaceKHR sf;
  VkAllocationCallbacks alloc = *allocator;

  vk::Result vk_res = static_cast<vk::Result>(
      ::glfwCreateWindowSurface(inst, m_whandle, &alloc, &sf));

  return std::make_pair(sf, vk_res);
}

vk::Result cx::Window::create_surface(
    const vk::Instance& inst, vk::SurfaceKHR& surface,
    const vk::AllocationCallbacks* allocator) const {
  auto pair = create_surface(inst, allocator);

  surface = pair.first;
  return pair.second;
}

const char* cx::Window::result_tostring(uint32_t result) {
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
      return "GLFW could not find support for the requested API on the system";
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

void cx::Window::check_error(uint32_t result) {
  if (result) return;

  const char* error_msg = result_tostring(result);
  LOG_ERROR(core) << "glfw error:" << error_msg;
  throw std::runtime_error(error_msg);
}
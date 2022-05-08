/**
 * @file window.h
 * @author liuzhichang (lzc3318619633@163.com)
 * @brief
 * @version 0.1
 * @date 2022-05-08
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

#include <cx/common/module.h>
#include <cx/common/singleton.h>
#include <cx/maths/vector2.h>

#include <cstdint>
#include <cstring>
#include <vulkan/vulkan.hpp>

struct GLFWwindow;
namespace cx {

/**
 * @brief 窗口类，引擎的编辑界面, 这里继承单例只是为了使用Self函数
 *
 */
class Window : public Module, public Singleton<Window> {
 public:
  struct Attribute {
    char* title;
    Vector2ui size;
    Vector2ui pos;
    bool fullscreen;
    bool resizeable;
    bool focused;

    Attribute()
        : title("window"),
          size(0),
          pos(0),
          fullscreen(false),
          resizeable(false),
          focused(false) {}
    Attribute(const Attribute& oth) { *this = oth; }

    Attribute& operator=(const Attribute& oth) {
      ::strcpy(title, oth.title);
      size = oth.size;
      pos = oth.pos;

      return *this;
    }
  };

  Window();
  Window(const Attribute& attribute);
  ~Window();

  void set_attribute(const Attribute& attribute) { m_attribute = attribute; }
  const Attribute& attribute() const { return m_attribute; }

  const char* title() const { return m_attribute.title; }
  void set_title(const char* title) { ::strcpy(m_attribute.title, title); }
  void set_title(const std::string& title) { set_title(title.c_str()); }

  uint32_t width() const { return m_attribute.size.w; }
  void set_width(uint32_t width) { m_attribute.size.w = width; }

  uint32_t height() const { return m_attribute.size.h; }
  void set_height(uint32_t height) { m_attribute.size.h = height; }

  const Vector2ui& desktop_size() const { return m_desktop_size; }
  const Vector2ui& size() const { return m_attribute.size; }
  void resize(uint32_t width, uint32_t height) {
    m_attribute.size.w = width;
    m_attribute.size.y = height;
  }

  void resize(const Vector2ui& size);

  const Vector2ui& position() const { return m_attribute.pos; }
  const void set_position(uint32_t x, uint32_t y);
  const void set_position(const Vector2ui& pos);

  void update() override;

  float aspect_ratio() const {
    return static_cast<float>(m_attribute.size.w) /
           static_cast<float>(m_attribute.size.h);
  }

  std::pair<const char**, uint32_t> get_inst_extension() const;

  std::pair<vk::SurfaceKHR, vk::Result> create_surface(
      const vk::Instance& inst,
      const vk::AllocationCallbacks* allocator = nullptr) const;
  vk::Result create_surface(
      const vk::Instance& inst, vk::SurfaceKHR& surface,
      const vk::AllocationCallbacks* allocator = nullptr) const;

  static void check_error(uint32_t result);

 private:
  void initWindow();

  static const char* result_tostring(uint32_t result);

 private:
  GLFWwindow* m_whandle;
  Attribute m_attribute;

  Vector2ui m_desktop_size;
};

}  // namespace cx
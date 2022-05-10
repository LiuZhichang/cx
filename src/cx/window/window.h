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

#include <cx/common/common.h>
#include <cx/common/flags.h>
#include <cx/common/internal.h>
#include <cx/common/module.h>
#include <cx/maths/vector2.h>

#include <cstdint>
#include <cstring>
#include <vulkan/vulkan.hpp>

struct GLFWwindow;
namespace cx {

/**
 * @brief 窗口类，引擎的主界面
 *
 */
class Window : public Module::Registrar<Window> {
  CX_INLINE CX_STATIC const bool Rgistered = Register(Stage::ePre);

  friend void CallBackWindowClose(GLFWwindow* window);

 public:
  /**
   * @brief 窗口状态
   *
   */
  enum class Status : uint32_t {
    eClosed = BIT(0),      // 窗口是否关闭ß
    eFullScreen = BIT(1),  // 尺寸是否充满屏幕
    eResizeable = BIT(2),  // 窗口是否可以更改大小
    eFocused = BIT(3),     // 是否可以获取焦点
    eAll = eFullScreen | eResizeable | eFocused
  };
  typedef Flags<Status> StatusFlags;

  /**
   * @brief 窗口属性
   *
   */
  struct Attribute {
    std::string title;   // 窗口标题
    Vector2ui size;      // 窗口大小
    Vector2ui pos;       // 窗口位置
    StatusFlags status;  // 窗口状态

    Attribute()
        : title("window"), size(800, 600), pos(0), status(Status::eAll) {}
    Attribute(const Attribute& oth) { *this = oth; }

    Attribute& operator=(const Attribute& oth) {
      title = oth.title;
      size = oth.size;
      pos = oth.pos;
      status = oth.status;
      return *this;
    }
  };

  Window();
  Window(const Attribute& attribute);
  ~Window();

  /**
   * @brief 设置窗口属性
   *
   * @param[in] attribute
   */
  void set_attribute(const Attribute& attribute);

  /**
   * @brief 获取窗口属性
   *
   * @return const Attribute& 窗口属性对象
   */
  const Attribute& attribute() const { return m_attribute; }

  /**
   * @brief 获取窗口标题
   *
   * @return const std::string& 窗口标题
   */
  const std::string& title() const { return m_attribute.title; }

  /**
   * @brief 设置窗口标题
   *
   * @param[in] title 窗口标题
   */
  void set_title(const std::string& title);

  /**
   * @brief 获取窗口宽度
   *
   * @return uint32_t 窗口宽度
   */
  uint32_t width() const { return m_attribute.size.w; }

  /**
   * @brief 设置窗口宽度
   *
   * @param[in] width 窗口宽度
   */
  void set_width(uint32_t width) { m_attribute.size.w = width; }

  /**
   * @brief 获取窗口高度
   *
   * @return uint32_t 窗口高度
   */
  uint32_t height() const { return m_attribute.size.h; }

  /**
   * @brief 设置窗口高度
   *
   * @param[in] width 窗口高度
   */
  void set_height(uint32_t height) { m_attribute.size.h = height; }

  /**
   * @brief 获取桌面大小
   *
   * @return const Vector2ui& 桌面大小
   */
  const Vector2ui& desktop_size() const { return m_desktop_size; }

  /**
   * @brief 获取窗口大小
   *
   * @return const Vector2ui& 窗口大小
   */
  const Vector2ui& size() const { return m_attribute.size; }

  /**
   * @brief 重设窗口大小
   *
   * @param[in] width 宽度
   * @param[in] height 高度
   */
  void resize(uint32_t width, uint32_t height) {
    m_attribute.size.w = width;
    m_attribute.size.y = height;
  }

  /**
   * @brief 重设窗口大小
   *
   * @param[in] const Vector2ui& 窗口大小 w h
   */
  void resize(const Vector2ui& size);

  /**
   * @brief 获取窗口位置
   *
   * @return const Vector2ui&  窗口位置 x y
   */
  const Vector2ui& position() const { return m_attribute.pos; }

  /**
   * @brief 设置窗口位置
   *
   * @param x x轴坐标
   * @param y y轴坐标
   */
  const void set_position(uint32_t x, uint32_t y);

  /**
   * @brief 设置窗口位置
   *
   * @param pos 窗口坐标 x y
   */
  const void set_position(const Vector2ui& pos);

  /**
   * @brief 窗口是否充满屏幕
   *
   * @return true 充满屏幕
   * @return false 未充满屏幕
   */
  bool fullscreen() const {
    return (m_attribute.status & Status::eFullScreen);
  };

  /**
   * @brief 窗口是否可以更改大小
   *
   * @return true 可以更改大小
   * @return false 不能更改大小
   */
  bool resizeable() const { return (m_attribute.status & Status::eResizeable); }

  void set_resizeable(bool enable);

  /**
   * @brief 窗口是否已经关闭
   *
   * @return true 关闭
   * @return false 未关闭
   */
  // bool closed() const { return (m_attribute.status & Status::eClosed); }
  bool closed();

  const StatusFlags& status() const { return m_attribute.status; }

  /**
   * @brief 关闭窗口，并不会释放窗口所占用的资源
   *
   */
  void close() {
    if (closed()) return;
    m_attribute.status |= Status::eClosed;
  }

  /**
   * @brief 更新函数，更新窗口状态
   *
   */
  void update() override;

  /**
   * @brief 窗口的宽高比
   *
   * @return float 宽高比例
   */
  float aspect_ratio() const {
    return static_cast<float>(m_attribute.size.w) /
           static_cast<float>(m_attribute.size.h);
  }

  /**
   * @brief 获取vulkan实例所需的扩展,以 k v 形式返回
   *
   * @return std::pair<const char**, uint32_t> 具体扩展与其个数
   */
  std::pair<const char**, uint32_t> get_inst_extensions_kv() const;

  /**
   * @brief 获取vulkan实例所需的扩展
   *
   * @return std::vector<const char*>&& 具体扩展
   */
  std::vector<const char*>&& get_inst_extensions() const;

  /**
   * @brief 创建surface，用于呈现图像
   *
   * @param[in] inst vulkan实例
   * @param[in] allocator 内存分配器
   * @return std::pair<vk::SurfaceKHR, vk::Result> suface对象,以及创建状态
   */
  std::pair<vk::SurfaceKHR, vk::Result> create_surface(
      const vk::Instance& inst,
      const vk::AllocationCallbacks* allocator = nullptr) const;

  /**
   * @brief 创建surface，用于呈现图像
   *
   * @param[in] inst vulkan实例
   * @param[out] surface surface对象引用
   * @param[in] allocator 内存分配器
   * @return vk::Result 创建状态
   */
  vk::Result create_surface(
      const vk::Instance& inst, vk::SurfaceKHR& surface,
      const vk::AllocationCallbacks* allocator = nullptr) const;
  /**
   * @brief 检查错误
   *
   * @param[in] result 错误码
   */
  static void check_error(uint32_t result);

  void action_close(const std::function<void()>& close_callback) {
    onClose = std::move(close_callback);
  }

 private:
  void init_Window();
  void update_window();
  void register_callback();
  static const char* result_tostring(uint32_t result);

 private:
  GLFWwindow* m_whandle;
  Attribute m_attribute;

  Vector2ui m_desktop_size;

 private:
  // action callback
  std::function<void()> onClose;
};

}  // namespace cx
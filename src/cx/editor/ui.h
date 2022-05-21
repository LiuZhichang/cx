#pragma once

namespace cx {

class Window;
namespace ui {

class EditorUI {
 public:
  void update();

 private:
  void init();
  void setup(Window* window);
};
}  // namespace ui
}  // namespace cx
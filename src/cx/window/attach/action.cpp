#include "action.h"

#include <cx/window/window.h>

void cx::window::action::CallBackWindowClose(GLFWwindow* _window) {
  cx::Window* window = cx::Window::Get();
  window->close();
}

void cx::window::action::CallBackWindowResize(GLFWwindow* _window, int w,
                                              int h) {}

void cx::window::action::CallBackWindowMove(GLFWwindow* _window, int x, int y) {
}

void CallBackWindowFocus(GLFWwindow* _window, int x, int y);
void CallBackWindowIconify(GLFWwindow* _window, int x, int y);
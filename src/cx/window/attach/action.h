#pragma once

struct GLFWwindow;
namespace cx::window::action {
extern void CallBackWindowClose(GLFWwindow* _window);
extern void CallBackWindowResize(GLFWwindow* _window, int w, int h);
extern void CallBackWindowMove(GLFWwindow* _window, int x, int y);
extern void CallBackWindowFocus(GLFWwindow* _window, int x, int y);
extern void CallBackWindowIconify(GLFWwindow* _window, int x, int y);
}  // namespace cx::window::action
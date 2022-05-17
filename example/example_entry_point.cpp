/**
 * @file example_entry_point.cpp
 * @author liuzhichang (lzc3318619633@163.com)
 * @brief 入口点的使用样例
 * @version 0.1
 * @date 2022-05-11
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <cx/entry_point.h>

// 日志器
static auto logger = CX_LOGGER("example");

class MyApp : public cx::App {
 public:
  MyApp(const Arguments& args) : cx::App("Test App", args) {}

  ~MyApp() {}

  void task() override { LOG_INFO(logger) << "Test Application run"; }

  void update() override { LOG_INFO(logger) << "App Update"; }

 private:
};

cx::App* CX_CreateApp(int argc, char** argv) { return new MyApp({argc, argv}); }
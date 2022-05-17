#include <cx/window/window.h>

int main(int argc, char const *argv[]) {
  auto logger = CX_LOGGER("window");
  logger->addAppender(cx::log::StdOutLogAppender::Create());
  cx::Window window;

  while (!window.closed()) {
    LOG_INFO(logger) << "window main loop";
    window.update();
  }

  return 0;
}

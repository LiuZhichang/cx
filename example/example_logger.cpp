#include <cx/common/defer.h>
#include <cx/common/log/log.h>

// 单线程测试
void one_thread() {
  // 获取一个日志器，如果没有会进行注册
  auto core = CX_LOGGER("core");

  // 设置日志等级
  core->setLevel(cx::log::Level());

  // 添加日志输出地
  core->addAppender(cx::log::StdOutLogAppender::Create());
  core->addAppender(cx::log::FileLogAppender::Create("one_thread_test.log"));

  // 输出 INFO 日志，可以自定义格式
  LOG_DEBUG(core) << "test logger";

  // 在test.log 中可以看到输出
}

// 多线程测试
void ts_test(int num) {
  static auto engine = CX_LOGGER("engine");

  engine->addAppender(cx::log::StdOutLogAppender::Create());
  engine->addAppender(cx::log::FileLogAppender::Create("n_thread_test.log"));

  std::thread* ths = new std::thread[num];
  defer {
    for (int i = 0; i < num; ++i) {
      if (ths[i].joinable()) {
        ths[i].join();
      }
    }

    delete[] ths;
  };

  auto task = [&](int i) {
    for (int i = 0; i < 5; ++i) {
      LOG_DEBUG(engine) << "thread:<" << i << ">"
                        << "task exec";
    }
  };

  for (int i = 0; i < num; ++i) {
    std::thread th(task, i);
    ths[i].swap(th);
  }
}

// 限定日志等级
void level_test() {
  auto logger = CX_LOGGER("cx");
  logger->addAppender(cx::log::StdOutLogAppender::Create());
  // 初始是默认等级，可以输出所以的日志
  LOG_INFO(logger) << "test level info";

  logger->setLevel(cx::log::Level::eDebug);

  LOG_INFO(logger) << "test level info2";
  LOG_DEBUG(logger) << "test level debug";
  LOG_ERROR(logger) << "test level error";
}

int main(int argc, char const* argv[]) {
  one_thread();

  level_test();
  std::cout
      << "----------------------------------------------------------------"
      << std::endl;

  ts_test(4);

  return 0;
}

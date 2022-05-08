//
//  log.hpp
//  Test
//
//  Created by 刘致畅 on 2021/9/13.
//

#ifndef log_hpp
#define log_hpp

#include <cx/common/singleton.h>
#include <cx/utils/sync/spink_lock.h>

#include <forward_list>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#define CX_LOG_LEVEL(logger, level)                                         \
  if (logger->getLevel() <= level)                                          \
  cx::LogWrap(logger,                                                       \
              cx::LogEvent::ptr(new cx::LogEvent(                           \
                  level, __FILE__, __func__, __LINE__, 0,                   \
                  std::this_thread::get_id(), time(0), logger->getName()))) \
      .getStrIO()

#define LOG_DEBUG(logger) CX_LOG_LEVEL(logger, cx::LogLevel::Level::DEBUG)
#define LOG_INFO(logger) CX_LOG_LEVEL(logger, cx::LogLevel::Level::INFO)
#define LOG_WARN(logger) CX_LOG_LEVEL(logger, cx::LogLevel::Level::WARN)
#define LOG_ERROR(logger) CX_LOG_LEVEL(logger, cx::LogLevel::Level::ERROR)
#define LOG_FATAL(logger) CX_LOG_LEVEL(logger, cx::LogLevel::Level::FATAL)

#define CX_LOG_ROOT() cx::LogManager::Self()->getRoot()
#define CX_LOGGER(name) cx::LogManager::Self()->getLogger(name)
#define CX_SYS_LOG() cx::LogManager::Self()->getLogger("system")

namespace cx {

#define implement public

#undef DEBUG

class Logger;

/**
 * @brief 日志等级
 */
class LogLevel {
 public:
  enum class Level { DEBUG, INFO, WARN, ERROR, FATAL, UNKNOWN };

  /**
   * @brief 将对应的日志等级转换为文本
   *
   * @param[in] level 日志等级
   *
   * @return 对应的日志等级文本
   */
  static const std::string toString(Level level);

  /**
   * @brief 将文本转换为对应的日志等级
   *
   * @param[in] level 日志等级
   *
   * @return 文本对应的日志等级
   */
  static const Level fromString(const std::string& level);
};

/**
 * @brief 日志事件
 */
class LogEvent {
 public:
  typedef std::shared_ptr<LogEvent> ptr;

  /**
   * @brief 日志事件类的构造函数
   *
   * @param[in] level            日志等级
   * @param[in] file            文件名
   * @param[in] funcName        函数名
   * @param[in] line            行号
   * @param[in] elapse        启动到现在的毫秒数
   * @param[in] threadId        线程id
   * @param[in] time            当前时间
   * @param[in] name            日志器名
   */
  LogEvent(LogLevel::Level level, const char* file, const char* funcName,
           uint32_t line, uint32_t elapse, std::thread::id threadId,
           uint64_t time, const std::string& name);

  /**
   * @brief 获取文件名
   *
   * @return 文件名
   */
  const char* getFile() const { return m_file; }

  /**
   * @brief 获取函数名
   *
   * @return 函数名
   */
  const char* getFuncName() const { return m_funcName; }

  /**
   * @brief 获取行号
   *
   * @return 行号
   */
  uint32_t getLine() const { return m_line; }

  /**
   * @brief 获取启动到现在的毫秒数
   *
   * @return 毫秒数
   */
  uint32_t getElapse() const { return m_elapse; }

  /**
   * @brief 获取线程id
   *
   * @return 线程id
   */
  std::thread::id getThreadId() const { return m_threadId; }

  /**
   * @brief 获取当前时间
   *
   * @return 时间
   */
  uint64_t getTime() const { return m_time; }

  /**
   * @brief 获取日志等级
   *
   * @return 日志等级
   */
  LogLevel::Level getLevel() const { return m_level; }

  /**
   * @brief 获取日志内容
   *
   * @return 日志内容
   */
  const std::string getContent() const { return m_ss.str(); }

  /**
   * @brief 获取字符流
   *
   * @return 字符流
   */
  std::stringstream& getStrIO() { return m_ss; }

  /**
   * @brief 获取日志器名
   *
   * @return 日志器名
   */
  std::string getName() const { return m_name; }

 private:
  const char* m_file;          // 文件名
  const char* m_funcName;      // 函数名
  std::thread::id m_threadId;  // 线程id
  uint32_t m_line;             // 行号
  uint32_t m_elapse;           // 毫秒数

  uint32_t m_coroutineId;   // 协程id
  uint64_t m_time;          // 时间
  std::string m_name;       // 日志器名
  LogLevel::Level m_level;  // 日志等级
  std::stringstream m_ss;   // 字符流
};

/**
 * @brief 日志格式器
 */
class LogFormatter {
 public:
  typedef std::shared_ptr<LogFormatter> ptr;

  /**
   * @brief 日志格式项
   */
  class FormatItem {
   public:
    typedef std::shared_ptr<FormatItem> ptr;

    /**
     * @brief 格式化输出日志
     *
     * @param[in] os 输出流
     * @param[in] event 日志事件
     */
    virtual void format(std::ostream& os, LogEvent::ptr event) = 0;
  };

 public:
  /**
   * @brief 日志格式器构造函数
   *
   * @param[in] pattern 日志格式模式串
   */
  LogFormatter(const std::string& pattern);

  /**
   * @brief 格式化输出日志
   *
   * @param[in] event 日志事件
   *
   * @return 日志文本
   */
  std::string format(LogEvent::ptr event);

  /**
   * @brief 格式化输出日志
   *
   * @param[in] ofs 输出流
   * @param[in] event 日志事件
   *
   * @return 输出流
   */
  std::ostream& format(std::ostream& ofs, LogEvent::ptr event);

  /**
   * @brief 模式串解析
   */
  void parse();

  /**
   * @brief 格式串格式是否错误
   *
   * @return 是否发生错误
   */
  bool isError() const { return m_error; }

  /**
   * @brief 获取模式串
   *
   * @return 模式串
   */
  const std::string getPattern() const { return m_pattern; }

 private:
  std::string m_pattern;                 // 模式串
  std::vector<FormatItem::ptr> m_items;  // 格式项
  bool m_error;                          // 错误
};

/**
 * @brief 日志输出地
 */
class LogAppender {
 public:
  typedef std::shared_ptr<LogAppender> ptr;
  typedef SpinkLock lock_t;
  typedef std::lock_guard<lock_t> lock_guard;

  /**
   * @brief 生成日志
   *
   * @param[in] level 日志等级
   * @param[in] event 日志事件
   */
  virtual void log(LogLevel::Level level, LogEvent::ptr event) = 0;

  /**
   * @brief 设置日志格式器
   *
   * @param[in] formatter 日志格式器
   */
  void setFormatter(LogFormatter::ptr formatter) {
    lock_guard lovk(m_mutex);
    m_formatter = formatter;
    m_hasFormatter = m_formatter ? true : false;
  }

  /**
   * @brief 获取日志格式器
   *
   * @return 日志格式器
   */
  LogFormatter::ptr getFormatter() {
    lock_guard lock(m_mutex);
    return m_formatter;
  }

  /**
   * @brief 是否有日志格式器
   *
   * @return 是否有格式器
   */
  bool hasFromatter() const { return m_hasFormatter; }

  /**
   * @brief 设置日志等级
   *
   * @param[in] level 日志等级
   */
  void setLevel(LogLevel::Level level) { m_level = level; }

  /**
   * @brief 获取日志等级
   *
   * @return 日志等级
   */
  LogLevel::Level getLevel() const { return m_level; }

 protected:
  LogFormatter::ptr m_formatter;                    // 日志格式器
  LogLevel::Level m_level = LogLevel::Level::INFO;  // 日志等级
  bool m_hasFormatter = false;  // 默认没有日志格式器
  lock_t m_mutex;               // 互斥量
};

/**
 * @brief 标准输出流日志输出地
 */
class StdOutLogAppender : implement LogAppender {
 public:
  typedef std::shared_ptr<StdOutLogAppender> ptr;

  /**
   * @brief 生成日志
   *
   * @param[in] level 日志等级
   * @param[in] event 日志事件
   */
  void log(LogLevel::Level level, LogEvent::ptr event) override;

  static ptr Create() { return ptr(new StdOutLogAppender); }

 private:
};

/**
 * @brief 文件流日志输出地
 */
class FileLogAppender : implement LogAppender {
 public:
  typedef std::shared_ptr<FileLogAppender> ptr;

  /**
   * @brief 文件流日志输出地构造函数
   *
   * @param[in] filename 文件名
   */
  FileLogAppender(const std::string& filename);

  /**
   * @brief 生成日志
   *
   * @param[in] level 日志等级
   * @param[in] event 日志事件
   */
  void log(LogLevel::Level level, LogEvent::ptr event) override;

  /**
   * @brief 重新打开文件
   *
   * @return 是否成功
   */
  bool reopen();

  static ptr Create(const char* file) { return ptr(new FileLogAppender(file)); }

 private:
  std::string m_filename;      // 文件名
  std::ofstream m_filestream;  // 文件流
  uint64_t m_lastTime = 0;     // 最后打开时间
};

/**
 * @brief 日志器
 */
class Logger {
 public:
  typedef std::shared_ptr<Logger> ptr;
  typedef SpinkLock lock_t;
  typedef std::lock_guard<lock_t> lock_guard;

  /**
   * @brief 日志器构造函数
   *
   * @param[in] name 日志器名
   */
  Logger(std::string name = "root");

  /**
   * @brief 生成日志
   *
   * @param[in] level 日志等级
   * @param[in] event 日志事件
   */
  void log(LogLevel::Level level, LogEvent::ptr event);

  /**
   * @brief 生成debug等级的日志
   *
   * @param[in] event 日志事件
   */
  void debug(LogEvent::ptr event);

  /**
   * @brief 生成info等级的日志
   *
   * @param[in] event 日志事件
   */
  void info(LogEvent::ptr event);

  /**
   * @brief 生成warn等级的日志
   *
   * @param[in] event 日志事件
   */
  void warn(LogEvent::ptr event);

  /**
   * @brief 生成error等级的日志
   *
   * @param[in] event 日志事件
   */
  void error(LogEvent::ptr event);

  /**
   * @brief 生成fatal等级的日志
   *
   * @param[in] event 日志事件
   */
  void fatal(LogEvent::ptr event);

  /**
   * @brief 添加日志输出地
   *
   * @param[in] appender 日志输出地
   */
  void addAppender(LogAppender::ptr appender);

  /**
   * @brief 删除日志输出地
   *
   * @param[in] appender
   */
  void delAppender(LogAppender::ptr appender);

  /**
   * @brief 清除日志输出地
   */
  void clearAppenders();

  /**
   * @brief 获取日志器配置
   *
   * @return 配置文本
   */
  std::string configString();

  /**
   * @brief 获取日志等级
   *
   * @return 日志等级
   */
  LogLevel::Level getLevel() const { return m_level; }

  /**
   * @brief 设置日志等级
   *
   * @param[in] val 日志等级
   */
  void setLevel(LogLevel::Level level) { m_level = level; }

  /**
   * @brief 设置日志格式器
   *
   * @param[in] formatter 日志格式器
   */
  void setFormatter(LogFormatter::ptr formatter);

  /**
   * @brief 设置日志格式器
   *
   * @param[in] foramtter 日志格式模式串
   */
  void setFormatter(const std::string& foramtter);

  /**
   * @brief 设置日志格式器
   *
   * @return 日志格式器
   */
  LogFormatter::ptr getFormatter();

  /**
   * @brief 获取日志器名
   *
   * @return 日志器名
   */
  std::string getName() const { return m_name; }

 private:
  std::string m_name;                               // 日志器名
  LogLevel::Level m_level;                          // 日志等级
  std::forward_list<LogAppender::ptr> m_appenders;  // 日志输出地
  LogFormatter::ptr m_formatter;                    // 日志格式器
  Logger::ptr m_root;                               // 主日志器
  lock_t m_mutex;                                   // 互斥量
};

/**
 * @brief 日志外覆器
 */
class LogWrap {
 public:
  /**
   * @brief 外覆器构造函数
   *
   * @param[in] logger 日志器
   * @param[in] event 日志事件
   */
  LogWrap(Logger::ptr logger, LogEvent::ptr event)
      : m_logger(logger), m_event(event){};

  /**
   * @brief 外覆器析构函数
   */
  ~LogWrap() { m_logger->log(m_event->getLevel(), m_event); }

  /**
   * @brief 获取日志事件
   *
   * @return 日志事件
   */
  LogEvent::ptr getEvent() const { return m_event; }

  /**
   * @brief 获取字符流
   *
   * @return 字符流
   */
  std::stringstream& getStrIO() { return m_event->getStrIO(); }

 public:
  Logger::ptr m_logger;   // 日志器
  LogEvent::ptr m_event;  // 日志事件
};

/**
 * @brief 日志管理器
 */
class LoggerManager {
 public:
  typedef SpinkLock lock_t;
  typedef std::lock_guard<lock_t> lock_guard;

  /**
   * @brief 日志管理器构造函数
   */
  LoggerManager() {
    m_root.reset(new Logger);
    m_root->addAppender(StdOutLogAppender::Create());
    m_loggers[m_root->getName()] = m_root;

    auto core = getLogger("core");
    auto engine = getLogger("engine");

    std::string target_log = CheckLogDir();
    core->addAppender(FileLogAppender::Create(target_log.c_str()));
    engine->addAppender(StdOutLogAppender::Create());
  }

  /**
   * @brief 获取日志器
   *
   * @param[in] name 日志器名
   *
   * @return 日志器
   */
  Logger::ptr getLogger(const std::string& name) {
    lock_guard lock(m_mutex);
    auto it = m_loggers.find(name);
    if (it != m_loggers.end()) return it->second;
    Logger::ptr logger = std::make_shared<Logger>(name);
    m_loggers[name] = logger;
    return logger;
  }

  /**
   * @brief 获取主日志器
   *
   * @return 主日志器
   */
  Logger::ptr getRoot() const { return m_root; }

  static void SetLogDir(const std::string& dir) { m_log_dir = dir; }

 private:
  static std::string CheckLogDir() {
    std::string cur_path = std::filesystem::current_path();
    std::filesystem::path target_log(cur_path + "/" + m_log_dir);
    if (!std::filesystem::exists(target_log)) {
      std::filesystem::create_directory(target_log);
    }

    std::chrono::time_point now = std::chrono::system_clock::now();
    time_t _now = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&_now), "%Y_%m_%d");

    std::string cur_day_log = target_log.string() + "/" + ss.str() + ".log";
    std::filesystem::path target_day_log(cur_day_log);
    if (!std::filesystem::exists(target_day_log)) {
      std::fstream fs;
      fs.open(cur_day_log, std::ios_base::out);
      fs.close();
    }

    return cur_day_log;
  }

 private:
  Logger::ptr m_root;                            // 主日志器
  std::map<std::string, Logger::ptr> m_loggers;  // 所有日志器
  lock_t m_mutex;                                // 互斥量
  static std::string m_log_dir;
};

typedef SingletonPtr<LoggerManager> LogManager;

}  // namespace cx

#endif /* log_hpp */

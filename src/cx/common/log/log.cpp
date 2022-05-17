//
//  log.cpp
//  Test
//
//  Created by 刘致畅 on 2021/9/13.
//

#include "log.h"

#include <functional>

namespace cx::log {

std::string cx::log::LoggerManager::m_log_dir = "runtime_log";

const std::string LogLevel::toString(Level level) {
  switch (level) {
    case Level::eInfo:
      return "INFO";
    case Level::eDebug:
      return "DEBUG";
    case Level::eWarn:
      return "WARNING";
    case Level::eError:
      return "ERROR";
    case Level::eFatal:
      return "FATAL";
    default:
      return "UNKNOWN";
  }
}

LogEvent::LogEvent(Level level, const char* file, const char* funcName,
                   uint32_t line, uint32_t elapse, std::thread::id threadId,
                   uint64_t time, const std::string& name)
    : m_file(file),
      m_funcName(funcName),
      m_line(line),
      m_elapse(elapse),
      m_threadId(threadId),
      m_time(time),
      m_name(name),
      m_level(level) {}

LogFormatter::LogFormatter(const std::string& pattern) : m_pattern(pattern) {
  parse();
}

std::string LogFormatter::format(LogEvent::ptr event) {
  std::stringstream ss;
  for (auto& item : m_items) {
    item->format(ss, event);
  }
  return ss.str();
}

std::ostream& LogFormatter::format(std::ostream& ofs, LogEvent::ptr event) {
  for (auto& item : m_items) {
    item->format(ofs, event);
  }
  return ofs;
}

class MessageFormatItem : public LogFormatter::FormatItem {
 public:
  MessageFormatItem(const std::string&) {}
  void format(std::ostream& os, LogEvent::ptr event) override {
    os << event->getContent();
  }
};
class LineFormatItem : public LogFormatter::FormatItem {
 public:
  LineFormatItem(const std::string&) {}
  void format(std::ostream& os, LogEvent::ptr event) override {
    os << event->getLine();
  }
};
class LevelFormatItem : public LogFormatter::FormatItem {
 public:
  LevelFormatItem(const std::string&) {}
  void format(std::ostream& os, LogEvent::ptr event) override {
    os << LogLevel::toString(event->getLevel());
  }
};
class ElapseFormatItem : public LogFormatter::FormatItem {
 public:
  ElapseFormatItem(const std::string&) {}
  void format(std::ostream& os, LogEvent::ptr event) override {
    os << event->getElapse();
  }
};
class NameFormatItem : public LogFormatter::FormatItem {
 public:
  NameFormatItem(const std::string&) {}
  void format(std::ostream& os, LogEvent::ptr event) override {
    os << event->getName();
  }
};
class ThreadIdFormatItem : public LogFormatter::FormatItem {
 public:
  ThreadIdFormatItem(const std::string&) {}
  void format(std::ostream& os, LogEvent::ptr event) override {
    os << event->getThreadId();
  }
};

class DateTimeFormatItem : public LogFormatter::FormatItem {
 public:
  DateTimeFormatItem(const std::string& format = "%Y:%m:%d %H:%M:%s")
      : m_format(format) {
    if (m_format.empty()) m_format = "%Y:%m:%d %H:%M:%s";
  }
  ~DateTimeFormatItem() {}
  void format(std::ostream& os, LogEvent::ptr event) override {
    struct tm tm;
    time_t time = event->getTime();
    localtime_r(&time, &tm);
    char buf[64];
    strftime(buf, sizeof(buf), m_format.c_str(), &tm);
    os << buf;
  }

 private:
  std::string m_format;
};

class FileNameFormatItem : public LogFormatter::FormatItem {
 public:
  FileNameFormatItem(const std::string& str = "") {}
  ~FileNameFormatItem() {}
  void format(std::ostream& os, LogEvent::ptr event) override {
    os << event->getFile();
  }
};

class NewLineFormatItem : public LogFormatter::FormatItem {
 public:
  NewLineFormatItem(const std::string& str = "") {}
  ~NewLineFormatItem() {}
  void format(std::ostream& os, LogEvent::ptr event) override { os << "\n"; }
};

class TabFormatItem : public LogFormatter::FormatItem {
 public:
  TabFormatItem(const std::string& str = "") {}
  ~TabFormatItem() {}
  void format(std::ostream& os, LogEvent::ptr event) override { os << "\t"; }
};

class StringFormatItem : public LogFormatter::FormatItem {
 public:
  StringFormatItem(const std::string& str) : m_str(str) {}
  void format(std::ostream& os, LogEvent::ptr event) override { os << m_str; }

 private:
  std::string m_str;
};

class FuncNameFormatItem : public LogFormatter::FormatItem {
 public:
  FuncNameFormatItem(const std::string& str) {}
  void format(std::ostream& os, LogEvent::ptr event) override {
    os << event->getFuncName();
  }
};

void LogFormatter::parse() {
  static std::unordered_map<
      std::string,
      std::function<LogFormatter::FormatItem::ptr(const std::string& str)> >
      s_format_items = {
#define XX(des, type)                                                          \
  {                                                                            \
#des, [](const std::string&                                                \
                 str) { return LogFormatter::FormatItem::ptr(new type(str)); } \
  }
          XX(m, MessageFormatItem),   // m:消息
          XX(p, LevelFormatItem),     // p:日志级别
          XX(r, ElapseFormatItem),    // r:累计毫秒数
          XX(c, NameFormatItem),      // c:日志名称
          XX(t, ThreadIdFormatItem),  // t:线程id
          XX(n, NewLineFormatItem),   // n:换行
          XX(d, DateTimeFormatItem),  // d:时间
          XX(f, FileNameFormatItem),  // f:文件名
          XX(l, LineFormatItem),      // l:行号
          XX(T, TabFormatItem),       // T:Tab
          XX(w, FuncNameFormatItem)   // w:函数名
#undef XX
      };

  if (m_pattern.empty()) {
    m_error = true;
    std::cerr << "[ERROR]"
              << "format pattern is empty" << std::endl;
    return;
  }
  enum class TYPE { cxAG, ITEM, REGEX, PENDING };
  TYPE type;
  char sym = '\0';
  int start = -1;
  for (int i = 0; i < m_pattern.size(); ++i) {
    char ch = m_pattern[i];
    if (ch == '%' && type != TYPE::REGEX) {
      if (i + 1 < m_pattern.size()) {
        if (m_pattern[i + 1] == '%') {
          type = TYPE::cxAG;
          continue;
        } else {
          type = TYPE::ITEM;
          continue;
        }
      }
      if (type == TYPE::cxAG)
        m_items.push_back(FormatItem::ptr(new StringFormatItem("%")));
    } else if (std::isalpha(ch) && type == TYPE::ITEM) {
      if (i + 1 < m_pattern.size()) {
        if (m_pattern[i + 1] == '{') {
          type = TYPE::PENDING;
          sym = ch;
          continue;
        }
      }
      std::string key(1, ch);
      auto it = s_format_items.find(key);
      if (it != s_format_items.end()) m_items.push_back(it->second(""));
    } else {
      if (ch == '{')
        if (i + 1 < m_pattern.size()) {
          if (m_pattern[i + 1] == '%') {
            type = TYPE::REGEX;
            start = i;
            continue;
          } else if (sym != '\0') {
            std::string key(1, sym);
            auto it = s_format_items.find(key);
            if (it != s_format_items.end()) m_items.push_back(it->second(""));
            m_items.push_back(FormatItem::ptr(new StringFormatItem("{")));
          }
        }
      if (ch == '}' && type == TYPE::REGEX && sym != '\0') {
        std::string key(1, sym);
        auto it = s_format_items.find(key);
        if (it != s_format_items.end() && start) {
          std::string tmp = m_pattern.substr(start + 1, i - start - 1);
          if (tmp.empty()) {
            m_error = true;
            std::cerr << "[ERROR]"
                      << "format not valid" << std::endl;
            return;
          }
          m_items.push_back(it->second(tmp));
        }
        sym = '\0';
        start = -1;
        type = TYPE::PENDING;
        continue;
      }
      if (type != TYPE::REGEX) {
        std::string tmp(1, ch);
        m_items.push_back(FormatItem::ptr(new StringFormatItem(tmp)));
      }
    }
  }
}

void StdOutLogAppender::log(Level level, LogEvent::ptr event) {
  if (level >= m_level) {
    lock_guard lock(m_mutex);
    if (!m_formatter->format(std::cout, event)) {
      std::cerr << "[ERROR]"
                << "format error" << std::endl;
    }
  }
}

FileLogAppender::FileLogAppender(const std::string& filename)
    : m_filename(filename) {
  reopen();
}

void FileLogAppender::log(Level level, LogEvent::ptr event) {
  if (level >= m_level) {
    lock_guard lock(m_mutex);
    if (!m_formatter->format(m_filestream, event)) {
      std::cerr << "[ERROR]"
                << "format error" << std::endl;
    }
  }
}

bool FileLogAppender::reopen() {
  lock_guard lock(m_mutex);
  if (m_filestream) {
    m_filestream.close();
  }
  m_filestream.open(m_filename, std::ios::app);
  return !!m_filestream;
}

Logger::Logger(std::string name) : m_name(name), m_level(Level()) {
  m_formatter.reset(new LogFormatter(
      "[%d{%Y-%m-%d %H:%M:%S}]%T%t%%T[%p]%T[%c]%T[%f:%l:%w]%T%m%n"));
}

void Logger::log(Level level, LogEvent::ptr event) {
  if (level >= m_level) {
    lock_guard lock(m_mutex);
    if (!m_appenders.empty())
      for (auto& appender : m_appenders) appender->log(level, event);
    else if (m_root != nullptr)
      m_root->log(level, event);
    else {
      std::cerr << "[FATAL]"
                << "\t"
                << "[" << m_name << "]"
                << "\tRuntime Error : log appender is empty"
                << "\t" << __FILE__ << "\t" << __LINE__ << "\n";
    }
  }
}

void Logger::debug(LogEvent::ptr event) { log(Level::eDebug, event); }

void Logger::info(LogEvent::ptr event) { log(Level::eInfo, event); }

void Logger::warn(LogEvent::ptr event) { log(Level::eWarn, event); }

void Logger::error(LogEvent::ptr event) { log(Level::eError, event); }

void Logger::fatal(LogEvent::ptr event) { log(Level::eFatal, event); }

void Logger::addAppender(LogAppender::ptr appender) {
  lock_guard lock(m_mutex);
  if (!appender->getFormatter()) {
    appender->setFormatter(m_formatter);
  }
  m_appenders.push_front(appender);
}

void Logger::delAppender(LogAppender::ptr appender) {
  lock_guard lock(m_mutex);
  m_appenders.remove(appender);
}

void Logger::clearAppenders() { m_appenders.clear(); }

void Logger::setFormatter(LogFormatter::ptr formatter) {
  lock_guard lock(m_mutex);
  m_formatter = formatter;

  for (auto& appender : m_appenders) {
    lock_guard lock(m_mutex);
    m_formatter = formatter;
    if (!appender->hasFromatter()) {
      appender->setFormatter(formatter);
    }
  }
}

void Logger::setFormatter(const std::string& foramtter) {
  LogFormatter::ptr new_formatter = std::make_shared<LogFormatter>(foramtter);
  if (new_formatter->isError()) {
    std::cerr << "Logger setFormatter name=" << m_name << " value=" << foramtter
              << " invalid formatter" << std::endl;
    return;
  }
  setFormatter(new_formatter);
}

LogFormatter::ptr Logger::getFormatter() {
  lock_guard lock(m_mutex);
  return m_formatter;
}

}  // namespace cx::log

#pragma once

#include <cx/common/common.h>
#include <cx/common/internal.h>
#include <cx/common/noncopyable.h>

#include <string>

namespace cx {

class App : public Noncopyable {
 public:
  struct Arguments {
    int argc;
    char** argv;
  };

  explicit App(const std::string name, const Arguments& arg,
               const Version& version = {1, 0, 0});
  virtual ~App();

  virtual CX_API void run() = 0;
  virtual CX_API void update() = 0;

  const std::string& name() const { return m_name; }

  void reset_name(const std::string& name) { m_name = name; }

  const Version& version() const { return m_version; }

  void update_version(const Version& version) { m_version = version; }

  const Arguments& arguments() const { return m_arguments; }

 protected:
  std::string m_name;
  Version m_version;
  Arguments m_arguments;
};
}  // namespace cx

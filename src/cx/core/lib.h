#pragma once

#include <cx/common/noncopyable.h>

#include <string>

namespace cx {

class Library : public Noncopyable {
 public:
  Library(const std::string& lib);

  bool load();

  void unload();

 private:
};

}  // namespace cx
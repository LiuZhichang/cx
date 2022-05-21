#pragma once

namespace cx::graphics {

class Render {
 public:
  virtual ~Render() = default;

  virtual bool start() = 0;

  virtual void update() = 0;

  private:
  bool m_started;
};

}  // namespace cx::graphics
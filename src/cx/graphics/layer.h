#pragma once

namespace cx {

class GLayer {
 public:
  virtual void onAttach() = 0;
  virtual void OnDetach() = 0;

 private:
};

}  // namespace cx
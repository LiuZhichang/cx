#include <cx/entry_point.h>

class MyApp : public cx::App {
 public:
  MyApp(const Arguments& args) : cx::App("CX Engine", args, {1, 0, 0}) {}

  void run() override;
  void update() override;

 private:
  static int counter;
};
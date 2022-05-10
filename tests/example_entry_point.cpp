#include <cx/entry_point.h>

static auto engine = CX_LOGGER("engine");

class MyApp : public cx::App {
 public:
  MyApp(const Arguments& args) : cx::App("Test App", args) {}

  ~MyApp() {}

  void run() override { LOG_INFO(engine) << "Application run"; }

  void update() override {
    
  }

 private:
};

cx::App* CX_CreateApp(int argc, char** argv) { return new MyApp({argc, argv}); }
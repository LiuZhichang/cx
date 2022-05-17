#include <cx/entry_point.h>

class MyApp : public cx::App {
 public:
  MyApp(const Arguments& args);

  void task() override;
  void update() override;

 private:
  void initWindow();
  void initEngine();

 private:
  static int counter;
};
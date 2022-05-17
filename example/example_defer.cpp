#include <cx/common/defer.h>

#include <iostream>
class A {
 public:
  A() { std::cout << "A constructor" << std::endl; }
  ~A() { std::cout << "A destructor" << std::endl; }
};

void atexit_call() { std::cout << "in main function exit after" << std::endl; }

int main(int argc, char const* argv[]) {
  atexit(atexit_call);

  A* a = new A();
  defer { delete a; };

  std::cout << "--------------------------------" << std::endl;

  return 0;
}

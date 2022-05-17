#include <cx/common/log/log.h>
#include <cx/config/config.h>

int main(int argc, char const *argv[]) {
  cx::Config::LoadFromDirectory("../config");

  return 0;
}

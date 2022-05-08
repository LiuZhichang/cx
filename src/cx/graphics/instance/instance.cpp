#include "instance.h"

#include <cx/common/internal.h>

cx::GInstance::GInstance() { init(std::vector<const char*>()); }

cx::GInstance::GInstance(const char** extensions, uint8_t extensions_count) {
  std::vector<const char*> _extensions(extensions,
                                       extensions + extensions_count);
  init(_extensions);
}

cx::GInstance::~GInstance() {}

template <typename T, typename Container>
void cx::GInstance::init(const Container& extensions) {
  if (extensions.size() == 0) {
  } else {
  }
}

bool cx::GInstance::isEnableValidations() const {
#if defined(CX_DEBUG_MODE)
  return true;
#else
  return false;
#endif
}

#include "application.h"

#include <cx/common/internal.h>
#include <cx/utils/log/log.h>

static auto core = CX_LOGGER("core");
static auto engine = CX_LOGGER("engine");

cx::App::App(const std::string name, const Arguments& arg,
             const Version& version)
    : m_name(std::move(name)), m_version(version), m_arguments(arg) {
#if !defined(CX_DEBUG_MODE)
  core->setLevel(LogLevel::Level::ERROR);
  engine->setLevel(LogLevel::Level::UNKNOWN);
#endif
}

cx::App::~App() {}
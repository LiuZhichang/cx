set_targetdir("$(buildir)/bin/example")
set_group("example")

add_deps("cx")
target("example_defer")
  add_files("example_defer.cpp")

target("example_config")
  add_files("example_config.cpp")

target("example_logger")
  add_files("example_logger.cpp")
  add_links("pthread")

target("example_entry_point")
  add_files("example_entry_point.cpp")

target("example_window")
  add_files("example_window.cpp")
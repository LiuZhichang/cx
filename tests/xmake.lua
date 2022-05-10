set_targetdir("$(buildir)/bin/tests")
add_deps("cx")

target("example_defer")
  add_files("example_defer.cpp")

target("example_logger")
  add_files("example_logger.cpp")
  add_links("pthread")

target("example_entry_point")
  add_files("example_entry_point.cpp")



target("app")
  add_files("*.cpp")
  add_deps("cx")
  set_targetdir("$(buildir)/bin")
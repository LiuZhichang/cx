target("cx")
  set_kind("shared")
  add_includedirs("trd/include")
  add_linkdirs("trd/lib")
  add_links("glfw", "vulkan")
  add_files("./**/*.cpp")
  set_targetdir("$(buildir)/lib")
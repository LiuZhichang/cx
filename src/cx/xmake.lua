target("cx")
  set_kind("shared")
  add_includedirs("trd/include")
  add_linkdirs("trd/lib")
  add_links("glfw", "vulkan", "yaml-cpp")
  add_files("./**/*.cpp")
  set_targetdir("$(buildir)/lib")

  if (is_plat("macos") and is_plat("linux")) then
    add_links("pthread")
  end 
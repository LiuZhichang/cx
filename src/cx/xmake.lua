target("cx")
  set_kind("shared")
  add_includedirs("vendor/include")
  add_linkdirs("vendor/lib")
  add_links("glfw", "vulkan", "yaml-cpp")
  add_files("./**/*.cpp", "vendor/include/stb/*.c")
  set_targetdir("$(buildir)/lib")

  if (is_plat("macos") and is_plat("linux")) then
    add_links("pthread")
  end 
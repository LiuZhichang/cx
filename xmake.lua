add_rules("mode.debug", "mode.release")

set_project("cx")
set_xmakever("2.5.5")
set_languages("cxx20")

if is_mode("debug") then
    add_defines("DEBUG")
    set_symbols("debug")
    set_optimize("none")
end 

-- check platform
if is_plat("windows") then 
    add_defines("CX_PLATFORM_WINDOWS")
elseif is_plat("Linux") then 
    add_defines("CX_PLATFORM_LINUX")
elseif  is_plat("macos") then
    add_defines("CX_PLATFORM_MAC")
-- TODO check android platform
end

add_includedirs("src")

includes("src/cx")
includes("src/sandbox")


set_project("cx")
set_xmakever("2.5.5")
set_languages("cxx17")
--set_config("example", "no")

add_rules("mode.debug", "mode.release")

if is_mode("debug") then
    add_defines("DEBUG")
    set_symbols("debug")
    -- set_optimize("none")
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

add_rpathdirs("src/cx/trd/lib")

add_includedirs("src")

-- includes("example")
includes("src/cx")
includes("src/sandbox")



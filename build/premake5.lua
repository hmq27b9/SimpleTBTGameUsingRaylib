workspace "SimpleTBTGameUsingRaylib"
    architecture "x86_64"
    configurations { "Debug", "Release" }

project "SimpleTBTGameUsingRaylib"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    targetdir "../bin/%{cfg.buildcfg}"
    objdir    "../obj/%{cfg.buildcfg}"

    files { "../**.hpp", "../**.cpp" }
    includedirs { ".." }

    filter "system:linux"
        links { "raylib", "GL", "m", "pthread", "dl", "rt", "X11" }

    filter "configurations:Debug"   defines { "DEBUG" }; symbols "On"
    filter "configurations:Release" defines { "NDEBUG" }; optimize "On"
workspace "t7-overhaul"
    configurations { "Debug", "Release" }
    platforms { "x64" }
    architecture "x86_64"
    location "build"

    cppdialect "C++20"
    staticruntime "On"
    characterset "ASCII"
    systemversion "latest"
    warnings "Extra"
    multiprocessorcompile "On"

    targetdir "%{wks.location}/bin/%{cfg.buildcfg}"
    objdir   "%{wks.location}/obj/%{cfg.buildcfg}/%{prj.name}"

    filter "configurations:Debug"
        defines { "DEBUG", "_DEBUG" }
        symbols "On"
        optimize "Off"
        runtime "Debug"

    filter "configurations:Release"
        defines { "NDEBUG" }
        symbols "Off"
        optimize "Size"
        runtime "Release"

    filter "system:windows"
        defines { "WIN32_LEAN_AND_MEAN", "NOMINMAX", "_CRT_SECURE_NO_WARNINGS" }

    filter {}

    group "deps"
        include "deps/minhook.lua"
    group ""

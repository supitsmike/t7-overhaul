project "client"
    kind "SharedLib"
    language "C++"
    targetname "client"

    pchheader "pch.hpp"
    pchsource "client/pch.cpp"

    files {
        "client/**.cpp",
        "client/**.hpp",
    }

    includedirs {
        "client",
        "common",
        "../deps/minhook/include",
    }

    links {
        "common",
        "minhook"
    }

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
        "../deps/minhook/include",
    }

    links { "minhook" }

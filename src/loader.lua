project "loader"
    kind "ConsoleApp"
    language "C++"

    pchheader "pch.hpp"
    pchsource "loader/pch.cpp"

    files {
        "loader/**.cpp",
        "loader/**.hpp",
    }

    includedirs {
        "loader",
        "common",
    }

    links {
        "common"
    }

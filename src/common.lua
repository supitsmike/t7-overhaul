project "common"
    kind "StaticLib"
    language "C++"

    files {
        "common/**.cpp",
        "common/**.hpp",
    }

    includedirs {
        "common",
        "../deps/minhook/include",
    }

    links {
        "minhook"
    }

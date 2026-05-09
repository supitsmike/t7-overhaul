project "minhook"
    kind "StaticLib"
    language "C"
    warnings "Off"

    files {
        "minhook/src/**.c",
        "minhook/src/**.h",
    }

    includedirs {
        "minhook/include",
    }

project "Glad"
    kind "StaticLib"
    language "C"
    staticruntime "off"

    location ".\\"

    targetdir (tp_bin_out)
    objdir (tp_int_out)

    files
    {
        "include/glad/glad.h",
        "include/KHR/khrplatform.h",
        "src/glad.c"
    }

    includedirs
    {
        "include"
    }

    filter "system:windows"
        systemversion "latest"
    
    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"
    filter "configurations:Release"
        runtime "Release"
        optimize "on"

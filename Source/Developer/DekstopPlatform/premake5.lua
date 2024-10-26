-- premake5.lua

project "DesktopPlatform"
    kind "SharedLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    targetdir (engine_bin_out)
    objdir (engine_int_out)

    location "%{wks.location}/Build/ProjectFiles"

    files
    {
        "%{wks.location}/Source/Developer/**.h",
        "%{wks.location}/Source/Developer/**.cpp"
    }

    includedirs
    {
        "%{wks.location}/Source/VeiM",
        "%{wks.location}/Source/Developer"
    }

    defines { "DESKTOPPLATFORM_EXPORTS"}

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Debug_Editor"
        runtime "Debug"
        symbols "on"
    
    filter "configurations:Development"
        runtime "Release"
        optimize "on"
        symbols "on"

    filter "configurations:Development_Editor"
        runtime "Release"
        optimize "on"
        symbols "on"

    filter "configurations:Shipping"
        runtime "Release"
        optimize "on"
        symbols "off"
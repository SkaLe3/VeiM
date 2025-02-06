-- premake5.lua

project "DesktopPlatform"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"
	buildoptions { "/utf-8" }

    targetdir (engine_bin_out)
    objdir (engine_int_out)
    
    location "%{wks.location}/Engine/Build/ProjectFiles"

    files
    {
        "%{wks.location}/Engine/Source/Developer/DesktopPlatform/Source/**.h",
        "%{wks.location}/Engine/Source/Developer/DesktopPlatform/Source/**.cpp"
    }

    includedirs
    {
        "%{wks.location}/Engine/Source/VeiM/Core/Source",
        "%{wks.location}/Engine/Source/Developer/DesktopPlatform/Source",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.GLFW}"
    }

    links 
    {
        "Core",
        "GLFW"
    }

    filter "configurations:Debug"
        defines "VM_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Debug_Editor"
        defines "VM_DEBUG"
        runtime "Debug"
        symbols "on"
        defines {"VM_WITH_EDITOR"}

    filter "configurations:Development"
        defines "VM_DEVELOPMENT"
        runtime "Release"
        optimize "on"
        symbols "on"

    filter "configurations:Development_Editor"
        defines "VM_DEVELOPMENT"
        runtime "Release"
        optimize "on"
        symbols "on"
        defines {"VM_WITH_EDITOR"}
        
    filter "configurations:Shipping"
        defines "VM_SHIPPING"
        runtime "Release"
        optimize "on"
        symbols "off"
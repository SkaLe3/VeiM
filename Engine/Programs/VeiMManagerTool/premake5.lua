-- premake5.lua

project "VeiMManagerTool"
    kind "ConsoleApp" -- change to WindowedApp
    language "C++"
    cppdialect "C++20"
    staticruntime "off"
    
    targetdir (engine_bin_out)
    objdir (engine_int_out)

    location "%{wks.location}/Engine/Build/ProjectFiles"

    files 
    {
        "Source/**.h",
        "Source/**.cpp",
        "Source/Resources/Resource.rc"
    }

    includedirs
    {
        "%{wks.location}/Engine/Source/VeiM",
        "%{wks.location}/Engine/Source/Developer",
        "%{wks.location}/Engine/ThirdParty", 
		"%{wks.location}/Engine/ThirdParty/glm",
		"%{wks.location}/Engine/ThirdParty/imgui",
		"%{wks.location}/Engine/ThirdParty/GLFW/include",
		"%{wks.location}/Engine/ThirdParty/spdlog/include",
		"%{wks.location}/Engine/ThirdParty/yaml-cpp/include",
		"%{wks.location}/Engine/ThirdParty/src"
    }

    links
    {
        "DesktopPlatform"
    }

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

    
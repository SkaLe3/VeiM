-- premake5.lua

project "VeiMManagerTool"
    kind "ConsoleApp" -- change to WindowedApp
    language "C++"
    cppdialect "C++20"
    staticruntime "off"
    
    targetdir (engine_bin_out)
    objdir (engine_int_out)

    files 
    {
        "Source/**.h",
        "Source/**.cpp",
        "Source/Resources/Resource.rc"
    }

    includedirs
    {
        "%{wks.location}/Source/VeiM",
        "%{wks.location}/Source/Developer",
        "%{wks.location}/ThirdParty", 
		"%{wks.location}/ThirdParty/glm",
		"%{wks.location}/ThirdParty/imgui",
		"%{wks.location}/ThirdParty/GLFW/include",
		"%{wks.location}/ThirdParty/spdlog/include",
		"%{wks.location}/ThirdParty/yaml-cpp/include",
		"%{wks.location}/ThirdParty/src"
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

    
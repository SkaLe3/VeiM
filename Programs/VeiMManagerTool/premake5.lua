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
        "%{wks.location}/VeiM/Source",
        "%{wks.location}/VeiM/ThirdParty", 
		"%{wks.location}/VeiM/ThirdParty/glm",
		"%{wks.location}/VeiM/ThirdParty/imgui",
		"%{wks.location}/VeiM/ThirdParty/GLFW/include",
		"%{wks.location}/VeiM/ThirdParty/spdlog/include",
		"%{wks.location}/VeiM/ThirdParty/yaml-cpp/include",
		"%{wks.location}/VeiM/ThirdParty/src"
    }

    links
    {

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

    
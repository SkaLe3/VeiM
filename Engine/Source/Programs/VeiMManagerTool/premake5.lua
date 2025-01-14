-- premake5.lua

local target_name = "VeiMManagerTool-%{cfg.platform}-%{cfg.buildcfg}"
local target_prefix = ""
local extension = "exe"
local app_name = "VeiMManagerTool"
local resource_defines = {}

project "VeiMManagerTool"
    kind "WindowedApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"
    
    targetdir (engine_bin_out)
    objdir (engine_int_out)
    targetname (target_name)
    targetprefix (target_prefix)

    debugdir "%{cfg.targetdir}"

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

    filter "system:windows"
        systemversion "latest"
        prebuildcommands {
            "rc /fo " .. engine_int_out .. "/Resource.rc.res %{wks.location}/Engine/Source/Programs/VeiMManagerTool/Source/Resources/Resource.rc",
            "rc /fo " .. engine_int_out .. "/Resource.rc2.res %{wks.location}/Engine/Source/Programs/VeiMManagerTool/Source/Resources/Resource.rc2"
        }

        linkoptions{
            engine_int_out .. "/Resource.rc.res",
            engine_int_out .. "/Resource.rc2.res"
        }

    filter "configurations:Debug"
        defines "VM_DEBUG"
		runtime "Debug"
		symbols "on"
	
	filter "configurations:Debug_Editor"
        defines "VM_DEBUG"
		runtime "Debug"
		symbols "on"
		
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

	filter "configurations:Shipping"
        defines "VM_SHIPPING"
		runtime "Release"
		optimize "on"
        symbols "off"

    
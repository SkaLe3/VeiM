-- premake5.lua

local target_name = "VeimManagerTool"
local target_prefix = ""
local target_suffix = ""
local extension = "exe"
local app_name = "VeimManagerTool"
local resource_defines = {}

project "VeimManagerTool"
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
        "%{wks.location}/Engine/Source/Programs/VeimManagerTool/Source/**.h",
        "%{wks.location}/Engine/Source/Programs/VeimManagerTool/Source/**.cpp",
        "%{wks.location}/Engine/Source/Programs/VeimManagerTool/Source/**.rc",
        "%{wks.location}/Engine/Source/Programs/VeimManagerTool/Source/**.rc2",
        "%{wks.location}/Engine/Source/Programs/VeimManagerTool/Source/**.ico"
    }

    includedirs
    {
        "%{wks.location}/Engine/Source/Programs/VeimManagerTool/Source",
        "%{wks.location}/Engine/Source/VeiM/Core/Source",
        "%{wks.location}/Engine/Source/Developer/DesktopPlatform/Source",
        "%{wks.location}/Engine/ThirdParty", 
		"%{wks.location}/Engine/ThirdParty/glm",
		"%{wks.location}/Engine/ThirdParty/imgui",
		"%{wks.location}/Engine/ThirdParty/GLFW/include",
		"%{wks.location}/Engine/ThirdParty/spdlog/include",
		"%{wks.location}/Engine/ThirdParty/yaml-cpp/include",
		"%{wks.location}/Engine/ThirdParty/src"
    }

    defines 
    {
        "YAML_CPP_STATIC_DEFINE"
    }

    links
    {
        "DesktopPlatform",
        "yaml-cpp"
    }

    filter "system:windows"
        systemversion "latest"
        prebuildcommands {
            "rc /fo " .. engine_int_out .. "/Resource.rc.res %{wks.location}/Engine/Source/Programs/VeimManagerTool/Source/Windows/Resources/Resource.rc",
            "rc /fo " .. engine_int_out .. "/Resource.rc2.res %{wks.location}/Engine/Source/Programs/VeimManagerTool/Source/Windows/Resources/Resource.rc2"
        }

    filter "configurations:Debug"
        defines "VM_DEBUG"
		runtime "Debug"
		symbols "on"
        targetsuffix "-%{cfg.platform}-Debug"
	
	filter "configurations:Debug_Editor"
        defines "VM_DEBUG"
		runtime "Debug"
		symbols "on"
        targetsuffix "-%{cfg.platform}-Debug"
		
	filter "configurations:Development"
        defines "VM_DEVELOPMENT"
		runtime "Release"
		optimize "on"
        symbols "on"
        targetsuffix"-%{cfg.platform}-Development"

	filter "configurations:Development_Editor"
        defines "VM_DEVELOPMENT"
		runtime "Release"
		optimize "on"
        symbols "on"
        targetsuffix "-%{cfg.platform}-Development"

	filter "configurations:Shipping"
        defines "VM_SHIPPING"
		runtime "Release"
		optimize "on"
        symbols "off"
        targetsuffix "-%{cfg.platform}-Shipping"

    
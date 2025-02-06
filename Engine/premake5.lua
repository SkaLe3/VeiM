-- premake5.lua
include "Dependencies.lua"
include "Common.lua"

workspace "VeiM"
	architecture "x64"
	startproject "VeimEditor"
	configurations { "Debug","Debug_Editor", "Development", "Development_Editor", "Shipping" }
	platforms {"Win64"}
	buildoptions { "/utf-8" }

	filter "action:vs*"
        buildoptions { "/MP" }
    filter {}

	location "..\\"
	
	filter "configurations:Debug or Development or Shipping"
		defines { "IS_UNIFIED" }
	filter {}

outputdir = "%{cfg.platform}"
engine_bin_out = "%{wks.location}/Engine/Binaries/" .. outputdir .. "/%{cfg.buildcfg}"
engine_int_out = "%{wks.location}/Engine/Intermediate/" .. outputdir .. "/%{prj.name}/%{cfg.buildcfg}"
tp_bin_out = "%{wks.location}/Engine/Binaries/ThirdParty/" .. outputdir .. "/%{prj.name}/%{cfg.buildcfg}"
tp_int_out = "%{wks.location}/Engine/Intermediate/ThirdParty/" .. outputdir .. "/%{prj.name}/%{cfg.buildcfg}"

imgui_mathlib_path = IncludeDir["glm"]
imgui_config_path = "%{wks.location}/Engine/Source/VeiM/Core/Source/ImGui"
imgui_config_file = "VeimImGuiConfig.h"


group "Dependencies"
	include "ThirdParty/GLFW"
	include "ThirdParty/Glad"
	include "ThirdParty/imgui"
	include "ThirdParty/yaml-cpp"
group ""

group "Engine"
	include "Source/VeiM/Core"
    include "Source/Editor/VeimEditor"
group ""

group "Developer"
    include "Source/Developer/DesktopPlatform"
group ""

group "Programs"
    include "Source/Programs/VeiMManagerTool"
group ""




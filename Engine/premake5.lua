-- premake5.lua
include "Dependencies.lua"
include "Common.lua"

workspace "VeiM"
	architecture "x64"
	startproject "VeiMEditor"
	configurations { "Debug","Debug_Editor", "Development", "Development_Editor", "Shipping" }
	platforms {"Win64"}
	buildoptions { "/utf-8" }

	filter "action:vs*"
        buildoptions { "/MP" }
    filter {}

	location "..\\"
	
outputdir = "%{cfg.platform}"
engine_bin_out = "%{wks.location}/Engine/Binaries/" .. outputdir .. "/%{cfg.buildcfg}"
engine_int_out = "%{wks.location}/Engine/Intermediate/" .. outputdir .. "/%{cfg.buildcfg}"
tp_bin_out = "%{wks.location}/Engine/Binaries/ThirdParty/" .. outputdir .. "/%{prj.name}/%{cfg.buildcfg}"
tp_int_out = "%{wks.location}/Engine/Intermediate/ThirdParty/" .. outputdir .. "/%{prj.name}/%{cfg.buildcfg}"

group "Dependencies"
	include "ThirdParty/GLFW"
	include "ThirdParty/Glad"
	include "ThirdParty/imgui"
	include "ThirdParty/yaml-cpp"
group ""

group "Engine"
	include "Source/VeiM"
    include "Source/VeiM-Editor"
group ""

group "Developer"
    include "Source/Developer/DekstopPlatform"
group ""

group "Programs"
    include "Source/Programs/VeiMManagerTool"
group ""

--group "Misc"
	--include "Sandbox"
--group ""






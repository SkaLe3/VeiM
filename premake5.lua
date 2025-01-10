-- premake5.lua
include "Dependencies.lua"
include "Common.lua"

workspace "VeiM"
	architecture "x64"
	startproject "VeiMEditor"
	configurations { "Debug","Debug_Editor", "Development", "Development_Editor", "Shipping" }
	buildoptions { "/MP" }
	
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
engine_bin_out = "%{wks.location}/Binaries/Engine/" .. outputdir .. "/Engine"
engine_int_out = "%{wks.location}/Intermediate/Engine/" .. outputdir .. "/Engine"
tp_bin_out = "%{wks.location}/Binaries/ThirdParty/" .. outputdir .. "/%{prj.name}"
tp_int_out = "%{wks.location}/Intermediate/ThirdParty/" .. outputdir .. "/%{prj.name}"

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
    include "Programs/VeiMManagerTool"
group ""

group "Misc"
	include "Sandbox"
group ""






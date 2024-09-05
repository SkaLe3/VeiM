-- premake5.lua
include "Dependencies.lua"
include "Common.lua"

workspace "VeiM"
	architecture "x64"
	startproject "VeiMEditor"
	configurations { "Debug","Debug_Editor", "Development", "Development_Editor", "Shipping" }
	buildoptions { "/MP" }
	
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
engine_bin_out = "%{wks.location}/Binaries/Engine/" .. outputdir .. "/%{prj.name}"
engine_int_out = "%{wks.location}/Intermediate/Engine/" .. outputdir .. "/%{prj.name}"
tp_bin_out = "%{wks.location}/Binaries/ThirdParty/" .. outputdir .. "/%{prj.name}"
tp_int_out = "%{wks.location}/Intermediate/ThirdParty/" .. outputdir .. "/%{prj.name}"

group "Dependencies"
	include "VeiM/ThirdParty/GLFW"
	include "VeiM/ThirdParty/Glad"
	include "VeiM/ThirdParty/imgui"
	include "VeiM/ThirdParty/yaml-cpp"
group ""

group "Engine"
	include "VeiM"
group ""

group "Tools"
	include "VeiM-Editor"
group ""

group "Misc"
	include "Sandbox"
group ""






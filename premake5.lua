-- premake5.lua
include "Dependencies.lua"

workspace "VeiM"
	architecture "x64"
	startproject "VeiMEditor"
	configurations { "Debug","Debug_Editor", "Development", "Development_Editor", "Shipping" }
	buildoptions { "/MP" }
	
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "VeiM/ThirdParty/GLFW"
	include "VeiM/ThirdParty/Glad"
	include "VeiM/ThirdParty/imgui"
	include "VeiM/ThirdParty/yaml-cpp"
group ""

group "Core"
	include "VeiM"
group ""

group "Tools"
	include "VeiM-Editor"
group ""

group "Misc"
	include "Sandbox"
group ""






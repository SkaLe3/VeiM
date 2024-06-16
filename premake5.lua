-- premake5.lua
include "Dependencies.lua"

workspace "VeiM"
	architecture "x64"
	startproject "VeiMEditor"
	configurations { "Debug", "Release", "Dist" }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "VeiM/vendor/GLFW"
	include "VeiM/vendor/Glad"
	include "VeiM/vendor/imgui"
	include "VeiM/vendor/yaml-cpp"
group ""

group "Core"
	include "VeiM"
group ""

group "Tools"
	include "VeiMEditor"
group ""

group "Misc"
	include "Sandbox"
group ""
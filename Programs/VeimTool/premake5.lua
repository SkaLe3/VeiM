-- premake5.lua
workspace "VeiMTool"
	architecture "x64"
	startproject "VeiMTool"
	configurations { "Debug", "Release" }
	buildoptions { "/MP" }
	
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"


include "VeiMTool"
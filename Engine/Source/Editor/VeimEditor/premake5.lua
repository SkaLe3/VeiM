-- premake5.lua Editor module config

-- It contains build setting for the VeiM Editor module

-- Editor target parameters
local target_name = "VeimEditor"		-- Name of the target file
local target_prefix = ""				-- Prefix for the target file
local target_suffix = ""				-- Suffix for the target file
local extension = "exe"					-- Extenstion for the target file
local app_name = "VeimEditor"			-- Application name for details
local resource_defines = {}				-- List of resource-related defines

-- Editor Module Definition
project "VeimEditor"
	-- General Project Settings
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"
	
	-- Output Directories
    targetdir (engine_bin_out)
	objdir (engine_int_out)
    targetname (target_name)
    targetprefix (target_prefix)
	debugdir "%{cfg.targetdir}"
	
	-- Project Files Location
    location "%{wks.location}/Engine/Build/ProjectFiles"

	-- Filters for Configuration with Editor-Specific Source files
	filter "configurations:Debug_Editor or Development_Editor"
    	files
    	{
    	    "%{wks.location}/Engine/Source/Editor/VeimEditor/**.h",
    	    "%{wks.location}/Engine/Source/Editor/VeimEditor/**.cpp",
			"%{wks.location}/Engine/Build/Resources/**.rc",
			"%{wks.location}/Engine/Build/Resources/**.ico",
			"%{wks.location}/Engine/Source/VeiM/Core/Resources/**.h",
			"%{wks.location}/Engine/Source/VeiM/Core/Resources/**.rc"
    	}
	filter {}

	-- Include Directories
    includedirs
    {
        "%{wks.location}/Engine/Source/Editor/VeimEditor/Source",
		"%{wks.location}/Engine/Source/Editor/VeimEditor/Utils",
        "%{wks.location}/Engine/Source/VeiM/Core/Source",
		"%{wks.location}/Engine/Source/Developer/DesktopPlatform/Source",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.ImNodeEditor}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.yaml_cpp}" ,
		"%{IncludeDir.entt}"
    }

	-- Editor Dependencies
	links 
	{ 
		"Core",									-- Link against the VeiM Engine
		"DesktopPlatform",
		"ImGui",
		"ImNodeEditor"
	}
	
	-- Define Macros
	defines 
	{
		"VM_EDITOR=1",
		"YAML_CPP_STATIC_DEFINE",				-- Use static YAML-CPP
        "VM_APP_NAME=\"" .. app_name .. "\""	-- Define application name
	}

    ------------------ Resources ------------------------

    -- Resource Defines
	table.insert(resource_defines, "BUILD_ICON_FILE_NAME=\\\"Default.ico\\\"")
    table.insert(resource_defines, "VM_APP_NAME=\\\"".. app_name .. "\\\"")
    table.insert(resource_defines, "ORIGINAL_FILE_NAME=\\\"".. target_prefix ..target_name .. target_suffix .. ".".. extension .."\\\"")

	-- Resource Compilation and Linking
	filter "system:windows"
		systemversion "latest"
        local resource_build_options = constructResourceDefineFlags(resource_defines)  
        prebuildcommands {
            "rc /fo " .. engine_int_out .. "/VeiM.rc.res %{wks.location}/Engine/Source/VeiM/Core/Resources/VeiM.rc",
            "rc /fo " .. engine_int_out .."/Default.rc2.res" .. resource_build_options .. " %{wks.location}/Engine/Build/Resources/Default.rc2"
        }

        linkoptions{
            engine_int_out .. "/VeiM.rc.res",
            engine_int_out .. "/Default.rc2.res"
        }

	-----------------------------------------------------	

	-- Debug Command for Non-Editor Configurations
	filter "configurations:Debug or Development or Shipping"
		debugcommand "C:\\Windows\\System32\\cmd.exe"

	------------------ Configuration Filters ------------------

	-- Debug Configuration
	filter "configurations:Debug"
		kind "None"
		runtime "Debug"
		symbols "on"
		targetsuffix "-%{cfg.platform}-Debug"
	
	-- Debug Editor Configuration
	filter "configurations:Debug_Editor"
		defines 		{"VM_WITH_EDITOR"}
		runtime 		"Debug"
		symbols 		"on"
		targetsuffix "-%{cfg.platform}-Debug"

	-- Development Configuration
	filter "configurations:Development"
		kind 			"None"
		runtime 		"Release"
		optimize 		"on"
        symbols 		"on"

	-- Development Editor Configuration
	filter "configurations:Development_Editor"
		defines 		{"VM_WITH_EDITOR"}
		runtime 		"Release"
		optimize 		"on"
        symbols 		"on"

	-- Shipping Configuration
	filter "configurations:Shipping"
		kind 			"None"
		runtime 		"Release"
		optimize 		"on"
        symbols 		"off"
	
	-----------------------------------------------------------

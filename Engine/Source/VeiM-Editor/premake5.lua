local target_name = "VeiMEditor"
local target_prefix = ""
local extension = "exe"
local app_name = "VeiMEditor"
local resource_defines = {}

project "VeiMEditor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

    targetdir (engine_bin_out)
	objdir (engine_int_out)
    targetname (target_name)
    targetprefix (target_prefix)

    location "%{wks.location}/Engine/Build/ProjectFiles"

    files
    {
        "%{wks.location}/Engine/Source/VeiM-Editor/**.h",
        "%{wks.location}/Engine/Source/VeiM-Editor/**.cpp"
    }

    includedirs
    {
        "%{wks.location}/Engine/Source/VeiM-Editor",
        "%{wks.location}/Engine/Source/VeiM",
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
		"VeiM"
	}
	defines 
	{
		"YAML_CPP_STATIC_DEFINE",
        "VM_APP_NAME=\"" .. app_name .. "\""
	}
    ------------------ resources ------------------------
    table.insert(resource_defines, "BUILD_ICON_FILE_NAME=\\\"Default.ico\\\"")
    table.insert(resource_defines, "VM_APP_NAME=\\\"".. app_name .. "\\\"")
    table.insert(resource_defines, "ORIGINAL_FILE_NAME=\\\"".. target_prefix ..target_name .. ".".. extension .."\\\"")

	filter "system:windows"
		systemversion "latest"
        local resource_build_options = constructResourceDefineFlags(resource_defines)  
        prebuildcommands {
            "rc /fo " .. engine_int_out .. "/VeiM.rc.res %{wks.location}/Engine/Source/VeiM/VeiM/Resources/VeiM.rc",
            "rc /fo " .. engine_int_out .."/Default.rc2.res" .. resource_build_options .. " %{wks.location}/Engine/Build/Resources/Default.rc2"
        }

        linkoptions{
            engine_int_out .. "/VeiM.rc.res",
            engine_int_out .. "/Default.rc2.res"
        }
	-----------------------------------------------------	
	filter "configurations:Debug"
		defines "VM_DEBUG"
		runtime "Debug"
		symbols "on"
		defines {"VM_WITH_EDITOR"} --temporary define
	
	filter "configurations:Debug_Editor"
		defines "VM_DEBUG"
		runtime "Debug"
		symbols "on"
		defines {"VM_WITH_EDITOR"} --temporary define
		
	filter "configurations:Development"
		defines "VM_DEVELOPMENT"
		runtime "Release"
		optimize "on"
        symbols "on"
		defines {"VM_WITH_EDITOR"} --temporary define

	filter "configurations:Development_Editor"
		defines "VM_DEVELOPMENT"
		runtime "Release"
		optimize "on"
        symbols "on"
		defines {"VM_WITH_EDITOR"} --temporary define

	filter "configurations:Shipping"
		defines "VM_SHIPPING"
		runtime "Release"
		optimize "on"
        symbols "off"
		defines {"VM_WITH_EDITOR"} --temporary define


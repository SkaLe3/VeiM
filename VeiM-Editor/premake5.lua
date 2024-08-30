project "VeiMEditor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

	targetdir ("%{wks.location}/Build/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/Build/bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "Source/**.h",
        "Source/**.cpp",

		-- resources
		"%{wks.location}/VeiM/Resources/Icons/VeiM.rc"
    }

    includedirs
    {
        "%{wks.location}/VeiM/Source",
		"%{wks.location}/VeiM/ThirdParty", 
		"%{wks.location}/VeiM/ThirdParty/glm",
		"%{wks.location}/VeiM/ThirdParty/imgui",
		"%{wks.location}/VeiM/ThirdParty/GLFW/include",
		"%{wks.location}/VeiM/ThirdParty/spdlog/include",
		"%{wks.location}/VeiM/ThirdParty/yaml-cpp/include",
		"%{wks.location}/VeiM/ThirdParty/src",

		-- resources
		"%{wks.location}/VeiM/Resources/Icons/VeiM"
    }

	links 
	{ 
		"VeiM"
	}
	defines 
	{
		"YAML_CPP_STATIC_DEFINE"
	}
	filter "system:windows"
		systemversion "latest"
		
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


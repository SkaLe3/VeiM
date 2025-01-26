project "VeiM"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

    targetdir (engine_bin_out)
    objdir (engine_int_out)

    location "%{wks.location}/Engine/Build/ProjectFiles"

	
	filter "configurations:Debug_Editor or Development_Editor"
    	files
    	{
        	"%{wks.location}/Engine/Source/VeiM/**.h",
        	"%{wks.location}/Engine/Source/VeiM/**.cpp",
			"%{wks.location}/Engine/ThirdParty/stb_image/**.h",
			"%{wks.location}/Engine/ThirdParty/stb_image/**cpp",
			"%{wks.location}/Engine/ThirdParty/glm/glm/**.hpp",
			"%{wks.location}/Engine/ThirdParty/glm/glm/**.inl"
    	}	
	filter "configurations:Debug or Development or Shipping"
		files
		{
			"%{wks.location}/Engine/Source/VeiM/**.h",
			"%{wks.location}/Engine/Source/VeiM/**.cpp",
			"%{wks.location}/Engine/ThirdParty/stb_image/**.h",
			"%{wks.location}/Engine/ThirdParty/stb_image/**cpp",
			"%{wks.location}/Engine/ThirdParty/glm/glm/**.hpp",
			"%{wks.location}/Engine/ThirdParty/glm/glm/**.inl"
		}	
		removefiles 
		{ 
			"VeiM/ImGui/**.h",
			"VeiM/ImGui/**.cpp",
			"VeiM/UI/**.h",
			"VeiM/UI/**.cpp" 
		}
	filter {}

    includedirs
    {
        "%{wks.location}/Engine/Source/VeiM",
		"%{wks.location}/Engine/ThirdParty",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.stb_image}"
    }

	defines
	{
        "_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE",
		"YAML_CPP_STATIC_DEFINE"
	}
    

	links 
	{ 
		"Glad",
		"GLFW",
		"yaml-cpp",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "VM_DEBUG"
		runtime "Debug"
		symbols "on"
	
	filter "configurations:Debug_Editor"
		defines "VM_DEBUG"
		runtime "Debug"
		symbols "on"
		defines {"VM_WITH_EDITOR"}
		links {"ImGui"}
		
	filter "configurations:Development"
		defines "VM_DEVELOPMENT"
		runtime "Release"
		optimize "on"
        symbols "on"

	filter "configurations:Development_Editor"
		defines "VM_DEVELOPMENT"
		runtime "Release"
		optimize "on"
        symbols "on"
		defines {"VM_WITH_EDITOR"}
		links {"ImGui"}

	filter "configurations:Shipping"
		defines "VM_SHIPPING"
		runtime "Release"
		optimize "on"
        symbols "off"


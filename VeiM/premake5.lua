project "VeiM"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

    targetdir (engine_bin_out)
    objdir (engine_int_out)

    files
    {
        "Source/**.h",
        "Source/**.cpp",
		"ThirdParty/stb_image/**.h",
		"ThirdParty/stb_image/**cpp",
		"ThirdParty/glm/glm/**.hpp",
		"ThirdParty/glm/glm/**.inl"
    }

    includedirs
    {
        "Source",
		"ThirdParty",
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
		"ImGui",
		"yaml-cpp",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		--removefiles { "Source/VeiM/ImGui/**.cpp", "Source/VeiM/ImGui/**.h","Source/VeiM/UI/**.cpp", "Source/VeiM/UI/**.h" }
		--temporary remove
		defines "VM_DEBUG"
		runtime "Debug"
		symbols "on"
		defines {"VM_WITH_EDITOR"} --temporary define
	
	filter "configurations:Debug_Editor"
		defines "VM_DEBUG"
		runtime "Debug"
		symbols "on"
		defines {"VM_WITH_EDITOR"}
		
	filter "configurations:Development"
		--removefiles { "Source/VeiM/ImGui/**.cpp", "Source/VeiM/ImGui/**.h","Source/VeiM/UI/**.cpp", "Source/VeiM/UI/**.h" }
				--temporary remove
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
		defines {"VM_WITH_EDITOR"}

	filter "configurations:Shipping"
		--removefiles { "Source/VeiM/ImGui/**.cpp", "Source/VeiM/ImGui/**.h","Source/VeiM/UI/**.cpp", "Source/VeiM/UI/**.h" }
				--temporary remove
		defines "VM_SHIPPING"
		runtime "Release"
		optimize "on"
        symbols "off"
		defines {"VM_WITH_EDITOR"} --temporary define


project "VeiM"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "src/**.h",
        "src/**.cpp"
		-- stb_image
		-- glm
    }

    includedirs
    {
        "src",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.ImGui}"
    }

	defines
	{
        "_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}
    

	links 
	{ 
		"Glad",
		"GLFW",
		"ImGui",
		--"yaml-cpp",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "VM_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "VM_RELEASE"
		runtime "Release"
		optimize "on"
        symbols "on"

    filter "configurations:Dist"
		defines "VM_Dist"
		runtime "Release"
		optimize "on"
        symbols "off"


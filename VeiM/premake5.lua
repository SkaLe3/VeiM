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
        "src/**.cpp",
		"vendor/stb_image/**.h",
		"vendor/stb_image/**cpp",
		"vendor/glm/glm/**.hpp",
		"vendor/glm/glm/**.inl"
    }

    includedirs
    {
        "src",
		"vendor",
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


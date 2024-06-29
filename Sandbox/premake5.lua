project "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "src/**.h",
        "src/**.cpp"
    }

    includedirs
    {
        "%{wks.location}/VeiM/src",
		"%{wks.location}/VeiM/vendor",
		"src",
		"%{wks.location}/VeiM/vendor/glm",
		"%{wks.location}/VeiM/vendor/imgui",	
    }

	links 
	{ 
		"VeiM"
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


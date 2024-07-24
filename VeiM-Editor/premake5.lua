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
        "Source/**.cpp"
    }

    includedirs
    {
        "%{wks.location}/VeiM/Source",
		"%{wks.location}/VeiM/ThirdParty"
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


project "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

	targetdir ("%{wks.location}/Engine/Build/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/Engine/Build/bin-int/" .. outputdir .. "/%{prj.name}")

	location "%{wks.location}/Engine/Build/ProjectFiles"

    files
    {
        "Source/**.h",
        "Source/**.cpp"
    }

    includedirs
    {
        "%{wks.location}/Engine/Source",
		"%{wks.location}/Engine/ThirdParty",
		"Source",
		"%{wks.location}/Engine/ThirdParty/glm",
		"%{wks.location}/Engine/ThirdParty/imgui",	
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
		
	filter "configurations:Debug Editor"
		defines {"VM_DEBUG","VM_WITH_EDITOR"} 
		runtime "Debug"
		symbols "on"

	filter "configurations:Developmnet"
		defines "VM_DEVELOPMENT"
		runtime "Release"
		optimize "on"
        symbols "on"

	filter "configurations:Developmnet Editor"
		defines {"VM_DEVELOPMENT","VM_WITH_EDITOR"} 
		runtime "Release"
		optimize "on"
        symbols "on"
    
	filter "configurations:SHIPPING"
		defines "VM_SHIPPING"
		runtime "Release"
		optimize "on"
        symbols "off"


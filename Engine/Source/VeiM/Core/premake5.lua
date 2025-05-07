project "Core"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

    targetdir (engine_bin_out)
    objdir (engine_int_out)

    location "%{wks.location}/Engine/Build/ProjectFiles"

	
	filter "configurations:Debug_Editor or Development_Editor"
    	kind "SharedLib"
		defines { "CORE_EXPORTS" }
		files
    	{
        	"%{wks.location}/Engine/Source/VeiM/Core/Source/**.h",
        	"%{wks.location}/Engine/Source/VeiM/Core/Source/**.cpp",
			"%{wks.location}/Engine/ThirdParty/stb_image/**.h",
			"%{wks.location}/Engine/ThirdParty/stb_image/**cpp",
			"%{wks.location}/Engine/ThirdParty/glm/glm/**.hpp",
			"%{wks.location}/Engine/ThirdParty/glm/glm/**.inl",
			"%{wks.location}/Engine/ThirdParty/ImGuizmo/ImGuizmo.h",
			"%{wks.location}/Engine/ThirdParty/ImGuizmo/ImGuizmo.cpp"
    	}	
	filter "configurations:Debug or Development or Shipping"
		kind "StaticLib"
		files
		{
			"%{wks.location}/Engine/Source/VeiM/Core/Source/**.h",
			"%{wks.location}/Engine/Source/VeiM/Core/Source/**.cpp",
			"%{wks.location}/Engine/ThirdParty/stb_image/**.h",
			"%{wks.location}/Engine/ThirdParty/stb_image/**cpp",
			"%{wks.location}/Engine/ThirdParty/glm/glm/**.hpp",
			"%{wks.location}/Engine/ThirdParty/glm/glm/**.inl"
		}	
		removefiles 
		{ 
			"Source/ImGui/**.h",
			"Source/ImGui/**.cpp",
			"Source/UI/**.h",
			"Source/UI/**.cpp" 
		}
	filter {}

    includedirs
    {
        "%{wks.location}/Engine/Source/VeiM/Core/Source",
		"%{wks.location}/Engine/ThirdParty",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.assimp}",
		"%{IncludeDir.assimpCfg}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.CityHash}"
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
		"assimp",
		"CityHash",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "VM_DEBUG"
		runtime "Debug"
		symbols "on"
		defines {"VM_GAME=1"}
	
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
		defines {"VM_GAME=1"}

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
		defines {"VM_GAME=1"}


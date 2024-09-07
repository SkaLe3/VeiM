project "yaml-cpp"
	kind "StaticLib"
	language "C++"

    targetdir (tp_bin_out)
    objdir (tp_int_out)

	files
	{
		"src/**.h",
        "src/**.cpp",

		"include/**.h"
	}

    includedirs
    {
        "include",
		"src"
    }

	defines
	{
		"YAML_CPP_STATIC_DEFINE"
	}

	filter "action:vs*"
		buildoptions {"/w"}

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++20"
        staticruntime "off"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Debug_Editor"
		runtime "Debug"
		symbols "on"

	filter "configurations:Development"
		runtime "Release"
		optimize "on"
		symbols "on"

	filter "configurations:Development_Editor"
		runtime "Release"
		optimize "on"
		symbols "on"

	filter "configurations:Shipping"
		runtime "Release"
		optimize "on"
		symbols "off"
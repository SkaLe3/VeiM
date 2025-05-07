project "CityHash"
	kind "StaticLib"
	language "C"
	staticruntime "off"

	location ".\\"


    targetdir (tp_bin_out)
    objdir (tp_int_out)

	files
	{
		"include/CityHash/city.h",
		"city.cc"
	}

    includedirs
    {
        "include/CityHash"
    }

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
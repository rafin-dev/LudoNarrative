project "yaml-cpp"
	kind "StaticLib"
	language "C++"
	location "%{wks.location}/LudoNarrative/vendor/yaml-cpp"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	defines
	{
		"YAML_CPP_STATIC_DEFINE"
	}

	files
	{
		"%{prj.location}/include/**.hpp",
        "%{prj.location}/include/**.h",
        "%{prj.location}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.location}/include"
	}

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"

	filter "configurations:Debug"
        symbols "on"
        optimize "off"
        runtime "Debug"
        
    filter "configurations:Release"
        optimize "On"
        runtime "Release"

    filter "configurations:Dist"
        optimize "On"
        runtime "Release"
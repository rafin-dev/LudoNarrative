project "Box2D"
	kind "StaticLib"
	language "C"
	location "%{wks.location}/LudoNarrative/vendor/Box2D"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
        "%{prj.location}/include/**.h",
        "%{prj.location}/src/**.h",
		"%{prj.location}/src/**.c"
	}

	includedirs
	{
		"%{prj.location}/include"
	}

	filter "system:windows"
		systemversion "latest"
		cdialect "C17"

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
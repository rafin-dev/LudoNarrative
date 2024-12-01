project "ImGui"
	kind "StaticLib"
	language "C++"
	location "%{wks.location}/LudoNarrative/vendor/imgui"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.location}/*.hpp",
        "%{prj.location}/*.h",
        "%{prj.location}/*.cpp"
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
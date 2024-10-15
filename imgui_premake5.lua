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
		staticruntime "On"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

    filter "configurations:Dist"
        runtime "Release"
        optimize "on"